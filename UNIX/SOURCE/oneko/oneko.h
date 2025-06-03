/*
 *	oneko  -  X11 猫
 *
 *	$Header: /home/sun/unix/kato/xsam/oneko/oneko.h,v 1.4 90/10/18 16:52:57 kato Exp Locker: kato $
 */

/*
 *	インクルードファイル
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#include <stdio.h>

#include <signal.h>
#include <math.h>
#include <sys/time.h>

/*
 *	カーソルビットマップファイルの読み込み
 */

#include "bitmaps/cursor.xbm"
#include "bitmaps/cursor_mask.xbm"

/*
 *	普通の猫用ビットマップファイルの読み込み
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
 *	とら猫用ビットマップファイルの読み込み
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
 *	ビットマスクファイルの読み込み
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
 *	定数定義
 */

#define	BITMAP_WIDTH		32	/* １キャラクタの幅 (ピクセル) */
#define	BITMAP_HEIGHT		32	/* １キャラクタの高さ (ピクセル) */

#define	AVAIL_KEYBUF		255
#define	EVENT_MASK		(KeyPressMask | ExposureMask)
#define	MAX_TICK		9999		/* Odd Only! */

#define	DEFAULT_FOREGROUND	"black"		/* フォアグラウンドカラー */
#define	DEFAULT_BACKGROUND	"white"		/* バックグラウンドカラー */
#define	DEFAULT_INTERVAL	125000L		/* インターバルタイム */
#define	DEFAULT_NEKO_SPEED	16		/* 猫の速度 */
#define	DEFAULT_IDLE_SPACE	6		/* 猫の感度 */

#define	NORMAL_NEKO		0		/* 普通の猫 */
#define	TORA_NEKO		1		/* とら猫 */

/*
 *	猫の状態定数
 */

#define	NEKO_STOP		0	/* 立ち止まった */
#define	NEKO_JARE		1	/* 顔を洗っている */
#define	NEKO_KAKI		2	/* 頭を掻いている */
#define	NEKO_AKUBI		3	/* あくびをしている */
#define	NEKO_SLEEP		4	/* 寝てしまった */
#define	NEKO_AWAKE		5	/* 目が覚めた */
#define	NEKO_U_MOVE		6	/* 上に移動中 */
#define	NEKO_D_MOVE		7	/* 下に移動中 */
#define	NEKO_L_MOVE		8	/* 左に移動中 */
#define	NEKO_R_MOVE		9	/* 右に移動中 */
#define	NEKO_UL_MOVE		10	/* 左上に移動中 */
#define	NEKO_UR_MOVE		11	/* 右上に移動中 */
#define	NEKO_DL_MOVE		12	/* 左下に移動中 */
#define	NEKO_DR_MOVE		13	/* 右下に移動中 */
#define	NEKO_U_TOGI		14	/* 上の壁を引っ掻いている */
#define	NEKO_D_TOGI		15	/* 下の壁を引っ掻いている */
#define	NEKO_L_TOGI		16	/* 左の壁を引っ掻いている */
#define	NEKO_R_TOGI		17	/* 右の壁を引っ掻いている */

/*
 *	猫のアニメーション繰り返し回数
 */

#define	NEKO_STOP_TIME		4
#define	NEKO_JARE_TIME		10
#define	NEKO_KAKI_TIME		4
#define	NEKO_AKUBI_TIME		6	/* もともと３だったけど長い方がいい */
#define	NEKO_AWAKE_TIME		3
#define	NEKO_TOGI_TIME		10

#define	PI_PER8			((double)3.1415926535/(double)8)

#define	MAXDISPLAYNAME		(64 + 5)	/* ディスプレイ名の長さ */

#define	NOTDEFINED		(-1)

/*
 *	マクロ定義
 */

#define IsTrue(str)     (strcmp((str), "true") == 0)
