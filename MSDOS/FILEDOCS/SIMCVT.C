/******************************************************************************

Written by reynolds@sun.com                                     01/21/90
Minor corrections to instructions and portability
		davidsen@crdos1.crd.ge.com			02/23/90

This SIMCVT.C filter should convert Simtel-20's "SIMIBM.IDX" file into a
readable "SIMIBM.LST" that is compatible with the other convert programs,
except for the run-date at the top of the output file.

This program, written in "C" should compile on both 4.3BSD Unix machines,
as well as IBM/pc compatible machines.  It works on both VAXen, and Suns.

To Compile on Unix, type "cc -o simcvt SIMCVT.C" creating simcvt.
To Compile on IBM/pcs, see your C manual that came with the compiler.

To run, type "simcvt < simibm.idx > simibm.lst

******************************************************************************/

#include <stdio.h>

main()

{
char  fs[10],dir[60],name[15],descr[60]; /* input variables */
char  inputline[257];                    /* for initial read */
int   rev,bits;                          /* input variables */
long  length,date;                       /* input variables */
char  lfs[10],ldir[60];                  /* stores last filesystem/directory */
char  type;                              /* output variable for 'A' or 'B' */
char  c;                                 /* picks off EOF,",linefeed */

printf("WSMR-SIMTEL20.ARMY.MIL PUBLIC DOMAIN LISTING\n\n");
printf("NOTE: Type B is Binary; Type A is ASCII\n");

inputline[256] = 0;

while (fgets(inputline, 256, stdin) != NULL)  {

   sscanf(inputline, 
   	    "\"%[^\"]\",\"%[^\"]\",\"%[^\"]\",%d,%ld,%d,%ld,\"%[^\"]\"",
            fs, dir, name, &rev, &length, &bits, &date, descr);

   type = 'B';                           /* Binary 8-bit */
   if (bits == 7) type = 'A';            /* ASCII  7-bit */

   if (strcmp(ldir,dir) || strcmp(lfs,fs)) {  /* New Directory */
      printf("\nDirectory %s%s\n",fs,dir);
      printf(" Filename   Type Length   Date    Description\n");
      printf("==============================================\n");
      strcpy(ldir, dir);          /* Remember last directory with ldir  */
      strcpy(lfs,fs);             /* Remember last file system with lfs */
   }                              /* End of the New Directory routine   */


   printf("%-12.12s  %c %7ld  %6ld  %s\n",name,type,length,date,descr);
   }
} /* end of main() program by Ray */

/*****************************************************************************

   This filter takes data in the following format:
"PD1:","<MSDOS.ADA>","ADA-LRM2.ARC",1,320086,8,890411,"The Ada Language Reference Manual reader (2/4)"

   And converts it to the following format:
ADA-LRM1.ARC  B  231947  890411  The Ada Language Reference Manual reader (1/4)

*****************************************************************************/
