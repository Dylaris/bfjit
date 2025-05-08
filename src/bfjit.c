#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ZD_DS_IMPLEMENTATION
#define ZD_DS_DYNAMIC_BUFFER
#define ZD_DS_DYNAMIC_ARRAY
#include "zd_ds.h"

// Brainfuck    C
//     >      ++ptr
//     <      --ptr
//     +      ++*ptr
//     -      --*ptr
//     .      fputc(*ptr, stdout);
//     ,      *ptr = fgetc(stdin);
//     [      while(*ptr) {
//     ]      }

enum optype {
    OP_SHL = '<',
    OP_SHR = '>',
    OP_INC = '+',
    OP_DEC = '-',
    OP_OUT = '.',
    OP_IN  = ',',
    OP_JZ  = '[',
    OP_JNZ = ']'
};

struct instruction {
    /*
     * meaning of operand for each instruction
     * for OP_JZ/OP_JNZ, it means the jump address 
     * for others, it means repeat times
     */
    int opcode;
    size_t operand;
};

static void read_file(const char *filename, struct zd_dynb *buf)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "open file error\n");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    zd_dynb_resize(buf, 2 * size);
    fread(buf->base, 1, size, fp);
    buf->base[size] = '\0';

    fclose(fp);
}

static void generate_IR(struct zd_dyna *insts, struct zd_dyna *addr_stack, char *code)
{
    char *ptr = code;

    while (*ptr) {
        char c = *(ptr++);
        switch (c) {
        case OP_SHL:
        case OP_SHR:
        case OP_INC:
        case OP_DEC:
        case OP_OUT:
        case OP_IN: {
            size_t count = 1;
            while (*ptr) {
                if (c != *ptr) break;
                count++;
                ptr++;
            }

            struct instruction inst = {
                .opcode = c,
                .operand = count
            };
            zd_dyna_append(insts, &inst);
        } break;

        case OP_JZ: {
            size_t addr = insts->count;   /* record the address of current instruction '[' */
            struct instruction inst = {
                .opcode = c,
                .operand = 0    /* backpatching when we match the closed paren */
            };

            zd_dyna_append(insts, &inst);
            zd_dyna_append(addr_stack, &addr);
        } break;

        case OP_JNZ: {
            if (addr_stack->count == 0) {
                fprintf(stderr, "ERROR: unbalanced parentheses\n");
                exit(1);
            }

            size_t *p_addr = (size_t *) zd_dyna_get(addr_stack, addr_stack->count - 1);
            zd_dyna_remove(addr_stack, addr_stack->count - 1, NULL);
            struct instruction inst = {
                .opcode = c,
                .operand = *p_addr + 1
            };

            zd_dyna_append(insts, &inst);
            struct instruction *backpatching_inst = (struct instruction *) zd_dyna_get(insts, *p_addr);
            backpatching_inst->operand = insts->count;
        } break;

        default:
            /* fprintf(stderr, "unknow character: %c\n", c); */
            break;
        }
    }
}

static void interpreter(struct zd_dyna *insts)
{
    struct zd_dynb mem = {0};
    zd_dynb_resize(&mem, 1024);
    memset(mem.base, 0, mem.capacity);
    char *dp = mem.base;    /* data pointer */
    size_t pc = 0;          /* program counter */
     
    while (pc < insts->count) {
        struct instruction *inst = (struct instruction *) zd_dyna_get(insts, pc);
        switch (inst->opcode) {
        case OP_SHL:
            dp -= inst->operand;
            if (dp < mem.base) {
                fprintf(stderr, "ERROR: memory underflow\n");
                exit(1);
            }
            pc++;
            break;

        case OP_SHR:
            dp += inst->operand;
            if (dp >= mem.base + mem.capacity) {
                fprintf(stderr, "ERROR: memory overflow\n");
                exit(1);
            }
            pc++;
            break;

        case OP_INC:
            *dp += inst->operand;
            pc++;
            break;

        case OP_DEC:
            *dp -= inst->operand;
            pc++;
            break;

        case OP_OUT:
            for (size_t i = 0; i < inst->operand; i++)
                fputc(*dp, stdout);
            pc++;
            break;

        case OP_IN:
            for (size_t i = 0; i < inst->operand; i++)
                *dp = fgetc(stdin);
            pc++;
            break;

        case OP_JZ:
            if (*dp == 0) pc = inst->operand;
            else          pc++;
            break;

        case OP_JNZ:
            if (*dp != 0) pc = inst->operand;
            else          pc++;
            break;

        default: 
            break;
        }
    }

    zd_dynb_destroy(&mem);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ./bfjit <FILE>\n"); 
        return 1;
    }

    struct zd_dynb code_buf = {0};
    struct zd_dyna insts = {0};
    struct zd_dyna addrs = {0};
    const char *program = argv[1];

    zd_dyna_init(&insts, sizeof(struct instruction));
    zd_dyna_init(&addrs, sizeof(size_t));

    read_file(program, &code_buf);
    generate_IR(&insts, &addrs, code_buf.base);
    interpreter(&insts);

#if 0
    for (size_t i = 0; i < insts.count; i++) {
        struct instruction *inst = (struct instruction *) zd_dyna_get(&insts, i);
        printf("[%zu] '%c' (%zu)\n", i, inst->opcode, inst->operand);
    }
#endif

    zd_dyna_destroy(&addrs, NULL);
    zd_dynb_destroy(&code_buf);
    zd_dyna_destroy(&insts, NULL);

    return 0;
}
