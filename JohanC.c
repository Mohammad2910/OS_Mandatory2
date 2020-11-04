#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

/*************************BIBLIOGRAHPY*************************
 * 
 *							DISCLAIMER:
 * I have not been able to complete all the tests only the 3 first test 
 * and the first subtest in test_alloc_4. 
 * The reason why I am not able to complete this test is becuase
 * my liked list is mirrored to the test standard. My allocation 
 * will, because of my linked list implementation, go from the 
 * higher ptr adresses to the lower. And to complete the test i need to go from
 * lower to higher.
 * 
 * https://www.tutorialspoint.com/c_standard_library/c_function_free.htm
 * 
 */

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

//defines that it is a struct memory list so we don't have to refer it every time as a struct
typedef struct memoryList
{
	// doubly-linked list
	struct memoryList *last;
	struct memoryList *next;

	int size;	// How many bytes in this block?
	char alloc; // 1 if this block is allocated,
				// 0 if this block is free.
	void *ptr;	// location of block in memory pool.

} memoryList;

strategies myStrategy = NotSet; // Current strategy

size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;
static struct memoryList *roving;
static int max_pointer_size;

/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL)
		free(myMemory); /* in case this is not the first time initmem2 is called */

	if (head)
	{
		free(head);
	}
	// Checks if head is null or not. If not then we can just free head beacuse
	// it has to be free the first time always.
	
	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */

	myMemory = malloc(sz);

	/* TODO: Initialize memory management structure. */

	head = malloc(sizeof(memoryList)); // creates the head

	head->next = NULL;	  //Sets the next value to null since this is the first
	head->last = NULL;	  //Sets the before value to null
	head->size = sz;	  //The size of the head is given when first instantiaed
	head->alloc = 0;	  //Sets the allocation to 0 which means that it is free
	head->ptr = myMemory; //Sets a pointer to the allocted block in the memory.

	//But since I am doing the Next fit it should be circular linked list so we could actually set
	//the next and last to be equal to itself to begin with.'
	head->next = head;
	head->last = head;
	roving = head;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);

	switch (myStrategy)
	{
	case NotSet:
		return NULL;
	case First:
		return NULL;
	case Best:
		return NULL;
	case Worst:
		return NULL;
	case Next:
		return next_fit(requested);
	}
	return NULL;
}

void *next_fit(size_t requested)
{
	//makinng a nodeholder for the roving
	memoryList *nodeHolder = roving;
	//creates a new node
	memoryList *newNode = malloc(sizeof(memoryList));

	do
	{

		// if the roving is less we just continue to the next node and move roving
		if (roving->size < requested || roving->alloc == 1)
		{
			//printf("Roving ptr: %p    Roving alloc : %d-- Roving Size: %d --- Requested size: %d---\n",roving->ptr,roving->alloc,roving->size,requested);
			//printf("roving ptr: %p    roving next ptr: %p\n ",roving->ptr, roving->next->ptr);
			//roving = roving->next;
			continue;
		}
		//printf("HELLLLLLOOOOLLFOJAOIJSFOIASJ");

		//if the roving (roving means current node location) is exactly the size of the requested
		//we can just allocate it to that one

		//if (roving->size == requested && roving->ptr != head->ptr)
		if (roving->size == requested)
		{
			roving->alloc = 1;
			//printf("roving pointer in the while loop %p\n", roving->ptr);
			roving = roving->next;
			return roving->last->ptr;
		}
		//First time spawning a new node we need to use the left(head) ptr + size (edgecase)
		if (head->next == head && roving->size > requested)
		{
			//printf("-- HELLO --- HEELo---\n");
			//sets the
			roving->last->next = newNode;

			newNode->size = requested;
			newNode->last = roving->last;
			newNode->next = head;
			newNode->alloc = 1;
			newNode->ptr = roving->ptr;

			roving->ptr = requested + head->ptr;

			//sets the newNode to be the rovings last
			roving->last = newNode;
			//removes the requested amount from the newNode
			roving->size -= requested;
			return newNode->ptr;
		}
		if (roving->ptr == head->ptr)
		{

			//	printf("roving ptr----&&---> %p\n", roving->ptr);

			newNode->next = roving->next;
			newNode->size = requested;
			newNode->last = head;
			newNode->alloc = 1;
			roving->next->last = newNode;
			roving->next = newNode;

			newNode->ptr = roving->ptr;
			roving->ptr = roving->ptr + requested;

			roving->size -= requested;

			return newNode->ptr;
		}


		
		//sets the pointer of node before roving's next to be newNode
		roving->last->next = newNode;

		newNode->size = requested;
		newNode->last = roving->last;
		newNode->next = roving;
		newNode->alloc = 1;

		newNode->ptr = roving->ptr;
		roving->ptr = roving->ptr + requested;
		//sets the newNode to be the rovings last

		roving->last = newNode;
		//printf("roving last--------> %d\n", roving->last);
		//printf("HEELLLLOOOO");
		//removes the requested amount from the newNode
		roving->size -= requested;

		//	printf("newnode pointer --- %p\n", newNode->ptr);
		//printf("roving pointer in while loop %p\n", roving->ptr);

		return newNode->ptr;

	} while ((roving = roving->next) != nodeHolder);
}

