#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctype.h>

#define BUFFER 2048
#define SEMAFORO_PADRE_FIGLI 0
#define SEMAFORO_S_PADRE 1
int WAIT(int sem_des, int num_semaforo) {
    struct sembuf operazioni[1] = {{num_semaforo, -1, 0}};
    return semop(sem_des, operazioni, 1);
}

int SIGNAL(int sem_des, int num_semaforo) {
    struct sembuf operazioni[1] = {{num_semaforo, +1, 0}};
    return semop(sem_des, operazioni, 1);
}
typedef struct {
    long mtype;
    char mtext[4];
} msg;

void contaLettera(int i, int id_shm, int id_sem,int id_coda){
    msg messaggio;
    int cont=0;
    char lettera=97+i;
    char *indirizzo_memoria;
    messaggio.mtype=1;
    if ((indirizzo_memoria = (char *)shmat(id_shm, NULL, SHM_RDONLY))==(char*)-1){
        perror("shmat");
        exit(1);
    };
    WAIT(id_sem,SEMAFORO_PADRE_FIGLI);

    while(strcmp(indirizzo_memoria,"exit")!=0){
        int contatore=0;
        for (int i=0;i<strlen(indirizzo_memoria);i++){
            if (lettera==tolower(indirizzo_memoria[i])) contatore++;
        }
        char lettere[1];
        lettere[0]=lettera;
        sprintf(messaggio.mtext,"%c=%d ",toupper(lettere[0]),contatore);
        if (msgsnd(id_coda,&messaggio,strlen(messaggio.mtext)+1,IPC_NOWAIT)==-1){
            perror("msgsend");
            exit(1);
        }
        WAIT(id_sem,SEMAFORO_PADRE_FIGLI);
    }
    exit(0);
}
void processoS(int ds_coda, int id_sem, int riga){
    int contaLettereTotali[26]={0};
    msg messaggio;
    char messaggioFinale[2048];
    strcpy(messaggioFinale,"");
    for (int i=0;i<26;i++){
        if (msgrcv(ds_coda,&messaggio, BUFFER,0,0)==-1){
            perror("msgrcv");
            exit(1);
        }

        strcat(messaggioFinale,messaggio.mtext);
    }
    printf("[S] riga n.%d: %s\n",riga,messaggioFinale);
    SIGNAL(id_sem,SEMAFORO_S_PADRE);
    exit(0);
}

int main(int argc, char *argv[]){
    FILE* fileDescriptor;
    key_t chiave=IPC_PRIVATE;
    int id_shm, id_sem;
    int descrittoreCoda, contarighe=0;
    char* fileName="testcompito.txt";
    char *indirizzo_memoria;
    char* riga;
    fileDescriptor=fopen(fileName,"r");
    if((descrittoreCoda=msgget(chiave, IPC_CREAT | IPC_EXCL | 0660))==-1){
        perror("msgget");
        exit(1);
    }; //Coda di messaggi condivisa tra L(i) e S
    if ((id_sem = semget(chiave, 2, IPC_CREAT|IPC_EXCL|0600))==-1){
        perror("semget");
        exit(1);
    }; //Creo i due semafori
    if ((id_shm =shmget(chiave, BUFFER, IPC_CREAT | 0600))==-1){
        perror("shmget");
        exit(1);
    }; //Area memoria condivisa tra L(i) e processo padre P
    if ((indirizzo_memoria = (char *)shmat(id_shm, NULL, 0))==(char*)-1){
        perror("shmat");
        exit(1);
    };
    semctl(id_sem, SEMAFORO_PADRE_FIGLI, SETVAL, 0);
    semctl(id_sem, SEMAFORO_S_PADRE, SETVAL,26);

    //printf("Il file %s e' stato aperto con descrittore di file %d.\n", fileName,fileDescriptor);

    for (int i=0;i<26;i++){
        if (fork()==0) contaLettera(i,id_shm,id_sem,descrittoreCoda);
    }
    while(fgets(indirizzo_memoria,BUFFER,fileDescriptor)!=NULL){
        contarighe++;
        if (fork()==0) processoS(descrittoreCoda, id_sem, contarighe); // Processo S in esecuzione
        printf("[P] riga n.%d: %s\n", contarighe, indirizzo_memoria);
        for (int i=0;i<26;i++) SIGNAL(id_sem,SEMAFORO_PADRE_FIGLI);
        WAIT(id_sem,SEMAFORO_S_PADRE);
    }
    strcpy(indirizzo_memoria,"exit");
    for (int i=0;i<26;i++) SIGNAL(id_sem,SEMAFORO_PADRE_FIGLI);
    wait(NULL);
    shmctl(id_shm,IPC_RMID,NULL);
    exit(0);
}