#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#include <stdint.h>

#define REGS_NUM 17
#define OPCODE_MAX 64
#define DEFAULT_MEM_SIZE 2048
#define STACK_SIZE 4096

#define LSH_BYTE(num) (num << 8)
#define GET_BYTE(num, n) (num >> (n*8)) & 0xFF
#define SET_BYTE(num, byte, n) num |= (byte << (8*n))
#define GET_HEX(num, n) (num >> n) & 0xF

#define GET_BIT(num, n) (num >> n) & 1
#define SET_BIT(num, n) num |= 1 << n
#define TOGGLE_BIT(num, n) num ^= 1 << n
#define CLEAR_BIT(num, n) num &= ~(1 << n)

#ifndef MADA_ALLOC
    #define MADA_ALLOC(block, size) realloc(block, size)
#endif // MADA_ALLOC

#define ZERO_MEM(block, size) memset(block, 0, size)

#define FORMAT_MEM_MODE(segment, mode) ((segment << 4) | mode)
#define GET_MEM_SEGMENT(format) (format >> 4)
#define GET_MEM_MODE(format) (format & 0xF)



enum Opcode
{
    O_NOP=0,
    O_ADD,
    O_ADDF,
    O_SUB,
    O_SUBF,
    O_MUL,
    O_MULF,
    O_DIV, // Division entera
    O_DIVF, // Division flotante
    O_NOT, // Negacion logica
    O_NEG, // Negativo
    O_NEGF,
    O_INC,
    O_DEC,
    O_STL,
    O_STB,
    O_STLE,
    O_STBE,
    O_STOR,
    O_LOAD,
    O_LI,
    O_ADDI,
    O_POP,
    O_JMP,
    O_JNE,
    O_JLE,
    O_JB,
    O_PUSH,
    O_SYSCALL,
};

enum Registers
{
    R1, // Proposito General
    R2,
    R3,
    R4, // Loops
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    PC, // Program Counter
    SP, // Stack Pointer
    BP, // Base Pointer
    SI, // Source Index
    DI, // Destiny Index
    RF  // Flags Register
};

enum Flags
{
    F_ZERO=0,
    F_BIGGER,
    F_LOWER,
    F_CARRY,
    F_SIGN,
};

enum Segments
{
    SEG_STACK,
    SEG_MEMORY,
    SEG_CODE,
};

enum Syscalls
{
    S_PRINTINT,
    S_PRINTSTR,
    S_PUTC,
    S_EXIT
};

typedef uint8_t Byte;
typedef uint16_t Word;
typedef uint32_t Dword;

typedef struct st_vm
{
    Byte* code;
    Byte* stack;
    Dword regs[REGS_NUM];
    Byte run_flag;
    Byte* memory;
    Dword memsize;

    void (*exec)(struct st_vm*);


} MadaVm;

typedef void (*OpcodeFn)(MadaVm*);

OpcodeFn operations[OPCODE_MAX];

void alloc_mem(MadaVm* self)
{
    if(!self->memory)
    {
        self->memory = malloc(self->memsize);
    }
    else
    {
        realloc(self->memory, self->memsize);
    }


    if(self->memory == NULL)
    {
        fprintf(stderr, "MadaVm| Error al iniciar la memoria: No hay espacio suficiente.\n");
        fprintf(stderr, "MadaVm| Error al iniciar la memoria: No se pueden reservar %d Bytes.\n",self->memsize);
        exit(-1);
    }
}


void printStack();

static void o_add(MadaVm* vm);
static void o_addi(MadaVm* vm);
static void o_sub(MadaVm* vm);
static void o_mul(MadaVm* vm);
static void o_div(MadaVm* vm);
static void o_li(MadaVm* vm);
static void o_neg(MadaVm* vm);
static void o_negf(MadaVm* vm);
static void o_stl(MadaVm* vm); // Store Less
static void o_stlf(MadaVm* vm); // Store Less Float
static void o_stb(MadaVm* vm); // Store Bigger
static void o_stbf(MadaVm* vm); // Store Bigger Float
static void o_stle(MadaVm* vm); // Store Less Equal
static void o_stlef(MadaVm* vm); // Store Less Equal Float
static void o_stbe(MadaVm* vm); // Store Bigger Equal
static void o_stbef(MadaVm* vm); // Store Bigger Equal Float
static void o_inc(MadaVm* vm);
static void o_stor(MadaVm* vm);
static void o_load(MadaVm* vm);
static void o_jne(MadaVm* vm);
static void o_jle(MadaVm* vm);
static void o_jb(MadaVm* vm);
static void o_jmp(MadaVm* vm);
static void o_push(MadaVm* vm);
static void o_pop(MadaVm* vm);
static void o_syscall(MadaVm* vm);
static void exec(MadaVm* vm);
static void syscall(MadaVm* vm, Byte syscall_code);
static Byte fetch8(MadaVm* self);
static Word fetch16(MadaVm* self);
static Dword fetch32(MadaVm* self);

