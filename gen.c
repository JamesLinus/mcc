#include "cc.h"

static FILE *outfp;

/**
 *  X86_64 registers
 *
 *  64-bit  32-bit  16-bit  8-bit
 *  rax     eax     ax      al,ah
 *  rbx     ebx     bx      bl,bh
 *  rcx     ecx     cx      cl,ch
 *  rdx     edx     dx      dl,dh
 *  rbp     ebp     bp
 *  rsp     esp     sp
 *  rsi     esi     si
 *  rdi     edi     di
 *  rip     eip     ip
 *  r8~r15
 *
 *  Segment registers
 *
 *  cs,ds,es,fs,gs,ss
 */

enum {
    RAX, RBX, RCX, RDX,
    RSI, RDI,
    R8, R9,
    R10, R11, R12, R13, R14, R15,
    INT_REGS
};

enum {
    XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7,
    XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
    FLOAT_REGS
};

static struct reg *iarg_regs[NUM_IARG_REGS];
static struct reg *farg_regs[NUM_FARG_REGS];
static struct reg *int_regs[INT_REGS];
static struct reg *float_regs[FLOAT_REGS];
static struct reg * rsp = &(struct reg){
    .r[Q] = "%rsp",
    .r[L] = "%esp",
    .r[W] = "%sp"
};
static struct reg * rbp = &(struct reg){
    .r[Q] = "%rbp",
    .r[L] = "%ebp",
    .r[W] = "%bp"
};
static struct reg * rip = &(struct reg){
    .r[Q] = "%rip",
    .r[L] = "%eip",
    .r[W] = "%ip"
};
static int idx[] = {
    -1, B, W, -1, L, -1, -1, -1, Q
};
static const char *suffix[] = {
    "b", "w", "l", "q"
};

static void emit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(outfp, "\t");
    vfprintf(outfp, fmt, ap);
    fprintf(outfp, "\n");
    va_end(ap);
}

static void emit_noindent(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(outfp, fmt, ap);
    fprintf(outfp, "\n");
    va_end(ap);
}

static inline struct reg *mkreg(struct reg *r)
{
    struct reg *reg = xmalloc(sizeof(struct reg));
    *reg = *r;
    return reg;
}

static void init_regs(void)
{
    int_regs[RAX] = mkreg(&(struct reg){
            .r[Q] = "%rax",
                .r[L] = "%eax",
                .r[W] = "%ax",
                .r[B] = "%al"});

    int_regs[RBX] = mkreg(&(struct reg){
            .r[Q] = "%rbx",
                .r[L] = "%ebx",
                .r[W] = "%bx",
                .r[B] = "%bl"});
    
    int_regs[RCX] = mkreg(&(struct reg){
            .r[Q] = "%rcx",
                .r[L] = "%ecx",
                .r[W] = "%cx",
                .r[B] = "%cl"});
    
    int_regs[RDX] = mkreg(&(struct reg){
            .r[Q] = "%rdx",
                .r[L] = "%edx",
                .r[W] = "%dx",
                .r[B] = "%dl"});

    int_regs[RSI] = mkreg(&(struct reg){
            .r[Q] = "%rsi",
                .r[L] = "%esi",
                .r[W] = "%si",
                .r[B] = "%sil"});

    int_regs[RDI] = mkreg(&(struct reg){
            .r[Q] = "%rdi",
                .r[L] = "%edi",
                .r[W] = "%di",
                .r[B] = "%dil"});

    for (int i = R8; i <= R15; i++) {
        int index = i - R8 + 8;
        int_regs[i] = mkreg(&(struct reg){
                                .r[Q] = format("%%r%d", index),
                                .r[L] = format("%%r%dd", index),
                                .r[W] = format("%%r%dw", index),
                                .r[B] = format("%%r%db", index)
                            });
    }

    // init integer regs
    iarg_regs[0] = int_regs[RDI];
    iarg_regs[1] = int_regs[RSI];
    iarg_regs[2] = int_regs[RDX];
    iarg_regs[3] = int_regs[RCX];
    iarg_regs[4] = int_regs[R8];
    iarg_regs[5] = int_regs[R9];

    // init floating regs
    for (int i = XMM0; i <= XMM15; i++) {
        const char *name = format("%%xmm%d", i - XMM0);
        float_regs[i] = mkreg(&(struct reg){.r[Q] = name, .r[L] = name});
        if (i <= XMM7)
            farg_regs[i - XMM0] = float_regs[i];
    }
}

