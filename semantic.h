#ifndef SEMANTIC_H_INCLUDED
#define SEMANTIC_H_INCLUDED

#include "parser.h"
#include "compiler.h"
#include "symbols.h"

#define EXPR_MISMATCH_ERROR 20
#define EXPR_UNDECL_VAR_ERROR 20


typedef struct st_semantic
{
    MadaParser parser;
    CodePool cpool;
    SymbolTable symTab;


    void (*binOpMismatchError)(struct st_semantic*, char*, int, int);
    void (*undeclaredVarError)(struct st_semantic*, char*, int, int);
    AstTag (*checkBinop)(struct st_semantic*, Ast*);
    AstTag (*checkVar)(struct st_semantic*, Ast*);
    AstTag (*checkExpr)(struct st_semantic*, Ast*);
    void (*analyze)(struct st_semantic*);

} MadaSemantic;

static void binOpMismatchError(MadaSemantic* self, char* expr, int line, int col);
static void undeclaredVarError(MadaSemantic* self, char* var, int line, int col);
static AstTag checkBinop(MadaSemantic* self, Ast* ast_binop);
static AstTag checkVar(MadaSemantic* self, Ast* ast_var);
static AstTag checkExpr(MadaSemantic* self, Ast* ast_expr);
static void analyze(MadaSemantic* self);

void initMadaSemantic(MadaSemantic* self)
{
    self->binOpMismatchError = binOpMismatchError;
    self->undeclaredVarError = undeclaredVarError;
    self->checkBinop =checkBinop;
    self->checkExpr = checkExpr;
    self->checkVar = checkVar;
    self->analyze = analyze;
    initMadaParser(&self->parser);
    initCodePool(&self->cpool);
    initSymbolTable(&self->symTab);
}


// Errores
static void binOpMismatchError(MadaSemantic* self, char* expr, int line, int col)
{
    char msg[] = "TypeError[%d][%d]: Los tipos entre los operandos de \"";
    printf(msg,line, col);
    printUntil(expr, '\n');
    printf("\" no coinciden.\n");

    int msg_len = strlen(msg);
    printArrow(msg_len, col-1);
    printArrow(msg_len, col-1);

    exit(EXPR_MISMATCH_ERROR);
}

static void undeclaredVarError(MadaSemantic* self, char* var, int line, int col)
{
    printf("UndeclaredError[%d][%d]: La variable \"%s\" no ha sido declarada.\n", line, col, var);

    exit(EXPR_UNDECL_VAR_ERROR);
}

// Utiles
static int isNumericTag(AstTag tag)
{
    if(tag == AST_INTEGER || tag == AST_REAL)
    {
        return 1;
    }

    return 0;
}

static AstTag isRealAst(AstTag operand)
{
    if(operand == AST_REAL)
    {
        return 1;
    }

    return 0;
}

static TokenType isAritmethicOp(TokenType op_type)
{
    switch(op_type)
    {
        case T_PLUS:
        case T_MINUS:
        case T_MUL:
        case T_DIV:
            return op_type;
            break;
        default:
            break;
    }
    return T_NONE;
}

static enum Opcode resolveOperator(TokenType op_type, int isFloat)
{
    switch(op_type)
    {
        case T_PLUS:
            if(isFloat) return O_ADDF;
            return O_ADD;
            break;
        case T_MINUS:
            if(isFloat) return O_SUBF;
            return O_SUB;
            break;
        case T_MUL:
            if(isFloat) return O_MULF;
            return O_MUL;
            break;
        case T_DIV:
            if(isFloat) return O_DIVF;
            return O_DIV;
            break;
        default:
            break;
    }
    return O_NOP;
}

//Emisor de codigo maquina


