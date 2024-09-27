#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED


#include "token.h"
#include "utils.h"

#define TOGGLE_FLAG(flag) flag = !flag
#define MAX_BUFFER 1024



char str_tok[30][30];

typedef struct st_lexer
{

    void (*next)(struct st_lexer*);    //Simulando metodos en C ( o recordando a metodos de python: metodo(self)
    // Donde self es un puntero a la instancia actual.

    void (*loadSource)(struct st_lexer*, char* source);

    char* source, *cur_source;   //Fuente
    //char str_toktype[30][30];
    int line_count;
    int col_count;
    int eol_flag;
    MadaToken current_token;


} MadaLexer;

static void next(MadaLexer* self); // Definido como estatico para simular encapsulamiento de POO
static void loadSource(MadaLexer* self, char* source);


//-----------Definiciones--------------


//Constructor
void initMadaLexer(MadaLexer* self)
{
    self->next = next;
    self->loadSource = loadSource;
    self->source = NULL; // puntero original
    self->cur_source = NULL; // puntero sobre la cual se itera
    self->current_token.type = T_NONE;
    self->line_count = self->col_count = 1;
    self->eol_flag = 0;

    //construye la tabla de cadenas para los tokens correspondientes con el fin de mostrar mejores mensajes de error a la larga.
    strcpy(str_tok[T_EOL], "Salto de Linea");
    strcpy(str_tok[T_ID], "Identificador");
    strcpy(str_tok[T_INTEGER], "Numero Entero");
    strcpy(str_tok[T_BOOL], "Valor logico");
    strcpy(str_tok[T_PLUS], "+");
    strcpy(str_tok[T_MINUS], "-");
    strcpy(str_tok[T_MUL], "*");
    strcpy(str_tok[T_DIV], "/");
    strcpy(str_tok[T_COLON], ":");
    strcpy(str_tok[T_EOF], "Fin de archivo");
    strcpy(str_tok[T_CPAREN], ")");
    strcpy(str_tok[T_OPAREN], "(");
    strcpy(str_tok[T_ALG], "algoritmo");

    //memcpy(self->str_toktype, str_tok, 30*4);

}

//Destructor
void destroyMadaLexer(MadaLexer* self)
{
    if(self->source != NULL)
    {
        free(self->source);
        self->source = NULL;
    }
}

static int isAlpha(char c)
{
    if((c >= 'a' && c <= 'z') || (c >= 'A' && c<= 'Z'))
    {
        return 1;
    }

    return 0;
}

static int isNum(char c)
{
    if( c >= '0' && c <= '9')
    {
        return 1;
    }

    return 0;
}

static void loadSource(MadaLexer* self, char* source)
{
    if(source)
    {
        self->source = source;
        self->cur_source = source;
    }
}

static TokenType isKeyword(char* word)
{
    if(!strcmp(word, "verdadero") || !strcmp(word, "falso"))
    {
        return T_BOOL;
    }
    else if(!strcmp(word, "algoritmo"))
    {
        return T_ALG;
    }
    else if(!strcmp(word, "entero"))
    {
        return T_INT_TYPE;
    }
    else if(!strcmp(word, "var"))
    {
        return T_VAR;
    }
    else if(!strcmp(word, "inicio"))
    {
        return T_BEGIN;
    }
    else if(!strcmp(word, "fin"))
    {
        return T_END;
    }
    else if(!strcmp(word, "real"))
    {
        return T_REAL_TYPE;
    }
    else if(!strcmp(word, "escribir"))
    {
        return T_WRITE;
    }

    return T_NONE;
}

/*
static void appendBuffer(char* buffer, char* text)
{
    int buffer_len = strlen(buffer);
    int text_len = strlen(text);

    if((buffer_len - text_len) < MAX_BUFFER)
    {
        strncat(buffer, text, text_len);
    }
}*/

static void invalidCharError(MadaLexer* self, char invalid)
{
    fprintf(stderr, "MadaLexer| InvalidCharError: El caracter \'%c\' es invalido.\n", invalid);
    exit(-1);
}

//Funcion que itera el codigo fuente y va obteniendo cada token hasta llegar al final del archivo (EOF)

