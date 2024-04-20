#ifndef PARALLELIZATION_HELPERS
#define PARALLELIZATION_HELPERS

#include <pthread.h>

pthread_mutex_t output_mutex;
unsigned int    max_threads;

/**
 *  @brief Get the minimum of two comparable values
 */
#define MIN2(A, B)      ((A) < (B) ? (A) : (B))


/**
 *  @brief Get the maximum of two comparable values
 */
#define MAX2(A, B)      ((A) > (B) ? (A) : (B))


/**
 * @brief Get the number of CPU cores.
*/
int
num_proc_cores(int  *num_cores,
               int  *num_cores_conf);

/**
 * @brief Get the max number of threads.
*/
int
max_user_threads(void);

#endif
