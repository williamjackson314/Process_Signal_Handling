#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>


void dieWithError(char *);

void greet(char myName)
{ 
	printf("Process %c, PID %d greets you.\n",myName,getpid());
	fflush(stdout);
}

void goaway(char myName)
{
	
	printf("Process %d exited normally.\n", myName);
	exit(0);
}

void mourn(char parName, char childName){

	int status;
    pid_t result;
	wait(&status);
	if (WIFEXITED(status)){
		printf("Process %d here: Process %d exited with status %d.\n", parName, childName, WEXITSTATUS(status));
	}
}


// char pid_to_name(pid_t base_id, pid_t proc_id){
	
// 	char names[] = {'A','B','C','D'};
// 	int i = (base_id - proc_id);

// 	return names[i];
// } 

int main()
{

  return 0;
}
