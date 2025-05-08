#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Brainfuck    C
//     >      ++ptr
//     <      --ptr
//     +      ++*ptr
//     -      --*ptr
//     .      fputc(*ptr, stdout);
//     ,      *ptr = fgetc(stdin);
//     [      while(*ptr) {
//     ]      }

typedef struct FileContent {
    char *buf;
    size_t size;
} FileContent;

static void read_file(const char *filename, FileContent *content)
{
    FILE *fp = fopen(filename, "r");
    assert(fp != NULL);

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = malloc(size+1);
    assert(buf != NULL);
    fread(buf, 1, size, fp);
    content->buf = buf;
    content->size = size;
}

char mem[1000] = {0};
char *ptr = mem;

static void run(char *buf, size_t size)
{
    size_t pos = 0;
    while (pos < size) {
        switch (buf[pos++]) {
        case '>': ++ptr; break;
        case '<': --ptr; break;
        case '+': ++*ptr; break;
        case '-': --*ptr; break;
        case '.': fputc(*ptr, stdout); break;
        case ',': *ptr = fgetc(stdin); break;
        case '[': {
            int num = 1;
            size_t end_pos = pos;
            while (num != 0) {
                if (buf[end_pos] == '[') num++;
                if (buf[end_pos] == ']') num--;
                end_pos++;
            }
            while (*ptr != 0) run(buf+pos, end_pos-pos-1);
            pos = end_pos;
        } break;
        case ']': break;
        default: break;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ./interpreter <FILE>\n");
        return 1;
    }

    const char *program = argv[1];
    FileContent content = {0};

    read_file(program, &content);
    run(content.buf, content.size);

    free(content.buf);

    return 0;
}
