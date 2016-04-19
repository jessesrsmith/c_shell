//Preprocessor Directives
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <errno.h>
#include <sys/times.h>

//Used to print "last login"
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

//Macros
#define BUFFER 300

//Globals
static char command[BUFFER]; //
static char copy[BUFFER];
static char *token;

//Used to print "last login"
static struct stat file_stat;

//Function Prototypes
void change_directory();
void run_time();
void redirect_output(char flag);
void run_program();

//Main
int main() {
  //Declaration
  char *redirect_pos;
  char* username;

  //Get user name
  username = getlogin(); 

  //Print "last login." Really just the last time the file was accessed. 
  stat(__FILE__, &file_stat);
  printf("Last login: %s", ctime(&file_stat.st_atime));

  //Control Loop
  while(1){
    printf("%s $ ", username);

    //Get user input
    fgets(command, sizeof command, stdin);

    //Copy user input
    strcpy(copy, command);

    //Tokenize command
    token = strtok(command, " \t\n()<>|&;");

    //If no input, continue loop.
    if(!token){
      continue;
    }

    //Exit
    else if(strcmp(token, "exit") == 0) {
      exit(0);
    }

    //CD
    else if(strcmp(token, "cd") == 0) {
      change_directory();
    }

    //Time
    else if(strcmp(token, "time") == 0) {
      run_time();
    }

    //Output Redirection
    //Search "copy" for '>'
    else if((redirect_pos = strchr(copy, '>'))) {
      //Look for second '>'
      if ( *(redirect_pos + 1) == '>') {
        redirect_output('a');
      } 
      else {
        redirect_output('w');
      }
    }

    //Unix Commands
    else {
      run_program();
    }
  }

  return 0;
}

void change_directory() {
  char *home;
  home = getenv("HOME");

  //Get name of directory
  token = strtok(NULL, " \t\n()<>|&;");

  //Allow for using empty "cd" to change to home directory
  if (token == NULL) {
    if (home == NULL) {
      printf("HOME environment variable not set. Home directory commands will not work.\n");
      return;
    }
    if(chdir(home) == -1) {
      perror("Error");
    }
    return;
  }
  
  //Allow for using '~' to cd relative to the home directory
  //Concatenate remainder of token and HOME (eg users/jessesmith + /token)
  if (token[0] == '~') {
    if (home == NULL) {
      printf("HOME environment variable not set. Home directory commands will not work.\n");
      return;
    }
    char relative[400];
    strcpy(relative, home);
    token = strcat(relative, &token[1]);
  }

  //Returns 0 on success, on error -1 is returned and errno is printed as string
  if(chdir(token) == -1) {
    perror("Error");
  }
}

void run_time() {
  struct tms start_time;
  struct tms end_time;
  long double clktck;

  //Get start time
  if (times(&start_time) == -1) {
    perror("Error");
    return;
  }

  //Get next token after "time"
  token = strtok(NULL, " \t\n()<>|&;");
  run_program();

  //Get end time
  if (times(&end_time) == -1) {
    perror("Error");
    return;
  }

  //Get clock ticks per scond
  clktck = sysconf(_SC_CLK_TCK);

  //Print user and system time
  printf("\nuser\t%.3lfs", (end_time.tms_cutime - start_time.tms_cutime) / (double)clktck);
  printf("\nsys\t%.3lfs\n", (end_time.tms_cstime - start_time.tms_cstime) / (double)clktck);
}

void redirect_output(char flag) {
  pid_t pid;
  char *args[BUFFER];
  int arg_num;
  FILE* fp;
  int status;

  status = 0;
  arg_num = 0;
  pid = fork(); 

  //An error occoured with the fork
  if(pid == -1) {
    perror("Error");
    exit(EXIT_FAILURE);
  }
  //Child process
  else if(pid == 0) {
    args[arg_num] = token;

    while(token != NULL) {
      token = strtok(NULL, " \t\n()<>|&;");
      arg_num++;
      args[arg_num] = token;
    }

    //Second to last token should be file name, redirect output
    fp = freopen(args[arg_num - 1], &flag, stdout);
    if (fp == NULL) {
      perror("Error");
      return;
    }
    args[arg_num - 1] = NULL;

    //Run the program specified
    if((execvp(args[0], args) == -1)) {
      perror("Error");
    }   

    fclose(fp); 
    _exit(EXIT_SUCCESS); 
  } 
  //Parent process
  else {
    if (wait(&status) == -1) {
      perror("Error");
      return;
    }

    //Checks 'status' for normal exit
    if (WIFEXITED(status) == 0) {
      printf("Child process did not exit normally.");
    } 
  }
}

void run_program() {
  pid_t pid;
  char *args[BUFFER];
  int arg_num;
  int status;

  status = 0;
  arg_num = 0;
  pid = fork(); 

  //An error occoured with the fork
  if(pid == -1) {
    perror("Error");
    exit(EXIT_FAILURE);
  }
  //Child process
  else if(pid == 0) {
    args[arg_num] = token;

    while(token != NULL) {
      token = strtok(NULL, " \t\n()<>|&;");
      arg_num++;
      args[arg_num] = token;
    }

    //Run the program specified
    if((execvp(args[0], args) == -1)) {
      perror("Error");
    } 
    
    _exit(EXIT_SUCCESS); 
  } 
  //Parent process
  else {
    if (wait(&status) == -1) {
      perror("Error");
      return;
    }

    //Checks 'status' for normal exit
    if (WIFEXITED(status) == 0) {
      printf("Child process did not exit normally.");
    } 
  }
}
