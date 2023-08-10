/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

#define NV 20			/* max number of command tokens */
#define NL 100			/* input buffer size */
char            line[NL];	/* command input buffer */


/*
	shell prompt
 */

void prompt(void){
  fprintf(stdout, "\n msh> ");
  fflush(stdout);
  return;
}


int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
  int             frkRtnVal;	/* value returned by fork sys call */
  int             wpid;		/* value returned by wait */
  char           *v[NV];	/* array of pointers to command line tokens */
  char           *sep = " \t\n";/* command line token separators    */
  int             i;		/* parse index */
  int             parameterCount; /* number of parameters passed */
  char            lastChar; /* last character in line input */
  bool            backgroundProcess; /* whether the current command should be run in background */

  /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    char* fgetsReturn = fgets(line, NL, stdin);
    if (fgetsReturn == NULL){
      printf("fgets is NULL\n");
    }
    fflush(stdin);

    if (feof(stdin)) {		/* non-zero on EOF  */

      fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
	      feof(stdin), ferror(stdin));
      exit(0);
    }
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
      continue;			/* to prompt */

    parameterCount = 0;
    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++) {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL) {
	      break;
      } else {
        parameterCount++;
      }
    }
    /* assert i is number of tokens + 1 */

    // If the command is "cd", don't use a child process and
    // run chdir() on the main process instead!
    if (strcmp(v[0], "cd") == 0) {
      int chdirReturn = chdir(v[1]);

      if (chdirReturn == -1){
        printf("chdir return is -1");
      }
      continue;
    } else if (strcmp(v[0], "exit") == 0) {
      return 0;
    }

    // If the last character input in the line is "&"
    // put the process in "background mode" and allow
    // for a new command to be run while the background
    // one is running
    // (i.e. don't wait for the child process to be done)
    if (parameterCount == 0){
      lastChar = line[strlen(line) - 1];

      backgroundProcess = lastChar == '&';
      if (backgroundProcess){
        line[strlen(line) - 1] = '\0';
      }
    } else {
      lastChar = v[parameterCount][strlen(v[parameterCount]) - 1];

      backgroundProcess = lastChar == '&';
      if (backgroundProcess){
        v[parameterCount][strlen(v[parameterCount]) - 1] = '\0';
      }
    }
    
    printf("Last char: %c Parameter count: %d Background process: %d\n", lastChar, parameterCount, backgroundProcess);
    printf("v[0]: %s v[1]: %s\n", v[0], v[1]);
    
    /* fork a child process to exec the command in v[0] */

    switch (frkRtnVal = fork()) {
    case -1:			/* fork returns error to parent process */
      {
	      break;
      }
    case 0:			/* code executed only by child process */
      {
        execvp(v[0], v);
        return 0;
      }
    default:			/* code executed only by parent process */
      {
        if (backgroundProcess){
          printf("Background process: %s", v[0]);
        } else {
          wpid = wait(0);
          printf("%s done id: %d\n", v[0], wpid);
        }

        break;
      }
    }				/* switch */
  }				/* while */

  return 0;
}				/* main */
