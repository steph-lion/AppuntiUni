#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define SEMAFORO_R 0
#define SEMAFORO_P 1
#define SEMAFORO_W 2
#define MAX_LEN 64

int WAIT(int sem_des, int num_semaforo) {
    struct sembuf operazioni[1] = {{num_semaforo, -1, 0}};
    return semop(sem_des, operazioni, 1);
}

int SIGNAL(int sem_des, int num_semaforo) {
    struct sembuf operazioni[1] = {{num_semaforo, +1, 0}};
    return semop(sem_des, operazioni, 1);
}
void processoR(int id_sem, int id_shm, char fileName[]){
    WAIT(id_sem, SEMAFORO_R);
    char* indirizzoMemoria;
    if ((indirizzoMemoria=(char*)shmat(id_shm, NULL, 0))==(char*)-1){
        perror("shmat");
        exit(1);
    }
    if(strcmp(fileName,"")==0){ //Nessun file di input
        char riga[MAX_LEN];
        printf("\nInserisci una parola da controllare: ");
        scanf("%s",riga);
        while(strcmp(riga,"exit")!=0){
            strcpy(indirizzoMemoria,riga);
            SIGNAL(id_sem,SEMAFORO_P);
            WAIT(id_sem,SEMAFORO_R);
            printf("\nInserisci una parola da controllare: ");
            scanf("%s",riga);
        }
    }
    else{
        FILE* file;
        file=fopen(fileName,"r");
        while(fgets(indirizzoMemoria,MAX_LEN,file)!=NULL){
            sscanf(indirizzoMemoria,"%[^\n]\n",indirizzoMemoria);
            SIGNAL(id_sem,SEMAFORO_P);
            WAIT(id_sem,SEMAFORO_R);
        }
    }
    strcpy(indirizzoMemoria,"exit");
    SIGNAL(id_sem,SEMAFORO_P);
    exit(0);
}
void processoW(int id_sem, int id_shm, char fileName[]){
    WAIT(id_sem,SEMAFORO_W);
    char* indirizzoMemoria;
    if ((indirizzoMemoria=(char*)shmat(id_shm, NULL, 0))==(char*)-1){
        perror("shmat");
        exit(1);
    }
    if(strcmp(fileName,"")==0){
        while(strcmp(indirizzoMemoria,"exit")!=0){
            printf("[W] La parola %s e' palindroma\n",indirizzoMemoria);
            SIGNAL(id_sem,SEMAFORO_R);
            WAIT(id_sem,SEMAFORO_W);
        }
    }
    else{
        FILE* file;
        if((file=fopen(fileName,"w"))==NULL){
            perror("fopen");
            exit(1);
        }
        while(strcmp(indirizzoMemoria,"exit")!=0){
            fprintf(file,"La parola %s e' palindroma\n",indirizzoMemoria);
            SIGNAL(id_sem,SEMAFORO_R);
            WAIT(id_sem,SEMAFORO_W);
        }
    }
    exit(0);
}
int isPalindrome(char parola[]){
    for (int i=0;i<strlen(parola)/2;i++){
        if (parola[i]!=parola[strlen(parola)-1-i]) return 0;
    }
    return 1;
}
int main(int argc, char* argv[]){

    int id_shm, id_sem;
    key_t chiave=IPC_PRIVATE;
    if ((id_shm=shmget(chiave, MAX_LEN, IPC_CREAT|IPC_EXCL|0600))==-1){
        perror("shmget");
        exit(1);
    }
    if ((id_sem=semget(chiave, 3, IPC_CREAT|IPC_EXCL|0600))==-1){
        perror("semget");
        exit(1);
    }
    semctl(id_sem, SEMAFORO_R, SETVAL,0);
    semctl(id_sem, SEMAFORO_W, SETVAL,0);
    semctl(id_sem, SEMAFORO_P, SETVAL,0);
    char* indirizzoMemoria;
    if ((indirizzoMemoria=(char*)shmat(id_shm, NULL, 0))==(char*)-1){
        perror("shmat");
        exit(1);
    }
    if (argc==1){
        if(fork()==0) processoR(id_sem, id_shm, "");
        if(fork()==0) processoW(id_sem, id_shm, "");
    }
    else if (argc==2){
        if(fork()==0) processoR(id_sem, id_shm, argv[1]);
        if(fork()==0) processoW(id_sem, id_shm, "");
    }
    else{
        if(fork()==0) processoR(id_sem, id_shm, argv[1]);
        if(fork()==0) processoW(id_sem, id_shm, argv[2]);
    }
    SIGNAL(id_sem,SEMAFORO_R);
    WAIT(id_sem,SEMAFORO_P);
    while(strcmp(indirizzoMemoria,"exit")!=0){
        int p=isPalindrome(indirizzoMemoria);
        if (p==1){
            SIGNAL(id_sem,SEMAFORO_W);
            WAIT(id_sem,SEMAFORO_P);
        }
        else{
            SIGNAL(id_sem,SEMAFORO_R);
            WAIT(id_sem,SEMAFORO_P);
        }
    }
    SIGNAL(id_sem,SEMAFORO_W);
    exit(0);
}