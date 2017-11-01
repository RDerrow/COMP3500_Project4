#include <pid_manager.h>
#include <types.h>
#include <lib.h>
#include <synch.h>



//PRIVATE FUNCTIONS PROTOTYPES (Please try to maintain these in the same order as the defs)
static struct pid_info_block* get_pid_block(pid_t pid);
static void set_pid_block(pid_t pid, struct pid_info_block* block);
static int is_pid_free(pid_t pid);
static int should_free_pid(struct pid_info_block* block);
static int scan_free();
static error_code delete_pid_block(pid_t pid);
static int validate_pid(pid_t pid);

static struct pid_info_block* create_pid_block(pid_t pid, pid_t parent);
static void destroy_pid_block(struct pid_info_block* block);

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
void 
set_pid_block(pid_t pid, struct pid_info_block* block)
{
	if (validate_pid(pid) == 0)
		return;


	pid_manager->pid_info_blocks[pid - 1] = block; //KLUDGE: if mapping changes I'll have to change this code twice.
	
}


static 
int 
is_pid_free(pid_t pid)
{

	return get_pid_block(pid) == NULL;

}


static
int
should_free_pid(struct pid_info_block* block)
{

	if (block->is_exited = 0)
		return 0;

	if (block->my_parent == NULL || is_pid_free(block->my_parent) == 1)
		return 1;

	if (get_pid_block(block->my_parent)->is_exited == 1)
		return 1;
	

	return 0;
}



static
int
scan_free()
{
	int start = pid_manager->next_pid;

	if (is_pid_free(start))
	{
		return start;
	}

	int i;

	do {

	i = (i % MAX_PIDS) + 1;


	} while (!is_pid_free(i) && i != start);


	if (i == start)
	{
		return -1;
	}



	return i;
}


static
int
validate_pid(pid_t pid)
{
	if (pid < 0 || pid > MAX_PIDS)
		return 0;

	return 1;
}




//pid_info_block functions
static
struct pid_info_block*
create_pid_block(pid_t pid, pid_t parent)
{

	struct pid_info_block* info_block = kmalloc(sizeof(struct pid_info_block));

	if (!info_block)
	{
		return NULL;
	}

	info_block->my_pid = pid;
	info_block->my_parent = parent;

	info_block->children = array_create();

	if (!info_block->children)
	{
		kfree(info_block);
		return NULL;
	}

	info_block->is_exited = 0;
	info_block->exit_status = 0;

	info_block->exit_cv = cv_create("info block cv");

	if (!info_block->exit_cv)
	{
		kfree(info_block->children);
		kfree(info_block);
		return NULL;
	}


	return info_block;
}

static
void 
destroy_pid_block(struct pid_info_block* block)
{
	if (!block)
	{
		panic("Cannot destroy empty pid block");
		return;
	}

	kprintf("Deleted pid %d\n", block->my_pid);

	array_destroy(block->children);
	cv_destroy(block->exit_cv);
	kfree(block);
}





//TODO: add function to update system when process returns

//PUBLIC PROTOTYPES (Only for functions that will go into the struct)
/*
static pid_t get_parent(pid_t pid);
static int get_exit_status(pid_t pid);
static int add_process(pid_t* p_pid, pid_t parent, int is_kernel);
static int end_process(pid_t pid, int exit_status);
static int wait_pid(pid_t caller, pid_t pid);

*/

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

static
int
is_pid_exited(pid_t pid)
{

	return get_pid_block(pid)->is_exited;


}



/*
Function: get_exit_status

Inputs: get_exit_status

Outputs:


Other Notes:
	This function should never crash the system.

*/
static 
int get_exit_status(pid_t pid) {
	assert(pid_manager != NULL);

	struct pid_info_block *info_block = get_pid_block(pid);

	if (info_block == NULL)
		return -1; //TODO: fix with actual status for nonexistent process


	return info_block->exit_status;
}


