#ifndef SEMANTIC_H_INCLUDED
#define SEMANTIC_H_INCLUDED

#include "parser.h"

#define EXPR_MISMATCH_ERROR 20

typedef struct st_symbolTable
{

}SymbolTable;

typedef struct st_semantic
{
    MadaParser parser;
    SymbolTable symTab;


    void (*binOpMismatchError)(struct st_semantic*, char*, int, int);
    AstTag (*checkBinop)(struct st_semantic*, Ast*);
    AstTag (*checkExpr)(struct st_semantic*, Ast*);
    void (*analyze)(struct st_semantic*);

} MadaSemantic;

static void binOpMismatchError(MadaSemantic* self, char* expr, int line, int col);
static AstTag checkBinop(MadaSemantic* self, Ast* ast_binop);
static AstTag checkExpr(MadaSemantic* self, Ast* ast_expr);
static void analyze(MadaSemantic* self);

void initMadaSemantic(MadaSemantic* self)
{
    self->binOpMismatchError = binOpMismatchError;
    self->checkBinop =checkBinop;
    self->checkExpr = checkExpr;
    self->analyze = analyze;
    initMadaParser(&self->parser);
}

// Errores
static void binOpMismatchError(MadaSemantic* self, char* expr, int line, int col)
{
    printf("TypeError[%d][%d]: Los tipos entre los operandos de \"",line, col);
    printUntil(expr, '\n');
    printf("\" no coinciden.\n");
    exit(EXPR_MISMATCH_ERROR);
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

static AstTag isRealOp(Ast* op)
{
    switch(op->tag)
    {
        case AST_BINOP:
            if(op->AST_BINOP.left->tag == AST_REAL || op->AST_BINOP.right->tag == AST_REAL)
            {
                return AST_REAL;
            }
            break;
        case AST_UNOP:
            if(op->AST_UNOP.right->tag == AST_REAL)
            {
                return AST_REAL;
            }
            break;
        default:
            break;

    }


    return AST_NONE;
}

static int isAritmethicOp(TokenType op_type)
{
    switch(op_type)
    {
        case T_PLUS:
        case T_MINUS:
        case T_MUL:
        case T_DIV:
            return 1;
            break;
        default:
            break;
    }
    return 0;
}

//Emisor de codigo maquina


static AstTag checkBinop(MadaSemantic* self, Ast* ast_binop)
{
    AstTag leftTag, rightTag;

    leftTag = checkExpr(self, ast_binop->AST_BINOP.left);
    rightTag = checkExpr(self, ast_binop->AST_BINOP.right);

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



        if(isRealOp(ast_binop))
        {
            return AST_REAL;
        }
        else
        {
            return AST_INTEGER;
        }
    }

    return AST_NONE;

}

static AstTag checkUnop(MadaSemantic* self, Ast* ast_unop)
{
    AstTag rightTag;

    rightTag = checkExpr(self, ast_unop->AST_UNOP.right);

    if(isAritmethicOp(ast_unop->AST_UNOP.op))
    {
        if(!isNumericTag(rightTag))
        {
            MadaToken token = ast_unop->token;
            self->binOpMismatchError(self, token.in_source_pos, token.line_num, token.col_num);
        }

        if(isRealOp(ast_unop))
        {
            return AST_REAL;
        }
        else
        {
            return AST_INTEGER;
        }
    }

    return AST_NONE;

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
        case AST_REAL:
        case AST_BOOL:
            return ast_expr->tag;
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

    destroyAstNode(self->parser.ast);
}

#endif // SEMANTIC_H_INCLUDED
