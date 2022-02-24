/** Jack Tomkiel
CSC345-01
Lab 5 Exercise 1 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE 10

int buf[BUF_SIZE];

pthread_mutex_t lock;

void* prod(void* param)
{
    int i;
    
    pthread_mutex_lock(&lock); /* 1 */
    for (i=0;i<BUF_SIZE;++i)
    {
        buf[i] = i;
    }
    pthread_mutex_unlock(&lock); /* 1 */

    pthread_exit(0);
}

void* cons(void* param)
{
    int i;

    pthread_mutex_lock(&lock); /* 1 */
    for (i=0;i<BUF_SIZE;++i)
    {
        printf("Buffer index %d = %d\n", i, buf[i]);
    }
    pthread_mutex_unlock(&lock); /* 1 */
    
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    pthread_t t_prod, t_cons;

    // initialize
    pthread_mutex_init(&lock, NULL); /* 1 */

    pthread_create(&t_prod, 0, prod, NULL);
    // have consumer start slightly later to avoid this problem, still not guarenteed you run the producer first
    sleep(0.5); /* 2 */
    pthread_create(&t_cons, 0, cons, NULL);

    pthread_join(t_prod, NULL);
    pthread_join(t_cons, NULL);

    /// must call this
    pthread_mutex_destroy(&lock); /* 1 */

    return 0;
}

// only enforced mutual exclusion here, not the order of execution, so the synchronization problem is still there

// since this is a dual core system, it is possible that the consumer be assigned to another processor so that is why
// it is first, even if it starts late
// lesson, not a silver bullet, need to apply in proper way
// we made sure we did mutex lock and ensure mutual exclusiveness, but this shows sync prob cannot be solved by mutex lock alone
