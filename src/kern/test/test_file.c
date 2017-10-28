
#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <array.h>
#include <machine/spl.h>
#include <machine/pcb.h>
#include <thread.h>
#include <curthread.h>
#include <scheduler.h>
#include <addrspace.h>
#include <vnode.h>
#include <test.h>


//private functions
/*
static
void
_flush_cache()
{
	return;
}
*/
#if 0
int grab(void* this_void)
{
	struct foo* this = (struct foo*)this_void;
	return this->x;
}


void
set_pointer(struct foo* holder)
{
	void* ptr = holder;
	
	int nested_grab() { return grab(holder); };

	holder->getx = nested_grab;

}
#endif

struct foo* foo;
int foo_get()
{
	return foo->x;
}



//public function
int
struct_test(int nargs, char** stuff)
{

	foo = kmalloc(sizeof(struct foo));

	foo->x = 4;
	foo->getx = foo_get;

	kprintf("Foo has a value of %d\n", foo->getx());


	return 0;

}
