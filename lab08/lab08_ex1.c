/** Jack Tomkiel
CSC345-01
Lab 8 Exercise 1 */

#include <stdlib.h>
#include <stdio.h>

/* PAGE_SIZE = 4KB = (4)*(1)*(2^10) = 2^12 = 4096 bytes */
#define PAGE_SIZE 4096

int main(int argc, char** argv)
{
    unsigned int addr;
    unsigned int page;
    unsigned int offset;

    addr =  atoi(argv[1]);
    page = addr / PAGE_SIZE;
    offset = addr % PAGE_SIZE;

    printf("The address %u is located at:\n", addr);
    printf("Page number = %u\n", page);
    printf("Offset = %u\n", offset);

    return 0;
}

/* to zip file: >>> lab08 % zip -r lab08.zip . -x ".*" -x "__MACOSX" <<< */