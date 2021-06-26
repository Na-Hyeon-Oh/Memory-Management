#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 1000000

typedef struct queue{
	int head;
	int tail;
	int* Q;
} Queue; // used for FIFO and WS

typedef struct traceWS{
	int cnt;
	int* last;
} traceWS; // last page state

int N; // # # of page per process
int M; // # of page frame
int W; // window size
int K; // length of page ref string
int* ref; // page reference string

int set(char* name); // set from input file
/* Page Replacement Implementation */
int MIN();
int FIFO();
int LRU();
int LFU();
int WS();
traceWS printMem_WS(int* input, int start, int end); // print page frame info in WS
void printPF(int f); // print # of page fault
void delete(); // free
void checkInput(); //^

int main(int argc, char* argv[]){
	
	if(set(argv[1]) < 0) {
		perror("> Error in Input Setting");
		return 0;
	}
	//checkInput();//^
	
	if(MIN() < 0){
		perror("> Error in MIN");
		delete();
		return 0;
		
	}
	if(FIFO() < 0){
		perror("> Error in FIFO");
		delete();
		return 0;
	}
	if(LRU() < 0){
		perror("> Error in LRU");
		delete();
		return 0;
	}
	if(LFU() < 0){
		perror("> Error in LFU");
		delete();
		return 0;
	}
	if(WS() < 0){
		perror("> Error in WS");
		delete();
		return 0;
	}
	
	// Delete ALL Usage of Memory
	delete();
	
	return 0;
}


/* Functions */
int set(char* name){

	/* Set input file */
	//char* fileName = name;
	char* fileName = "input.txt";
	
	FILE* input = fopen(fileName, "r");
	if(!input){
		perror("Failed to open input file. Terminate.");
		return -1;
	}
	
	/* Read input file*/
	int i, tmp;
	char line[MAX];
	char* ptr;
	
	memset(line, 0, sizeof(char) * MAX); //line initializes
	if(fgets(line, sizeof(line), input) == NULL){
		puts("Insufficient Input.");
		return -1;
	}// 1 line gets

	// get N
	// 0 < N <= 100
	if((ptr = strtok(line, " ")) != NULL){
		N = atoi(ptr);

		if(N <= 0 || N > 100) {
			puts("N from input file is wrong.");
			return -1;
		}
	}
	else{
	       puts("Insufficient Input.");
       	       return -1;
	}

	// get M
	// 0 < M <= 20 for MIN / FIFO/ LRU / LFU
	// NO MATTER of M value for WS
	if((ptr = strtok(NULL, " ")) != NULL){
		M = atoi(ptr);

		if(M <= 0 || M > 20){
			puts("M from input file is wrong.");
			return -1;
		}
	}
	else {
		puts("Insufficient Input.");
		return -1;
	}

	// get W
	// 0 < W <= 100 for WS
	// NO MATTER of W value for MIN / FIFO / LRU / LFU
	if((ptr = strtok(NULL, " ")) != NULL){
		W = atoi(ptr);

		if(W <= 0 || W > 100){
			puts("W from input file is wrong.");
			return -1;
		}
	}
	else{
		puts("Insufficient Input.");
		return -1;
	}

	// get K
	// 0 <= K <= 1000
	if((ptr = strtok(NULL, " ")) != NULL){
		K = atoi(ptr);

		if(K < 0 || K > 1000){
			puts("K from input file is wrong.");
			return -1;
		}
	}
	else{
		puts("Insufficient Input.");
		return -1;
	}

	// check
	if((ptr = strtok(NULL, " ")) != NULL) {
		puts("Too much Input at 1st line.");
		return -1;
	}


	if(K > 0){
		memset(line, 0, sizeof(char) * MAX);
 		if(fgets(line, sizeof(line), input) == NULL){  
 			puts("Insufficient Input.");  
 			return -1;   
  		}// 2nd line gets

		// set ref
		ref = (int*) malloc(sizeof(int) * K);
		for(i = 0; i < K; i++) ref[i] = 0;

		// get reference string
		i=0;
		if((ptr = strtok(line, " ")) != NULL){
			tmp = atoi(ptr);

			if(tmp > N-1 || tmp < 0){
				puts("Invalid Page Reference");
				free(ref);
				return -1;
			}
	
			ref[i] = tmp;
		}
		else{
			puts("Insufficient Input.");
			free(ref);
			return -1;
		}
		for(i = 1; i < K; i++){
			if((ptr = strtok(NULL, " ")) != NULL){
				tmp = atoi(ptr);
	
				if(tmp > N-1 || tmp < 0){
					puts("Invalid Page Reference");
					free(ref);
					return -1;
				}
	
				ref[i] = tmp;
			}
			else{
				puts("Insufficient Input.");
				free(ref);
				return -1;
			}
		}

		// check
		if((ptr = strtok(NULL, " ")) != NULL) {    
		   	puts("Too much Input at 2nd line.");  
			free(ref);
	 		return -1;
       		}
	}

	memset(line, 0, sizeof(char) * MAX); 
	if(fgets(line, MAX, input) != NULL){
       		puts("Too much Input.");   
	      	free(ref);	
	    	return -1; 
	}// 3rd line gets 

	return 0;
}

