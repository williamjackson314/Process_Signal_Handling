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
  
  // sigset_t mask, prev_mask;

  // sigfillset(&mask);
  // sigprocmask(SIG_BLOCK, &mask, &prev_mask);
  
  sigalarm_count += 1;
  alarm(PERIOD);
  
  // sigprocmask(SIG_SETMASK, &prev_mask, NULL);
  
} 

void sigint_handler(int signum){
  
  // sigset_t mask, prev_mask;

  // sigfillset(&mask);
  // sigprocmask(SIG_BLOCK, &mask, &prev_mask);
  
  sigint_count += 1;
  
  //sigprocmask(SIG_SETMASK, &prev_mask, NULL);
  
}

int main() {

  struct sigaction sigint_action;
  struct sigaction sigalarm_action;

  sigset_t mask, prev;
  
  pid_t pid;
  pid_t parent_pid;

  sigint_action.sa_handler = sigint_handler;
  sigemptyset(&sigint_action.sa_mask);
  sigint_action.sa_flags = SA_RESTART;

  sigalarm_action.sa_handler = sigalarm_handler;
  sigemptyset(&sigalarm_action.sa_mask);
  sigalarm_action.sa_flags = SA_RESTART;

  sigaction(SIGINT, &sigint_action, NULL);
  sigaction(SIGALRM, &sigalarm_action, NULL);
  
  if ((pid = fork()) == 0){
    parent_pid = getppid();
    getchar();

    for (int i = 0; i < LIMIT; i++) {
      kill(parent_pid, SIGINT);  
    }
    printf("Child: finished sending SIGINT %d times\n", LIMIT);
    exit(0);
  }

    sigemptyset(&mask);
    sigemptyset(&prev);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, &prev);

    alarm(PERIOD);
    
    while ((sigint_count < LIMIT) && (sigalarm_count < LIMIT)){

      printf(".\n");
      sigsuspend(&prev);
      
    }

    sigprocmask(SIG_SETMASK, &prev, NULL);
    printf("SIGINT counter = %d \nSIGALARM counter = %d\n", sigint_count, sigalarm_count);
    exit(0);

}
