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

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;


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

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
	if (head != NULL) free(head);

	myMemory = malloc(sz);
	
	/* TODO: Initialize memory management structure. */
	head = (struct memoryList*) malloc(sizeof (struct memoryList));
	head -> last = NULL; 
	head -> next = NULL;
	head -> size = mySize;
	head -> alloc = 0; 
	head -> ptr = myMemory;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	static struct memoryList *holder;
  	static struct memoryList *node;
	assert((int)myStrategy > 0);
	
	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First:

				node = head;
				// Finding block to allocate memory
				while (node != NULL) {
					if ((node->size >= requested) && (node->alloc == 0)){
						holder = node;
						break;
					} else {
						node = head->next;
					}
				}
				
				// Allocating memory to the block with use of a temporary node/block
				// Took inspiration from Jakob's solution shown in Lecture 6.
				struct memoryList *temp = malloc (sizeof(struct memoryList));
				temp->size = requested;
				temp->alloc = 1;
				temp->last = holder->last;
				if( holder->last != NULL) {
					holder->last->next = temp;
				}
				temp->next = holder;
				temp->ptr = holder->ptr;
				holder->last = temp;
				holder->size = holder->size - requested;
				holder->ptr = holder->ptr + requested;
				if (holder->size == 0) {
					free(holder);
					temp->next = NULL;
				}
				if (temp->last == NULL) {
					head = temp;
				}
				printf("%p\n\n", temp->ptr);
				return temp->ptr;
	  case Best:
	            return NULL;
	  case Worst:
	            return NULL;
	  case Next:
	            return NULL;
	  }
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block) {
	struct memoryList* NodeToFree = NULL;
	static struct memoryList *node;
	node = head;

	while (node != NULL) {
		printf("%p\n", node->ptr);
		printf("%p\n\n", block);
		if(node->ptr == block) {
		NodeToFree = node;
		NodeToFree->alloc = 0;
			if ((NodeToFree->last != NULL) && (NodeToFree->last->alloc == 0)) {
				
				NodeToFree->last->size += NodeToFree->size;
				NodeToFree->last->next = NodeToFree->next;
				if ( NodeToFree->next != NULL) {
					NodeToFree->next->last = NodeToFree->last;
				}
				free(NodeToFree);

			} else if ((NodeToFree->next != NULL) && (NodeToFree->next->alloc == 0)) {
				
				NodeToFree->next->size += NodeToFree->size;
				NodeToFree->next->last = NodeToFree->last;
				head = NodeToFree->next;
				free(NodeToFree);
			}
			break;
		} else if (node->next != NULL) {
			node = node->next;
		} else {
			break;
		}

	}

	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes() {
	static struct memoryList *node;
	int areas = 0;
	node = head;

	while(node != NULL) {
		if(node->alloc == 0) {
			areas++;
		}
		node = node->next;
	}
	
	return areas;
}

/* Get the number of bytes allocated */
int mem_allocated(){
	static struct memoryList *node;
	int AllocBytes = 0;
	node = head;

	while(node){
		if(node->alloc == 1) {
			AllocBytes += node->size;
		}
		node = node->next;
	} 
	
	return AllocBytes;
}

/* Number of non-allocated bytes */
int mem_free() {
	static struct memoryList *node;
	/* NOTE: I could also use "return mySize - mem_allocated();" instead of the following code*/
	int non_alloc = 0;
	node = head;

	while (node){
		if (node->alloc == 0) {
			non_alloc += node->size;
		}
		node = node->next;
	}
	return non_alloc;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free() {
	static struct memoryList *node;
	static struct memoryList *holder;
	int freebytes = 0;
	node = head;

	while (node){
		if(node->alloc == 0){
			if(holder == NULL) {
				holder = node;
				holder->size = node->size;
			} else if (holder->size < node->size) {
				holder->size = node->size; 
			}
			node = node->next;
		} else {
			node = node->next;
		}
	}

	freebytes = holder->size;
	free(holder);
	
	return freebytes;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size) {
	static struct memoryList *node;
	int blocks = 0;
	node = head;

	while (node) {
		if(node->alloc == 0) {
			if (node->size < size) {
				blocks++;
			}
			node = node->next;
		} else {
			node = node->next;
		}
	}
	
	return blocks;
}       

char mem_is_alloc(void *ptr){
	static struct memoryList *node;
    node = head;

	while (node){
		if (node == ptr) {
			if (node->alloc == 0) {
				return 0;
			} else {
				return 1;
			}
		} else {
			node = node->next;
		}
	}

	return 0;
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
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
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
	struct memoryList* currMemory = head;
	printf("Head: %p\n", head);
	while(currMemory != NULL){
        printf("Size: %d, Alloc: %d\n", currMemory->size, currMemory->alloc);
        currMemory = currMemory->next;    
    }
	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    //printf("a");
	strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(200);
	printf("A: %p\n", a);
    b = mymalloc(50);
	printf("B: %p\n", b);
	// c = mymalloc(100);
 	myfree(b);
	//d = mymalloc(50);
	// myfree(a);
	// e = mymalloc(25);
	
	print_memory();
	// print_memory_status();
	
}