Byte _global_stack[STACK_SIZE]; // Se declara global para tener mas espacio en el arreglo (el stack ofrece menos espacio)
                                // Si se declara local estara alojada en el stack, al ser declarada como global se aloja en la seccion .data del binario

void initVm(MadaVm* vm)
{
    vm->exec = exec;
    vm->code = NULL;
    vm->stack = _global_stack;
    vm->memory = NULL;
    vm->memsize = DEFAULT_MEM_SIZE;
    alloc_mem(vm);
    ZERO_MEM(vm->regs, REGS_NUM*sizeof(int)); //inicializa los registros a cero.
    vm->regs[SP] = 0;
    vm->run_flag = 0;

    operations[O_ADD] = o_add;
    operations[O_ADDI] = o_addi;
    operations[O_SUB] = o_sub;
    operations[O_MUL] = o_mul;
    operations[O_DIV] = o_div;
    operations[O_LI] = o_li;
    operations[O_NEG] = o_neg;
    operations[O_NEGF] = o_negf;
    operations[O_INC] = o_inc;
    operations[O_STL] = o_stl;
    operations[O_STB] = o_stb;
    operations[O_STLE] = o_stle;
    operations[O_STBE] = o_stbe;
    operations[O_INC] = o_inc;
    operations[O_STOR] = o_stor;
    operations[O_LOAD] = o_load;
    operations[O_JNE] = o_jne;
    operations[O_JLE] = o_jle;
    operations[O_JB] = o_jb;
    operations[O_JMP] = o_jmp;
    operations[O_PUSH] = o_push;
    operations[O_POP] = o_pop;
    operations[O_SYSCALL] = o_syscall;
}

static void update_flags(MadaVm* vm, Dword value)
{
    if(!value)
    {
        SET_BIT(vm->regs[RF], F_ZERO);
    }
    else
    {
        CLEAR_BIT(vm->regs[RF], F_ZERO);
    }

    if(!((value >> 16) - 0xFFFF)) // Si es negativo pone a 1 el flag F_SIGN
    {
        SET_BIT(vm->regs[RF], F_SIGN);
    }
    else
    {
        CLEAR_BIT(vm->regs[RF], F_SIGN);
    }

    // Hace reset a las flags que no se manejan automaticamente
    CLEAR_BIT(vm->regs[RF], F_LOWER);
    CLEAR_BIT(vm->regs[RF], F_BIGGER);
}

static void o_add(MadaVm* vm)
{
    Byte reg1, reg2;//, reg3;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);

    vm->regs[reg1] = vm->regs[reg1] + vm->regs[reg2];
    update_flags(vm, vm->regs[reg1]);
}

static void o_addi(MadaVm* vm)
{
    Byte reg1;
    Dword imm;

    reg1 = fetch8(vm);
    imm = fetch32(vm);

    vm->regs[reg1] = vm->regs[reg1] + imm;
    update_flags(vm, vm->regs[reg1]);
}

static void o_sub(MadaVm* vm)
{
    Byte reg1, reg2;//, reg3;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);

    vm->regs[reg1] = vm->regs[reg1] - vm->regs[reg2];
    update_flags(vm, vm->regs[reg1]);
}

static void o_mul(MadaVm* vm)
{
    Byte reg1, reg2;//, reg3;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);

    vm->regs[reg1] = vm->regs[reg1] * vm->regs[reg2];
    update_flags(vm, vm->regs[reg1]);
}

static void o_div(MadaVm* vm)
{
    Byte reg1, reg2;//, reg3;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);

    if(!vm->regs[reg2])
    {
        fprintf(stderr, "MadaVm| RuntimeError[0x%X]: No se puede dividir entre 0.\n", vm->regs[PC]);
        exit(-1);
    }

    vm->regs[reg1] = vm->regs[reg1] / vm->regs[reg2];
    update_flags(vm, vm->regs[reg1]);
}

static void o_stl(MadaVm* vm) // Store Less
{
    Byte reg1, reg2;//, reg3;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);

    vm->regs[reg1] = vm->regs[reg1] < vm->regs[reg2];
    update_flags(vm, vm->regs[reg1]);
}

static void o_stb(MadaVm* vm) // Store Bigger
{
    Byte reg1, reg2;//, reg3;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);

    vm->regs[reg1] = vm->regs[reg1] > vm->regs[reg2];
    update_flags(vm, vm->regs[reg1]);
}

