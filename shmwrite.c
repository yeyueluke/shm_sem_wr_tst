#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h> //declare O_CREAT O_RDWR
#include <sys/time.h>
#include <sys/stat.h>        /* For mode constants */

typedef struct _Teacher
{
    char name[64];
    long long  timestamp;
    int count;
}Teacher;

int main()
{
    int shmid = -1;
    key_t key = 0x2234;
    Teacher *p = NULL;
    int count = 0;
    sem_t *semr = NULL, *semw = NULL;
    semr = sem_open("sem_r", O_CREAT | O_RDWR, 0666, 0);
    if (semr == SEM_FAILED )
    {
        printf("errno = %d\n", errno );
        return -1;
    }

    semw = sem_open("sem_w", O_CREAT | O_RDWR, 0666, 1 );
    if (semw == SEM_FAILED)
    {
        printf("errno = %d\n", errno );
        return -1;
    }

    shmid = shmget(key, sizeof(Teacher), 0666 | IPC_CREAT );
    if ( shmid == -1 )
    {
        perror("shmget");
        return -1;
    }

    p = (Teacher*)shmat(shmid, NULL, 0);
    if (p == (Teacher*)(-1))
    {
        perror("shmat");
        return -1;
    }
    struct timeval tv;

    while(1)
    {
        sem_wait(semw);
        //printf(">name:");
        strcpy(p->name, "aaaa");
        gettimeofday(&tv,NULL);
        p->timestamp = tv.tv_sec * 1000000 + tv.tv_usec;
        p->count = count;
        printf("count = %d,timestamp = %lld\n",count,p->timestamp);
        count++;
        sem_post(semr);
    }
    return 0;
}
