/*
 * catlock.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use LOCKS/CV'S to solve the cat syncronization problem in 
 * this file.
 */


/*
 * 
 * Includes
 *
 */

#include <types.h>
#include <lib.h>
#include <test.h>
#include <thread.h>
#include <synch.h>


/*
 * 
 * Constants
 *
 */

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

#if 0 /* set 1 for stress test */

#define NCATS 25
#define NMICE 17

#else

#define NCATS 6
#define NMICE 2

#endif

/*
 * Other defines related to problem
 */

#define MAX_WAIT 2

#define TURN_COUNT 2

#define CAT_CODE 1
#define MOUSE_CODE 2

#define TRUE 1
#define FALSE 0


/*
 * Global Variables
 */

int *dishes;
int volatile cats_done;
int volatile mice_done;

int volatile cats_admitted_this_turn;
int volatile mice_admitted_this_turn;

int volatile cats_left_this_turn;
int volatile mice_left_this_turn;

int volatile cats_waiting;
int volatile mice_waiting;




int whos_turn;


struct lock *lock;

struct cv *empty_dish;

struct cv *mouse_turn;
struct cv *cat_turn;

struct cv *all_done;







/****************************************************************************/
/*			HELPER FUNCTIONS				    */
/****************************************************************************/

/*
 * clear_turn_counters()
 *
 * Arguments:
 *      null
 *
 * Returns:
 *      null
 *
 * Notes:
 * 	Sets all variables associated with a given turn to 0
 *
 */
static
void
clear_turn_counters()
{

	cats_admitted_this_turn = 0;
	mice_admitted_this_turn = 0;

	cats_left_this_turn = 0;
	mice_left_this_turn = 0;


}

/*
 * catmouse_global_init()
 *
 * Arguments:
 *      null
 *
 * Returns:
 *      null
 *
 * Notes:
 * 	Allocates memory for global pointers and initializes other global variables
 *
 */
static
void
catmouse_global_init()
{
	lock = lock_create("lock");
	
	mouse_turn = cv_create("Mouse Turn Signal");
	cat_turn = cv_create("Cat turn signal");
	all_done = cv_create("All done");
	empty_dish = cv_create("Has an empty dish");


	
	dishes = NULL;
	dishes = (int*)kmalloc(sizeof(int) * NFOODBOWLS);
	assert(dishes);

	int i;
	for (i = 0; i < NFOODBOWLS; i++)
	{
		dishes[i] = 0;
	}

	cats_done = 0;
	mice_done = 0;
	whos_turn = 0;

	clear_turn_counters();

	cats_waiting = 0;
	mice_waiting = 0;

}




/*
 * catmouse_global_free()
 *
 * Arguments:
 *      null
 *
 * Returns:
 *      null
 *
 * Notes:
 * 	frees all memory allocated during assignment
 *
 */
static
void
catmouse_global_free()
{
	lock_destroy(lock);


	cv_destroy(all_done);
	cv_destroy(cat_turn);
	cv_destroy(mouse_turn);
	cv_destroy(empty_dish);

	kfree(dishes);


}



/*
 * check_finished()
 *
 * Arguments:
 *      int my_type: integer which tells if calling animal is cat or mouse
 *
 * Returns:
 *      null
 *
 * Notes:
 * 	increments the counters for finished animals and checks if everyone has
 * 	finished eating.
 *
 */
static
void
check_finished(int my_type)
{

	assert(lock_do_i_hold(lock));


	if (my_type == CAT_CODE)
		cats_done++;
	else if (my_type == MOUSE_CODE)
		mice_done++;
	else
		panic("Invalid animal code");



	if (cats_done == NCATS && mice_done == NMICE)
		cv_signal(all_done, lock);


}

/*
 * get_empty_dish()
 *
 * Arguments:
 *      none
 *
 * Returns:
 *      integer contaning index of first unused/empty dish
 *
 * Notes:
 * 	returns -1 if no empty dish is found
 *
 */
static
int
get_empty_dish()
{

	assert(lock_do_i_hold(lock));


	//check that there is an empty dish
	int i = 0;
	while ((i < NFOODBOWLS) && (dishes[i] != 0))
	{
		//kprintf("Are we in loop\n");
		i++;
		//TODO: Check if wrong animal type is at dish
	}

	if (i == NFOODBOWLS)
		return -1;

	return i;

}

/*
 * cat_turn_logic()
 *
 * Arguments:
 *      none
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 * 	This function determines whether a cat will be admitted to 
 *	the eating area.
 *
 */
static
void
cat_turn_logic()
{
	//Enter the wait queue
	cats_waiting++;

	
	while (whos_turn != CAT_CODE || cats_admitted_this_turn >= TURN_COUNT)
	{
		cv_wait(cat_turn, lock);
	}
	
	//The thread should be holding the lock at this point so
	//incrementing cats_admitted outside the while loop should not
	//be a problem. I hope.

	cats_waiting--;
	cats_admitted_this_turn++;
	assert(cats_admitted_this_turn <= TURN_COUNT);
	assert(cats_waiting >= 0);
}

/*
 * mouse_turn_logic()
 *
 * Arguments:
 *      none
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 * 	This function determines whether a mouse will be admitted to 
 *	the eating area.
 *
 */
static
void
mouse_turn_logic()
{
	//Enter the wait queue
	mice_waiting++;

	
	while (whos_turn != MOUSE_CODE || mice_admitted_this_turn >= TURN_COUNT)
	{
		cv_wait(mouse_turn, lock);
	}

	mice_waiting--;
	mice_admitted_this_turn++;
	assert(mice_admitted_this_turn <= TURN_COUNT);
	assert(mice_waiting >= 0);

}

