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

#include <stdio.h> // for printf
#include <unistd.h> // for pause(), getlogin(), gethostname(), getcwd()
#include <linux/limits.h> // for ARG_MAX
#include <limits.h> // for PATH_MAX, HOST_NAME_MAX
#include <regex> // for std::regex, std::regex_replace
// for printw(), mvinch(), A_CHARTEXT(), mvaddch(), refresh(), initscr(),
// getmaxx(), getmaxy(), entwin():
#include <ncurses.h>
#include <stdlib.h> // for rand(), srand(), malloc()
#include <time.h> // for time()
#include <vector> // for std::vector

// Derived from the NAME_REGEX definition from /usr/share/adduser/adduser.conf:
#define NAME_REGEX = "^[a-z][-a-z0-9_]*"
#define SLEEP usleep(9000)
//#define SLEEP usleep(60000)
//#define SLEEP usleep(2000)

typedef struct
{
    unsigned x; // The char's x position on the ncurses window
    unsigned y; // The char's y position on the ncurses window
    char c;     // The char's value
    bool can_still_fall_down = true;
} Pos_tuple;

unsigned width;
unsigned height;
unsigned cannot_fall_down_count = 0;


// --------------------------------------------------------------------------
// ----------- reading console in- and output helper functions --------------
// --------------------------------------------------------------------------

// Reads the name of the logged in user, the host name and the current
// working directory and prints a corresponding prompt.
void print_current_prompt()
{
    char *username = getlogin();
    char *hostname = (char *) malloc(HOST_NAME_MAX);
    gethostname(hostname, HOST_NAME_MAX);
    char *absolute_cwd = (char *) malloc(PATH_MAX);
    getcwd(absolute_cwd, PATH_MAX);
    std::regex home_dir_regex("/home/[a-zA-Z]*");
    std::string cwd_cpp = std::regex_replace(absolute_cwd, home_dir_regex, "~");
    const char *relative_cwd = cwd_cpp.c_str();
    
    printf("%s@%s %s $ ", username, hostname, relative_cwd);
}


// Waits for the user to type some console input and returns those input
// as string (char array).
char *retrieve_user_input()
{
    char *input = (char *) malloc(ARG_MAX);
    fgets(input, ARG_MAX, stdin);
    return input;
}


// --------------------------------------------------------------------------
// ------------------- falling chars helper functions -----------------------
// --------------------------------------------------------------------------


// Reads in the content of the file with the passed filename and prints it 
// to the screen.
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
}


// Returns 1 if numbers contains number, 0 otherwise.
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


int main(int argc, char *argv[])
{
    print_current_prompt();
    printf("%s\n", retrieve_user_input());
    getchar(); // So that the abovely generated output is visible
               // before the ncurses mode is started.
    
    initscr(); // Start ncurses mode
    
    const char *filename = "test_file.txt";
    width = getmaxx(stdscr); // = number of columns
    height = getmaxy(stdscr);
    std::vector<Pos_tuple> char_positions;
    
    print_file_to_screen(filename);
    
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
