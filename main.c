#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"
#include "ast.h"

#define LEXER_NEXT(instance) instance.next(&instance)

int main()
{
    /*
    MadaParser parser;

    initMadaParser(&parser);

    char fuente[256] = "\n\n-(123.2 - 456) + hola\n\n\0";
    parser.lexer.loadSource(&parser.lexer, fuente);
    parser.parse(&parser);

    printAst(parser.ast);*/

    MadaSemantic sem;
    initMadaSemantic(&sem);

    char fuente[256] = "\n\n-(1+2) + 12\n\n\0";
    sem.parser.lexer.loadSource(&sem.parser.lexer, fuente);
    sem.analyze(&sem);
    //sem.parser.parse(&sem.parser);
    //printAst(sem.parser.ast, 1);


    return 0;
}