static AstTag checkBinop(MadaSemantic* self, Ast* ast_binop)
{
    AstTag leftTag, rightTag;

    leftTag = checkExpr(self, ast_binop->AST_BINOP.left);
    rightTag = checkExpr(self, ast_binop->AST_BINOP.right);

    AstTag res = AST_NONE;

    if(isAritmethicOp(ast_binop->AST_BINOP.op))
    {

        if(!isNumericTag(leftTag))
        {
            MadaToken token = ast_binop->AST_BINOP.left->token;
            self->binOpMismatchError(self, token.in_source_pos, token.line_num, token.col_num);
        }
        else if(!isNumericTag(rightTag))
        {
            MadaToken token = ast_binop->AST_BINOP.left->token;
            self->binOpMismatchError(self, token.in_source_pos, token.line_num, ast_binop->AST_BINOP.right->token.col_num);
        }

        res = AST_INTEGER;

        int isReal=0;

        if(isRealAst(ast_binop->AST_BINOP.right->tag))
        {
            isReal = 1;
            res = AST_REAL;
        }

        if(isRealAst(ast_binop->AST_BINOP.left->tag))
        {
            isReal = 1;
            res = AST_REAL;
        }

        Byte operation = resolveOperator(ast_binop->AST_BINOP.op, isReal);

        self->cpool.append(&self->cpool, O_POP);
        self->cpool.append(&self->cpool, R2);
        self->cpool.append(&self->cpool, O_POP);
        self->cpool.append(&self->cpool, R1);
        self->cpool.append(&self->cpool, operation);
        self->cpool.append(&self->cpool, R1);
        self->cpool.append(&self->cpool, R2);
        self->cpool.append(&self->cpool, O_PUSH);
        self->cpool.append(&self->cpool, R1);


        return res;


    }

    return AST_NONE;

}

static AstTag checkUnop(MadaSemantic* self, Ast* ast_unop)
{
    AstTag rightTag;
    AstTag res = AST_NONE;

    rightTag = checkExpr(self, ast_unop->AST_UNOP.right);

    if(isAritmethicOp(ast_unop->AST_UNOP.op))
    {
        if(!isNumericTag(rightTag))
        {
            MadaToken token = ast_unop->token;
            self->binOpMismatchError(self, token.in_source_pos, token.line_num, token.col_num);
        }

        if(isRealAst(ast_unop->AST_UNOP.right->tag))
        {
            return AST_REAL;
        }
        else
        {
            self->cpool.append(&self->cpool, O_POP);
            self->cpool.append(&self->cpool, R1);
            self->cpool.append(&self->cpool, O_NEG);
            self->cpool.append(&self->cpool, R1);
            self->cpool.append(&self->cpool, O_PUSH);
            self->cpool.append(&self->cpool, R1);
            return AST_INTEGER;
        }

    }

    return AST_NONE;

}

static AstTag checkVar(MadaSemantic* self, Ast* ast_var)
{
    char* var_name = ast_var->AST_VAR.varname;
    MadaToken token = ast_var->token;

    SymVar* var = (SymVar*)getHashmap(&self->symTab.vars, var_name);

    if(!var)
    {
        self->undeclaredVarError(self, var_name, token.line_num, token.col_num);
    }

    return AST_VAR;
}

static AstTag checkExpr(MadaSemantic* self, Ast* ast_expr)
{
    switch(ast_expr->tag)
    {
        case AST_BINOP:
            return checkBinop(self, ast_expr);
            break;
        case AST_UNOP:
            return checkUnop(self, ast_expr);
            break;
        case AST_INTEGER:

            self->cpool.append(&self->cpool, O_LI);
            self->cpool.append(&self->cpool, R1);
            self->cpool.append32(&self->cpool, (Dword)ast_expr->AST_INTEGER.int_value);
            self->cpool.append(&self->cpool, O_PUSH);
            self->cpool.append(&self->cpool, R1);

            return ast_expr->tag;
            break;
        case AST_REAL:

            self->cpool.append(&self->cpool, O_LI);
            self->cpool.append(&self->cpool, R1);
            self->cpool.append32(&self->cpool, (Dword)ast_expr->AST_REAL.flt_value);
            self->cpool.append(&self->cpool, O_PUSH);
            self->cpool.append(&self->cpool, R1);


            return ast_expr->tag;
            break;
        case AST_BOOL:
            return ast_expr->tag;
            break;
        case AST_VAR:
            return self->checkVar(self, ast_expr);
            break;
        default:
            break;
    }

    return AST_NONE;
}

static void analyze(MadaSemantic* self)
{
    self->parser.parse(&self->parser);
    self->checkExpr(self, self->parser.ast);

    self->cpool.append(&self->cpool, O_SYSCALL);
    self->cpool.append(&self->cpool, S_PRINTINT);
    self->cpool.append(&self->cpool, O_SYSCALL);
    self->cpool.append(&self->cpool, S_EXIT);

    destroyAstNode(self->parser.ast);
}

#endif // SEMANTIC_H_INCLUDED
