/*
 * Allows MSDOS program to open more than 20 files
 *
 * Use as:
 * mfile (size, array)
 *   int size; 	      - sizeof(array) is new maximum number of open files
 *   char far* array; - array for the new table of file handlers
 *
 * Modified for Turbo C version of the MFILE.ASM from LISTSERV@VM1.NODAK.EDU
 *
 * Author: A.Galtchenko (alex@mars.ibioc.serpukhov.su)
 * Date: 8 jul 1991
 */
#include	<dos.h>

typedef	char far* FARPTR;

void mfile (int size, char far* array)

{
    unsigned	psp;
    int far*	pl;
    FARPTR far*	pt;
    int		i;

    psp = getpsp();			/* get PSP */
    pl = (int far*)MK_FP(psp,0x32);	/* pointer to size of the filetable header */
    pt = (FARPTR far*)MK_FP(psp,0x34);	/* pointer to filetable header */
    if (size > *pl) {
	for (i = 0; i < *pl; i++)	/* save current filetable header */
	    array[i] = (*pt)[i];        /* into new table		 */
	for (i = *pl; i < size; i++)	/* fill rem of new table         */
	    array[i] = 0xff;
	*pt = array;                    /* new address of program's actual file table */
	*pl = size;			/* new maximum number of open files */
    }
}
