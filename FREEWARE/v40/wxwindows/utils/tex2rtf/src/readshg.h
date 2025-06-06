/*
 * readshg.h
 *
 * Petr Smilauer's .SHG (Segmented Hypergraphics file) reading
 * code.
 * Note: .SHG is undocumented (anywhere!) so this is reverse-engineering
 * and guesswork at its best.
 */

#ifndef readshgh
#define readshgh

#include  <stdio.h>
#include <stdlib.h>

typedef enum { TypePopup = 0xE2, TypeJump = 0xE3, TypeMacro = 0xC8} HotspotType;

#define NOT_VISIBLE  0x04

typedef struct
{
  unsigned char   hotspotType;// combines HotspotType /w NOT_VISIBLE if appropriate
  unsigned char   flag;       // NOT_VISIBLE or 0 ??
  unsigned char   skip;       // 0, always??
  unsigned short  left,
                  top,
                  width,      // left+width/top+height give right/bottom,
                  height;     // =>right and bottom edge are not 'included'
  unsigned char   magic[4];   // wonderful numbers: for macros, this seems
    // (at least first 2 bytes) to represent offset into macro-strings block.
} ShgInfoBlock;   // whole block is just 15 bytes long. How weird!

#define OFF_OFFSET    0x20    // this is offset, where WORD (?) lies
#define OFFSET_DELTA  9       // we must add this to get real offset from file beginning

struct HotSpot
{
          HotspotType type;
          unsigned int left,
                      top,
                      right,
                      bottom;
          char        szHlpTopic_Macro[65];
          Bool        IsVisible;
};

// Returns the number of hotspots, and the array of hotspots.
// E.g.
// HotSpots *array;
// int n = ParseSHG("thing.shg", &array);

extern int ParseSHG( const char* fileName, HotSpot **hotspots);

// Converts Windows .SHG file to HTML map file
extern Bool SHGToMap(char *filename, char *defaultFile);

#endif
