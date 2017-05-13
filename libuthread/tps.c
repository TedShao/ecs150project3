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

struct tps{
    struct queue * q;
}*t;


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
    
    return 0;
}

int tps_create(void)
{
    char * ourmmapfile = "mapfile.txt";
    int fd = open(ourmmapfile,O_CREAT); //make a file for mmap function
    if(fd == -1)
        return -1;
    struct tpsNode * node = (struct tpsNode *)malloc(sizeof(struct tpsNode)); 
    //allocate memory to tpsnode
    if (node)
    {
        node->TID = pthread_self();
        node->ourmmap = mmap(NULL, TPS_SIZE, 0, MAP_PRIVATE, fd, 0);
        
        
        queue_enqueue(t->q,(void*)node);
    }
    if (!node)
        return -1;
    
    return 0;
}

int findTID(queue_t queue, void *data, void *arg)
{
    struct tpsNode * temp;

    temp = data;

    if (temp->TID == (*(pthread_t*)arg))
    {
        return 1;
    }

    return 0;
}

int tps_destroy(void)
{
    pthread_t TID = pthread_self();
    
    struct tpsNode * node;
    int retval = queue_iterate(t->q,findTID,(void*)TID,(void*)&node);
    
    if (retval != 0)
        return -1;
    
    munmap(node->ourmmap,TPS_SIZE);
    
    queue_delete(t->q,(void*)node);
    
    return 0;
}

int tps_read(size_t offset, size_t length, char *buffer)
{
    
    return 0;
}

int tps_write(size_t offset, size_t length, char *buffer)
{
    
    return 0;
}

int tps_clone(pthread_t tid)
{
    pthread_t curtid = pthread_self();
    
    struct tpsNode * temp;
    struct tpsNode * curTPS;
    int retval = queue_iterate(t->q,findTID,(void*)tid,(void*)&temp);
    int retval2 = queue_iterate(t->q,findTID,(void*)curtid,(void*)&curTPS);
    
    if (retval != 0)
        return -1;
    
    if (retval2 == 0 && retval == 0)
    {
        if (curTPS->ourmmap != NULL || temp->ourmmap == NULL)
            return -1;
        else
            memcpy(curTPS->ourmmap, temp->ourmmap, TPS_SIZE);
    }
    
    return 0;
}