int MIN(){
	puts("MIN");
	
	int pageFault = 0; 
     
	if(K == 0) {
		printPF(pageFault);
		return 0;
	}

	puts("  t   |  ref  | Page Fault | Memory state");

	int t; // time // page reference string index  
	int* mem = (int*) calloc(sizeof(int), M); // page frame  
	int* remainT = (int*) calloc(sizeof(int), N); // remain time to next look-up table
	int available = 0; // > M: inavailable, else : available to put in page frame  
	int pageFault_flag = 0; // 0: no page fault, 1: page fault exists  
	int pageFault_index = -1;
	int i, range;  
	int tmp, tmpVal;  
	int min; // page index of longest remained time to next used
	int min_remainT; // longest remained time to next used

	for(t = 0; t < K; t++){  
 		pageFault_index = -1;		
		min = 0;              
	     	min_remainT = -1; // initialize

		tmp = ref[t];        
      
		printf("%5d | %5d | ", t + 1, tmp);     
   
		// find whether there is current page reference                 
		range = available;                 
		for(i = 0; i < range; i++){
			// NO Page Fault                         
			if(tmp == mem[i]){ // find current page reference             
	    			pageFault_flag = 0;            
	   			break;      
     			}

			tmpVal = remainT[mem[i]];  
 			if(min_remainT < tmpVal){ // tie-breaking : with smaller page num
				min = i; // find longest remained time page from remainT array and update   
  				min_remainT = tmpVal;
			}

			// Page Fault                         
			if(i == range - 1){      
     				pageFault_flag = 1;       
     			
				// page frame is available       
				if(available < M){ // check Page Frame       
      					pageFault_index = available;
					mem[available++] = tmp; // into (available)th page frame      
     				}         
	
				// full page frame       
				else{     
					pageFault_index = min;
    					mem[min] = tmp; // replace      
     				}         
			}      
     		}           
	 
		// first entry 
		if(available == 0) {               
	      		pageFault_flag = 1;           
			pageFault_index = available;
	  		mem[available++] = tmp;       
      		}

		// store min Value from current time + 1
		for(i = 0; i < available; i++){
			tmpVal = t + 1;

			while((tmpVal < K) && (mem[i] != ref[tmpVal])){    
		   		tmpVal++;
	       		} 
			if(tmpVal <= K){
		       		if((tmpVal < K) && (mem[i] == ref[tmpVal])) {
					remainT[mem[i]] = tmpVal - t;
				}
				else if(tmpVal == K) remainT[mem[i]] = 10000;
			}
		}
	 
		// memory state                 
		if(pageFault_flag != 0) {              
	     		printf("    YES    | ");        
       			pageFault++;        
       		}         
		else printf("     NO    | ");          
	 	for(i = 0; i < available; i++){       
      			if(pageFault_index != i) printf("%d ", mem[i]);
	       		else printf("*%d ", mem[i]);		
		}              
	     	puts("");        
       	}
	
	printPF(pageFault);
		
	free(remainT);
	free(mem);	

	return 0;
}

