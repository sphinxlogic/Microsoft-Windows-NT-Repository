/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $XFree86: mit/server/ddx/x386/vga16/ibm/vgaSolid.c,v 2.2 1994/03/08 04:51:14 dawes Exp $ */
/* Header: /andrew/X11/r3src/release/server/ddx/ibm/vga/RCS/vgaSolid.c,v 6.2 89/04/29 21:46:50 jeff Exp */
/* Source: /andrew/X11/r3src/release/server/ddx/ibm/vga/RCS/vgaSolid.c,v */

#include "X.h"

#include "OScompiler.h"

/* #include "ibmIOArch.h" /* GJA */

#include "vgaVideo.h"
#include "vgaReg.h"

#undef TRUE
#undef FALSE
#define TRUE 1
#define FALSE 0

#ifdef USE_ASM
extern void fastFill();
extern void fastFillRMW();
#else

static void fastFill( destination, bytewidth, height )
register volatile unsigned char *destination ;
register const unsigned int bytewidth ;	/* MUST BE > 0 !! */
register unsigned int height ;		/* MUST BE > 0 !! */
{
int stop_count = bytewidth ;
register int row_jump = BYTES_PER_LINE - bytewidth ;
#if !defined(OLDHC) && defined(BSDrt) && !defined(i386)
register const int notZero = ~0x0 ;
#else
#define notZero ( ~0 )
#endif

#define SINGLE_STORE \
    ( *( (VgaMemoryPtr) destination ) = notZero ); \
    VINCRW(destination) ; stop_count--;

VSETRW(destination);

/* TOP OF FIRST LOOP */
BranchPoint:

switch ( bytewidth & 0xF ) { /* Jump into loop at mod 16 remainder */
	LoopTop :
	case 0x0 : SINGLE_STORE ;
	case 0xF : SINGLE_STORE ;
	case 0xE : SINGLE_STORE ;
	case 0xD : SINGLE_STORE ;
	case 0xC : SINGLE_STORE ;
	case 0xB : SINGLE_STORE ;
	case 0xA : SINGLE_STORE ;
	case 0x9 : SINGLE_STORE ;
	case 0x8 : SINGLE_STORE ;
	case 0x7 : SINGLE_STORE ;
	case 0x6 : SINGLE_STORE ;
	case 0x5 : SINGLE_STORE ;
	case 0x4 : SINGLE_STORE ;
	case 0x3 : SINGLE_STORE ;
	case 0x2 : SINGLE_STORE ;
	case 0x1 : SINGLE_STORE ;
/* FIRST LOOP */
		if ( stop_count )
			goto LoopTop ;
/* SECOND LOOP */
		if ( --height ) {
			destination += row_jump ;
			VCHECKRWO(destination);
			stop_count = bytewidth ;
			goto BranchPoint ;
		}
		else
			return ;
#undef SINGLE_STORE
}
/*NOTREACHED*/
}

/* For Read-Modify-Write Case */
static void fastFillRMW( destination, bytewidth, height )
register volatile unsigned char *destination ;
register const unsigned int bytewidth ;	/* MUST BE > 0 !! */
register unsigned int height ;		/* MUST BE > 0 !! */
{
int stop_count = bytewidth ;
register int row_jump = BYTES_PER_LINE - bytewidth ;
#if !defined(OLDHC) && defined(BSDrt) && !defined(i386)
register const int notZero = ~0x0 ;
#else
#define notZero ( ~0 )
#endif
register int tmp ;

#define SINGLE_STORE \
    tmp = *( (VgaMemoryPtr) destination ) ; \
    ( *( (VgaMemoryPtr) destination ) = notZero ) ; \
    VINCRW(destination) ; stop_count-- ;

VSETRW(destination);

/* TOP OF FIRST LOOP */
BranchPoint:

switch ( bytewidth & 0xF ) { /* Jump into loop at mod 16 remainder */
	LoopTop :
	case 0x0 : SINGLE_STORE ;
	case 0xF : SINGLE_STORE ;
	case 0xE : SINGLE_STORE ;
	case 0xD : SINGLE_STORE ;
	case 0xC : SINGLE_STORE ;
	case 0xB : SINGLE_STORE ;
	case 0xA : SINGLE_STORE ;
	case 0x9 : SINGLE_STORE ;
	case 0x8 : SINGLE_STORE ;
	case 0x7 : SINGLE_STORE ;
	case 0x6 : SINGLE_STORE ;
	case 0x5 : SINGLE_STORE ;
	case 0x4 : SINGLE_STORE ;
	case 0x3 : SINGLE_STORE ;
	case 0x2 : SINGLE_STORE ;
	case 0x1 : SINGLE_STORE ;
/* FIRST LOOP */
		if ( stop_count )
			goto LoopTop ;
/* SECOND LOOP */
		if ( --height ) {
			destination += row_jump ;
			VCHECKRWO(destination);
			stop_count = bytewidth ;
			goto BranchPoint ;
		}
		else
			return ;
}
#undef SINGLE_STORE
/*NOTREACHED*/
}
#endif


