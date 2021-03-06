#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "queue.h"

struct node {
    void *data;
    struct node *next;
};

struct queue{
    struct node * head, *tail;
    int num_nodes;
};

queue_t queue_create(void)
{
    struct queue * q = (struct queue *)malloc(sizeof(struct queue));

    q->head = NULL;
    q->tail = NULL;
    
    q->num_nodes = 0;

    if (q == NULL)
    {
        return NULL;
    }

    return q;
}

int queue_destroy(queue_t queue)
{

    if(queue->head != NULL || queue == NULL)
        return -1;
    else
        free(queue);

    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    struct node *node = (struct node *)malloc(sizeof(struct node));

    node->data = data;
    node->next = NULL;

    if (queue == NULL || data == NULL)
            return -1;


    if (queue->head == NULL)
    {
        queue->head = node;
        queue->tail = node;
    }
    else
    {
        queue->tail->next = node;
        queue->tail = node;
    }
    queue->num_nodes++;

    return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
    struct node *tempNode = (struct node *)malloc(sizeof(struct node));

    if (queue == NULL || data == NULL || queue->head == NULL)
            return -1;

    tempNode = queue->head;
    queue->head = queue->head->next;
    *data = tempNode->data;

    if (tempNode == queue->tail)
      {
    	queue->tail = NULL;
    	queue->head = NULL;
      }
    
    queue->num_nodes--;

    return 0;
}

int queue_delete(queue_t queue, void *data)
{
    struct node *curr = queue->head;
    struct node *prev = NULL;

    if (queue == NULL || data == NULL || queue->head == NULL)
        return -1;

    if (queue->head->data == data)
    {
        struct node *nxt;
        if (queue->head->next != NULL) 
        {
            curr->data = curr->next->data;
            nxt = curr->next;
            curr->next = nxt->next;
        }
        free(curr);
    }

    else
    {
        while (curr != NULL)
        {
            if(curr->data == data)
            {
              break;
            }
            else
            {
                prev = curr;
                curr = curr->next;
            }
        }
        
        if (curr == NULL) 
            return -1;
        
        if (curr != NULL)
        {
            if (curr->next == NULL && curr->data == data)
            {
                queue->tail = prev;
                free(curr);
            }
            
            else
            {
                prev->next = curr->next;
                free(curr);
            }
        }
    }

    queue->num_nodes--;
    return 0;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
    struct node * tempnode = queue->head;

    if (func == NULL || queue == NULL)
            return -1;

    while (tempnode != NULL)
    {
        if (tempnode->data != NULL)
        {
            int retval = func(queue,tempnode->data,arg);
            if (retval == 1)
            {
                if (data != NULL)
                {
                    *data = tempnode->data;
                    break;
                }
            } 
        }
        
        tempnode = tempnode->next;
    }


    return 0;
}

int queue_length(queue_t queue)
{
    return queue->num_nodes;
}
