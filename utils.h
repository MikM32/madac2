#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

void printUntil(char* str, char delim)
{
    while(*str != delim && *str != '\0')
    {
        fputc(*str, stdout);
        str++;
    }
}

size_t getFileSize(FILE* fptr)
{
    fseek(fptr, 0L, SEEK_END);
    size_t fsize = ftell(fptr);
    rewind(fptr);

    return fsize;
}

void printArrow(unsigned int padding, unsigned int arrow_pos)
{
    // Sangria antes de escribir la flecha
    printf("%*c", padding, ' ');

    printf("%*c\n", arrow_pos, '^');
}

#endif // UTILS_H_INCLUDED