/*
 * cat_end_turn_logic()
 *
 * Arguments:
 *      none
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 * 	This function determines whether a turn switch should occur
 *	after a cat eats
 *
 */
static
void
cat_end_turn_logic()
{
	cats_left_this_turn++;

	if (cats_left_this_turn < cats_admitted_this_turn)
		return;


	if (cats_left_this_turn < TURN_COUNT && cats_waiting > 0)
	{
		return;
	}



	if (mice_waiting != 0)
	{
		whos_turn = MOUSE_CODE;
		cv_broadcast(mouse_turn, lock);
	}
	else if (cats_waiting != 0)
	{
		cv_broadcast(cat_turn, lock);
	}
	else
	{
		whos_turn = 0;
	}
		



	clear_turn_counters();

}


/*
 * mouse_end_turn_logic()
 *
 * Arguments:
 *      none
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 * 	This function determines whether a turn switch should occur
 *	after a mouse eats
 *
 */
static
void
mouse_end_turn_logic()
{
	mice_left_this_turn++;

	if (mice_left_this_turn < mice_admitted_this_turn)
		return;

	if (mice_left_this_turn < TURN_COUNT && mice_waiting > 0)
	{
		return;
	}
		




	if (cats_waiting != 0)
	{
		whos_turn = CAT_CODE;
		cv_broadcast(cat_turn, lock);
	}
	else if (mice_waiting != 0)
	{
		cv_broadcast(mouse_turn, lock);
	}
	else
	{
		whos_turn = 0;
	}

	clear_turn_counters();

}


/*
 * eat()
 *
 * Arguments:
 *      int type: Determines animal type
 *	int number: Holds animal's number
 *	int my_dish: Holds animal's dish number
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      prints output to screen
 *
 */
static
void
eat(int type, int number, int my_dish)
{

	if (type == CAT_CODE)
	{
		kprintf("Cat %d is eating at dish %d\n", number + 1, my_dish + 1);
		clocksleep(random() % MAX_WAIT);
		kprintf("Cat %d is leaving the dish %d\n", number + 1, my_dish + 1);
	}
	else
	{
		kprintf("Mouse %d is eating at dish %d\n", number + 1, my_dish + 1);
		clocksleep(random() % MAX_WAIT);
		kprintf("Mouse %d is leaving the dish %d\n", number + 1, my_dish + 1);

	}

}

/*
 * petition_guardian()
 *
 * Arguments:
 *      int my_type: determines if caller is a cat or a mouse.
 *
 * Returns:
 *      an integer holding the animals assigned dish
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */
static
int
petition_guardian(int my_type)
{
	lock_acquire(lock);

	//Getting bowl number


	//Claim empty turn
	if (whos_turn == 0)
		whos_turn = my_type;


	//Limit number of animals that may enter without turn switch
	assert(my_type == CAT_CODE || my_type == MOUSE_CODE);


	if (my_type == CAT_CODE)
		cat_turn_logic();
	else
		mouse_turn_logic();


	int my_bowl = get_empty_dish();
	//Assign available bowl to petitioning animal
	//We must guarantee that turn will not switch with animals
	//in this loop
	while (my_bowl == -1)
	{
		cv_wait(empty_dish, lock);
		my_bowl = get_empty_dish();
	}
	
	dishes[my_bowl] = my_type;



	lock_release(lock);

	return my_bowl;
}

/*
 * return_dish_to_guardian()
 *
 * Arguments:
 *      int type: determines if the caller is a cat or a mouse
 *      int my_dish: the animals dish that is being returned
 *      
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      The function releases the dish resource and
 *	calls the functions that determine a turn switch
 *
 */
static
void
return_dish_to_guardian(int type, int my_dish)
{
	lock_acquire(lock);

	dishes[my_dish] = 0;


	assert(type == CAT_CODE || type == MOUSE_CODE);

	if (type == CAT_CODE)
		cat_end_turn_logic();
	else
		mouse_end_turn_logic();


	cv_signal(empty_dish, lock);

	check_finished(type);

	lock_release(lock);

}


/****************************************************************************/
/*			PRIMARY FUNCTIONS				    */
/****************************************************************************/


/*
 * catlock()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS -
 *      1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
catlock(void * unusedpointer, 
        unsigned long catnumber)
{

	int my_dish = petition_guardian(CAT_CODE);
	eat(CAT_CODE, (int)catnumber, my_dish);
	return_dish_to_guardian(CAT_CODE, my_dish);

	(void)unusedpointer;
	
}
	

/*
 * mouselock()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long mousenumber: holds the mouse identifier from 0 to 
 *              NMICE - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using locks/cv's.
 *
 */

static
void
mouselock(void * unusedpointer,
          unsigned long mousenumber)
{

	int my_dish = petition_guardian(MOUSE_CODE);
	eat(MOUSE_CODE, (int)mousenumber, my_dish);
	return_dish_to_guardian(MOUSE_CODE, my_dish);
	
	(void)unusedpointer;

}


/*
 * catmouselock()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catlock() and mouselock() threads.  Change
 *      this code as necessary for your solution.
 */

int
catmouselock(int nargs,
             char ** args)
{

	catmouse_global_init();

        int index, error;
   
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;
   
        /*
         * Start NCATS catlock() threads.
         */

        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catlock thread", 
                                    NULL, 
                                    index, 
                                    catlock, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catlock: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

        /*
         * Start NMICE mouselock() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mouselock thread", 
                                    NULL, 
                                    index, 
                                    mouselock, 
                                    NULL
                                    );
      
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mouselock: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }



	lock_acquire(lock);
	cv_wait(all_done, lock);


	catmouse_global_free();

        return 0;
}

/*
 * End of catlock.c
 */
