#include <stdio.h>
#include <stdlib.h>
#include <t1lib.h>  /* All needed declarations */

int main( void)
{

  GLYPH *glyph;
  int i;
  
  /* Set our environment to an existent config file directory */
  putenv( "T1LIB_CONFIG=./t1lib.config");

  /* Pad bitmaps to 16 bits, the default being 8 bits */
  T1_SetBitmapPad( 16);
  
  /* Initialize t1lib and return if error occurs. No logfile will be
     generated */
  if ((T1_InitLib(NO_LOGFILE)==NULL)){
    fprintf(stderr, "Initialization of t1lib failed\n");
    return(-1);
  }

  /* For every font in the database, generate a glyph for the string
     "Test" at 25 bp. Use Kerning. Then dump an ASCII representation
     of the glyph to stdout */
  for( i=0; i<T1_Get_no_fonts(); i++){
    glyph=T1_SetString( i, "Test", 0, 0, T1_KERNING, 25.0, NULL);
    T1_DumpGlyph( glyph);
  }

  /* Close library and free all data */
  T1_CloseLib();
  
  return( 0);
  
}
