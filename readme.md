# Wash Shell
Dylan Beppu  
CSC 3350  
Seattle Pacific University  
Professor Andy Cameron

## About
This shell program is a extra credit assignment for CSC 3350. The point of this is to show how shell works and 

## Requirements
* Linux based OS or WSL
* Ubuntu 22.04 or higher
* GCC compiler


## Build & Run

Build instructors:  
`gcc wash.c head_nine.c -o WASH`

Run instructions:  
`./WASH`

If using input redirection on wsl, use a utility such as dos2unix to convert the file to the program can run.
Usage `dos2unix <file>`

## Flags
`-h` Displays the built in commands for wash then exits


## Commands

`help` - Displays this help screen with information about the program's usage.  

`exit` - Exits the shell.  

`pwd` - Prints the current working directory.  

`cd <path>` - Changes the current directory to the specified path. Use '..' to move to the parent directory. If no path is provided, it changes to the home directory.

`head_nine <flags>` - Displays the first 9 lines of the specified file. 

`setpath <path>` - Sets the PATH variable to the specified value.

`echo <message>` - Prints the specified message to the console.

`./<program>` - Executes a program located in the current directory.

`<program>` - Searches for the program in the directories specified in the PATH variable and executes it.

`<command> > <filename>`   - Redirects the output of a command to a file. Creates two files: <filename>.output for normal output and <filename>.error for errors. If no filename is provided or multiple arguments are given, an error is displayed.\n\n");