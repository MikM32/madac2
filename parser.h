#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "lexer.h"
#include "ast.h"

#define NOSOURCECODE_ERR 10
#define VOIDTOKEN_ERR 9
#define UNEXPECTED_TOKEN_ERR 8

typedef struct st_parser
{
    MadaLexer lexer;
    Ast *ast;

    void (*match)(struct st_parser*, TokenType expected_type);
    void (*parse)(struct st_parser*);
    void (*parseError)(struct st_parser*, MadaToken token, TokenType expected);

    Ast* (*primary)(struct st_parser*);
    Ast* (*unary)(struct st_parser*);
    Ast* (*factor)(struct st_parser*);
    Ast* (*term)(struct st_parser*);
    Ast* (*comparison)(struct st_parser*);
    Ast* (*expression)(struct st_parser*);
    Ast* (*vardecl)(struct st_parser*);

} MadaParser;

static void match(MadaParser* self, TokenType expected_type);
static void parseError(MadaParser* self, MadaToken token, TokenType expected);

static Ast* primary(MadaParser* self);
static Ast* unary(MadaParser* self);
static Ast* factor(MadaParser* self);
static Ast* term(MadaParser* self);
static Ast* comparison(MadaParser* self);
static Ast* vardecl(MadaParser* self);

static void parse(MadaParser* self);

void initMadaParser(MadaParser* self)
{
    self->match = match;
    self->primary = primary;
    self->unary = unary;
    self->factor = factor;
    self->term = term;
    self->comparison = comparison;
    self->vardecl = vardecl;
    self->parse = parse;
    self->parseError = parseError;
    self->ast = NULL;
    initMadaLexer(&self->lexer);
}

static void parseError(MadaParser* self, MadaToken token, TokenType expected)
{
    char* msg;
    if(token.val != NULL)
    {
        msg = token.val;
    }
    else
    {
        msg = str_tok[token.type];
    }

    fprintf(stderr, "SyntaxError[%d][%d]: se esperaba \"%s\" en lugar de \"%s\"\n", token.line_num, token.col_num, str_tok[expected], msg);
    exit(UNEXPECTED_TOKEN_ERR);
}

static void exprError(MadaParser* self, MadaToken token)
{
    fprintf(stderr, "SyntaxError[%d][%d]: Expresion incompleta\n", token.line_num, token.col_num);
    exit(-1);
}

static void match(MadaParser* self, TokenType expected_type)
{
    if(self->lexer.source == NULL)
    {
        printf("LexerError: El lexer no ha cargado un codigo fuente.\n");
        exit(NOSOURCECODE_ERR);
    }

    if(self->lexer.current_token.type == T_NONE)
    {
        printf("LexerError: El lexer no ha iniciado el recorrido.\n");
        exit(VOIDTOKEN_ERR);
    }


    if(self->lexer.current_token.type != expected_type)
    {
        self->parseError(self, self->lexer.current_token, expected_type);
    }

}

static Ast* primary(MadaParser* self)
{
    Ast* ast_node=NULL;
    MadaToken token_aux;

    switch(self->lexer.current_token.type)
    {

        case T_OPAREN:

            self->lexer.next(&self->lexer);

            ast_node = self->term(self);
            self->match(self, T_CPAREN);

            self->lexer.next(&self->lexer);

            break;

        case T_INTEGER:

            ast_node = astInteger(self->lexer.current_token);
            //destroyMadaToken(&self->lexer.current_token);

            self->lexer.next(&self->lexer);
            break;
        case T_REAL:

            ast_node = astReal(self->lexer.current_token);
            //destroyMadaToken(&self->lexer.current_token);

            self->lexer.next(&self->lexer);
            break;
        case T_BOOL:

            ast_node = astBool(self->lexer.current_token);
            //destroyMadaToken(&self->lexer.current_token);

            self->lexer.next(&self->lexer);
            break;
        case T_ID:

            token_aux = self->lexer.current_token;
            self->lexer.next(&self->lexer);

            // Llamadas a funciones (SOLO FUNCIONES YA QUE SE USAN EN EXPRESIONES AL RETORNAR UN VALOR)
            /*
            if(self->lexer.current_token.type = T_OPAREN)
            {
                self->lexer.next(&self->lexer);
            }*/

            ast_node = astVar(token_aux);
            //destroyMadaToken(&token_aux);

            break;
        case T_EOL:
        case T_EOF:
            exprError(self, self->lexer.current_token);
            break;
        default:
            exprError(self, self->lexer.current_token);
            break;

    }

    return ast_node;

}

static Ast* unary(MadaParser* self)
{
    Ast* ast_node = NULL;

    if(self->lexer.current_token.type == T_MINUS)
    {
        MadaToken curToken = self->lexer.current_token;
        self->lexer.next(&self->lexer);
        ast_node = astUnop(curToken, self->primary(self));
    }
    else
    {
        ast_node = self->primary(self);
    }

    // FUTURO: Aqui podria ir un if que verifique si la operacion que se realiza es una desreferencia

    return ast_node;
}