int FIFO(){
	puts("FIFO");
	
	int pageFault = 0;   
 
	if(K == 0) {
		printPF(pageFault);  
		return 0;
	}

	puts("  t   |  ref  | Page Fault | Memory state");
	
	int t; // time // page reference string index         
	/* page frame as Queue */
	Queue mem;
	mem.head = 0;
	mem.tail = 0;
	mem.Q = (int*) malloc(sizeof(int) * M);
    	int qFlag = 0; // > M: full Q, 0 : empty Q
	int pageFault_flag = 0; // 0: no page fault, 1: page fault exists
	int pageFault_index = -1;
	int i, range;
	int tmp;
	//int fifoControl; // head-tail control of Q
	
	for(t = 0; t < K; t++){
		pageFault_index = -1; // initialize

		tmp = ref[t];
	   
		printf("%5d | %5d | ", t + 1, tmp);    
	  
		// find whether there is current page reference    
		range = qFlag;      
	     	for(i = 0; i < range; i++){         
			//printf("%d %d \n", mem.head, mem.tail);//^
			// NO Page Fault                 
			if(tmp == mem.Q[i]){ // find current page reference                
		       		pageFault_flag = 0;                
			  	break;
	    		}
	     
			// Page Fault                 
			if(i == range - 1){
				pageFault_flag = 1;            
		  
				// page frame is available                         
				if(qFlag < M){ // check Page Frame 
					pageFault_index = qFlag; // loc of page fault

					mem.Q[qFlag++] = tmp; // into (available)th page frame
				   	
					if(mem.tail == (M - 1)){
						mem.tail = 0;
						mem.head = 1;
					}
					else mem.tail++;
					if(M != 1 && mem.tail == mem.head) {
						perror("inavailable fifo\n");
						return -1;
					}
	       			}          
		 
				// full page frame                        
				else{ 
				     	pageFault_index = mem.head;

	   				mem.Q[mem.head] = tmp; // replace
				   	
					mem.tail = mem.head;
					if(mem.head == (M - 1)) mem.head = 0;
					else mem.head++;
					if(M != 1 && mem.tail == mem.head){
						perror("inavailable fifo\n");
						return -1;
					}
		  		}		
		      	}  
	 	}  
	
		// first entry         
		if(qFlag == 0) {     
	    		pageFault_flag = 1;
		   	pageFault_index = qFlag;	
	    		mem.Q[qFlag++] = tmp;
	   	}

		//printf(">> %d %d   ", mem.head, mem.tail);
		
		// memory state
		if(pageFault_flag != 0){
       			printf("    YES    | ");   
  			pageFault++; 
    		}      
     		else printf("     NO    | ");

	        //fifoControl = mem.head;	
       		for(i = 0; i < qFlag; i++){
			if(pageFault_index != i) printf("%d ", mem.Q[i]);
			else printf("*%d ", mem.Q[i]);

			//if(fifoControl == (M - 1)) fifoControl = 0;                         
			//else fifoControl++;
       		}        
       		puts("");   
  	}

	printPF(pageFault);

	free(mem.Q);

	return 0;
}

int LRU(){
	puts("LRU");

	int pageFault = 0;

	//check input
	if(K == 0) {
		printPF(pageFault);
		return 0;
	}

	puts("  t   |  ref  | Page Fault | Memory state");

	int t; // time // page reference string index
	int* mem = (int*) calloc(sizeof(int), M); // page frame    
    	int* time = (int*) calloc(sizeof(int), N); // recent time trace look-up table
	int available = 0; // > M: inavailable, else : available to put in page frame  
  	int pageFault_flag = 0; // 0: no page fault, 1: page fault exists
     	int pageFault_index = -1; // to express where page fault exists	
   	int i, range;   
   	int tmp, tmpVal;    
    	int lru; // least recently used page index         
	int lru_time; // least recently used time

	for(t = 0; t < K; t++){
		pageFault_index = -1;
		lru = 0;
		lru_time = 10000; // initialize   
	  
		tmp = ref[t];
		 
		printf("%5d | %5d | ", t + 1, tmp);    
	   
		// find whether there is current page reference   
		range = available;     
	     	for(i = 0; i < range; i++){
	       		// NO Page Fault                 
			if(tmp == mem[i]){ // find current page reference                         
				pageFault_flag = 0;                        
				break;           
		   	}                  
			 
			tmpVal = time[mem[i]];                 
			if(lru_time > tmpVal){        
				lru = i; // find Least Recently Used page from time array and update
				lru_time = tmpVal;      
      			}

			// Page Fault  
			if(i == range - 1){    
    				pageFault_flag = 1;                                  
				 
				// page frame is available                                 
				if(available < M){ // check Page Frame
					pageFault_index = available;					
					mem[available++] = tmp; // into (available)th page frame                
				}              
	     			// full page frame               
				else{
				 	pageFault_index = lru;	
       					mem[lru] = tmp; // replace 
 				}            
	    		}                
		}             
	    
		// first entry              
		if(available == 0) {                
			pageFault_flag = 1;         
		        pageFault_index = 0;	
	  		mem[available++] = tmp;
		}

		time[tmp] = t + 1; // set recent time array                  
		 
		// memory state                 
		if(pageFault_flag != 0) {          
	  		printf("    YES    | ");        
			pageFault++;         
	 	}           
	   	else printf("     NO    | ");            
	    	for(i = 0; i < available; i++){          
	  		if(pageFault_index != i) printf("%d ", mem[i]);
			else printf("*%d ", mem[i]);	
		}        
		puts("");     
     	}       
      
	printPF(pageFault);
	 
	free(time);
	free(mem);	 		 	

	return 0;
}

