#include <stdio.h>

int computeFactorial(int num)   {
        int f = 1;
        
        for(int i = 1; i <= num; i++)   {
                f = f * i;
        }
        return f;
        
}

int main()      {
        int num = 6;
        int factorial = 1;
        
        factorial = computeFactorial(num);
        printf("Factorial of %d is: %d\n", num, factorial);
        
        return 0;
}
