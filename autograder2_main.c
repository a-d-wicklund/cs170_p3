/**
 * Sean Gillen April 2019
 *
 * This file will automatically grade your cs170 project 2.
 * Please see the README for more info
 */


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#define PASS 1
#define FAIL 0

#define TEST_WAIT_MILI 2000 // how many miliseconds do we wait before assuming a test is hung

void lock();
void unlock();

/*
    TEST 0 TO 14 are the test from Sean for project 2
    TEST 22 to 28 are Ryan's tests for project 3
*/

//if your code compiles you pass test 0 for free
//==============================================================================
static int test0(void){
    return PASS;
}

//basic pthread create and exit tests
//==============================================================================
static void* _thread_dummy(void* arg){
    pthread_t a = pthread_self();
    pthread_exit(0);
}

static void* _thread_dummy_loop(void* arg){
    pthread_t a = pthread_self();
    for(int i = 0; i < 100000000; i++);
    pthread_exit(0);
}

static int test1(void){
    pthread_t tid1 = 0;
    pthread_t tid2 = 0;

    pthread_create(&tid1, NULL,  &_thread_dummy, NULL);
    pthread_create(&tid2, NULL,  &_thread_dummy_loop, NULL);

    if(tid1 != tid2){
        return PASS;
    }else{
        return FAIL;
    }
}


//basic pthread self test
//==============================================================================
static pthread_t global_tid1 = 15000;
static void* _thread_self_test(void* arg){
    global_tid1 = pthread_self();
    pthread_exit(0);
}

static int test2(void){
    pthread_t tid1 = 0;

    pthread_create(&tid1, NULL,  &_thread_self_test, NULL);

    while((global_tid1 != tid1)); //failure occurs on a timeout
    return PASS;
}


//scheduler test 1
//==============================================================================
static int a3 = 0;
static int b3 = 0;

static void* _thread_schedule_a(void* arg){
    while(1){
        a3 = 1;
    }

}
static void* _thread_schedule_b(void* arg){
    while(1){
        if(a3){
            b3 = 1;
        }
    }
}

static int test3(void){
    pthread_t tid1; pthread_t tid2;

    pthread_create(&tid1, NULL,  &_thread_schedule_a, NULL);
    pthread_create(&tid2, NULL,  &_thread_schedule_b, NULL);

    while(b3 != 1); //just wait, failure occurs if there is a timeout
    return PASS;


}


//scheduler test 2
//==============================================================================
static int a4;
static void* _thread_inc4(void* arg){

    for(int i = 0; i < 10; i++){
      // printf("thread inc4: i%i\n", i);
        a4++;
    }
    pthread_exit(0);

}

static int test4(void){
    pthread_t tid1; pthread_t tid2;

    pthread_create(&tid1, NULL,  &_thread_inc4, NULL);
    pthread_create(&tid2, NULL,  &_thread_inc4, NULL);


    for(int i = 0; i < 10; i++){
      // printf("main test4\n");
        a4++;
    }

    while(a4 != 30); //just wait, failure occurs if there is a timeout
    return PASS;

}


//scheduler test 3
//==============================================================================
static int a5;
static void* _thread_inc1(void* arg){
    a5++;
    // printf("a5: %i\n", a5);
    pthread_exit(0);

}

static int test5(void){
    pthread_t tid1;

    for(int i = 0; i < 128; i++){
        pthread_create(&tid1, NULL,  &_thread_inc1, NULL);
	// printf("created thread %i\n", i);
    }

    while(a5 != 128); //just wait, failure occurs if there is a timeout
    return PASS;

}


//passing arguments
//==============================================================================
static int e;
static void* _thread_arg(void* arg){
    e = *(int*)arg;
    pthread_exit(0);
}

static int test6(void){
    pthread_t tid1; pthread_t tid2;

    int arg = 244567;
    pthread_create(&tid1, NULL,  &_thread_arg, &arg);

    while(e != arg); //just wait, failure occurs if there is a timeout
    return PASS;

}