static struct reg * get_int_reg(struct tac *tac)
{
    
}

static struct reg * get_float_reg(struct tac *tac)
{
    
}

static void dispatch_reg(struct operand *operand)
{
    node_t *sym = operand->sym;
    struct addr **addrs = SYM_X_ADDRS(sym);
    
    // if in reg
    if (addrs[ADDR_REGISTER]) {
        return;
    }

    // if has free reg
    for (int i = 0; i < ARRAY_SIZE(int_regs); i++) {
        struct reg *reg = int_regs[i];
        if (vec_len(reg->vars) == 0) {
            // select

            return;
        }
    }
}

/*
  Integer Conversion

  1. signed => signed

  widden

  int8 => int64: movsbq (Signed+Byte+Quad)
  int8 => int32: movsbl (Signed+Byte+Long)
  int8 => int16: movsbw (Signed+Byte+Word)

  int16 => int64: movswq (Signed+Word+Quad)
  int16 => int32: movswl (Signed+Word+Long)

  int32 => int64: cltq (Convert+Long+To+Quad)

  narrow

  int64 => int32: movq, movl
  int64 => int16: movq, movw
  int64 => int8: movq, movb

  int32 => int16: movl, movw
  int32 => int8: movl, movb

  int16 => int8: movzwl, movb

  2. unsigned => unsigned

  widden

  uint8 => uint64: movzbl, movq
  uint8 => uint32: movzbl, movl
  uint8 => uint16: movzbl, movw

  uint16 => uint64: movzwl, movq
  uint16 => uint32: movzwl, movl

  uint32 => uint64: movl, movq


  narrow

  uint64 => uint32: movq, movl
  uint64 => uint16: movq, movw
  uint64 => uint8: movq, movb

  uint32 => uint16: movl, movw
  uint32 => uint8: movl, movb

  uint16 => uint8: movzwl, movb

  3. signed => unsigned

  int8 => uint8: movzbl, movb
  int8 => uint16: movsbw, movw
  int8 => uint32: movsbl, movl
  int8 => uint64: movsbq, movq

  int16 => uint8: movzwl, movb
  int16 => uint16: movzwl, movw
  int16 => uint32: movswl, movl
  int16 => uint64: movswq, movq

  int32 => uint8: movl, movb
  int32 => uint16: movl, movw
  int32 => uint32: movl, movl
  int32 => uint64: movl, cltq

  int64 => uint8: movq, movb
  int64 => uint16: movq, movw
  int64 => uint32: movq, movl
  int64 => uint64: movq, movq

  4. unsigned => signed

  uint8 => int8: movzbl, movb
  uint8 => int16: movzbl, movw
  uint8 => int32: movzbl, movl
  uint8 => int64: movzbl, movq

  uint16 => int8: movzwl, movb
  uint16 => int16: movzwl, movw
  uint16 => int32: movzwl, movl
  uint16 => int64: movzwl, movq

  uint32 => int8: movl, movb
  uint32 => int16: movl, movw
  uint32 => int32: movl, movl
  uint32 => int64: movl, movq

  uint64 => int8: movq, movb
  uint64 => int16: movq, movw
  uint64 => int32: movq, movl
  uint64 => int64: movq, movq
 */

static void conv_si_si(struct tac *tac)
{
    if (tac->from_opsize < tac->to_opsize) {
        // widden
    } else if (tac->from_opsize > tac->to_opsize) {
        // narrow
    }
}

