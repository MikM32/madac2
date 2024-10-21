#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

#include "linked_list.h"

enum ast_tag
{
    AST_NONE=0,
    AST_INTEGER=1,
    AST_REAL,
    AST_BOOL,
    AST_VAR,
    AST_UNOP,
    AST_BINOP,
    AST_VARDECL,
    AST_VARBLOCK,
    AST_VARASSIGN,
    AST_FORUNTIL,
    AST_COMPOUND_STMT,
    AST_CODEBLOCK,
    AST_ALG,

};

typedef enum ast_tag AstTag;

typedef struct st_ast
{
    enum ast_tag tag;
    MadaToken token;
    //int line_num;
    //char* lexem; // Lexema

    union
    {
        //Expresiones
        struct AST_INTEGER{ int int_value; } AST_INTEGER;
        struct AST_REAL{ float flt_value; } AST_REAL;
        struct AST_VAR{ char* varname; } AST_VAR;
        struct AST_UNOP{ struct st_ast* right; TokenType op;} AST_UNOP;
        struct AST_BINOP{ struct st_ast* left, *right; TokenType op;} AST_BINOP;

        //Statements (sentencias)
        struct AST_VARDECL{ char* varname; MadaToken type;} AST_VARDECL;
        struct AST_VARBLOCK{ List vardecls; } AST_VARBLOCK;
        struct AST_VARASSIGN{ char* varname;  struct st_ast* value_ast;} AST_VARASSIGN;
        struct AST_FORUNTIL{ struct st_ast* iterator;  struct st_ast* expression; struct st_ast* statements;} AST_FORUNTIL;
        struct AST_COMPOUND_STMT{ List statements; } AST_COMPOUND_STMT;
        struct AST_CODEBLOCK{ struct st_ast* compound_stmts; } AST_CODEBLOCK;
        struct AST_ALG{ struct st_ast* varblock; struct st_ast* codeblock; } AST_ALG;
    };

} Ast; // Abstract Syntax Tree

static char* newString(char* text)
{
    unsigned int text_size = strlen(text)+1;
    char* lexem = malloc(text_size);
    if(!lexem) printf("newString[error]: No hay memoria suficiente."), exit(-1);

    memcpy(lexem, text, text_size);

    return lexem;

}

Ast* astInteger(MadaToken token)
{
    Ast* ast_node = malloc(sizeof(Ast));

    ast_node->tag = AST_INTEGER;
    ast_node->token = token;
    /*ast_node->line_num = token.line_num;
    ast_node->lexem = newString(token.val);*/
    ast_node->AST_INTEGER.int_value = atoi(token.val);

    //destroyMadaToken(&token);

    return ast_node;
}

Ast* astReal(MadaToken token)
{
    Ast* ast_node = malloc(sizeof(Ast));

    ast_node->tag = AST_REAL;
    ast_node->token = token;
    /*ast_node->line_num = token.line_num;
    ast_node->lexem = newString(token.val);*/
    ast_node->AST_REAL.flt_value = atof(token.val);

    //destroyMadaToken(&token);

    return ast_node;
}

Ast* astBool(MadaToken token)
{
    Ast* ast_node = malloc(sizeof(Ast));

    ast_node->tag = AST_BOOL;
    ast_node->token = token;
    /*ast_node->line_num = token.line_num;
    ast_node->lexem = newString(token.val);*/
    if(!strcmp(token.val, "falso"))
    {
        ast_node->AST_INTEGER.int_value = 0;
    }
    else
    {
        ast_node->AST_INTEGER.int_value = 1;
    }

    //destroyMadaToken(&token);

    return ast_node;
}

Ast* astVar(MadaToken token)
{
    Ast* ast_node = malloc(sizeof(Ast));

    ast_node->tag = AST_VAR;
    ast_node->token = token;

    ast_node->AST_VAR.varname = newString(token.val);

    /*ast_node->lexem = ast_node->AST_VAR.varname;
    ast_node->line_num = token.line_num;*/

    //destroyMadaToken(&token);

    return ast_node;
}

