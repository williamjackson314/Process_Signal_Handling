#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include "wrappers.h" 

//Declare global variables	
pid_t Bpid;
pid_t Dpid;

void dieWithError(char *);

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

	if (proc_id == Bpid)
		return 'B';
	else if (proc_id == Dpid)
		return 'D';

} 

int main()
{
	pid_t base_pid = getpid();
	pid_t pid;
	pid_t child_pid;
	int status;
	char curr_name;
	char child_name;

	greet('A');

	if ((Bpid = Fork()) == 0) {
		
		greet('B');
		
		if ((pid = Fork()) == 0) {
			greet('C');
			goaway('C');
		}
		
		Wait(&status);
		mourn('B', 'C', status);
		goaway('B');
	}
	else {
		
		if ((Dpid = Fork()) == 0) {
			
			greet('D');
			goaway('D');
			
		}
	
		child_pid = Wait(&status);
		child_name = pid_to_name(child_pid);
		mourn('A', child_name, status);

	}

	child_pid = Wait(&status);
	child_name = pid_to_name(child_pid);
	mourn('A', child_name, status);
	
	goaway('A');

  return 0;
}
