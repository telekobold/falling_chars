#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define SLEEP usleep(9000)

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
        {
            printw("%s", line);
            // Control output:
            def_prog_mode();
            endwin();
            printf("%s", line);
            reset_prog_mode();
            refresh();
        }
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
        mvaddch(char_pos.y+i, char_pos.x, ' ');
        mvaddch(char_pos.y+i+1, char_pos.x, char_pos.c);
        refresh();
        SLEEP;
    }
}

int main(int argc, char *argv[])
{
    initscr(); // Start ncurses mode
    
    // Control output:
    const char *filename = "test_file_2.txt";
    const char *test_output_filename = "test_output.txt";
    //FILE *test_output_file;
    //test_output_file = fopen(test_output_filename, "w");
    
    width = getmaxx(stdscr); // = number of columns
    height = getmaxy(stdscr);
    /*
    def_prog_mode(); // Save the current terminal content
    endwin();
    printf("width = %d\n", width);
    printf("height = %d\n", height);
    if (test_output_file != NULL){
        fprintf(test_output_file, "width = %d\n", width);
        fprintf(test_output_file, "height = %d\n", height);
    }
    getchar();
    reset_prog_mode(); // Restore the saved terminal content
    refresh();
    */
    
    print_file_to_screen(filename);
    //getch(); // TODO: To be removed in the production version
    Pos_tuple window_columns[width][height];
    
    unsigned non_space_char_count = 0;
    unsigned space_count = 0;
    unsigned i = 0, j = 0;
    for(; i < width; i++)
    {
        for(j = 0; j < height; j++)
        {
            char c = mvinch(j,i) & A_CHARTEXT;
            if(c != ' ')
            {
                /*
                def_prog_mode();
                endwin();
                printf("Detected non-space char %c\n", c);
                reset_prog_mode();
                refresh();
                */
                non_space_char_count++;
                Pos_tuple tuple = {i, j, c};
                /*
                def_prog_mode();
                endwin();
                printf("Adding new Pos_tuple {%d, %d, %c}\n", i, j, c);
                reset_prog_mode();
                refresh();
                */
                window_columns[i][j] = tuple;
            } else 
                space_count++;
        }
        if(j < height)
        {
            Pos_tuple tuple = {-1, -1, '\0'};
            window_columns[i][j] = tuple;
        }
    }
    //endwin();
    /*
    // Control output:
    printf("i = %d\n", i);
    printf("j = %d\n", j);
    printf("The window contains %d non-space chars.\n", non_space_char_count);
    printf("The window contains %d spaces.\n", space_count);
    */
    
    //if (test_output_file != NULL){
        //for(int i = 0; width; i++){
        //for(i = 0; i < 21; i++){
        for(i = 1; i >= 0; i--)
        {
            //for(int j = 0; window_columns[i][j].x != -1 && window_columns[i][j].y != -1 && window_columns[i][j].c != '\0'; j++){
            //for(j = 1; j >=0; j--){
            unsigned n = 21;
            unsigned n_rand_numbers[n];
            get_n_rand_numbers(n, n_rand_numbers);
            for(j = 0; j < n; j++)
            {
                let_char_fall_down(window_columns[n_rand_numbers[j]][i]);
                //printf("Char at position (%d, %d) = %c\n", window_columns[i][j].x, window_columns[i][j].y, window_columns[i][j].c);
                //fprintf(test_output_file, "Char at position (%d, %d): %c\n", window_columns[i][j].x, window_columns[i][j].y, window_columns[i][j].c);
                //linecount++;
            }
        }
    //}
    /*
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 22; j++){
            char c = mvinch(i,j) & A_CHARTEXT;
            printf("mvinch(%d, %d) = %c\n", i ,j, c);
        }
    }
    */
    pause(); // Sleep forever, e.g. until Ctrl. + C is activated.
    //endwin();
}
