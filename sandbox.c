#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>


static int a6;
static void* _thread_sem2(void* arg){
    sem_t *sem;
    int local_var, i;

    sem = (sem_t*) arg;

    sem_wait(sem);
    local_var = a6;
    for (i=0; i<5; i++) {
        local_var += pthread_self();
    }
	printf("about to sleep\n");
    for (i=0; i<3; i++) {   // sleep for 3 schedules
        sleep(1);
    }
    a6 = local_var;

    sem_post(sem);
    pthread_exit(0);
}

static int test6(void){
    pthread_t tid1, tid2;
    sem_t sem;

    sem_init(&sem, 0, 1);
    a6 = 10;

    pthread_create(&tid1, NULL, &_thread_sem2, &sem);
    pthread_create(&tid2, NULL, &_thread_sem2, &sem);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    sem_destroy(&sem);

    if (a6 != (10 + (tid1*5) + (tid2*5)))
        return 1;
    return 0;

}


void *func(int *input){

	printf("Inside thread function\n");
	sleep(1);
	printf("After sleep\n");
	pthread_exit(0);

}

int main(){

	pthread_t thread;
	
	/*
	int arg;
	pthread_create(&thread, NULL, &func, &arg);
	int *ret;
	pthread_join(thread,NULL);
	*/

	
	printf("After join. About to begin next test\n");	
	int bad = test6();
	if(!bad)
		printf("PASSED!!\n");
    else
        printf("FAILED :(\n");
    


}
