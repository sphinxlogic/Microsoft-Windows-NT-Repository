/*
 *	oneko  -  X11 $@G-(J
 *
 *	$Header: /home/sun/unix/kato/xsam/oneko/oneko.h,v 1.4 90/10/18 16:52:57 kato Exp Locker: kato $
 */

/*
 *	$@%$%s%/%k!<%I%U%!%$%k(J
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#include <stdio.h>

#include <signal.h>
#include <math.h>
#include <sys/time.h>

/*
 *	$@%+!<%=%k%S%C%H%^%C%W%U%!%$%k$NFI$_9~$_(J
 */

#include "bitmaps/cursor.xbm"
#include "bitmaps/cursor_mask.xbm"

/*
 *	$@IaDL$NG-MQ%S%C%H%^%C%W%U%!%$%k$NFI$_9~$_(J
 */

#include "bitmaps/awake.xbm"
#include "bitmaps/down1.xbm"
#include "bitmaps/down2.xbm"
#include "bitmaps/dtogi1.xbm"
#include "bitmaps/dtogi2.xbm"
#include "bitmaps/dwleft1.xbm"
#include "bitmaps/dwleft2.xbm"
#include "bitmaps/dwright1.xbm"
#include "bitmaps/dwright2.xbm"
#include "bitmaps/jare2.xbm"
#include "bitmaps/kaki1.xbm"
#include "bitmaps/kaki2.xbm"
#include "bitmaps/left1.xbm"
#include "bitmaps/left2.xbm"
#include "bitmaps/ltogi1.xbm"
#include "bitmaps/ltogi2.xbm"
#include "bitmaps/mati2.xbm"
#include "bitmaps/mati3.xbm"
#include "bitmaps/right1.xbm"
#include "bitmaps/right2.xbm"
#include "bitmaps/rtogi1.xbm"
#include "bitmaps/rtogi2.xbm"
#include "bitmaps/sleep1.xbm"
#include "bitmaps/sleep2.xbm"
#include "bitmaps/up1.xbm"
#include "bitmaps/up2.xbm"
#include "bitmaps/upleft1.xbm"
#include "bitmaps/upleft2.xbm"
#include "bitmaps/upright1.xbm"
#include "bitmaps/upright2.xbm"
#include "bitmaps/utogi1.xbm"
#include "bitmaps/utogi2.xbm"

/*
 *	$@$H$iG-MQ%S%C%H%^%C%W%U%!%$%k$NFI$_9~$_(J
 */

#include "bitmaps/awake_tora.xbm"
#include "bitmaps/down1_tora.xbm"
#include "bitmaps/down2_tora.xbm"
#include "bitmaps/dtogi1_tora.xbm"
#include "bitmaps/dtogi2_tora.xbm"
#include "bitmaps/dwleft1_tora.xbm"
#include "bitmaps/dwleft2_tora.xbm"
#include "bitmaps/dwright1_tora.xbm"
#include "bitmaps/dwright2_tora.xbm"
#include "bitmaps/jare2_tora.xbm"
#include "bitmaps/kaki1_tora.xbm"
#include "bitmaps/kaki2_tora.xbm"
#include "bitmaps/left1_tora.xbm"
#include "bitmaps/left2_tora.xbm"
#include "bitmaps/ltogi1_tora.xbm"
#include "bitmaps/ltogi2_tora.xbm"
#include "bitmaps/mati2_tora.xbm"
#include "bitmaps/mati3_tora.xbm"
#include "bitmaps/right1_tora.xbm"
#include "bitmaps/right2_tora.xbm"
#include "bitmaps/rtogi1_tora.xbm"
#include "bitmaps/rtogi2_tora.xbm"
#include "bitmaps/sleep1_tora.xbm"
#include "bitmaps/sleep2_tora.xbm"
#include "bitmaps/up1_tora.xbm"
#include "bitmaps/up2_tora.xbm"
#include "bitmaps/upleft1_tora.xbm"
#include "bitmaps/upleft2_tora.xbm"
#include "bitmaps/upright1_tora.xbm"
#include "bitmaps/upright2_tora.xbm"
#include "bitmaps/utogi1_tora.xbm"
#include "bitmaps/utogi2_tora.xbm"

/*
 *	$@%S%C%H%^%9%/%U%!%$%k$NFI$_9~$_(J
 */

