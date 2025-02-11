
#include<stdio.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<string.h>
#include<sys/mman.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<pthread.h>
/*Modify the code to:
Add two child processes reading the counter.
Introduce random delays using usleep() to simulate real-world asynchronous behavior.
Bonus: Implement a simple mutex using pthread_mutex_t (with PTHREAD_PROCESS_SHARED attribute) to prevent race conditions when reading/writing the counter.*/
int main(){
    int *a=mmap(0,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    pthread_mutex_t *mutex=mmap(0,sizeof(pthread_mutex_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(mutex,&attr);




    *a=0;
    pid_t child=fork();
    
    if (child==0){
        for (int i=0;i<1000;i++){

            pthread_mutex_lock(mutex);
            printf("the avlue have been increased to by process 1\n %d ",++(*a));
            pthread_mutex_unlock(mutex);
        }
        exit(0);
    }
    pid_t child2=fork();
    if (child2==0){
        for (int i=0;i<1000;i++){
            pthread_mutex_lock(mutex);
            printf("the value have been increased %d \n",++(*a));
            
            pthread_mutex_unlock(mutex);
        }
        exit(0);
    }

    wait(NULL);
    wait(NULL);
    for (int i=0;i<10;i++){
            pthread_mutex_lock(mutex);
            printf("the value of a is %d \n",*a);
            pthread_mutex_unlock(mutex);

        }

    pthread_mutex_destroy(mutex);
    munmap(a,sizeof(int));
}