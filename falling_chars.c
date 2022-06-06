#include <stdio.h>
#include <ncurses.h>

unsigned width; // = number of columns
unsigned height;
char **window_columns;

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
    
    const char *filename = "test_file.txt";
    print_file_to_screen(filename);
    getch();
    
    //clear(); // Would lead to that nothing would be printed in the following
               // lines:
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            chtype mvinch_return = mvinch(i,j);
            char mvinch_char = mvinch_return & A_CHARTEXT;
            //window_columns[i][j] = mvinch_char;
            def_prog_mode();
            endwin();
            //putchar(window_columns[i][j]);
            putchar(mvinch_char);
            reset_prog_mode();
            refresh();
        }
        def_prog_mode();
        endwin();
        putchar('\n');
        reset_prog_mode();
        refresh();
    }
    endwin();
}
