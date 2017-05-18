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
function when the reference counter is greater than one.

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
section. If there are no more resources available, it will check to see if there 
is anything in the waiting queue. If there is something in the waiting queue, 
we will take it out of the queue and unblock that thread we took out. Once we
are done we have the thread exit the critical section.


## Thread Private Storage API

### Tps_read() Function
The Tps read function is to copy a page into the buffer to verify if the message
is correctly copied. This function has arguments of length and offset. The 
offset variable is used to copy at the offset instead of the beginning of the 
page. Since ourmmap is not protected when created, we need to utilize the
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
also have a pointer that points to the page of the thread whose tid was passed
into the function. We check to see if the current thread already has a TPS and 
also check if the thread we are cloning has a TPS.

### Segv_Handler
For the handler, we iterate through our pages to see which page has a page 
fault with the p_fault function that was given to us. When we find the TPS page
that does this, we print out a message saying that the segfault was caused by 
having the wrong protections on our page. We restore the default signals and 
then raise a signal to make the program crash.

# Callback Functions

## findPage() Function
This callback function uses queue_iterate to find a page within a TPS. It checks
to see if the pointer of arg is equal to the mmap. 

    int findPage(queue_t queue, void *data, void *arg)
    {
        struct tpsNode * temp;
        temp = data;
        if (temp->pageptr->ourmmap == arg)
            return 1;
        return 0;
    }


## findTID() Function
This callback function uses queue_iterate to find a matching TID. If the TID is
found it will return the address to the TPS in the queue. 

    int findTID(queue_t queue, void *data, void *arg)
    {
        struct tpsNode * temp;
        temp = data;
        pthread_t argtid = (*(pthread_t *)arg);
        if (temp->TID == argtid)
            return 1;
        return 0;
    }
    
    
