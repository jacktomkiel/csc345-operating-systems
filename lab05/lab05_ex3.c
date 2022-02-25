/** Jack Tomkiel
CSC345-01
Lab 5 Exercise 3 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

# define NUM_THREADS 4

pthread_mutex_t lock;

int pts_total = 0;
int pts_circle = 0;

void* worker(void* param)
{
    int n_to_drw = *((int*)param);
    for (int i = 0; i < n_to_drw; ++i){
        double x = 2.0 * (double)rand() / (double)RAND_MAX - 1.0;
        double y = 2.0 * (double)rand() / (double)RAND_MAX - 1.0;
        
        if ((x * x + y * y) < 1.0 ){
            pthread_mutex_lock(&lock);
            ++pts_circle;
            pthread_mutex_unlock(&lock);
        }
    }
    pthread_exit(0);
}
int main(int argc, char** argv)
{
    if (argc < 2)
	{
		printf("Give argument!\n");
		exit(0);
	}

    pthread_t tid[NUM_THREADS];
    pts_total = atoi(argv[1]);

    pthread_mutex_init(&lock, NULL);
    int pts_to_draw = pts_total/4;

    pthread_create(&tid[0], 0, worker, &pts_to_draw);
    pthread_create(&tid[1], 0, worker, &pts_to_draw);
    pthread_create(&tid[2], 0, worker, &pts_to_draw);
    int pts_to_draw_last = pts_to_draw + pts_total % NUM_THREADS;
    pthread_create(&tid[3], 0, worker, &pts_to_draw_last);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);

    double pi = 4.0 * ((double)pts_circle / (double)pts_total);
    printf("PI = %f\n", pi);

    pthread_mutex_destroy(&lock);
    return 0;
}