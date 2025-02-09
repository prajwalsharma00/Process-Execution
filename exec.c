#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


int main(){
    char* args[]={"echo","hello","world",0};
    execvp("echo",args);// here echo is the process which can be fined in the unix so our program serach for it , tehn the argss that passes to that process so hello world 
        // is pringted in our terminal 
    printf("execvp failed: ");
}