/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void *block)
{
	memoryList *nodeHolder = head;

	do
	{
		//Keeps going in the loop until we are on the correct node
		if (nodeHolder->ptr != block)
		{
			continue;
		}
		memoryList *nodeFree = nodeHolder->next;

		//Starts by freeig the node form it's memory ( might not always be nessecary )
		//printf("Last& cyurennt & next ----------------- %p & %p %p \n", nodeHolder->last->ptr, nodeHolder->ptr, nodeHolder->next->ptr);

		nodeHolder->alloc = 0;

		//printf("----------------------------------------\n");
		//checks if the node to the right is empty and is not head
		if (nodeHolder->next->alloc == 0 && nodeHolder->next != head)
		{

			//printf("This happens if the RIGHT node is empty\n");
			//if the node to the right is empty then we need to colaps it down to the nodeholder
			nodeHolder->size += nodeHolder->next->size;

			nodeHolder->ptr = nodeHolder->next->ptr;

			//chaning the pointers in the linked list
			nodeHolder->next->next->last = nodeHolder;
			nodeHolder->next = nodeHolder->next->next;

			//free the node after removed from the linked list
			free(nodeFree);
		}
		//checks if the node to the left is empty
		//printf("Last& cyurennt & next ----------------- %p & %p %p \n", nodeHolder->last->ptr, nodeHolder->ptr, nodeHolder->next->ptr);

		if (nodeHolder->last->alloc == 0)
		{
			//makes a copy to free after the node has been removed.

			nodeFree = nodeHolder;
			//	printf("nodefree struct pointer -> %p\t nodeholder -> %p\n", nodeFree, nodeHolder);
			//	printf("This happens if the LEFT node is empty\n");
			nodeHolder = nodeHolder->last;

			//if the node to the right is empty then we need to colaps it down to the nodeholder
			nodeHolder->size += nodeHolder->next->size;

			nodeHolder->ptr = nodeHolder->next->ptr;

			//chaning the pointers in the linked list
			nodeHolder->next->next->last = nodeHolder;
			nodeHolder->next = nodeHolder->next->next;
			//Free the struct (needs to free before the next is changed)
			free(nodeFree);
		}

		//printf("Before return nodeholder%p\n", nodeHolder->ptr);

		return nodeHolder;

	} while ((nodeHolder = nodeHolder->next) != head);

	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	memoryList *newnodeHolder = head;
	int amount = 0;
	do
	{
		if (head->size == 0 && newnodeHolder == head)
		{
			amount = amount - 1;
		}
		if (newnodeHolder->alloc != 1)
		{
			amount = amount + 1;
		}

		newnodeHolder = newnodeHolder->next;
	} while (newnodeHolder != head);

	return amount;
}

/* Get the number of bytes allocated */
int mem_allocated()
{

	int UsedBytes = 0;			   //Coutner for the amount  used blocks
	memoryList *nodeHolder = head; //initial block that we starts is head and is then used when using next.
								   //using while loop to go through all of the nodes in our double linked list
	while (1)
	{

		//If it doesn't have anything allocated it will return 1 in it's alloc variable
		if (nodeHolder->alloc == 1)
		{
			//addes the amount of bytes that is allocated for the current node but is used.
			UsedBytes = UsedBytes + nodeHolder->size;
		}

		//Because we are running the double linked list in a loop if the next node is head we know that
		//we have returned to the place where we started because it is also where we stareted.
		if (nodeHolder->next == head)
		{
			//Just to break out of the while loop
			break;
		}

		//goes to the next node in the double linked list.
		nodeHolder = nodeHolder->next;
	}
	//returens the Amount of bytes that is in use.
	return UsedBytes;
}

