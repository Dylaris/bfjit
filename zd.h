/*
 * Author: Dylaris
 * Copyright (c) 2025
 * License: MIT
 * Date: 2025-05-09
 *
 * All rights reserved
 */

/*
 * Usage: Define a few macros to enable the corresponding functions
 *
 * ZD_STATIC            Restricts the function scope to static
 * ZD_IMPLEMENTATION    Includes function implementation when defined, otherwise only the header is included
 *
 * ZD_TEST              Simple testing tool
 * ZD_LOG               Simple logging for information
 * ZD_FILE              Some operations about file
 * ZD_DYNASM            A simple way to use 'dynasm' (A stupid thing :->)
 * ZD_COMMAND_LINE      Some operations about command line (option, ...)
 * ZD_DS_DYNAMIC_ARRAY  Dynamic array
 * ZD_DS_DYNAMIC_BUFFER Dynamic buffer
 * ZD_DS_STRING         String
 * ZD_DS_STACK          Stack
 */

#ifndef _ZD_H_
#define _ZD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef ZD_DEF
#ifdef ZD_STATIC
#define ZD_DEF static
#else
#define ZD_DEF extern
#endif /* ZD_STATIC */
#endif /* ZD_DEF */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef ZD_TEST

struct zd_testsuite {
    size_t pass_count;
    size_t fail_count;
    const char *name;
};

static struct zd_testsuite *__suite_ptr__;

ZD_DEF void zd_pass(const char *msg);
ZD_DEF void zd_fail(const char *msg);
ZD_DEF void zd_run_test(struct zd_testsuite *suite, char *(*test)(void));
ZD_DEF void zd_test_summary(struct zd_testsuite *suite);

#endif /* ZD_TEST */

#ifdef ZD_DS_DYNAMIC_BUFFER

struct zd_dynb {
    char *base;
    size_t capacity;
};

ZD_DEF void zd_dynb_resize(struct zd_dynb *db, int size);
ZD_DEF void zd_dynb_destroy(void *arg);

#endif /* ZD_DS_DYNAMIC_BUFFER */

#ifdef ZD_DS_DYNAMIC_ARRAY

struct zd_dyna {
    void *base;
    size_t count;
    size_t capacity;
    size_t size;    /* size of each element */
    size_t _pos;    /* iterator position */
};

ZD_DEF void zd_dyna_init(struct zd_dyna *da, size_t size);
ZD_DEF void zd_dyna_append(struct zd_dyna *da, void *elem);
ZD_DEF void zd_dyna_insert(struct zd_dyna *da, size_t idx, void *elem);
ZD_DEF void zd_dyna_remove(struct zd_dyna *da, size_t idx, void (*clear_item)(void *));
ZD_DEF void *zd_dyna_set(struct zd_dyna *da, size_t idx, void *elem, void (*clear_item)(void *));
ZD_DEF void *zd_dyna_get(struct zd_dyna *da, size_t idx);
ZD_DEF void zd_dyna_destroy(struct zd_dyna *da, void (*clear_item)(void *));
ZD_DEF void *zd_dyna_next(struct zd_dyna *da);

#endif /* ZD_DS_DYNAMIC_ARRAY */

#ifdef ZD_DS_STRING

struct zd_string {
    char *buf;
    size_t length;
    size_t capacity;
};

