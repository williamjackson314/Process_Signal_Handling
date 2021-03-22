#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include "wrappers.h"

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
	pid_t C_pid;
	int status;
	char curr_name;
	char child_name;

	struct sigaction sigusr1_action;
	struct sigaction sigusr2_action;
	sigset_t unblock, block_all, old_mask;

	if (sigfillset(&block_all) < 0) dieWithError("\nsig fill set error"); // fill all bits in blocked set
	if (sigemptyset(&unblock) < 0) dieWithError("\nsig empty set error");

	sigusr1_action.sa_handler = sigusr1_handler;
	if (sigemptyset(&sigusr1_action.sa_mask) < 0) dieWithError("\nsig empty set error");  // empty blocked set
	sigusr1_action.sa_flags = SA_RESTART;

	sigusr2_action.sa_handler = sigusr2_handler;
	if (sigemptyset(&sigusr2_action.sa_mask) < 0) dieWithError("\nsig empty set error");  // empty blocked set
	sigusr2_action.sa_flags = SA_RESTART;

	sigaction(SIGUSR1, &sigusr1_action, NULL);
	sigaction(SIGUSR2, &sigusr2_action, NULL);

	greet('A');

	if ((B_pid = fork()) == 0) {
		
		greet('B');
		

		
		if ((C_pid = fork()) == 0) {
			greet('C');
			kill(parent_pid, SIGUSR1); //Sending signal to A, however may need to send to B, then to A
			
			if (sigprocmask(SIG_BLOCK, &block_all, &old_mask) < 0) dieWithError("\nsig proc mask error");
			while(sigusr2_flag == 0){
				sigsuspend(&unblock);
			}
			
			sigusr2_flag = 0;
			if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) dieWithError("\nsig proc mask error");
			
			goaway('C');
		}
		
		wait(&status);
		kill(parent_pid, SIGUSR1); // signify that process C has terminated, so process D can now exit
		mourn('B', 'C', status);
		goaway('B');
	}
	else {
		
		if ((D_pid = fork()) == 0) {
			
			if (sigprocmask(SIG_BLOCK, &block_all, &old_mask) < 0) dieWithError("\nsig proc mask error");
			
			while(sigusr1_flag == 0){
				sigsuspend(&unblock);
			}
			
			sigusr1_flag = 0;
			if (sigprocmask(SIG_SETMASK,&old_mask, NULL) < 0) dieWithError("\nsig proc mask error");		
			
			greet('D');
			kill(parent_pid, SIGUSR2); //signal that process D has called greet, so process C can now exit
			
			if (sigprocmask(SIG_BLOCK, &block_all, &old_mask) < 0) dieWithError("\nsig proc mask error");
			
			while(sigusr1_flag == 0){
				sigsuspend(&unblock);
			}
			
			sigusr1_flag = 0;
			if (sigprocmask(SIG_SETMASK,&old_mask, NULL) < 0) dieWithError("\nsig proc mask error");	
			
			goaway('D');
			
		}
		
		// Waits for signal from process C indicating that it has called greet, then sends that signal to process D
		if (sigprocmask(SIG_BLOCK, &block_all, &old_mask) < 0) dieWithError("\nsig proc mask error");
		
		while(sigusr1_flag == 0){	//waits for signal handler to change condition flag
			sigsuspend(&unblock);
		}
		
		sigusr1_flag = 0;
		if (sigprocmask(SIG_SETMASK,&old_mask, NULL) < 0) dieWithError("\nsig proc mask error"); // reset to original mask set
		kill(D_pid, SIGUSR1);

		// Waits for signal from process D indicating that it has called greet, then sends that signal to process C
		if (sigprocmask(SIG_BLOCK, &block_all, &old_mask) < 0) dieWithError("\nsig proc mask error");
		
		while(sigusr2_flag == 0){
			sigsuspend(&unblock);
		}
		
		sigusr2_flag = 0;
		if (sigprocmask(SIG_SETMASK,&old_mask, NULL) < 0) dieWithError("\nsig proc mask error");
		kill(C_pid, SIGUSR2);

		// Waits for signal indicating that process C has has terminated, then sends that signal to D
		if (sigprocmask(SIG_BLOCK, &block_all, &old_mask) < 0) dieWithError("\nsig proc mask error");
		
		while(sigusr1_flag == 0){
			sigsuspend(&unblock);
		}
		
		sigusr1_flag = 0;
		if (sigprocmask(SIG_SETMASK,&old_mask, NULL) < 0) dieWithError("\nsig proc mask error");
		kill(D_pid, SIGUSR1);

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
