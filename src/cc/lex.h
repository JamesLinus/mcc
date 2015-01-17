#ifndef cc_lex_h
#define cc_lex_h

#define EOI  -1

// token
enum {
#define _a(x, y, z)     x,
#define _x(a, b, c, d)  a=c,
#define _t(a, b, c, d)  a,
#include "token.h"
    TOKEND
};

union value {
    long i;
    unsigned long u;
    float f;
    long double d;
    void *p;
};

typedef struct {
    const char *file;
    unsigned line;
    unsigned column;
} Source;

typedef struct {
    int id;
    const char *name;
    union value v;
    Source src;
} Token;

extern Token *token;

// lex
extern const char * token_print_function(void *data);

extern void init_input();
extern int  gettok();
extern int  lookahead();
extern void match(int t);
extern const char *tname(int t);

#endif
