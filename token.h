#ifndef TOKEN_H_INCLUDED
#define TOKEN_H_INCLUDED

#include <string.h>

typedef enum enum_toktype
{
    T_NONE=0,
    T_ID=1,
    T_INTEGER = 2,
    T_REAL,
    T_BOOL,
    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_OPAREN,
    T_CPAREN,
    T_ALG,
    T_BEGIN,
    T_END,
    T_EOL,
    T_EOF

}TokenType;

typedef struct st_token
{
    TokenType type;
    char* val;
    char* in_source_pos; // apunta a la direccion del caracter que empieza la cadena del token en el codigo fuente original (o raw)
    int line_num;
    int col_num;
} MadaToken;

void initMadaToken(MadaToken* token, TokenType type, char* val, char* raw_pos, int line_num, int col_num)
{
    token->type = type;

    if(val != NULL)
    {
        int str_size = strlen(val)+1; // Le suma 1 para que copie tambien el byte nulo
        token->val = malloc(str_size);
        memcpy(token->val,val, str_size);
    }
    else
    {
        token->val = val;
    }

    token->line_num = line_num;
    token->col_num = col_num;
    token->in_source_pos = raw_pos;
}

void destroyMadaToken(MadaToken* token)
{
    if(token->type != T_NONE)
    {
        token->type = T_NONE;
    }

    if(token->val != NULL)
    {
        free(token->val);
        token->val= NULL;
    }
}

#endif // TOKEN_H_INCLUDED
