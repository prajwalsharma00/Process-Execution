#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<sys/mman.h>
#include<stdlib.h>
#include<sys/stat.h>

#define FILEPATH "testfile.txt"
#define FILESIZE 4096
int main(){
    int fd;
    char *mapped;
    fd=open(FILEPATH,O_RDWR|O_CREAT,0666);
    if (fd==0-1){
        perror("Error in opening the file\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd,FILESIZE)==-1){
        perror("Error in resizing the file\n");

    }
    //mapped file to the memory 
    mapped=mmap(NULL,FILESIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if (mapped==MAP_FAILED){
        perror("erro inmapping the file\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    strcpy(mapped,"hello mmap!, this is written direcly to the file");
    
    dup2(fd,STDOUT_FILENO);
    printf("printing in the file direclty");

    if (munmap(mapped,FILESIZE)==-1){
        perror("eroro \n");
    }
    close(fd);

}

// nmap(address ,lengtth tomap, protmemory pretct liek read,wiret,exec, flag mshareprivatepublic 
// file to be map , off_t offset startin point in file from where we want to )