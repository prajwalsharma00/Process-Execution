
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    pid_t child=fork();
    if (child==0){
        execv("/bin/ls",(char*[]){"ls","-l",NULL});
    }else{
        wait(NULL);
        printf("child have been sucesffuly runned\n");
}
    pid_t child2=fork();
    if (child2==0){
        execvp("ls",(char*[]){"ls","-a",NULL});
    }else{
        wait(NULL);
        printf("child have been sucesffuly runned\n");
}

}