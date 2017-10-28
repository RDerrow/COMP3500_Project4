#include <pid_manager.h>


#define NULL 0 //deal with it

//PRIVATE FUNCTIONS PROTOTYPES
static pid_t get_parent(pid_t pid);
static int get_exit_status(pid_t pid);
static error_code delete_pid_block(pid_t pid);


//static int create_pid_block(pid_t pid);




//PRIVATE FUNCTION DEFINITIONS




//PUBLIC FUNCTIONS HERE
//These were declared in pid_manager.h

struct pid_manager*
create_pid_manager()
{

	return NULL;
}

void
destroy_pid_manager()
{
	return;
}









