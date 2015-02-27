#include "cc.h"

static struct type * specifiers(int *sclass);

static int kinds[] = {
#define _a(a, b, c, d)  d,
#define _x(a, b, c, d)  c,
#define _t(a, b, c)     c,
#include "token.h"
};

int kind(int t)
{
    if (t < 0)
	return 0;
    else if (t < 128)
	return kinds[t];
    else if (t >= ID && t < TOKEND)
        return kinds[128+t-ID];
    else
        return 0;
}

int is_typename(struct token *t)
{
    return kind(t->id) & (TYPE_SPEC|TYPE_QUAL) || is_typedef_name(t->name);
}

static void func_declarator()
{
    
}

static struct node * parameter_type_list()
{
    struct node *ret = NULL;

    enterscope();

    for (;;) {
	struct type *basety = NULL;
	int sclass;

	basety = specifiers(&sclass);

	
    }

    if (SCOPE > PARAM)
	exitscope();

    return ret;
}

static struct node * func_proto(struct type *ftype)
{
    struct node *ret = NULL;

    if ((token->id != ID && kind(token->id) & FIRST_DECL) ||
	(token->id == ID && is_typedef_name(token->name))) {

	ret = parameter_type_list();
	    
    } else if (token->id == ID) {
	enter_scope();

	if (SCOPE > PARAM)
	    exit_scope();
    } else if (token->id != ')') {
	error("invalid token '%k' in parameter list", token);
    }
    
    return ret;
}

static struct type * func_or_array()
{
    struct type *ty = NULL;
    
    for (; token->id == '(' || token->id == '['; ) {
        if (token->id == '[') {
            struct type *atype = array_type();
            match('[');
	    if (token->id == STATIC) {
		match(STATIC);
		
	    } else if (kind(token->id) & TYPE_QUAL) {
		
	    } else if (kind(token->id) & FIRST_ASSIGN_EXPR) {
		assign_expression();
	    } else if (token->id == '*') {
		match('*');
	    }
	    skipto(']');
            attach_type(&ty, atype);
        } else {
	    struct type *ftype = function_type();
            match('(');
            ftype->u.f.proto = func_proto(ftype);
	    skipto(')');
            attach_type(&ty, ftype);
        }
    }
        
    return ty;
}

static struct type * abstract_func_or_array()
{
    struct type *ty = NULL;

    for (; token->id == '(' || token->id == '['; ) {
	if (token->id == '[') {
	    struct type *atype = array_type();
	    match('[');
	    if (token->id == '*') {
		if (lookahead()->id != ']') 
		    assign_expression();
		else
		    match('*');
	    } else if (kind(token->id) & FIRST_ASSIGN_EXPR) {
		assign_expression();
	    }
	    skipto(']');
	    attach_type(&ty, atype);
	} else {
	    struct type *ftype = function_type();
	    match('(');
	    ftype->u.f.proto = parameter_type_list();
	    skipto(')');
	    attach_type(&ty, ftype);
	}
    }
    
    return ty;
}

static struct type * pointer()
{
    struct type *ret = NULL;
    int con, vol, res, type;
    
    assert(token->id == '*');
    
    for (;;) {
        int *p, t = token->id;
        switch (token->id) {
            case CONST:
                p = &con;
                break;
                
            case VOLATILE:
                p = &vol;
                break;
                
            case RESTRICT:
                p = &res;
                break;
                
            case '*':
            {
                struct type *pty = pointer_type();
                con = vol = res = type = 0;
                p = &type;
                prepend_type(&ret, pty);
            }
                break;
                
            default:
                p = NULL;
                break;
        }
        
        if (p == NULL)
            break;
        
        if (*p != 0)
            warning("duplicate type qulifier '%k'", token);
        
        *p = t;
        
        if (t == CONST || t == VOLATILE || t == RESTRICT)
            ret = qual(t, ret);

	gettok();
    }
        
    return ret;
}

static struct decl * enum_decl()
{
    return NULL;
}

static struct decl * record_decl()
{
    return NULL;
}

static struct type * specifiers(int *sclass)
{
    int cls, sign, size, type;
    int cons, vol, res, inl;
    struct type *basety;
    int ci;			// _Complex, _Imaginary
        
    basety = NULL;
    cls = sign = size = type = 0;
    cons = vol = res = inl = 0;
    ci = 0;