void vgaFillSolid( color, alu, planes, x0, y0, lx, ly )
unsigned long int color ;
const int alu ;
unsigned long int planes ;
register int x0 ;
register const int y0 ;
register int lx ;
register const int ly ;		/* MUST BE > 0 !! */
{
register volatile unsigned char *dst ;
register tmp ;
register tmp2 ;
register tmp3 ;
unsigned int data_rotate_value = VGA_COPY_MODE ;
unsigned int read_write_modify = FALSE ;
unsigned int invert_existing_data = FALSE ;

{	/* Start GJA */
	extern int x386VTSema;

	if ( !x386VTSema ) {
		offFillSolid( color, alu, planes, x0, y0, lx, ly );
		return;
	}
}	/* End GJA */

if ( ( lx == 0 ) || ( ly == 0 ) )
	return;

switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
		color = 0 ;
		break ;
	case GXnor:		/* 0x8 NOT src AND NOT dst */
		invert_existing_data = TRUE ;
	case GXandInverted:	/* 0x4 NOT src AND dst */
		color = ~color ;
	case GXand:		/* 0x1 src AND dst */
		data_rotate_value = VGA_AND_MODE ;
		read_write_modify = TRUE ;
	case GXcopy:		/* 0x3 src */
		break ;
	case GXnoop:		/* 0x5 dst */
		return ;
	case GXequiv:		/* 0x9 NOT src XOR dst */
		color = ~color ;
	case GXxor:		/* 0x6 src XOR dst */
		data_rotate_value = VGA_XOR_MODE ;
		read_write_modify = TRUE ;
		planes &= color ;
		break ;
	case GXandReverse:	/* 0x2 src AND NOT dst */
		invert_existing_data = TRUE ;
		data_rotate_value = VGA_AND_MODE ;
		read_write_modify = TRUE ;
		break ;
	case GXorReverse:	/* 0xb src OR NOT dst */
		invert_existing_data = TRUE ;
		data_rotate_value = VGA_OR_MODE ;
		read_write_modify = TRUE ;
		break ;
	case GXnand:		/* 0xe NOT src OR NOT dst */
		invert_existing_data = TRUE ;
	case GXorInverted:	/* 0xd NOT src OR dst */
		color = ~color ;
	case GXor:		/* 0x7 src OR dst */
		data_rotate_value = VGA_OR_MODE ;
		read_write_modify = TRUE ;
		break ;
	case GXcopyInverted:	/* 0xc NOT src */
		color = ~color ;
		break ;
	case GXinvert:		/* 0xa NOT dst */
		data_rotate_value = VGA_XOR_MODE ;
		read_write_modify = TRUE ;
	case GXset:		/* 0xf 1 */
		color = VGA_ALLPLANES ;
	default:
		break ;
}

if ( !( planes &= VGA_ALLPLANES ) )
	return ;

/*
 * Set The Plane-Enable
 */
SetVideoSequencer( Mask_MapIndex, planes ) ;
SetVideoGraphics( Enb_Set_ResetIndex, planes ) ;
/*
 * Put Display Into SET/RESET Write Mode
 */
SetVideoGraphics( Graphics_ModeIndex, VGA_WRITE_MODE_3 ) ;
/*
 * Set The Color in The Set/Reset Register
 */
SetVideoGraphics( Set_ResetIndex, color & VGA_ALLPLANES ) ;
/*
 * Set The Function-Select In The Data Rotate Register
 */
SetVideoGraphics( Data_RotateIndex, data_rotate_value ) ;

