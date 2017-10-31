#include <pid_manager.h>
#include <types.h>
#include <lib.h>
#include <synch.h>



//PRIVATE FUNCTIONS PROTOTYPES (Please try to maintain these in the same order as the defs)
struct pid_info_block* get_pid_block(pid_t pid);
static pid_t get_parent(pid_t pid);
static int get_exit_status(pid_t pid);
static error_code delete_pid_block(pid_t pid);



//PRIVATE FUNCTION DEFINITIONS


/*
function: get_pid_block

Inputs: pid number of target process

Outputs:
	Returns pointer to pid info block or returns null if target does not exist.

Other Notes:
	Will panic and crash OS if given a pid that is out of bounds.
	//TODO: Consider if this is really the best way to handle this.

*/
struct pid_info_block*
get_pid_block(pid_t pid)
{

	if (pid < 0 || pid > MAX_PIDS)
	{
		panic("The given pid value is invalid!\nI'm sorry master. I failed.");
		return NULL;
	}


	struct pid_info_block* target = pid_manager->pid_info_blocks[pid - 1];

	return target;
}



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

//TODO: add function to update system when process returns


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















