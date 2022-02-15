/** Jack Tomkiel
CSC345-01
Lab 3 Excercise 1 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int len;
int *nums;

int r_min, r_max;
double r_avg, r_med, r_std;

void *myMin(void *param)
{
    int i;
    r_min = nums[0];
    for (i = 0; i < len; ++i)
    {
        if (r_min > nums[i])
            r_min = nums[i];
    }
    pthread_exit(0);
}

void *myMax(void *param)
{
    int i;
    r_max = nums[0];
    for (i = 0; i < len; ++i)
    {
        if (r_max < nums[i])
            r_max = nums[i];
    }
    pthread_exit(0);
}

void *myAverage(void *param)
{
    int i;
    r_avg = 0;
    for (i = 0; i < len; ++i)
    {
        r_avg += nums[i];
    }
    r_avg /= len;
    pthread_exit(0);
}

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

void *myMedian(void *param)
{
    int n;
    qsort(nums, len, sizeof(int), compare);
    // if even
    if (len % 2 == 0)
    {
        r_med = ( nums[len / 2] + nums[ (len/2) - 1 ] ) / 2.0;
    }
    // else odd
    else
    {
        r_med = nums[len / 2];
    }
    pthread_exit(0);
}

void *myStd(void *param)
{
    float mean;
    float mean_diff = 0;

    for (int i = 0; i < len; ++i)
    {
        mean += nums[i];
    }
    mean = mean / len;

    for (int i = 0; i < len; ++i)
    {
        mean_diff += ((nums[i] - mean) * (nums[i] - mean));
    }

    mean_diff = mean_diff / len;
    r_std = sqrt(mean_diff);

    pthread_exit(0);
}

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printf("Give arguments!\n");
        exit(0);
    }

    // since the name of the program is an argument, the length is argc - 1
    len = argc - 1;
    // alocate memory
    nums = (int *)malloc(sizeof(int) * len);

    for (int i = 1; i < argc; ++i)
    {
        nums[i - 1] = atoi(argv[i]);
    }

    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;
    pthread_t tid4;
    pthread_t tid5;

    pthread_create(&tid1, 0, myMin, NULL);
    pthread_create(&tid2, 0, myMax, NULL);
    pthread_create(&tid3, 0, myAverage, NULL);
    pthread_create(&tid4, 0, myMedian, NULL);
    pthread_create(&tid5, 0, myStd, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
    pthread_join(tid5, NULL);

    printf("Min = %d\n", r_min);
    printf("Max = %d\n", r_max);
    printf("Average = %f\n", r_avg);
    printf("Median = %f\n", r_med);
    printf("Standard Deviation = %f\n", r_std);

    free(nums);
    
    return 0;
}