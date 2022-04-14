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
    int cur = cur_loc;
    int *req_tmp = (int *)malloc(sizeof(int) * num_req);

    int i, j;

    for (i = 0; i < num_req; ++i)
    {
        req_tmp[i] = req[i];
    }

    // sort req (in increasing order)
    for (i = 0; i < num_req; ++i)
    {
        for (j = i; j < num_req; ++j)
        {
            if (req_tmp[j] < req_tmp[i])
            { // if j is smaller than i, flip them
                int t = req_tmp[j];
                req_tmp[j] = req_tmp[i];
                req_tmp[i] = t;
            }
        }
    }

    // decide direction
    for (i = 0; i < num_req; ++i)
    {
        if (cur < req_tmp[i])
            break;
    }

    if (i == 0)
    {
        // left to right (low to high)
        // compute distance in the direction using similar to fcfs
        for (j = 0; j < num_req; ++j)
        {
            total += abs(req_tmp[j] - cur);
            cur = req_tmp[j];
        }
    }
    else if (i == num_req)
    {
        // right to left (high to low)
        // compute distance in the direction using similar to fcfs
        for (j = (num_req - 1); j >= 0; --j)
        {
            total += abs(req_tmp[j] - cur);
            cur = req_tmp[j];
        }
    }
    else
    {
        if (abs(cur - req_tmp[i - 1]) < abs(cur - req_tmp[i]))
        {
            // right to left (high to low)
            for (j = (i - 1); j >= 0; --j)
            {
                total += abs(req_tmp[j] - cur);
                cur = req_tmp[j];
            }
            cur = 0;
            // jump to original location and compute distance of remainder
            for (j = i; j < num_req; ++j)
            {
                total += abs(req_tmp[j] - cur);
                cur = req_tmp[j];
            }
        }
        else
        {
            // left to right (low to high)
            for (j = i; j < num_req; ++j)
            {
                total += abs(req_tmp[j] - cur);
                cur = req_tmp[j];
            }
            cur = max_sec;

            // jump to the original location and compute distance of remainder
            for (j = (i - 1); j >= 0; --j)
            {
                total += abs(req_tmp[j] - cur);
                cur = req_tmp[j];
            }
        }
    }

    free(req_tmp);

    return total;
}

int cscan()
{
    // sort req
    // compute distance in the increasing direction
    // jump to the beginning of the disk
    // compute the remainder
    int total = 0;
    int min = req[0];
    int max = req[0];
    int max_b_cur = req[0];

    for (int i = 1; i < num_req; ++i)
    {
        if (req[i] > max_b_cur && req[i] < cur_loc)
        {
            max_b_cur = req[i];
        }
        if (req[i] > max)
        {
            max = req[i];
        }
        if (req[i] < min)
        {
            min = req[i];
        }
    }

    if (cur_loc < max && cur_loc > min)
    {
        total = (max_sec - cur_loc) + max_b_cur;
    }
    else if (cur_loc <= min)
    {
        total = (max - cur_loc);
    }
    else if (cur_loc >= max)
    {
        total = (max_sec - cur_loc) + max;
    }
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

    // for debugging, prints requests array, max sectors, number of requests, and current locations
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