#ifndef ZD_IMPLEMENTATION
#define zd_string_appendm
#else
#define zd_string_appendm(p_obj, fmt, ...)                  \
    do {                                                    \
        char buf[1024];                                     \
        snprintf(buf, sizeof(buf), (fmt), ##__VA_ARGS__);   \
        zd_string_append(p_obj, buf, 0);                    \
    } while (0)
#endif
ZD_DEF void zd_string_append(struct zd_string *str, void *new_str, size_t size);
ZD_DEF struct zd_string zd_string_sub(struct zd_string *str, size_t src, size_t dest);
ZD_DEF void zd_string_destroy(void *arg);

#endif /* ZD_DS_STRING */

#ifdef ZD_DS_STACK

struct zd_stack {
    void *base;
    int top;
    size_t capacity;
    size_t size;    /* size of each element */
};

ZD_DEF void zd_stack_init(struct zd_stack *stk, size_t size);
ZD_DEF void zd_stack_push(struct zd_stack *stk, void *elem);
ZD_DEF void *zd_stack_pop(struct zd_stack *stk);
ZD_DEF void *zd_stack_top(struct zd_stack *stk);
ZD_DEF void zd_stack_destroy(struct zd_stack *stk, void (*clear_item)(void *));

#endif /* ZD_DS_STACK */

#ifdef ZD_FILE

ZD_DEF int zd_file_load(const char *filename, char **buf);
ZD_DEF int zd_file_dump(const char *filename, char *buf, size_t size);

#endif /* ZD_FILE */

#ifdef ZD_COMMAND_LINE

struct zd_cmdlopt {
    struct zd_string name; 
    struct zd_dyna vals;    /* each element is zd_string */
};

struct zd_cmdl {
    struct zd_string program;
    size_t count;
    struct zd_dyna nopts;   /* each element is zd_string */
    struct zd_dyna opts;    /* each element is zd_cmdlopt */
};

ZD_DEF void zd_cmdl_build(struct zd_cmdl *cmdl, int argc, char **argv);
ZD_DEF struct zd_dyna zd_cmdl_getopt(struct zd_cmdl *cmdl, const char *optname, int *is_valid);
ZD_DEF void zd_cmdl_destroy(void *arg);
ZD_DEF void zd_cmdlopt_destroy(void *arg);

#endif /* ZD_COMMAND_LINE */

#ifdef ZD_DYNASM

#if defined(__linux__)
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#elif defined(__WIN32)
#else
#endif

#define ZD_PAGE_SIZE 4096
#define ZD_ASSEMBLER "fasm"

ZD_DEF void *zd_dynasm_map(size_t size);
ZD_DEF void *zd_dynasm_do(char *code, void *addr);
ZD_DEF void zd_dynasm_free(void *addr);

#endif /* ZD_DYNASM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef ZD_IMPLEMENTATION

#ifdef ZD_COMMAND_LINE

ZD_DEF void zd_cmdl_build(struct zd_cmdl *cmdl, int argc, char **argv)
{
    zd_string_append(&cmdl->program, argv[0], strlen(argv[0]));
    cmdl->count = (size_t) argc;
    zd_dyna_init(&cmdl->nopts, sizeof(struct zd_string));
    zd_dyna_init(&cmdl->opts,  sizeof(struct zd_cmdlopt));

    for (int i = 1; i < argc; i++) {
        struct zd_string arg = {0}; 
        zd_string_append(&arg, argv[i], strlen(argv[i]));

        if (arg.buf[0] == '-') { /* option */
            struct zd_cmdlopt opt = {0};
            zd_dyna_init(&opt.vals, sizeof(struct zd_string));

            /* add the correspoding option values to opt */
            opt.name = arg;
            for (i = i + 1; i < argc; i++) {
                /* new option */
                if (argv[i][0] == '-') {
                    i = i - 1;  /* we have i++ at the end of outside for-loop */
                    break;
                }
                /* option value for current option */
                struct zd_string val = {0}; 
                zd_string_append(&val, argv[i], strlen(argv[i]));
                zd_dyna_append(&opt.vals, &val);
            }

            zd_dyna_append(&cmdl->opts, &opt);
        } else { /* not a option */
            zd_dyna_append(&cmdl->nopts, &arg);
        }
    }
}

ZD_DEF struct zd_dyna zd_cmdl_getopt(struct zd_cmdl *cmdl, const char *optname, int *is_valid)
{
    *is_valid = 0;
    struct zd_dyna res = {0};

    for (size_t i = 0; i < cmdl->opts.count; i++) {
        struct zd_cmdlopt *saved_opt = (struct zd_cmdlopt *) zd_dyna_get(&cmdl->opts, i);
        if (strcmp(saved_opt->name.buf, optname) == 0) {
            res = saved_opt->vals;
            *is_valid = 1;
            break;
        }
    }

    return res;
}

ZD_DEF void zd_cmdl_destroy(void *arg)
{
    struct zd_cmdl *cmdl = (struct zd_cmdl *) arg;
    zd_string_destroy(&cmdl->program);
    cmdl->count = 0;
    zd_dyna_destroy(&cmdl->nopts, zd_string_destroy);
    zd_dyna_destroy(&cmdl->opts,  zd_cmdlopt_destroy);
}

ZD_DEF void zd_cmdlopt_destroy(void *arg)
{
    struct zd_cmdlopt *opt = (struct zd_cmdlopt *) arg;
    zd_string_destroy(&opt->name);
    zd_dyna_destroy(&opt->vals, zd_string_destroy);
}

#endif /* ZD_COMMAND_LINE */

#ifdef ZD_LOG

#define ZD_LOG_COLOR_RESET   "\x1b[0m"
#define ZD_LOG_COLOR_RED     "\x1b[31m"
#define ZD_LOG_COLOR_GREEN   "\x1b[32m"
#define ZD_LOG_COLOR_YELLOW  "\x1b[33m"

#define ZD_LOG_INFO 1
#define ZD_LOG_ERRO 2
#define ZD_LOG_GOOD 3

#define zd_log(type, fmt, ...)                                                  \
    do {                                                                        \
        char buf[1024];     /* f*ck string concat in C */                       \
        switch ((type)) {                                                       \
        case ZD_LOG_INFO:                                                       \
            snprintf(buf, sizeof(buf), "[%sINFO%s] %s\n", ZD_LOG_COLOR_YELLOW,  \
                    ZD_LOG_COLOR_RESET, (fmt), ##__VA_ARGS__);                  \
            fprintf(stderr, "%s", buf);                                         \
            break;                                                              \
                                                                                \
        case ZD_LOG_ERRO:                                                       \
            snprintf(buf, sizeof(buf), "[%sERRO%s] %s\n", ZD_LOG_COLOR_RED,     \
                    ZD_LOG_COLOR_RESET, (fmt), ##__VA_ARGS__);                  \
            fprintf(stderr, "%s", buf);                                         \
            break;                                                              \
                                                                                \
        case ZD_LOG_GOOD:                                                       \
            snprintf(buf, sizeof(buf), "[%sGOOD%s] %s\n", ZD_LOG_COLOR_GREEN,   \
                    ZD_LOG_COLOR_RESET, (fmt), ##__VA_ARGS__);                  \
            fprintf(stderr, "%s", buf);                                         \
            break;                                                              \
                                                                                \
        default: break;                                                         \
        }                                                                       \
    } while(0)

#endif /* ZD_LOG */

#ifdef ZD_FILE

/* return -1 if error, the file size if success (buf is NULL means get the file size) */
ZD_DEF int zd_file_load(const char *filename, char **buf)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return -1;

    long saved_offset = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, saved_offset, SEEK_SET);

    if (buf == NULL) {
        fclose(fp);
        return size;
    }

    *buf = malloc(size+1);    
    if (*buf == NULL) {
        fclose(fp);
        return -1;
    }

    size_t read_size = fread(*buf, 1, size, fp);
    if (read_size != size) {
        free(*buf);
        *buf = NULL;
        fclose(fp);
        return -1;
    }
    (*buf)[size] = '\0';

    fclose(fp);

    return size;
}

/* return -1 if error, the file size if success */
ZD_DEF int zd_file_dump(const char *filename, char *buf, size_t size)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) return -1;

    size_t write_size = fwrite(buf, 1, size, fp);
    if (write_size != size) {
        fclose(fp);
        return -1;
    }

    return size;
}

