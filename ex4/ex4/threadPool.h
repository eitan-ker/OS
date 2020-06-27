//
// Created by eizzker on 24/06/2020.
//

#ifndef EX4_THREADPOOL_H
#define EX4_THREADPOOL_H


#include <sys/types.h>
#include "osqueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    void (*function)(void *);
    void *argument;
} Thread_task;

typedef struct thread_pool
{
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    int thread_count; // amount of threads to make
    int is_active;
    int run_when_active_task;
    OSQueue *task;
    int numberOfThreadsAlive;
}ThreadPool;


// creates thread pool acording to the number of threads asked
ThreadPool* tpCreate(int numOfThreads);

// destroys thread pool
void tpDestroy(ThreadPool* threadPool, int shouldWaitForTasks);

// insert a task to work on
int tpInsertTask(ThreadPool* threadPool, void (*computeFunc) (void *), void* param);

// initialize thread pool
void initializeThreadPool(ThreadPool **threadPool, int numOfThreads);

// runs this fun until we have a task(after insert) to execute
void *executeThreadTask(void *args);

#endif //EX4_THREADPOOL_H
