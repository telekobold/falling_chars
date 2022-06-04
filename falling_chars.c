#include <stdio.h>
#include <ncurses.h>

void get_chars();

int main(int argc, char *argv[])
{
    /*
    initscr(); // Start ncurses mode
    printw("Lorem ipsum dolor sit amet");
    getch(); // Wait for user input
    endwin(); // End ncurses mode
    */
    
    const char *filename = "test_file.txt";
    FILE *textfile;
    textfile = fopen(filename, "r");
    if (textfile != NULL){
        initscr(); // Start ncurses mode
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while((read = getline(&line, &len, textfile)) != -1)
        {
            //printf("%s", line);
            printw("%s", line);
        }
        getch(); // Wait for user input so that the input is shown on the screen
        endwin(); // End ncurses mode
    }
}
