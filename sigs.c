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

/* YOUR CODE HERE - Signal handler for SIGINT */


int main() {
  /* YOUR CODE HERE - the textbook and slides have examples
   * of how to install signal handlers, block signals, etc.
   */
  
}
