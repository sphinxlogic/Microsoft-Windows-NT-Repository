/*
 * BDM2: Banked dumb monochrome driver
 * Pascal Haible 8/93, haible@izfm.uni-stuttgart.de
 *
 * bdm2/driver/hgc1280/hgc1280Ports.h
 *
 * see bdm2/COPYRIGHT for copyright and disclaimers.
 */

/* $XFree86: mit/server/ddx/x386/bdm2/drivers/hgc1280/hgc1280Port.h,v 2.2 1993/12/01 12:37:18 dawes Exp $ */

/* Primary I/O Base		0x3B0
 * Secondary I/O Base		0x390
 */
#if 0
#define HGC_PORT_BASE           0x3B0
#define HGC_PORT_INDEX          HGC_PORT_BASE
#define HGC_PORT_DATA           HGC_PORT_BASE+0x01
#define HGC_PORT_CONTROL        HGC_PORT_BASE+0x08
#define HGC_PORT_CRT_STATUS     HGC_PORT_BASE+0x0A
#define HGC_PORT_CONFIG         HGC_PORT_BASE+0x0F
#endif

#if defined(C_STYLE_HEX_CONSTANTS)
#undef CONST
#define CONST(a)	(a)
#endif


#define HGC_PRIM_PORT_BASE           CONST(0x3B0)
#define HGC_PRIM_PORT_INDEX          CONST(0x3B0)
#define HGC_PRIM_PORT_DATA           CONST(0x3B1)
#define HGC_PRIM_PORT_CONTROL        CONST(0x3B8)
#define HGC_PRIM_PORT_CRT_STATUS     CONST(0x3BA)
#define HGC_PRIM_PORT_CONFIG         CONST(0x3BF)

#define HGC_SEC_PORT_BASE           CONST(0x390)
#define HGC_SEC_PORT_INDEX          CONST(0x390)
#define HGC_SEC_PORT_DATA           CONST(0x391)
#define HGC_SEC_PORT_CONTROL        CONST(0x398)
#define HGC_SEC_PORT_CRT_STATUS     CONST(0x39A)
#define HGC_SEC_PORT_CONFIG         CONST(0x39F)

#define HGC_REG_BANK1           CONST(0x22)
#define HGC_REG_BANK2           CONST(0x24)
#define HGC_REG_SHIFT_DISPLAY   CONST(0x2D)
#define HGC_REG_LEFT_BORDER     CONST(0x2A)
#define HGC_REG_RIGHT_BORDER    CONST(0x2B)

