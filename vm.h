#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#define REGS_NUM 17

typedef struct st_vm
{
    char* code;
    int regs[REGS_NUM];


} MadaVm;

void initVm(MadaVm* vm)
{

}

static void exec(MadaVm* vm)
{

}

static void syscall(MadaVm* vm, short syscall_code)
{

}



#endif // VM_H_INCLUDED
