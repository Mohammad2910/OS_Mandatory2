#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
	// doubly-linked list
	struct memoryList *last;
	struct memoryList *next;

	int size;	// How many bytes in this block?
	char alloc; // 1 if this block is allocated,
				// 0 if this block is free.
	void *ptr;	// location of block in memory pool.
};

strategies myStrategy = Worst; // Current strategy

size_t mySize;
void *myMemory = NULL;

static struct memoryList *head; 
static struct memoryList *next; // gemmer værdien for next fit, hvor memoryList sidst slap


void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL)
		free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
	if (head != NULL)
	{
		static struct memoryList *crntBlock; //Creating pointer to the head of the doubly linked list							 
		for (crntBlock = head; crntBlock != NULL; crntBlock = crntBlock->next)  //
		{
			free(crntBlock->last); 
		}
		free(crntBlock);
	}

	myMemory = malloc(sz);

	/* TODO: Initialize memory management structure. */
	// Pointer to the allocated size of memoryList initialized to head
	head = (struct memoryList *)malloc(sizeof(struct memoryList));
	head->last = NULL;
	head->next = NULL;
	head->size = sz;
	head->alloc = 0;
	head->ptr = myMemory;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

// Find the memory block that is furthest from the correct size.
void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);

	size_t leftoverSize = 0;
	
	struct memoryList *crntWorstFit = NULL; //pointer for the worstfit memory block
	
	struct memoryList *crntBlock = head; //pointer to memory block that i can compare in the search of the worst fit

	while (crntBlock != NULL) //while loop that goes through doubly linked list
	{
		// checks whether memory blocks in the list are allocated or not & whether the size of the block is bigger or equals to the one requested.
		if (crntBlock->size >= requested && crntBlock->alloc == 0)
		{
			// allocates worstfit if it hasnt been done already
			if (crntWorstFit == NULL) 
				crntWorstFit = crntBlock;
			
			// check if size is bigger than the current worstfits size. If so; allocate the bigger memory block to it.
			if (crntBlock->size > crntWorstFit->size)
				crntWorstFit = crntBlock;
		}	
		crntBlock = crntBlock->next;
	}
	// Error handling, return if the worstfit is not allocated.
	if (crntWorstFit == NULL)
	{
		return;
	}

	// if the worstfit block size is equal to the requested block size; return the block
	if (crntWorstFit->size == requested)
	{
		crntWorstFit->alloc = 1;
		return crntWorstFit->ptr;
	}
	else
	{
		
		// new memory block with the size of the difference between worstfit and requested.
		leftoverSize = crntWorstFit->size - requested;

		static struct memoryList *newBlock;
		newBlock = (struct memoryList *)malloc(sizeof(struct memoryList));
		newBlock->next = crntWorstFit->next;
		newBlock->last = crntWorstFit;
		newBlock->size = leftoverSize;
		newBlock->alloc = 0;
		newBlock->ptr = (char *)crntWorstFit->ptr + requested;

		if (crntWorstFit->next != NULL)
		{
			// If the new memory block is between two blocks: Worstfits next block's last is set to be the new block.
			crntWorstFit->next->last = newBlock;
		}
		
			
		crntWorstFit->size = requested;
		crntWorstFit->alloc = 1;
		crntWorstFit->next = newBlock;


		return crntWorstFit->ptr;
	}
}