//does your stack work?
//==============================================================================
static int _thread_fib(int a){
    if(a == 0) {
        return 0;
    }
    if(a == 1){
        return 1;
    }

    return _thread_fib(a-1) + _thread_fib(a-2);
}


static void* _thread_fcn(void* arg){
    int fib = *(int*)arg;
    *(int*)arg = _thread_fib(fib);
    pthread_exit(0);
}


static int test7(void){
    pthread_t tid1; pthread_t tid2;

    int arg = 12;
    pthread_create(&tid1, NULL,  &_thread_fcn, &arg);

    while(arg != 144); //just wait, failure occurs if there is a timeout
    return PASS;

}


//Start of private tests


//basic pthread self test
//==============================================================================
static pthread_t global_tid2 = 57; 
static void* _thread_self_test2(void* arg){
    global_tid2 = pthread_self();
    pthread_exit(0);
}

static int test8(void){
    pthread_t tid1 = 0;
    
    pthread_create(&tid1, NULL,  &_thread_self_test2, NULL);
    
    while((global_tid2 != tid1)); //failure occurs on a timeout
    return PASS;
}


//do you actually resume execution? or do you call start routine over and over?
//==============================================================================
static int a9 = 0;
static int b9 = 0;
static int c9 = 0;

static void* _thread_schedule_c(void* arg){
    if(b9 == 0){
        while(b9 != 1){
            a9 = 1;
        }
        c9 = 1;
    }
}

static void* _thread_schedule_d(void* arg){
    while(1){
        if(a9){
            b9 = 1;
        }
    }
}

static int test9(void){
    pthread_t tid1; pthread_t tid2;
        
    pthread_create(&tid1, NULL,  &_thread_schedule_c, NULL);
    pthread_create(&tid2, NULL,  &_thread_schedule_d, NULL);
    
    while(c9 != 1); //just wait, failure occurs if there is a timeout
    return PASS;
        

}

//pthread create from a thread
//==============================================================================
static int a10; 


static void* _thread_create_inc(void* arg){
    a10 = 1;
}

static void* _thread_create(void* arg){
    pthread_t tid1;
    
    pthread_create(&tid1, NULL, &_thread_create_inc, NULL);
    pthread_exit(0);
}

static int test10(void){
    pthread_t tid1 = 0;
    
    pthread_create(&tid1, NULL,  &_thread_create, NULL);
    
    while((a10 != 1)); //failure occurs on a timeout
    return PASS;
}


//Let's combine some previous tests
//==============================================================================
static int test11(void){
    return( test1() && test7());
}


//==============================================================================
static int test12(void){
    return( test5() && test7());
}


//==============================================================================
static int test13(void){
    return( test4() && test6());
}


//another scheduler test
//==============================================================================
static int a14 = 0;
static void* _thread_inc14(void* arg){
    a14++;
    pthread_exit(0);
    
}

static int test14(void){
    pthread_t tid1; 
    
    for(int i = 0; i < 11; i++){
        pthread_create(&tid1, NULL,  &_thread_inc14, NULL);
    }
    
    while(a14 != 11); //just wait, failure occurs if there is a timeout
    
    for(int i = 0; i < 13; i++){
        pthread_create(&tid1, NULL,  &_thread_inc14, NULL);
    }

    while(a14 != 11 + 13); //just wait, failure occurs if there is a timeout
    
    return PASS;

}




static int a15 = 0;
static int iterator9 = 60;
static pthread_t test9_ids[128];
static void* _test_15_counter(void* arg)
{
    sem_t *sem = (sem_t *)arg;
    int ret_val = sem_wait(sem); 
    // Successful sem_wait returns 0
    if (ret_val == 0)
        a15++;
    pthread_exit(0);
}

static void* _test_15_simple_counter(void *arg)
{
    a15++;
}

