#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define BUFFERSIZE 500
struct data
{
    pthread_mutex_t *mutex;
    int read_current_position;
    int write_current_postion;
    long long maximum_size;
    char *file_name;
    int filedescriptor;
};

void *download(void *args)

{
    printf("IN the download thread \n");
    struct data *threads_data = (struct data *)args;
    char buffer[BUFFERSIZE + 1];

    while (1)
    {
        pthread_mutex_lock(threads_data->mutex);
        if (threads_data->read_current_position >= threads_data->maximum_size)
        {
            pthread_mutex_unlock(threads_data->mutex);
            break;
        }
        off_t current_position = lseek(threads_data->filedescriptor, threads_data->read_current_position, SEEK_SET);
        if (current_position == -1)
        {
            printf("lseek error \n");
            pthread_mutex_unlock(threads_data->mutex);
            break;
        }
        int data = read(threads_data->filedescriptor, buffer, sizeof(buffer));
        if (data <= 0)
        {
            printf("Completed \n");
            pthread_mutex_unlock(threads_data->mutex);
            break;
        }
        threads_data->read_current_position += (data);
        buffer[BUFFERSIZE] = '\0';

        printf("%s \n", buffer);
        pthread_mutex_unlock(threads_data->mutex);
    }
    return NULL;
}

int main()
{
    // maintiann the shared memory for this we will use the file shared,txt
    int filepd = open("shared.txt", O_CREAT | O_RDWR, 0644);
    char uploading_file[1024] = "input.txt";
    // printf("[COMPUTER]: ENTER THE UPLOADING FILE NAME \n ");
    // scanf("%s", uploading_file);
    struct stat a;
    stat(uploading_file, &a);
    long long size = (long long)a.st_size;

    ftruncate(filepd, size);
    char *buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, filepd, 0);
    pthread_mutex_t *processmutex = mmap(NULL, sizeof(pthread_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(processmutex, &attr);

    pid_t uploader = fork();
    if (uploader == 0)
    {

        struct data *data = (struct data *)malloc(sizeof(struct data));

        data->read_current_position = 0;
        data->write_current_postion = 0;

        data->mutex = malloc(sizeof(pthread_mutex_t));
        data->maximum_size = size;
        data->file_name = uploading_file;
        pthread_t threads[2];
        printf("here \n");
        int filepd = open(data->file_name, O_RDONLY, 0644);
        data->filedescriptor = filepd;
        pthread_mutex_init(data->mutex, NULL);
        printf("all teh data have been shared \n");

        for (int i = 0; i < 2; i++)
        {
            pthread_create(&threads[i], NULL, download, data);
        }
        for (int i = 0; i < 2; i++)
        {
            pthread_join(threads[i], NULL);
        }
        close(filepd);
        pthread_mutex_destroy(data->mutex);
        free(data->mutex);
        free(data);
        exit(0);
    }

    pid_t downloader = fork();
    if (downloader == 0)
    {
        // for the donwlaoder

        exit(0);
    }
    wait(NULL);
    wait(NULL);
    // int status;
    // pid_t child_pid;
    // while ((child_pid = waitpid(-1, &status, WNOHANG)) != -1)
    // {
    //     if (child_pid > 0)
    //     {
    //         if (WIFEXITED(status))
    //         {
    //             printf("Exit have been sucessful with exit status %d \n", WEXITSTATUS(status));
    //         }
    //     }
    // }
}

/*
Main Idea:
1st process upload the file :
        READ TEH FILE FROM OUR INPUT.TXT
        UPLOAD IT TO THE SHARED MEMORY SPACE TO BE SHARED WITHT THE TOHER PROCESS
        WE CAN TAKE ADVANATEAGE OF MUTLPLE THREAD TO WRITE THE DATA TO MULTPLE THREAD

2ND PROCESS DOWNLOAD TEH UPLOADED FILE FROM TEH MEMORY SPACE TO THE OUTPUT.TXT
        WE READ TEH FILE
        WE MAKE USE OF 2 THREAD BOTH THREAD READ THE FILE
        WE MAKE SUCH THAT A BUFFER IS GIVEN IN BOTH THREAD IT DOWNLOAD TEH CONTEXT
        INTO BUFFER ADN ADD THEM WHEN THEIR TURNS COMES
//lets code adn figure out along the way

*/