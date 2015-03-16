// node

_ns(BEGIN_NODE_ID)

// decl
_ns(BEGIN_DECL_ID)

_n(TU_DECL,           "TranslationUnitDecl")
_n(VAR_DECL,          "VarDecl")
_n(FUNC_DECL,         "FuncDecl")
_n(TYPEDEF_DECL,      "TypedefDecl")
_n(ENUM_DECL,         "EnumDecl")
_n(STRUCT_DECL,       "StructDecl")
_n(UNION_DECL,        "UnionDecl")

_ns(END_DECL_ID)

// expr
_ns(BEGIN_EXPR_ID)

_n(BINARY_EXPR,       "BinaryExpr")
_n(UNARY_EXPR,        "UnaryExpr")
_n(ADDR_EXPR,         "AddrExpr")
_n(COMMA_EXPR,        "CommaExpr")
_n(CAST_EXPR,         "CastExpr")
_n(CALL_EXPR,         "CallExpr")
_n(ARGS_EXPR,         "ArgumentExpr")

_ns(BEGIN_LITERAL_ID)

_n(INTEGER_LITERAL,         "IntegerLiteral")
_n(FLOAT_LITERAL,           "FloatLiteral")
_n(STRING_LITERAL,          "StringLiteral")

_ns(END_LITERAL_ID)

_ns(END_EXPR_ID)

// stmt
_ns(BEGIN_STMT_ID)

_n(BREAK_STMT,        "BreakStmt")
_n(CASE_STMT,         "CaseStmt")
_n(COMPOUND_STMT,     "CompoundStmt")
_n(CONTINUE_STMT,     "ContinueStmt")
_n(DEFAULT_STMT,      "DefaultStmt")
_n(DO_WHILE_STMT,     "DoWhileStmt")
_n(ELSE_STMT,         "ElseStmt")
_n(EXPR_STMT,         "ExprStmt")
_n(FOR_STMT,          "ForStmt")
_n(GOTO_STMT,         "GotoStmt")
_n(LABEL_STMT,        "LabelStmt")
_n(IF_STMT,           "IfStmt")
_n(RETURN_STMT,       "ReturnStmt")
_n(SWITCH_STMT,       "SwitchStmt")
_n(WHILE_STMT,        "WhileStmt")

_ns(END_STMT_ID)

_ns(END_NODE_ID)

#undef _n
#undef _ns