#include <stdio.h>
#include <stdlib.h>
#include <string.h>



void PrintHelp(){
	printf("Im the help screen\n\n");
}



int main(int argc, char *argv[]) {
    
	// Check for args
    if (argc > 1) {
		if (strcmp(argv[1], "-h") == 0){
			PrintHelp();
			return 0;
		} 
		else {
			printf("Unrecognized flag\n");
		}
    }



	// Variable initialization
	char input[1024];


	// Main cmd loop
	while (1) {
		
		// printf("Current directory: %s\n", dirPath);
        // printf("> ");


		// Handle input
        if (!fgets(input, sizeof(input), stdin)) {
            break; // Exit on EOF or input error
        }

        // Remove trailing newline character and handle leading/trailing spaces
        input[strcspn(input, "\n")] = '\0';
        char *trimmedInput = input;
        while (*trimmedInput == ' ') trimmedInput++; // Skip leading spaces
        char *end = trimmedInput + strlen(trimmedInput) - 1;
        while (end > trimmedInput && *end == ' ') *end-- = '\0'; // Remove trailing spaces

		printf(input);

		
	}
	
   
    
    return 0;
}