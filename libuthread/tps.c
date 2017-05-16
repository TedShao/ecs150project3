#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <sys/mman.h>
#include <unistd.h>

#include "queue.h"
#include "thread.h"
#include "tps.h"

struct tpsNode{
    pthread_t TID;//tid using TPS
    void * ourmmap;//pointer to address of mapping
};


struct queue * q;

int findTID(queue_t queue, void *data, void *arg);

/*static void segv_handler(int sig, siginfo_t *si, void *context)
{
    *
     * Get the address corresponding to the beginning of the page where the
     * fault occurred
     *
    void *p_fault = (void*)((uintptr_t)si->si_addr & ~(TPS_SIZE - 1));

    
     * Iterate through all the TPS areas and find if p_fault matches one of them
     
    
    if ( There is a match )
         Printf the following error message 
        fprintf(stderr, "TPS protection error!\n");

     In any case, restore the default signal handlers 
    signal(SIGSEGV, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
     And transmit the signal again in order to cause the program to crash 
    raise(sig);
}*/


int tps_init(int segv)
{
    /*if (segv) {
        struct sigaction sa;

        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = segv_handler;
        sigaction(SIGBUS, &sa, NULL);
        sigaction(SIGSEGV, &sa, NULL);
    }*/
    
    q = queue_create(); //creating queue
    
    return 0;
}

int tps_create(void)
{
    pthread_t curtid = pthread_self();
    struct tpsNode * node = (struct tpsNode *)malloc(sizeof(struct tpsNode)); 
    struct tpsNode * temp;
    queue_iterate(q,findTID,(void *)curtid,(void*)&temp);
    if (temp==NULL)
        return -1;
    if (!node)
        return -1;    
    if(node)
    {
        node->TID = curtid;
        node->ourmmap = (char *)mmap(NULL, TPS_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0); 
        
        if (node->ourmmap == NULL)
                return -1;
        
        queue_enqueue(q,(void*)node);
    }
    return 0;
}

int findTID(queue_t queue, void *data, void *arg)
{
    struct tpsNode * temp;
    
    temp = data;
    pthread_t argtid = (*(pthread_t *)arg);

    if (temp->TID == argtid)
        return 1;

    return 0;
}

int tps_destroy(void)
{
    pthread_t TID = pthread_self();
    
    struct tpsNode * node;
    queue_iterate(q,findTID,(void*)TID,(void*)&node);
    
    if (!node)
        return -1;
    
    munmap(&node->ourmmap,TPS_SIZE);
    
    queue_delete(q,(void*)node);
    
    return 0;
}

int tps_read(size_t offset, size_t length, char *buffer)
{
    if (length > TPS_SIZE)
        return -1;
    
    pthread_t curtid = pthread_self();
    struct tpsNode * curTPS;
    queue_iterate(q,findTID, (void*) curtid,(void *) &curTPS);
    if (curTPS==NULL)
        return -1;
    memcpy(buffer,curTPS->ourmmap+offset,length);
    return 0;
}

int tps_write(size_t offset, size_t length, char *buffer)
{
    if (length > TPS_SIZE)
        return -1;
    
    pthread_t curtid = pthread_self();
    struct tpsNode * curTPS;
    queue_iterate(q,findTID,(void *) curtid,(void *) &curTPS);
    if(curTPS==NULL)
        return -1;
    memcpy(curTPS->ourmmap+offset,buffer,length);
    return 0;
}

int tps_clone(pthread_t tid)
{
    
    pthread_t curtid = pthread_self();
    
    struct tpsNode * temp;
    queue_iterate(q,findTID,(void*)tid,(void*)&temp);
    
    struct tpsNode * node = (struct tpsNode*)malloc(sizeof(struct tpsNode));
    
    if(node)
    {
        node->TID = curtid;
        node->ourmmap =  (char *)mmap(NULL, TPS_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0); 
        memcpy(node->ourmmap,temp->ourmmap,TPS_SIZE);
        
        if ( node->ourmmap == NULL)
                return -1;
        
        queue_enqueue(q,(void*)node);
    }

    
    return 0;
}

