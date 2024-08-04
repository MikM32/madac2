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

#endif // UTILS_H_INCLUDED
