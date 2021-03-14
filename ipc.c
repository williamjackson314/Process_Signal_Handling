#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>

typedef void (*sighandler_t)(int);
void dieWithError(char *);


void greet(char myName) {
  printf("Process %c, PID %d greets you.\n",myName,getpid());
  fflush(stdout);
}

int main() {
  /* YOUR CODE HERE - See Instructions */
  return 0;
}
