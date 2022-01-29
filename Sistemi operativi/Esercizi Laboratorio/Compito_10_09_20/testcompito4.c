//Compito 10 settembre 2020. Non funzionante a causa di dubbi su come formattare bene la parola presa da fgets

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <ctype.h>

void processoR(int fd[2], char* fileName){
    FILE* fpipeR_W;
    close(fd[0]);
    fpipeR_W=fdopen(fd[1],"w");
    struct stat sb;
    char* mappatura_file;
    int fileDescriptor;
    if ((fileDescriptor=open(fileName,O_RDONLY))==-1){
        perror("open");
        exit(1);
    }
    if (fstat(fileDescriptor, &sb) == -1) {
        perror("fstat");
        exit(1);
    }
    if ((mappatura_file=mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fileDescriptor, 0))==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
    close(fileDescriptor);
    fprintf(fpipeR_W,"%s",mappatura_file);
    fclose(fpipeR_W);
    exit(0);
}
void processoW(int fd[2]){
    FILE* fPipeP_R;
    close(fd[1]);
    fPipeP_R=fdopen(fd[0],"r");
    char riga[255];
    int i=0;
    while(fgets(riga,255,fPipeP_R)) printf("[%d]: %s",++i,riga);
    fclose(fPipeP_R);
    exit(0);
}
int isPalindrome(char* word){
    int len=strlen(word)-2;
    printf("N. c= %d\n",len);
    for (int i=0;i<strlen(word)/2;i++){
        printf("Sto comparando %c e %c \n", word[i], word[len-i]);
        if (tolower(word[i])!=tolower(word[len-i])) return 1;
    }
    printf("E' palindroma: %s\n",word);
    return 0;
}
int main(int argc, char* argv[]){
    FILE* fPipeR_R;
    FILE* fPipeP_W;
    int pipeR[2],pipeP[2];
    char riga[255];
    if ((pipe(pipeP))==-1){
        perror("pipe");
        exit(1);
    }
    if ((pipe(pipeR))==-1){
        perror("pipe");
        exit(1);
    }
    if (fork()==0) processoW(pipeP);
    if (fork()==0) processoR(pipeR, argv[1]);
    close(pipeR[1]);
    close(pipeP[0]);
    fPipeR_R=fdopen(pipeR[0],"r");
    fPipeP_W=fdopen(pipeP[1],"w");
    while(fgets(riga,255,fPipeR_R)!=NULL){
        for (int i=0;i<strlen(riga);i++) printf("Carattere: %c\n",riga[i]);
        int p=isPalindrome(riga);
        if (p==0)
            fprintf(fPipeP_W,"%s\n",riga);
    }
    fclose(fPipeR_R);
    fclose(fPipeP_W);
    exit(0);
}