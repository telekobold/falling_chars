/*
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Michael Merz <www.telekobold.de> wrote this file. As long as you retain this 
 * notice you can do whatever you want with this stuff. If we meet some day, and 
 * you think this stuff is worth it, you can buy me a beer in return. 
 * telekobold.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 */

// In C++, unlike C, some imports are not necessary:
//#include <stdio.h> // for printf()
//#include <stdlib.h> // for malloc()
#include <unistd.h> // for getlogin(), gethostname(), getcwd()
#include <linux/limits.h> // for ARG_MAX
#include <limits.h> // for PATH_MAX, HOST_NAME_MAX
#include <regex> // for std::regex, std::regex_replace
#include <iostream> // for std::cout
// for printw(), mvinch(), A_CHARTEXT(), mvaddch(), refresh(), initscr(),
// getmaxx(), getmaxy(), entwin():
#include <ncurses.h>
//#include <stdlib.h> // for rand(), srand(), malloc(), system()
//#include <time.h> // for time()
//#include <vector> // for std::vector

// Derived from the NAME_REGEX definition from /usr/share/adduser/adduser.conf:
#define HOME_DIR_REGEX "/home/[a-z][-a-z0-9_]*"
#define SLEEP usleep(9000)

typedef struct
{
    unsigned x; // The char's x position on the ncurses window
    unsigned y; // The char's y position on the ncurses window
    char c;     // The char's value
    bool can_still_fall_down = true;
} Pos_tuple;

unsigned line_count = 0; // Counts already printed lines
unsigned width;
unsigned height;
unsigned cannot_fall_down_count = 0;


// --------------------------------------------------------------------------
// ----------- reading console in- and output helper functions --------------
// --------------------------------------------------------------------------

// Reads the name of the logged in user, the host name and the current
// working directory and returns a corresponding unix console prompt.
std::string get_current_prompt()
{
    // getlogin() returns the string as (char *) and must be converted
    // to std::string:
    std::string username = std::string(getlogin());
    // gethostname() needs a (char *), so the host name must be converted to 
    // std::string after calling gethostname():
    char *hostname_c = (char *) malloc(HOST_NAME_MAX);
    gethostname(hostname_c, HOST_NAME_MAX);
    std::string hostname = std::string(hostname_c);
    free(hostname_c);
    char *absolute_cwd = (char *) malloc(PATH_MAX);
    getcwd(absolute_cwd, PATH_MAX);
    std::regex home_dir_regex(HOME_DIR_REGEX);
    std::string relative_cwd = std::regex_replace(absolute_cwd, home_dir_regex, "~");
    free(absolute_cwd);
    
    return username + "@" + hostname + " " + relative_cwd + " $ ";
}


// Calls `get_current_prompt()` and both prints its return value to the console
// and writes it to the passed file. Assumes that `file` is properly initialized.
void print_current_prompt(FILE *file)
{
    std::string current_prompt = get_current_prompt();
    std::cout << current_prompt;
    // Convert current prompt from std::string to (char *) since fprintf
    // needs (char *):
    const char *current_prompt_c = current_prompt.c_str();
    fprintf(file, "%s", current_prompt_c);
    line_count++;
}


// Reads a command from the console, writes it to the passed file, executes it 
// and prints the command's output to the console and to the passed file. 
// Assumes that `file` is properly initialized.
void read_and_execute_console_input(FILE *file)
{
    char *input = (char *) malloc(ARG_MAX);
    // Reads a command from the console:
    fgets(input, ARG_MAX, stdin);
    // ...writes it to `file`:
    fprintf(file, "%s", input);
    line_count++;
    // ...executes it:
    FILE *pipe = popen(input, "r");
    // ...and prints its output to the console and to `file`:
    if(pipe != NULL && file != NULL){
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while((read = getline(&line, &len, pipe)) != -1)
        {
            printf("%s", line);
            fprintf(file, "%s", line);
            line_count++;
        }
    }
    pclose(pipe);
    free(input);
}


// --------------------------------------------------------------------------
// ------------------- falling chars helper functions -----------------------
// --------------------------------------------------------------------------

// Reads in the content of the file with the passed filename and prints it 
// to the screen. Assumes that the program is currently in ncurses mode.
void print_file_to_screen(const char *filename)
{
    FILE *textfile;
    textfile = fopen(filename, "r");
    if(textfile != NULL){
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while((read = getline(&line, &len, textfile)) != -1)
            printw("%s", line);
    }
    refresh();
}


// Returns 1 if `numbers` contains `number`, 0 otherwise.
unsigned contains(const unsigned *numbers, const unsigned numbers_size, const unsigned number)
{
    for(unsigned i = 0; i < numbers_size; i++)
        if(numbers[i] == number)
            return 1;
    return 0;
}


// Returns a list of n numbers between 0 and n, randomly shuffled, but unique 
// (meaning that each number appears only once in the list)
// or `NULL` for n <= 0.
void get_n_rand_numbers(unsigned n, unsigned *rand_numbers)
{
    if(n <= 0)
        rand_numbers = NULL;
    srand((unsigned) time(0)); // Seed rand()'s random number generator
    unsigned lower = 0, upper = n;
    for(unsigned i = 0; i < n; i++)
    {
        unsigned rand_num = (rand() % (upper - lower + 1)) + lower; // % upper + 1
        // Avoid that the same number is added twice to the list:
        while(contains(rand_numbers, n, rand_num))
            rand_num = (rand() % (upper - lower + 1)) + lower; // % upper + 1
        rand_numbers[i] = rand_num;
    }
}


