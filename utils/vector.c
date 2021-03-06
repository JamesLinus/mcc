#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils.h"

#define VEC_INIT_SIZE   16

static void vec_grow(struct vector *v)
{
    void *oldmem = v->mem;
    v->alloc <<= 1;
    v->mem = xmalloc(v->alloc * sizeof(void *));
    memcpy(v->mem, oldmem, v->len * sizeof(void *));
    if (v->len > 0)
        free(oldmem);
}

struct vector *vec_new(void)
{
    struct vector *v = xmalloc(sizeof(struct vector));
    v->len = 0;
    v->alloc = VEC_INIT_SIZE;
    vec_grow(v);
    return v;
}

struct vector *vec_new1(void *val)
{
    struct vector *v = vec_new();
    vec_push(v, val);
    return v;
}

void vec_free(struct vector *v)
{
    free(v->mem);
    free(v);
}

void vec_purge(struct vector *v)
{
    for (int i = 0; i < v->len; i++)
        free(v->mem[i]);
    vec_free(v);
}

void *vec_at(struct vector *v, int index)
{
    assert(index >= 0 && index < v->len);
    return v->mem[index];
}

void *vec_at_safe(struct vector *v, int index)
{
    if (index >= 0 && index < v->len)
        return v->mem[index];
    else
        return NULL;
}

void vec_set(struct vector *v, int index, void *val)
{
    assert(index >= 0 && index < v->len);
    v->mem[index] = val;
}

void vec_set_safe(struct vector *v, int index, void *val)
{
    if (val)
        vec_set(v, index, val);
}

void *vec_head(struct vector *v)
{
    if (v->len == 0)
        return NULL;
    return v->mem[0];
}

void *vec_tail(struct vector *v)
{
    if (v->len == 0)
        return NULL;
    return v->mem[v->len - 1];
}

void vec_add(struct vector *v, struct vector *v2)
{
    for (int i = 0; i < vec_len(v2); i++)
        vec_push(v, vec_at(v2, i));
}

void vec_push(struct vector *v, void *val)
{
    assert(val);
    if (v->len == v->alloc)
        vec_grow(v);
    v->mem[v->len++] = val;
}

void vec_push_safe(struct vector *v, void *val)
{
    if (val)
        vec_push(v, val);
}

void vec_push_front(struct vector *v, void *val)
{
    assert(val);
    if (v->len == v->alloc)
        vec_grow(v);
    memmove(v->mem + 1, v->mem, v->len * sizeof(void *));
    v->mem[0] = val;
    v->len++;
}

void *vec_pop(struct vector *v)
{
    if (v->len == 0)
        return NULL;
    void *r = v->mem[v->len - 1];
    v->len--;
    return r;
}

void *vec_pop_front(struct vector *v)
{
    if (v->len == 0)
        return NULL;
    void *r = v->mem[0];
    v->len--;
    memmove(v->mem, v->mem + 1, v->len * sizeof(void *));
    return r;
}

void vec_clear(struct vector *v)
{
    v->len = 0;
}

size_t vec_len(struct vector *v)
{
    if (v == NULL)
        return 0;
    return v->len;
}

struct vector *vec_reverse(struct vector *v)
{
    struct vector *r = vec_new();
    for (int i = vec_len(v) - 1; i >= 0; i--)
        vec_push(r, vec_at(v, i));
    return r;
}

// Add elements to vector from a null-terminated array
void vec_add_array(struct vector *v, void **array)
{
    if (array == NULL)
        return;
    for (int i = 0; array[i]; i++)
        vec_push(v, array[i]);
}

void **vtoa(struct vector *v)
{
    void **array = NULL;
    int vlen = vec_len(v);
    if (vlen > 0) {
        array = zmalloc((vlen + 1) * sizeof(void *));
        memcpy(array, v->mem, vlen * sizeof(void *));
    }
    return array;
}

int array_len(void **array)
{
    int i;
    if (array == NULL)
        return 0;
    for (i = 0; array[i]; i++) ;
    return i;
}
