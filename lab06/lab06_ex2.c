/** Jack Tomkiel
CSC345-01
Lab 6 Exercise 2 */

// 

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

int main(int argc, char** argv)
{
    if (argc != 3){
        printf("./lab06_ex1 nNorth nSouth\n");
        return 1;
    }
    return 0;
}

