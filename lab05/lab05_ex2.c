/** Jack Tomkiel
CSC345-01
Lab 5 Exercise 2 */

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

int main(int argc, char** argv)
{
    // 
    sem_t sem;

    // initialize
    if (sem_init(&sem,0,1) == -1)
        printf("%s\n", strerror(errno));
    
    // to wait
    if (sem_wait(&sem) != 0)
        printf("%s\n", strerror(errno));

    printf("*** Critical Section ***\n");
    
    // to signal
    if (sem_post(&sem) != 0)
        printf("%s\n", strerror(errno));

    printf("*** Non-Critical Section ***\n");

    // destroy after use
    if (sem_destroy(&sem) != 0)
        printf("%s\n", strerror(errno));

    return 0;
}