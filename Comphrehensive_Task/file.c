#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#define BUFFERSIZE 10
struct data // share teh infomraiotn betweent hte threads to downlaod the data from the file to the shared part of the memory  and show in the buffer
{
    pthread_mutex_t *mutex;
    int read_current_position;
    int write_current_postion;
    long long maximum_size;
    char *file_name;
    int writingfiledescriptor;
    int filedescriptor;
    char *sharedmemory;
    pthread_cond_t *cond;
};
int *done = 0;
struct upload_data // to share teh data between the threads uploading the data from teh shared buffer
{
    char *buffer;
    int writing_position;
    int file_descriptor;
    char *outputfilename;
    long long maximum_size;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
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
            *done = 1;
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
        int data = read(threads_data->filedescriptor, buffer, BUFFERSIZE);
        buffer[BUFFERSIZE] = '\0';
        if (data <= 0)
        {
            printf("Completed \n");
            pthread_mutex_unlock(threads_data->mutex);
            break;
        }
        strcpy(threads_data->sharedmemory + threads_data->read_current_position, buffer);
        pthread_cond_broadcast(threads_data->cond);
        threads_data->read_current_position += (data);
        pthread_mutex_unlock(threads_data->mutex);
    }

    return NULL;
}

void *upload(void *args)
{
    printf("int eh uplaod area \n");

    struct upload_data *datas = (struct upload_data *)args;
    while (1)
    {
        pthread_mutex_lock(datas->mutex);
        printf("here is the value \n");
        if (datas->writing_position >= (int)datas->maximum_size)
        {
            pthread_mutex_unlock(datas->mutex);
            break;
        }
        printf("here is the value 1\n");
        while (!done)
        {
            printf("IN the wait conditon of the functio \n  and writing postion = %d \n", datas->writing_position);
            pthread_cond_wait(datas->cond, datas->mutex);
        }
        printf("here is the value 71\n");
        printf("datas->writing positon : %d , Maximum size %d \n", datas->writing_position, (int)datas->maximum_size);
        off_t wposition = lseek(datas->file_descriptor, datas->writing_position, SEEK_SET);
        if (wposition == -1)
        {
            printf("Erorr in getting the lseek \n");
        }
        printf("here isd the value 1\n");
        char buffer[11];
        int remaining = datas->maximum_size - datas->writing_position;
        int copy_size = (remaining < 10) ? remaining : 10;
        strncpy(buffer, datas->buffer + datas->writing_position, copy_size);
        buffer[10] = '\0';
        printf("The buffer is -- %s \n", buffer);

        int data_written = write(datas->file_descriptor, buffer, copy_size);
        printf("datas->writing positon : %d , Maximum size %d \n", datas->writing_position, (int)datas->maximum_size);

        if (data_written == -1)
        {
            pthread_mutex_unlock(datas->mutex);
            break;
        }
        datas->writing_position += copy_size;
        printf("datas->writing positon : %d , Maximum size %d \n", datas->writing_position, (int)datas->maximum_size);
        printf("here is the value 1\n");
        pthread_mutex_unlock(datas->mutex);
    }

    return NULL;
}
int main()
{
    // maintiann the shared memory for this we will use the file shared,txt
    printf("hello how are ou \n");
    int filepd = open("shared.txt", O_CREAT | O_RDWR, 0644);
    if (filepd == -1)
    {
        printf("error in creatirng the file \n");
    }
    char uploading_file[1024] = "input.txt";
    struct stat a;
    stat(uploading_file, &a);
    long long size = (long long)a.st_size;

    ftruncate(filepd, size);
    char *buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, filepd, 0);
    pthread_mutex_t *processmutex = mmap(NULL, sizeof(pthread_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    pthread_cond_t *processcond = mmap(NULL, sizeof(pthread_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    done = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(processmutex, &attr);
    pthread_cond_init(processcond, NULL);

    pid_t downloader = fork();
    if (downloader == 0)
    {

        struct data *data = (struct data *)malloc(sizeof(struct data));

        data->read_current_position = 0;
        data->write_current_postion = 0;

        data->mutex = processmutex;
        data->maximum_size = size;
        data->file_name = uploading_file;
        data->sharedmemory = buffer;
        data->cond = processcond;
        pthread_t threads[2];
        printf("here \n");
        int filepd = open(data->file_name, O_RDONLY, 0644);
        data->filedescriptor = filepd;
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
        free(data);
        exit(0);
    }
    pid_t uploader = fork();
    if (uploader == 0)

    {

        struct upload_data *datas = (struct upload_data *)malloc(sizeof(struct upload_data));
        datas->outputfilename = "output.txt";
        datas->writing_position = 0;
        datas->mutex = processmutex;
        datas->cond = processcond;
        datas->buffer = buffer;
        int filedescritpor = open("output.txt", O_CREAT | O_RDWR, 0666);
        printf("the file descritpor value is correct %d\n", filedescritpor);
        datas->maximum_size = size;
        datas->file_descriptor = filedescritpor;
        pthread_t threads[3];
        printf("here is here \n");
        for (int i = 0; i < 3; i++)

        {
            if ((pthread_create(&threads[i], NULL, upload, datas)) != 0)
            {
                printf("Error in creatinhg the threads\n");
            }
        }
        for (int i = 0; i < 3; i++)
        {
            if ((pthread_join(threads[i], NULL)) != 0)

            {
                printf("Error in joining the threads\n");
            }
        }

        close(filedescritpor);
    }

    wait(NULL);
    wait(NULL);
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