#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>

static int a5;
static void* _thread_arg(void* arg){
    sem_t *sem = (sem_t*) arg;

    sem_wait(sem);
    a5 = 20;
    sem_post(sem);
    
    pthread_exit(0);
}

static int test5(void){
    pthread_t tid1;
    sem_t sem;
    int i;

    sem_init(&sem, 0, 1);
    a5 = 10;

    sem_wait(&sem);
    pthread_create(&tid1, NULL,  &_thread_arg, &sem);

    for (i=0; i<3; i++) {  // sleep for 3 schedules
        sleep(1);
    }
    
    if (a5 != 10)
        return 1;
    sem_post(&sem);

    pthread_join(tid1, NULL);
    sem_destroy(&sem);

    return 0;
}


void *func(int *input){

	printf("Inside thread function\n");

	int a = 5;

	pthread_exit(&a);

}

int main(){

	pthread_t thread;
	
	int arg;
	//pthread_create(&thread, NULL, &func, &arg);
	int *ret;
	//pthread_join(thread,&ret);
	printf("After join. About to begin next test\n");	
	int bad = test1();
	if(!bad)
		printf("PASSED!!\n");
    else
        printf("FAILED :(\n");
    


}