#endif /* ZD_FILE */

#ifdef ZD_TEST

#define ZD_TEST_COLOR_RESET   "\x1b[0m"
#define ZD_TEST_COLOR_RED     "\x1b[31m"
#define ZD_TEST_COLOR_GREEN   "\x1b[32m"

#ifndef zd_type_cast
#define zd_type_cast(obj, type) (type (obj))  
#endif /* zd_type_cast */

#ifndef zd_assert
#define zd_assert(exp, msg)                                     \
    do {                                                        \
        if ((exp)) zd_pass(msg);                                \
        else       zd_fail(msg);                                \
    } while (0)
#endif /* zd_assert */

ZD_DEF void zd_pass(const char *msg)
{
    int case_id = __suite_ptr__->pass_count + __suite_ptr__->fail_count + 1;
    printf("<suite:%s> case %03d: [" ZD_TEST_COLOR_GREEN "pass" 
            ZD_TEST_COLOR_RESET "] %s\n", __suite_ptr__->name, case_id, 
            (msg != NULL) ? msg : ""); 
    fflush(stdout);
    __suite_ptr__->pass_count += 1;
}

ZD_DEF void zd_fail(const char *msg)
{
    int case_id = __suite_ptr__->pass_count + __suite_ptr__->fail_count + 1;
    printf("<suite:%s> case %03d: [" ZD_TEST_COLOR_RED "fail" 
            ZD_TEST_COLOR_RESET "] %s\n", __suite_ptr__->name, case_id,
            (msg != NULL) ? msg : ""); 
    fflush(stdout);
    __suite_ptr__->fail_count += 1;
}

