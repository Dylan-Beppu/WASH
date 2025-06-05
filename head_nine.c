#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // For Unix systems

/**
 * @brief Displays the first N lines of a file or stdin.
 * 
 * Usage: ./head_nine [-h] [-n N] [file.txt]
 * - If [file.txt] is specified, print the first N lines of that file (default is 9 lines).
 * - If no file is specified, read from stdin until CTRL+D and print the first N lines.
 * - The [-n N] flag specifies the number of lines to print.
 * - The [-h] flag prints a usage summary.
 */
void print_usage() {
    printf("Usage: ./head_nine [-h] [-n N] [file.txt]\n");
    printf("Options:\n");
    printf("  -h       Print this help message.\n");
    printf("  -n N     Print the first N lines (default is 9).\n");
    printf("  file.txt Specify the file to read from. If omitted, read from stdin.\n");
}

void HeadNine(int argc, char *argv[]) {
    int numLines = 9; // Default number of lines
    char *filename = NULL;
    FILE *file = NULL;

    // Output the current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("Error getting current working directory");
        return;        
    }

	if(argc == 0){
		perror("Expected 1 or more arguments");
        return;
	}

	
    // Parse command-line arguments
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_usage();
            return;
        } else if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                numLines = atoi(argv[++i]);
                if (numLines <= 0) {
                    fprintf(stderr, "Error: Invalid number of lines specified.\n");
                    return;
                }
            } else {
                fprintf(stderr, "Error: Missing argument for -n.\n");
                return;
            }
        } else {
            filename = argv[i];
        }
    }


	while (*filename == ' ') filename++; // Get rid of white space before file name


    // Open file or use stdin
    if (filename) {
        // Combine current working directory with filename
        char fullPath[1026];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, filename);

        file = fopen(fullPath, "r");
        if (file == NULL) {
            fprintf(stderr, "Error: Unable to open file '%s'.\n", fullPath);
            perror("Error opening file");
            return;
        }
    } else {
        file = stdin;
        printf("Reading from stdin. Press CTRL+D to finish input.\n");
    }

    // Read and print the first N lines
    char line[1024];
    int lineCount = 0;
    while (fgets(line, sizeof(line), file) != NULL && lineCount < numLines) {
        printf("%s", line);
        lineCount++;
    }

    // Close file if not stdin
    if (file != stdin) {
        fclose(file);
    }
}