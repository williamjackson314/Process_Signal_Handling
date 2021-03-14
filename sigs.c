#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

typedef void (*sighandler_t)(int);
void dieWithError(char *);

#define LIMIT 20
#define PERIOD 5

/* global variables for communication between handlers and main */
int sigalarm_count = 0;
int sigint_count = 0;

/* YOUR CODE HERE - Signal handler for SIGALRM */
void sigalarm_handler(int signum){
  
  sigalarm_count += 1;
  alarm(PERIOD);

} 
/* YOUR CODE HERE - Signal handler for SIGINT */
void sigint_handler(int signum){
  sigint_count += 1;
}

int main() {

  struct sigaction sigint_action;
  struct sigaction sigalarm_action;

  sigset_t mask, prev;
  
  pid_t pid;
  pid_t parent_pid;
  if ((pid = fork()) == 0){
    parent_pid = getppid();
    getchar();

    for (int i = 0; i < LIMIT; i++) {
      kill(parent_pid, SIGINT);  
    }
    printf("Child: finished sending SIGINT %d times", LIMIT);
    exit(0);
  }
    sigint_action.sa_handler = sigint_handler;
    sigemptyset(&sigint_action.sa_mask);
    sigalarm_action.sa_handler = sigalarm_handler;
    sigemptyset(&sigalarm_action.sa_mask);
   
    sigaction(SIGINT, &sigint_action, NULL);
    sigaction(SIGALRM, &sigalarm_action, NULL);
    
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &mask, &prev);

    alarm(PERIOD);
    
    while ((sigint_count <= LIMIT) || (sigalarm_count <= LIMIT)){

      printf(".");
      sigsuspend(&prev);
    }

    printf("SIGINT counter = %d \nSIGALARM counter = %d", sigint_count, sigalarm_count);
    exit(0);

}