int LFU(){
	puts("LFU");

	int pageFault = 0; 

	if(K == 0) {
		printPF(pageFault);
		return 0;
	}

	puts("  t   |  ref  | Page Fault | Memory state");

	int t; // time // page reference string index
	int* mem = (int*) calloc(sizeof(int), M); // page frame
	int* cnt = (int*) calloc(sizeof(int), N); // count array // page reference count look-up table
	int* time = (int*) calloc(sizeof(int), N); // recent time trace look-up table // for tie breaking
	int available = 0; // > M: inavailable, else : available to put in page frame
	int pageFault_flag = 0; // 0: no page fault, 1: page fault exists
	int pageFault_index = -1;
	int i, range;
	int tmp, tmpVal;
	int lfu; // least frequently used page index
	int lfu_cnt; // least frequently used cnt

	for(t = 0; t < K; t++){
		pageFault_index = -1;
		lfu = 0;
		lfu_cnt = 10000; // initialize

		tmp = ref[t];
		printf("%5d | %5d | ", t + 1, tmp);

		// find whether there is current page reference
		range = available;
		for(i = 0; i < range; i++){
			// NO Page Fault
			if(tmp == mem[i]){ // find current page reference
				pageFault_flag = 0;
				break;
			}

			tmpVal = cnt[mem[i]];
			if(lfu_cnt >= tmpVal){
				if(lfu_cnt > tmpVal){
					lfu = i; // find Least Frequently Used page from cnt array and update
					lfu_cnt = tmpVal;
				}
				else if(time[mem[lfu]] > time[mem[i]]){
					lfu = i;
					lfu_cnt = tmpVal;
				} // tie-breaking
			}

			// Page Fault
			if(i == range - 1){
				pageFault_flag = 1;

				// page frame is available
				if(available < M){ // check Page Frame
					pageFault_index = available;
					mem[available++] = tmp; // into (available)th page frame
				}

				// full page frame
				else{
					pageFault_index = lfu;
					mem[lfu] = tmp; // replace
				}
			}
		}
		
		// first entry
		if(available == 0) { 
	       		pageFault_flag = 1;
		        pageFault_index = 0;	
			mem[available++] = tmp;
		}

		cnt[tmp]++; // increase cnt array
		time[tmp] = t + 1; // set recent time array

		// memory state
		if(pageFault_flag != 0) {
			printf("    YES    | ");
			pageFault++;
		}
		else printf("     NO    | ");
		for(i = 0; i < available; i++){
			if(pageFault_index != i) printf("%d ", mem[i]);
			else printf("*%d ", mem[i]);
		}
		puts("");
	}

	printPF(pageFault);

	free(cnt);
	free(time);
	free(mem);

	return 0;
}