static Ast* factor(MadaParser* self)
{
    Ast* ast_node = self->unary(self);

    if(self->lexer.current_token.type == T_MUL || self->lexer.current_token.type == T_DIV)
    {
        MadaToken curToken = self->lexer.current_token;
        self->lexer.next(&self->lexer);

        ast_node = astBinop(ast_node, curToken, self->factor(self));
    }

    return ast_node;
}

static Ast* term(MadaParser* self)
{
    Ast* ast_node = self->factor(self);

    if(self->lexer.current_token.type == T_PLUS || self->lexer.current_token.type == T_MINUS)
    {
        MadaToken curToken = self->lexer.current_token;
        self->lexer.next(&self->lexer);

        ast_node = astBinop(ast_node, curToken, self->term(self));
    }

    return ast_node;
}

static Ast* comparison(MadaParser* self)
{
    Ast* ast_node = self->term(self);

    if(self->lexer.current_token.type == T_LESS || self->lexer.current_token.type == T_LESS_EQ
       || self->lexer.current_token.type == T_BIGGER || self->lexer.current_token.type == T_BIGGER_EQ)
    {
        MadaToken curToken = self->lexer.current_token;
        self->lexer.next(&self->lexer);

        ast_node = astBinop(ast_node, curToken, self->comparison(self));
    }

    return ast_node;
}

static Ast* vardecl(MadaParser* self)
{
    Ast* ast_node = NULL;

    self->match(self, T_ID);

    MadaToken var_token = self->lexer.current_token;
    self->lexer.next(&self->lexer);

    self->match(self, T_COLON);

    self->lexer.next(&self->lexer);

    if(self->lexer.current_token.type == T_INT_TYPE || self->lexer.current_token.type == T_REAL_TYPE
        || self->lexer.current_token.type == T_ID)
    {
        ast_node = astVarDecl(var_token, self->lexer.current_token);

        self->lexer.next(&self->lexer);
        self->match(self, T_EOL);
        self->lexer.next(&self->lexer);
    }
    else
    {
        exit(-10);
    }

    return ast_node;
}

static Ast* varblock(MadaParser* self)
{
    Ast* ast_node = NULL;
    List vardecls;

    initLinkedList(&vardecls, 0);

    self->match(self, T_EOL);
    self->lexer.next(&self->lexer);

    self->match(self, T_VAR);
    self->lexer.next(&self->lexer);
    self->match(self, T_EOL);
    self->lexer.next(&self->lexer);

    while(self->lexer.current_token.type != T_BEGIN && self->lexer.current_token.type != T_EOF)
    {
        appendLinkedList(&vardecls, self->vardecl(self));
        // Continuar aqui
    }

    if(self->lexer.current_token.type == T_EOF)
    {
        exit(-1);
        //error
    }

    ast_node = astVarBlock(vardecls);

    return ast_node;
}

                                        // Se agrega un parametro adicional para poder construir el objeto Ast con su token de id
static Ast* var_assign(MadaParser* self, MadaToken id_token)
{
    Ast* ast_node = NULL;

    self->match(self, T_ASSIGN);
    self->lexer.next(&self->lexer);

    ast_node = astVarAssign(id_token, self->comparison(self));

    return ast_node;
}

static Ast* statement(MadaParser* self)
{
    Ast* ast_node = NULL;
    MadaToken token_aux;


    switch(self->lexer.current_token.type)
    {
        case T_ID:
            token_aux = self->lexer.current_token;
            self->lexer.next(&self->lexer);

            if(self->lexer.current_token.type == T_ASSIGN) // Asignacion a variables
            {
                ast_node = var_assign(self, token_aux);
            }
            else if(self->lexer.current_token.type == T_OPAREN) // LLamadas a funciones o procedimientos
            {
                //no implementado aun
            }

            break;
        default:
            break;
    }

    return ast_node;
}

static Ast* compound_statement(MadaParser* self)
{
    Ast* ast_node = NULL;
    Ast* ast_acum = NULL;
    List statements;

    self->match(self, T_BEGIN);
    self->lexer.next(&self->lexer);

    self->match(self, T_EOL);
    self->lexer.next(&self->lexer);

    initLinkedList(&statements, 0);

    while(self->lexer.current_token.type != T_END && self->lexer.current_token.type != T_EOF)
    {
        ast_acum = statement(self);
        if(ast_acum) appendLinkedList(&statements, ast_acum);

        self->lexer.next(&self->lexer);
    }

    if(self->lexer.current_token.type == T_EOF)
    {
        exit(-1);
        //error
    }

    ast_node = astCompoundStmt(statements);

    return ast_node;
}

static Ast* algorithm(MadaParser* self)
{
    Ast* ast_node = NULL;
    Ast* varblock_node = NULL;
    Ast* compound_stmts_node = NULL;

    self->match(self, T_ALG);

    self->lexer.next(&self->lexer);

    self->match(self, T_ID);
    self->lexer.next(&self->lexer);

    varblock_node = varblock(self);
    compound_stmts_node = compound_statement(self);

    ast_node = astAlgorithm(varblock_node, compound_stmts_node);

    return ast_node;
}

static void parse(MadaParser* self)
{
    self->lexer.next(&self->lexer);
    self->ast = algorithm(self);
}

#endif // PARSER_H_INCLUDED
