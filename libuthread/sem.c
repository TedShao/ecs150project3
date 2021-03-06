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
    if (s)
    {
        s->sem_count = 0;
        s->sem_count = count;
        s->waiting = queue_create();
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
    enter_critical_section();
    
    if (sem == NULL)
       return -1; 
    
    while (sem->sem_count == 0)
    {
        queue_enqueue(sem->waiting,(void*)pthread_self());
        thread_block();
    }
    
    if (sem->sem_count > 0)
        sem->sem_count -= 1;
    
    exit_critical_section();
    return 0;
}

int sem_up(sem_t sem)
{   
    enter_critical_section();
    
    if (sem == NULL)
        return -1;
    
    if (sem->sem_count == 0)
    {
        if (queue_length(sem->waiting) != 0)
        {
            pthread_t tid;
            queue_dequeue(sem->waiting,(void*)&tid);
            thread_unblock(tid);//unblock first blocked thread
        }
    }
    
    
    sem->sem_count += 1;
    
    exit_critical_section();
    
    return 0;
}