static void get_reg(struct tac *tac)
{
    switch (tac->op) {
        // bop
    case IR_ADDI:
    case IR_ADDF:
    case IR_SUBI:
    case IR_SUBF:
    case IR_MULI:
    case IR_MULF:
    case IR_IMULI:
    case IR_DIVI:
    case IR_DIVF:
    case IR_IDIVI:
    case IR_MOD:
    case IR_OR:
    case IR_AND:
    case IR_XOR:
    case IR_LSHIFT:
    case IR_RSHIFT:
        break;

        // uop
    case IR_NOT:
    case IR_MINUSI:
    case IR_MINUSF:
        break;

    case IR_ASSIGNI:
    case IR_ASSIGNF:
        break;

    case IR_PARAM:
    case IR_CALL:
        break;

    case IR_CONV_FF:
        break;
    case IR_CONV_F_SI:
    case IR_CONV_F_UI:
    case IR_CONV_SI_F:
    case IR_CONV_UI_F:
        break;
    case IR_CONV_SI_SI:
        conv_si_si(tac);
        break;
    case IR_CONV_SI_UI:
    case IR_CONV_UI_SI:
    case IR_CONV_UI_UI:
        break;

    case IR_IF_I:
    case IR_IF_F:
    case IR_IF_FALSE_I:
    case IR_IF_FALSE_F:
        break;

    case IR_RETURNI:
    case IR_RETURNF:
        break;

    case IR_LABEL:
    case IR_GOTO:
        break;

    case IR_SUBSCRIPT:
    case IR_ADDRESS:
    case IR_INDIRECTION:
        break;
        
    case IR_NONE:
        break;

    default:
        cc_assert(0);
    }
}

// static struct addr * make_addr_with_type(int kind)
// {
//     struct addr *addr = zmalloc(sizeof(struct addr));
//     addr->kind = kind;
//     return addr;
// }

// static struct addr * make_memory_addr(void)
// {
//     return make_addr_with_type(ADDR_MEMORY);
// }

// static struct addr * make_stack_addr(void)
// {
//     return make_addr_with_type(ADDR_STACK);
// }

// static struct addr * make_register_addr(void)
// {
//     return make_addr_with_type(ADDR_REGISTER);
// }

static void emit_tac(struct tac *tac)
{
    get_reg(tac);
    switch (tac->op) {
        // bop
    case IR_ADDI:
    case IR_ADDF:
    case IR_SUBI:
    case IR_SUBF:
    case IR_MULI:
    case IR_MULF:
    case IR_IMULI:
    case IR_DIVI:
    case IR_DIVF:
    case IR_IDIVI:
    case IR_MOD:
    case IR_OR:
    case IR_AND:
    case IR_XOR:
    case IR_LSHIFT:
    case IR_RSHIFT:
        break;

        // uop
    case IR_NOT:
    case IR_MINUSI:
    case IR_MINUSF:
        break;

    case IR_ASSIGNI:
    case IR_ASSIGNF:
        break;

    case IR_PARAM:
    case IR_CALL:
        break;

    case IR_CONV_FF:
    case IR_CONV_F_SI:
    case IR_CONV_F_UI:
    case IR_CONV_SI_F:
    case IR_CONV_UI_F:
        break;
    case IR_CONV_SI_SI:
    case IR_CONV_SI_UI:
    case IR_CONV_UI_SI:
    case IR_CONV_UI_UI:
        break;

    case IR_IF_I:
    case IR_IF_F:
    case IR_IF_FALSE_I:
    case IR_IF_FALSE_F:
        break;

    case IR_RETURNI:
    case IR_RETURNF:
        break;

    case IR_LABEL:
    case IR_GOTO:
        break;

    case IR_SUBSCRIPT:
    case IR_ADDRESS:
    case IR_INDIRECTION:
        break;
        
    case IR_NONE:
        break;

    default:
        cc_assert(0);
    }
}

