/* $XFree86: mit/server/ddx/x386/vga16/ibm/wm3.c,v 2.1 1994/02/25 15:02:52 dawes Exp $ */
#include "compiler.h"
#include "vgaReg.h"
#include "vgaVideo.h"
#include "X.h"
#include "gcstruct.h"
#include "wm3.h"

/* Ferraro is wrong. GJA */
#define COPY (0 << 3)
#define AND  (1 << 3)
#define OR   (2 << 3)
#define XOR  (3 << 3)

wm3_set_regs(pGC)
GC *pGC;
{
    int post_invert = 0;
    int ALU;

    switch(pGC->alu) {
    case GXclear:        /* rop0 = RROP_BLACK;  rop1 = RROP_BLACK; */
        pGC->fgPixel = 0;
        pGC->bgPixel = 0;
        ALU = COPY;
        break;
    case GXand:          /* rop0 = RROP_BLACK;  rop1 = RROP_NOP; */
        ALU = AND;
        break;
    case GXandReverse:   /* rop0 = RROP_BLACK;  rop1 = RROP_INVERT; -- TRICKY */
        pGC->fgPixel = ~pGC->fgPixel;
        pGC->bgPixel = ~pGC->bgPixel;
        ALU = OR;
        post_invert = 1;
        break;
    case GXcopy:         /* rop0 = RROP_BLACK;  rop1 = RROP_WHITE; */
        ALU = COPY;
        break;
    case GXandInverted:  /* rop0 = RROP_NOP;    rop1 = RROP_BLACK; */
        pGC->fgPixel = ~pGC->fgPixel;
        pGC->bgPixel = ~pGC->bgPixel;
        ALU = AND;
        break;
    case GXnoop:         /* rop0 = RROP_NOP;    rop1 = RROP_NOP; */
        return 0;
    case GXxor:          /* rop0 = RROP_NOP;    rop1 = RROP_INVERT; */
        ALU = XOR;
        break;
    case GXor:           /* rop0 = RROP_NOP;    rop1 = RROP_WHITE; */
        ALU = OR;
        break;
    case GXnor:          /* rop0 = RROP_INVERT; rop1 = RROP_BLACK; -- TRICKY*/
        ALU = OR;
        post_invert = 1;
        break;
    case GXequiv:        /* rop0 = RROP_INVERT; rop1 = RROP_NOP; */
        pGC->fgPixel = ~pGC->fgPixel;
        pGC->bgPixel = ~pGC->bgPixel;
        ALU = XOR;
        break;
    case GXinvert:       /* rop0 = RROP_INVERT; rop1 = RROP_INVERT; */
        pGC->fgPixel = 0x0F;
        pGC->bgPixel = 0x0F;
        ALU = XOR;
        break;
    case GXorReverse:    /* rop0 = RROP_INVERT; rop1 = RROP_WHITE; -- TRICKY */
        pGC->fgPixel = ~pGC->fgPixel;
        pGC->bgPixel = ~pGC->bgPixel;
        ALU = AND;
        post_invert = 1;
        break;
    case GXcopyInverted: /* rop0 = RROP_WHITE;  rop1 = RROP_BLACK; */
        pGC->fgPixel = ~pGC->fgPixel;
        pGC->bgPixel = ~pGC->bgPixel;
        ALU = COPY;
        break;
    case GXorInverted:   /* rop0 = RROP_WHITE;  rop1 = RROP_NOP; */
        pGC->fgPixel = ~pGC->fgPixel;
        pGC->bgPixel = ~pGC->bgPixel;
        ALU = OR;
        break;
    case GXnand:         /* rop0 = RROP_WHITE;  rop1 = RROP_INVERT; -- TRICKY */
        ALU = OR;
        post_invert = 1;
        break;
    case GXset:          /* rop0 = RROP_WHITE;  rop1 = RROP_WHITE; */
        pGC->fgPixel = 0x0F;
        pGC->bgPixel = 0x0F;
        ALU = COPY;
        break;
    }


    SetVideoSequencer(Mask_MapIndex, (pGC->planemask & VGA_ALLPLANES));
    SetVideoGraphics(Enb_Set_ResetIndex, VGA_ALLPLANES);
    SetVideoGraphics(Set_ResetIndex, pGC->fgPixel);
    SetVideoGraphics(Bit_MaskIndex, 0xFF);
    SetVideoGraphics(Graphics_ModeIndex, 3); /* Write Mode 3 */
    SetVideoGraphics(Data_RotateIndex, ALU);

    return post_invert;
}

/*
   Now we will have to set the alu.
   Problematic is: How do we handle IsDoubleDash, which draws with both fg
   and bg colour?
   The answer is: We take care to store the ink colour in one register only.
   Then we need set only this register to change the ink.
 */
 
/* -- MORE NOTES:
   We might try to 'wrap' the mfb functions in a 16-colour wrapper that does
   all operations once. However, this does not work:
   Some operations (esp) CopyArea may for example cause expositions.
   Such expositions will cause data to be copied in from backing store,
   and this copying in may damage the contents of the VGA registers.
   So we must take care to set the VGA registers at each place where they could
   be modified.
 */
