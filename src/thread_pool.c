/**
 * @file thread_pool.c
 * @brief Thread pool implementation for concurrent operations
 * @version 1.0.0
 */

#include "thread_pool.h"

/**
 * @brief Worker thread function
 */
static void* worker_thread(void *arg) {
    ThreadPool *pool = (ThreadPool*)arg;

    if (!pool) {
        LOG_ERROR("Worker thread received NULL pool");
        return NULL;
    }

    LOG_DEBUG("Worker thread %lu started", (unsigned long)pthread_self());

    while (true) {
        WorkItem *work = NULL;

        // Lock queue mutex to get work
        pthread_mutex_lock(&pool->queue_mutex);

        // Wait for work or shutdown signal
        while (pool->work_queue_head == NULL && !atomic_load(&pool->shutdown)) {
            pthread_cond_wait(&pool->work_cond, &pool->queue_mutex);
        }

        // Check for shutdown
        if (atomic_load(&pool->shutdown)) {
            pthread_mutex_unlock(&pool->queue_mutex);
            break;
        }

        // Get work from queue
        work = pool->work_queue_head;
        if (work) {
            pool->work_queue_head = work->next;
            if (pool->work_queue_head == NULL) {
                pool->work_queue_tail = NULL;
            }
            atomic_fetch_sub(&pool->pending_count, 1);
        }

        pthread_mutex_unlock(&pool->queue_mutex);

        // Execute work
        if (work) {
            atomic_fetch_add(&pool->working_count, 1);

            if (work->function) {
                work->function(work->arg);
            }

            free(work);

            atomic_fetch_sub(&pool->working_count, 1);

            // Signal that work is done
            pthread_mutex_lock(&pool->queue_mutex);
            pthread_cond_signal(&pool->done_cond);
            pthread_mutex_unlock(&pool->queue_mutex);
        }
    }

    LOG_DEBUG("Worker thread %lu exiting", (unsigned long)pthread_self());
    return NULL;
}

/**
 * @brief Create and initialize a thread pool
 */
ThreadPool* thread_pool_create(size_t thread_count) {
    if (thread_count == 0 || thread_count > MAX_THREADS) {
        LOG_ERROR("Invalid thread count: %zu", thread_count);
        return NULL;
    }

    LOG_INFO("Creating thread pool with %zu threads", thread_count);

    // Allocate thread pool structure
    ThreadPool *pool = safe_calloc(1, sizeof(ThreadPool));

    // Initialize fields
    pool->thread_count = thread_count;
    pool->work_queue_head = NULL;
    pool->work_queue_tail = NULL;
    atomic_store(&pool->working_count, 0);
    atomic_store(&pool->pending_count, 0);
    atomic_store(&pool->shutdown, false);

    // Initialize mutex and condition variables
    if (pthread_mutex_init(&pool->queue_mutex, NULL) != 0) {
        LOG_ERROR("Failed to initialize queue mutex");
        free(pool);
        return NULL;
    }

    if (pthread_cond_init(&pool->work_cond, NULL) != 0) {
        LOG_ERROR("Failed to initialize work condition");
        pthread_mutex_destroy(&pool->queue_mutex);
        free(pool);
        return NULL;
    }

    if (pthread_cond_init(&pool->done_cond, NULL) != 0) {
        LOG_ERROR("Failed to initialize done condition");
        pthread_cond_destroy(&pool->work_cond);
        pthread_mutex_destroy(&pool->queue_mutex);
        free(pool);
        return NULL;
    }

    // Allocate thread array
    pool->threads = safe_calloc(thread_count, sizeof(pthread_t));

    // Create worker threads
    for (size_t i = 0; i < thread_count; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            LOG_ERROR("Failed to create worker thread %zu", i);

            // Set shutdown flag and cleanup
            atomic_store(&pool->shutdown, true);
            pthread_cond_broadcast(&pool->work_cond);

            // Wait for created threads
            for (size_t j = 0; j < i; j++) {
                pthread_join(pool->threads[j], NULL);
            }

            free(pool->threads);
            pthread_cond_destroy(&pool->done_cond);
            pthread_cond_destroy(&pool->work_cond);
            pthread_mutex_destroy(&pool->queue_mutex);
            free(pool);
            return NULL;
        }
    }

    LOG_INFO("Thread pool created successfully");
    return pool;
}

