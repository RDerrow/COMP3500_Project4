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

struct pid_manager*
create_pid_manager()
{

	struct pid_manager* new_man = NULL;	

	new_man = kmalloc(sizeof(struct pid_manager));
	assert(pid_manager);

	int i;
	for (i = 0; i < MAX_PIDS; i++)
	{
		new_man->pid_info_blocks[i] = NULL;
	}

	new_man->next_pid = 1;
	new_man->pid_lock = lock_create("pid manager lock");
	assert(new_man->pid_lock);

	
	new_man->get_parent = get_parent;
	new_man->get_exit_status = get_exit_status;


	return new_man;
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