static void* _test_15_create60more(void* arg)
{
    sem_t *sem = (sem_t *)arg;
    pthread_t tid1;

    for (int i = 0; i < 60; i++)
    {
        pthread_create(&tid1, NULL,  &_test_15_simple_counter, NULL);
        lock();
        test9_ids[iterator9] = tid1;
        ++iterator9;
        unlock();
        sem_post(sem);
    }
    pthread_exit(0);
}

static int test15(void){
    pthread_t tid1;
    sem_t sem;
    sem_init(&sem, 0, 0);


    for(int i = 0; i < 60; i++){
        pthread_create(&tid1, NULL,  &_test_15_counter, &sem);
        test9_ids[i] = tid1;
    }
    sleep(1);
    // Nothing should've incremented a15 because they should all be blocked on the semaphore
    if (a15 != 0)
    {
        return FAIL;
    }
    pthread_create(&tid1, NULL, &_test_15_create60more, &sem);
    for(int i = 0; i < 120; ++i)
    {
         // printf("ids[%i]: %si\n", i, ids[i]);
        int retval = pthread_join(test9_ids[i], NULL);
        if (retval != 0)
        {
            printf("FAIL round1: %i\n", i);
            if (retval == ESRCH)
            {
                printf("ESRCH\n");
            }
            if (retval == EDEADLK)
            {
                printf("EDEADLK\n");
            }
            if (retval == EINVAL)
            {
                printf("EINVAL\n");
            }
            return FAIL;
        }
    }
    
    while(a15 != 120);

    return PASS;

}



static int ee;
static void* _crappier_test(void* arg){
    ee = *(int*)arg + 1;
    pthread_exit(0);
}

static void* _crap_test(void* arg){
    pthread_t tid1;
    int trash = *(int*)arg + 1;
    pthread_create(&tid1, NULL,  &_crappier_test, &trash);
    pthread_exit(0);
}

static int test16(void){
    pthread_t tid1; pthread_t tid2;

    int trash = 0;
    pthread_create(&tid1, NULL,  &_crap_test, &trash);

    while(ee != 2); //just wait, failure occurs if there is a timeout
    return PASS;

}


static int crap_count = 0;

static void* _arg_crap(void* arg){
    crap_count++;
    *(int*)arg = *(int*)arg + crap_count;
    pthread_exit(0);
}

static int test17(void){
    pthread_t tid1; pthread_t tid2;

    int arg = 0;
    for(int i = 0; i < 128; i++) {
        pthread_create(&tid1, NULL,  &_arg_crap, &arg);
    }
    while(arg != 8256); //just wait, failure occurs if there is a timeout
    return PASS;

}

static int _stack_crappy(int a){
    if(a == 0) {
        return 1;
    }

    return _stack_crappy(a-1) + _stack_crappy(a-1) + _stack_crappy(a-1) + _stack_crappy(a-1) + _stack_crappy(a-1);
}

static void* _stack_crap(void* arg){
    int lol = *(int*)arg;
    *(int*)arg = _stack_crappy(lol);
    pthread_exit(0);
}


static int test18(void){
    pthread_t tid1; pthread_t tid2;

    int arg = 5;
    pthread_create(&tid1, NULL,  &_stack_crap, &arg);

    while(arg != 3125); //just wait, failure occurs if there is a timeout
    return PASS;

}


static int a19 = 0;
static void* _thread_inc19(void* arg){
    pthread_t tid1;
    int arg2 = 12;
    pthread_create(&tid1, NULL,  &_thread_fcn, &arg2);

    while(arg2 != 144); //just wait, failure occurs if there is a timeout
    a19++;
    pthread_exit(0);

}

static int test19(void){
    pthread_t tid1;

    for(int i = 0; i < 10; i++){
        pthread_create(&tid1, NULL,  &_thread_inc19, NULL);
    }

    while(a19 != 10);

    for(int i = 0; i < 20; i++){
        pthread_create(&tid1, NULL,  &_thread_inc19, NULL);
	// printf("created thread %i\n", i);
    }

    while (a19 != 30);

    return PASS;

}

static void* _thread_init_20(void* arg)
{
    *((int*)arg) = 1;
}

