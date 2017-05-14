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
    char * ourmmapfile;
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
    printf("TPS_INIT\n");
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
    printf("TPS_CREATE\n");
    pthread_t curtid = pthread_self();
    struct tpsNode * node;
    int retval = queue_iterate(q,findTID,(void*)curtid,(void*)&node);
    
    char * filename = "file.txt";
    int fd = open(filename, O_CREAT);
    
    if (retval == 0 && node->ourmmap != NULL)
        return -1;
    
    if (retval == 0 && node->ourmmap == NULL)
        node->ourmmap = mmap(NULL, TPS_SIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE, fd, 0); 
    
    if (retval != 0)
    {
        node = (struct tpsNode *)malloc(sizeof(struct tpsNode)); 
        if (node)
        {
            printf("created a node\n");
            node->TID = curtid;
            node->ourmmap = mmap(NULL, TPS_SIZE, 0, MAP_PRIVATE, fd, 0);
            node->ourmmapfile = filename;
            queue_enqueue(q,(void*)node);
        }
        if (!node)
            return -1;
    }
    
    return 0;
}

int findTID(queue_t queue, void *data, void *arg)
{
    struct tpsNode * temp;

    temp = data;

    if (temp->TID == (*(pthread_t*)arg))
        return 1;

    return 0;
}

int tps_destroy(void)
{
    pthread_t TID = pthread_self();
    
    struct tpsNode * node;
    int retval = queue_iterate(q,findTID,(void*)TID,(void*)&node);
    
    if (retval != 0)
        return -1;
    
    munmap(node->ourmmap,TPS_SIZE);
    
    node->ourmmap = NULL;
    //queue_delete(t->q,(void*)node);
    
    return 0;
}

int tps_read(size_t offset, size_t length, char *buffer)
{
    printf("IN READ\n");
    int fd;
    pthread_t curtid = pthread_self();
    struct tpsNode * curTPS;
    int retval = queue_iterate(q,findTID,(void *) curtid,(void *) &curTPS);
    if (retval!=0)
        return -1;

    char * map = curTPS->ourmmap;
    fd = open(curTPS->ourmmapfile,O_RDONLY);
    if (fd==-1)
        return -1;
    for (int i = offset; i < length; i++)
    {
        buffer[i] = map[i];
    }
    return 0;
}

int tps_write(size_t offset, size_t length, char *buffer)
{
    printf("IN WRITE\n");
    int fd;
    pthread_t curtid= pthread_self();
    struct tpsNode * curTPS;
    int retval = queue_iterate(q,findTID,(void *) curtid,(void *) &curTPS);
    if (retval!=0)
        return -1;

    fd = open(curTPS->ourmmapfile,O_WRONLY);
    if (fd==-1)
        return -1;
    char * map = curTPS->ourmmap;
    for (int i = offset; i < length; i++)
    {
        map[i] = buffer[i];
        //printf("%c",curTPS->ourmmap[i]);
    }
    
    return 0;
}

int tps_clone(pthread_t tid)
{
    pthread_t curtid = pthread_self();
    
    struct tpsNode * temp;
    struct tpsNode * curTPS;
    int retval = queue_iterate(q,findTID,(void*)tid,(void*)&temp);
    int retval2 = queue_iterate(q,findTID,(void*)curtid,(void*)&curTPS);
    
    if (retval != 0||retval2 !=0)
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

