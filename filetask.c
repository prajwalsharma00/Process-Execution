/*
Task:

Create a program that:
Opens a file called "logfile.txt".
Writes 10 random numbers (one per line).
Closes and reopens the file.
Reads and displays the numbers from the file.
Bonus Challenge: Use lseek() to read numbers in reverse order.


*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#define FILE "logfile.txt"
#include <time.h>
#include <sys/types.h>
void write_file()
{
    int fd = open(FILE, O_CREAT | O_WRONLY, 0644);
    int count = 0;
    while (count < 10)
    {
        int random = rand() % 1000;
        char nextlin = '\n';
        int byte_written = write(fd, &random, sizeof(random));
        int char_written = write(fd, &nextlin, sizeof(nextlin));
        if (byte_written == 4 && char_written == 1)
        {
            printf("the byte have been written corrctly for the %d process \n", count);
        }
        count++;
    }
    printf("All teh date have been written correclty to the file \n");
    close(fd);
}

void readfile()
{
    int filepd = open(FILE, O_RDONLY, 0644);
    if (filepd > 2)
    {
        printf("FILE HAVE BEEN OPENED CORRECTLY\n");
    }
    else
    {
        printf("FILE COULDNOT BE OPEND\n");
    }

    // for reerse order
    int lseek_count = -5;
    lseek(filepd, lseek_count, SEEK_END);
    printf("the current positon is %lld", lseek(filepd, 0, SEEK_CUR));
    int data;
    while ((read(filepd, &data, sizeof(data))) > 0)
    {
        printf("%d\n", data);
        lseek_count -= 5;
        if ((lseek(filepd, lseek_count, SEEK_END)) == -1)
        {
            break;
        };
        data = 0;
    }

    // to print the data in correct order
    // int data;
    // while ((read(filepd, &data, sizeof(data))) != 0)
    // {
    //     printf("the value of teh written terms are %d \n", data);
    //     lseek(filepd, 1, SEEK_CUR);
    // }
}
int main()
{
    srand(time(NULL));
    // write_file();
    readfile();
}