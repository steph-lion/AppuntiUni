#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
void processoFiglio(int ds[2]){
    FILE* pipeR;
    close(ds[1]);
    pipeR=fdopen(ds[0],"r");
    char riga[1024];
    fgets(riga,1024,pipeR);
    printf("[1] %s\n",riga);
    exit(0);
}
int main(int argc, char* argv[]){
    int dsPipe[2];
    if ((pipe(dsPipe))==-1){
        perror("pipe");
        exit(1);
    }
    FILE* pipeW;
    if (fork()==0) processoFiglio(dsPipe);
    pipeW=fdopen(dsPipe[1],"w");
    close(dsPipe[0]);
    fprintf(pipeW,"Ciao\n");
    printf("[0] Messaggio inviato.\n");
}