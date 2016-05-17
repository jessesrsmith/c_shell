# A Shell in C

This is an implementation of a Unix shell. The shell reads input using `fgets()` and then either executes the command itself or forks and calls the appropriate program using `freopen()`. Return values are handled. 

The C-shell supports the following commands:

1. The internal shell command "exit" which terminates the shell.  
   **Concepts**: shell commands, exiting the shell  
   **System calls**: `exit()`  
 
2. The internal shell command "cd" which changes the present working directory  
   **Details**: This command takes a relative or absolute path and changes the present working directory to that path  
   **Concepts**: present working directory, absolute and relative paths  
   **System calls**: `chdir()`  
 
3. Any UNIX command, with or without arguments  
   **Details**: Your shell must block until the command completes and, if the return code is abnormal, print out a message to that effect. Argument 0 is the name of the command  
   **Concepts**: Forking a child process, waiting for it to complete, synchronous execution, Command-line parameters  
   **System calls**: `fork()`, `execvp()`, `exit()`, `wait()`  
 
4. The internal shell command “time” that reports the user and system time of the program that is specified.  
   **Example**:  `time ls -l`  
   **Details**: If the first token is “time”, executes the program that follows, waiting for it to complete. When it finishes, reports the user and system time spent executing the program.  
   **Concepts**: user and system time  
   **System calls**: times()  
 
5. Any UNIX command, with or without arguments, whose output is redirected to a file  
   **Example**: `ls -l > foo`  
   **Details**: This takes the output of the command and put it in the named file  
   **Concepts**: File operations, output redirection  
   **System calls**: `freopen()`  
 
6. Any UNIX command, with or without arguments, whose input is redirected from a file  
   **Example**:  `more < foo`  
   **Details**: This takes the named file and makes it the standard input of the program  
   **Concepts**: File operations, input redirection  
   **System calls**: `freopen()`  
 
