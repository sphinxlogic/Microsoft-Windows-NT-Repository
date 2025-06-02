/********************************************************************
*  WR_PROT.C
*
*       Check to see if a drive is write protected
*
*       Compiled with MSC 5.1   2/5/90   by Kevin English
*       kje2282@venus.tamu.edu  kje2282@tamvenus.bitnet
*
*       Uses the DOS Interupts 25H and 26H to do an absolute
*       read, then write of logical sector 0 of the drive.
*
*       resultant ERRORLEVELs:
*          2 : error accessing drive
*          1 : write protected
*          0 : writable
*/
#include <dos.h>
char buffer[2048];

int main()
{
   union REGS inregs, outregs;
   struct SREGS segregs;
   char far *bufferp;

   inregs.h.al = 0;   /* drive number 0,1 = a,b: 128,129=c,d */
   inregs.x.cx = 1;   /* sectors to read */
   inregs.x.dx = 0;   /* starting sector number */
   bufferp = buffer;
   inregs.x.bx = FP_OFF(bufferp);
   segregs.ds  = FP_SEG(bufferp);

   int86x(0x25, &inregs, &outregs, &segregs);
   if (outregs.x.cflag == 1) {
      printf("error reading drive A\n");
      return 2;
   }

   int86x(0x26, &inregs, &outregs, &segregs);
   if (outregs.x.cflag == 1) {
      printf("error writing drive A\n");
      return 1;
   }

   return 0;
}
