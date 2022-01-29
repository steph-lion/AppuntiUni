#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#define SEMAFORO_G 0
#define SEMAFORO_P 1
#define SEMAFORO_T 2
typedef struct{
    char P1_Move;
    char P2_Move;
    int winner;
    int currentMatch;
    int exit;
}match;

int WAIT(int sem_des, int num_semaforo) {
    struct sembuf operazioni[1] = {{num_semaforo, -1, 0}};
    return semop(sem_des, operazioni, 1);
}

int SIGNAL(int sem_des, int num_semaforo) {
    struct sembuf operazioni[1] = {{num_semaforo, +1, 0}};
    return semop(sem_des, operazioni, 1);
}
void processoP(int id_process, int id_shm, int id_sem){
    WAIT(id_sem,SEMAFORO_P);
    srand(time(0)+getpid());
    match* indirizzoMemoria;
    if ((indirizzoMemoria=(match*)shmat(id_shm,NULL,0))==(match*)-1){
        printf("Errore in shmat");
        exit(1);
    }
    while(indirizzoMemoria->exit==0){
        int num=rand()%3;
        switch(num){
            case 0:
                printf("[P%d] Mossa 'Carta'\n",id_process);
                if(id_process==1) indirizzoMemoria->P1_Move='C';
                else indirizzoMemoria->P2_Move='C';
                break;
            case 1:
                printf("[P%d] Mossa 'Forbice'\n",id_process);
                if(id_process==1) indirizzoMemoria->P1_Move='F';
                else indirizzoMemoria->P2_Move='F';
                break;
            case 2:
                printf("[P%d] Mossa 'Sasso'\n",id_process);
                if(id_process==1) indirizzoMemoria->P1_Move='S';
                else indirizzoMemoria->P2_Move='S';
                break;
        }
        SIGNAL(id_sem,SEMAFORO_G);
        WAIT(id_sem,SEMAFORO_P);
    }
    SIGNAL(id_sem,SEMAFORO_G);
    exit(0);
}
void processoG(int id_shm,int id_sem){
    WAIT(id_sem,SEMAFORO_G);
    WAIT(id_sem,SEMAFORO_G);
    match* indirizzoMemoria;
    if ((indirizzoMemoria=(match*)shmat(id_shm,NULL,0))==(match*)-1){
        printf("Errore in shmat");
        exit(1);
    }
    while (indirizzoMemoria->exit==0){
        char mossa1=indirizzoMemoria->P1_Move;
        char mossa2=indirizzoMemoria->P2_Move;
        if (mossa1==mossa2){
            printf("[G] Partita N.%d patta, da ripetere.\n",indirizzoMemoria->currentMatch);
            SIGNAL(id_sem,SEMAFORO_P);
            SIGNAL(id_sem,SEMAFORO_P);
        }
        else{
            if ((mossa1=='C' && mossa2=='S') || (mossa1=='S' && mossa2=='F') ||(mossa1=='F') && (mossa2=='C')){
                printf("[G] Partita N.%d vinta da P1\n",indirizzoMemoria->currentMatch);
                indirizzoMemoria->winner=1;
            }
            else{
                printf("[G] Partita N.%d vinta da P2\n",indirizzoMemoria->currentMatch);
                indirizzoMemoria->winner=2;
            }
            SIGNAL(id_sem,SEMAFORO_T);
        }
        WAIT(id_sem,SEMAFORO_G);
        WAIT(id_sem,SEMAFORO_G);
    }
    exit(0);
}
void processoT(int id_sem, int id_shm, int n_matches){
    match* indirizzoMemoria;
    if ((indirizzoMemoria=(match*)shmat(id_shm,NULL,0))==(match*)-1){
        printf("Errore in shmat");
        exit(1);
    }
    int vittorie[2]={0};
    for(int i=1;i<=n_matches;i++){
        indirizzoMemoria->currentMatch=i;
        SIGNAL(id_sem,SEMAFORO_P);
        SIGNAL(id_sem,SEMAFORO_P);
        WAIT(id_sem,SEMAFORO_T);
        vittorie[indirizzoMemoria->winner-1]++;
        printf("[T] Classifica temporanea: P1 = %d | P2 = %d\n",vittorie[0],vittorie[1]);
    }
    printf("[T] Classifica definitiva: P1 = %d | P2 = %d\n",vittorie[0],vittorie[1]);
    indirizzoMemoria->exit=1;
    SIGNAL(id_sem,SEMAFORO_P);
    SIGNAL(id_sem,SEMAFORO_P);
    exit(0);
}
int main(int argc, char* argv[]){
    if (argc<2 || argc>2){
        printf("Il comando giusto e' %s <numero partite>\n",argv[0]);
        exit(0);
    }
    int n_matches=atoi(argv[1]);
    int id_sem;
    int id_shm;
    match* indirizzoMemoria;
    key_t chiave=IPC_PRIVATE;
    if ((id_shm=shmget(chiave,sizeof(match),IPC_CREAT|IPC_EXCL|0600))==-1){
        printf("Errore in shmget\n");
        exit(1);
    }
    if ((id_sem=semget(chiave, 3, IPC_CREAT|IPC_EXCL|0600))==-1){
        printf("Errore in semget");
        exit(1);
    }
    if ((indirizzoMemoria=(match*)shmat(id_shm,NULL,0))==(match*)-1){
        printf("Errore in shmat");
        exit(1);
    }
    semctl(id_sem,SEMAFORO_G,SETVAL,0);
    semctl(id_sem,SEMAFORO_P,SETVAL,0);
    semctl(id_sem,SEMAFORO_T,SETVAL,0);
    if (fork()==0) processoP(1,id_shm,id_sem);
    if (fork()==0) processoP(2,id_shm,id_sem);
    if (fork()==0) processoG(id_shm, id_sem);
    if (fork()==0) processoT(id_sem, id_shm, n_matches);
    
}