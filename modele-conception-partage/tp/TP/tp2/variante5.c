#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>

typedef struct data
{
    int compteur;
    double real;
}data;
data * ptr_shared_mem;

void decremente(int nb)
{
    ptr_shared_mem->real = 45;
    for(int i = 0; i < nb; i++ )
    {
        ptr_shared_mem->compteur--;
        printf("decremente : %d\n",ptr_shared_mem->compteur);
        /*ptr_shared_mem->real = ptr_shared_mem->compteur *100;*/
    }
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

    key_t key;
    if((key = ftok("key",8))==-1)
    {
        perror("Erreur sur ftok\n");
        exit(1);
    }
    /*gestion memoire partage*/
    // creation du segment de memoire partage si il n'existe pas.
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
    decremente(nb);

    if(shmdt(ptr_shared_mem)==-1)
    {
        perror("detachement impossible") ;
        exit(1) ;
    }
    exit(0);

    printf("value: %d\n", ptr_shared_mem->compteur);
    if(shmdt(ptr_shared_mem)==-1)
    {
        perror("detachement impossible") ;
        exit(1) ;
    }

    /*
       if( shmctl(id,IPC_RMID,NULL) == -1)
       {
       perror("Failed to destroy shared mem\n");
       exit(1);
       }
       */

    return EXIT_SUCCESS;
}
