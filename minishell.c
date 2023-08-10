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
#define NP 100		  /* max number of concurrent background processes */
char            line[NL];	/* command input buffer */


/*
	shell prompt
 */

void prompt(void){
  // fprintf(stdout, " msh> ");
  fflush(stdout);
  return;
}

// Prints all child processes that are done and
// updates all job ids to fill in gaps left by done
// child processes
void reportDoneChildProcesses(int processes[NP]){
  bool noProcessesActive = true;

  // For every job in the processes list...
  for (int jobno = 0; jobno < NP; jobno++){
    int pid = processes[jobno];

    // If the job number has an active job...
    if (pid != 0){
      // Check if the associated child process is still active
      int wpid = waitpid(pid, 0, WNOHANG);
      if (wpid == 0){
        // If a child process is still going, don't reset
        // the last job id to 1
        noProcessesActive = false;
      } else {
        // If a child is done, report it, and mark as done
        printf("[%d]+  Done                    %d\n", jobno, wpid);
        // -1 indicates a job is done, but the job id isn't free yet
        processes[jobno] = -1;
      }
    }
  }

  // If no processes are active, reset the highest available
  // job number to 1
  if (noProcessesActive){
    for (int jobNo = 1; jobNo < NP; jobNo++){
      processes[jobNo] = 0;
    }
  }

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
  int             processes[NP] = {0}; /* Array of active process job numbers */


  /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    char* fgetsReturn = fgets(line, NL, stdin);
    if (fgetsReturn == NULL){
      printf("fgets is NULL\n");
    }
    fflush(stdin);
    reportDoneChildProcesses(processes);

    if (feof(stdin)) {		/* non-zero on EOF  */

      // fprintf(stderr, "EOF pid %d feof %d ferror %d\n", getpid(),
	    //   feof(stdin), ferror(stdin));
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

        // If the last parameter is "&" then remove it
        if (strcmp(v[parameterCount], "&") == 0){
          v[parameterCount] = NULL;
        } else {
          v[parameterCount][strlen(v[parameterCount]) - 1] = '\0';
        }
      }
    }
    
    // printf("Last char: %c Parameter count: %d Background process: %d\n", lastChar, parameterCount, backgroundProcess);
    // printf("v[0]: %s v[1]: %s\n", v[0], v[1]);
    
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
          // If we're executing a background process,
          // find the first available job number,
          // and assign that to the process
          for (int jobNo = 1; jobNo < NP; jobNo++){
            if (processes[jobNo] == 0){
              processes[jobNo] = frkRtnVal;
              // Report the process as started
              printf("[%d] %d\n", jobNo, frkRtnVal);
              break;
            }
            
          }

          } else {
          // If we aren't executing a background process,
          // then wait for the child process to finish
          wpid = waitpid(frkRtnVal, 0, 0);
          if (wpid == -1){
            printf("wpid is -1\n");
          }
          
        }

        break;
      }
    }				/* switch */
  }				/* while */

  return 0;
}				/* main */
