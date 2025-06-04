#include <stdio.h>
#include <conio.h>
#include <libtext.h>
#include <go32.h>
#include <dpmi.h>
#include <sys/nearptr.h>
#include <malloc.h>
#include "aalib.h"

static int dos_init(aa_context * c, int mode)
{
    return 1;
}
static void dos_uninit(aa_context * c)
{
}

static int mygetch()
{
    int i = getch();
    if (i) {
	if (i == 8)
	    return AA_BACKSPACE;
	if (i == 27)
	    return AA_ESC;
	return (i);
    }
    if (!i)
	switch (getch()) {
	case 72:
	    return AA_UP;
	case 80:
	    return AA_DOWN;
	case 75:
	    return AA_LEFT;
	case 77:
	    return AA_RIGHT;
	default:
	    return AA_UNKNOWN;
	}
    return (AA_UNKNOWN);
}

static int dos_getchar(aa_context * c1, int wait)
{
    if (wait)
	return (mygetch());
    if (kbhit())
	return (mygetch());
    else
	return (AA_NONE);
}


struct aa_kbddriver kbd_dos_d =
{
    "dos", "dos keyboard driver 1.0",
    0,
    dos_init,
    dos_uninit,
    dos_getchar,
};