static void* pthread_create_f20(void* arg)
{
    pthread_t tid2;
    pthread_create(&tid2, NULL, &_thread_init_20, arg);
}

static int test20(void)
{
    pthread_t tid1;

    int a0 = 0;
    pthread_create(&tid1, NULL, &pthread_create_f20, &a0);

    while(a0 != 1);
    return PASS;

}

//end of tests
//==============================================================================

//More tests of mine

static int a21 = 0;
static int iterator = 1;
static pthread_t ids[128];
static void* _more_stuff(void* arg){
    a21++;
}

static void* _thread_inc21(void* arg){
    
    pthread_t tid;
    a21++;

    pthread_create(&(tid), NULL,  &_more_stuff, NULL);
    lock();
    ids[iterator] = tid;
    iterator += 2;
    unlock();
    // printf("i: %i, a21: %i\n",i, a21);
    pthread_exit(0);

}

static int test21(void){
    pthread_t tid;
    for(int i = 0; i < 100; i+=2){
        // printf("i: %i\n", i);
        pthread_create(&tid, NULL,  &_thread_inc21, NULL);
        ids[i] = tid;
    // printf("created thread %i\n", i);
    }

    for(int i = 0; i < 100; ++i)
    {
        // printf("ids[%i]: %si\n", i, ids[i]);
        int retval = pthread_join(ids[i], NULL);
        if (retval != 0)
        {
            if (retval == ESRCH)
            {
                printf("ESRCH\n");
            }
            if (retval == EDEADLK)
            {
                printf("EDEADLK\n");
            }
            if (retval == EINVAL)
            {
                printf("EINVAL\n");
            }
            return FAIL;
        }
        // printf("round 1: %i, a21: %i\n",i, a21);
    }
    
    while(a21 != 100);
    // printf("finished first round\n");

    iterator = 1;
    for (int i = 0; i < 128; i+=2){
        pthread_create(&tid, NULL,  &_thread_inc21, &i);
        ids[i] = tid;
    }

    for(int i = 0; i < 128; ++i)
    {
       int retval = pthread_join(ids[i], NULL);
        if (retval != 0)
        {
            printf("FAIL round2: %i\n", i);
            if (retval == ESRCH)
            {
                printf("ESRCH\n");
            }
            if (retval == EDEADLK)
            {
                printf("EDEADLK\n");
            }
            if (retval == EINVAL)
            {
                printf("EINVAL\n");
            }
            return FAIL;
        }
    }
    while(a21 != 228);

    return PASS;

}


// Semaphore tests
//basic errorno test
//==============================================================================
static void* _thread_errno_test(void* arg){
    pthread_exit(0);
}

static int test22(void){
    pthread_t tid1;
        
    pthread_create(&tid1, NULL,  &_thread_errno_test, NULL);
    
    pthread_join(tid1, NULL);
    sleep(1);
    int rtn = pthread_join(tid1, NULL);
    if (rtn != ESRCH)
        return FAIL;
    return PASS;
}


//basic pthread join test
//==============================================================================
static void* _thread_join_test(void* arg){
    sleep(1);
    pthread_exit(0);
}

static int test23(void){
    pthread_t tid1 = 0;
    
    pthread_create(&tid1, NULL,  &_thread_join_test, NULL);
    
    int rtn = pthread_join(tid1, NULL); //failure occurs on a timeout
    if (rtn != 0)
        return FAIL;
    return PASS;
}


//join test 2
//==============================================================================
static int a41;
static int a42;
static void* _thread_slow_join(void* arg){
    sleep(1);
    a42 = 12;
    pthread_exit(0);
}
static void* _thread_fast_join(void* arg){
    a41 = 11;
    pthread_exit(0);
}

static int test24(void){
    pthread_t tid1; pthread_t tid2;
    void *rtn1, *rtn2;
        
    pthread_create(&tid1, NULL,  &_thread_fast_join, NULL);
    pthread_create(&tid2, NULL,  &_thread_slow_join, NULL);
   
    pthread_join(tid2, &rtn2);
    pthread_join(tid1, &rtn1);
    
    if (a41 != 11 || a42 != 12)
        return FAIL;
    return PASS;
}


