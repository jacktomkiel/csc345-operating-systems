/** Jack Tomkiel
CSC345-01
Lab 10 Exercise 1 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h> // needed for abs()

int max_sec; // max sectors
int num_req; // number of requests
int cur_loc; // current location
// the last line of "input.txt" is the initial position

int *req; // pointer to store the requests

int fcfs()
{
    int total = 0;
    int cur = cur_loc; // cur is a local copy of cur_loc so that cur_loc can be reused

    for (int i = 0; i < num_req; ++i)
    {
        total += abs(req[i] - cur);
        cur = req[i];
    }

    return total;
}

int scan()
{
    int total = 0;

    return total;
}

int cscan()
{
    int total = 0;
    
    return total;
}

int main(int argc, char **argv)
{
    FILE *fp = fopen("input.txt", "rt"); // file pointer to read in

    // read in data
    fscanf(fp, "%d", &max_sec); // max sectors
    fscanf(fp, "%d", &num_req); // number of requests

    req = (int *)malloc(sizeof(int) * num_req); // prepare request array

    // read in using a loop
    for (int i = 0; i < num_req; ++i) // loops the number of request times
    {
        fscanf(fp, "%d", &(req[i])); // scan each request
    }

    fscanf(fp, "%d", &cur_loc); // scan in the initial position as the current location (last line of "input.txt")

    fclose(fp);

    // for debugging, 
    // prints requests array, max sectors, number of requests, and current locations
    // for (int i = 0; i < num_req; ++i) {
    //     printf("%d\n", req[i]);
    // }
    // printf("max_sec: %d, num_req: %d, cur_loc: %d\n", max_sec, num_req, cur_loc);

    fp = fopen("output.txt", "wt");

    fprintf(fp, "%d\n", fcfs());
    fprintf(fp, "%d\n", scan());
    fprintf(fp, "%d\n", cscan());

    fclose(fp);

    free(req); // free requests array

    return 0;
}