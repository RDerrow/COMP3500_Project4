#include <pid_manager.h>
#include <types.h>
#include <lib.h>


//#define NULL 0 //NULL defined in types

//PRIVATE FUNCTIONS PROTOTYPES
static pid_t get_parent(pid_t pid);
static int get_exit_status(pid_t pid);
static error_code delete_pid_block(pid_t pid);


//static int create_pid_block(pid_t pid);




//PRIVATE FUNCTION DEFINITIONS
static
pid_t get_parent(pid_t pid) {
	assert(pid_manager != NULL);
	struct pid_info_block *info_block = pid_manager->pid_info_blocks[pid];
	return info_block->my_parent;
}

static 
int get_exit_status(pid_t pid) {
	assert(pid_manager != NULL);
	struct pid_info_block *info_block = pid_manager->pid_info_blocks[pid];
	return info_block->exit_status;
}



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









