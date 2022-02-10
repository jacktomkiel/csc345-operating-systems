/** Jack Tomkiel
CSC345-01
Lab 3 Excercise 2 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int pts_total = 0;
int pts_circle = 0;

void* runner(void* param)
{
    // generate pts_total within square
    // x in [-1,1]; y in [1,1]
    // use the maximum that the random function can generate
    for (int i = 0; i < pts_total; ++i){
        double x = 2.0 * (double)rand() / (double)RAND_MAX - 1.0;
        double y = 2.0 * (double)rand() / (double)RAND_MAX - 1.0;
        
        if ((x * x + y * y) < 1.0 ){
            ++pts_circle;
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

    pts_total = atoi(argv[1]);

    pthread_t tid;

    pthread_create(&tid, 0, runner, NULL);

    pthread_join(tid, NULL);

    double pi = 4.0 * ((double)pts_circle / (double)pts_total);
    printf("PI = %f\n", pi);

    return 0;
}