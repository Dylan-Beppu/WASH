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
	char input[1024]; //input variable
	char path[1024] = "/bin";  //path variable

	// Main cmd loop
	while (1) {
		//variables inits
		char currWorkingDir[1024];

		//Get directory here so cd and running stuff is easer
		getcwd(currWorkingDir, sizeof(currWorkingDir));

		//TODO: remove eventualty
		printf("%s>", currWorkingDir); // Prit here so i know where im at 



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
            char *pathArgs = trimmedInput + 7;
            while (*pathArgs == ' ') pathArgs++; // Skip leading spaces

            // Check if at least one argument is provided
            if (strlen(pathArgs) == 0) {
                printf("Error: setpath requires at least one argument.\n");
            } else {
                // Update the local PATH variable
                if (snprintf(path, sizeof(path), "%s", pathArgs) >= sizeof(path)) {
                    printf("Error: PATH is too long.\n");
                } else {
                    printf("Path updated to: %s\n", path);
                }
            }
		
		} else if(strcmp(trimmedInput, "$path") == 0) {
			printf("Current PATH directories:\n");
            char *pathCopy = strdup(path); // Create a copy of the path to avoid modifying the original
            char *pathDir = strtok(pathCopy, " ");
            while (pathDir != NULL) {
                printf("- %s\n", pathDir);
                pathDir = strtok(NULL, " ");
            }
            free(pathCopy); // Free the allocated memory


		
		} else if(strcmp(trimmedInput, "help") == 0) {
			PrintHelp();

		} else if(strncmp(trimmedInput, "./", 2) == 0) {
			//TODO: check for redirection


			// Extract the path to the application
            char *appPath = trimmedInput;
            while (*appPath == ' ') appPath++; // Skip leading spaces

            // Check if the file exists and is executable
            if (access(appPath, X_OK) == 0) {

                // Fork a child process
                pid_t pid = fork();
                if (pid == 0) {
                    // Child process: execute the application
                    char *args[] = {appPath, NULL}; // Arguments for the application
                    if (execvp(appPath, args) == -1) {
                        perror("Error running application");
                    }
                    exit(EXIT_FAILURE); // Exit child process if execvp fails
                } else if (pid < 0) {
                    // Fork failed
                    perror("Fork failed");
                } else {
                    // Parent process: wait for the child process to finish
                    int status;
                    if (waitpid(pid, &status, 0) == -1) {
                        perror("Error waiting for child process");
                    }
                }
            } else {


				


                // File does not exist or is not executable
                perror("Error: File does not exist or is not executable");
            }

		} else {
			// Extract the command name (first word of the input)
            char *appName = strtok(trimmedInput, " "); // Extract the application name
            char *args = strtok(NULL, ""); // Extract the rest of the string as arguments

            // Manually iterate through the path directories
            char fullPath[1024];
            int start = 0, end = 0;
            int pathLen = strlen(path);
            int found = 0;

            for (int i = 0; i <= pathLen; i++) {
                // Check for ':' delimiter or end of string
                if (path[i] == ' ' || path[i] == '\0') {
                    end = i;

                    // Extract the directory from the path
                    char dir[1024];
                    strncpy(dir, path + start, end - start);
                    dir[end - start] = '\0'; // Null-terminate the directory string

                    // Construct the full path to the executable
                    snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, appName);

                    // Check if the file exists and is executable
                    if (access(fullPath, X_OK) == 0) {
                        // Fork a child process to execute the program
                        pid_t pid = fork();
                        if (pid == 0) {
                            // Child process: execute the program
                            char *execArgs[1024];
                            int argIndex = 0;

                            // Add the application name as the first argument
                            execArgs[argIndex++] = fullPath;

                            // Split the arguments and add them to the execArgs array
                            if (args != NULL) {
                                char *arg = strtok(args, " ");
                                while (arg != NULL) {
                                    execArgs[argIndex++] = arg;
                                    arg = strtok(NULL, " ");
                                }
                            }

                            // Null-terminate the arguments array
                            execArgs[argIndex] = NULL;

                            // Execute the program
                            if (execvp(fullPath, execArgs) == -1) {
                                perror("Error running program");
                            }
                            exit(EXIT_FAILURE); // Exit child process if execvp fails
                        } else if (pid < 0) {
                            // Fork failed
                            perror("Fork failed");
                        } else {
                            // Parent process: wait for the child process to finish
                            int status;
                            if (waitpid(pid, &status, 0) == -1) {
                                perror("Error waiting for child process");
                            }
                        }
                        found = 1;
                        break; // Exit the loop once the program is executed
                    }

                    // Move to the next directory in the path
                    start = i + 1;
                }
            }

            // If no executable was found, print an error message
            if (!found) {
                printf("Not a valid command: %s\n", trimmedInput);
            }

		}
		/*So wants for other stuff:
			Clear
			Date
			Ls
		*/



		
	}
	
   
    
    return 0;
}