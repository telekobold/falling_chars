#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#define SLEEP usleep(9000)
//#define SLEEP usleep(60000)
//#define SLEEP usleep(3000)

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

// Reads in the content of the file with the passed filename and prints it 
// to the screen.
void print_file_to_screen(const char *filename)
{
    FILE *textfile;
    textfile = fopen(filename, "r");
    if (textfile != NULL){
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
// a stack of one or more characters. Also does some additional checks which
// help to ensure that after a few iterations there is no space character 
// between each char and the top.
void let_char_fall_down(Pos_tuple *char_pos)
{
    if (not char_pos->can_still_fall_down)
        return;
    bool char_detected = false;
    // Check if there is any space char between the char at the current position
    // and the bottom of the window. If not, there will be no way for this char
    // to fall down:
    for(unsigned i = char_pos->y; i < height-1; i++)
    {
        char current_char = mvinch(i, char_pos->x) & A_CHARTEXT;
        if (current_char == ' '){
            char_detected = true;
            break;
        }
    }
    if(not char_detected)
    {
        char_pos->can_still_fall_down = true;
        cannot_fall_down_count++;
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
    initscr(); // Start ncurses mode
    
    const char *filename = "test_file.txt";
    width = getmaxx(stdscr); // = number of columns
    height = getmaxy(stdscr);
    //std::vector<std::vector<Pos_tuple>> window_columns(width);
    std::vector<Pos_tuple> char_positions;
    
    print_file_to_screen(filename);
    
    unsigned i = 0, j = 0;
    for(; i < width; i++)
    {
        //std::vector<Pos_tuple> column;
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
        //window_columns[i] = column;
    }
    
    unsigned n_rand_numbers[char_positions.size()];
    get_n_rand_numbers(char_positions.size(), n_rand_numbers);
    while(cannot_fall_down_count < char_positions.size())
    {
        /*
        getch();
        refresh();
        def_prog_mode();
        endwin();
        printf("cannot_fall_down_count = %d\n", cannot_fall_down_count);
        printf("char_positions.size() = %ld\n", char_positions.size());
        getchar();
        reset_prog_mode();
        refresh();
        */
        for(i = 0; i < char_positions.size(); i++)
            let_char_fall_down(&char_positions[n_rand_numbers[i]]);
    }
    
    pause(); // Sleep forever, e.g. until Ctrl. + C is activated.
    // Will be never executed since the program is terminated before
    // using Ctrl. + C:
    endwin();
    printf("Terminated program.");
}
