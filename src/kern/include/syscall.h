#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);
//TODO: add prototypes for system call functions
void _exit(int exitcode);
int sys_fork(struct trapframe *tf, pid_t *retval);



#endif /* _SYSCALL_H_ */
