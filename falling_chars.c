#include <stdio.h>
#include <ncurses.h>

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
    
    const char *filename = "test_file.txt";
    
    initscr(); // Start ncurses mode
    
    print_file_to_screen(filename);
    
    getch(); // Wait for user input so that the input is shown on the screen
    endwin(); // End ncurses mode
}