static void o_stle(MadaVm* vm) // Store Less Equal
{
    Byte reg1, reg2;//, reg3;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);

    vm->regs[reg1] = vm->regs[reg1] <= vm->regs[reg2];
    update_flags(vm, vm->regs[reg1]);
}

static void o_inc(MadaVm* vm)
{
    Byte reg1;

    reg1 = fetch8(vm);
    vm->regs[reg1]++;

    update_flags(vm, vm->regs[reg1]);
}

static void o_load(MadaVm* vm)
{
    Byte format;
    Byte reg;
    Dword dir;

    format = fetch8(vm);
    reg = fetch8(vm);

    if(GET_MEM_MODE(format))
    {
        dir = fetch32(vm);
    }
    else
    {
        dir = fetch8(vm);
        dir = vm->regs[dir];
    }

    switch(GET_MEM_SEGMENT(format))
    {
        case SEG_STACK:

            if(dir >= STACK_SIZE) exit(-12);
            for(int i=0; i<4; i++)
            {
                vm->regs[reg] = LSH_BYTE(vm->regs[reg]);
                vm->regs[reg] += vm->stack[dir++];
            }
            break;

        case SEG_MEMORY:

            for(int i=0; i<4; i++)
            {
                vm->regs[reg] = LSH_BYTE(vm->regs[reg]);
                vm->regs[reg] += vm->memory[dir++];
            }
            break;
        default:
            break;
    }
}

static void o_stor(MadaVm* vm)
{
    Byte format;
    Byte reg;
    Dword dir;

    //segment = fetch8(vm);
    format = fetch8(vm);
    reg = fetch8(vm);


    printf("reg6[%d] ", vm->regs[R6]);

    if(GET_MEM_MODE(format))
    {
        dir = fetch32(vm);

        printf("spdir[%d]\n", dir);

        Byte cur_byte=0;

        switch(GET_MEM_SEGMENT(format))
        {
            case SEG_STACK:

                if(dir >= STACK_SIZE) exit(-12);

                cur_byte = GET_BYTE(vm->regs[reg], 0);
                vm->stack[dir+3] = cur_byte;
                cur_byte = GET_BYTE(vm->regs[reg], 1);
                vm->stack[dir+2] = cur_byte;
                cur_byte = GET_BYTE(vm->regs[reg], 2);
                vm->stack[dir+1] = cur_byte;
                cur_byte = GET_BYTE(vm->regs[reg], 3);
                vm->stack[dir] = cur_byte;
                break;

            case SEG_MEMORY:
                break;

            default:
                break;
        }
    }
    else
    {
        dir = fetch8(vm);

        switch(GET_MEM_SEGMENT(format))
        {
            case SEG_STACK:
                vm->stack[vm->regs[dir]+3] = GET_BYTE(vm->regs[reg], 0);
                vm->stack[vm->regs[dir]+2] = GET_BYTE(vm->regs[reg], 1);
                vm->stack[vm->regs[dir]+1] = GET_BYTE(vm->regs[reg], 2);
                vm->stack[vm->regs[dir]] = GET_BYTE(vm->regs[reg], 3);

                printf("spdir[%d]\n",vm->regs[dir] );
                break;

            case SEG_MEMORY:
                break;

            default:
                break;
        }
    }


}

static void o_stbe(MadaVm* vm) // Store Bigger Equal
{
    Byte reg1, reg2;//, reg3;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);

    vm->regs[reg1] = vm->regs[reg1] >= vm->regs[reg2];
    update_flags(vm, vm->regs[reg1]);
}

static void o_li(MadaVm* vm)
{
    Byte reg1;
    Dword imm;

    reg1 = fetch8(vm);
    imm = fetch32(vm);

    vm->regs[reg1] = imm;
    update_flags(vm, vm->regs[reg1]);
}

static void o_neg(MadaVm* vm)
{
    Byte reg1;

    reg1 = fetch8(vm);

    vm->regs[reg1] = -vm->regs[reg1];
    update_flags(vm, vm->regs[reg1]);
}

static void o_negf(MadaVm* vm)
{
    Byte reg1;

    reg1 = fetch8(vm);

    float temp = (float)vm->regs[reg1];
    temp = -temp;

    vm->regs[reg1] = *((Dword*)&temp);
    update_flags(vm, vm->regs[reg1]);
}

static void o_syscall(MadaVm* vm)
{
    Byte syscall_code = fetch8(vm);

    syscall(vm, syscall_code);
}

static void o_jne(MadaVm* vm)
{
    Byte reg1, reg2;
    Dword addr;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);
    addr = fetch32(vm);

    printf(".\n");

    if(vm->regs[reg1] != vm->regs[reg2])
    {
        vm->regs[PC] = addr;
    }
}

