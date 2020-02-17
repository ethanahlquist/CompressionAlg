#ifndef BITPACKING_H
#define BITPACKING_H

typedef struct{
   unsigned short data;
   int size;
}LeftOver;

void packBits(unsigned short *, int, LeftOver *);
int unloadLeftOver(LeftOver *, unsigned short *);
int releaseLeftOver(LeftOver *, unsigned short *);

#endif