static void emit_tacs(struct vector *tacs)
{
    for (int i = 0; i < vec_len(tacs); i++) {
        struct tac *tac = vec_at(tacs, i);
        emit_tac(tac);
    }
}

static void mark_die(node_t *sym)
{
    int kind = SYM_X_KIND(sym);
    if (kind == SYM_KIND_REF ||
        kind == SYM_KIND_TMP) {
        SYM_X_USES(sym).live = false;
        SYM_X_USES(sym).use_tac = NULL;
    }
}

static void mark_live(node_t *sym, struct tac *tac)
{
    int kind = SYM_X_KIND(sym);
    if (kind == SYM_KIND_REF ||
        kind == SYM_KIND_TMP) {
        SYM_X_USES(sym).live = true;
        SYM_X_USES(sym).use_tac = tac;
    }
}

static void scan_tac_uses(struct tac *tac)
{
    struct operand *result = tac->result;
    struct operand *l = tac->args[0];
    struct operand *r = tac->args[1];

    if (result)
        result->uses = SYM_X_USES(result->sym);
    if (l)
        l->uses = SYM_X_USES(l->sym);
    if (r)
        r->uses = SYM_X_USES(r->sym);
    // mark
    if (result)
        mark_die(result->sym);
    if (l)
        mark_live(l->sym, tac);
    if (r)
        mark_live(r->sym, tac);
}

static void init_sym_uses(node_t *sym)
{
    struct addr **addrs = SYM_X_ADDRS(sym);
    if (addrs[ADDR_MEMORY] ||
        addrs[ADDR_STACK]) {
        SYM_X_USES(sym).live = true;
        SYM_X_USES(sym).use_tac = NULL;
    } else {
        SYM_X_USES(sym).live = false;
        SYM_X_USES(sym).use_tac = NULL;
    }
}

static void init_tacs(struct vector *tacs)
{
    for (int i = 0; i < vec_len(tacs); i++) {
        struct tac *tac = vec_at(tacs, i);
        struct operand *result = tac->result;
        struct operand *l = tac->args[0];
        struct operand *r = tac->args[1];
        if (result)
            init_sym_uses(result->sym);
        if (l)
            init_sym_uses(l->sym);
        if (r)
            init_sym_uses(r->sym);
    }
}

static void scan_tacs(struct vector *tacs)
{
    for (int i = vec_len(tacs) - 1; i >= 0; i--) {
        struct tac *tac = vec_at(tacs, i);
        scan_tac_uses(tac);
    }
}

static void init_text(node_t *decl)
{
    struct vector *tacs = DECL_X_TACS(decl);
    init_tacs(tacs);
    scan_tacs(tacs);
}

static size_t call_stack_size(node_t *call)
{
    node_t **args = EXPR_ARGS(call);
    size_t extra_size = 0;
    int num_int = 0;
    int num_float = 0;
    for (int i = 0; i < LIST_LEN(args); i++) {
        node_t *ty = AST_TYPE(args[i]);
        size_t size = ROUNDUP(TYPE_SIZE(ty), 8);
        
        if (isint(ty) || isptr(ty)) {
            num_int++;
            if (num_int > NUM_IARG_REGS)
                extra_size += size;
        } else if (isfloat(ty)) {
            num_float++;
            if (num_float > NUM_FARG_REGS)
                extra_size += size;
        } else if (isstruct(ty) || isunion(ty)) {
            extra_size += size;
        } else {
            cc_assert(0);
        }
    }
    return extra_size;
}

static size_t extra_stack_size(node_t *decl)
{
    size_t extra_stack_size = 0;
    node_t **calls = DECL_X_CALLS(decl);
    for (int i = 0; i < LIST_LEN(calls); i++) {
        node_t *call = calls[i];
        size_t size = call_stack_size(call);
        extra_stack_size = MAX(extra_stack_size, size);
    }
    return extra_stack_size;
}