Ast* astUnop(MadaToken token, Ast* right)
{
    Ast* ast_node = malloc(sizeof(Ast));

    ast_node->tag = AST_UNOP;
    ast_node->token = token;

    ast_node->AST_UNOP.op = token.type;
    ast_node->AST_UNOP.right = right;
    /*ast_node->line_num = token.line_num;

    unsigned int lexem_size = strlen(right->lexem) + 1; //se le suma 1 y 1 por el simbolo de la operacion y el caracter nulo
    ast_node->lexem = malloc(lexem_size+1);

    memcpy(ast_node->lexem, str_tok[token.type], 2);
    strncat(ast_node->lexem, "(", 2);
    strncat(ast_node->lexem, right->lexem, lexem_size);
    strncat(ast_node->lexem, ")", 2);*/

    return ast_node;
}

Ast* astBinop(Ast* left, MadaToken token, Ast* right)
{
    Ast* ast_node = malloc(sizeof(Ast));

    ast_node->tag = AST_BINOP;
    ast_node->token = token;
    ast_node->AST_BINOP.left = left;
    ast_node->AST_BINOP.right = right;
    ast_node->AST_BINOP.op = token.type;
    /*
    ast_node->line_num = token.line_num;

    unsigned int leftSize = strlen(left->lexem), rightSize = strlen(right->lexem);
    unsigned int lexem_size = leftSize + rightSize + 1; //se le suma 1 y 1 por el simbolo de la operacion y el caracter nulo
    ast_node->lexem = malloc(lexem_size+1);

    strncpy(ast_node->lexem, "(", 2);
    strncat(ast_node->lexem, left->lexem, leftSize+1);
    strncat(ast_node->lexem, str_tok[token.type], 1);
    strncat(ast_node->lexem, right->lexem, rightSize);
    strncat(ast_node->lexem, ")", 2);*/


    return ast_node;
}

Ast* astVarDecl(MadaToken var_token, MadaToken type_token)
{
    Ast* ast_node = malloc(sizeof(Ast));

    ast_node->tag = AST_VARDECL;
    ast_node->token = var_token;
    ast_node->AST_VARDECL.varname = var_token.val;

    ast_node->AST_VARDECL.type = type_token;

    return ast_node;
}

Ast* astVarBlock(List vardecls)
{
    Ast* ast_node = malloc(sizeof(Ast));

    ast_node->tag = AST_VARBLOCK;
    ast_node->AST_VARBLOCK.vardecls = vardecls;

    return ast_node;
}

Ast* astVarAssign(MadaToken id_token, Ast* value_ast)
{
     Ast* ast_node = malloc(sizeof(Ast));
     ast_node->tag = AST_VARASSIGN;
     ast_node->token = id_token;

     ast_node->AST_VARASSIGN.varname = id_token.val;
     ast_node->AST_VARASSIGN.value_ast = value_ast;

     return ast_node;
}

Ast* astForUntil(Ast* iterator, Ast* expr, Ast* stmts)
{
    Ast* ast_node = malloc(sizeof(Ast));
    ast_node->tag = AST_FORUNTIL;

    ast_node->AST_FORUNTIL.iterator = iterator;
    ast_node->AST_FORUNTIL.expression = expr;
    ast_node->AST_FORUNTIL.statements = stmts;

    return ast_node;
}

Ast* astCompoundStmt(List statements)
{
    Ast* ast_node = malloc(sizeof(Ast));
    ast_node->tag = AST_COMPOUND_STMT;

    ast_node->AST_COMPOUND_STMT.statements = statements;

    return ast_node;
}

Ast* astCodeBlock(Ast* compound_stmt)
{
    Ast* ast_node = malloc(sizeof(Ast));
    ast_node->tag = AST_CODEBLOCK;

    ast_node->AST_CODEBLOCK.compound_stmts = compound_stmt;
    return ast_node;
}

Ast* astAlgorithm(Ast* varblock, Ast* codeblock)
{
    Ast* ast_node = malloc(sizeof(Ast));
    ast_node->tag = AST_ALG;

    ast_node->AST_ALG.varblock = varblock;
    ast_node->AST_ALG.codeblock = codeblock;

    return ast_node;
}

