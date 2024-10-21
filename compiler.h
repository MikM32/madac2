#ifndef COMPILER_H_INCLUDED
#define COMPILER_H_INCLUDED

#include "vm.h"
#include "token.h"

#define POOL_INITIAL_CAP 512

typedef struct st_codepool
{
    int capacity;
    int count;
    Byte* code;

    void (*append)(struct st_codepool*, Byte);
    void (*append32)(struct st_codepool*, Dword);

}CodePool; // Piscina e' codigo :v

static void append(CodePool* self, Byte bytecode);
static void append32(CodePool* self, Dword bytecode);

void alloc_pool(CodePool* pool)
{
    if(!pool->code)
    {
        pool->code = malloc(pool->capacity);
    }
    else
    {
        pool->code = realloc(pool->code, pool->capacity);
    }

    if(!pool->code)
    {
        fprintf(stderr, "CodePool| AllocErr: no se pudo alojar el codepool por falta de memoria.\n");
        fprintf(stderr, "CodePool| AllocErr: allocSize=%d.\n", pool->capacity);
        exit(-1);
    }
}

void initCodePool(CodePool* pool)
{
    pool->append = append;
    pool->append32 = append32;
    pool->capacity = POOL_INITIAL_CAP;
    pool->code = NULL;
    pool->count = 0;
    alloc_pool(pool);
}

void destroyCodePool(CodePool* pool)
{
    free(pool->code);
    pool->capacity = POOL_INITIAL_CAP;
    pool->count = 0;
}

static void append(CodePool* self, Byte bytecode)
{
    if(self->count >= self->capacity)
    {
        self->capacity *= 2; // Aumenta el tamaño del arreglo dinamico
        alloc_pool(self);
    }

    self->code[self->count]=bytecode;
    self->count++;
}

void set32(CodePool* self, size_t pos, Dword bytecode)
{
    if(self->count >= self->capacity)
    {
        self->capacity *= 2; // Aumenta el tamaño del arreglo dinamico
        alloc_pool(self);
    }

    self->code[pos] = GET_BYTE(bytecode, 3);
    self->code[pos+1] = GET_BYTE(bytecode, 2);
    self->code[pos+2] = GET_BYTE(bytecode, 1);
    self->code[pos+3] = GET_BYTE(bytecode, 0);
}

static void append32(CodePool* self, Dword bytecode)
{
    if(self->count >= self->capacity)
    {
        self->capacity *= 2; // Aumenta el tamaño del arreglo dinamico
        alloc_pool(self);
    }

    self->code[self->count] = GET_BYTE(bytecode, 3);
    self->count++;
    self->code[self->count] = GET_BYTE(bytecode, 2);
    self->count++;
    self->code[self->count] = GET_BYTE(bytecode, 1);
    self->count++;
    self->code[self->count] = GET_BYTE(bytecode, 0);
    self->count++;
}

//Devuelve el opcode de la operacion respectiva a su tipo de token
enum Opcode resolveOperator(TokenType op_type, int isFloat)
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
        case T_LESS:
            return O_STL;
        case T_BIGGER:
            return O_STB;
        case T_LESS_EQ:
            return O_STLE;
        case T_BIGGER_EQ:
            return O_STBE;
        default:
            break;
    }
    return O_NOP;
}


void disassemble(CodePool* pool)
{
    int pc = 0;
    while(pc < pool->count)
    {
        switch(pool->code[pc])
        {
        case O_LOAD:
            printf("LOAD ");
            break;
        default:
            break;
        }
        pc++;
    }
}

void printPool(CodePool* pool)
{
    for(int i=0; i<pool->count; i++)
    {
        printf("0x%x ", pool->code[i]);
    }
    printf("\n");
}


// Emitters




#endif // COMPILER_H_INCLUDED
