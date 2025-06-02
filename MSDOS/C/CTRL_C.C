Date: Tue, 26 Jul 88 21:04:04 CDT
From: galvin%circle.UUCP@cs.wisc.edu (John Galvin)
Subject: Re: Control-c Trapping Under Msc: The Day After

    After I saw your message, I decided to do a bit more
thorough of a job.  The result is included below.  It is made up
of two files: ctrl_c.c and ctrl_c.h.  I have tested these
routines some, and they appear to handle Ctrl-C and Ctrl-Break
with no problem.  This code was written for MS C v5.1.  Some
rewriting *will* be necessary to get it to work under previous
versions of the compiler.  In particular, the do_break() and
do_intcpt() routines would have to be coded in assembler. 
Fortunately, the do_break() and do_intcpt() are merely C
versions of the stuff in intercep.asm.  So if you had to, you
could remove those routines from ctrl_c.c and replace
do_break(INTERCEPT) with intercept() and do_break(RELEASE) with
release().  If you have 5.1 things should work fine.  Have a
nice day :-) !

                           --John

------------------------ ctrl_c.h: cut here ---------------------------------
/*
**      ctrl_c.h
**
**      Author:  John Galvin
**      Date:    25-Jul-1988
**      Purpose: trap Ctrl-C and Ctrl-Break without printing anything.
**
**      Note:
**          Use of these routines will prevent Ctrl-C and Ctrl-Break
**          from terminating your program.  They will also prevent
**          Ctrl-P, Ctrl-S, and Ctrl-Z from being interpreted by
**          MS-DOS as they normally are.  Your program will be able
**          to read *all* of these characters, so if you don't want
**          them in your input stream, you will have to do something
**          about them yourself.  This code was written to work in
**          all standard memory models of the Microsoft C compiler
**          version 5.1.  Do not use getch() or getche() with these
**          routines.
**
**      $Id: ctrl_c.h 1.1 88/07/25 22:04:48 galvin Exp $
**
**      Copyright (c) 1988 by John W. Galvin.  All Rights Reserved.
**
**      Permission is hereby granted by the author for anyone to use this
**      code in their own programs provided the following conditions are met:
**      This copyright notice is left intact.  Any changes to this text/code
**      must be commented and credited to the correct author somewhere
**      in this file.
**
**      $Log:   ctrl_c.h $
**      Revision 1.1  88/07/25  22:04:48  galvin
**      Initial revision
**      
**
*/

#ifndef TRUE
#   define TRUE     1
#endif

#ifndef FALSE
#   define FALSE    0
#endif

#define INTERCEPT       0
#define RELEASE         1
#define GET             0x00
#define SET             0x01
#define CHKON           0x01
#define CHKOFF          0x00


#ifdef LINT_ARGS

int             ctrl_c(int);
int             get_ctrl_c_chk(void);
int             set_ctrl_c_chk(int);
int             rawio(int, int);
unsigned int    ioctl(int, int, unsigned int);

#else

extern int          ctrl_c(), get_ctrl_c_chk(), set_ctrl_c_chk(), 
rawio();
extern unsigned int ioctl();

#endif


------------------------ ctrl_c.c: cut here ---------------------------------
/*
**      ctrl_c.c
**
**      Author:  John Galvin
**      Date:    25-Jul-1988
**      Purpose: trap Ctrl-C and Ctrl-Break without printing anything.
**
**      Note:
**          Use of these routines will prevent Ctrl-C and Ctrl-Break
**          from terminating your program.  They will also prevent
**          Ctrl-P, Ctrl-S, and Ctrl-Z from being interpreted by
**          MS-DOS as they normally are.  Your program will be able
**          to read *all* of these characters, so if you don't want
**          them in your input stream, you will have to do something
**          about them yourself.  This code was written to work in
**          all standard memory models of the Microsoft C compiler
**          version 5.1.  Do not use getch() or getche() with these
**          routines.  See the #ifdef'd section of code below for an
**          example of how to use these functions.
**
**      Copyright (c) 1988 by John W. Galvin.  All Rights Reserved.
**
**      Permission is hereby granted by the author for anyone to use this
**      code in their own programs provided the following conditions are met:
**      This copyright notice is left intact.  Any changes to this text/code
**      must be commented and credited to the correct author somewhere
**      in this file.
**
**      $Log:   ctrl_c.c $
**      Revision 1.1  88/07/25  22:04:46  galvin
**      Initial revision
**      
**
*/

#ifndef _lint
static char *rcsid = "$Id: ctrl_c.c 1.1 88/07/25 22:04:46 galvin Exp $";
#endif

#include <stdio.h>
#include <dos.h>
#include "ctrl_c.h"


