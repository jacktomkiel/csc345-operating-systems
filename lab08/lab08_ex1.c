/** Jack Tomkiel
CSC345-01
Lab 8 Exercise 1 */

#include <stdlib.h>
#include <stdio.h>

#define PAGE_SIZE 4096

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Enter only 1 argument.\n");
        return 1;
    }
    
    unsigned int virtual_address =  (unsigned int)atoi(argv[1]);
    int page = virtual_address / PAGE_SIZE;
    int offset = virtual_address % PAGE_SIZE;

    printf("The address %u is located at:\n", virtual_address);
    printf("page number = %d\n", page);
    printf("offeset = %d\n", offset);

    return 0;
}