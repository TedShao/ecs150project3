# Intro
For this project, we implemented an API for semaphores and also implemented
an API for Thread Private Storage.

# Data Structures

## Semaphore Struct
For our semaphore struct, we had a sem count and a queue for each of the 
threads that were waiting. Sem count is used for determining how many resources
are available.
    
    
    struct semaphore{
    int sem_count;
    struct queue * waiting;
    };
    
## TPS Struct
In the TPS struct (tpsNode) we have a pointer to a Page and a reference counter.
The pointer to the page is allocated in the create function and in the write
function when referece counter is greater than one.

    struct tpsNode{
    pthread_t TID;//tid using TPS
    struct page * pageptr;
    };
    
    

## Page Struct 
In the Page struct (page) we have a pointer to the page that is allocated using 
the mmap function and the int variable ref_counter is a counter that becomes
incremented every time we have a TPS using the same Page. If the ref_counter 
is greater than one and the write function is called by a thread, it becomes
decremented. 

    struct page{
    void * ourmmap;
    int ref_counter;
    };
    
# Functionality    

## Semaphore API

### Sem_down() Function 
For the sem_down function, we have the thread enter the critical section. If 
there an no more resources, we put the threads that are waiting in the waiting
queue and block those threads. If we have resources we will decrement the 
current number of resources. The thread that exits the critical section will
be the thread that does stuff between the sem_up and sem_down functions.

### Sem_up() Function
For our sem_up function, we first have the thread enter the critical
section. If there are no more resources avaiable, it will check to see if there 
is anything in the waiting queue. If there is something in the waiting queue, 
we will take it out of the queue and unblock that thread we took out. Once we
are done we have the thread exit the critical section.


## Thread Private Storage

### Tps_read() Function
The Tps read function is to copy a page into the buffer to verify if the message
is correctly copied. This function has arguments of length and offset. The 
offset variable is used to copy at the offset instead of the beginning of the 
page page. Since ourmmap is not protected when created, we need to utilize the
mprotect function to do a protected read (PROT_READ). Then once we have 
completed the copy we set the protection back to none (PROT_NONE).

### Tps_write() Function
The Tps write function copies the buffer into the memory page using memcpy. 
As in Tps read it iterates through our queue that contains the Tps and returns
that TPS (TpsNode). Before we make a memcpy we use a protected write 
(PROT_WRITE) using mprotect and once the memcpy has been completed the copy we 
set the protection back to none (PROT_NONE). 

### Tps_clone() Function 
The Tps clone function makes a new Tps object for the current running thread.
This Tps object will have the tid of the current running thread and will 
also have a pointer that points to the page of the thread that we passed into

### Callback Functions

## findPage() Function

    int findPage(queue_t queue, void *data, void *arg)
    {
        struct tpsNode * temp;
        temp = data;
        if (temp->pageptr->ourmmap == arg)
            return 1;
        return 0;
    }


## findTID() Function