static void o_jle(MadaVm* vm)
{
    Byte reg1, reg2;
    Dword addr;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);
    addr = fetch32(vm);

    printf(".\n");

    if(vm->regs[reg1] <= vm->regs[reg2])
    {
        vm->regs[PC] = addr;
    }
}

static void o_jb(MadaVm* vm)
{
    Byte reg1, reg2;
    Dword addr;

    reg1 = fetch8(vm);
    reg2 = fetch8(vm);
    addr = fetch32(vm);


    if(vm->regs[reg1] > vm->regs[reg2])
    {
        vm->regs[PC] = addr;
    }
}

static void o_jmp(MadaVm* vm)
{
    Dword addr;

    addr = fetch32(vm);

    printf(".\n");

    vm->regs[PC] = addr;

}

static void o_push(MadaVm* vm)
{
    if(vm->stack[SP] >= STACK_SIZE)
    {
        fprintf(stderr, "MadaVm| StackError: Desbordamiento de pila.\n");
        exit(-1);
    }
    Byte reg1 = fetch8(vm);

    vm->stack[vm->regs[SP]++] = GET_BYTE(vm->regs[reg1], 0);
    vm->stack[vm->regs[SP]++] = GET_BYTE(vm->regs[reg1], 1);
    vm->stack[vm->regs[SP]++] = GET_BYTE(vm->regs[reg1], 2);
    vm->stack[vm->regs[SP]++] = GET_BYTE(vm->regs[reg1], 3);

}

static void o_pop(MadaVm* vm)
{
    if(vm->stack[SP] < 0)
    {
        fprintf(stderr, "MadaVm| PopError: Pila Vacia.\n");
        exit(-1);
    }
    Byte reg1 = fetch8(vm);
    vm->regs[reg1] = 0;

    for(int i=0; i<4;i++)
    {
        vm->regs[reg1] = LSH_BYTE(vm->regs[reg1]);
        vm->regs[reg1] += vm->stack[--vm->regs[SP]];
    }


}

static Byte fetch8(MadaVm* self)
{
    if(!self->code)
    {
        fprintf(stderr, "MadaVm| FetchError: No se ha cargado codigo que ejecutar.\n");
        exit(-1);
    }

    Dword pc = self->regs[PC];
    Byte res = (self->code)[pc];
    self->regs[PC]++;

    return res;
}

static Word fetch16(MadaVm* self)
{
    if(!self->code)
    {
        fprintf(stderr, "MadaVm| FetchError: No se ha cargado codigo que ejecutar.\n");
        exit(-1);
    }

    Word res;
    for(int i=0; i<2; i++)
    {
        res = LSH_BYTE(res);
        res += self->code[self->regs[PC]++];
    }

    return res;
}

static Dword fetch32(MadaVm* self)
{
    if(!self->code)
    {
        fprintf(stderr, "MadaVm| FetchError: No se ha cargado codigo que ejecutar.\n");
        exit(-1);
    }

    Dword res = 0;
    for(int i=0; i<4; i++)
    {
        res = LSH_BYTE(res);
        res += self->code[self->regs[PC]++];
    }
    return res;
}

static void exec(MadaVm* vm)
{
    if(vm->code == NULL)
    {
        printf("MadaVm| ExecError: no se ha cargado un codigo el cual ejecutar.\n");
        exit(-1);
    }

    vm->run_flag = 1;

    while(vm->run_flag)
    {
        enum Opcode opcode = fetch8(vm);

        if(opcode >= OPCODE_MAX)
        {
            printf("MadaVm| ExecError[0x%X]: opcode desconocido.\n", vm->regs[PC]-1);
            exit(-2);
        }

        operations[opcode](vm);

        breakStack(vm, 2);
        /*
        printStack();
        getchar();
        system("cls");*/

    }

    printf("\nMadaVm| Ejecucion finalizada con exito!.\n");
}

static void syscall(MadaVm* vm, Byte syscall_code)
{
    switch(syscall_code)
    {
        case S_EXIT:
            vm->run_flag = 0;
            break;
        case S_PRINTINT:
            fprintf(stdout, "%d", (int32_t)vm->regs[R1]);
            break;
        case S_PRINTSTR:
            break;
        default:
            break;
    }
}


void breakStack(MadaVm* vm, size_t dir)
{
    if(vm->regs[SP] == dir)
    {
        printStack(vm);
        getchar();
        system("cls");
    }
}

void printStack(MadaVm* vm)
{
        printf("-------------------\n");
        for(int i=0; i<30; i++)
        {
            printf("STACK:%d: %d", i, _global_stack[i]);

            if(i == vm->regs[SP])
            {
                printf(" <---- SP");
            }

            printf("\n");
        }
}


#endif // VM_H_INCLUDED
