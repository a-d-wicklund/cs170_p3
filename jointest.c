#include <stdio.h>
#include <stdlib.h>
#include <pthreads.h>


void* func1(int input){

	printf("Inside thread function\n");

	pthread_exit(input*input);
}

int main(){

	pthread_t tid;
	int input;
	int in = 5;
	int ret;
	pthread_create(&tid, NULL, &func1, &in);
	pthread_join(tid,&ret);
	printf("Return value is %d\n", ret);

}
