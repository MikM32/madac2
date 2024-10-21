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
    AstTag (*checkVar)(struct st_semantic*, Ast*, short, short*);
    AstTag (*checkExpr)(struct st_semantic*, Ast*);
    void (*analyze)(struct st_semantic*);

} MadaSemantic;

static void binOpMismatchError(MadaSemantic* self, char* expr, int line, int col);
static void undeclaredVarError(MadaSemantic* self, char* var, int line, int col);
static AstTag checkBinop(MadaSemantic* self, Ast* ast_binop);
static AstTag checkVar(MadaSemantic* self, Ast* ast_var, short, short*);
static AstTag checkExpr(MadaSemantic* self, Ast* ast_expr);
static AstTag checkStatement(MadaSemantic*, Ast*);
static AstTag checkCompoundStmts(MadaSemantic*, Ast*);

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
        case T_LESS:
        case T_BIGGER:
        case T_LESS_EQ:
        case T_BIGGER_EQ:
            return op_type;
            break;
        default:
            break;
    }
    return T_NONE;
}



//Emisor de codigo maquina

static AstTag checkType(MadaSemantic* self, Ast* ast_type)
{

}

static AstTag checkVarDecl(MadaSemantic* self, Ast* ast_vardcl)
{
    char* varname = ast_vardcl->AST_VARDECL.varname;
    char* typename = ast_vardcl->AST_VARDECL.type.val;

    SymType type;

    // Verifica si el nombre de la variable no coincide con el nombre del tipo
    if(ast_vardcl->AST_VARDECL.type.type == T_ID) // Es un tipo definido por el usuario
    {
        if(!strcmp(varname, typename))
        {
            fprintf(stderr, "Error el nombre de la variable y el el nombre del tipo no deben coincidir.\n");
            exit(-1);
        }
    }
    else
    {
        switch(ast_vardcl->AST_VARDECL.type.type)
        {
            case T_REAL_TYPE:
            case T_INT_TYPE:
                type.nativeType = T_INT_TYPE;
                type.size = 4; // 4 bytes
                break;
            default:
                break;
        }
    }

    SymVar* var = newSymVar(varname, type,self->symTab.vars.count, self->symTab.current_scope, 0);
    setHashmap(&self->symTab.vars, varname, var, 0);

    self->cpool.append(&self->cpool, O_ADDI);
    self->cpool.append(&self->cpool, SP);
    self->cpool.append32(&self->cpool, type.size);

    // Verifica si la variable ya ha sido declarada antes
    // Verifica si el tipo de la variable existe
}

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

static AstTag checkVar(MadaSemantic* self, Ast* ast_var, short return_ref, short* is_global)
{
    char* var_name = ast_var->AST_VAR.varname;
    MadaToken token = ast_var->token;

    SymVar* var = (SymVar*)getHashmap(&self->symTab.vars, var_name);

    if(!var)
    {
        self->undeclaredVarError(self, var_name, token.line_num, token.col_num);
    }


    self->cpool.append(&self->cpool, O_LOAD);

    if(is_global) *is_global = var->is_global;

    Byte segment;

    if(!var->is_global)
    {
        segment = SEG_STACK;
    }
    else
    {
        segment = SEG_MEMORY;
    }

    self->cpool.append(&self->cpool, FORMAT_MEM_MODE(segment, 1));

    self->cpool.append(&self->cpool, R1);
    self->cpool.append32(&self->cpool, var->id*4);

    self->cpool.append(&self->cpool, O_PUSH);
    self->cpool.append(&self->cpool, R1);


    if(return_ref) // devuelve la referencia de la variable
    {
        self->cpool.append(&self->cpool, O_LI);
        self->cpool.append(&self->cpool, R1);
        self->cpool.append32(&self->cpool, var->id*4);
        self->cpool.append(&self->cpool, O_PUSH);
        self->cpool.append(&self->cpool, R1);
    }

    switch(var->type.nativeType)
    {
        case T_INT_TYPE:
            return AST_INTEGER;
            break;
        case T_REAL_TYPE:
            return AST_REAL;
            break;
        default:
            break;
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
            return self->checkVar(self, ast_expr, 0, NULL);
            break;
        default:
            break;
    }

    return AST_NONE;
}