#define BREAKINTR       0x1B
#define DOS             0x21
#define CTRLCCHK        0x33
#define GETVECT         0x35
#define SETVECT         0x25
#define IOCTL           0x44
#define DEVFLAG         0x80
#define RAWFLAG         0x20



/*
**      Name:
**
**          get_ctrl_c_chk() - return status of MS-DOS Ctrl-C checking.
**
**      Synopsis:
**
**          int     get_ctrl_c_chk()
**
**      Description:
**
**          This function determines the current level of Ctrl-C
**          checking being performed by MS-DOS.  The effect of this
**          function is roughly analagous to issuing a "BREAK" command
**          at the MS-DOS command prompt.
**
**      Return Values:
**
**          This function returns FALSE if MS-DOS is only checking for
**          Ctrl-C in the 0x01 - 0x0C group of int 0x21 functions, or
**          if an error occurred.  A return of TRUE indicates that MS-DOS
**          is checking for Ctrl-C before each DOS service is performed.
**
**      Bugs:
**
**          
*/
int     get_ctrl_c_chk()
{
    union REGS      regs;

    regs.h.ah = CTRLCCHK;
    regs.h.al = GET;
    int86(DOS, &regs, &regs);

    if (regs.x.cflag)
        return(FALSE);

    return(regs.h.dl);
}



/*
**      Name:
**
**          set_ctrl_c_chk() - set the level of MS-DOS Ctrl-C checking.
**
**      Synopsis:
**
**          int     set_ctrl_c_chk(value)
**          int     value;
**
**      Description:
**
**          This function sets the level of Ctrl-C checking that MS-DOS
**          will perform.  Value should be one of the two manifest
**          constants CHKON or CHKOFF.  Calling set_ctrl_c_chk() with a
**          value of CHKON is equivalent to issuing a "BREAK ON" command
**          at the MS-DOS command line.  Calling set_ctrl_c_chk() with a
**          value of CHKOFF is equivalent to issuing a "BREAK OFF" command
**          at the MS-DOS command line.
**
**      Return Values:
**
**          This function returns FALSE if an error occurred, TRUE otherwise.
**
**      Bugs:
**
**          
*/
int     set_ctrl_c_chk(value)
int     value;
{
    union REGS      regs;

    regs.h.ah = CTRLCCHK;
    regs.h.al = SET;
    regs.h.dl = value;
    int86(DOS, &regs, &regs);

    return(!regs.x.cflag);
}



/*
**      Name:
**
**          ioctl() - MS-DOS i/o control for devices interface.
**
**      Synopsis:
**
**          unsigned int    ioctl(handle, op, value)
**          int             handle;
**          int             op;
**          unsigned int    value;
**
**      Description:
**
**          This routine provides a limited interface to the MS-DOS
**          ioctl functions.  It primarily intended to get/set the
**          device attribute bits for a file handle.  Handle should
**          an open file handle (hopefully for a device).  Op should
**          be one of SET or GET.  If op is SET, the device attribute
**          bits for handle will be set to those specified in value.
**
**      Return Values:
**
**          This function returns FALSE if an error occurred.  Otherwise
**          The new device attribute word will be returned.  The attribute
**          word may possibly be equal to FALSE.
**
**      Bugs:
**
**          
*/
unsigned int    ioctl(handle, op, value)
int             handle;
int             op;
unsigned int    value;
{
    union REGS  regs;


    regs.h.ah = IOCTL;
    regs.h.al = op;
    regs.x.bx = handle;
    regs.x.dx = value & 0xFF;
    int86(DOS, &regs, &regs);

    if (regs.x.cflag)
        return(FALSE);

    return(regs.x.dx);
}



/*
**      Name:
**
**          rawio() - set/reset a device to/from raw i/o mode.
**
**      Synopsis:
**
**          int     rawio(handle, raw)
**          int     handle;
**          int     raw;
**
**      Description:
**
**          Rawio() uses ioctl() to set/reset a device to/from raw i/o
**          mode.  When a device is in raw mode, and Ctrl-C checking is
**          turned off, Ctrl-C, Ctrl-S, Ctrl-P, and Ctrl-Z may be read
**          as data without having MS-DOS interpret them.  Handle must
**          refer to an open file/device.  If raw is TRUE, the device
**          will be set to raw mode.  Otherwise, the device will be reset
**          from raw mode.
**
**      Return Values:
**
**          This function returns a non-zero value if the device was
**          in raw mode, 0 otherwise.
**
**      Bugs:
**
**          No checks are made for errors from ioctl().
*/
int     rawio(handle, raw)
int     handle;
int     raw;
{
    unsigned int    flags;

    flags = ioctl(handle, GET, 0);
    if (flags & DEVFLAG) {
        if (raw)
            ioctl(handle, SET, flags | RAWFLAG);
        else
            ioctl(handle, SET, flags & ~RAWFLAG);
    }

    return(flags & RAWFLAG);
}



