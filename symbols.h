#ifndef SYMBOLS_H_INCLUDED
#define SYMBOLS_H_INCLUDED

#include "hashtable.h"

typedef struct st_symVar
{
    char* var_name;
    uint16_t id;
    uint16_t scope;

}SymVar;

typedef struct st_symbolTable
{
    Hashmap vars;
}SymbolTable;

SymVar* newSymVar(char* var_name, uint16_t id, uint16_t scope_level)
{
    SymVar* var = malloc(sizeof(SymVar));
    if(!var)
    {
        exit(-1);
    }

    var->var_name = var_name;
    var->id = id;
    var->scope = scope_level;

    return var;
}

void initSymbolTable(SymbolTable* st)
{
    initHashmap(&st->vars);
}

#endif // SYMBOLS_H_INCLUDED