static AstTag checkVarAssign(MadaSemantic* self, Ast* ast_varassign, short push_var_ref, short* is_global)
{
    AstTag res = AST_NONE;

    SymVar* var;

    if(ast_varassign->AST_VARASSIGN.varname)
    {
        var = (SymVar*)getHashmap(&self->symTab.vars, ast_varassign->AST_VARASSIGN.varname);

        if(!var) exit(-10);
    }

    res = checkExpr(self, ast_varassign->AST_VARASSIGN.value_ast);

    self->cpool.append(&self->cpool, O_POP);
    self->cpool.append(&self->cpool, R1);
    self->cpool.append(&self->cpool, O_STOR);

    if(is_global) *is_global = var->is_global;

    Byte segment;

    if(!var->is_global)
    {
        segment = SEG_STACK;
    }
    else
    {
        segment = SEG_MEMORY;
    }

    self->cpool.append(&self->cpool, FORMAT_MEM_MODE(segment, 1));

    self->cpool.append(&self->cpool, R1);
    self->cpool.append32(&self->cpool, var->id*4);

    self->cpool.append(&self->cpool, O_PUSH);
    self->cpool.append(&self->cpool, R1);

    if(push_var_ref)
    {
        self->cpool.append(&self->cpool, O_LI);
        self->cpool.append(&self->cpool, R1);
        self->cpool.append32(&self->cpool, var->id*4);
        self->cpool.append(&self->cpool, O_PUSH);
        self->cpool.append(&self->cpool, R1);
    }

    return res;

}

static AstTag checkForUntil(MadaSemantic* self, Ast* ast_foruntil)
{
    AstTag res = AST_NONE;
    Dword loop_addr;
    Dword cmp_addr=0;
    Dword end_loop_addr=0;
    short is_global;

    if(ast_foruntil->AST_FORUNTIL.iterator->tag == AST_VAR)
    {
        res = checkVar(self, ast_foruntil->AST_FORUNTIL.iterator, 1, &is_global);
    }
    else
    {
        res = checkVarAssign(self, ast_foruntil->AST_FORUNTIL.iterator, 1, &is_global);
    }

    // referencia al iterador en memoria
    self->cpool.append(&self->cpool, O_POP);
    self->cpool.append(&self->cpool, R6);

    // valor del iterador
    self->cpool.append(&self->cpool, O_POP);
    self->cpool.append(&self->cpool, R5);

    // Guardar direccion de cpool aqui para hacer jump
    loop_addr = self->cpool.count;

    self->cpool.append(&self->cpool, O_LOAD);

    Byte segment = (is_global) ? SEG_MEMORY: SEG_STACK;

    self->cpool.append(&self->cpool, FORMAT_MEM_MODE(segment, 0));
    self->cpool.append(&self->cpool, R5);
    self->cpool.append(&self->cpool, R6);

    res = checkExpr(self, ast_foruntil->AST_FORUNTIL.expression);
    // valor de la expresion
    self->cpool.append(&self->cpool, O_POP);
    self->cpool.append(&self->cpool, R7);

    self->cpool.append(&self->cpool, O_JB);
    self->cpool.append(&self->cpool, R5);
    self->cpool.append(&self->cpool, R7);
    cmp_addr = self->cpool.count;
    self->cpool.append32(&self->cpool, 0); //Se parcheara mas adelante la direccion de salto

    self->cpool.append(&self->cpool, O_PUSH);
    self->cpool.append(&self->cpool, R7);

    self->cpool.append(&self->cpool, O_PUSH);
    self->cpool.append(&self->cpool, R6);

    self->cpool.append(&self->cpool, O_PUSH);
    self->cpool.append(&self->cpool, R5);

    res = checkCompoundStmts(self, ast_foruntil->AST_FORUNTIL.statements);

    self->cpool.append(&self->cpool, O_POP);
    self->cpool.append(&self->cpool, R5);

    self->cpool.append(&self->cpool, O_POP);
    self->cpool.append(&self->cpool, R6);

    self->cpool.append(&self->cpool, O_POP);
    self->cpool.append(&self->cpool, R7);

    self->cpool.append(&self->cpool, O_INC);
    self->cpool.append(&self->cpool, R5);

    self->cpool.append(&self->cpool, O_STOR);

    self->cpool.append(&self->cpool, FORMAT_MEM_MODE(segment, 0)); // carga la direccion a guardar de un registro dado
    self->cpool.append(&self->cpool, R5); // acumulador del iterador
    self->cpool.append(&self->cpool, R6); // direccion en memoria del iterador


    self->cpool.append(&self->cpool, O_JMP);
    self->cpool.append32(&self->cpool, loop_addr);

    end_loop_addr = self->cpool.count;

    // Se parchea la direccion de salto del final del bucle
    set32(&self->cpool, cmp_addr, end_loop_addr);


    return res;
}

