#include <stdio.h>
#include <ncurses.h>

int width;
int height;

void get_chars();

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
    
    const char *filename = "test_file.txt";
    width = getmaxx(stdscr);
    height = getmaxy(stdscr);
    def_prog_mode(); // Save the current terminal content
    endwin();
    printf("width = %d\n", width);
    printf("height = %d\n", height);
    getchar();
    reset_prog_mode(); // Restore the saved terminal content
    refresh();
    print_file_to_screen(filename);
    
    getch(); // Wait for user input so that the input is shown on the screen
    endwin(); // End ncurses mode
}
