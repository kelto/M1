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

// memoire partage
data * ptr_shared_mem;



void incremente(int nb)
{
    for(int i = 0; i < nb; i++ )
    {
        ptr_shared_mem->compteur++;
        printf("incremente : %d\n",ptr_shared_mem->compteur);
        printf("real: %f\n",ptr_shared_mem->real);
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
    int id = shmget(key,0,0);
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
    incremente(nb);

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

    return EXIT_SUCCESS;
}
