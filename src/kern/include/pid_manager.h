#ifndef _PID_MANAGER_H_
#define _PID_MANAGER_H_

#include <types.h>
#include <synch.h>
typedef int error_code; //temp

//Test that checkout works to switch branches.

enum pid_error_code {
	SUCCESS = 0,
	PID_OUT_OF_BOUNDS = -1,
	PID_NOT_FOUND = -2
};


#define MAX_PIDS 1024

struct pid_info_block {

	pid_t my_pid;
	pid_t my_parent;
	int is_exited;
	int exit_status;

	struct cv* exit_cv;


};


struct pid_manager {

	//DATA
	struct pid_info_block *pid_info_blocks[MAX_PIDS];

	int number_of_procs;
	int next_pid;
	struct lock* pid_lock;

	//PUBLIC INTERFACE
	pid_t (*get_parent)(pid_t);
	int (*get_exit_status)(pid_t);

}; 
//GLOBAL VARIABLE
struct pid_manager* pid_manager;

//Global Functions

int create_pid_manager();
void destroy_pid_manager();



#endif