/*
  stack layout

  High  | ...           |
        +---------------+ <--- rbp+16
        | return address|
        +---------------+ <--- rbp+8
        | saved rbp     |
        +---------------+ <--- rbp
        | local vars    |
        +---------------+
        | params        |
        +---------------+
        | call params   |
  Low   +---------------+ <--- rsp
        | ...           |
 */

static void emit_function_prologue(gdata_t *gdata)
{
    node_t *decl = GDATA_TEXT_DECL(gdata);
    
    if (GDATA_GLOBAL(gdata))
        emit(".globl %s", GDATA_LABEL(gdata));
    emit(".text");
    emit_noindent("%s:", GDATA_LABEL(gdata));
    emit("pushq %s", rbp->r[Q]);
    emit("movq %s, %s", rsp->r[Q], rbp->r[Q]);

    size_t localsize = 0;
    // local vars
    for (int i = LIST_LEN(DECL_X_LVARS(decl)) - 1; i >= 0; i--) {
        node_t *lvar = DECL_X_LVARS(decl)[i];
        node_t *sym = DECL_SYM(lvar);
        node_t *ty = SYM_TYPE(sym);
        size_t size = TYPE_SIZE(ty);
        int align = TYPE_ALIGN(ty);
        localsize = ROUNDUP(localsize, align) + size;
        SYM_X_LOFF(sym) = - localsize;
    }
    localsize = ROUNDUP(localsize, 16);

    // params
    node_t *ty = SYM_TYPE(DECL_SYM(decl));
    node_t **args = TYPE_PARAMS(ty);
    size_t stack_off = 16;      // rbp+16
    for (int i = 0; i < LIST_LEN(args); i++) {
        node_t *sym = args[i];
        node_t *ty = SYM_TYPE(sym);
        size_t size = TYPE_SIZE(ty);
        int align = TYPE_ALIGN(ty);
        if (isscalar(ty)) {
            localsize = ROUNDUP(localsize, align) + size;
            SYM_X_LOFF(sym) = - localsize;
        } else if (isstruct(ty) || isunion(ty)) {
            SYM_X_LOFF(sym) = stack_off;
            stack_off += ROUNDUP(size, 8);
        } else {
            cc_assert(0);
        }
    }
    localsize = ROUNDUP(localsize, 16);

    // calls
    localsize += extra_stack_size(decl);

    if (localsize > 0)
        emit("subq $%llu, %s", localsize, rsp->r[Q]);
}

static void emit_function_params(node_t *decl)
{
    node_t *ty = SYM_TYPE(DECL_SYM(decl));
    node_t **args = TYPE_PARAMS(ty);
    int num_int = 0;
    int num_float = 0;
    
    for (int i = 0; i < LIST_LEN(args); i++) {
        node_t *sym = args[i];
        node_t *ty = SYM_TYPE(sym);
        size_t size = TYPE_SIZE(ty);
        int align = TYPE_ALIGN(ty);

        if (isint(ty) || isptr(ty)) {
            num_int++;
            if (num_int > NUM_IARG_REGS) {
                
            } else {
                struct reg *ireg = iarg_regs[num_int-1];
                emit("mov%s %s, %ld(%s)",
                     suffix[idx[size]],
                     ireg->r[idx[size]],
                     SYM_X_LOFF(sym), rbp->r[Q]);
            }
        } else if (isfloat(ty)) {
            num_float++;
            if (num_float > NUM_FARG_REGS) {
                
            } else {
                
            }
        }
    }
}

static void emit_function_epilogue(gdata_t *gdata)
{
    emit("leave");
    emit("ret");
}

static void emit_text(gdata_t *gdata)
{
    node_t *decl = GDATA_TEXT_DECL(gdata);
    
    emit_function_prologue(gdata);
    emit_function_params(decl);
    // init
    init_text(decl);
    emit_tacs(DECL_X_TACS(decl));
    emit_function_epilogue(gdata);
}