/* Do Left Edge */
if ( tmp = x0 & 07 ) {
	tmp2 = SCRRIGHT8( ( (unsigned) 0xFF ), tmp ) ;
	/* Catch The Cases Where The Entire Region Is Within One Byte */
	if ( ( lx -= 8 - tmp ) < 0 ) {
		tmp2 &= SCRLEFT8( 0xFF, -lx ) ;
		lx = 0 ;
	}
	/* Set The Bit Mask Reg */
        SetVideoGraphics(Bit_MaskIndex, tmp2 ) ;
	if ( invert_existing_data == TRUE ) {
                SetVideoGraphics( Set_ResetIndex, VGA_ALLPLANES ) ;
		SetVideoGraphics( Data_RotateIndex, VGA_XOR_MODE ) ;
		dst = SCREENADDRESS( x0, y0 ); VSETRW(dst) ;
		for ( tmp = ly;
		      tmp-- ; ) {
			tmp3 = *( (VgaMemoryPtr) dst ) ;
			*( (VgaMemoryPtr) dst ) = tmp2 ;
			dst += BYTES_PER_LINE; VCHECKRWO(dst);
		}
                SetVideoGraphics( Set_ResetIndex, color & VGA_ALLPLANES ) ;
		SetVideoGraphics( Data_RotateIndex, data_rotate_value ) ;
			/* Un-Set XOR */
	}
	dst = SCREENADDRESS( x0, y0 ); VSETRW(dst) ;
	for ( tmp = ly;
	      tmp-- ; ) {
		tmp3 = *( (VgaMemoryPtr) dst ) ;
		*( (VgaMemoryPtr) dst ) = tmp2 ;
		dst += BYTES_PER_LINE; VCHECKRWO(dst) ;
	}
	if ( !lx ) { /* All Handled In This Byte */
		return ;
	}
	x0 = ( x0 + 8 ) & ~07 ;
}

/* Fill The Center Of The Box */
if ( ROW_OFFSET( lx ) ) {
	SetVideoGraphics(Bit_MaskIndex, 0xFF ) ;
	if ( invert_existing_data == TRUE ) {
                SetVideoGraphics( Set_ResetIndex, VGA_ALLPLANES ) ;
		SetVideoGraphics( Data_RotateIndex, VGA_XOR_MODE ) ;
		fastFillRMW( SCREENADDRESS( x0, y0 ),
			     ROW_OFFSET( lx ), ly ) ;
                SetVideoGraphics( Set_ResetIndex, color & VGA_ALLPLANES ) ;
		SetVideoGraphics( Data_RotateIndex, data_rotate_value ) ;
			/* Un-Set XOR */
		/* Point At The Bit Mask Reg */
	}
	(* ( ( read_write_modify == FALSE ) ? fastFill : fastFillRMW ) )
		( SCREENADDRESS( x0, y0 ), ROW_OFFSET( lx ), ly ) ;
}

/* Do Right Edge */
if ( tmp = BIT_OFFSET( lx ) ) { /* x0 Now Is Byte Aligned */
	/* Set The Bit Mask */
	SetVideoGraphics( Bit_MaskIndex,
		(tmp2 = SCRLEFT8( 0xFF, ( 8 - tmp ) ) ) ) ;
	if ( invert_existing_data == TRUE ) {
                SetVideoGraphics( Set_ResetIndex, VGA_ALLPLANES ) ;
		SetVideoGraphics( Data_RotateIndex, VGA_XOR_MODE ) ;
		dst = SCREENADDRESS( ( x0 + lx ), y0 ); VSETRW(dst) ;
		for ( tmp = ly; 
		      tmp-- ; ) {
			tmp3 = *( (VgaMemoryPtr) dst ) ;
			*( (VgaMemoryPtr) dst ) = tmp2 ;
			dst += BYTES_PER_LINE; VCHECKRWO(dst) ;
		}
                SetVideoGraphics( Set_ResetIndex, color & VGA_ALLPLANES ) ;
		SetVideoGraphics( Data_RotateIndex, data_rotate_value ) ;
			/* Un-Set XOR */
	}
	dst = SCREENADDRESS( ( x0 + lx ), y0 ); VSETRW(dst) ;
	for ( tmp = ly; 
	      tmp-- ; ) {
		tmp3 = *( (VgaMemoryPtr) dst ) ;
		*( (VgaMemoryPtr) dst ) = tmp2 ;
		dst += BYTES_PER_LINE ; VCHECKRWO(dst) ;
	}
}
/* Disable Set/Reset Register */
SetVideoGraphics( Enb_Set_ResetIndex, 0 ) ;


return ;
}