//join test 3
//==============================================================================
static void* _thread_dummy_loop25(void* arg){
    pthread_t a = pthread_self();
    int *val;
    for(int i = 0; i < 100000; i++);
    
    val  = (int *) malloc(sizeof(int));
    *val = 42;

    return (void *) val;
}

static int test25(void){
    pthread_t tid;
    void *val;
    int i;
    int tids[128];
    
    for (i=0; i<128; i++) {
        pthread_create(&tid, NULL, &_thread_dummy_loop25, NULL);
        tids[i] = tid;
    }

    for (i=0; i<128; i++) {
        pthread_join(tids[i], &val);
        if (*(int*)val != 42)
        {
            return FAIL;
        }
    }

    return PASS;
}


//basic semaphore test
//==============================================================================
static int a26;
static void* _thread_arg26(void* arg){
    sem_t *sem = (sem_t*) arg;

    sem_wait(sem);
    a26 = 20;
    sem_post(sem);
    
    pthread_exit(0);
}

static int test26(void){
    pthread_t tid1;
    sem_t sem;
    int i;

    sem_init(&sem, 0, 1);
    a26 = 10;

    sem_wait(&sem);
    pthread_create(&tid1, NULL,  &_thread_arg26, &sem);

    for (i=0; i<3; i++) {  // sleep for 3 schedules
        sleep(1);
    }
    
    if (a26 != 10)
        return FAIL;
    sem_post(&sem);

    pthread_join(tid1, NULL);
    sem_destroy(&sem);

    return PASS;
}


//semaphore test 2
//==============================================================================
static int a27;
static void* _thread_sem2(void* arg){
    sem_t *sem;
    int local_var, i;

    sem = (sem_t*) arg;

    sem_wait(sem);
    local_var = a27;
    for (i=0; i<5; i++) {
        local_var += pthread_self();
    }
    for (i=0; i<3; i++) {   // sleep for 3 schedules
        sleep(1);
    }
    a27 = local_var;

    sem_post(sem);
    pthread_exit(0);
}

static int test27(void){
    pthread_t tid1, tid2;
    sem_t sem;

    sem_init(&sem, 0, 1);
    a27 = 10;

    pthread_create(&tid1, NULL, &_thread_sem2, &sem);
    pthread_create(&tid2, NULL, &_thread_sem2, &sem);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    sem_destroy(&sem);

    if (a27 != (10 + (tid1*5) + (tid2*5)))
        return FAIL;
    return PASS;

}

//basic lock test
//==============================================================================
static int l;
static void* _thread_lock_test(void* arg){
    lock();
    l = 20;
    unlock();

    pthread_exit(0);
}

static int test28(void){
    pthread_t tid;
    int i;

    pthread_create(&tid, NULL,  &_thread_lock_test, NULL);

    lock(); // need to lock after init is called
    l = 10;
    sleep(1);

    if (l != 10)
        return FAIL;
    unlock();
    
    pthread_join(tid, NULL);

    if (l != 20)
        return FAIL;
    return PASS;
}


// Test error codes

static int test29(void)
{
    // Try to make pshared not 0
    sem_t sem;

    int return_val = sem_init(&sem, 3, 1);
    if (return_val == 0)
    {
        // ENOSYS
        return FAIL;
    }
    return PASS;
}

static int test30(void)
{
    // Try to give a value greater than SEM_MAX_VALUE
    sem_t sem;

    int return_val = sem_init(&sem, 0, 65537);
    if (return_val == 0)
    {
        // EINVAL
        return FAIL;
    }
    return PASS;
}


static int test31(void)
{
    // Try to increment a semaphore value greater than SEM_MAX_VALUE
    sem_t sem;

    sem_init(&sem, 0, 65536);
    int return_val = sem_post(&sem);
    if (return_val == 0)
    {
        // EOVERFLOW
        return FAIL;
    }
    return PASS;
}


