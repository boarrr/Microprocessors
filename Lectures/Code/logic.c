#include <stdio.h>

int main(void)
{
    int x = 1;
    int y = 2;

    // Boolean Comparison
    if (x && y)
        printf("yes\n");

    // Bitwise AND
    if (x & y)
        printf("yes too!\n");

    // Bitwise OR
    printf("%d\n", x | y);

    // Not X, hex result
    printf("%x\n", ~x);
}