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
 * @brief Displays the help screen with information about the program's usage.s
 * 
 * This function prints a simple help message to the console.
 */
void PrintHelp(){
	/*
       This call to printf writes the extracted message to the standard output (console).
       The argument is the message string, which is printed as-is. printf returns the 
       number of characters written, or a negative value if an error occurs. No explicit 
       error handling is implemented here, as printf errors are rare in this context.
	   Im not rewriting this for all printf so this is it for the printf IO systemcall
    */
    printf("WASH commands:");
	printf("  help          - Displays this help screen with information about the program's usage.\n");
    printf("  exit          - Exits the shell.\n\n");
    printf("  pwd           - Prints the current working directory.\n");
    printf("  cd <path>     - Changes the current directory to the specified path.\n");
    printf("                  Use '..' to move to the parent directory.\n");
    printf("                  If no path is provided, it changes to the home directory.\n");
    printf("  head_nine <file> - Displays the first 9 lines of the specified file.\n\n");
    printf("  setpath <path> - Sets the PATH variable to the specified value.\n");
    printf("  $path          - Prints the current PATH variable (for debugging purposes).\n");
    printf("  echo <message> - Prints the specified message to the console.\n\n");
    printf("  ./<program>    - Executes a program located in the current directory.\n");
    printf("  <program>      - Searches for the program in the directories specified in the PATH variable and executes it.\n\n");
    printf(" <command> > <filename>   - Redirects the output of a command to a file.\n");
    printf("                  Creates two files: <filename>.output for normal output and <filename>.error for errors.\n");
    printf("                  If no filename is provided or multiple arguments are given, an error is displayed.\n\n");
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
	char *dir = input + 2;
	while (*dir == ' ') dir++; // Skip leading spaces

	// Handle no input
	if (strlen(dir) == 0) {
		/*
           This call to getenv retrieves the value of the "HOME" environment variable.
           The argument is the name of the environment variable ("HOME"). It returns 
           a pointer to the value string, or NULL if the variable is not set. If NULL 
           is returned, an error message is printed to inform the user.
        */
		const char *homeDir = getenv("HOME");
		if (homeDir) {
			/*
               This call to chdir changes the current working directory to the path 
               specified by `homeDir`. It returns 0 on success or -1 on failure. If -1 
               is returned, an error message is printed using perror, and the directory 
               remains unchanged. (does not work on windows)
            */ 
			if (chdir(homeDir) != 0) {
				perror("cd");
			}
		} else {
			printf("HOME environment variable not set.\n");
		}
	}

	// Handle ".."
	else if (strcmp(dir, "..") == 0) {
		/*
           This call to chdir changes the current working directory to the parent 
           directory (".."). It returns 0 on success or -1 on failure. If -1 is returned, 
           an error message is printed using perror, and the directory remains unchanged.
        */
		if (chdir("..") != 0) {
			perror("cd");
		}
	}

	// Handle other paths
	else {
		/*
           This call to chdir changes the current working directory to the path 
           specified by `path`. It returns 0 on success or -1 on failure. If -1 is 
           returned, an error message is printed using perror, and the directory 
           remains unchanged.
        */
		if (chdir(dir) != 0) {
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
        // Update the global PATH variable
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

	/*
       This call to access checks whether the file specified by `appPath` 
       exists and is executable. The first argument is the file path, and 
       the second argument is the mode to check (X_OK means executable). 
       It returns 0 if the file is accessible, or -1 if not. If -1 is 
       returned, an error message is printed using perror.
    */
    if (access(appPath, X_OK) == 0) {

        /*
           This call to fork creates a new process. It returns the process ID 
           of the child process to the parent, 0 to the child process, or -1 
           if the operation failed. If -1 is returned, an error message is 
           printed using perror, and the parent process continues without 
           executing the child process.
        */
        pid_t pid = fork();
        if (pid == 0) {
            // Child process: execute the application
            char *args[] = {appPath, NULL}; // Arguments for the application

			/*
               This call to execvp replaces the current process image with a new 
               process image specified by `appPath`. The first argument is the 
               program name, and the second argument is an array of arguments 
               passed to the program. If the operation fails, it returns -1, and 
               an error message is printed using perror. The child process exits 
               immediately after the failure.
            */
            if (execvp(appPath, args) == -1) {
                perror("Error running application");
            }

			/*
               This call to exit terminates the child process. The argument 
               `EXIT_FAILURE` indicates that the process exited due to an error.
            */
            exit(EXIT_FAILURE); // Exit child process if execvp fails

        } else if (pid < 0) {
            // Fork failed
            perror("Fork failed");
        } else {
            // Parent process: wait for the child process to finish
            int status;

			/*
               This call to waitpid waits for the child process specified by `pid` 
               to terminate. The first argument is the process ID of the child, 
               the second argument is a pointer to store the exit status, and the 
               third argument is 0 (no special options). If the operation fails, 
               it returns -1, and an error message is printed using perror.
            */
            if (waitpid(pid, &status, 0) == -1) {
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

            /*
               This call to access checks whether the file specified by `fullPath` 
               exists and is executable. The first argument is the file path, and 
               the second argument is the mode to check (X_OK means executable). 
               It returns 0 if the file is accessible, or -1 if not. If -1 is 
               returned, the loop continues to the next directory.
            */
            if (access(fullPath, X_OK) == 0) {
				/*
                   This call to fork creates a new process. It returns the process ID 
                   of the child process to the parent, 0 to the child process, or -1 
                   if the operation failed. If -1 is returned, an error message is 
                   printed using perror, and the parent process continues without 
                   executing the child process.
                */
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

                    /*
                       This call to execvp replaces the current process image with a new 
                       process image specified by `fullPath`. The first argument is the 
                       program name, and the second argument is an array of arguments 
                       passed to the program. If the operation fails, it returns -1, and 
                       an error message is printed using perror. The child process exits 
                       immediately after the failure.
                    */
                    if (execvp(fullPath, execArgs) == -1) {
                        perror("Error running program");
                    }

					/*
                       This call to exit terminates the child process. The argument 
                       `EXIT_FAILURE` indicates that the process exited due to an error.
                    */
                    exit(EXIT_FAILURE); // Exit child process if execvp fails

                } else if (pid < 0) {
                    // Fork failed
                    perror("Fork failed");
                } else {
                    // Parent process: wait for the child process to finish
                    int status;

					/*
                       This call to waitpid waits for the child process specified by `pid` 
                       to terminate. The first argument is the process ID of the child, 
                       the second argument is a pointer to store the exit status, and the 
                       third argument is 0 (no special options). If the operation fails, 
                       it returns -1, and an error message is printed using perror.
                    */
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

/**
 * @brief Displays the first 9 lines of a file.
 * 
 * This function reads a file line by line and prints the first 9 lines to the console.
 * If the file cannot be opened, it prints an error message.
 */
void HeadNine(char* input) {
    // Extract the filename from the input
    char *filename = input + 9; // Skip "head_nine"
    while (*filename == ' ') filename++; // Skip leading spaces

    /*
		This call to fopen takes the path to a file to open 
   		(the argument is `filename`, extracted from the user's input),
		followed by the access mode ("r" means read-only). fopen returns 
		a FILE pointer representing the open file, or NULL if the 
		operation failed. If NULL is returned, an error message is 
		printed using perror, and the function exits early.
	*/
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // Read and print the first 9 lines
    char line[1024];
    int lineCount = 0;

	/*
       This call to fgets reads a line from the file specified by `file` 
       into the buffer `line`. The first argument is the buffer to store 
       the line, the second argument is the size of the buffer, and the 
       third argument is the FILE pointer. It returns the buffer on success 
       or NULL on failure or EOF. The loop continues until EOF or the 
       specified number of lines is read.
    */
    while (fgets(line, sizeof(line), file) != NULL && lineCount < 9) {
        printf("%s", line);
        lineCount++;
    }

    /*
       This call to fclose closes the file specified by `file`. The argument 
       is the FILE pointer returned by fopen. It returns 0 on success or EOF 
       on failure. No explicit error handling is implemented here.
    */
    fclose(file);
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

		/*
           This call to getcwd retrieves the current working directory 
           and stores it in the `currWorkingDir` buffer. The first argument 
           is the buffer to store the directory path, and the second argument 
           is the size of the buffer. If the operation fails, it returns NULL, 
           but no explicit error handling is implemented here.
        */
		getcwd(currWorkingDir, sizeof(currWorkingDir));


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

        // Check for redirection (">")
        char *redirect = strchr(trimmedInput, '>');
        if (redirect != NULL) {
            *redirect = '\0'; // Split the input at '>'
            redirect++;       // Move to the filename part
            while (*redirect == ' ') redirect++; // Skip leading spaces in the filename

            // Check for missing filename
            if (strlen(redirect) == 0) {
                printf("Error: No filename provided for redirection.\n");
                continue;
            }

            // Check for multiple arguments after the filename
            char *extraArgs = strchr(redirect, ' ');
            if (extraArgs != NULL) {
                printf("Error: Multiple arguments provided for redirection. Only one filename is allowed.\n");
                continue;
            }

            // Create filenames for output and error files
            char outputFileName[1024];
            char errorFileName[1024];
            snprintf(outputFileName, sizeof(outputFileName), "%s.output", redirect);
            snprintf(errorFileName, sizeof(errorFileName), "%s.error", redirect);

            /*
               This call to open creates or truncates the file specified by 
               `outputFileName` or `errorFileName` for writing. The first argument 
               is the file path, the second argument specifies the access mode 
               (O_WRONLY | O_CREAT | O_TRUNC means write-only, create if it doesn't 
               exist, and truncate if it does), and the third argument specifies 
               the file permissions (0644 means read/write for owner, read-only 
               for others). It returns a file descriptor, or -1 if the operation 
               failed. If -1 is returned, an error message is printed using perror.
            */
            int outputFile = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            int errorFile = open(errorFileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            // Open the output file for writing
            if (outputFile == -1) {
                perror("Error opening output file for redirection");
                continue;
            }

            // Open the error file for writing
            if (errorFile == -1) {
                perror("Error opening error file for redirection");
                close(outputFile);
                continue;
            }

            // Save the current stdout and stderr file descriptors
            savedStdout = dup(STDOUT_FILENO);
            int savedStderr = dup(STDERR_FILENO);
            if (savedStdout == -1 || savedStderr == -1) {
                perror("Error saving stdout or stderr");
                close(outputFile);
                close(errorFile);
                continue;
            }

			/*
   				This call to dup duplicates the file descriptor specified by 
   				STDOUT_FILENO (standard output). It returns a new file descriptor, 
				or -1 if the operation failed. If -1 is returned, an error message
				is printed using perror, and the program continues without redirection.
			*/
            if (dup2(outputFile, STDOUT_FILENO) == -1) {
                perror("Error redirecting stdout");
                close(outputFile);
                close(errorFile);
                close(savedStdout);
                close(savedStderr);
                continue;
            }

			/*
   				This call to dup duplicates the file descriptor specified by 
   				STDERR_FILENO (standard error). It returns a new file descriptor,
				or -1 if the operation failed. If -1 is returned, an error message
				is printed using perror, and the program continues without redirection.
			*/
            if (dup2(errorFile, STDERR_FILENO) == -1) {
                perror("Error redirecting stderr");
                close(outputFile);
                close(errorFile);
                close(savedStdout);
                close(savedStderr);
                continue;
            }

            close(outputFile); // Close the output file descriptor, as it's now duplicated
            close(errorFile);  // Close the error file descriptor, as it's now duplicated
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

        } else if (strcmp(trimmedInput, "help") == 0) {
            PrintHelp();
		
		} else if (strncmp(trimmedInput, "head_nine", 9) == 0) {
            HeadNine(trimmedInput);
			
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