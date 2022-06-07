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
//int print_file_to_screen(const char *filename, FILE *test_output_file)
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

int main(int argc, char *argv[])
{
    initscr(); // Start ncurses mode
    
    // Control output:
    const char *filename = "test_file_2.txt";
    const char *test_output_filename = "test_output.txt";
    FILE *test_output_file;
    test_output_file = fopen(test_output_filename, "w");
    unsigned linecount = 0;
    
    width = getmaxx(stdscr);
    height = getmaxy(stdscr);
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
    
    print_file_to_screen(filename);
    getch();
    Pos_tuple window_columns[width][height];
    
    int non_space_char_count = 0;
    int space_count = 0;
    for(int i = 0; i < width; i++){
        int j = 0;
        for(; j < height; j++){
            char c = mvinch(j,i) & A_CHARTEXT;
            if(c != ' '){
                def_prog_mode();
                endwin();
                printf("Detected non-space char %c\n", c);
                reset_prog_mode();
                refresh();
                non_space_char_count++;
                Pos_tuple tuple = {i, j, c};
                window_columns[i][j] = tuple;
            } else 
                space_count++;
        }
        if(j < height){
            Pos_tuple tuple = {-1, -1, '\0'};
            window_columns[i][j] = tuple;
        }
    }
    endwin();
    // Control output:
    printf("The window contains %d non-space chars.\n", non_space_char_count);
    printf("The window contains %d spaces.\n", space_count);
    
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 21; j++){
            char c = mvinch(i,j) & A_CHARTEXT;
            printf("mvinch(%d, %d) = %c\n", i ,j, c);
        }
    }
}