/* Number of non-allocated bytes */
int mem_free()
{

	int NoneUsedBytes = 0;		   //Coutner for the amount none used blocks
	memoryList *nodeHolder = head; //initial block that we starts is head and is then used when using next.
								   //using while loop to go through all of the nodes in our double linked list
	while (1)
	{

		//If it doesn't have anything allocated it will return 0 in it's alloc variable
		if (nodeHolder->alloc == 0)
		{
			//addes the amount of bytes that is allocated for the current node but is not used.
			NoneUsedBytes = NoneUsedBytes + nodeHolder->size;
		}

		//Because we are running the double linked list in a loop if the next node is head we know that
		//we have returned to the place where we started because it is also where we stareted.
		if (nodeHolder->next == head)
		{
			//Just to break out of the while loop
			break;
		}

		//goes to the next node in the double linked list.
		nodeHolder = nodeHolder->next;
	}
	//returens the Amount of bytes that is not in use.
	return NoneUsedBytes;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{

	//Creates the holder for the current node and sets it to head first
	memoryList *nodeHolder = head;
	//Current largest
	int curLargest = 0;

	//as long as we are not at the head again
	do
	{
		//checks if the nodeholder has free memory
		if (nodeHolder->alloc == 0)
		{
			//checks if the nodeholders current size is larger than the currently largest size
			if (nodeHolder->size > curLargest)
			{
				curLargest = nodeHolder->size;
			}
		}
	} while (nodeHolder->next == head);

	//returns the Largest size of free memory in the stack

	return curLargest;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	//Creates the holder for the current node and sets it to head first
	memoryList *nodeHolder = head;
	//Current smallest
	int amount = 0;

	//as long as we are not at the head again
	do
	{
		//checks if the nodeholder has free memory
		if (nodeHolder->alloc == 0)
		{
			//checks if the nodeholders current size is smaller than the given size
			if (nodeHolder->size < size)
			{
				amount++;
			}
		}
		nodeHolder = nodeHolder->next;
	} while (nodeHolder != head);

	//returns the amount of free blocks smaller than size
	return amount;
}

char mem_is_alloc(void *ptr)
{
	memoryList *nodeHolder = head;
	do
	{
		if (nodeHolder->ptr == ptr)
		{
			return nodeHolder->alloc;
		}
		nodeHolder = nodeHolder->next;
	} while (nodeHolder != head);

	return 0;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */

//Returns a pointer to the memory pool.
void *mem_pool()
{
	//printf("head last pointer --------   %p\n", head->last->ptr);
	return head->last->ptr;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}

// Get string name for a strategy.
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
	case Best:
		return "best";
	case Worst:
		return "worst";
	case First:
		return "first";
	case Next:
		return "next";
	default:
		return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char *strategy)
{
	if (!strcmp(strategy, "best"))
	{
		return Best;
	}
	else if (!strcmp(strategy, "worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy, "first"))
	{
		return First;
	}
	else if (!strcmp(strategy, "next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}

/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{

	memoryList *nodeHolder = head;
	do
	{
		printf("----------------\n");
		printf("Alloc %d \n", nodeHolder->alloc);
		printf("Pointer actuall memory block -----> %p\n", nodeHolder->ptr);
		printf("Node pointer to the left %p\n", nodeHolder->last->ptr);
		printf("Node pointer to the right %p\n", nodeHolder->next->ptr);
		printf("Size of node %d\n", nodeHolder->size);
		printf("Roving pointer %p\n", roving->ptr);

	} while ((nodeHolder = nodeHolder->next) != head);
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n", mem_allocated(), mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n", mem_free(), mem_holes(), mem_largest_free());
	printf("Average hole size is %f.\n\n", ((float)mem_free()) / mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv)
{
	strategies strat;
	void *a, *b, *c, *d, *e;
	if (argc > 1)
		strat = strategyFromString(argv[1]);
	else
		strat = Next;

	/* A simple example.  
	   Each algorithm should produce a different layout. */

	void *lastPointer = NULL;
	initmem(strat, 10);
	int i;

	for (i = 0; i < 10; i++)
	{
		void *pointer = mymalloc(1);
		if (i > 0 && pointer != (lastPointer + 1))
		{
		}
		printf("MY FREEE ------%p && %p \n", pointer, lastPointer);
		lastPointer = pointer;
	}

	for (i = 1; i < 10; i += 2)
	{

		printf("MY FREEE ------%x\n", mem_pool() + i);
		myfree(mem_pool() + i);
	}

	for (i = 1; i < 10; i += 2)
	{
		void *pointer = mymalloc(1);
		if (i > 1 && pointer != (lastPointer + 2))
		{
			printf("Allocate ------%p && %p \n", pointer, lastPointer);
		}
		lastPointer = pointer;
	}

	// a = mymalloc(1);
	// b = mymalloc(2);
	// c = mymalloc(1);

	// myfree(b);

	// d = mymalloc(2);

	//d = mymalloc(1);

	//myfree(a);
	// d = mymalloc(50);
	// myfree(a);
	// e = mymalloc(25);

	print_memory();
	//print_memory_status();
}
