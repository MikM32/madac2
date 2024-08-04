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
    Ast* (*expression)(struct st_parser*);

} MadaParser;

static void match(MadaParser* self, TokenType expected_type);
static void parseError(MadaParser* self, MadaToken token, TokenType expected);

static Ast* primary(MadaParser* self);
static Ast* unary(MadaParser* self);
static Ast* factor(MadaParser* self);
static Ast* term(MadaParser* self);

static void parse(MadaParser* self);

void initMadaParser(MadaParser* self)
{
    self->match = match;
    self->primary = primary;
    self->unary = unary;
    self->factor = factor;
    self->term = term;
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
        msg = self->lexer.str_toktype[token.type];
    }

    printf("SyntaxError[%d]: se esperaba \"%s\" en lugar de \"%s\"\n", token.line_num, str_tok[expected], msg);
    exit(UNEXPECTED_TOKEN_ERR);
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
            /*
            if(self->lexer.current_token.type = T_OPAREN)
            {
                self->lexer.next(&self->lexer);
            }*/

            ast_node = astVar(token_aux);
            //destroyMadaToken(&token_aux);

            break;
        default:
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

static void parse(MadaParser* self)
{
    self->lexer.next(&self->lexer);
    self->ast = self->term(self);
}

#endif // PARSER_H_INCLUDED