// test pthread_join_errors
//==============================================================================

static int test32(void){
    pthread_t tid1 = 0;
        
    int rtn = pthread_join(0, NULL); //failure occurs on a timeout
    if (rtn != EDEADLK)
    {
        return FAIL;
    }
    return PASS;
}


// Testing 2 threads trying to join on the same thread. Get EINVAL error code 

static void* _dummy_thread33(void* arg)
{
    while(1);
}

static void* _thread_multiple_join_test(void* arg){
    pthread_t tid2 = *(pthread_t *)arg;
    
    pthread_join(tid2, NULL);
    pthread_exit(0);
}

static int test33(void){
    pthread_t tid1 = 0;
    pthread_t tid2 = 0;



    pthread_create(&tid2, NULL,  &_dummy_thread33, NULL);
    pthread_create(&tid1, NULL,  &_thread_multiple_join_test, &tid2);
    sleep(1); // You need to sleep the main thread to switch threads and ensure that the other thread calls join first so that this errors out.

    int rtn = pthread_join(tid2, NULL); //failure occurs on a timeout
    if (rtn != EINVAL)
    {
        return FAIL;
    }
    return PASS;
}


// Test two threads trying to join each other. this should give deadlock error code
static void* _thread_join_deadlock_test(void* arg){

    pthread_join(0, NULL);
    pthread_exit(0);
}

static int test34(void){
    pthread_t tid1 = 0;


    pthread_create(&tid1, NULL,  &_thread_join_deadlock_test, NULL);
    sleep(1); // You need to sleep the main thread to switch threads and ensure that the other thread calls join first so that this errors out.

    int rtn = pthread_join(tid1, NULL); //failure occurs on a timeout
    if (rtn != EDEADLK)
    {
        return FAIL;
    }
    return PASS;
}



// Test that if one thread posts and the value is > 0, then the other thread that calls sem_wait will not block

static int a35 = 0;
static int iterator35 = 60;
static pthread_t test35_ids[128];
static void* _test_35_counter(void* arg)
{
    sem_t *sem = (sem_t *)arg;
    int ret_val = sem_wait(sem); 

    // Successful sem_wait returns 0
    if (ret_val == 0)
        a35++;
    pthread_exit(0);
}

static void* _test_35_create60more(void* arg)
{
    sem_t *sem = (sem_t *)arg;
    pthread_t tid1;

    for (int i = 0; i < 60; i++)
    {
        pthread_create(&tid1, NULL,  &_test_35_counter, sem);
        lock();
        test35_ids[iterator35] = tid1;
        ++iterator35;
        unlock();
        sem_post(sem);
    }
    pthread_exit(0);
}

static int test35(void){
    pthread_t tid1;

    sem_t sem;
    sem_init(&sem, 0, 0);

    for (int i = 0; i < 60; i++)
    {
        sem_post(&sem);
    }

    for(int i = 0; i < 60; i++){
        pthread_create(&tid1, NULL,  &_test_35_counter, &sem);
        test35_ids[i] = tid1;
    }
    sleep(1);

    while (a35 != 60); // Timeout will occur

    pthread_create(&tid1, NULL, &_test_35_create60more, &sem);
    pthread_join(tid1, NULL);

    for(int i = 0; i < 120; ++i)
    {
         // printf("ids[%i]: %si\n", i, ids[i]);
        int retval = pthread_join(test35_ids[i], NULL);
        if (retval != 0)
        {
            printf("FAIL round1: %i\n", i);
            if (retval == ESRCH)
            {
                printf("ESRCH\n");
            }
            if (retval == EDEADLK)
            {
                printf("EDEADLK\n");
            }
            if (retval == EINVAL)
            {
                printf("EINVAL\n");
            }
            return FAIL;
        }
    }
    
    while(a35 != 120);

    return PASS;

}



