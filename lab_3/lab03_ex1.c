/** Jack Tomkiel
CSC345-01
Lab 3 Excercise 1 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int len = 7;
int *nums;

int r_min;
int r_max;
double r_avg;

void *myMin(void *param)
{
    int i;
    r_min= nums[0];
    for ( i = 0; i < len; ++i ) {
        if ( r_min> nums[i] ) r_min= nums[i];
    }
    pthread_exit(0);
}

void *myMax(void *param)
{
    int i;
    r_max= nums[0];
    for ( i = 0; i < len; ++i ) {
        if ( r_max < nums[i] ) r_max = nums[i];
    }
    pthread_exit(0);
}

void *myAverage(void *param)
{
    int i;
    r_avg = 0;
    for ( i = 0; i < len; ++i ) {
        r_avg += nums[i];
    }
    r_avg /= len;
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    if (argc <= 1)
	{
		printf("Give arguments!\n");
		exit(0);
	}

    len = argc -1;
    nums = (int*)malloc(sizeof(int)*len);
    for (int i=1; i < argc; ++i)    {
        nums[i-1] = atoi(argv[i]);
    }

    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;

    pthread_create (&tid1, 0, myMin, NULL);
    pthread_create (&tid2, 0, myMax, NULL);
    pthread_create (&tid3, 0, myAverage, NULL);
    pthread_join (tid1, NULL);
    pthread_join (tid2, NULL);
    pthread_join (tid3, NULL);
    printf ("Min = %d\n", r_min);
    printf ("Max = %d\n", r_max);
    printf ("Average = %f\n", r_avg);

    free(nums);
    return 0;
}