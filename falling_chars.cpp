#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

//#define SLEEP usleep(9000)
//#define SLEEP usleep(60000)
#define SLEEP usleep(3000)

typedef struct
{
    int x; // The char's x position on the ncurses window
    int y; // The char's y position on the ncurses window
    char c; // The char's value
} Pos_tuple;
unsigned width;
unsigned height;

// Reads in the content of the file with the passed filename and prints it 
// to the screen.
//int print_file_to_screen(const char *filename, FILE *test_output_file)
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
// "fall down" until the char reached the bottom of the window or a stack of
// one or more already fallen down characters at the bottom of the window.
void let_char_fall_down(Pos_tuple char_pos)
{
    for(unsigned i = 0; char_pos.y+i < height-1; i++)
    {
        char next_char = mvinch(char_pos.y+i+1,char_pos.x) & A_CHARTEXT;
        if (next_char != ' ')
            break;
        // Overwrite the previous position of the char:
        mvaddch(char_pos.y+i, char_pos.x, ' ');
        // ...and write it one y-position ahead:
        mvaddch(char_pos.y+i+1, char_pos.x, char_pos.c);
        refresh();
        SLEEP;
    }
}

int main(int argc, char *argv[])
{
    initscr(); // Start ncurses mode
    
    const char *filename = "test_file.txt";
    width = getmaxx(stdscr); // = number of columns
    height = getmaxy(stdscr);
    //Pos_tuple null_tuple = {-1, -1, '\0'};
    //std::vector<std::vector<Pos_tuple>> window_columns(width, std::vector<Pos_tuple>(height, null_tuple));
    std::vector<std::vector<Pos_tuple>> window_columns(width);
    
    std::vector<Pos_tuple> one = {{0,0,'a'}, {0,1,'b'}};
    std::vector<Pos_tuple> two = {{3,1,'f'}};
    window_columns[0] = one;
    window_columns[1] = two;
    
    print_file_to_screen(filename);
    
    unsigned i = 0, j = 0;
    for(; i < width; i++)
    {
        std::vector<Pos_tuple> column;
        for(j = 0; j < height; j++)
        {
            char c = mvinch(j,i) & A_CHARTEXT;
            if(c != ' ')
            {
                Pos_tuple tuple = {i, j, c};
                column.push_back(tuple);
            }
        }
        window_columns[i] = column;
    }
    
    /*
    endwin();
    printf("window_columns.size() = %d\n", window_columns.size());
    for(i = 0; i < window_columns.size(); i++)
    {
        printf("window_columns[%d].size() = %d\n", i, window_columns[i].size());
        for(j = 0; j < window_columns[i].size(); j++)
        {
            printf("window_columns[%d][%d].x = %d\n", i, j, window_columns[i][j].x);
            printf("window_columns[%d][%d].y = %d\n", i, j, window_columns[i][j].y);
            printf("window_columns[%d][%d].c = %c\n", i, j, window_columns[i][j].c);
        }
    }
    */
    
    for(i = 0; i < window_columns.size(); i++)
    {
        for(j = 0; j < window_columns[i].size(); j++)
        {
            let_char_fall_down(window_columns[i][j]);
        }
    }
    
    /*
    for(i = 0; i < window_columns.size(); i++)
    {
        unsigned n_rand_numbers[width];
        get_n_rand_numbers(width, n_rand_numbers);
        for(j = 0; j < window_columns[i].size(); j++)
        {
            let_char_fall_down(window_columns[i][n_rand_numbers[j]]);
        }
    }
    */
    
    /*
    for(i = height-1; i >= 0; i--)
    {
        unsigned n = width;
        unsigned n_rand_numbers[n];
        get_n_rand_numbers(n, n_rand_numbers);
        for(j = 0; j < width; j++)
            if(window_columns[i][j].x != null_tuple.x && window_columns[i][j].y != null_tuple.y && window_columns[i][j].c != null_tuple.c)
                let_char_fall_down(window_columns[n_rand_numbers[i]][j]);
    }
    */
    
    /*
    for(i = 1; i >= 0; i--)
    {
        unsigned n = 21;
        unsigned n_rand_numbers[n];
        get_n_rand_numbers(n, n_rand_numbers);
        for(j = 0; j < n; j++)
            let_char_fall_down(window_columns[n_rand_numbers[i]][j]); // [j][i]
    }
    */
    
    pause(); // Sleep forever, e.g. until Ctrl. + C is activated.
    endwin();
}
