#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    /*
     * machine code for:
     *   mov eax, 0
     *   ret
     */
    unsigned char code[] = { 0xb8, 0x00, 0x00, 0x00, 0x00, 0xc3 };

    if (argc < 2) {
        fprintf(stderr, "Usage: ./hellojit <INT>\n");
        return 1;
    }

    /*
     * overwrite immediate value '0' in the instruction
     * with the user's value.
     *   mov eax, <value>
     *   ret
     */
    int imm = atoi(argv[1]);
    memcpy(&code[1], &imm, 4);

    /*
     * allocate writable/executable memory
     */
    void *mem = mmap(NULL, sizeof(code), PROT_WRITE | PROT_EXEC,
            MAP_ANON | MAP_PRIVATE, -1, 0);
    memcpy(mem, code, sizeof(code));

    /* the function return the user's value */
    int (*func)(void) = mem;

    return func();
}