/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void *block)
{
	struct memoryList *freeBlock = NULL;
	struct memoryList *crntBlock = head; // Usage: traverse the list, start at head.

	// Error handling
	if(crntBlock == NULL){
		printf("Head is null! \n");
		return;
	}

	// Loop wherein we search the block to be freed.
		while (crntBlock != NULL)
		{
		// If the current block matches the seeked block: sets the block to free to current block.
			if (crntBlock->ptr == block)
			{
				freeBlock = crntBlock;
				break;
			}
			crntBlock = crntBlock->next;
		}
		
		// Error handling
		if (freeBlock == NULL)
		{
			printf("The block was not found \n");
			return;
		}
		

		freeBlock->alloc = 0;

		/* Combining blocks: need to check the freeBlock's neighbours for allocation or not */

		// Checking left neighbours and merging the two blocks if not allocated
		if(freeBlock->last != NULL && freeBlock->last->alloc == 0){

			// Adds the size of the freeBlock to the left neighbours size.
			freeBlock->last->size = freeBlock->last->size + freeBlock->size;
			
			// Updates the pointer of the last block's next to point to the freeBlock's next.
			freeBlock->last->next = freeBlock->next;

			// If the freeBlock is the end: update the right neighbour's next to be freeBlock's last.
			if(freeBlock->next != NULL){
				freeBlock->next->last = freeBlock->last;
			}

			crntBlock = freeBlock->last;
			free(freeBlock);
		}

		// Checking right neighbours and merging the two blocks if not allocated
		if(crntBlock->next != NULL && crntBlock->next->alloc == 0)
		{
			// Adds the size of the crntBlock to the right neighbour's size.  **Remember  the previous line of code: "crntBlock = freeBlock->last;"
			crntBlock->next->size = crntBlock->next->size + crntBlock->size;

			// Updates the pointer of the next block's last to point to the crntBlock's last.
			crntBlock->next->last = crntBlock->last;

			// If the block seeked block is at the head: update the left neighbour to be freeBlock's next.
			// Else set the head to be the seeked block's next.
			if (crntBlock->last != NULL)
			{
				crntBlock->last->next = crntBlock->next;
			} else
			{
				head = crntBlock->next;
			}
			free(crntBlock);
		}
		
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
	int num_mem_holes = 0;
	struct memoryList *crntBlock = head;

	while (crntBlock != NULL)
	{
		if (crntBlock->alloc == 0)
			num_mem_holes++;

		crntBlock = crntBlock->next;
			
	}
	return num_mem_holes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	int num_mem_alloc = 0;
	struct memoryList *crntBlock = head;

	while (crntBlock != NULL)
	{
		if (crntBlock->alloc == 1)
		{
			num_mem_alloc += crntBlock->size;
		}
		crntBlock = crntBlock->next;
	}

	return num_mem_alloc;
}

/* Number of non-allocated bytes */
int mem_free()
{
	return mySize - mem_allocated();
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	int num_mem_largest_free = 0;
	struct memoryList *crntBlock = head;

	while (crntBlock != NULL)
	{
		if (crntBlock->alloc == 0 && crntBlock->size > num_mem_largest_free)
				num_mem_largest_free = crntBlock->size;	
		
		crntBlock = crntBlock->next;
	}

	return num_mem_largest_free;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	int num_mem_small_free = 0;
	struct memoryList *crntBlock = head;

	while (crntBlock != NULL)
	{
		if (crntBlock->alloc == 0 && crntBlock->size <= size)
			num_mem_small_free++;

		crntBlock = crntBlock->next;
	}
	return num_mem_small_free;
}

char mem_is_alloc(void *ptr)
{
	char isAllocated = 0;
	struct memoryList *crntBlock = head;

	while (crntBlock != NULL)
	{
		if (crntBlock->alloc == 1 && crntBlock->ptr == ptr)
		{
			isAllocated = 1;
		}
		crntBlock = crntBlock->next;
	}
	return isAllocated;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */

//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
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
	struct memoryList *crntMem = head;
	while (crntMem != NULL)
	{
		printf("Size: %d, Alloc: %d\n", crntMem->size, crntMem->alloc);
		crntMem = crntMem->next;
	}

	return;
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
		strat = Worst;

	/* A simple example.  
	   Each algorithm should produce a different layout. */

	initmem(strat, 500);

	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	// myfree(b);
	d = mymalloc(50);
	// myfree(a);
	e = mymalloc(25);

	print_memory();
	print_memory_status();
}
