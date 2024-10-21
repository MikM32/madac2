#ifndef MADA_H_INCLUDED
#define MADA_H_INCLUDED

#include "semantic.h"
#include "vm.h"

typedef struct st_mada
{
    MadaSemantic analyzer;
    MadaVm vm;

}Mada;

void initMada(Mada* mada)
{
    initMadaSemantic(&mada->analyzer);
    initVm(&mada->vm);
}

void buildFile(Mada* self)
{
    FILE* fobj = fopen("code.mobj", "wb");

    if(!fobj)
    {
        fprintf(stderr, "Mada| No se pudo crear el archivo objeto.\n");
        exit(-1);
    }

    fwrite(self->analyzer.cpool.code, sizeof(Byte), self->analyzer.cpool.count, fobj);

    fclose(fobj);
}

void run(Mada* self, char* srcfile)
{
    FILE* fptr = fopen(srcfile, "r+");

    if(fptr == NULL)
    {
        fprintf(stderr, "Mada| RunError: No se encontro el archivo \"%s\".\n", srcfile);
        exit(-1);
    }

    size_t fsize = getFileSize(fptr);
    char* source_code = calloc(fsize, 1);
    fread(source_code, sizeof(char), fsize, fptr);

    fclose(fptr);

    self->analyzer.parser.lexer.loadSource(&self->analyzer.parser.lexer, source_code);

    self->analyzer.analyze(&self->analyzer);
    self->vm.code = self->analyzer.cpool.code;

    //buildFile(self);

    self->vm.exec(&self->vm);

}

#endif // MADA_H_INCLUDED
