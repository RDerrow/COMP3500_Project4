#ifndef _PID_MANAGER_H_
#define _PID_MANAGER_H_

#include <types.h>
#include <synch.h>
typedef int error_code; //temp


enum pid_error_code {
	SUCCESS,
	PREEXISTING_MANAGER,
	PID_DOES_NOT_EXIST
};




#define MAX_PIDS 1024

struct pid_info_block {

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

struct pid_manager* create_pid_manager();
void destroy_pid_manager();



#endif
