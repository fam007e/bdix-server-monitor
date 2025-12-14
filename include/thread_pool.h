/**
 * @file thread_pool.h
 * @brief Thread pool implementation for concurrent server checking
 * @version 1.0.0
 */

#ifndef BDIX_THREAD_POOL_H
#define BDIX_THREAD_POOL_H

#include "common.h"
#include <pthread.h>

/**
 * @brief Work item function signature
 * @param arg Work item argument
 * @return Result pointer (unused, should return NULL)
 */
typedef void* (*thread_pool_func_t)(void *arg);

/**
 * @brief Thread pool work item
 */
typedef struct work_item {
    thread_pool_func_t function;    // Function to execute
    void *arg;                      // Function argument
    struct work_item *next;         // Next item in queue
} WorkItem;

/**
 * @brief Thread pool structure
 */
typedef struct {
    pthread_t *threads;             // Array of worker threads
    size_t thread_count;            // Number of threads

    WorkItem *work_queue_head;      // Queue head
    WorkItem *work_queue_tail;      // Queue tail
    pthread_mutex_t queue_mutex;    // Queue protection mutex
    pthread_cond_t work_cond;       // Work available condition
    pthread_cond_t done_cond;       // All work done condition

    _Atomic size_t working_count;   // Number of threads currently working
    _Atomic size_t pending_count;   // Number of pending work items
    _Atomic bool shutdown;          // Shutdown flag
} ThreadPool;

/**
 * @brief Create and initialize a thread pool
 *
 * @param thread_count Number of worker threads to create
 * @return Pointer to thread pool or NULL on error
 */
ThreadPool* thread_pool_create(size_t thread_count);

/**
 * @brief Add work to the thread pool
 *
 * @param pool Pointer to thread pool
 * @param function Function to execute
 * @param arg Argument to pass to function
 * @return BDIX_SUCCESS on success, error code otherwise
 */
int thread_pool_add_work(ThreadPool *pool, thread_pool_func_t function, void *arg);

/**
 * @brief Wait for all work to complete
 *
 * @param pool Pointer to thread pool
 * @return BDIX_SUCCESS on success
 */
int thread_pool_wait(ThreadPool *pool);

/**
 * @brief Destroy thread pool and free resources
 *
 * @param pool Pointer to thread pool
 */
void thread_pool_destroy(ThreadPool *pool);

/**
 * @brief Get number of pending work items
 *
 * @param pool Pointer to thread pool
 * @return Number of pending items
 */
size_t thread_pool_pending_count(const ThreadPool *pool);

/**
 * @brief Get number of working threads
 *
 * @param pool Pointer to thread pool
 * @return Number of working threads
 */
size_t thread_pool_working_count(const ThreadPool *pool);

/**
 * @brief Check if thread pool is idle
 *
 * @param pool Pointer to thread pool
 * @return true if idle (no pending work and no working threads)
 */
bool thread_pool_is_idle(const ThreadPool *pool);

#endif // BDIX_THREAD_POOL_H
