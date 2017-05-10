#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"


struct semaphore{
    int sem_count;
};

struct queue * s_queue; //waiting for resources


sem_t sem_create(size_t count)
{
    struct semaphore * s = (struct semaphore *)malloc(sizeof(struct semaphore));
    s->sem_count = 0;
    
    if (s)
    {
        s->sem_count = count;
        return s;
    }
    return NULL;
}



int sem_destroy(sem_t sem)
{
    queue_delete(s_queue, (void*)sem);
    
    if (sem == NULL)
        return -1;
    
    return 0;
}

int sem_down(sem_t sem)
{
    if (sem == NULL)
        return -1;
    
    
    if (sem->sem_count - 1 < 0)
    {
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
        //thread_unblock();//unblock first blocked thread
    
    sem->sem_count++;
    
    return 0;
}

