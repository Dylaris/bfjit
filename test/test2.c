#include <stdio.h>

extern void printchar(char *ptr);
extern char readchar(void);

int main(void)
{
    // char ch = 'a';
    // printchar(&ch);

    char ch = readchar();
    printchar(&ch);

    return 0;
}
