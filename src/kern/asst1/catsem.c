/*
 * catsem.c
 *
 * 30-1-2003 : GWA : Stub functions created for CS161 Asst1.
 *
 * NB: Please use SEMAPHORES to solve the cat syncronization problem in 
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


/*
 * 
 * Constants
 *
 */

/*
 * Number of food bowls.
 */

#define NFOODBOWLS 2

/*
 * Number of cats.
 */

#define NCATS 6

/*
 * Number of mice.
 */

#define NMICE 2



/*
 * Globals
 */
int turn;
int dishes_in_use;
int cats_done;
int mice_done;
int cats_gone;
int mice_gone;
struct semaphore *vars_mutex;
struct semaphore *turn_mutex;
struct semaphore *all_done;
struct semaphore *cat_sem;
struct semaphore *mouse_sem;

/*
 * Global flags
 */
#define DISH1 1
#define DISH2 2
#define CAT_CODE 1
#define MOUSE_CODE 2


/*
 * 
 * Function Definitions
 * 
 */

/****************************************************************************/
/*			HELPER FUNCTIONS				    */
/****************************************************************************/


/*
 * catmouse_global_init()
 *
 * Arguments:
 * 	none
 *
 * Returns:
 *	
 *
 *
 * Notes:
 * 
 * 
 */
static
void
catmouse_global_init()
{

	//Initialize globals
	turn = 0;
	dishes_in_use = 0;
	mice_done = 0;
	cats_done = 0;
	cats_gone = 0;
	mice_gone = 0;
	vars_mutex = sem_create("help_variable_lock", 1);
	turn_mutex = sem_create("turn_mutex", 1);
	cat_sem = sem_create("cat_sem", 2);
	mouse_sem = sem_create("mouse_sem", 2);
	all_done = sem_create("Finished", 0);


}

/*
 * catmouse_global_free()
 *
 * Arguments:
 * 	none
 *
 * Returns:
 *	integer with assigned dish number 
 *
 *
 * Notes:
 * 
 */
static
void
catmouse_global_free()
{


	sem_destroy(vars_mutex);
	sem_destroy(turn_mutex);
	sem_destroy(cat_sem);
	sem_destroy(mouse_sem);
	sem_destroy(all_done);

	vars_mutex = NULL;
	turn_mutex = NULL;
	cat_sem = NULL;
	mouse_sem = NULL;
	all_done = NULL;
}



/*
 * release_dish()
 *
 * Arguments:
 * 	integer containing parent dish number
 *	integer containing whether parent is cat or mouse
 *
 * Returns:
 *	nothing
 *
 *
 * Notes:
 * will wait for var_mutex
 * uses global variables
 * will release the given dish
 */

static 
void 
release_dish(int my_dish, int cat_or_mouse)
{	
	
	//Begin critical section to access shared helper variables
	P(vars_mutex);
	
	//unset my_dish bit.
	dishes_in_use &= ~my_dish;

	/* This section is exported to check_finished to address a potential
	 * synchronization issue
	if (cat_or_mouse == CAT_CODE)
		cats_done++;
	else if (cat_or_mouse == MOUSE_CODE)
		mice_done++;
	else
		panic("Invalid animal code");
	*/

	


	V(vars_mutex);
	//End critical section to access shared helper variables


}



/*
 * claim_dish()
 *
 * Arguments:
 * 	none
 *
 * Returns:
 *	integer with assigned dish number 
 *
 *
 * Notes:
 * waits on vars_mutex
 * uses global variables
 */

static 
int 
claim_dish(int animal_code)
{
	int my_dish;

	P(vars_mutex);

	assert(dishes_in_use != (DISH1 | DISH2));

	

	if (!(dishes_in_use & DISH1))
		{
			if(animal_code == CAT_CODE) {
				cats_gone++;
			}
			if(animal_code == MOUSE_CODE) {
				mice_gone++;
			}
			dishes_in_use |= DISH1;
			my_dish = DISH1;
		}
	else
		{
			if(animal_code == CAT_CODE) {
				cats_gone++;
			}
			if(animal_code == MOUSE_CODE) {
				mice_gone++;
			}
			dishes_in_use |= DISH2;
			my_dish = DISH2; 
		}

	if (animal_code == CAT_CODE && cats_gone % 3 == 0 | cats_gone == NCATS) {
		turn = 2;
	}
	else if (animal_code == MOUSE_CODE && mice_gone % 3 == 0| mice_gone == NMICE) {
		turn = 1;
	}

	V(vars_mutex);

	//kprintf("%d cats gone.\n", cats_gone);
	//kprintf("%d mice gone.\n", mice_gone);

	return my_dish;
}





/*
 * is_my_turn()
 *
 * Arguments:
 * 	integer containing parent animal code
 *
 * Returns:
 *	true if turn matches parent animal code or false
 *
 *
 * Notes:
 * will aquire shared variable mutex
 */

