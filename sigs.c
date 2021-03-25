#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "wrappers.h"

typedef void (*sighandler_t)(int);
void dieWithError(char *);

#define LIMIT 20
#define PERIOD 5

/* global variables for communication between handlers and main */
int sigalarm_count = 0;
int sigint_count = 0;


void sigalarm_handler(int signum){
  
  sigset_t mask, prev_mask;

  if (sigfillset(&mask) < 0) dieWithError("\nsig fill set error");
  if (sigprocmask(SIG_BLOCK, &mask, &prev_mask) < 0) dieWithError("\nsig proc mask error");
  
  sigalarm_count += 1;
  alarm(PERIOD);
  
  if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) < 0) dieWithError("\nsig proc mask error");
  
} 

void sigint_handler(int signum){
  
  sigset_t mask, prev_mask;

  if (sigfillset(&mask) < 0) dieWithError("\nsig fill set error");
  if (sigprocmask(SIG_BLOCK, &mask, &prev_mask) < 0) dieWithError("\nsig proc mask error");
  
  sigint_count += 1;
  
  if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) < 0) dieWithError("\nsig proc mask error");
  
}

int main() {

  struct sigaction sigint_action;
  struct sigaction sigalarm_action;

  sigset_t mask, prev;
  
  pid_t pid;
  pid_t parent_pid;

  sigint_action.sa_handler = sigint_handler;
  if (sigemptyset(&sigint_action.sa_mask) < 0) dieWithError("\nsig empty set error");
  sigint_action.sa_flags = SA_RESTART;

  sigalarm_action.sa_handler = sigalarm_handler;
  if(sigemptyset(&sigalarm_action.sa_mask) < 0) dieWithError("\nsig empty set error"); 
  sigalarm_action.sa_flags = SA_RESTART;

  if (sigaction(SIGINT, &sigint_action, NULL) < 0) dieWithError("\nsig action error");
  if (sigaction(SIGALRM, &sigalarm_action, NULL) < 0) dieWithError("\nsig action error");
  
  if ((pid = fork()) == 0){
    parent_pid = getppid();
    getchar();

    for (int i = 0; i < LIMIT; i++) {
      if (kill(parent_pid, SIGINT) < 0) dieWithError("\nkill error"); 
    }
    printf("Child: finished sending SIGINT %d times\n", LIMIT);
    exit(0);
  }

    if (sigemptyset(&mask) < 0) dieWithError("\nsig empty set error");
    if (sigemptyset(&prev) < 0 ) dieWithError("\nsig empty set error");

    if (sigaddset(&mask, SIGINT) < 0) dieWithError("\nsig add set error");
    if (sigaddset(&mask, SIGALRM) < 0) dieWithError("\nsig add set error");

    if (sigprocmask(SIG_BLOCK, &mask, &prev) < 0) dieWithError("\nsig proc mask error");

    alarm(PERIOD);
    
    while ((sigint_count < LIMIT) && (sigalarm_count < LIMIT)){

      printf(".\n");
      sigsuspend(&prev);

    }

    if (sigprocmask(SIG_SETMASK, &prev, NULL) < 0) dieWithError("\nsig proc mask error");
    printf("SIGINT counter = %d \nSIGALARM counter = %d\n", sigint_count, sigalarm_count);
    exit(0);

}