// Takes a char at an existing position on the standard window and lets it 
// "fall down" until the char reached the bottom of the window or the top of 
// a stack of one or more characters at the bottom of the window. 
// Also does some additional checks which help to ensure that after a few 
// iterations all chars are either at the bottom of the window or on a stack
// at the bottom of the window.
// Assumes that the program is currently in ncurses mode and that `char_pos`
// is properly initialized.
void let_char_fall_down(Pos_tuple *char_pos)
{
    if(not char_pos->can_still_fall_down)
        return;
    bool space_detected = false;
    // Check if there is any space char between the char at the current position
    // and the bottom of the window. If not, there will be no way for this char
    // to fall down:
    for(unsigned i = char_pos->y; i < height-1; i++)
    {
        char current_char = mvinch(i, char_pos->x) & A_CHARTEXT;
        if(current_char == ' '){
            space_detected = true;
            break;
        }
    }
    if(not space_detected)
    {
        char_pos->can_still_fall_down = false;
        cannot_fall_down_count++;
        return;
    }
    
    unsigned i = char_pos->y;
    for(; i < height-1; i++)
    {
        char next_char = mvinch(i+1, char_pos->x) & A_CHARTEXT;
        if(next_char != ' ')
            break;
        mvaddch(i, char_pos->x, ' ');
        mvaddch(i+1, char_pos->x, char_pos->c);
        refresh();
        SLEEP;
    }
    // Update the y position of the char in the char_positions array:
    char_pos->y = i;
}


int main()
{
    // Clear the console so that only new output is still visible:
    system("clear");
    
    // ------ Collect some console in- and output. While doing so, ------
    // ------------ simulate a "normal" console environment: ------------
    
    initscr(); // Start ncurses mode
    width = getmaxx(stdscr); // = number of columns
    height = getmaxy(stdscr);
    // Temporary leave ncurses mode:
    def_prog_mode(); // Save the current terminal content
    endwin();
    
    unsigned max_lines = height / 3;
    /*
    printf("height = %d\n", height);
    printf("width = %d\n", width);
    printf("max_lines = %d\n", max_lines);
    */
    
    std::string output_file_path_cpp = "test_output.txt";
    // Convert output file path from std::string to (char *) since fprintf
    // needs (char *):
    const char *output_file_path = output_file_path_cpp.c_str();
    FILE *output_file = fopen(output_file_path, "w");
    
    print_current_prompt(output_file);
    
    while(line_count < max_lines)
    {
        read_and_execute_console_input(output_file);
        print_current_prompt(output_file);
    }
    // So that this file can be opened again in read mode:
    fclose(output_file);
    
    // ------ Go back to ncurses mode, print the collected console ------
    // ------------- in- and output to the screen, freeze it ------------
    // ----------- and let each char fall down char after char: ---------
    
    reset_prog_mode(); // Restore the saved terminal content
    refresh();
    
    std::vector<Pos_tuple> char_positions;
    print_file_to_screen(output_file_path);
    
    //getch();
    //endwin();
    
    unsigned i = 0, j = 0;
    // Collect all chars on the current window that are not ' ':
    for(; i < width; i++)
    {
        for(j = 0; j < height; j++)
        {
            char c = mvinch(j,i) & A_CHARTEXT;
            if(c != ' ')
            {
                Pos_tuple tuple = {i, j, c};
                //column.push_back(tuple);
                char_positions.push_back(tuple);
            }
        }
    }
    
    unsigned n_rand_numbers[char_positions.size()];
    get_n_rand_numbers(char_positions.size(), n_rand_numbers);
    // Let the collected chars fall down until all chars reached either the
    // bottom of the window or the top of a stack at the bottom of the window:
    while(cannot_fall_down_count < char_positions.size())
    {
        for(i = 0; i < char_positions.size(); i++)
            let_char_fall_down(&char_positions[n_rand_numbers[i]]);
        /*
        // Debug output:
        getch();
        refresh();
        def_prog_mode();
        endwin();
        // NOTE: The current values are (strangely) always from the last loop pass:
        printf("cannot_fall_down_count = %d\n", cannot_fall_down_count);
        printf("char_positions.size() = %ld\n", char_positions.size());
        for(j = 0; j < char_positions.size(); j++){
            printf("char_positions[%d].can_still_fall_down = %s\n", j, char_positions[j].can_still_fall_down ? "true" : "false");
            printf("char_positions[%d].x = %d\n", j, char_positions[j].x);
            printf("char_positions[%d].y = %d\n\n", j, char_positions[j].y);
        }
        for(j = 0; j < 3; j++)
            printf("--------------------\n");
        getchar();
        reset_prog_mode();
        refresh();
        */
    }
    
    /*
    // Debug output:
    def_prog_mode();
    endwin();
    printf("Going now into pause() mode");
    getchar();
    reset_prog_mode();
    refresh();
    */
    pause(); // Sleep forever, e.g. until Ctrl. + C is activated.
    // Will be never executed since the program is terminated before
    // using Ctrl. + C:
    endwin();
    printf("Terminated program.");
}