static int a36 = 0;
static pthread_t test36_ids[30];
static void* _test_36_counter(void* arg)
{
    sem_t *sem = (sem_t *)arg;
    int ret_val = sem_wait(sem); 

    // Successful sem_wait returns 0
    if (ret_val == 0)
        a36++;
    pthread_exit(0);
}

// Test nothing should block if semaphore is initialized with a high enough value
static int test36(void){
    pthread_t tid1;

    sem_t sem;
    sem_init(&sem, 0, 30);

    for(int i = 0; i < 30; i++){
        pthread_create(&tid1, NULL,  &_test_36_counter, &sem);
        test36_ids[i] = tid1;
    }
    sleep(1);

    while (a36 != 30); // Timeout will occur

    for(int i = 0; i < 30; ++i)
    {
         // printf("ids[%i]: %si\n", i, ids[i]);
        int retval = pthread_join(test36_ids[i], NULL);
        if (retval != 0)
        {
            printf("FAIL round1: %i\n", i);
            if (retval == ESRCH)
            {
                printf("ESRCH\n");
            }
            if (retval == EDEADLK)
            {
                printf("EDEADLK\n");
            }
            if (retval == EINVAL)
            {
                printf("EINVAL\n");
            }
            return FAIL;
        }
    }

    return PASS;

}

static int test37(void)
{
    // Test what happens when you pass NULL into sem_init, sem_wait, sem_post, and sem_destroy
    // If I pass in NULL into the sem_t parameter, this is not a valid semaphore. 
    // -1 is returned on error
    // 0 is only returned on success
    if ( sem_init(NULL, 0, 30) == 0 || sem_wait(NULL) == 0 || sem_destroy(NULL) == 0 || sem_post(NULL) == 0 )
    {
        return FAIL;
    }
    return PASS;
}


/**
 *  Some implementation details: Main spawns a child process for each
 *  test, that way if test 2/20 segfaults, we can still run the remaining
 *  tests. It also hands the child a pipe to write the result of the test.
 *  the parent polls this pipe, and counts the test as a failure if there
 *  is a timeout (which would indicate the child is hung).
 */
 // #define NUM_TESTS 1
 #define NUM_TESTS 38

static int (*test_arr[NUM_TESTS])(void) = {&test0, &test1, &test2, &test3, &test4, &test5, &test6, &test7, &test8, &test9, &test10, &test11, &test12, &test13, &test14, &test15, &test16, &test17, &test18, &test19, &test20, &test21, &test22, &test23, &test24, &test25, &test26, &test27, &test28, &test29, &test30, &test31, &test32, &test33, &test34, &test35, &test36, &test37};
// static int (*test_arr[NUM_TESTS])(void) = {&test9};


int main(void){

    int status; pid_t pid;
    int pipe_fd[2]; int timeout; struct pollfd poll_fds;
    int score = 0; int total_score = 0;

    int devnull_fd = open("/dev/null", O_WRONLY);

    pipe(pipe_fd);
    poll_fds.fd = pipe_fd[0]; // only going to poll the read end of our pipe
    poll_fds.events = POLLRDNORM; //only care about normal read operations

    for(int i = 0; i < NUM_TESTS; i++){
        score = 0;
        pid = fork();

        //child, launches the test
        if (pid == 0){
            dup2(devnull_fd, STDOUT_FILENO); //begone debug messages
            dup2(devnull_fd, STDERR_FILENO);

            score = test_arr[i]();

            write(pipe_fd[1], &score, sizeof(score));
            exit(0);
        }

        //parent, polls on the pipe we gave the child, kills the child,
        //keeps track of score
        else{

            if(poll(&poll_fds, 1, TEST_WAIT_MILI)){
                read(pipe_fd[0], &score, sizeof(score));
            }

            total_score += score;
            kill(pid, SIGKILL);
            waitpid(pid,&status,0);


            if(score){
                printf("test %i : PASS\n", i);
            }
            else{
                printf("test %i : FAIL\n", i);
            }
        }
    }

    printf("total score was %i / %i\n", total_score, NUM_TESTS);
    return 0;
}
