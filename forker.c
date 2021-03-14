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
	printf("Process %c exited normally.\n", myName);
	exit(0);
}

void mourn(char parent_name, char child_name, int status){

	if (WIFEXITED(status)){
		printf("Process %c here: Process %c exited with status %d.\n", parent_name, child_name, WEXITSTATUS(status));
	}
}


char pid_to_name(pid_t base_id, pid_t proc_id){
	
	char names[] = {'A','B','C','D'};
	int i = (base_id - proc_id);

	return names[i];
} 

int main()
{
	pid_t base_pid = getpid();
	pid_t pid;
	int status;
	char curr_name;

	curr_name = pid_to_name(base_pid, getpid());
	greet(curr_name);

	if ((pid = fork()) == 0) {
		
		curr_name = pid_to_name(base_pid, getpid());
		greet(curr_name);
		
		pid = fork();
		
		if (pid == 0) {

			curr_name = pid_to_name(base_pid, getpid());
			greet(curr_name);
			goaway(curr_name);
		}

		pid_t child_pid = wait(&status);
		char child_name = pid_to_name(base_pid, child_pid);
		mourn(curr_name, child_name);
		goaway(curr_name);
	}

	if ((pid = fork()) == 0) {

		curr_name = pid_to_name(base_pid, getpid());
		greet(curr_name);
		goaway(curr_name);
		
	}
 
	pid_t child_pid = wait(&status);
	char child_name = pid_to_name(base_pid, child_pid);
	mourn(curr_name, child_name);
	goaway(curr_name);

  return 0;
}
