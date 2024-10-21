#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mada.h"
#include "hashtable.h"

int main()
{
    /*
    MadaSemantic sem;
    initMadaSemantic(&sem);

    char fuente[256] = "-(1+2) + 1\n\n\0";
    sem.parser.lexer.loadSource(&sem.parser.lexer, fuente);
    sem.analyze(&sem);

    printPool(&sem.cpool);
    */

    Mada mada;
    initMada(&mada);

    run(&mada, "test.mc");
    disassemble(&mada.analyzer.cpool);


    return 0;
}
