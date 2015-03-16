#ifndef _LIB_H
#define _LIB_H

// misc.c
extern void * cc_malloc(size_t size);
extern void cc_free(void *p);
extern void die(const char *fmt, ...);
extern int array_len(void **array);

// string.c
struct string {
    char     *str;
    unsigned size;
    unsigned capelems;
    unsigned reserve;
};
extern struct string * new_string();
extern void free_string(struct string *s);

extern unsigned str_len(struct string *s);
extern void str_cats(struct string *s, char *src);
extern void str_catn(struct string *s, char *src, int len);
extern void str_catd(struct string *s, long d);

// vector.c
struct vector {
    void   **mem;
    int    elemsize;
    int    elems;
    int    capelems;
    int    reserve;
};
extern struct vector *new_vector();
extern void free_vector(struct vector *v);
extern void purge_vector(struct vector *v);

extern void * vec_at(struct vector *v, int index);
extern void vec_push(struct vector *v, void *elem);
extern int vec_len(struct vector *v);
extern void vec_add_from_array(struct vector *v, void **array);
extern void vec_add_from_vector(struct vector *v, struct vector *v2);
extern void vec_foreach(struct vector *v, void (*func)(void *elem, void *context), void *context);

// string to array
extern char * stoa(struct string *s);
// vector to array
extern void ** vtoa(struct vector *v);

#endif