#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <syscall.h>


/*
TODO: write system call functions.

These should be machine independent.


*/




/*
function: sys_fork

inputs:	
	-struct trapframe* tf
	-pid_t*	retval

outputs:
	-returned by reference in retval
		-return 0 in child
		-return pid of child to parent
		-return -1 if fail

	-return 0 if success. Other if failed.

notes:
	-if operation fails, set global variable errno to appropriate error
	code specified in man pages


algorithm:
	see slide 25 of lecture 29
	
*/


//sys_waitpid


//sys_getpid



//sys_exit
