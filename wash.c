#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //ignore error, linux library




//use Doxygen for function comments

/**
 * @brief Displays the help screen with information about the program's usage.
 * 
 * This function prints a simple help message to the console.
 */
void PrintHelp(){
	printf("Im the help screen\n\n");
}








int main(int argc, char *argv[]) {
    
	// Check for args
    if (argc > 1) {
		if (strcmp(argv[1], "-h") == 0){
			PrintHelp();
			return 0;

		} else {
			printf("Unrecognized flag\n");
		}
    }



	// Variable initialization
	char input[1024];


	// Main cmd loop
	while (1) {
		
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

		//TODO: Remove eventually
		// printf("Raw  input: |%s|\n", input);
		printf("\ttrim input: |%s|\n", trimmedInput);



		//variables inits
		char currWorkingDir[1024];

		//Get directory here so cd and running stuff is easer
		getcwd(currWorkingDir, sizeof(currWorkingDir));


		//handle inputs
		if (strcmp(trimmedInput, "exit") == 0) {
			return 0;

		} else if(strncmp(trimmedInput, "echo", 4) == 0) {
		    char *message = trimmedInput + 4; // ignore echo
			while (*message == ' ') message++;
			//TODO: check for redirection here
			
		    // Print the remaining message
    		printf("%s\n", message);
			
		} else if(strcmp(trimmedInput, "pwd") == 0) {
			printf("%s\n", currWorkingDir); // Print the current working directory

		} else if(strncmp(trimmedInput, "cd", 2) == 0) {
			char *path = trimmedInput + 2;
            while (*path == ' ') path++; // Skip leading spaces

            // Handle no input 
            if (strlen(path) == 0) {
                const char *homeDir = getenv("HOME");
                if (homeDir) {
					//Set the path to home, handle error if present (does not work on windows)
                    if (chdir(homeDir) != 0) {
                        perror("cd");
                    }
                } else {
                    printf("HOME environment variable not set.\n");
                }
            } 

            // Handle ".." 
            else if (strcmp(path, "..") == 0) {
				//Set the path to parent folder, handle error if present
                if (chdir("..") != 0) {
					perror("cd");                    
                }
            } 
	
            // Handle other paths
            else {
				//Set the path to child path string, handle error if present
                if (chdir(path) != 0) {
                    perror("cd");
                }
            }
			


		} else if(strncmp(trimmedInput, "setpath", 7) == 0) {
			


		
		} else if(strcmp(trimmedInput, "help") == 0) {
			PrintHelp();

		} else if(strncmp(trimmedInput, "./", 2) == 0) {
			//TODO: fork and run the file if exists
			//TODO: check for redirection


		} else {
			printf("Not a valid command: %s\n", trimmedInput);

		}
		/*So wants for other stuff:
			Clear
			Date
		*/



		
	}
	
   
    
    return 0;
}