static 
int 
add_process(pid_t* p_pid, pid_t parent_id, int is_kernel)
{

	

	(void)is_kernel; //will need this later to fix a problem

	*p_pid = 0;

	if (pid_manager->number_of_procs == MAX_PIDS)
	{
		return -1;
	}

	lock_acquire(pid_manager->lock);

	int i = scan_free();


	if (i == -1)
	{
		panic("Someone filled all the PIDs without telling the manager");
	}

	struct pid_info_block* block = create_pid_block(i, parent_id);

	if (!block)
	{
		lock_release(pid_manager->lock);
		return -1;
	}

	*p_pid = i;
	set_pid_block(i, block);
	pid_manager->next_pid = i + 1;
	pid_manager->number_of_procs++;

	struct pid_info_block* parent_block = get_pid_block(parent_id);

	if (parent_block)
	{
		array_add(parent_block->children, (void*)block);

	}

	lock_release(pid_manager->lock);

	kprintf("Process %d was added by %d.\n", *p_pid, parent_id);
	
	return 0;

}


static
int
end_process(pid_t pid, int exit_status)
{
	

	struct pid_info_block* block = get_pid_block(pid);

	if (!block)
		return -1; //TODO: replace with proper return code

	

	lock_acquire(pid_manager->lock);

	cv_broadcast(block->exit_cv, pid_manager->lock);

	
	block->is_exited = 1;
	block->exit_status = exit_status;

	struct pid_info_block* child;
	int i;
	for (i = 0; i < array_getnum(block->children); i++)
	{
		child = array_getguy(block->children, i);
		
		if (should_free_pid(child))
		{
			set_pid_block(child->my_pid, NULL);
			destroy_pid_block(child);
			pid_manager->number_of_procs--;
		}
	}

	if (should_free_pid(block))
	{
		pid_manager->number_of_procs--;
		set_pid_block(pid, NULL);
		destroy_pid_block(block);
	}


	kprintf("Process %d finished with exit code %d.\n", pid, exit_status);
	assert(pid_manager->number_of_procs >= 0);
	lock_release(pid_manager->lock);
	return 0;

}

static 
int 
wait_pid(pid_t caller, pid_t pid)
{

	if (caller == pid)
		return -1; //TODO: set errno

	if (get_parent(pid) != caller)
		return -1; //TODO: set errno

	if (!(validate_pid(caller) && validate_pid(pid)))
		return -1; //TODO: set errno

	lock_acquire(pid_manager->lock);

	struct pid_info_block* block = get_pid_block(pid);

	if (!block->is_exited)
	{
		cv_wait(block->exit_cv, pid_manager->lock);
	}

	

	int block_exit_status = get_pid_block(pid)->exit_status;

	lock_release(pid_manager->lock);

	return block_exit_status;	
}

//PUBLIC FUNCTIONS HERE
//These were declared in pid_manager.h

//Pid_manager functions
void
pid_manager_bootstrap()
{

	pid_manager = kmalloc(sizeof(struct pid_manager));
	assert(pid_manager);

	int i;
	for (i = 0; i < MAX_PIDS; i++)
	{
		pid_manager->pid_info_blocks[i] = NULL;
	}

	pid_manager->next_pid = 1;
	pid_manager->lock = lock_create("pid manager lock");
	assert(pid_manager->lock);

	
	pid_manager->get_parent = get_parent;
	pid_manager->get_exit_status = get_exit_status;
	pid_manager->add_process = add_process;
	pid_manager->end_process = end_process;
	pid_manager->wait_pid = wait_pid;

	return;
}

void
destroy_pid_manager()
{
	struct pid_info_block* block;
	int i;
	for (i = 0; i < MAX_PIDS; i++)
	{
		block = get_pid_block(i);
		if (block != NULL)
			destroy_pid_block(block);
	}


	kfree(pid_manager->lock);
	kfree(pid_manager);
}














