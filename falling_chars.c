#include <stdio.h>
#include <ncurses.h>

typedef struct {
    int x;
    int y;
    char c; // only for testing purposes
} Pos_tuple;

unsigned width; // = number of columns
unsigned height;

// Reads in the content of the file with the passed filename and prints it 
// to the screen.
int print_file_to_screen(const char *filename)
{
    FILE *textfile;
    textfile = fopen(filename, "r");
    if (textfile != NULL){
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while((read = getline(&line, &len, textfile)) != -1)
        {
            //printf("%s", line);
            printw("%s", line);
        }
    }
}

int main(int argc, char *argv[])
{
    /*
    initscr(); // Start ncurses mode
    printw("Lorem ipsum dolor sit amet");
    getch(); // Wait for user input
    endwin(); // End ncurses mode
    */
    initscr(); // Start ncurses mode
    
    width = getmaxx(stdscr);
    height = getmaxy(stdscr);
    def_prog_mode(); // Save the current terminal content
    endwin();
    printf("width = %d\n", width);
    printf("height = %d\n", height);
    getchar();
    reset_prog_mode(); // Restore the saved terminal content
    refresh();
    
    const char *filename = "test_file_2.txt";
    print_file_to_screen(filename);
    getch();
    Pos_tuple window_columns[width][height];
    
    //clear(); // Would lead to that nothing would be printed in the following
               // lines:
    int non_space_char_count = 0;
    int space_count = 0;
    for(int i = 0; i < width; i++){
        int j = 0;
        for(; j < height; j++){
            chtype mvinch_return = mvinch(j,i); // j,i
            //char mvinch_char = mvinch_return & A_CHARTEXT;
            char c = mvinch(j,i) & A_CHARTEXT;
            if(c != ' '){
                non_space_char_count++;
                Pos_tuple tuple = {i, j, c};
                window_columns[i][j] = tuple;
            } else {
                space_count++;
            }
        }
        if(j < height){
            Pos_tuple tuple = {-1, -1, '\0'};
            window_columns[i][j] = tuple;
        }
    }
    endwin();
    // Control output:
    // expected: 36
    printf("The window contains %d non-space chars.\n", non_space_char_count);
    printf("The window contains %d spaces.\n", space_count);
    
    /*
    // Control output:
    def_prog_mode();
    endwin();
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            if(window_columns[i][j].x == -1){
                break;
            }
            putchar(window_columns[i][j].c);
        }
    }
    */
    /*
    // Control output:
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            if (window_columns[i][j].x != -1){
                char c = mvinch(j,i) & A_CHARTEXT;
                def_prog_mode();
                endwin();
                putchar(c);
                reset_prog_mode();
                refresh();
            }
        }
        def_prog_mode();
        endwin();
        putchar('\n');
        reset_prog_mode();
        refresh();
    }
    */
}
