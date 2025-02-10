
/*
*/
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
int folder=0;
int count=0;

void list_directory(char* path,int depth){
    DIR *dir=opendir(path);
    if (dir==NULL) return;
    struct dirent *da;

    printf("%*s[FOLDER]  %s \n",depth*4," ",path);
    while((da=readdir(dir))!=NULL){
        if ( strcmp(da->d_name,".")==0 || strcmp(da->d_name,"..")==0){
            continue;
        }
        folder++;
        char buffer[1024];
        snprintf(buffer,sizeof(buffer),"%s/%s",path,da->d_name);
        if (da->d_type==DT_DIR){
            list_directory(buffer,depth+1);
        }
    }

    rewinddir(dir);
    //reading the file;

    while((da=readdir(dir))!=NULL){
        struct stat files;
        if ( strcmp(da->d_name,".")==0 || strcmp(da->d_name,"..")==0){
            continue;
        }else{
            if (da->d_type==DT_REG){
                count++;
                stat(da->d_name,&files);
                printf("%*s[FILE] : %s [SIZE : %lld KB ]\n",(depth+1)*4," ",da->d_name,files.st_size/(1024));

            }
           
        }
    }




}





int main(int argc,char** argv){
    char* path=argv[0];
    list_directory(".",0);
    printf("%*sTotal Files in this Folder: %d\n",(1+2)*4,"",count);
    printf("%*sTotal Folder in this Folder: %d\n",(1)*4,"",folder);
}