static void emit_data(gdata_t *gdata)
{
    if (GDATA_GLOBAL(gdata))
        emit(".globl %s", GDATA_LABEL(gdata));
    emit(".data");
    if (GDATA_ALIGN(gdata) > 1)
        emit(".align %d", GDATA_ALIGN(gdata));
    emit_noindent("%s:", GDATA_LABEL(gdata));
    for (int i = 0; i < LIST_LEN(GDATA_DATA_XVALUES(gdata)); i++) {
        struct xvalue *value = GDATA_DATA_XVALUES(gdata)[i];
        switch (value->size) {
        case Zero:
            emit(".zero %s", value->name);
            break;
        case Byte:
            emit(".byte %s", value->name);
            break;
        case Word:
            emit(".short %s", value->name);
            break;
        case Long:
            emit(".long %s", value->name);
            break;
        case Quad:
            emit(".quad %s", value->name);
            break;
        default:
            die("unknown size");
            break;
        }
    }
}

static void emit_bss(gdata_t *gdata)
{
    emit("%s %s,%llu,%d",
         GDATA_GLOBAL(gdata) ? ".comm" : ".lcomm",
         GDATA_LABEL(gdata),
         GDATA_SIZE(gdata),
         GDATA_ALIGN(gdata));
}

static void emit_compounds(struct dict *compounds)
{
    struct vector *keys = compounds->keys;
    if (vec_len(keys)) {
        for (int i = 0; i < vec_len(keys); i++) {
            const char *label = vec_at(compounds->keys, i);
            gdata_t *gdata = dict_get(compounds, label);
            emit_data(gdata);
        }
    }
}

static void emit_strings(struct dict *strings)
{
    struct vector *keys = strings->keys;
    if (vec_len(keys)) {
        emit(".section .rodata");
        for (int i = 0; i < vec_len(keys); i++) {
            const char *name = vec_at(strings->keys, i);
            const char *label = dict_get(strings, name);
            emit_noindent("%s:", label);
            emit(".asciz %s", name);
        }
    }
}

static void emit_floats(struct dict *floats)
{
    struct vector *keys = floats->keys;
    if (vec_len(keys)) {
        emit(".section .rodata");
        for (int i = 0; i < vec_len(keys); i++) {
            const char *name = vec_at(floats->keys, i);
            const char *label = dict_get(floats, name);
            node_t *sym = lookup(name, constants);
            cc_assert(sym);
            node_t *ty = SYM_TYPE(sym);
            emit(".align %d", TYPE_ALIGN(ty));
            emit_noindent("%s:", label);
            switch (TYPE_KIND(ty)) {
            case FLOAT:
                {
                    float f = SYM_VALUE_D(sym);
                    emit(".long %u", *(uint32_t *)&f);
                }
                break;
            case DOUBLE:
            case LONG+DOUBLE:
                {
                    double d = SYM_VALUE_D(sym);
                    emit(".quad %llu", *(uint64_t *)&d);
                }
                break;
            default:
                cc_assert(0);
            }
        }
    }
}

static void gen_init(FILE *fp)
{
    outfp = fp;
    init_regs();
}

void gen(struct externals *exts, FILE * fp)
{
    cc_assert(errors == 0 && fp);
    
    gen_init(fp);
    for (int i = 0; i < vec_len(exts->gdatas); i++) {
        gdata_t *gdata = vec_at(exts->gdatas, i);
        switch (GDATA_ID(gdata)) {
        case GDATA_BSS:
            emit_bss(gdata);
            break;
        case GDATA_DATA:
            emit_data(gdata);
            break;
        case GDATA_TEXT:
            emit_text(gdata);
            break;
        default:
            die("unknown gdata id '%d'", GDATA_ID(gdata));
            break;
        }
    }
    emit_compounds(exts->compounds);
    emit_strings(exts->strings);
    emit_floats(exts->floats);
    emit(".ident \"mcc: %d.%d\"", MAJOR(version), MINOR(version));
}
