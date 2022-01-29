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

typedef struct{
    long mtype;
    char lettera;
    int contLettera;
}msg;

void contaLettera(int id_shm, int id_coda, int id_sem, int i){
    WAIT(id_sem,SEMAFORO_PADRE_FIGLI);
    msg messaggio;
    messaggio.mtype=1;
    char lettera=97+i;
    messaggio.lettera=lettera;
    char* indirizzoMemoria;
    if ((indirizzoMemoria=(char*)shmat(id_shm,NULL,0))==(char*)-1){
        perror("shmat figli");
        exit(1);
    }
    int cont;
    while(strcmp(indirizzoMemoria,"exit")!=0){
        cont=0;
        for(int j=0;j<strlen(indirizzoMemoria);j++){
            if (lettera==tolower(indirizzoMemoria[j])) cont++;
        }
        messaggio.contLettera=cont;
        if((msgsnd(id_coda, &messaggio, sizeof(msg)-sizeof(long),0))==-1){
            perror("msgsend");
            exit(1);
        }
        WAIT(id_sem,SEMAFORO_PADRE_FIGLI);
    }
    
    exit(0);

}
void processoS(int id_coda, int id_sem){
    int contaLettere[26]={0};
    msg messaggio;
    for (int i=0;i<26;i++){
        printf("Ci entro\n");
        if ((msgrcv(id_coda, &messaggio,sizeof(msg)-sizeof(long),0,0))==-1){
            perror("msgrcv");
            exit(1);
        }
        printf("%c=%d",toupper(messaggio.lettera),messaggio.contLettera);
        contaLettere[messaggio.lettera-97]+=messaggio.contLettera;
    }
    printf("\n");
    SIGNAL(id_sem,SEMAFORO_S_PADRE);
    exit(0);
}
int main(int argc, char* argv[]){
    char* fileName=argv[1];
    key_t chiave=IPC_PRIVATE;
    FILE* file;
    char* indirizzoMemoria;
    int id_coda;
    int id_sem;
    int id_shm;
    if ((id_shm=shmget(chiave, BUFFER, IPC_CREAT|IPC_EXCL|0600))==-1){
        perror("shmget");
    }
    if((indirizzoMemoria=(char*)shmat(id_shm, NULL,0))==(char*)-1){
        perror("shmat");
    }
    if((id_sem=semget(chiave, 2, IPC_CREAT|IPC_EXCL|0600))==-1){
        perror("semget");
    }
    if (id_coda=msgget(chiave, IPC_CREAT|IPC_EXCL|0600)==-1){
        perror("msgget");
    }
    semctl(id_sem,SEMAFORO_PADRE_FIGLI,SETVAL,0);
    semctl(id_sem,SEMAFORO_S_PADRE,SETVAL,0);
    file=fopen(fileName,"r");
    for (int i=0;i<26;i++){
        if (fork()==0) contaLettera(id_shm, id_coda, id_sem, i);
    }
    
    while (fgets(indirizzoMemoria,BUFFER,file)!=NULL){
        printf("[P] Riga analizzata: %s\n",indirizzoMemoria);
        for (int i=0;i<26;i++) SIGNAL(id_sem,SEMAFORO_PADRE_FIGLI);
        if (fork()==0) processoS(id_coda, id_sem);
        WAIT(id_sem,SEMAFORO_S_PADRE);
    }
    strcpy(indirizzoMemoria,"exit"); //Soluzione sporca per far capire ai figli quando fermarsi. Soluzione più pulita altrimenti?
    for (int i=0;i<26;i++) SIGNAL(id_sem,SEMAFORO_PADRE_FIGLI);
    exit(0);
}