/**
 * @brief Add work to the thread pool
 */
int thread_pool_add_work(ThreadPool *pool, thread_pool_func_t function, void *arg) {
    if (!pool || !function) {
        LOG_ERROR("Invalid parameters for adding work");
        return BDIX_ERROR_INVALID_INPUT;
    }

    if (atomic_load(&pool->shutdown)) {
        LOG_WARN("Cannot add work to shutdown pool");
        return BDIX_ERROR;
    }

    // Create work item
    WorkItem *work = safe_malloc(sizeof(WorkItem));
    work->function = function;
    work->arg = arg;
    work->next = NULL;

    // Add to queue
    pthread_mutex_lock(&pool->queue_mutex);

    if (pool->work_queue_tail) {
        pool->work_queue_tail->next = work;
    } else {
        pool->work_queue_head = work;
    }
    pool->work_queue_tail = work;

    atomic_fetch_add(&pool->pending_count, 1);

    // Signal worker thread
    pthread_cond_signal(&pool->work_cond);

    pthread_mutex_unlock(&pool->queue_mutex);

    LOG_DEBUG("Work added to pool (pending: %zu)",
              atomic_load(&pool->pending_count));

    return BDIX_SUCCESS;
}

/**
 * @brief Wait for all work to complete
 */
int thread_pool_wait(ThreadPool *pool) {
    if (!pool) {
        LOG_ERROR("Cannot wait on NULL pool");
        return BDIX_ERROR_INVALID_INPUT;
    }

    LOG_DEBUG("Waiting for all work to complete...");

    pthread_mutex_lock(&pool->queue_mutex);

    // Wait while there is pending work or working threads
    while (atomic_load(&pool->pending_count) > 0 ||
           atomic_load(&pool->working_count) > 0) {
        pthread_cond_wait(&pool->done_cond, &pool->queue_mutex);
    }

    pthread_mutex_unlock(&pool->queue_mutex);

    LOG_DEBUG("All work completed");
    return BDIX_SUCCESS;
}

/**
 * @brief Destroy thread pool and free resources
 */
void thread_pool_destroy(ThreadPool *pool) {
    if (!pool) {
        return;
    }

    LOG_INFO("Destroying thread pool");

    // Set shutdown flag
    atomic_store(&pool->shutdown, true);

    // Wake up all worker threads
    pthread_mutex_lock(&pool->queue_mutex);
    pthread_cond_broadcast(&pool->work_cond);
    pthread_mutex_unlock(&pool->queue_mutex);

    // Wait for all threads to exit
    for (size_t i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
        LOG_DEBUG("Joined worker thread %zu", i);
    }

    // Free any remaining work items in queue
    pthread_mutex_lock(&pool->queue_mutex);

    WorkItem *work = pool->work_queue_head;
    while (work) {
        WorkItem *next = work->next;
        free(work);
        work = next;
    }

    pthread_mutex_unlock(&pool->queue_mutex);

    // Destroy synchronization primitives
    pthread_cond_destroy(&pool->done_cond);
    pthread_cond_destroy(&pool->work_cond);
    pthread_mutex_destroy(&pool->queue_mutex);

    // Free thread array
    free(pool->threads);

    // Free pool structure
    free(pool);

    LOG_INFO("Thread pool destroyed");
}

/**
 * @brief Get number of pending work items
 */
size_t thread_pool_pending_count(const ThreadPool *pool) {
    if (!pool) {
        return 0;
    }

    return atomic_load(&pool->pending_count);
}

/**
 * @brief Get number of working threads
 */
size_t thread_pool_working_count(const ThreadPool *pool) {
    if (!pool) {
        return 0;
    }

    return atomic_load(&pool->working_count);
}

/**
 * @brief Check if thread pool is idle
 */
bool thread_pool_is_idle(const ThreadPool *pool) {
    if (!pool) {
        return true;
    }

    return (atomic_load(&pool->pending_count) == 0 &&
            atomic_load(&pool->working_count) == 0);
}
