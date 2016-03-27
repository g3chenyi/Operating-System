#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

unsigned long current;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	
	//assume first entry(idx 0) is least used
	unsigned long least_used = coremap[0].access_count;
	int evict;
	int i;

	//loop over to find the actual least used 
	for(i=0; i < memsize; i ++){
		if (coremap[i].access_count < least_used || coremap[i].access_count == least_used){
			least_used = coremap[i].access_count; 
			evict = i;
		}
	}

	return evict;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	current ++;
	int i;
	for (i=0; i<memsize; i++){
		if (coremap[i].pte == p){
			coremap[i].access_count = current;
			break;	
		}
	}
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	current = 0;
}