static 
int 
is_my_turn(int animal_code)
{
	P(vars_mutex);
	int is_my_turn = (animal_code == turn);
	V(vars_mutex);
	
	return is_my_turn;
}


/*
 * wait_turn()
 *
 * Arguments:
 * 	integer containing parent animal code
 *
 * Returns:
 *	nothing
 *
 *
 * Notes:
 * will aquire turn when available
 */

static 
void 
wait_turn(int animal_code)
{	
	P(turn_mutex);	

	P(vars_mutex);

	assert(turn == 0);
	turn = animal_code;

	V(vars_mutex);
	
}

/*
 * try_release_turn()
 *
 * Arguments:
 * 	none
 *
 * Returns:
 *	nothing
 *
 *
 * Notes:
 * will release turn if no one else is eating
 */

static 
void 
try_release_turn()
{	

	P(vars_mutex);

	if (dishes_in_use == 0)
	{
		turn = 0;
		V(turn_mutex);
	}

	V(vars_mutex);
	
}

/*
 * check_finished()
 *
 * Arguments:
 * 	none
 *
 * Returns:
 *	nothing
 *
 *
 * Notes:
 * signals all done if everyone has eaten. Must be called after all
 * other code components which use the semaphores.
 */
static
void
check_finished(int cat_or_mouse)
{


	int is_finished = 0;

	P(vars_mutex);



	if (cat_or_mouse == CAT_CODE)
		cats_done++;
	else if (cat_or_mouse == MOUSE_CODE)
		mice_done++;
	else
		panic("Invalid animal code");



	if (cats_done == NCATS && mice_done == NMICE)
		is_finished = 1;

	V(vars_mutex);

	
	if (is_finished == 1)
		V(all_done);

}



/****************************************************************************/
/*			PRIMARY FUNCTIONS				    */
/****************************************************************************/

/*
 * catsem()
 *
 * Arguments:
 *      void * unusedpointer: currently unused.
 *      unsigned long catnumber: holds the cat identifier from 0 to NCATS - 1.
 *
 * Returns:
 *      nothing.
 *
 * Notes:
 *      Write and comment this function using semaphores.
 *
 */

static
void
catsem(void * unusedpointer, 
       unsigned long catnumber)
{


	int my_dish = 0;

	P(cat_sem);

	while (!is_my_turn(CAT_CODE))
	{
		wait_turn(CAT_CODE);
	}

	
	my_dish = claim_dish(CAT_CODE);	

	kprintf("Cat %d is eating at dish %d\n", catnumber, my_dish);
	clocksleep(1);
	kprintf("Cat %d is leaving the dish %d\n", catnumber, my_dish);

	release_dish(my_dish, CAT_CODE);

	try_release_turn();

	V(cat_sem);


	check_finished(CAT_CODE);

        (void) unusedpointer;

}
        

/*
 * mousesem()
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
 *      Write and comment this function using semaphores.
 *
 */

static
void
mousesem(void * unusedpointer, 
         unsigned long mousenumber)
{

	int my_dish = 0;


	P(mouse_sem);


	while (!is_my_turn(MOUSE_CODE))
	{
		wait_turn(MOUSE_CODE);
	}
	

	my_dish = claim_dish(MOUSE_CODE);	

	kprintf("Mouse %d is eating at dish %d\n", mousenumber, my_dish);
	clocksleep(1);
	kprintf("Mouse %d is leaving the dish %d\n", mousenumber, my_dish);

	release_dish(my_dish, MOUSE_CODE);

	try_release_turn();


	V(mouse_sem);
	

	check_finished(MOUSE_CODE);

	(void)unusedpointer;


}


/*
 * catmousesem()
 *
 * Arguments:
 *      int nargs: unused.
 *      char ** args: unused.
 *
 * Returns:
 *      0 on success.
 *
 * Notes:
 *      Driver code to start up catsem() and mousesem() threads.  Change this 
 *      code as necessary for your solution.
 */

int
catmousesem(int nargs,
            char ** args)
{
        int index, error;


	catmouse_global_init();
   
        /*
         * Avoid unused variable warnings.
         */

        (void) nargs;
        (void) args;
   
        /*
         * Start NCATS catsem() threads.
         */

        for (index = 0; index < NCATS; index++) {
           
                error = thread_fork("catsem Thread", 
                                    NULL, 
                                    index, 
                                    catsem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
                 
                        panic("catsem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }
        
        /*
         * Start NMICE mousesem() threads.
         */

        for (index = 0; index < NMICE; index++) {
   
                error = thread_fork("mousesem Thread", 
                                    NULL, 
                                    index, 
                                    mousesem, 
                                    NULL
                                    );
                
                /*
                 * panic() on error.
                 */

                if (error) {
         
                        panic("mousesem: thread_fork failed: %s\n", 
                              strerror(error)
                              );
                }
        }

	P(all_done);


	catmouse_global_free();

        return 0;
}


/*
 * End of catsem.c
 */
