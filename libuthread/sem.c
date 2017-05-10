#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"


struct semaphore{
    int sem_count;
    struct queue * waiting;
};

 //waiting for resources

sem_t sem_create(size_t count)
{
    struct semaphore * s = (struct semaphore *)malloc(sizeof(struct semaphore));
    s->sem_count = 0;
    
    s->waiting = queue_create();
    
    if (s)
    {
        s->sem_count = count;
        return s;
    }
    return NULL;
}



int sem_destroy(sem_t sem)
{   
    if (sem == NULL)
        return -1;
    
    free(sem);
    
    return 0;
}

int sem_down(sem_t sem)
{
    if (sem == NULL)
       return -1;
    
    
    if (sem->sem_count - 1 < 0)
    {
        queue_enqueue(sem->waiting,(void*)pthread_self());
        thread_block();
        return -1;
    }
    
    sem->sem_count--;
    
    return 0;
}

int sem_up(sem_t sem)
{   
    if (sem == NULL)
        return -1;
    
    if (sem->sem_count == 0)
    {
        pthread_t tid;
        queue_dequeue(sem->waiting,(void*)&tid);
        thread_unblock(tid);//unblock first blocked thread
    }
    
    sem->sem_count++;
    
    return 0;
}

