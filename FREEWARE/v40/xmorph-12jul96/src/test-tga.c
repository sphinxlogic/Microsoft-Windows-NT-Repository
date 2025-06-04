#include <stdio.h>
#include "tga.h"
#include "image.h"

main(argc, argv)
int argc;
char **argv;
{
  char row[320];
  int indx, jndx;
  int rl;
  rgba_image_t img;

  img.ncols=320;
  img.nrows=1;
  img.ri=row;

  row[0]=0;

  for(indx=1; indx<320; indx++) {
    memset(row, 0, indx);
    memset(row+indx, 1, 320-indx);
    rl=tga_run_length(&img, 0, 0, 8);
    printf("repeats indx=%i rl=%i\n", indx, rl);
  }

  row[0]=0;
  for(indx=1; indx<320; indx++) {
    for(jndx=0; jndx<indx; jndx++) {
      row[jndx]=jndx;
    }
    memset(row+indx, 255, 320-indx);
    rl=tga_run_length(&img, 0, 0, 8);
    printf("distincts indx=%i rl=%i\n", indx, rl);
  }

  row[0]=0;
  for(indx=1; indx<320; indx++) {
    for(jndx=0; jndx<=indx; jndx++) {
      row[jndx]=jndx/2;
    }
    memset(row+indx, 255, 320-indx);
    rl=tga_run_length(&img, 0, 0, 8);
    printf("double distincts indx=%i rl=%i\n", indx, rl);
  }

  row[0]=0;
  for(indx=1; indx<320; indx++) {
    for(jndx=0; jndx<=indx; jndx++) {
      row[jndx]=jndx/3;
    }
    memset(row+indx, 255, 320-indx);
    rl=tga_run_length(&img, 0, 0, 8);
    printf("triple distincts indx=%i rl=%i\n", indx, rl);
  }
}
