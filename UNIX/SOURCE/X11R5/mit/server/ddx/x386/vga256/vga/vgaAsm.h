/* $XFree86: mit/server/ddx/x386/vga256/vga/vgaAsm.h,v 2.0 1994/02/10 21:27:08 dawes Exp $ */

/* Definitions for VGA bank assembler routines */

#if defined(__386BSD__) || defined(__NetBSD__) || defined(__FreeBSD__) || defined(__bsdi__)
#define VGABASE CONST(0xFF000000)
#else
#define VGABASE CONST(0xF0000000)
#endif

