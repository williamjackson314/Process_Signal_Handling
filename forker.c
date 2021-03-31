#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include "wrappers.h"

//Declare global variables	
pid_t B_pid, D_pid;

void greet(char myName)
{ 
	printf("Process %c, PID %d greets you.\n",myName,getpid());
	fflush(stdout);
}

void goaway(char myName)
{
	printf("Process %c exits normally.\n", myName);
	exit(0);
}

void mourn(char parent_name, char child_name, int status){

	if (WIFEXITED(status)){
		printf("Process %c here: Process %c exited with status %x.\n", parent_name, child_name, WEXITSTATUS(status));
	}

}

char pid_to_name(pid_t proc_id){

	if (proc_id == B_pid)
		return 'B';
	else if (proc_id == D_pid)
		return 'D';

} 

int main()
{
	pid_t pid, child_pid;
	int status;
	char child_name;

	greet('A');

	if ((B_pid = Fork()) == 0) {
		
		greet('B');
		
		if ((pid = Fork()) == 0) {
			greet('C');
			goaway('C');
		}
		
		Wait(&status);
		mourn('B', 'C', status);
		goaway('B');
	}

	if ((D_pid = Fork()) == 0) {
		
		greet('D');
		goaway('D');
		
	}

	child_pid = Wait(&status);
	child_name = pid_to_name(child_pid);
	mourn('A', child_name, status);

	child_pid = Wait(&status);
	child_name = pid_to_name(child_pid);
	mourn('A', child_name, status);
	
	goaway('A');

  return 0;
}
