#include <types.h>
#include <syscall.h>
#include <thread.h>
#include <curthread.h>
#include <lib.h>

/* Sample implementation of sys_getpid() */


int
sys_getpid(pid_t *retval)
{

	*retval = curthread->t_pid;
	//kprintf("Does this actually even work?\n");
	//kprintf(*retval);
	//kprintf(5);
	return 0;

}
