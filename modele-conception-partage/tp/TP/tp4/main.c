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


// semaphore
int semid;
union semun
{
    int val;
    struct semid_ds * buf;
    unsigned short *array;
}arg;
struct sembuf op;

#define MUTEX 0
#define DEST 1

short * get_nb(int nb)
{
    unsigned short * init = calloc(nb, sizeof(short));
    init[0] = 1;

    return init;
}

void fils(int id, int nb)
{
    for(int i = 0; i < 20; i++)
    {
        op.sem_num = id;
        op.sem_flg = 0;
        op.sem_op = -1;
        if(semop(semid,&op,1) == -1)
        {
            perror("Error with sem P\n");
            exit(1);
        }
        printf("%d - youhou\n",id);
        op.sem_num = (id+1)%nb;
        op.sem_flg = 0;
        op.sem_op = 1;
        if(semop(semid, &op, 1) ==-1)
        {
            perror("Error with sem V\n");
            exit(1);
        }
    }

}

int main(int argc, char * argv[])
{

    int nb = atoi(argv[1]);
    /*creation du tableau d'init*/
    unsigned short * init = get_nb(nb);

    key_t key ;
    if((key=ftok("key",8))==-1)
    {
        perror("failed ftok\n");
        exit(1);
    }

    /*creation des semaphore (nb sem cree)*/

    if((semid = semget(key,nb,IPC_CREAT|0777))== -1)
    {
        perror("Erreur sur semget\n");
        exit(1);
    }

    /*initialisation des semaphore*/
    arg.array = init;
    if(semctl(semid,0,SETALL,arg) == -1)
    {
        perror("Erreur sur smctl\n");
        exit(1);
    }


    for(int i = 0; i < nb; i++)
    {
        if(!fork())
        {
            fils((nb-1-i),nb);
            exit(0);
        }

    }

    while(wait(NULL) !=-1);

    if(semctl(semid,0,IPC_RMID,0) == -1)
    {
        perror("Failed to destroy sem\n");
        exit(1);
    }

    free(init);

    return EXIT_SUCCESS;

}
