#ifndef _PID_MANAGER_H_
#define _PID_MANAGER_H_

#include <synch.h>
typedef int pid_t; //temp
typedef int error_code; //temp


#define MAX_PIDS 1024

struct pid_info_block {

	pid_t myparent;
	int is_exited;
	int exit_status;

	struct cv* exit_cv;


};


struct pid_manager {

	//DATA
	struct pid_info_block pid_info_blocks[MAX_PIDS];

	int number_of_procs;
	int next_pid;
	struct lock* pid_lock;

	//PUBLIC INTERFACE


}; 

//GLOBAL VARIABLE
struct pid_manager* pid_manager;

//Global Functions

struct pid_manager* create_pid_manager();
void destroy_pid_manager();



#endif
