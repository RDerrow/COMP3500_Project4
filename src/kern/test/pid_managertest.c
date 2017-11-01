#include <test.h>
#include <pid_manager.h>
//#include <types.h>
#include <lib.h>
#include <thread.h>
//TODO:add test code here

int sampletest(int useless, char** really_useless)
{


	kprintf("This is a sample test\n");

	return 0;
}


static int recursive_fork(int i, char** blah)
{

	if (i < 100)
	{

		thread_fork("fb", i+1, NULL, recursive_fork, NULL);
		thread_fork("fb", i+1, NULL, recursive_fork, NULL);


	}

	return 0;



}


int forkbomb(int i, char** really_useless)
{

		thread_fork("fb", 1, NULL, recursive_fork, NULL);
		thread_fork("fb", 1, NULL, recursive_fork, NULL);



	return 0;
}


int procnum(int i, char** blah)
{

	kprintf("Currently %d PIDS are in use\n", pid_manager->number_of_procs);

	return 0;
}
