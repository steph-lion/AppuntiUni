#include <stdio.h>
#include <stdlib.h>
int main(){
    FILE* fileDescriptor;
    fileDescriptor=fopen("testcompito.txt","r");
    char *buffer=malloc(2048);
    while(fgets(buffer,2048,fileDescriptor)){
        printf("%s",buffer);
    }
}