
#include <stdio.h>
#include <pthread.h>
#include "threadPool.h"

ThreadPool *tpCreate(int numOfThreads) {
    int i = 0, j = 0, checkCreate;

    ThreadPool *threadPool;
    // try to allocate memory for the thread pool
    if ((threadPool = (ThreadPool *) malloc(sizeof(ThreadPool))) == NULL) {
        perror("Error in allocating memory to thread pool\n");
        return NULL;
    }

    // Initialize thread pool's fields (as a struct)
    threadPool->thread_count = numOfThreads;
    threadPool->run_when_active_task = 1;
    threadPool->is_active = 1;
    threadPool->task = osCreateQueue();
    threadPool->threads = (pthread_t*) calloc (threadPool->thread_count, sizeof(pthread_t));

    if (threadPool->threads == NULL) {
        free(threadPool);
        perror("Error in allocating memory to thread pool\n");
    }
    // Initialize the mutex
    if (pthread_mutex_init(&threadPool->lock, NULL) != 0) {
        perror("Error in allocating memory to thread pool\n");
    }
    // Initialize the condition
    if (pthread_cond_init(&threadPool->notify, NULL) != 0) {
        perror("Error in allocating memory to thread pool\n");
    }

    for (i = 0; i < threadPool->thread_count; i++) {
        if (pthread_create(&(threadPool->threads[i]), NULL, executeThreadTask, (void *) threadPool) != 0) {
            for (j = 0; j < i; j++) {
                pthread_cancel(threadPool->threads[j]);
                pthread_join(threadPool->threads[j], NULL);
            }
            free(threadPool->threads);
            free(threadPool);
            perror("Error\n");
        }
    }
    return threadPool;
}

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {
    int i = 0;

    // Marks that the pool is inactive and check if the pool should wait for more tasks to finish before destruction
    threadPool->is_active = 0;
    threadPool->run_when_active_task = shouldWaitForTasks;

    if (pthread_cond_broadcast(&threadPool->notify) != 0) {
        perror("Error in broadcast\n");
    }

    if (pthread_mutex_unlock(&threadPool->lock) != 0) {
        perror("Error in unlock\n");
    }

    for (i = 0; i < threadPool->thread_count; i++) {
        if (pthread_join(threadPool->threads[i], NULL) != 0) {
            perror("Error in system Call\n");
            free(threadPool->threads);
            free(threadPool);
            exit(1);
        }
    }
    while (!osIsQueueEmpty(threadPool->task)) {
        Thread_task *thread_task = osDequeue(threadPool->task);
        free(thread_task);
    }
    free(threadPool->threads);
    osDestroyQueue(threadPool->task);
    pthread_mutex_destroy(&(threadPool->lock));
    pthread_cond_destroy(&(threadPool->notify));
    free(threadPool);
}

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {
    if (threadPool->is_active == 0) {
        return -1;
    }

    Thread_task *thread_task;
    if ((thread_task = (Thread_task *) malloc(sizeof(Thread_task))) == NULL) {
        perror("Error in allocating memory to Task\n");
        free(threadPool->threads);
        free(threadPool);
        exit(1);
    }
    thread_task->function = computeFunc;
    thread_task->argument = param;
    if (pthread_mutex_lock(&(threadPool->lock))) {
        perror("Error in lock\n");
        free(thread_task);
        free(threadPool->threads);
        free(threadPool);
        exit(1);
    }
    OSQueue *queue = threadPool->task;

    osEnqueue(queue, thread_task);
    if ((pthread_cond_signal(&threadPool->notify)) != 0) {
        perror("Error in system Call\n");
        free(thread_task);
        free(threadPool->threads);
        free(threadPool);
        exit(1);
    }
    if (pthread_mutex_unlock(&(threadPool->lock))) {
        perror("Error in unlock\n");
        free(thread_task);
        free(threadPool->threads);
        free(threadPool);
        exit(1);
    }
    return 0;
}

void *executeThreadTask(void *args) {
    ThreadPool *threadPool = (ThreadPool *) args;

    // running while thread pool is running
    while (threadPool->run_when_active_task) {
        // check is lock successful
        if (pthread_mutex_lock(&(threadPool->lock))) {
            perror("Error in lock\n");
            free(threadPool->threads);
            free(threadPool);
            exit(1);
        }
        // if there are task to execute
        if (!osIsQueueEmpty(threadPool->task)) {
            Thread_task *thread_task = osDequeue(threadPool->task);
            if (pthread_mutex_unlock(&(threadPool->lock))) {
                perror("Error in unlock\n");
                free(threadPool->threads);
                free(threadPool);
                exit(1);
            }
            thread_task->function(thread_task->argument);
            free(thread_task);
        } else { // if there are no tasks to execure
            if (!threadPool->is_active == 1) {
                if (pthread_mutex_unlock(&(threadPool->lock))) {
                    perror("Error in unlock\n");
                    free(threadPool->threads);
                    free(threadPool);
                    exit(1);
                }
                break;
            }
            pthread_cond_wait(&(threadPool->notify), &threadPool->lock);
            if (pthread_mutex_unlock((&(threadPool->lock)))) {
                perror("Error in unlock\n");
                free(threadPool->threads);
                free(threadPool);
                exit(1);
            }
        }
    }
}