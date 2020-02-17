/*
 * Functions in relation to accessing and minimizing
 * the space consumption of a (unsigned short)
 */
#include "bitPacking.h"

void packBits(unsigned short *toPack, int packSize, LeftOver *LO)
{
   unsigned short mask = 0;
   unsigned short prev_LO = LO->data; /*& ((1 << LO->size) -1);*/
   int LO_prevSize = LO->size;

   /* toPack takes 8 bits, LO takes others */
   LO->size = (packSize - 8 + LO_prevSize);

   mask = (1 << LO->size) -1; /* mask of bits to send to LO */
   LO->data = (*toPack) & mask; /* copy toPack Leftovers */

   /* shift toPack to remove bits saved in LO */
   (*toPack) >>= LO->size; /* Logical Right Shift */

   /* shift so that prev_LO does not overwite meaningfull toPack bits */
   prev_LO <<= (8 - LO_prevSize); /* at most shifts 8 */

   /* mask size (pS - LO) at LSB */
   mask = (1 << (8 - LO_prevSize)) -1;

   /* add the contents of (prev Left Over) to toPack */
   (*toPack) = ( (*toPack) & mask ) + prev_LO;
}

/*
 * Note that unloadChar is restricted to the size of a
 * char, but is a unsigned short to allow bit operations with other shorts
 */
int unloadLeftOver(LeftOver *LO, unsigned short *unloadChar)
{
   short mask;
   (*unloadChar) = 0;
   if(LO->size >= 8) {
      LO->size -= 8;
      /* create a mask of the upper 8 MSBs */
      mask = ((1 << 8) -1) << (LO->size);
      (*unloadChar) = ( LO->data & mask ) >> LO->size;
      /* keep only the last bits to remain in LO */
      LO->data &= (1 << LO->size) -1;
      return 1;
   }
   return 0;
}

int releaseLeftOver(LeftOver * LO, unsigned short *unloadChar){

   short mask;
   (*unloadChar) = 0;
   if(LO->size > 0) {
      LO->size -= 8;
      /* create a mask of the upper 8 MSBs */
      mask = ((1 << 8) -1) << (LO->size);
      (*unloadChar) = ( LO->data & mask ) >> LO->size;
      /* keep only the last bits to remain in LO */
      LO->data &= (1 << LO->size) -1;
      return 1;
   }
   return 0;
}