    for (;;) {
        int *p, t = token->id;
	unsigned line = src.line;
        switch (token->id) {
	case AUTO:
	case EXTERN:
	case REGISTER:
	case STATIC:
	case TYPEDEF:
	    p = &cls;
	    gettok();
	    break;
                
	case CONST:
	    p = &cons;
	    gettok();
	    break;
                
	case VOLATILE:
	    p = &vol;
	    gettok();
	    break;
                
	case RESTRICT:
	    p = &res;
	    gettok();
	    break;
                
	case INLINE:
	    p = &inl;
	    gettok();
	    break;
                
	case ENUM:
	    p = &type;
	    basety = enum_decl()->node.symbol->type;
	    break;
                
	case STRUCT:
	case UNION:
	    p = &type;
	    basety = record_decl()->node.symbol->type;
	    break;
             
	case LONG:
	    if (size == LONG) {
		t = LONG + LONG;
		size = 0;	// clear
	    }
	    // go through
	case SHORT:
	    p = &size;
	    gettok();
	    break;
            
	case FLOAT:
	    p = &type;
	    basety = floattype;
	    gettok();
	    break;
	    
	case DOUBLE:
	    p = &type;
	    basety = doubletype;
	    gettok();
	    break;
	    
	case VOID:
	    p = &type;
	    basety = voidtype;
	    gettok();
	    break;
	    
	case CHAR:
	    p = &type;
	    basety = chartype;
	    gettok();
	    break;
	    
	case INT:
	    p = &type;
	    basety = inttype;
	    gettok();
	    break;
	    
	case _BOOL:
	    p = &type;
	    basety = booltype;
	    gettok();
	    break;
                
	case SIGNED:
	case UNSIGNED:
	    p = &sign;
	    gettok();
	    break;

	case _COMPLEX:
	case _IMAGINARY:
	    p = &ci;
	    gettok();
	    break;
                
	case ID:
	    if (is_typedef_name(token->name)) {
		p = &type;
		gettok();
	    } else {
		p = NULL;
	    }
	    break;
	    
	default:
	    p = NULL;
	    break;
        }
        
        if (p == NULL)
            break;
        
        if (*p != 0) {
	    if (p == &cls)
		errorf(line, "duplicate storage class specifier at '%s'", tname(t));
	    else if (p == &cons || p == &res || p == &vol || p == &inl)
		warningf(line, "duplicate '%s' declaration specifier", tname(t));
	    else if (p == &ci)
		errorf(line, "duplicate _Complex/_Imaginary specifier at '%s'", tname(t));
	    else if (p == &sign)
		errorf(line, "duplicate signed/unsigned speficier at '%s'", tname(t));
	    else if (p == &type || p == &size)
		errorf(line, "duplicate type specifier at '%s'", tname(t));
	    else
		assert(0);
	}
        
        *p = t;
    }
    
    // default is int
    if (type == 0) {
        if (sign == 0 && size == 0)
	    error("missing type specifier");
        type = INT;
        basety = inttype;
    }
    
    // type check
    if ((size == SHORT && type != INT) ||
        (size == LONG + LONG && type != INT) ||
        (size == LONG && type != INT && type != DOUBLE)) {
        if (size == LONG + LONG)
            error("%s %s %s is invalid", tname(size/2), tname(size/2), tname(type));
        else
            error("%s %s is invalid", tname(size), tname(type));
    } else if ((sign && type != INT && type != CHAR)) {
        error("'%s' cannot be signed or unsigned", tname(type));
    } else if ((ci && type != DOUBLE && type != FLOAT)) {
	error("'%s' cannot be %s", tname(type), tname(ci));
    }
    
    if (type == CHAR && sign)
        basety = sign == UNSIGNED ? unsignedchartype : signedchartype;
    else if (size == SHORT)
        basety = sign == UNSIGNED ? unsignedshorttype : shorttype;
    else if (type == INT && size == LONG)
        basety = sign == UNSIGNED ? unsignedlongtype : longtype;
    else if (size == LONG + LONG)
        basety = sign == UNSIGNED ? unsignedlonglongtype : longlongtype;
    else if (type == DOUBLE && size == LONG)
        basety = longdoubletype;
    else if (sign == UNSIGNED)
        basety = unsignedinttype;
    
    // qulifier
    if (cons)
        basety = qual(CONST, basety);
    if (vol)
        basety = qual(VOLATILE, basety);
    if (res)
        basety = qual(RESTRICT, basety);
    if (inl)
        basety = qual(INLINE, basety);
    
    *sclass = cls;
        
    return basety;
}

struct decl * initializer_list()
{
    return NULL;
}

struct decl * declaration()
{
    BEGIN_CALL(declaration);
    Log.v("%d %k", token->id, token);
    END_CALL(declaration);
    return NULL;
}

static void abstract_declarator(struct type **ty)
{
    assert(ty);

    if (token->id == '*') {
	struct type *pty = pointer();
	prepend_type(ty, pty);
    }

    if (token->id == '(') {
	if (kind(lookahead()->id) & FIRST_DECL) {
	    struct type *faty = abstract_func_or_array();
	    prepend_type(ty, faty);
	} else {
	    match('(');
	    abstract_declarator(ty);
	    match(')');
	}
    } else if (token->id == '[') {
	struct type *faty = abstract_func_or_array();
	prepend_type(ty, faty);
    } else {
	error("expect '(' or '[' at '%k'", token);
    }
}

static void declarator(struct type **ty, const char **id)
{
    assert(ty && id);
    
    if (token->id == '*') {
	struct type *pty = pointer();
	prepend_type(ty, pty);
    }

    if (token->id == ID) {
	*id = token->name;
	match(ID);
	if (token->id == '[' || token->id == '(') {
	    struct type *faty = func_or_array();
	    prepend_type(ty, faty);
	}
    } else if (token->id == '(') {
	match('(');
	declarator(ty, id);
	match(')');
    } else {
	error("expect identifier or '(' at '%k'", token);
    }
}

static struct decl * external_decl()
{
    struct decl *ret = NULL;
    struct type *basety;
    int sclass;

    basety = specifiers(&sclass);
    if (token->id == ID || token->id == '*' || token->id == '(') {
	const char *id = NULL;
	struct type *ty = NULL;

	// declarator
	declarator(&ty, &id);
	
    } else if (token->id == ';') {
	// struct/union/enum
    } else {
	error("invalid token '%k' in declaration", token);
    }

    return ret;
}

struct decl * translation_unit()
{
    struct decl *ret = decl_node(TU_DECL, GLOBAL);
    struct node *node = NULL;
    
    for (; token->id != EOI; ) {
	if (kind(token->id) & FIRST_DECL) {
	    struct node *node1 = concat_node(NODE(external_decl()), NULL);
	    if (node)
	        node->kids[1] = node1;
	    else
		ret->node.kids[0] = node1;

	    node = node1;
	} else {
	    error("invalid token '%k'", token);
	    gettok();
	}
    }
    return ret;
}