static AstTag checkStatement(MadaSemantic* self, Ast* ast_stmt)
{
    AstTag res = AST_NONE;

    switch(ast_stmt->tag)
    {
        case AST_VARASSIGN:

            res = checkVarAssign(self, ast_stmt, 0, NULL);

            break;
        case AST_FORUNTIL:

            res = checkForUntil(self, ast_stmt);
            break;
        default:
            break;
    }

    return res;
}

static AstTag checkCompoundStmts(MadaSemantic* self, Ast* ast_compound)
{
    AstTag res = AST_NONE;
    List stmts = ast_compound->AST_COMPOUND_STMT.statements;
    Ast* current_stmt = (Ast*)dequeueList(&stmts);

    while(current_stmt)
    {
        res = checkStatement(self, current_stmt);
        current_stmt = (Ast*)dequeueList(&stmts);
    }

    return res;
}

static AstTag checkCodeBlock(MadaSemantic* self, Ast* ast_codeblock)
{
    AstTag res = AST_NONE;

    self->symTab.current_scope++;

    res = checkCompoundStmts(self, ast_codeblock->AST_CODEBLOCK.compound_stmts);

    self->symTab.current_scope--;

    return res;
}

static AstTag checkVarBlock(MadaSemantic* self, Ast* ast_varblock)
{
    AstTag res = AST_NONE;
    List vardecls = ast_varblock->AST_VARBLOCK.vardecls;
    Ast* current_vardecl = (Ast*)dequeueList(&vardecls);

    while(current_vardecl)
    {
        res = checkVarDecl(self, current_vardecl);
        current_vardecl = (Ast*)dequeueList(&vardecls);
    }

    return res;
}

static AstTag checkAlgorithm(MadaSemantic* self, Ast* ast_alg)
{
    AstTag res =AST_NONE;

    res = checkVarBlock(self, ast_alg->AST_ALG.varblock);
    res = checkCodeBlock(self, ast_alg->AST_ALG.codeblock);

    return res;
}

static void analyze(MadaSemantic* self)
{
    self->parser.parse(&self->parser);
    checkAlgorithm(self, self->parser.ast);

    //checkVarDecl(self, self->parser.ast);
    self->cpool.append(&self->cpool, O_SYSCALL);
    self->cpool.append(&self->cpool, S_PRINTINT);
    self->cpool.append(&self->cpool, O_SYSCALL);
    self->cpool.append(&self->cpool, S_EXIT);

    destroyAstNode(self->parser.ast);
}

#endif // SEMANTIC_H_INCLUDED