ZD_DEF void zd_run_test(struct zd_testsuite *suite, char *(*test)(void))
{
    __suite_ptr__ = suite;
    char *msg = test();
    if (msg) printf("<suite:%s> %s\n", __suite_ptr__->name, msg);
}

ZD_DEF void zd_test_summary(struct zd_testsuite *suite)
{
    printf("TEST SUITE: <%s>\n", suite->name);
    printf("      PASS: %zu\n", suite->pass_count);
    printf("      FAIL: %zu\n", suite->fail_count);
    printf("     TOTAL: %zu\n", suite->pass_count + suite->fail_count);
}

#undef ZD_TEST_COLOR_RESET
#undef ZD_TEST_COLOR_RED
#undef ZD_TEST_COLOR_GREEN

#endif /* ZD_TEST */

#ifdef ZD_DS_DYNAMIC_ARRAY

ZD_DEF void zd_dyna_init(struct zd_dyna *da, size_t size)
{
    da->base = NULL;
    da->count = 0;
    da->capacity = 0;
    da->size = size;
    da->_pos = 0;
}

ZD_DEF void zd_dyna_append(struct zd_dyna *da, void *elem)
{
    if (da->capacity <= da->count) {
        #define ZD_DA_INIT_CAP 10
        da->capacity = (da->capacity == 0) ? ZD_DA_INIT_CAP : (2 * da->capacity);
        #undef ZD_DA_INIT_CAP
        da->base = realloc(da->base, da->size * da->capacity);
        assert(da->base != NULL);
    }
    void *dest = zd_dyna_get(da, da->count++);
    memcpy(dest, elem, da->size);
}

ZD_DEF void zd_dyna_insert(struct zd_dyna *da, size_t idx, void *elem)
{
    if (idx > da->count) 
        return;
    else if (idx == da->count) 
        zd_dyna_append(da, elem);
    else {
        if (da->capacity <= da->count) {
            #define ZD_DA_INIT_CAP 10
            da->capacity = (da->capacity == 0) ? ZD_DA_INIT_CAP : (2 * da->capacity);
            #undef ZD_DA_INIT_CAP
            da->base = realloc(da->base, da->size * da->capacity);
            assert(da->base != NULL);
        }
        void *dest = zd_dyna_get(da, idx);
        memcpy((char *) dest + da->size, dest, (da->count-idx) * da->size);
        memcpy(dest, elem, da->size);
        da->count++;
    }
}

