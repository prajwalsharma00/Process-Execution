
/*

🚀 Your Coding Task:
Simulate a Data Race:

Write a program where two processes (not threads) increment the same shared memory variable without using any synchronization.
Observe the unpredictable results.
Fix the Data Race:

Modify your code to prevent the data race using POSIX shared memory and a mutex with PTHREAD_PROCESS_SHARED attribute.
Let me know once you’ve attempted the task! 🎯*/
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<fcntl.h>



int main(){
    int *a=mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    pthread_mutex_t *mutex=mmap(NULL,sizeof(pthread_mutex_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    if (mutex==MAP_FAILED){
        printf("error while giving het mmap\n");
    }
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
    
    pthread_mutex_init(mutex,&attr);



    *a=0;
    pid_t child1=fork();
    if (child1==0){
        for (int i=0;i<100000;i++){
            pthread_mutex_lock(mutex);
            (*a)++;
            pthread_mutex_unlock(mutex);
        }
        printf("The value of a is %d \n",*a);
        exit(0);
    }
    pid_t child2=fork();
    if (child2==0){
        for (int i=0;i<100000;i++){
            pthread_mutex_lock(mutex);
            (*a)++;
            pthread_mutex_unlock(mutex);
        }
        printf("The value of b  is %d \n",*a);
        exit(0);

    }
    wait(NULL);
    wait(NULL);
    printf("the final value of the a after two process change is %d \n",*a);
    pthread_mutex_destroy(mutex);
    pthread_mutexattr_destroy(&attr);
    munmap(a,sizeof(int));
    munmap(mutex,sizeof(pthread_mutex_t));
}



