
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>

typedef struct data
{
    int compteur;
    double real;
}data;

// memoire partage
data * ptr_shared_mem;

// semaphore
int semid;
unsigned short init[] = {1,0};
union semun
{
    int val;
    struct semid_ds * buf;
    unsigned short *array;
}arg;
struct sembuf op;

#define MUTEX 0
#define DEST 1


void operation(int nb)
{
    ptr_shared_mem->real = 45;
    for(int i = 0; i < nb; i++ )
    {
        op.sem_num = MUTEX;
        op.sem_flg = 0;
        op.sem_op = -1;
        if(semop(semid, &op, 1) == -1)
        {
            perror("Erreur sur semop P \n");
            exit(1);
        }

        ptr_shared_mem->compteur++;
        printf("incremente : %d\n",ptr_shared_mem->compteur);

        op.sem_num = MUTEX;
        op.sem_flg = 0;
        op.sem_op = 1;
        if(semop(semid, &op, 1) == -1)
        {
            perror("Erreur sur semop V \n");
            exit(1);
        }

    }

    printf("real: %f\n",ptr_shared_mem->real);
}

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        perror("Need 2 args\n");
        exit(1);
    }

    /*nb d'incrementation/decrementation*/
    int nb = atoi(argv[1]);

    // creation semaphore
    key_t key;
    if((key = ftok("key",8))==-1)
    {
        perror("Erreur sur ftok\n");
        exit(1);
    }

    if((semid = semget(key,2,IPC_CREAT|0777))== -1)
    {
        perror("Erreur sur semget\n");
        exit(1);
    }

    arg.array = init;
    if(semctl(semid,0,SETALL,arg) == -1)
    {
        perror("Erreur sur smctl\n");
        exit(1);
    }


    /*gestion memoire partage*/
    int id = shmget(key,sizeof(data), 0777 | IPC_CREAT);
    if(id == -1)
    {
        perror("Failed to create shared memory segment\n");
        exit(1);
    }

    // On attache le pointeur
    ptr_shared_mem = shmat(id,NULL,0);
    if(ptr_shared_mem == (void *)-1)
    {
        perror("failed to attach with shmat");
        exit(1);
    }
    ptr_shared_mem->compteur = 0;
    operation(nb);

    op.sem_op = -1;
    op.sem_num = DEST;
    op.sem_flg = 0;
    printf("destroy shared\n");
    if(semop(semid, &op, 1) == -1)
    {
        perror("Failed semop on destroying shared memory\n");
        exit(1);
    }

    printf("compteur fin : %d\n", ptr_shared_mem->compteur);
    if(shmdt(ptr_shared_mem)==-1)
    {
        perror("detachement impossible") ;
        exit(1) ;
    }

    if( shmctl(id,IPC_RMID,NULL) == -1)
    {
       perror("Failed to destroy shared mem\n");
       exit(1);
    }
    if(semctl(semid,0,IPC_RMID,0) == -1)
    {
        perror("Failed to destroy semaphore\n");
        exit(1);
    }
    return EXIT_SUCCESS;
}
