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
  int             highestJobNo;  /* job number */
  int             lineLength;


  /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    char* fgetsReturn = fgets(line, NL, stdin);
    if (fgetsReturn == NULL){
      printf("fgets is NULL\n");
    }
    fflush(stdin);
    lineLength = strlen(line);

    if (feof(stdin)) {		/* non-zero on EOF  */

      fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
	      feof(stdin), ferror(stdin));
      exit(0);
    }
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
      continue;			/* to prompt */

    v[0] = strtok(line, sep);
    for (i = 1; i < NV; i++) {
      v[i] = strtok(NULL, sep);
      if (v[i] == NULL)
	      break;
    }
    /* assert i is number of tokens + 1 */

    // If the command does not end with a "&", don't fork a child process and run it on
    // the main one instead
    
    printf("%c\n", line[lineLength - 1]);

    /* fork a child process to exec the command in v[0] */

    switch (frkRtnVal = fork()) {
    case -1:			/* fork returns error to parent process */
      {
	      break;
      }
    case 0:			/* code executed only by child process */
      {        
        // int localJobNo = highestJobNo;
        highestJobNo++;
        execvp(v[0], v);
      }
    default:			/* code executed only by parent process */
      {
        wpid = wait(0);
        printf("%s done id: %d\n", v[0], wpid);
        break;
      }
    }				/* switch */
  }				/* while */

  return 0;
}				/* main */
