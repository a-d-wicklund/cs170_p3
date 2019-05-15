#include <pthread.h>
#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
//#include <threads.h>

enum status{RUNNING, BLOCKED, TERMINATED, TRASH};

int initHappened = 0;
int curID = 1;
int availID[129] = {0};

void schedule();

//Structure that holds info about the thread. Need an array of these.   
typedef struct ThreadControlBlock{
    pthread_t tid;
	enum status stat;
	void *retval;
	int joinedBy;
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


void lock(){
	//Block the SIGALRM signal 
	sigset_t sigs;
	sigemptyset(&sigs);
	sigaddset(&sigs, SIGALRM);
	sigprocmask(SIG_BLOCK,&sigs,NULL);

}

void unlock(){
	//Allow all signals to interrupt again. 
	sigset_t sigs;
	sigemptyset(&sigs);
	sigprocmask(SIG_BLOCK,&sigs,NULL); //Immediately unblock all signals. Will this cause the unlock function to get interrupted?
	useconds_t t = ualarm(0,0); //Time remaining 
	//If time remaining is zero, the signal occured while in the critical section. reschedule immediately
	if(t == 0){
		schedule();
	}
	//Otherwise, start up the timer again. 
	else{
		ualarm(t,0);
	}
}

int sem_init(sem_t *sem, int pshared, unsigned int value){
	return 0;
}

int sem_destroy(sem_t *sem){
	return 0;
}

int sem_wait(sem_t *sem){
	return 0;
}

int sem_post(sem_t *sem){
	return 0;
}


//Find the next thread with status RUNNING and move the head pointer to that thread
int findNextValid(){
	//lq* cur = in;
	printf("Finding next running thread\n");
	//lock();
	lq* origHead = head->next;
	head = head->next;
	tail = tail->next;
	while(head->block->stat != RUNNING){
		//printf("Spinning\n");
		// If it is marked as trash, that means it has been terminated and its return grabbed.
		if(head->block->stat == TRASH){
			printf("Removing trash\n");
			availID[head->block->tid] = 0;
			free(head->block->sp);
			free(head->block);
			head = head->next;
			tail->next = head;
		}
		head = head->next; 
		tail = tail->next; //tail should always be one behind head
		//If head has made it all the way past original head, exit
		if(head == origHead)
			return 0;
	}
	return 1;
	//unlock();
	//What if there isn't a running block in the whole queue? 
}

lq* findNodeByID(int tid){

	//lock();
	lq* cur = head;
	int id = cur->block->tid;
	while(tid != id){
		if(cur->next == head) //Reached the end without finding thread
			return NULL;
		cur = cur->next;
		id = cur->block->tid;
	}
	return cur;
	//unlock();
}


pthread_t createID(){
  
	while(availID[curID] == -1){
		curID = (curID+1)%130;
	}
	availID[curID] = -1;
    return curID; 
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
	ualarm(50000,0);
	printf("Entered scheduler. Saving state of tid %d\n",head->block->tid);
	if(setjmp(head->block->jbuf) == 0){
		//printf("Stuck here\n");
		findNextValid(head->next);
		printf("about to jump to thread with ID %d and function at %p\n", head->block->tid, head->block->startFunc); 
		longjmp(head->block->jbuf,1);
    }
    else{
		printf("why\n");
        return;
    }

}

void wrapper(){
	printf("entered wrapper for thread %d\n", head->block->tid);
    void * ret = (*(head->block->startFunc))(head->block->arg);
   	printf("exiting wrapper for thread %d\n", head->block->tid);

    pthread_exit(ret);
}

int pthread_join(pthread_t thread, void **valueptr){
	if(thread == head->block->tid)
		return EDEADLK;
	lock();
	printf("inside pthread_join for thread %d\n",thread);
	lq* node = findNodeByID(thread);
	if(node == NULL)
		return ESRCH;
	if(node->block->stat != TERMINATED){
		node->block->joinedBy = pthread_self();
		head->block->stat = BLOCKED;
		head->block->stat = TERMINATED;
		ualarm(0,0);
		printf("rescheduling\n");
		schedule();
	}
	printf("Came back to pthread_join(). About to save return from thread %d\n", thread);	
	if(valueptr != NULL)
		*valueptr = node->block->retval; //save the return value
	printf("save return value\n");
	node->block->stat = TRASH; //This may cause problems later. revisit
	return 0;
	unlock();
}

void pthread_init(){
    
	//Set main function to head, allocate its block
    head = malloc(sizeof(struct LinkedQueue));
   
	head->block = malloc(sizeof(struct ThreadControlBlock));
	head->block->tid = (pthread_t) 0;
	head->block->stat = RUNNING;
	head->block->joinedBy = -1;
	availID[0] = -1;

	tail = head;
	tail->next = head;
    printf("Main's node's pointer is %p\n",head);

	
	struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = schedule;
    sigact.sa_flags = SA_NODEFER;
    if(sigaction(SIGALRM, &sigact, NULL) == -1)
        perror("Error: cannot handle SIGALRM");

	ualarm(50000, 0);//Send SIGALRM right away. Then, at 50ms intervals.
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void* (*start_routine) (void*), void *arg){

	//Initialize first thread for main
	if(!initHappened){
        pthread_init();
        initHappened = 1;
    }
	//printf("Tail before adding new: %d\nHead before adding new: %d\n",tail->block->tid, head->block->tid); 
	printf("List is: %d %d %d ... %d %d\n",head->block->tid, head->next->block->tid, head->next->next->block->tid, tail->block->tid, tail->next->block->tid);

	//Place new block at tail end of queue  
	lq* tmp = malloc(sizeof(struct LinkedQueue));
    //Set the new block to be the tail
 	tail->next = tmp;
	tail = tail->next;
	tail->next = head; 
	tail->block = malloc(sizeof(struct ThreadControlBlock));			

	tail->block->stat = RUNNING;
	tail->block->joinedBy = -1;
	pthread_t nextID = createID();
    tail->block->tid = nextID;
    *thread = nextID;
	printf("Creating thread with ID %d\n",nextID);

	printf("List is: %d %d %d ... %d %d\n",head->block->tid, head->next->block->tid, head->next->next->block->tid, tail->block->tid, tail->next->block->tid);

	tail->block->startFunc = start_routine;
	tail->block->arg = arg;

    tail->block->sp = malloc(32767);
    long* stackTop = (long*) (tmp->block->sp + 32767);//long pointer to top of stack.

	if(setjmp(tail->block->jbuf) == 0){
		//set the PC and stacl pointer to address of wrapper function and top of stack, respectively
   		*((long*) (&(tail->block->jbuf))+6) = i64_ptr_mangle((long)stackTop);
    	*((long*) (&(tail->block->jbuf))+7) = i64_ptr_mangle((long)&wrapper);
	}
	else{
		pthread_exit(0);
	}
 
    return 0;    
    
}
void pthread_exit(void *retval){
    printf("Exiting thread  %d\n", head->block->tid);
   	head->block->stat = TERMINATED;
	head->block->retval = retval;
	//int *val = (int*)retval;
	//printf("Retval is %d\n",*val);

	//If pthread_join has already been called on this thread, set it to running 
	if(head->block->joinedBy != -1){
		lq* n = findNodeByID(head->block->joinedBy); //Find the tcb for the thread that is waiting to join 
		n->block->stat = RUNNING; //Change the status of that thread to RUNNING (Wake it up)
	}
	printf("List is: %d %d %d ... %d %d\n",head->block->tid, head->next->block->tid, head->next->next->block->tid, tail->block->tid, tail->next->block->tid);

	if(!findNextValid())
		exit(0);

	printf("Found one\n");
	printf("jumping to %d\n",head->block->tid);
    longjmp(head->block->jbuf, 1);

    while(1);
}





