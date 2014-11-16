#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define SIZE 4

#define PROD 0
#define CONS 1
#define MUTEX_PROD 2
#define MUTEX_CONS 3


typedef struct ringBuffer_
{
    char buffer[SIZE];
    int index_read;
    int index_write;
}RingBuffer;

RingBuffer * ring;

int semid;
unsigned short init[] = {SIZE,0,1,1};
union semun
{
    int val;
    struct semid_ds * buf;
    unsigned short *array;
}arg;
struct sembuf op;

void init_sem(key_t key)
{
    if((semid = semget(key,4, IPC_CREAT | 0777)) == -1)
    {
        perror("failed to get sem\n");
        exit(1);
    }

    arg.array = init;

    if(semctl(semid,0,SETALL,arg)==-1)
    {
        perror("failed to init sem\n");
        exit(1);
    }
}

void init_shared(key_t key)
{

    int id = shmget(key,sizeof(RingBuffer),IPC_CREAT | 0777);
    if(id == -1)
    {
        perror("Failed to create shared mem\n");
        exit(1);
    }

    ring = shmat(id,NULL,0);
    if(ring == (void *)-1)
    {
        perror("failed to attach with shmat");
        exit(1);
    }

    ring->index_read = 0;
    ring->index_write = 0;
}

void init_all()
{
    key_t key;
    if((key = ftok("key",8)) == -1)
    {
        perror("ftok failed\n");
        exit(1);
    }

    init_sem(key);
    init_shared(key);


}

void push(char message)
{

}

char pop()
{

}

int main(int argc, const char *argv[])
{

    init_all();


    return 0;
}
