#include <types.h>
#include <lib.h>
#include <curthread.h>
#include <thread.h>
#include <syscall.h>
<<<<<<< HEAD
#include <curthread.h>
=======
#include <pid_manager.h>
>>>>>>> master


/*
TODO: write system call functions.

These should be machine independent.


*/



/*int
sys_fork(struct trapframe *tf, pid_t *retval) {

	struct trapframe *new_tf = kmalloc(sizeof(struct trapframe));
	new_tf = *tf;

	int result = 0;//thread_fork(curthread->t_name, new_tf, 0, 	child_thread, retval);

	if (result != 0) {
		kfree(new_tf);
		return result;
	}

	return result;

}*/
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
int
sys_getpid(pid_t* retval)
{
	
	*retval = pid_manager->get_pid(curthread);
	return 0;
}


//sys_exit
void
sys_exit(int code)
{
	_exit(code);
	panic("Thread did not exit correctly!");
}
