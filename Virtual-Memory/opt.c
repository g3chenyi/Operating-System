#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"

#define MAXLINE 256

extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char *tracefile;

addr_t *trace_vaddr;

int trace_len;
int current;


/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
     
    int i;
    int evict = 0;
    int furtherest = 0;

    //find out the furthest one
	for(i=0;i<memsize;i++){
		if (coremap[i].fur > furtherest){
			furtherest = coremap[i].fur;
			evict = i;
		}	
    }
                                 

	return evict; 
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {

	int i;
	int k=0;
	// store trace into correspoinding page
	for(i=0; i<memsize; i++){
		if(coremap[i].pte == p){
			k = i;	
			coremap[i].trace = trace_vaddr[current];
			break;		
		}	
	}
	// calculate how far away this page will be accessed next time
	int fur = trace_len - current;
	for(i = current +1; i < trace_len; i++){
		if (trace_vaddr[i] == coremap[k].trace){
			fur = i - current;
			break;		
		}	
	}
	coremap[k].fur = fur;
	current ++;
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	// load in tracefile and store vaddr into an array
	FILE *tfp;
	char type;
	addr_t vaddr;
	int i;
	char buf[MAXLINE];

	if(tracefile != NULL) {
		if((tfp = fopen(tracefile, "r")) == NULL) {
			perror("Error opening tracefile:");
			exit(1);
		}
	}

	trace_len = 0;

	while(fgets(buf, MAXLINE, tfp) != NULL){
		trace_len += 1;
	}

	trace_vaddr = (addr_t *)malloc(trace_len * sizeof(addr_t));

	if((tfp = fopen(tracefile, "r")) == NULL) {
		perror("Error opening tracefile:");
		exit(1);
	}

	i = 0;
	vaddr = 0;
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			trace_vaddr[i] = vaddr;
			i ++;
		}
	}

	fclose(tfp);

	current = 0;

}

