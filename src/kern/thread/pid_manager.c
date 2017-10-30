#include <pid_manager.h>
#include <types.h>
#include <lib.h>
#include <synch.h>


//#define NULL 0 //NULL defined in types

//PRIVATE FUNCTIONS PROTOTYPES
static pid_t get_parent(pid_t pid);
static int get_exit_status(pid_t pid);
static error_code delete_pid_block(pid_t pid);


//static int create_pid_block(pid_t pid);


//PUBLIC FUNCTIONS HERE
//These were declared in pid_manager.h

int
create_pid_manager()
{


	if(pid_manager)
	{
		return PREEXISTING_MANAGER;
	}


	//pid_manager is global
	pid_manager = kmalloc(sizeof(struct pid_manager));

	int i;
	for (i = 0; i < MAX_PIDS; i++)
	{
		pid_manager->pid_info_blocks[i] = NULL;
	}

	pid_manager->next_pid = 1;
	pid_manager->pid_lock = lock_create("pid manager lock");
	assert(pid_manager->pid_lock);

	
	pid_manager->get_parent = get_parent;
	pid_manager->get_exit_status = get_exit_status;




	return SUCCESS;
}

void
destroy_pid_manager()
{
	return;
}




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