static void next(MadaLexer* self)
{
    if(self->source == NULL)
    {
        printf("Lexer: No se ha cargado un codigo fuente que analizar.\n");
        return;
    }


    self->current_token.type = T_NONE; // en este caso T_NONE sirve para indicar que se omitio un flujo de caracteres y se volvera a iterar
    while(self->current_token.type == T_NONE)
    {
        if(*self->cur_source == ' ')
        {
            self->cur_source++;
            self->col_count++;

        }
        else if(*self->cur_source == '\n')
        {
            if(self->eol_flag) initMadaToken(&self->current_token, T_EOL, NULL, self->cur_source, self->line_count, self->col_count);

            self->line_count++;
            self->col_count=1;

            self->cur_source++;
            self->eol_flag=0;
        }
        else if(isAlpha(*self->cur_source))
        {

            char buffer[256]= {0};
            int i=0;

            char* sourcePos = self->cur_source;
            int curCol = self->col_count;

            while(isAlpha(*self->cur_source) || isNum(*self->cur_source))
            {
                buffer[i] = *self->cur_source; //Posible bug (si el identificador sobrepasa los 255 caracteres no se maneja excepcion alguna)
                i++;
                self->cur_source++;
                self->col_count++;
            }

            TokenType kType = isKeyword(buffer);
            if(kType != T_NONE)
            {
                initMadaToken(&self->current_token, kType, buffer, sourcePos, self->line_count, curCol);
            }
            else
            {
                initMadaToken(&self->current_token, T_ID, buffer, sourcePos, self->line_count, curCol);
            }

            self->eol_flag=1;
        }
        else if(isNum(*self->cur_source))
        {
            char buffer[256]= {0};
            int i=0;

            char* sourcePos = self->cur_source;
            int curCol = self->col_count;

            while(isNum(*self->cur_source))
            {
                buffer[i] = *self->cur_source; //Posible bug (si el identificador sobrepasa los 255 caracteres no se maneja excepcion alguna)
                i++;
                self->cur_source++;
                self->col_count++;
            }


            if((*self->cur_source) == '.')
            {
                buffer[i] = *self->cur_source;
                i++;
                self->cur_source++;
                self->col_count++;
                while(isNum(*self->cur_source))
                {
                    buffer[i] = *self->cur_source;
                    i++;
                    self->cur_source++;
                    self->col_count++;
                }
                initMadaToken(&self->current_token, T_REAL, buffer, sourcePos, self->line_count, curCol);
            }
            else
            {
                initMadaToken(&self->current_token, T_INTEGER, buffer, sourcePos, self->line_count, curCol);
            }

            self->eol_flag=1;
        }
        else if(*self->cur_source == '+')
        {
            initMadaToken(&self->current_token, T_PLUS, NULL, self->cur_source, self->line_count, self->col_count);
            self->cur_source++;
            self->col_count++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '-')
        {
            if(*(self->cur_source+1) == '>')
            {
                initMadaToken(&self->current_token, T_ASSIGN, NULL, self->cur_source, self->line_count, self->col_count);
                self->cur_source+=2;
                self->col_count+=2;
            }
            else
            {
                initMadaToken(&self->current_token, T_MINUS, NULL, self->cur_source, self->line_count, self->col_count);
                self->cur_source++;
                self->col_count++;
            }

            self->eol_flag=1;
        }
        else if(*self->cur_source == '*')
        {
            initMadaToken(&self->current_token, T_MUL, NULL, self->cur_source, self->line_count, self->col_count);
            self->cur_source++;
            self->col_count++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '/')
        {
            initMadaToken(&self->current_token, T_DIV, NULL, self->cur_source, self->line_count, self->col_count);
            self->cur_source++;
            self->col_count++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '=')
        {
            initMadaToken(&self->current_token, T_EQU, NULL, self->cur_source, self->line_count, self->col_count);
            self->cur_source++;
            self->col_count++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '<')
        {
            if(*(self->cur_source+1) == '=' )
            {
                initMadaToken(&self->current_token, T_LESS_EQ, NULL, self->cur_source, self->line_count, self->col_count);
                self->cur_source+=2;
                self->col_count+=2;
            }
            else
            {
                initMadaToken(&self->current_token, T_LESS, NULL, self->cur_source, self->line_count, self->col_count);
                self->cur_source++;
                self->col_count++;
            }

            self->eol_flag=1;
        }
        else if(*self->cur_source == '>')
        {
            if(*(self->cur_source+1) == '=' )
            {
                initMadaToken(&self->current_token, T_BIGGER_EQ, NULL, self->cur_source, self->line_count, self->col_count);
                self->cur_source+=2;
                self->col_count+=2;
            }
            else
            {
                initMadaToken(&self->current_token, T_BIGGER, NULL, self->cur_source, self->line_count, self->col_count);
                self->cur_source++;
                self->col_count++;
            }

            self->eol_flag=1;
        }
        else if(*self->cur_source == '(')
        {
            initMadaToken(&self->current_token, T_OPAREN, NULL, self->cur_source, self->line_count, self->col_count);
            self->cur_source++;
            self->col_count++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == ')')
        {
            initMadaToken(&self->current_token, T_CPAREN, NULL, self->cur_source, self->line_count, self->col_count);
            self->cur_source++;
            self->col_count++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == ':')
        {
            initMadaToken(&self->current_token, T_COLON, NULL, self->cur_source, self->line_count, self->col_count);
            self->cur_source++;
            self->col_count++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '"')
        {
            int col = self->col_count;
            self->cur_source++;
            self->col_count++;

            char* str_source = self->cur_source;
            //int i=0;

            while(*self->cur_source != '"' || *self->cur_source != '\0')
            {
                self->cur_source++;
                self->col_count++;
            }

            initMadaToken(&self->current_token, T_CPAREN, str_source, str_source, self->line_count, col);
            self->cur_source++;
            self->col_count++;
            self->eol_flag=1;
        }
        else if(*self->cur_source == '\0')
        {
            initMadaToken(&self->current_token, T_EOF, NULL, self->cur_source, self->line_count, self->col_count);
            //Llego al final del codigo fuente por lo que deja de iterar (no se hace self->cur_source++)
        }
        else // Caracter no soportado. Ejem: ñ
        {
            invalidCharError(self, *self->cur_source);
        }

    }

}


#endif // LEXER_H_INCLUDED
