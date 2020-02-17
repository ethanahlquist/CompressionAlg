#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "indexTrie.h"
#include "bitPacking.h"

void printUsage()
{
   fprintf(stderr, "Usage: lzwCompress [-rN] file\nWhere: N is a number "
      "from 9 to 15 specifying the recycle code as a power of 2\n");
   exit(EXIT_FAILURE);
}

void flagCases(char *arg, int *maxCodeSize)
{
   if(sscanf(arg, "-r%d", maxCodeSize)){
      if(*maxCodeSize < 9 || *maxCodeSize > 15)
         printUsage();
   }
   else
      printUsage();
}

static void parseFlags(int argc, char *argv[],
   char **targetName, int *maxCodeSize)
{
   int i;
   if(argc == 1)
      printUsage();
   for(i = 1; i < argc; i++){
      if(!strncmp(argv[i], "-", 1))
         flagCases(argv[i], maxCodeSize);
      else{
         if(*targetName == NULL)
            *targetName = argv[i];
         else
            printUsage();
      }
   }
}

FILE *openFile(char *filename, const char *mode, Files file){

   FILE *newFile;

   if(NULL == (newFile = fopen(filename, mode))) {
      fprintf(stderr, "lzwCompress: %s: ", filename);
      free(filename);
      if(file.in != NULL)
         fclose(file.in);
      if(file.out != NULL)
         fclose(file.out);
      MY_ERROR();
   }
   free(filename);
   return newFile;
}

Files createFilePointers(char *UnallocedInName)
{
   char *outName;
   Files file;
   char *inName;

   file.in = NULL;
   file.out = NULL;

   MY_MALLOC(inName, strlen(UnallocedInName) +1);
   strcpy(inName, UnallocedInName);

   file.in = openFile(inName, "r", file);

   MY_MALLOC(outName, strlen(UnallocedInName) + 5);
   sprintf(outName, "%s.lzw", UnallocedInName);

   file.out = openFile(outName, "w", file);

   return file;
}

void finish_lzw(Trie *trie, unsigned short buf, LeftOver *LO, Files file)
{
   unsigned short unloadChar = 0;

   if(trie->CurrentSize > (1 << trie->packSize))
      trie->packSize++;

   packBits(&buf, trie->packSize, LO);
   fputc(buf, file.out);

   if(unloadLeftOver(LO, &unloadChar))
      fputc(unloadChar, file.out);

   /* Check for packSize and Recycle */
   if(trie->CurrentSize >= (1 << trie->packSize))
      trie->packSize++;

   buf = 256;

   packBits(&buf, trie->packSize, LO);
   fputc(buf, file.out);

   while(releaseLeftOver(LO, &unloadChar))
      fputc(unloadChar, file.out);

   free(trie->nodes);
   free(trie);
   fclose(file.in);
   fclose(file.out);

   exit(EXIT_SUCCESS);
}

void getAndPack(unsigned short * buf, Trie *trie, Files file,
   int *prevChar, LeftOver *LO){

   if(!getCode(buf, trie, file, prevChar))
      finish_lzw(trie, *buf, LO, file);

   packBits(buf, trie->packSize, LO);
   fputc((*buf), file.out);
}

void checkSizes(Trie *trie, unsigned short * sqPackSize)
{
   if(trie->CurrentSize > *sqPackSize){
      trie->packSize++;

      if(trie->packSize > trie->RecycleSize)
         TrieRecycle(trie, trie->RecycleSize);
      else
         TrieResize(trie);

      *sqPackSize = 1 << (trie->packSize);
   }
}

void lzw(char *inName, char maxCodeSize)
{
   unsigned short buf = 0;
   Trie *trie;
   unsigned short sqPackSize = 512;
   int prevChar;
   unsigned short unloadChar;

   LeftOver LO = {0,0};

   Files file = createFilePointers(inName);
   trie = TrieCreate(maxCodeSize); /*packSize*/

   if(EOF == (prevChar = fgetc(file.in)))
      exit(EXIT_SUCCESS);
   fputc(maxCodeSize, file.out);

   while(1){

      getAndPack(&buf, trie, file, &prevChar, &LO);

      /* Check for packSize and Recycle */
      checkSizes(trie, &sqPackSize);

      if(unloadLeftOver(&LO, &unloadChar))
         fputc(unloadChar, file.out);
   }
}

int main(int argc, char *argv[])
{
   char *inName = NULL;
   int maxCodeSize = 0; /* default value */

   parseFlags(argc, argv, &inName, &maxCodeSize);

   /* if 0, make maxCode 12 */
   maxCodeSize = (maxCodeSize == 0 ? 12 : maxCodeSize);

   if(inName == NULL)
      inName = argv[1];

   lzw(inName, maxCodeSize);

   exit(EXIT_SUCCESS);
}
