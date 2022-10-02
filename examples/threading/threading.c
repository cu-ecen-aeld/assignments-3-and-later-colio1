#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
// https://fotos.johanna-unternaehrer.ch/p769275231
// https://darnellrenee.zenfolio.com/p858690713/h2e56d3dd#h3e30f219

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    int rc=0;
    struct thread_data* threadParams = (struct thread_data *)thread_param;

    // Set success boolean
    threadParams->thread_complete_success = false;

    // 
    usleep(threadParams->obtain_ms_value*1000);
    rc=pthread_mutex_lock(threadParams->mutex);
    if(rc != 0)
    {
        printf("pthread_mutex_lock failed");
    }
    else
    {
        usleep(threadParams->release_ms_value*1000);
        rc=pthread_mutex_unlock(threadParams->mutex);
        if(rc != 0)
            printf("pthread_mutex_unlock failed");
        else
            threadParams->thread_complete_success = true;
    }

   return (void *)thread_param;
}

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
    */
    int rc=0;
    struct thread_data* threadParams = (struct thread_data *)malloc(sizeof(struct thread_data));

    threadParams->mutex = mutex;
    threadParams->obtain_ms_value = wait_to_obtain_ms;
    threadParams->release_ms_value = wait_to_release_ms;

    rc=pthread_create(thread, NULL, threadfunc, (void *)threadParams);
    if(rc < 0)
        printf("pthread_create error for threadfunc");
    else
    {
        printf("pthread_create successful for threadfunc\n");
        return true;
    }

    return false;
}

