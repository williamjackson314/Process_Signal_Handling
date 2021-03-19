#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

//Declare global variables	
pid_t B_pid;
pid_t D_pid;
sig_atomic_t sigusr1_flag = 0;
sig_atomic_t sigusr2_flag = 0;

void dieWithError(char *);

void sigusr1_handler(){

	sigusr1_flag = 1;
}

void sigusr2_handler(){

	sigusr2_flag = 1;
}

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
	pid_t parent_pid = getpid();
	pid_t child_pid;
	int status;
	char curr_name;
	char child_name;
	sigset_t unblock, block_all, old_mask;

	if (sigfillset(&block_all) < 0) dieWithError(); // fill all bits in blocked set
	if (sigemptyset(&unblock) < 0) dieWithError();  // empty blocked set

	greet('A');

	if ((B_pid = fork()) == 0) {
		
		greet('B');
		
		pid_t temp_pid;
		
		if ((temp_pid = fork()) == 0) {
			greet('C');
			kill(parent_pid, SIGUSR1); //Sending signal to A, however may need to send to B, then to A

			goaway('C');
		}
		
		wait(&status);
		kill(parent_pid, SIGUSR1); // signify that process C has terminated, so process D can now exit
		mourn('B', 'C', status);
		goaway('B');
	}
	else {
		
		if ((D_pid = fork()) == 0) {
			
			greet('D');
			kill(parent_pid, SIGUSR2); //signal that process D has called greet, so process C can now exit
			goaway('D');
			
		}
	
		child_pid = wait(&status);
		child_name = pid_to_name(child_pid);
		mourn('A', child_name, status);

	}

	child_pid = wait(&status);
	child_name = pid_to_name(child_pid);
	mourn('A', child_name, status);
	
	goaway('A');

  return 0;
}
