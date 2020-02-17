#ifndef INDEXTRIE_C
#define INDEXTRIE_C

#include <stdio.h>
#include <stdlib.h>

struct TrieNode;
struct Trie;

typedef struct {

   FILE *in;
   FILE *out;

} Files;

typedef struct {

   unsigned short code;
   unsigned short array[256];

} TrieNode;

typedef struct {

   unsigned short RecycleSize;
   unsigned short CurrentSize;
   unsigned short packSize;
   int hasRecycled;
   TrieNode *nodes;

} Trie;

/*
 * there should not be a problem with starting at root, saying t->theArray[getNode()]
 * although you are useing a pointer, you are actually indexing in.
 */

Trie *TrieCreate(unsigned short);

void TrieRecycle(Trie *, unsigned short);

int getCode(unsigned short *, Trie *trie, Files file, int *);

void TrieResize(Trie *);

#define MY_ERROR() { perror(NULL); exit(EXIT_FAILURE); }

#define MY_MALLOC(_ptr,_size) \
   if (NULL == ((_ptr) = malloc(_size))) { \
      fprintf(stderr, "Failed to allocate memory, in %s at line %d.\n"\
         ,__FILE__, __LINE__); \
      exit(EXIT_FAILURE);\
   }


#define MY_CALLOC(_ptr,_num,_type) \
   if (NULL == ((_ptr) = calloc(_num, sizeof(_type)))) { \
      fprintf(stderr, "Failed to allocate memory, in %s at line %d.\n"\
         ,__FILE__, __LINE__); \
      exit(EXIT_FAILURE);\
   }

#endif

