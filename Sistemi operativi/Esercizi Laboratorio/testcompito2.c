//COMPITO DEL 28 SETTEMBRE 2021 FUNZIONANTE
#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>
#include <wait.h>
#define BUFFER 2048
#define SEMAFORO_R_FIGLI 0
#define SEMAFORO_R_O 1

typedef struct{
    long type;
    char text[4];
}msg;
int WAIT(int sem_des, int num_semaforo){
    struct sembuf operazioni[1] = {{num_semaforo,-1,0}};
    return semop(sem_des, operazioni, 1);
}
int SIGNAL(int sem_des, int num_semaforo){
struct sembuf operazioni[1] = {{num_semaforo,+1,0}};
return semop(sem_des, operazioni, 1);
}
void processoR(FILE* f, int shm, int sem, int n_proc, int vettorePipeO[2], int vettorePipeR[2], int dsCoda){
    close(vettorePipeR[0]);
    close(vettorePipeO[1]);
    char* indirizzoMemoria;
    msg message;
    if ((indirizzoMemoria=(char*)shmat(shm,NULL,0))==(char*)-1){
        perror("shmat");
        exit(-1);
    }
    int contatore=1;
    while(fgets(indirizzoMemoria,BUFFER,f)!=NULL){
        printf("[R] Riga da analizzare n.%d: %s\n",contatore++,indirizzoMemoria);
        int positive=0;
        for (int i=0;i<n_proc;i++) SIGNAL(sem, SEMAFORO_R_FIGLI);
        for (int i=0;i<n_proc;i++){
            if (msgrcv(dsCoda, &message, BUFFER,0,0)==-1){
                perror("msgrcv");
                exit(-1);
            }
            if (strcmp(message.text,"t")!=0) positive=1;
        }
        if (positive==0){
            write(vettorePipeR[1], indirizzoMemoria, BUFFER);
            SIGNAL(sem,SEMAFORO_R_O);
            WAIT(sem,SEMAFORO_R_O);
        }
    }
    strcpy(indirizzoMemoria,"exit");
    for (int i=0;i<n_proc;i++) SIGNAL(sem, SEMAFORO_R_FIGLI);
    write(vettorePipeR[1], "exit", BUFFER);
    SIGNAL(sem,SEMAFORO_R_O);
    exit(0);
}
void processoW(int shm, int sem, char* word, int dsCoda){
    char* indirizzoMemoria;
    msg message;
    message.type=1;
    if ((indirizzoMemoria=(char*)shmat(shm, NULL,0 ))==(char*)-1){
        perror("shmat");
        exit(-1);
    }
    WAIT(sem, SEMAFORO_R_FIGLI);
    while(strcmp(indirizzoMemoria, "exit")!=0){
        if (strcasestr(indirizzoMemoria, word)!=NULL){
            strcpy(message.text,"t");
            msgsnd(dsCoda, &message, 1+sizeof(char),0);
        }
        else{ 
            strcpy(message.text,"f");
            msgsnd(dsCoda, &message, 1+sizeof(char),0);
        }
        WAIT(sem,SEMAFORO_R_FIGLI);
    }
    exit(0);
}
void processoO(int sem, int vettorePipeO[2],int vettorePipeR[2]){
    close(vettorePipeR[1]);
    close(vettorePipeO[0]);
    WAIT(sem, SEMAFORO_R_O);
    char* output=malloc(BUFFER);
    read(vettorePipeR[0], output, BUFFER);
    while(strcmp(output,"exit")!=0){
        if (strcmp(output,"nothing")!=0)printf("[O]La ricerca ha trovato una corrispondenza nelle parole: %s\n", output);
        SIGNAL(sem,SEMAFORO_R_O);
        WAIT(sem, SEMAFORO_R_O);
        read(vettorePipeR[0], output, BUFFER);
    }
    exit(0);
}
int main(int argc, char *argv[]){
    FILE* file;
    key_t chiave=IPC_PRIVATE;
    int id_shm, id_sem;
    char* pathName="testcompito2.txt";
    char* indirizzoMemoria;
    int vettorePipeR[2];
    int vettorePipeO[2];
    int dsCoda;
    if (pipe(vettorePipeR) == -1) {
        perror("pipe");
        exit(1);
    }
    if (pipe(vettorePipeO) == -1) {
        perror("pipe");
        exit(1);
    }
    if((id_shm=shmget(chiave, BUFFER, IPC_CREAT |0600))==-1){
        perror("shmget");
        exit(-1);
    }
    if ((id_sem=semget(chiave, 2, IPC_CREAT|IPC_EXCL | 0600))==-1){
        perror("semget");
        exit(-1);
    }
    if((dsCoda=msgget(chiave, IPC_CREAT | IPC_EXCL | 0660))==-1){
        perror("msgget");
        exit(1);
    }
    semctl(id_sem, SEMAFORO_R_FIGLI, SETVAL,0);
    semctl(id_sem,SEMAFORO_R_O, SETVAL,0);
    file=fopen(pathName,"r");
    for (int i=1;i<argc;i++){
        if (fork()==0) processoW(id_shm, id_sem, argv[i], dsCoda);
    }
    if (fork()==0) processoR(file, id_shm, id_sem, argc-1, vettorePipeO,vettorePipeR, dsCoda);
    if (fork()==0) processoO(id_sem, vettorePipeO,vettorePipeR);
    wait(NULL);
    printf("Programma finito\n");
}