ZD_DEF void zd_dyna_remove(struct zd_dyna *da, size_t idx, void (*clear_item)(void *))
{
    if (da->count > 0 && idx < da->count) {
        void *elem = zd_dyna_get(da, idx);
        if (clear_item != NULL) clear_item(elem);
        memcpy(zd_dyna_get(da, idx), zd_dyna_get(da, idx + 1),
                da->size * (da->count - idx - 1));
        da->count--;
    }
}

/* zd_dyna_set means: { clear_item(da[idx]); da[idx] = elem; } */
ZD_DEF void *zd_dyna_set(struct zd_dyna *da, size_t idx, void *elem, void (*clear_item)(void *))
{
    if (idx >= da->count) return NULL;
    void *dest = zd_dyna_get(da, idx);
    if (clear_item != NULL && dest != NULL) clear_item(dest);
    memcpy(dest, elem, da->size);
    return dest;
}

/* zd_dyna_get means: return da[idx]; */
ZD_DEF void *zd_dyna_get(struct zd_dyna *da, size_t idx)
{
    if (idx >= da->count) return NULL;
    return (char *) da->base + da->size * idx;
}

ZD_DEF void zd_dyna_destroy(struct zd_dyna *da, void (*clear_item)(void *))
{
    if (clear_item != NULL) {
        for (size_t i = 0; i < da->count; i++) {
            void *item = (char *) da->base + da->size * i;
            clear_item(item);
        }
    }
    free(da->base);
    da->count = 0;
    da->capacity = 0;
    da->size = 0;
    da->_pos = 0;
}

/* A iterator */
ZD_DEF void *zd_dyna_next(struct zd_dyna *da)
{
    if (da->_pos > da->count) da->_pos = 0;
    return zd_dyna_get(da, da->_pos++);
}

#endif /* ZD_DS_DYNAMIC_ARRAY */

#ifdef ZD_DS_STRING

ZD_DEF void zd_string_append(struct zd_string *str, void *new_str, size_t len)
{
    if (len == 0) len = strlen(new_str);
    while (str->capacity <= str->length + len + 1) {
        #define ZD_STRING_INIT_CAP 128
        str->capacity = (str->capacity == 0) ? ZD_STRING_INIT_CAP : (2 * str->capacity);
        #undef ZD_STRING_INIT_CAP
        str->buf = realloc(str->buf, str->capacity);
        assert(str->buf != NULL);
    }
    memcpy(str->buf + str->length, new_str, len);
    str->length += len;
    str->buf[str->length] = '\0';
}

ZD_DEF struct zd_string zd_string_sub(struct zd_string *str, size_t src, size_t dest)
{
    struct zd_string res = {0};
    if (src >= str->length || dest > str->length || src >= dest) return res;
    zd_string_append(&res, str->buf + src, dest - src);
    return res;
}

ZD_DEF void zd_string_destroy(void *arg)
{
    struct zd_string *str = (struct zd_string *) arg;
    if (str->buf != NULL) free(str->buf);
    str->buf = NULL;
    str->length = 0;
    str->capacity = 0;
}

#endif /* ZD_DS_STRING */

#ifdef ZD_DS_DYNAMIC_BUFFER

ZD_DEF void zd_dynb_resize(struct zd_dynb *db, int size)
{
    if (size > 0) {
        db->capacity += size;
        db->base = realloc(db->base, db->capacity);
    } else {
        size_t abs_size = (size_t) -size;
        db->capacity = (db->capacity > abs_size) ? (db->capacity - abs_size) : 0;
        db->base = realloc(db->base, db->capacity);
    }
    assert((db->capacity >  0 && db->base != NULL) ||
           (db->capacity == 0 && db->base == NULL));
}

ZD_DEF void zd_dynb_destroy(void *arg)
{
    struct zd_dynb *db = (struct zd_dynb *) arg;
    if (db->base != NULL) {
        free(db->base);
        db->base = NULL;
    }
    db->capacity = 0;
}

#endif /* ZD_DS_DYNAMIC_BUFFER */

#ifdef ZD_DS_STACK

ZD_DEF void zd_stack_init(struct zd_stack *stk, size_t size)
{
    stk->base = NULL;
    stk->top = -1;
    stk->capacity = 0;
    stk->size = size;
}

