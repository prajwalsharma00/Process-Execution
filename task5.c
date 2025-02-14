/*
🧩 First Task (Multithreading Basics):
Task:

Create a program with 5 threads, each thread should:

Print its thread ID.
Sleep for a random time.
Print a completion message.
The main thread should wait for all threads to finish before exiting.

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
volatile int order = 0;
int current_working_thread = -1;
time_t thread_time;
int thread_completed = 0;
pthread_t threads[5];

void cleaner(void *args)
{
    printf("THREAD HAVE BEEN CANCELLED \n");
    pthread_mutex_unlock(&mutex);
}

void *thread_function(void *args)
{
    pthread_mutex_lock(&mutex);
    int *a = (int *)args;
    int value = *a;
    current_working_thread = value;
    thread_time = time(NULL);
    pthread_cleanup_push(cleaner, NULL);

    // while (order != value)
    // {
    //     pthread_cond_wait(&cond, &mutex);
    // }
    int random = rand() % 5;
    printf("slelp for %d \n", random);
    sleep(random);

    printf("[THREAD NO: %d THREAD ID :%lu]: THE TASK HAVE BEEN COMPLETED \n", *a, (unsigned long)pthread_self());
    free(a);
    order++;
    thread_completed++;

    // pthread_cond_signal(&cond);
    pthread_cleanup_pop(0);
    pthread_mutex_unlock(&mutex);
    return NULL;
}
void *check(void *args)

{
    time_t current_time = time(NULL);
    while (thread_completed != 5)
    {
        if (current_working_thread < 0)
        {
        }
        else
        {
            if ((current_time - thread_time) > 1)
            {
                printf("[THREAD_CANCEL ] : THREAD NUMBER %d have been cancelled\n ", current_working_thread);
                thread_completed++;
                pthread_cancel(threads[current_working_thread]);
            }
            sleep(1);
            current_time = time(NULL);
        }
    }
    return NULL;
}

int main()

{
    srand(time(NULL));

    for (int i = 0; i < 5; i++)
    {
        int *a = (int *)malloc(sizeof(int));
        *a = i;
        if ((pthread_create(&threads[i], NULL, thread_function, a)) == -1)
        {
            fprintf(stdout, "[ERROR]: Error in Creating the Thread \n");
        }
    }
    pthread_t check_time;
    pthread_create(&check_time, NULL, check, NULL);

    for (int i = 0; i < 5; i++)
    {
        if ((pthread_join(threads[i], NULL)) == -1)
        {
            fprintf(stdout, "[ERROR]: Error in Joining the Thread \n");
        }
    }
    pthread_join(check_time, NULL);
    fprintf(stdout, "[PARENT THREAD %lu ] : ALL THREAD HAVE COMPLETED THEIR TASK \n", (unsigned long)pthread_self());
}
