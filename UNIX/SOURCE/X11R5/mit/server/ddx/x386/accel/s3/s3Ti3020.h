/*
 * Copyright 1994 by Robin Cutshaw <robin@paros.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Robin Cutshaw not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Robin Cutshaw makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ROBIN CUTSHAW DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ROBIN CUTSHAW BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: mit/server/ddx/x386/accel/s3/s3Ti3020.h,v 2.1 1994/01/15 02:50:40 dawes Exp $ */

#include "compiler.h"
#include <X11/Xfuncproto.h>

/*
#define MAX_TI3020_CLOCK       135000
#define MAX_TI3020_CLOCK_FAST  200000
*/

/* Direct standard VGA registers, special index and data registers */

#define TI_WRITE_ADDR		0x3C8   /* CR55 low bit == 0 */
#define TI_RAMDAC_DATA		0x3C9   /* CR55 low bit == 0 */
#define TI_PIXEL_MASK		0x3C6   /* CR55 low bit == 0 */
#define TI_READ_ADDR		0x3C7   /* CR55 low bit == 0 */
#define TI_INDEX_REG		0x3C6   /* CR55 low bit == 1 */
#define TI_DATA_REG		0x3C7   /* CR55 low bit == 1 */

/* Indirect indexed registers */

#define TI_CURS_X_LOW		0x00
#define TI_CURS_X_HIGH		0x01    /* only lower 4 bits are used */
#define TI_CURS_Y_LOW		0x02
#define TI_CURS_Y_HIGH		0x03    /* only lower 4 bits are used */
#define TI_SPRITE_ORIGIN_X	0x04
#define TI_SPRITE_ORIGIN_Y	0x05
#define TI_CURS_CONTROL		0x06
#define   TI_CURS_SPRITE_ENABLE 0x40
#define   TI_CURS_X_WINDOW_MODE 0x10
#define   TI_CURS_CTRL_MASK     (TI_CURS_SPRITE_ENABLE | TI_CURS_X_WINDOW_MODE)
#define TI_CURS_RAM_ADDR_LOW	0x08
#define TI_CURS_RAM_ADDR_HIGH	0x09
#define TI_CURS_RAM_DATA	0x0A
#define TI_WINDOW_START_X_LOW	0x10
#define TI_WINDOW_START_X_HIGH	0x11
#define TI_WINDOW_STOP_X_LOW	0x12
#define TI_WINDOW_STOP_X_HIGH	0x13
#define TI_WINDOW_START_Y_LOW	0x14
#define TI_WINDOW_START_Y_HIGH	0x15
#define TI_WINDOW_STOP_Y_LOW	0x16
#define TI_WINDOW_STOP_Y_HIGH	0x17
#define TI_MUX_CONTROL_1	0x18
#define   TI_MUX1_PSEUDO_COLOR	0x80
#define TI_MUX_CONTROL_2	0x19
#define   TI_MUX2_BUS_VGA	0x98
#define   TI_MUX2_BUS_PIX64	0x1C
#define TI_INPUT_CLOCK_SELECT	0x1A
#define   TI_ICLK_CLK0		0x00
#define   TI_ICLK_CLK0_DOUBLE	0x10
#define   TI_ICLK_CLK1		0x01
#define   TI_ICLK_CLK1_DOUBLE	0x11
#define TI_OUTPUT_CLOCK_SELECT	0x1B
#define   TI_OCLK_VGA		0x3E
#define   TI_OCLK_S_V4_R8	0x53
#define TI_PALETTE_PAGE		0x1C
#define TI_GENERAL_CONTROL	0x1D
#define TI_OVERSCAN_COLOR_RED	0x20
#define TI_OVERSCAN_COLOR_GREEN	0x21
#define TI_OVERSCAN_COLOR_BLUE	0x22
#define TI_CURSOR_COLOR_0_RED	0x23
#define TI_CURSOR_COLOR_0_GREEN	0x24
#define TI_CURSOR_COLOR_0_BLUE	0x25
#define TI_CURSOR_COLOR_1_RED	0x26
#define TI_CURSOR_COLOR_1_GREEN	0x27
#define TI_CURSOR_COLOR_1_BLUE	0x28
#define TI_AUXILLARY_CONTROL	0x29
#define   TI_AUX_SELF_CLOCK	0x08
#define   TI_AUX_W_CMPL		0x01
#define TI_GENERAL_IO_CONTROL	0x2A
#define   TI_GIC_ALL_BITS	0x1F
#define TI_GENERAL_IO_DATA	0x2B
#define   TI_GID_S3_DAC_6BIT	0x1C
#define   TI_GID_S3_DAC_8BIT	0x1E
#define   TI_GID_TI_DAC_6BIT	0x1D
#define   TI_GID_TI_DAC_8BIT	0x1F
#define TI_COLOR_KEY_OLVGA_LOW	0x30
#define TI_COLOR_KEY_OLVGA_HIGH	0x31
#define TI_COLOR_KEY_RED_LOW	0x32
#define TI_COLOR_KEY_RED_HIGH	0x33
#define TI_COLOR_KEY_GREEN_LOW	0x34
#define TI_COLOR_KEY_GREEN_HIGH	0x35
#define TI_COLOR_KEY_BLUE_LOW	0x36
#define TI_COLOR_KEY_BLUE_HIGH	0x37
#define TI_COLOR_KEY_CONTROL	0x38
#define TI_SENSE_TEST		0x3A
#define TI_TEST_DATA		0x3B
#define TI_CRC_LOW		0x3C
#define TI_CRC_HIGH		0x3D
#define TI_CRC_CONTROL		0x3E
#define TI_ID			0x3F
#define   TI_VIEWPOINT_ID	0x20


_XFUNCPROTOBEGIN

extern void s3OutTiIndReg(
#if NeedFunctionPrototypes
	unsigned char,
	unsigned char,
	unsigned char
#endif
);

extern unsigned char s3InTiIndReg(
#if NeedFunctionPrototypes
	unsigned char
#endif
);

_XFUNCPROTOEND
