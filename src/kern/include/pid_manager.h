#ifndef _PID_MANAGER_H_
#define _PID_MANAGER_H_

#include <types.h>
#include <synch.h>
#include <array.h>
#include <thread.h>


typedef int error_code; //temp

//Test that checkout works to switch branches.

enum pid_error_code {
	SUCCESS = 0,
	PID_OUT_OF_BOUNDS = -1,
	PID_NOT_FOUND = -2
};


#define MAX_PIDS 100

struct pid_info_block {

	pid_t my_pid;
	pid_t my_parent;
	struct array* children;
	int is_exited;
	int exit_status;

	struct cv* exit_cv;


};


struct pid_manager {

	//DATA
	struct pid_info_block *pid_info_blocks[MAX_PIDS];

	int number_of_procs;
	int next_pid;
	struct lock* lock;

	//PUBLIC INTERFACE
	pid_t (*get_parent)(pid_t pid);
	pid_t (*get_pid)(struct thread* thread);
	int (*get_exit_status)(pid_t pid);
	int (*is_finished)(pid_t pid);
	int (*inform_process_add)(struct thread* newguy, pid_t parent);
	int (*inform_process_exit)(pid_t pid, int exit_status);
	int (*wait_pid)(pid_t target, pid_t caller, int* process_exit_status);

}; 
//GLOBAL VARIABLE
struct pid_manager* pid_manager;

//Global Functions

void pid_manager_bootstrap();
void destroy_pid_manager();





#endif