#include "bitmasks/mati2_mask.xbm"
#include "bitmasks/jare2_mask.xbm"
#include "bitmasks/kaki1_mask.xbm"
#include "bitmasks/kaki2_mask.xbm"
#include "bitmasks/mati3_mask.xbm"
#include "bitmasks/sleep1_mask.xbm"
#include "bitmasks/sleep2_mask.xbm"
#include "bitmasks/awake_mask.xbm"
#include "bitmasks/up1_mask.xbm"
#include "bitmasks/up2_mask.xbm"
#include "bitmasks/down1_mask.xbm"
#include "bitmasks/down2_mask.xbm"
#include "bitmasks/left1_mask.xbm"
#include "bitmasks/left2_mask.xbm"
#include "bitmasks/right1_mask.xbm"
#include "bitmasks/right2_mask.xbm"
#include "bitmasks/upright1_mask.xbm"
#include "bitmasks/upright2_mask.xbm"
#include "bitmasks/upleft1_mask.xbm"
#include "bitmasks/upleft2_mask.xbm"
#include "bitmasks/dwleft1_mask.xbm"
#include "bitmasks/dwleft2_mask.xbm"
#include "bitmasks/dwright1_mask.xbm"
#include "bitmasks/dwright2_mask.xbm"
#include "bitmasks/utogi1_mask.xbm"
#include "bitmasks/utogi2_mask.xbm"
#include "bitmasks/dtogi1_mask.xbm"
#include "bitmasks/dtogi2_mask.xbm"
#include "bitmasks/ltogi1_mask.xbm"
#include "bitmasks/ltogi2_mask.xbm"
#include "bitmasks/rtogi1_mask.xbm"
#include "bitmasks/rtogi2_mask.xbm"


/*
 *	$@Dj?tDj5A(J
 */

#define	BITMAP_WIDTH		32	/* $@#1%-%c%i%/%?$NI}(J ($@%T%/%;%k(J) */
#define	BITMAP_HEIGHT		32	/* $@#1%-%c%i%/%?$N9b$5(J ($@%T%/%;%k(J) */

#define	AVAIL_KEYBUF		255
#define	EVENT_MASK		(KeyPressMask | ExposureMask)
#define	MAX_TICK		9999		/* Odd Only! */

#define	DEFAULT_FOREGROUND	"black"		/* $@%U%)%"%0%i%&%s%I%+%i!<(J */
#define	DEFAULT_BACKGROUND	"white"		/* $@%P%C%/%0%i%&%s%I%+%i!<(J */
#define	DEFAULT_INTERVAL	125000L		/* $@%$%s%?!<%P%k%?%$%`(J */
#define	DEFAULT_NEKO_SPEED	16		/* $@G-$NB.EY(J */
#define	DEFAULT_IDLE_SPACE	6		/* $@G-$N46EY(J */

#define	NORMAL_NEKO		0		/* $@IaDL$NG-(J */
#define	TORA_NEKO		1		/* $@$H$iG-(J */

/*
 *	$@G-$N>uBVDj?t(J
 */

#define	NEKO_STOP		0	/* $@N)$A;_$^$C$?(J */
#define	NEKO_JARE		1	/* $@4i$r@v$C$F$$$k(J */
#define	NEKO_KAKI		2	/* $@F,$rA_$$$F$$$k(J */
#define	NEKO_AKUBI		3	/* $@$"$/$S$r$7$F$$$k(J */
#define	NEKO_SLEEP		4	/* $@?2$F$7$^$C$?(J */
#define	NEKO_AWAKE		5	/* $@L\$,3P$a$?(J */
#define	NEKO_U_MOVE		6	/* $@>e$K0\F0Cf(J */
#define	NEKO_D_MOVE		7	/* $@2<$K0\F0Cf(J */
#define	NEKO_L_MOVE		8	/* $@:8$K0\F0Cf(J */
#define	NEKO_R_MOVE		9	/* $@1&$K0\F0Cf(J */
#define	NEKO_UL_MOVE		10	/* $@:8>e$K0\F0Cf(J */
#define	NEKO_UR_MOVE		11	/* $@1&>e$K0\F0Cf(J */
#define	NEKO_DL_MOVE		12	/* $@:82<$K0\F0Cf(J */
#define	NEKO_DR_MOVE		13	/* $@1&2<$K0\F0Cf(J */
#define	NEKO_U_TOGI		14	/* $@>e$NJI$r0z$CA_$$$F$$$k(J */
#define	NEKO_D_TOGI		15	/* $@2<$NJI$r0z$CA_$$$F$$$k(J */
#define	NEKO_L_TOGI		16	/* $@:8$NJI$r0z$CA_$$$F$$$k(J */
#define	NEKO_R_TOGI		17	/* $@1&$NJI$r0z$CA_$$$F$$$k(J */

/*
 *	$@G-$N%"%K%a!<%7%g%s7+$jJV$72s?t(J
 */

#define	NEKO_STOP_TIME		4
#define	NEKO_JARE_TIME		10
#define	NEKO_KAKI_TIME		4
#define	NEKO_AKUBI_TIME		6	/* $@$b$H$b$H#3$@$C$?$1$ID9$$J}$,$$$$(J */
#define	NEKO_AWAKE_TIME		3
#define	NEKO_TOGI_TIME		10

#define	PI_PER8			((double)3.1415926535/(double)8)

#define	MAXDISPLAYNAME		(64 + 5)	/* $@%G%#%9%W%l%$L>$ND9$5(J */

#define	NOTDEFINED		(-1)

/*
 *	$@%^%/%mDj5A(J
 */

#define IsTrue(str)     (strcmp((str), "true") == 0)
