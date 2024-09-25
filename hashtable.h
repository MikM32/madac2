#ifndef HASHTABLE_H_INCLUDED
#define HASHTABLE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define HM_INITIAL_CAP 20
#define HM_GROW_FACTOR 2

typedef struct st_bucket
{
    char* key;
    void* value;

} Bucket;

typedef struct st_hashmap
{
    size_t capacity;
    size_t count;

    Bucket* buckets;
} Hashmap;

uint64_t getHash(char* key, size_t key_len)
{
    uint64_t hash = FNV_OFFSET;
    for(size_t i=0; i<key_len; i++)
    {
        hash ^= (uint64_t)(unsigned char )(key[i]); // hash = hash XOR key[i]
        hash *= FNV_PRIME;
    }
    return hash;
}

void initHashmap(Hashmap* hm)
{
    hm->capacity = HM_INITIAL_CAP;
    hm->buckets = calloc(hm->capacity, sizeof(Hashmap));

    if(!hm->buckets)
    {
        fprintf(stderr, "Error al inicializar hashmap: no hay espacio en memoria (Bytes requeridos: %d).\n", hm->capacity*sizeof(Hashmap));
        exit(-1);
    }

    hm->count = 0;

}

void growHashmap(Hashmap* hm)
{
    hm->capacity *= HM_GROW_FACTOR;
    hm->buckets = realloc(hm->buckets, hm->capacity*sizeof(Hashmap));

    if(!hm->buckets)
    {
        fprintf(stderr, "Error al expandir hashmap: no hay espacio en memoria (Bytes requeridos: %d).\n", hm->capacity*sizeof(Hashmap));
        exit(-1);
    }
}

uint8_t _setHashmap(Hashmap* hm, char* key, void* value, short dup_key)
{
    size_t key_len = strlen(key);
    uint64_t hash = getHash(key, key_len);
    size_t index = (size_t)(hash % hm->capacity); // Se complica la compresion al escribir (size_t)(hash & (hm->capacity-1)) xd

    char* key_acum=NULL;

    while(hm->buckets[index].key != NULL)
    {
        key_acum = hm->buckets[index].key;
        if(!strncmp(key_acum, key, key_len))
        {
            hm->buckets[index].value = value;

            return 1; // Ya existia el bucket
        }


        index++;
        if(index >= hm->capacity)
        {
            //Si el indice alcanza el final, regresa al inicio. Esto porque puede que se empiece a iterar en medio del arreglo
            index = 0;
        }

    }

    hm->count++;

    if(dup_key) // duplica la llave en memoria (si no se quiere utilizar un mismo valor en memoria)
    {
        key = strdup(key);
        if(!key)
        {
            exit(-1);
        }
    }

    hm->buckets[index].key = key;
    hm->buckets[index].value = value;

    return 0; // No existia el bucket
}

uint8_t setHashmap(Hashmap* hm, char* key, void* value, short dup_key)
{
    if(!key)
    {
        exit(-2);
    }
    if(!value)
    {
        exit(-3);
    }

    if(hm->count >= hm->capacity/2)
    {
        growHashmap(hm);
    }

    return _setHashmap(hm, key, value, dup_key);
}

void* getHashmap(Hashmap* hm, char* key)
{
    size_t key_len = strlen(key);
    uint64_t hash = getHash(key, key_len);
    size_t index = (size_t)(hash % hm->capacity);

    char* key_acum=NULL;

    while(hm->buckets[index].key != NULL)
    {
        key_acum = hm->buckets[index].key;
        if(!strncmp(key_acum, key, key_len))
        {
            return hm->buckets[index].value;
        }


        index++;
        if(index >= hm->capacity)
        {
            //Si el indice alcanza el final, regresa al inicio. Esto porque puede que se empiece a iterar en medio del arreglo
            index = 0;
        }

    }

    return NULL; // No se encontro el elemento
}

#endif // HASHTABLE_H_INCLUDED
