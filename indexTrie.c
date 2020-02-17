#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "indexTrie.h"

#define SHORT_MAX 65535

Trie *TrieCreate(unsigned short size)
{
   Trie *trie;
   unsigned short i;

   if(NULL == (trie = calloc(1, sizeof(Trie))))
      MY_ERROR();

   trie->RecycleSize = size;
   trie->hasRecycled = 0;

   trie->CurrentSize = 257;
   trie->packSize = 9;

   /* make an empty table with 2^size nodes */
   if(NULL == (trie->nodes = calloc( (1 << 9), sizeof(TrieNode) )))
      MY_ERROR();

   for(i = 0; i < (1 << 9); i++)
      memset(trie->nodes[i].array, -1, 256 * sizeof(unsigned short));

   return trie;
}

void TrieRecycle(Trie *trie, unsigned short size)
{
   int i;

   for(i = 0; i < (1 << 9); i++)
      memset(trie->nodes[i].array, -1, 256 * sizeof(unsigned short));

   trie->hasRecycled = 1;
   trie->RecycleSize = size;
   trie->CurrentSize = 257;
   trie->packSize = 9;
}

int getCode(unsigned short *outCode, Trie *trie, Files file, int *ch)
{
   *outCode = *ch;

   if(EOF == (*ch = fgetc(file.in)))
      return 0;

   while(1){

      if(SHORT_MAX == trie->nodes[*outCode].array[*ch]){
         trie->nodes[*outCode].array[*ch] = trie->CurrentSize;
         trie->nodes[*outCode].code = trie->CurrentSize++;

         return 1; /* Successful outCode found */
      }
      else
         (*outCode) = trie->nodes[*outCode].array[*ch];

      if(EOF == (*ch = fgetc(file.in)))
         return 0;
   }
   return 0;
}

void TrieResize(Trie *trie)
{
   int i;
   int sqPackSize = (1 << trie->packSize);

   if(NULL == (trie->nodes = realloc(trie->nodes,
      sqPackSize * sizeof(TrieNode))))
      MY_ERROR();

   for(i = trie->CurrentSize -1; i < sqPackSize; i++)
      memset(trie->nodes[i].array, -1, 256 * sizeof(unsigned short));
}
