#include <pid_manager.h>
#include <types.h>
#include <lib.h>
#include <synch.h>



//PRIVATE FUNCTIONS PROTOTYPES (Please try to maintain these in the same order as the defs)
static struct pid_info_block* get_pid_block(pid_t pid);
static error_code delete_pid_block(pid_t pid);
static int validate_pid(pid_t pid);

static struct pid_info_block create_pid_block(pid_t pid, pid_t parent);
static void destroy_pid_block();



//PRIVATE FUNCTION DEFINITIONS


/*
function: get_pid_block

Inputs: pid number of target process

Outputs:
	Returns pointer to pid info block or returns null if target does not exist.

Other Notes:
	Does not validate inputs

*/
static
struct pid_info_block*
get_pid_block(pid_t pid)
{
	if (validate_pid(pid) == 0)
		return NULL;


	struct pid_info_block* target = pid_manager->pid_info_blocks[pid - 1];
	return target;
}


static
int
validate_pid(pid_t pid)
{
	if (pid < 0 || pid > MAX_PIDS)
		return 0;

	return 1;
}

//TODO: add function to update system when process returns

//PUBLIC PROTOTYPES (Only for functions that will go into the struct)
static pid_t get_parent(pid_t pid);
static int get_exit_status(pid_t pid);


//PUBLIC FUNCTIONS HERE
//These were declared in pid_manager.h

int
create_pid_manager()
{

	pid_manager = kmalloc(sizeof(struct pid_manager));
	assert(pid_manager);

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

	return 1;
}

void
destroy_pid_manager()
{
	kfree(pid_manager);
}



/*
Function: get_parent

Inputs: get parent of input pid

Outputs:
	Return -1 if pid is invalid for any reason (out of bounds or not found)
	Return 0 if process has no parent
	Return parent_pid if successfull

Other Notes:
	This function should never crash the system.
*/
static
pid_t get_parent(pid_t pid) {
	assert(pid_manager != NULL);

	struct pid_info_block *info_block = get_pid_block(pid);

	if (info_block == NULL)
		return -1;

	return info_block->my_parent;
}





/*
Function: get_exit_status

Inputs: get parent of input pid

Outputs:
	Return -1 if pid is invalid for any reason (out of bounds or not found)
	Return 0 if process has no parent
	Return parent_pid if successfull

Other Notes:
	This function should never crash the system.

*/
static 
int get_exit_status(pid_t pid) {
	assert(pid_manager != NULL);

	struct pid_info_block *info_block = get_pid_block(pid);

	if (info_block == NULL)
		return -1;

	return info_block->exit_status;
}













