#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>

#define FILE "memory.bin"

int main()
{
    int fd = open(FILE, O_CREAT | O_RDWR, 0644);
    if (fd > 0)
    {
        fprintf(stdout, "FIle have been correclty created\n");
    }
    if ((ftruncate(fd, sizeof(int))) == -1)
    {
        printf("error in turncsating the file \n");
    };
    int *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        fprintf(stdout, "Error in creaing the memory \n");
    }
    *ptr = 0;

    pthread_mutex_t *mutex = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mutex == MAP_FAILED)
    {
        fprintf(stdout, "Error inc reating the mutex\n");
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    printf("here this \n");
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(mutex, &attr);

    pid_t child = fork();
    if (child == 0)

    {
        printf("int he cihlhd\n");
        // child process
        // we increase teh value of the ptr which is sotre in our file
        //  we increase from here and read from teh parent
        for (int i = 0; i < 10; i++)
        {
            pthread_mutex_lock(mutex);
            (*ptr)++;
            fprintf(stdout, "[CHILD] I HAVE INCREASED THE VALUE OF COUNTER TO %d\n", *ptr);
            pthread_mutex_unlock(mutex);
            usleep(10000);
        }
    }
    else
    {
        for (int i = 0; i < 10; i++)
        {
            usleep(10000);
            pthread_mutex_lock(mutex);
            fprintf(stdout, "[PARENT: ] I HAVE READ TEH VALUE OF COUTNER TO %d\n", *ptr);
            pthread_mutex_unlock(mutex);
        }
    }

    munmap(mutex, sizeof(pthread_mutex_t));
    munmap(ptr, sizeof(int));
    close(fd);
}