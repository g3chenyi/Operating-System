#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int clock_hand;
/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	while(coremap[clock_hand].bit == 1){
			//recently accessed, update bit and go to next 
			coremap[clock_hand].bit = 0;
			clock_hand = (clock_hand+1)%memsize; 
		}
	
	//return 0;
	return clock_hand;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {

	int i;
	for(i=0; i<memsize; i++){
		if(coremap[i].pte == p){
			coremap[i].bit = 1;
			break;	
	}	
	}
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	// pick a randon position to start
	clock_hand = (int)(random() % memsize);
}
