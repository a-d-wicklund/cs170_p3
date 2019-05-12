#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void* func(int *input){
	printf("in function\n");
    int i = 0;
	//while(1);
        //if (i++ % 1000000 == 0)
        printf("%d\n",1);
        //usleep(10000);
        i++;
	//}
}


void* func2(int *input){
	printf("in function\n");
    int i = 0;
	//while(i<10000) {
        //if (i++ % 1000000 == 0)
        printf("%d\n",2);
		pthread_exit(0);
        //usleep(10000);
        //i++;
	//}
}
void* func3(int *input){
	printf("in function\n");
    int i = 0;
	//while(i<10000) {
        //if (i++ % 1000000 == 0)
        printf("%d\n",3);
        //usleep(10000);
        //i++;
	//}
}



int main(){

    pthread_t thread, thread2, thread3;
    
    //comment
    int in = 5;
    int in2 = 5;
    //pthread_create(&thread, NULL, &func, &in);
    //pthread_create(&thread2, NULL, &func2, &in2);
    //pthread_create(&thread3, NULL, &func3, &in2);
    //printf("Thread ID: %d\n", pthread_self());
    int i = 0;
	for(i = 0; i < 5; i++){
		pthread_create(&thread, NULL, &func, &in);
	}
	while(1) {
        //if (i++ % 1000000 == 0)
        //printf("%d\n",i);
        //usleep(10000);
        //i++;
	}
    pthread_exit(0);

    printf("still going\n");
}
