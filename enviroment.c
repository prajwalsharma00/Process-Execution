#include<stdio.h>
#include<unistd.h>





int main(){
    pid_t child=fork();
    char* env[]={"gretting=helloworld",NULL};
    char* arg[]={"env",NULL};
    if (child==0){
        execve("/usr/bin/env",arg,env);
    }
    else{
        printf("child have been run sucessfully]\n");
    
    }
   
}