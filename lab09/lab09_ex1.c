/** Jack Tomkiel
CSC345-01
Lab 9 Exercise 1 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int total_frames;

int total_refs;
int *page_refs;

int main(int argc, char **argv)
{
    FILE *fp = fopen("input.txt", "rt");
    if (fp == NULL) return -1;

    fscanf(fp, "%d", &total_refs);

    page_refs = (int *)malloc(sizeof(int) * total_refs);
    
    for (int i = 0; i < total_refs; ++i)
    {
        fscanf(fp, "%d", &(page_refs[i]));
    }

    fclose(fp);

    printf("total: %d\n", total_refs);
    for(int i = 0; i < total_refs; ++i)
    {
        printf("%d ", page_refs[i]);
    }
    printf("\n");

    free(page_refs);

    return 0;
}



