#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //ignore error, linux library
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> // For open()

// Know this isent the best practice, but a lot of functions use path and need it updated
char path[1024] = "/bin";  //path variable

//use Doxygen for function comments

/**
 * @brief Displays the help screen with information about the program's usage.
 * 
 * This function prints a simple help message to the console.
 */
void PrintHelp(){
	//TODO: Finish this out properly once everything is written up
	printf("Im the help screen\n\n");
}


/**
 * @brief Repeats what the user inputs when running the command
 * 
 * This function prints the users input to console.
 */
void Echo(char* input) {
	char* message = input + 4; // ignore echo
	while (*message == ' ') message++; // Ignore whitespace
		
	// Print the remaining message
    printf("%s\n", message);
}

/**
 * @brief Handles the CD command
 * 
 * This changes what directory the program is in.
 */
void ChangeDir(char* input) {
	char *path = input + 2;
	while (*path == ' ') path++; // Skip leading spaces

	// Handle no input
	if (strlen(path) == 0) {
		const char *homeDir = getenv("HOME");
		if (homeDir) {
			// Set the path to home, handle error if present (does not work on windows)
			if (chdir(homeDir) != 0) {
				perror("cd");
			}
		} else {
			printf("HOME environment variable not set.\n");
		}
	}

	// Handle ".."
	else if (strcmp(path, "..") == 0) {
		// Set the path to parent folder, handle error if present
		if (chdir("..") != 0) {
			perror("cd");
		}
	}

	// Handle other paths
	else {
		// Set the path to child path string, handle error if present
		if (chdir(path) != 0) {
			perror("cd");
		}
	}
}

/**
 * @brief Sets the PATH variable to a new value.
 * 
 * This function updates the local PATH variable with the provided input.
 * It expects the input to start with "setpath ".
 */
void SetPath(char* input) {
	char *pathArgs = input + 7;
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
}

/**
 * @brief Prints the current PATH directories.
 * 
 * This function splits the PATH variable by spaces and prints each directory.
 */
void PrintPath() {
    printf("Current PATH directories:\n");
    char *pathCopy = strdup(path); // Create a copy of the path to avoid modifying the original
    char *pathDir = strtok(pathCopy, " ");
    while (pathDir != NULL) {
        printf("- %s\n", pathDir);
        pathDir = strtok(NULL, " ");
    }
    free(pathCopy); // Free the allocated memory
}


/**
 * @brief Runs a program specified by the user.
 * 
 * This function executes a program located in the current directory or in the PATH directories.
 * It forks a child process to run the program and waits for it to finish.
 */
void RunProgram(char* input){
    // Extract the path to the application
    char *appPath = input;
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
            if (waitpid(pid, &status, 0) == -1)
            {
                perror("Error waiting for child process");
            }
        }
    } else{

        // File does not exist or is not executable
        perror("Error: File does not exist or is not executable");
    }
}

/**
 * @brief Handles searching the PATH directories and executing a program.
 * 
 * This function searches the directories in the PATH variable for the specified program
 * and executes it if found.
 */
void HandlePath(char* input) {
    // Extract the command name (first word of the input)
    char *appName = strtok(input, " "); // Extract the application name
    char *args = strtok(NULL, ""); // Extract the rest of the string as arguments

    // Manually iterate through the path directories
    char fullPath[1026];
    int start = 0, end = 0;
    int pathLen = strlen(path);
    int found = 0;

    for (int i = 0; i <= pathLen; i++) {
        // Check for ' ' delimiter or end of string
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
        printf("Not a valid command: %s\n", input);
    }
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
  	int redirectOutput = 0; // Flag to indicate if output redirection is active
    int savedStdout = -1;   // To store the original stdout file descriptor
	
	// Main cmd loop
	while (1) {
		
		//variables inits
		char currWorkingDir[1024];

		//Get directory here so cd and running stuff is easer
		getcwd(currWorkingDir, sizeof(currWorkingDir));

		//TODO: remove eventually
		printf("%s>", currWorkingDir); // Print here so i know where im at 



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

         // Check for redirection (">")
        char *redirect = strchr(trimmedInput, '>');
        if (redirect != NULL) {
            *redirect = '\0'; // Split the input at '>'
            redirect++;       // Move to the filename part
            while (*redirect == ' ') redirect++; // Skip leading spaces in the filename

            if (strlen(redirect) == 0) {
                printf("Error: No filename provided for redirection.\n");
                continue;
            }

            // Open the file for writing
            int outputFile = open(redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (outputFile == -1) {
                perror("Error opening file for redirection");
                continue;
            }

            // Save the current stdout file descriptor
            savedStdout = dup(STDOUT_FILENO);
            if (savedStdout == -1) {
                perror("Error saving stdout");
                close(outputFile);
                continue;
            }

            // Redirect stdout to the file
            if (dup2(outputFile, STDOUT_FILENO) == -1) {
                perror("Error redirecting stdout");
                close(outputFile);
                close(savedStdout);
                continue;
            }

            close(outputFile); // Close the file descriptor, as it's now duplicated
            redirectOutput = 1; // Set the flag to indicate redirection is active
        }

		// Trim trailing spaces from trimmedInput after removing redirection
        char *endTrimmed = trimmedInput + strlen(trimmedInput) - 1;
        while (endTrimmed > trimmedInput && *endTrimmed == ' ') *endTrimmed-- = '\0';


        // Handle inputs
        if (strcmp(trimmedInput, "exit") == 0) {
            if (redirectOutput) {
                dup2(savedStdout, STDOUT_FILENO); // Restore stdout
                close(savedStdout);
            }
            return 0;

        } else if (strncmp(trimmedInput, "echo", 4) == 0) {
            Echo(trimmedInput);

        } else if (strcmp(trimmedInput, "pwd") == 0) {
            printf("%s\n", currWorkingDir); // Print the current working directory

        } else if (strncmp(trimmedInput, "cd", 2) == 0) {
            ChangeDir(trimmedInput);

        } else if (strncmp(trimmedInput, "setpath", 7) == 0) {
            SetPath(trimmedInput);

        } else if (strcmp(trimmedInput, "$path") == 0) {
            PrintPath();

        } else if (strcmp(trimmedInput, "help") == 0) {
            PrintHelp();

		} else if (strcmp(trimmedInput, "date") == 0) {

        } else if (strncmp(trimmedInput, "./", 2) == 0) {
            RunProgram(trimmedInput);

        } else {
            HandlePath(trimmedInput);
        }


        // Restore stdout if it was redirected
        if (redirectOutput) {
            dup2(savedStdout, STDOUT_FILENO); // Restore stdout
            close(savedStdout); // Close the saved file descriptor
            redirectOutput = 0; // Reset the redirection flag
            savedStdout = -1; // Reset savedStdout
        }
	}
	
   
    
    return 0;
}