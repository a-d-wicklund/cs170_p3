#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

static void* _thread_dummy_loop(void* arg){
    pthread_t a = pthread_self();
    int *val;
    for(int i = 0; i < 100000; i++);
    
    val  = (int *) malloc(sizeof(int));
    *val = 42;

    return (void *) val;
}

static int test4(void){
    pthread_t tid;
    void *val;
    int i;
	int size = 2;
    int tids[size];
    
    for (i=0; i<size; i++) {
        pthread_create(&tid, NULL, &_thread_dummy_loop, NULL);
        tids[i] = tid;
    }

    for (i=0; i<size; i++) {
        pthread_join(tids[i], &val);
        if (*(int*)val != 42)
            return 1;
    }

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
	int bad = test4();
	if(!bad)
		printf("PASSED!!\n");
    else
        printf("FAILED :(\n");
    


}
