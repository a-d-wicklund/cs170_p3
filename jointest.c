#include <stdio.h>
#include <stdlib.h>
#include <pthreads.h>


int main(){

pthread_t tid;
int input;
int in = 5;;
pthread_create(&tid, NULL, &func1, &in);



}