ZD_DEF void zd_stack_push(struct zd_stack *stk, void *elem)
{
    stk->top++;
    if (stk->capacity <= (size_t) stk->top) {
        #define ZD_STK_INIT_CAP 10
        stk->capacity = (stk->capacity == 0) ? ZD_STK_INIT_CAP : (2 * stk->capacity);
        #undef ZD_STK_INIT_CAP
        stk->base = realloc(stk->base, stk->size * stk->capacity);
        assert(stk->base != NULL);
    }
    void *dest = (char *) stk->base + stk->size * stk->top;
    memcpy(dest, elem, stk->size);
}

ZD_DEF void *zd_stack_pop(struct zd_stack *stk)
{
    if (stk->top == -1) return NULL; 
    return (char *) stk->base + stk->size * stk->top--;
}

ZD_DEF void *zd_stack_top(struct zd_stack *stk)
{
    if (stk->top == -1) return NULL; 
    return (char *) stk->base + stk->size * stk->top;
}

ZD_DEF void zd_stack_destroy(struct zd_stack *stk, void (*clear_item)(void *))
{
    if (clear_item != NULL) {
        for (int i = 0; i <= stk->top; i++) {
            void *item = (char *) stk->base + stk->size * i;
            clear_item(item);
        }
    }
    free(stk->base);
    stk->top = -1;
    stk->capacity = 0;
    stk->size = 0;
}

#endif /* ZD_DS_STACK */

#ifdef ZD_DYNASM

#if defined(__linux__)

ZD_DEF void *zd_dynasm_map(size_t size)
{
    if (size == 0) size = ZD_PAGE_SIZE;
    void *addr = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (addr == MAP_FAILED) return NULL;
    return addr;
}

ZD_DEF void *zd_dynasm_do(char *code, void *addr)
{
    if (code == NULL || addr == NULL) return NULL;

    /* write the code into a asm file */

    char tmp_asm[] = "/tmp/tempfile_XXXXXX";

    int asm_fd = mkstemp(tmp_asm);
    if (asm_fd < 0) return NULL;

    char buf[ZD_PAGE_SIZE];
    snprintf(buf, sizeof(buf), "use64\n%s", code);
    size_t asm_size = strlen(buf);

    if (write(asm_fd, buf, asm_size) != (ssize_t) asm_size) {
        close(asm_fd);
        unlink(tmp_asm);
        return NULL;
    }
    close(asm_fd);

    /* use assembler (fasm) to compile asm file, and output binary file */

    char tmp_bin[] = "/tmp/tempfile_XXXXXX";

    int bin_fd = mkstemp(tmp_bin);
    if (bin_fd < 0) return NULL;

    pid_t pid = fork(); 
    if (pid == 0) {
        close(bin_fd);

        /* diable the output from child process */
        int devnull_fd = open("/dev/null", O_WRONLY);
        if (devnull_fd < 0) _exit(1);

        dup2(devnull_fd, STDOUT_FILENO);
        dup2(devnull_fd, STDERR_FILENO);
        close(devnull_fd);

        execlp(ZD_ASSEMBLER, ZD_ASSEMBLER, tmp_asm, tmp_bin, NULL);
        _exit(1);
    }
    waitpid(pid, NULL, 0);

    unlink(tmp_asm);

    /* load the machine code into an mapped executable page */

    size_t read_size = read(bin_fd, buf, sizeof(buf));
    if (read_size > ZD_PAGE_SIZE) read_size = ZD_PAGE_SIZE;

    memcpy(addr, buf, read_size);

    close(bin_fd);
    unlink(tmp_bin);

    return addr;
}

ZD_DEF void zd_dynasm_free(void *addr)
{
    munmap(addr, ZD_PAGE_SIZE);
}

#elif defined(__WIN32)
#else
#endif /* platform */

#endif /* ZD_DYNASM */

#endif /* ZD_IMPLEMENTATION */

#endif /* _ZD_H_ */