int WS(){
	puts("WS");      
    	
	int pageFault = 0;    
  
	if(K == 0) {
		printPF(pageFault);   
	 	return 0;
	}	
	
	puts("  t   |  ref  | Page Fault | Memory state (# of frames allocated)");
	
	int t; // time // page reference string index  
 	Queue mem; // page frame
        mem.head = 0;
	mem.tail = 0;
	mem.Q = (int*) calloc(sizeof(int), N);
	traceWS last;
	int allocated = 0; // # of allocated page frame
	int pageFault_flag = 0; // 0: no page fault, 1: page fault exists             
       	int i, range;      
     	int tmp, tmpVal;
	
	for(t = 0; t < K; t++){     
		tmp = ref[t];   
	 	
		printf("%5d | %5d | ", t + 1, tmp); 

		// find whether there is current page reference    
		range = allocated;
		tmpVal = 0;
		//printf(" > %d %d %d <", allocated, mem.tail, tmpVal);
		for(i = 0; i < range; i++){
	     		// NO Page Fault
			//printf(" >> %d %d <<", tmpVal, last.last[tmpVal]);
			if(tmp == last.last[tmpVal]){ // find current page reference         
		   		pageFault_flag = 0;
				 
				tmpVal = mem.tail;	
				mem.Q[tmpVal] = tmp; 
				 
				if(mem.tail == (N - 1)) mem.tail = 0;         
				else mem.tail++;

	    			break;      
	     		}
			tmpVal++;	

			// Page Fault  
			if(i == range - 1){       
      				pageFault_flag = 1;
			 
				tmpVal = mem.tail;
				mem.Q[tmpVal] = tmp;
				
				if(mem.tail == (N - 1)) mem.tail = 0;
				else mem.tail++;

				if(N != 1 && mem.tail == mem.head){
					printf("invalid q");
					return -1;
				}
			}

     		}
       		
		// first entry  
		if(allocated == 0) {     			
		   	pageFault_flag = 1; 
	       	
		  	mem.Q[mem.head] = tmp;
			
			if(mem.tail == (N - 1)) mem.tail = 0;
			else mem.tail++;
			if(N != 1 && mem.tail == mem.head){
				printf("invalid q");
				return -1;
			}
	       	}
		 
		// VA  
		// re-set start point according to window size W
		else if(t >= W){         
		       	if(mem.head == (N - 1)) mem.head = 0;       
		     	else mem.head++;

			if(N != 1 && mem.tail == mem.head){
				printf("invalid q");
				return -1;
			}
	      	}
	      
		// memory state
		if(pageFault_flag != 0) {       
		     	printf("    YES    | ");  
			pageFault++; 
	       	}
		else printf("     NO    | "); 
	
		if(t < W) tmpVal = mem.head;
		else if(mem.head != 0) tmpVal = mem.head - 1;
		else tmpVal = N - 1;
		last = printMem_WS(mem.Q, tmpVal, mem.tail);
		allocated = last.cnt;
	 
		printf(" ( %d )", allocated);
	       	puts("");	
	}

	printPF(pageFault);

	return 0;
}

traceWS printMem_WS(int* input, int start, int end){
	int* result = (int*) malloc(sizeof(int) * N);
	int i, tmp, tmpVal;
	int size;
	traceWS ret; // store value of last page frame
	ret.cnt = 0;

	for(i = 0; i < N; i++) result[i] = -1;
	if(end <= start) end += N;
	//else if(end == start) printf("err\n");
	size = end - start;

	tmpVal = start;
	//printf("**** %d %d %d ", size, start, end);
	for(i = 0; i < size; i++){
		tmp = input[tmpVal];
		
		if(result[tmp] != 1){
			result[tmp] = 1;
			ret.cnt++;
		}

		if(tmpVal == (N - 1)) tmpVal = 0;
		else tmpVal++;
	}

	ret.last = (int*) calloc(sizeof(int), ret.cnt);

	// print
	tmp = 0;
	for(i = 0 ; i < N; i++){
		if(result[i] != -1) {
			printf("%2d ", i);

			// store value
			ret.last[tmp++] = i;
		}
		else printf("%2c ", '-');
	}

	if(tmp != ret.cnt){
		perror("> Error in WS\n");
		delete();
		exit(1);
	}

	return ret;
}

void printPF(int f){
	printf(" => Page Fault : %d\n", f);
	puts("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");
}

void checkInput(){
	int i;

	puts("\n< I N P U T >");

	printf("N: %d\nM: %d\nW: %d\nK: %d\n", N, M, W, K);
	for(i=0; i < K; i++){
	       printf("%d ", ref[i]);
	}	       

	puts("\n");
}

void delete(){
	free(ref);	
}
