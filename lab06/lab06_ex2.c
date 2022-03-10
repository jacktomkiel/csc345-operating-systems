/** Jack Tomkiel
CSC345-01
Lab 6 Exercise 2 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct _FARMER {
    pthread_t t;
    int isNorth;
    int idx;
    int waitfor;
} FARMER;

#define MAX_WAIT 3

/* Bridge */ 
pthread_mutex_t t;
pthread_cond_t c;

int waiting_north = 0;
int waiting_south = 0;
int on_bridge = 0;
int prev_turn = 0; // 0 for south, 1 for north

void enter_bridge_north(FARMER* f)
{
    printf("M1: North farmer #%d is about to enter the bridge\n", f-> idx);
    ++waiting_north; 

    pthread_mutex_lock(&t);

    while (on_bridge == 1 || (prev_turn == 0) && waiting_south > 0)
    {
        pthread_cond_wait(&c, &t);
    }
    --waiting_north;
    prev_turn = 0;
    on_bridge = 1;
    printf("M2: North farmer #%d entered the bridge\n", f->idx);
}

void enter_bridge_south(FARMER* f)
{
    printf("M1: South farmer #%d is about to enter the bridge\n", f->idx);
    ++waiting_south;

    pthread_mutex_lock(&t);

    while (on_bridge || (prev_turn == 1) && waiting_north > 0)
    {
        pthread_cond_wait(&c, &t);
    }
    --waiting_south;
    prev_turn = 1;
    on_bridge = 1;
    printf("M2: South farmer #%d entered the bridge\n", f->idx);
}

void exit_bridge(FARMER* f)
{
    on_bridge = 0;

    if (f->isNorth)
    {
        printf("M5: North farmer #%d has left the bridge\n", f->idx);
    }
    else
    {
        printf("M5: South farmer #%d has left the bridge\n", f->idx);
    }

    pthread_cond_broadcast(&c);
    pthread_mutex_unlock(&t);

    
}

void* pass_bridge(void* param)
{
    FARMER* f = (FARMER*) param;

    /* On the bridge */
    if (f->isNorth) {
        enter_bridge_north(f);
        printf("  M3: North farmer #%d will pass the bridge in %d seconds\n", f->idx, f->waitfor);
        sleep(f->waitfor);
        printf("  M4: North farmer #%d has finished passing the bridge\n", f->idx);
    } 
    else {
        enter_bridge_south(f);
        printf("  M3: South farmer #%d will pass the bridge in %d seconds\n", f->idx, f->waitfor);
        sleep(f->waitfor);
        printf("  M4: South farmer #%d has finished passing the bridge\n", f->idx);
    }

	exit_bridge(f);

    pthread_exit(0);
}

int main(int argc, char** argv)
{
    if (argc != 3){
        printf("./lab06_ex1 nNorth nSouth\n");
        return 1;
    }

    int nNorth = atoi(argv[1]);
    int nSouth = atoi(argv[2]);

    printf(" * We have %d farmers from the north and %d farmers from the south *\n", nNorth, nSouth);

    FARMER* fN = (FARMER*) malloc( sizeof(FARMER) * nNorth );
    FARMER* fS = (FARMER*) malloc( sizeof(FARMER) * nSouth );

    pthread_cond_init(&c, NULL);
    pthread_mutex_init(&t, NULL);

    for (int i = 0; i < nNorth; ++i)
    {
        fN[i].isNorth = 1;
        fN[i].idx = i;
        fN[i].waitfor = rand() % MAX_WAIT; 
        pthread_create( &(fN[i].t), 0, pass_bridge, &(fN[i]) );
    }

    for (int i = 0; i < nSouth; ++i)
    {
        fS[i].isNorth = 0;
        fS[i].idx = i;
        fS[i].waitfor = rand() % MAX_WAIT; 
        pthread_create( &(fS[i].t), 0, pass_bridge, &(fS[i]) );
    }
    
    for (int i = 0; i < nNorth; ++i)
    {
        pthread_join( fN[i].t, NULL );
    }
    for (int i = 0; i < nSouth; ++i)
    {
        pthread_join( fS[i].t, NULL );
    }

    if ( (waiting_north && waiting_south) == 0)
    {
        printf("* All farmers passed *\n");
    }
    
    pthread_mutex_destroy(&t);
    pthread_cond_destroy(&c);

    free(fN);
    free(fS);

    return 0;
}