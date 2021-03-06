#ifndef RQUEUE_H
#define RQUEUE_H


#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define	INIT_SIZE_QUEUE 8

typedef struct {
	    void**array;				
        int first;                      /* position of first element */
        int last;                       /* position of last element */
        int count;                      /* number of queue elements */
	int size;
} queue;

extern void init_queue(queue *q);
extern void enqueue(queue *q, void*x);
extern void* dequeue(queue *q);
extern void resize(queue *q);
extern bool Queue_has(queue *q, void*x);

#endif
