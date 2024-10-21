#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

typedef struct st_madaBlock
{
    size_t size;

    size_t offset;

    struct st_madaBlock * next;
}MadaBlock;

typedef struct st_madaArena
{

}MadaArena;

void* madaAlloc(void* ptr, size_t size, int is_realloc)
{
    if(!is_realloc)
    {
        ptr = malloc(size);
    }
    else
    {
        ptr = realloc(ptr, size);
    }
    if(!ptr)
    {
        fprintf(stderr, "madaAlloc():ERROR: no hay suficiente espacio en memoria.\n");
        exit(-1);
    }

    return ptr;

}

#endif // MEMORY_H_INCLUDED