/*
**      Name:
**
**          do_intcpt() - Ctrl-Break Interrupt (0x1B) routine.
**
**      Synopsis:
**
**          void interrupt cdecl far    do_intcpt()
**
**      Description:
**
**          This routine is the dummy interrupt routine which is used
**          to intercept the IBM PC Break interrupt.
**
**      Return Values:
**
**          None.
**
**      Bugs:
**
**          
*/
static void interrupt cdecl far do_intcpt()
{
    return;
}



/*
**      Name:
**
**          do_break() - intercept/release the IBM PC Break Interrupt.
**
**      Synopsis:
**
**          static void do_break(op)
**          int         op;
**
**      Description:
**
**          If passed the manifest constant INTERCEPT, this routine
**          will use do_intcpt() to intercept the IBM PC Ctrl-Break
**          interrupt.  If this routine is not used while trapping
**          Ctrl-C and Ctrl-Break, a ^C may turn up *after* the porgram
**          exits.  When passed the manifest constant RELEASE, do_break()
**          will de-install its interrupt handler.
**
**      Return Values:
**
**          None.
**
**      Bugs:
**
**          No error checks are performed on the returns of int86x().
*/
static void do_break(op)
int         op;
{
    union REGS          regs;
    struct SREGS        segs;
    static unsigned int oldseg;
    static unsigned int oldofs;

    if (op == INTERCEPT) {
        segread(&segs);
        regs.h.ah = GETVECT;
        regs.h.al = BREAKINTR;
        int86x(DOS, &regs, &regs, &segs);
        oldseg = segs.es;
        oldofs = regs.x.bx;

        segread(&segs);
        regs.x.dx = (unsigned int) do_intcpt;
        segs.ds   = ((unsigned long) do_intcpt) >> 16;
    }
    else {
        segread(&segs);
        regs.x.dx = oldofs;
        segs.ds   = oldseg;
    }
    regs.h.ah = SETVECT;
    regs.h.al = BREAKINTR;
    int86x(DOS, &regs, &regs, &segs);
}



/*
**      Name:
**
**          ctrl_c() - nullify Ctrl-C and Ctrl-Break.
**
**      Synopsis:
**
**          int     ctrl_c(op)
**          int     op;
**
**      Description:
**
**          If passed the manifest constant INTERCEPT, ctrl_c() will
**          prevent MS-DOS from interpreting Ctrl-C, Ctrl-Break, Ctrl-P,
**          Ctrl-S, and Ctrl-Z as it normally does.  I.E. a Ctrl-C or
**          Ctrl-Break will not cause a ^C to be output and will not
**          abort the program.  Do not use getch(), or getche() while
**          this routine is in effect, they do not properly support
**          this mode.  If passed the manifest constant RELEASE, ctrl_c()
**          will return the Ctrl character interpretation that existed
**          before ctrl_c(INTERCEPT) was called.
**
**      Return Values:
**
**          Ctrl_c() always returns TRUE.
**
**      Bugs:
**
**          No error checking is done.
*/
int     ctrl_c(op)
int     op;
{
    static unsigned int outflg;
    static unsigned int inflg;
    static unsigned int ctrlcchk;


    if (op == INTERCEPT) {
        do_break(INTERCEPT);
        inflg  = rawio(fileno(stdin), TRUE);
        outflg = rawio(fileno(stdout), TRUE);
        if ((ctrlcchk = get_ctrl_c_chk()) == CHKON)
            set_ctrl_c_chk(CHKOFF);
    }
    else {
        set_ctrl_c_chk(ctrlcchk);
        rawio(fileno(stdout), outflg);
        rawio(fileno(stdin), inflg);
        do_break(RELEASE);
    }

    return(TRUE);
}



#ifdef CTRL_CDEBUG

#include <bios.h>

int     main()
{
    int     achar;

    ctrl_c(INTERCEPT);
    while ((achar = _bios_keybrd(_KEYBRD_READ) & 0xFF) != ' ')
        putch(achar);
    ctrl_c(RELEASE);
    return(0);
}

#endif


----------------------------- the end ---------------------------------------
  
--  
John Galvin         ARPA:    galvin@circle.UUCP
1810 Fordem Ave. #6 UUCP:    ...!uwvax!geowhiz!circle!galvin
Madison, Wi  53704  FidoNet: Sysop of 1:121/0, and 1:121/1.  (608) 249-0275
