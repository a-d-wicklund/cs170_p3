#include <stdio.h>
#include <stdlib.h>


void *func(int *input){

	printf("Inside thread function\n");

	int a = 5;

	pthread_exit(&a);

}

int main(){

	pthread_t thread;
	
	int arg;
	pthread_create(&thread, NULL, &func, &arg);
	int *ret;
	pthread_join(thread,&ret);
	printf("After join. About to end\n");	
}