void destroyAstNode(Ast* ast_node)
{
    Ast* acum = NULL, *del=NULL;
    ListIterator it;

    switch(ast_node->tag)
    {
        case AST_ALG:

            destroyAstNode(ast_node->AST_ALG.varblock);
            free(ast_node->AST_ALG.varblock);

            destroyAstNode(ast_node->AST_ALG.codeblock);
            free(ast_node->AST_ALG.codeblock);

            break;
        case AST_CODEBLOCK:

            destroyAstNode(ast_node->AST_CODEBLOCK.compound_stmts);
            free(ast_node);

            break;
        case AST_FORUNTIL:

            destroyAstNode(ast_node->AST_FORUNTIL.iterator);
            free(ast_node->AST_FORUNTIL.iterator);

            destroyAstNode(ast_node->AST_FORUNTIL.expression);
            free(ast_node->AST_FORUNTIL.expression);

            destroyAstNode(ast_node->AST_FORUNTIL.statements);
            free(ast_node->AST_FORUNTIL.statements);
            break;
        case AST_VARASSIGN:

            destroyMadaToken(&ast_node->token);

            destroyAstNode(ast_node->AST_VARASSIGN.value_ast);
            free(ast_node->AST_VARASSIGN.value_ast);

            break;
        case AST_BINOP:

            destroyAstNode(ast_node->AST_BINOP.left);
            free(ast_node->AST_BINOP.left);

            destroyAstNode(ast_node->AST_BINOP.right);
            free(ast_node->AST_BINOP.right);

            destroyMadaToken(&ast_node->token);
            break;
        case AST_UNOP:

            destroyAstNode(ast_node->AST_UNOP.right);
            free(ast_node->AST_UNOP.right);

            destroyMadaToken(&ast_node->token);
            break;
        case AST_VARDECL:
        case AST_INTEGER:
        case AST_REAL:
        case AST_BOOL:
            destroyMadaToken(&ast_node->token);
            break;
        default:
            break;
    }
}

void printAst(Ast* ast_root, int level)
{
    Ast* acum = NULL;
    ListIterator it;

    switch(ast_root->tag)
    {
        case AST_ALG:
            printf("%*cALGORITMO\n", level, ' ');

            printAst(ast_root->AST_ALG.varblock, level+2);
            printAst(ast_root->AST_ALG.codeblock, level+2);
            break;
        case AST_VARBLOCK:
            initListIterator(&it, ast_root->AST_VARBLOCK.vardecls);

            acum = iterateList(&it);
            while(acum)
            {
                printAst(acum, level+2);
                acum = iterateList(&it);
            }
            break;
        case AST_VARDECL:
            break;
        case AST_COMPOUND_STMT:
            break;
        case AST_CODEBLOCK:

            break;
        case AST_VARASSIGN:
            break;
        case AST_BINOP:

            printf("%*cOPERACION BINARIA: %s\n", level, ' ', str_tok[ast_root->AST_BINOP.op]);
            printAst(ast_root->AST_BINOP.left, level+2);
            printAst(ast_root->AST_BINOP.right, level+2);
            break;
        case AST_UNOP:

            printf("%*cOPERACION UNARIA: %s\n", level, ' ', str_tok[ast_root->AST_UNOP.op]);
            printAst(ast_root->AST_UNOP.right, level+2);
            break;
        case AST_INTEGER:
            printf("%*cENTERO: %d\n", level, ' ', ast_root->AST_INTEGER.int_value);
            break;
        case AST_REAL:
            printf("%*cREAL: %f\n", level, ' ', ast_root->AST_REAL.flt_value);
            break;
        case AST_BOOL:
            printf("%*cBOOL: ", level, ' ');
            if(ast_root->AST_INTEGER.int_value)
            {
                printf("verdadero\n");
            }
            else
            {
                printf("falso\n");
            }
            break;
        case AST_VAR:
            printf("%*cVARIABLE: %s\n", level, ' ', ast_root->AST_VAR.varname);
            break;
        default:
            break;
    }
}


#endif // AST_H_INCLUDED
