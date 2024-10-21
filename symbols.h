#ifndef SYMBOLS_H_INCLUDED
#define SYMBOLS_H_INCLUDED

#include "hashtable.h"

typedef struct st_symType
{
    char* type_name;
    uint32_t size;
    TokenType nativeType;

}SymType;

typedef struct st_symVar
{
    char* var_name;
    uint16_t id;
    uint16_t scope;
    uint8_t is_global;

    SymType type;
}SymVar;

typedef struct st_symbolTable
{
    Hashmap vars;
    uint16_t current_scope;
}SymbolTable;

SymVar* newSymVar(char* var_name, SymType type, uint16_t id, uint16_t scope_level, uint8_t is_global)
{
    SymVar* var = malloc(sizeof(SymVar));
    if(!var)
    {
        exit(-1);
    }

    var->var_name = var_name;
    var->id = id;
    var->scope = scope_level;
    var->is_global = is_global;
    var->type = type;

    return var;
}

void initSymbolTable(SymbolTable* st)
{
    initHashmap(&st->vars);
    st->current_scope = -1;
}

#endif // SYMBOLS_H_INCLUDED
