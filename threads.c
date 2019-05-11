#include <pthread.h>
#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <semaphore.h>

enum status{RUNNING, BLOCKED, READY};

int initHappened = 0;

int availID[129] = {0};


//Structure that holds info about the thread. Need an array of these.   
typedef struct ThreadControlBlock{
    pthread_t tid;
	enum status stat;
    jmp_buf jbuf;
    char* sp;
    void* (*startFunc) (void*);
    void* arg;
}tcb;


typedef struct LinkedQueue{
    struct LinkedQueue* next;
    tcb* block;
}lq;

lq* head;
lq* tail;

pthread_t createID(){
    pthread_t i = 1;
    while(availID[i] == -1)
        i++;
    availID[i] = -1;
    return i;
}

pthread_t pthread_self(){
    return head->block->tid;
    //How does this access an instance of the struct? 
}

static long int i64_ptr_mangle(long int p)
{
    long int ret;
    asm(" mov %1, %%rax;\n"
        " xor %%fs:0x30, %%rax;"
        " rol $0x11, %%rax;"
        " mov %%rax, %0;"
    : "=r"(ret)
    : "r"(p)
    : "%rax"
    );
    return ret;
}

void schedule(){
	printf("Entered scheduler. Saving state of tid %d\n",head->block->tid);
	if(setjmp(head->block->jbuf) == 0){
		
        if(head->next != NULL){//Put current head at the end, change head to next
            tail->next = head;
            head = head->next;
            tail = tail->next;
            tail->next = NULL;
		}
		printf("about to jump to thread with ID %d and function at %p\n", head->block->tid, head->block->startFunc); 
		longjmp(head->block->jbuf,1);
    }
    else{
        return;
    }

}

void wrapper(){
	printf("entered wrapper for thread %d\n", head->block->tid);
    (*(head->block->startFunc))(head->block->arg);
   	printf("exiting wrapper for thread %d\n", head->block->tid);

    pthread_exit(0);
}



void pthread_init(){
    
	//Set main function to head, allocate its block
    head = malloc(sizeof(struct LinkedQueue));
   
	head->block = malloc(sizeof(struct ThreadControlBlock));
	head->block->tid = (pthread_t) 0;

	tail = head;
    printf("Main's node's pointer is %p\n",head);
	
	struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = schedule;
    sigact.sa_flags = SA_NODEFER;
    if(sigaction(SIGALRM, &sigact, NULL) == -1)
        perror("Error: cannot handle SIGALRM");

	ualarm(50000, 50000);//Send SIGALRM right away. Then, at 50ms intervals.
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void* (*start_routine) (void*), void *arg){

	//Initialize first thread for main
	if(!initHappened){
        pthread_init();
        initHappened = 1;
    }
 
	//Place new block at tail end of queue  
	lq* tmp = malloc(sizeof(struct LinkedQueue));
    //Set the new block to be the tail
 	tail->next = tmp;
	tail = tail->next;
	tail->next = NULL; 
	tail->block = malloc(sizeof(struct ThreadControlBlock));			

	pthread_t nextID = createID();
    tail->block->tid = nextID;
    *thread = nextID;
	printf("Creating thread with ID %d\n",nextID);

    //printf("tmp points to address %p\nhead points to address %p\ntail points to address %p\n",tmp->block->tid, head->block->tid, tail->block->tid);

	tail->block->startFunc = start_routine;
	tail->block->arg = arg;

    tail->block->sp = malloc(32767);
    long* stackTop = (long*) (tmp->block->sp + 32767);//long pointer to top of stack.

	if(setjmp(tail->block->jbuf) == 0){
		//set the PC and stacl pointer to address of wrapper function and top of stack, respectively
   		*((long*) (&(tail->block->jbuf))+6) = i64_ptr_mangle((long)stackTop);
    	*((long*) (&(tail->block->jbuf))+7) = i64_ptr_mangle((long)&wrapper);
		//printf("after mangle\n");
	}
	else{
		pthread_exit(0);
	}
 
    return 0;    
    
}
void pthread_exit(void *retval){
    printf("Exiting thread  %d. Next up, %d\n", head->block->tid, head->next->block->tid);
	availID[head->block->tid] = 0;
	//lq* tmp = head;
    free(head->block->sp);
    free(head->block);
    head = head->next;
	//free(tmp);
    //printf("head should be null but is %p\n", head);
    if(head == NULL){
        exit(0);
    }
    else
		printf("jumping to %d\n",head->block->tid);
        longjmp(head->block->jbuf, 1);

    while(1);
}



