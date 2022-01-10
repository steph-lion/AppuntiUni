#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>

#define BUFFER 2048
#define SEMAFORO_PADRE_FIGLI 0
void bidProcess(){

}
int main(int argc, char* argv[]){
    if(argc<3){
        printf("N. argomenti inferiori per far partire il programma");
        exit(0);
    }
    FILE* file;
    char* pathName=argv[1];
    char* indirizzoMemoria;
    key_t chiave=IPC_PRIVATE;
    int id_shm, id_coda, id_sem;
    file=fopen(pathName,"r");
    if((id_shm=shmget(chiave,BUFFER, IPC_CREAT|IPC_EXCL|0600))==-1){
        perror("shmget");
        exit(-1);
    }
    if((indirizzoMemoria=(char*)shmat(id_shm,NULL,0))==(char*)-1){
        perror("shmat");
        exit(-1);
    }
    if((id_coda=msgget(chiave, IPC_CREAT|IPC_EXCL|0600))==-1){
        perror("msgget");
        exit(-1);
    }
    if((id_sem=semget(chiave,1,IPC_CREAT|IPC_EXCL|0600))==-1){
        perror("semget");
        exit(-1);
    }
    semctl(id_sem,SEMAFORO_PADRE_FIGLI,SETVAL,0);
    int numBidders(atoi(argv[2]));
    for (int i=0;i<numBidders;i++){
        char name[3];
        snprintf(name,3,"B%d",i);
        if (fork()==0) bidProcess(name, id_shm, id_coda, id_sem);
    }
    while(fgets(indirizzoMemoria,BUFFER,file)!=NULL){
        
    }


    /*char* prova=malloc(2048);
    strcpy(prova,"ciao,393,384");
    char* first_part=strtok(prova,",");
    char* second_part=strtok(NULL,",");
    char* third_part=strtok(NULL,",");
    printf("L'output è: %s %s %s",first_part,second_part,third_part);*/
}