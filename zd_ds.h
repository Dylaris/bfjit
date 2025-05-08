/*
 * Author: Dylaris
 * Copyright (c) 2025
 * License: MIT
 * Date: 2025-05-07
 *
 * All rights reserved
 */

#ifndef ZD_DS_H
#define ZD_DS_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef ZD_DS_DEF
#ifdef ZD_DS_STATIC
#define ZD_DS_DEF static
#else
#define ZD_DS_DEF extern
#endif /* ZD_DS_STATIC */
#endif /* ZD_DS_DEF */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 'zd_ds_cast(ptr, (int *))' (do not ignore the paren around type) */
#ifndef zd_ds_cast
#define zd_ds_cast(ptr, type) (type (ptr))  
#endif /* zd_ds_cast */

#if defined(ZD_DS_DYNAMIC_BUFFER) || defined(ZD_DS_ALL)

struct zd_dynb {
    char *base;
    size_t capacity;
};

ZD_DS_DEF void zd_dynb_resize(struct zd_dynb *db, int size);
ZD_DS_DEF void zd_dynb_destroy(struct zd_dynb *db);

#endif /* defined(ZD_DS_DYNAMIC_BUFFER) || defined(ZD_DS_ALL) */

#if defined(ZD_DS_DYNAMIC_ARRAY) || defined(ZD_DS_ALL)

struct zd_dyna {
    void *items;
    size_t count;
    size_t capacity;
    size_t size;    /* size of each element */
};

ZD_DS_DEF void zd_dyna_init(struct zd_dyna *da, size_t size);
ZD_DS_DEF void zd_dyna_append(struct zd_dyna *da, void *elem);
ZD_DS_DEF void zd_dyna_remove(struct zd_dyna *da, size_t idx, void (*clear_item)(void *));
ZD_DS_DEF void *zd_dyna_set(struct zd_dyna *da, size_t idx, void *elem, void (*clear_item)(void *));
ZD_DS_DEF void *zd_dyna_get(struct zd_dyna *da, size_t idx);
ZD_DS_DEF void zd_dyna_destroy(struct zd_dyna *da, void (*clear_item)(void *));
ZD_DS_DEF void *zd_dyna_next(struct zd_dyna *da);

#endif /* defined(ZD_DS_DYNAMIC_ARRAY) || defined(ZD_DS_ALL) */

#if defined(ZD_DS_STRING) || defined(ZD_DS_ALL)

struct zd_string {
    char *buf;
    size_t length;
    size_t capacity;
};

ZD_DS_DEF void zd_string_append(struct zd_string *str, void *new_str, size_t size);
ZD_DS_DEF struct zd_string zd_string_sub(struct zd_string *str, size_t src, size_t dest);
ZD_DS_DEF void zd_string_destroy(struct zd_string *str);

#endif /* defined(ZD_DS_STRING) || defined(ZD_DS_ALL) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ZD_DS_H */

#ifdef ZD_DS_IMPLEMENTATION

#if defined(ZD_DS_DYNAMIC_ARRAY) || defined(ZD_DS_ALL)

ZD_DS_DEF void zd_dyna_init(struct zd_dyna *da, size_t size)
{
    da->items = NULL;
    da->count = 0;
    da->capacity = 0;
    da->size = size;
}

ZD_DS_DEF void zd_dyna_append(struct zd_dyna *da, void *elem)
{
    if (da->capacity <= da->count) {
        #define ZD_DA_INIT_CAP 10
        da->capacity = (da->capacity == 0) ? ZD_DA_INIT_CAP : (2 * da->capacity);
        #undef ZD_DA_INIT_CAP
        da->items = realloc(da->items, da->size * da->capacity);
        assert(da->items != NULL);
    }
    void *dest = zd_dyna_get(da, da->count++);
    memcpy(dest, elem, da->size);
}

ZD_DS_DEF void zd_dyna_remove(struct zd_dyna *da, size_t idx, void (*clear_item)(void *))
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
ZD_DS_DEF void *zd_dyna_set(struct zd_dyna *da, size_t idx, void *elem, void (*clear_item)(void *))
{
    if (idx >= da->count) return NULL;
    void *dest = zd_dyna_get(da, idx);
    if (clear_item != NULL && dest != NULL) clear_item(dest);
    memcpy(dest, elem, da->size);
    return dest;
}

/* zd_dyna_get means: return da[idx]; */
ZD_DS_DEF void *zd_dyna_get(struct zd_dyna *da, size_t idx)
{
    if (idx >= da->count) return NULL;
    return (char *) da->items + da->size * idx;
}

ZD_DS_DEF void zd_dyna_destroy(struct zd_dyna *da, void (*clear_item)(void *))
{
    if (clear_item != NULL) {
        for (size_t i = 0; i < da->count; i++) {
            void *item = (char *) da->items + da->size * i;
            clear_item(item);
        }
    }
    free(da->items);
    da->count = 0;
    da->capacity = 0;
    da->size = 0;
}

/* A iterator */
ZD_DS_DEF void *zd_dyna_next(struct zd_dyna *da)
{
    static size_t pos = 0;
    if (pos >= da->count) pos = 0;
    return zd_dyna_get(da, pos++);
}

#endif /* defined(ZD_DS_DYNAMIC_ARRAY) || defined(ZD_DS_ALL) */

#if defined(ZD_DS_STRING) || defined(ZD_DS_ALL)

ZD_DS_DEF void zd_string_append(struct zd_string *str, void *new_str, size_t len)
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

ZD_DS_DEF struct zd_string zd_string_sub(struct zd_string *str, size_t src, size_t dest)
{
    struct zd_string res = {0};
    if (src >= str->length || dest > str->length || src >= dest) return res;
    zd_string_append(&res, str->buf + src, dest - src);
    return res;
}

ZD_DS_DEF void zd_string_destroy(struct zd_string *str)
{
    if (str->buf != NULL) free(str->buf);
    str->buf = NULL;
    str->length = 0;
    str->capacity = 0;
}

#endif /* defined(ZD_DS_STRING) || defined(ZD_DS_ALL) */

#if defined(ZD_DS_DYNAMIC_BUFFER) || defined(ZD_DS_ALL)

ZD_DS_DEF void zd_dynb_resize(struct zd_dynb *db, int size)
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

ZD_DS_DEF void zd_dynb_destroy(struct zd_dynb *db)
{
    if (db->base != NULL) {
        free(db->base);
        db->base = NULL;
    }
    db->capacity = 0;
}

#endif /* defined(ZD_DS_DYNAMIC_BUFFER) || defined(ZD_DS_ALL) */

#endif /* ZD_DS_IMPLEMENTATION */
