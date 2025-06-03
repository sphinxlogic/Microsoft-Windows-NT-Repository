/*--------------------------------------------------------------
 *
 *	xneko  -  X11 ǭ
 *
 *			Original Writer: Masayuki Koba
 *			Programmed by Masayuki Koba, 1990
 *
 *--------------------------------------------------------------
 *
 *��Introduction:
 *
 *�����ܥץ����� Macintosh �Υǥ������������꡼ "neko" ��
 *��ư��� X11 �ǥޥͤ���ΤǤ���
 *
 *����Macintosh "neko" �ν���ʥǥ�����˷ɰդ�ɽ���Ĥġ�����
 *���ץ����򳧤���������ޤ���
 *
 *--------------------------------------------------------------
 *
 *��Special Thanks to
 *
 *	toshi-w	�ġ�Macintosh neko �ξҲ��
 *	shio-m	�ġ���X11 �� neko ���ߤ������פȥ����򤳤ͤ���
 *	disco	�ġ�X11 �ƥ��˥��롦���ɥХ�����
 *
 *	HOMY	�ġ��Х���Ŧ��
 *
 *		"xneko"  Presented by Masayuki Koba (masa-k).
 *
 *--------------------------------------------------------------
 *
 *��Manifest:
 *
 *�����ܥץ����� Public Domain Software �Ǥ���ž�ܡ����ɤ�
 *����ͳ�˹ԤäƲ�������
 *
 *�����ʤ�������Ԥϡ��ܥץ�������Ѥ��뤳�Ȥˤ�ä�������
 *���㳲�������פˤĤ��Ƥ��ä�����Ǥ������ޤ���
 *
 *--------------------------------------------------------------
 *
 *��Bugs:
 *
 * if using -root option, the cursor remains as a mouse throughout
 * 
 *�����ʣ���X11 �� .Xdefaults �������ޤ�ä���̵�뤷�Ƥ��ޤ���
 *
 *�����ʣ���ǭ��ư����֤�Ʊ�����Ƥ��뤿�ᡢ�ޥ����ΰ�ư����
 *����������ݡ���󥰤��Ƥ��ޤ������äơ��ޥ���������ư���
 *�����������ʤ�����̵�̤ʥޥ�����ɸ�ɤ߼���ԤäƤ��ޤ��ޤ���
 *
 *�����ʣ��˥�����ɥ����������󲽤���Ƥ⡢�����פ�����褷
 *���������ĤŤ��ޤ���������ʬ�ϡ����ߤΥ�����ɥ��ξ��֤����
 *���������å����ơ��������󲽤���Ƥ�����ϴ����˥��٥���Ԥ�
 *���������ˤʤ�褦�˽��Ѥ��ʤ���Фʤ�ޤ��� (����ʤ��ȡ�
 *���������Ǥ���Τ��ʤ���X10 �ǤϤǤ��ޤ�������)
 *
 *�����ʣ��˥ꥵ������Υ�����ɥ�����ü�˾������ʤä�����ư��
 *�����������ݾڤǤ��ޤ���
 *
 *�����ʣ�������ʤ�г��ݤ���������ɥ��� Pixmap �ϥץ����
 *����������λ���˲�������ɬ�פ�����ޤ������ܥץ����Ϥ���
 *���������ؤ�򥵥ܤäƤ��ꡢ���ˤ��Ե��������ʤäƤ��ޤ���
 *�����������̤� exit() ���˥����ƥब;ʬ�ʥ꥽�������������
 *������������ޤ������ϣӤ˥Х���������� xneko ���٤ⵯ
 *��������ư����ȡ����Τ�������å��ΰ褬��­���Ƥ��ޤ����Ȥ�
 *���������ʤ뤫�⤷��ޤ���
 *
 *�����ʣ��˻��֤�Ʊ������ɬ�����������¹Ԥ��뤿�ᡢ�������
 *���������Ǥ� Idle 90 �� 95% �Ȥʤꡢ�����ƥ�ãУդ� 5 �� 10%
 *�������������񤷤ޤ�����xtachos ��Ĵ�٤ޤ�������
 *
 *--------------------------------------------------------------
 *
 *��System (Machine):
 *
 *�����ܥץ�����ư����ǧ���������ƥ๽���ϰʲ����̤ꡣ
 *
 *	��NWS-1750 (NEWS)��NWP-512D
 *	��NEWS-OS 3.2a (UNIX 4.3BSD)��X11 Release 2
 *
 *	��Sun 3��X11 Release 4
 *
 *--------------------------------------------------------------*/
#ifdef VMS

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <signal.h>

#include "DECW$INCLUDE:Xlib.h"
#include "DECW$INCLUDE:Xutil.h"
#include "DECW$INCLUDE:Xatom.h"
#include "DECW$INCLUDE:keysym.h"

#else

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <math.h>
#include <sys/time.h>
#endif

#ifndef	lint
static char
	rcsid[] = "$Header: xneko.c,v 1.12 90/03/15 11:38:21 masa-k Locked $";
static char	WriterMessage[] = "xneko: Programmed by Masayuki Koba, 1990";
#endif


/*
 *	X11 ǭ �ӥåȥޥåץե����������
 *
 *		"icon.xbm"		�ġ���������
 *		"cursor.xbm"		�ġ���������
 *		"cursor_mask.xbm"	�ġ���������ʥޥ�����
 *
 *		"space.xbm"		�ġ����ڡ���
 *
 *		"mati2.xbm"		�ġ��Ԥ���
 *		"jare2.xbm"		�ġ�����죲
 *		"kaki1.xbm"		�ġ��ߤ���
 *		"kaki2.xbm"		�ġ��ߤ���
 *		"mati3.xbm"		�ġ��Ԥ����ʤ����ӡ�
 *		"sleep1.xbm"		�ġ����룱
 *		"sleep2.xbm"		�ġ����룲
 *
 *		"awake.xbm"		�ġ��ܳФ�
 *
 *		"up1.xbm"		�ġ��壱
 *		"up2.xbm"		�ġ��売
 *		"down1.xbm"		�ġ�����
 *		"down2.xbm"		�ġ�����
 *		"left1.xbm"		�ġ�����
 *		"left2.xbm"		�ġ�����
 *		"right1.xbm"		�ġ�����
 *		"right2.xbm"		�ġ�����
 *		"upleft1.xbm"		�ġ����壱
 *		"upleft2.xbm"		�ġ����売
 *		"upright1.xbm"		�ġ����壱
 *		"upright2.xbm"		�ġ����売
 *		"dwleft1.xbm"		�ġ�������
 *		"dwleft2.xbm"		�ġ�������
 *		"dwright1.xbm"		�ġ�������
 *		"dwright2.xbm"		�ġ�������
 *
 *		"utogi1.xbm"		�ġ����᤮��
 *		"utogi2.xbm"		�ġ����᤮��
 *		"dtogi1.xbm"		�ġ����᤮��
 *		"dtogi2.xbm"		�ġ����᤮��
 *		"ltogi1.xbm"		�ġ����᤮��
 *		"ltogi2.xbm"		�ġ����᤮��
 *		"rtogi1.xbm"		�ġ����᤮��
 *		"rtogi2.xbm"		�ġ����᤮��
 *
 *	�������Υե������ bitmap ���ޥ�ɤ��Խ���ǽ�Ǥ���
 *
 *		(bitmap size �� 32x32 ... Macintosh ICON resource size.)
 *
 */
#ifdef VMS

#include "SYS$DISK:[.BITMAPS]icon.xbm"
#include "SYS$DISK:[.BITMAPS]cursor.xbm"
#include "SYS$DISK:[.BITMAPS]cursor_mask.xbm"

#include "SYS$DISK:[.BITMAPS]space.xbm"

#include "SYS$DISK:[.BITMAPS]mati2.xbm"
#include "SYS$DISK:[.BITMAPS]jare2.xbm"
#include "SYS$DISK:[.BITMAPS]kaki1.xbm"
#include "SYS$DISK:[.BITMAPS]kaki2.xbm"
#include "SYS$DISK:[.BITMAPS]mati3.xbm"
#include "SYS$DISK:[.BITMAPS]sleep1.xbm"
#include "SYS$DISK:[.BITMAPS]sleep2.xbm"

#include "SYS$DISK:[.BITMAPS]awake.xbm"

#include "SYS$DISK:[.BITMAPS]up1.xbm"
#include "SYS$DISK:[.BITMAPS]up2.xbm"
#include "SYS$DISK:[.BITMAPS]down1.xbm"
#include "SYS$DISK:[.BITMAPS]down2.xbm"
#include "SYS$DISK:[.BITMAPS]left1.xbm"
#include "SYS$DISK:[.BITMAPS]left2.xbm"
#include "SYS$DISK:[.BITMAPS]right1.xbm"
#include "SYS$DISK:[.BITMAPS]right2.xbm"
#include "SYS$DISK:[.BITMAPS]upright1.xbm"
#include "SYS$DISK:[.BITMAPS]upright2.xbm"
#include "SYS$DISK:[.BITMAPS]upleft1.xbm"
#include "SYS$DISK:[.BITMAPS]upleft2.xbm"
#include "SYS$DISK:[.BITMAPS]dwleft1.xbm"
#include "SYS$DISK:[.BITMAPS]dwleft2.xbm"
#include "SYS$DISK:[.BITMAPS]dwright1.xbm"
#include "SYS$DISK:[.BITMAPS]dwright2.xbm"

#include "SYS$DISK:[.BITMAPS]utogi1.xbm"
#include "SYS$DISK:[.BITMAPS]utogi2.xbm"
#include "SYS$DISK:[.BITMAPS]dtogi1.xbm"
#include "SYS$DISK:[.BITMAPS]dtogi2.xbm"
#include "SYS$DISK:[.BITMAPS]ltogi1.xbm"
#include "SYS$DISK:[.BITMAPS]ltogi2.xbm"
#include "SYS$DISK:[.BITMAPS]rtogi1.xbm"
#include "SYS$DISK:[.BITMAPS]rtogi2.xbm"

#include "SYS$DISK:[.BITMAPS]mati2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]jare2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]kaki1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]kaki2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]mati3_mask.xbm"
#include "SYS$DISK:[.BITMAPS]sleep1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]sleep2_mask.xbm"

#include "SYS$DISK:[.BITMAPS]awake_mask.xbm"

#include "SYS$DISK:[.BITMAPS]up1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]up2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]down1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]down2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]left1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]left2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]right1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]right2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]upright1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]upright2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]upleft1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]upleft2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]dwleft1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]dwleft2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]dwright1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]dwright2_mask.xbm"

#include "SYS$DISK:[.BITMAPS]utogi1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]utogi2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]dtogi1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]dtogi2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]ltogi1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]ltogi2_mask.xbm"
#include "SYS$DISK:[.BITMAPS]rtogi1_mask.xbm"
#include "SYS$DISK:[.BITMAPS]rtogi2_mask.xbm"

#else

#include "bitmaps/icon.xbm"
#include "bitmaps/cursor.xbm"
#include "bitmaps/cursor_mask.xbm"

#include "bitmaps/space.xbm"

#include "bitmaps/mati2.xbm"
#include "bitmaps/jare2.xbm"
#include "bitmaps/kaki1.xbm"
#include "bitmaps/kaki2.xbm"
#include "bitmaps/mati3.xbm"
#include "bitmaps/sleep1.xbm"
#include "bitmaps/sleep2.xbm"

#include "bitmaps/awake.xbm"

#include "bitmaps/up1.xbm"
#include "bitmaps/up2.xbm"
#include "bitmaps/down1.xbm"
#include "bitmaps/down2.xbm"
#include "bitmaps/left1.xbm"
#include "bitmaps/left2.xbm"
#include "bitmaps/right1.xbm"
#include "bitmaps/right2.xbm"
#include "bitmaps/upright1.xbm"
#include "bitmaps/upright2.xbm"
#include "bitmaps/upleft1.xbm"
#include "bitmaps/upleft2.xbm"
#include "bitmaps/dwleft1.xbm"
#include "bitmaps/dwleft2.xbm"
#include "bitmaps/dwright1.xbm"
#include "bitmaps/dwright2.xbm"

#include "bitmaps/utogi1.xbm"
#include "bitmaps/utogi2.xbm"
#include "bitmaps/dtogi1.xbm"
#include "bitmaps/dtogi2.xbm"
#include "bitmaps/ltogi1.xbm"
#include "bitmaps/ltogi2.xbm"
#include "bitmaps/rtogi1.xbm"
#include "bitmaps/rtogi2.xbm"

#include "bitmaps/mati2_mask.xbm"
#include "bitmaps/jare2_mask.xbm"
#include "bitmaps/kaki1_mask.xbm"
#include "bitmaps/kaki2_mask.xbm"
#include "bitmaps/mati3_mask.xbm"
#include "bitmaps/sleep1_mask.xbm"
#include "bitmaps/sleep2_mask.xbm"

#include "bitmaps/awake_mask.xbm"

#include "bitmaps/up1_mask.xbm"
#include "bitmaps/up2_mask.xbm"
#include "bitmaps/down1_mask.xbm"
#include "bitmaps/down2_mask.xbm"
#include "bitmaps/left1_mask.xbm"
#include "bitmaps/left2_mask.xbm"
#include "bitmaps/right1_mask.xbm"
#include "bitmaps/right2_mask.xbm"
#include "bitmaps/upright1_mask.xbm"
#include "bitmaps/upright2_mask.xbm"
#include "bitmaps/upleft1_mask.xbm"
#include "bitmaps/upleft2_mask.xbm"
#include "bitmaps/dwleft1_mask.xbm"
#include "bitmaps/dwleft2_mask.xbm"
#include "bitmaps/dwright1_mask.xbm"
#include "bitmaps/dwright2_mask.xbm"

#include "bitmaps/utogi1_mask.xbm"
#include "bitmaps/utogi2_mask.xbm"
#include "bitmaps/dtogi1_mask.xbm"
#include "bitmaps/dtogi2_mask.xbm"
#include "bitmaps/ltogi1_mask.xbm"
#include "bitmaps/ltogi2_mask.xbm"
#include "bitmaps/rtogi1_mask.xbm"
#include "bitmaps/rtogi2_mask.xbm"

#endif

/*
 *	������
 */

#define	BITMAP_WIDTH		32	/* ������饯������ (�ԥ�����) */
#define	BITMAP_HEIGHT		32	/* ������饯���ι⤵ (�ԥ�����) */

#define	WINDOW_WIDTH		320	/* ������ɥ����� (�ԥ�����) */
#define	WINDOW_HEIGHT		256	/* ������ɥ��ι⤵ (�ԥ�����) */

#define	DEFAULT_BORDER		2	/* �ܡ����������� */

#define	DEFAULT_WIN_X		1	/* ������ɥ������غ�ɸ */
#define	DEFAULT_WIN_Y		1	/* ������ɥ������ٺ�ɸ */

#define	AVAIL_KEYBUF		255

#define	EVENT_MASK1		( KeyPressMask | StructureNotifyMask )

#define	EVENT_MASK2		( KeyPressMask | \
				  ExposureMask | \
				  StructureNotifyMask )

#define	MAX_TICK		9999		/* Odd Only! */

#ifdef VMS
#define	INTERVAL		1000000		/* ���󥿡��Х륿���� */
#else
#define	INTERVAL		125000L		/* ���󥿡��Х륿���� */
#endif

#define	NEKO_SPEED		16

#define	NORMAL_STATE		1
#define	DEBUG_LIST		2
#define	DEBUG_MOVE		3

/* ǭ�ξ������ */

#define	NEKO_STOP		0	/* Ω���ߤޤä� */
#define	NEKO_JARE		1	/* ������äƤ��� */
#define	NEKO_KAKI		2	/* Ƭ���ߤ��Ƥ��� */
#define	NEKO_AKUBI		3	/* �����Ӥ򤷤Ƥ��� */
#define	NEKO_SLEEP		4	/* ���Ƥ��ޤä� */
#define	NEKO_AWAKE		5	/* �ܤ��Ф᤿ */
#define	NEKO_U_MOVE		6	/* ��˰�ư�� */
#define	NEKO_D_MOVE		7	/* ���˰�ư�� */
#define	NEKO_L_MOVE		8	/* ���˰�ư�� */
#define	NEKO_R_MOVE		9	/* ���˰�ư�� */
#define	NEKO_UL_MOVE		10	/* ����˰�ư�� */
#define	NEKO_UR_MOVE		11	/* ����˰�ư�� */
#define	NEKO_DL_MOVE		12	/* �����˰�ư�� */
#define	NEKO_DR_MOVE		13	/* �����˰�ư�� */
#define	NEKO_U_TOGI		14	/* ����ɤ�����ߤ��Ƥ��� */
#define	NEKO_D_TOGI		15	/* �����ɤ�����ߤ��Ƥ��� */
#define	NEKO_L_TOGI		16	/* �����ɤ�����ߤ��Ƥ��� */
#define	NEKO_R_TOGI		17	/* �����ɤ�����ߤ��Ƥ��� */

/* ǭ�Υ��˥᡼����󷫤��֤���� */

#define	NEKO_STOP_TIME		4
#define	NEKO_JARE_TIME		10
#define	NEKO_KAKI_TIME		4
#define	NEKO_AKUBI_TIME		3
#define	NEKO_AWAKE_TIME		3
#define	NEKO_TOGI_TIME		10

#define	PI_PER8			((double)3.1415926535/(double)8)

#define	DIRNAMELEN		255


/*
 *	�����Х��ѿ�
 */
#ifdef VMS
static long neko$q_interval[2];
static long neko$l_timer_efn;
#endif

static char		*ProgramName;		/* ���ޥ��̾�� */

Display			*theDisplay;
int			theScreen;
int			theDepth;
unsigned long		theBlackPixel;
unsigned long		theWhitePixel;
Window			theWindow;
Window			sWindow;
Cursor			theCursor;

static int		WindowWidth;
static int		WindowHeight;

static int		WindowPointX;
static int		WindowPointY;

static unsigned long	BorderWidth = DEFAULT_BORDER;

long			IntervalTime = INTERVAL;

int			EventState;		/* ���٥�Ƚ����� �����ѿ� */

int			NekoTickCount;		/* ǭư����� */
int			NekoStateCount;		/* ǭƱ����֥����� */
int			NekoState;		/* ǭ�ξ��� */

int			MouseX;			/* �ޥ����غ�ɸ */
int			MouseY;			/* �ޥ����ٺ�ɸ */

int			PrevMouseX = 0;		/* ľ���Υޥ����غ�ɸ */
int			PrevMouseY = 0;		/* ľ���Υޥ����ٺ�ɸ */

int			NekoX;			/* ǭ�غ�ɸ */
int			NekoY;			/* ǭ�ٺ�ɸ */

int			NekoMoveDx;		/* ǭ��ư��Υ�� */
int			NekoMoveDy;		/* ǭ��ư��Υ�� */

int			NekoLastX;		/* ǭ�ǽ�����غ�ɸ */
int			NekoLastY;		/* ǭ�ǽ�����ٺ�ɸ */
GC			NekoLastGC;		/* ǭ�ǽ����� GC */

double			NekoSpeed = (double)NEKO_SPEED;

double			SinPiPer8Times3;	/* sin( ���С��� ) */
double			SinPiPer8;		/* sin( �С��� ) */

Pixmap			SpaceXbm;

Pixmap			Mati2Xbm;
Pixmap			Jare2Xbm;
Pixmap			Kaki1Xbm;
Pixmap			Kaki2Xbm;
Pixmap			Mati3Xbm;
Pixmap			Sleep1Xbm;
Pixmap			Sleep2Xbm;

Pixmap			AwakeXbm;

Pixmap			Up1Xbm;
Pixmap			Up2Xbm;
Pixmap			Down1Xbm;
Pixmap			Down2Xbm;
Pixmap			Left1Xbm;
Pixmap			Left2Xbm;
Pixmap			Right1Xbm;
Pixmap			Right2Xbm;
Pixmap			UpLeft1Xbm;
Pixmap			UpLeft2Xbm;
Pixmap			UpRight1Xbm;
Pixmap			UpRight2Xbm;
Pixmap			DownLeft1Xbm;
Pixmap			DownLeft2Xbm;
Pixmap			DownRight1Xbm;
Pixmap			DownRight2Xbm;

Pixmap			UpTogi1Xbm;
Pixmap			UpTogi2Xbm;
Pixmap			DownTogi1Xbm;
Pixmap			DownTogi2Xbm;
Pixmap			LeftTogi1Xbm;
Pixmap			LeftTogi2Xbm;
Pixmap			RightTogi1Xbm;
Pixmap			RightTogi2Xbm;

Pixmap			Mati2MaskXbm;
Pixmap			Jare2MaskXbm;
Pixmap			Kaki1MaskXbm;
Pixmap			Kaki2MaskXbm;
Pixmap			Mati3MaskXbm;
Pixmap			Sleep1MaskXbm;
Pixmap			Sleep2MaskXbm;

Pixmap			AwakeMaskXbm;

Pixmap			Up1MaskXbm;
Pixmap			Up2MaskXbm;
Pixmap			Down1MaskXbm;
Pixmap			Down2MaskXbm;
Pixmap			Left1MaskXbm;
Pixmap			Left2MaskXbm;
Pixmap			Right1MaskXbm;
Pixmap			Right2MaskXbm;
Pixmap			UpLeft1MaskXbm;
Pixmap			UpLeft2MaskXbm;
Pixmap			UpRight1MaskXbm;
Pixmap			UpRight2MaskXbm;
Pixmap			DownLeft1MaskXbm;
Pixmap			DownLeft2MaskXbm;
Pixmap			DownRight1MaskXbm;
Pixmap			DownRight2MaskXbm;

Pixmap			UpTogi1MaskXbm;
Pixmap			UpTogi2MaskXbm;
Pixmap			DownTogi1MaskXbm;
Pixmap			DownTogi2MaskXbm;
Pixmap			LeftTogi1MaskXbm;
Pixmap			LeftTogi2MaskXbm;
Pixmap			RightTogi1MaskXbm;
Pixmap			RightTogi2MaskXbm;

GC			SpaceGC;

GC			Mati2GC;
GC			Jare2GC;
GC			Kaki1GC;
GC			Kaki2GC;
GC			Mati3GC;
GC			Sleep1GC;
GC			Sleep2GC;

GC			AwakeGC;

GC			Up1GC;
GC			Up2GC;
GC			Down1GC;
GC			Down2GC;
GC			Left1GC;
GC			Left2GC;
GC			Right1GC;
GC			Right2GC;
GC			UpLeft1GC;
GC			UpLeft2GC;
GC			UpRight1GC;
GC			UpRight2GC;
GC			DownLeft1GC;
GC			DownLeft2GC;
GC			DownRight1GC;
GC			DownRight2GC;

GC			UpTogi1GC;
GC			UpTogi2GC;
GC			DownTogi1GC;
GC			DownTogi2GC;
GC			LeftTogi1GC;
GC			LeftTogi2GC;
GC			RightTogi1GC;
GC			RightTogi2GC;

typedef struct {
    GC			*GCCreatePtr;
    Pixmap		*BitmapCreatePtr;
    Pixmap		*MaskCreatePtr;
    char		*PixelPattern;
    char		*MaskPattern;
    unsigned int	PixelWidth;
    unsigned int	PixelHeight;
} BitmapGCData;

BitmapGCData	BitmapGCDataTable[] =
{
    { &SpaceGC, &SpaceXbm, NULL, space_bits, NULL, space_width, space_height },
    { &Mati2GC, &Mati2Xbm, &Mati2MaskXbm, mati2_bits, mati2_mask_bits, mati2_width, mati2_height },
    { &Jare2GC, &Jare2Xbm, &Jare2MaskXbm, jare2_bits, jare2_mask_bits, jare2_width, jare2_height },
    { &Kaki1GC, &Kaki1Xbm, &Kaki1MaskXbm, kaki1_bits, kaki1_mask_bits, kaki1_width, kaki1_height },
    { &Kaki2GC, &Kaki2Xbm, &Kaki2MaskXbm, kaki2_bits, kaki2_mask_bits, kaki2_width, kaki2_height },
    { &Mati3GC, &Mati3Xbm, &Mati3MaskXbm, mati3_bits, mati3_mask_bits, mati3_width, mati3_height },
    { &Sleep1GC, &Sleep1Xbm, &Sleep1MaskXbm, sleep1_bits, sleep1_mask_bits, sleep1_width, sleep1_height },
    { &Sleep2GC, &Sleep2Xbm, &Sleep2MaskXbm, sleep2_bits, sleep2_mask_bits, sleep2_width, sleep2_height },
    { &AwakeGC, &AwakeXbm, &AwakeMaskXbm, awake_bits, awake_mask_bits, awake_width, awake_height },
    { &Up1GC, &Up1Xbm, &Up1MaskXbm, up1_bits, up1_mask_bits, up1_width, up1_height },
    { &Up2GC, &Up2Xbm, &Up2MaskXbm, up2_bits, up2_mask_bits, up2_width, up2_height },
    { &Down1GC, &Down1Xbm, &Down1MaskXbm, down1_bits, down1_mask_bits, down1_width, down1_height },
    { &Down2GC, &Down2Xbm, &Down2MaskXbm, down2_bits, down2_mask_bits, down2_width, down2_height },
    { &Left1GC, &Left1Xbm, &Left1MaskXbm, left1_bits, left1_mask_bits, left1_width, left1_height },
    { &Left2GC, &Left2Xbm, &Left2MaskXbm, left2_bits, left2_mask_bits, left2_width, left2_height },
    { &Right1GC, &Right1Xbm, &Right1MaskXbm, right1_bits, right1_mask_bits, right1_width, right1_height },
    { &Right2GC, &Right2Xbm, &Right2MaskXbm, right2_bits, right2_mask_bits, right2_width, right2_height },
    { &UpLeft1GC, &UpLeft1Xbm, &UpLeft1MaskXbm, upleft1_bits, upleft1_mask_bits, upleft1_width, upleft1_height },
    { &UpLeft2GC, &UpLeft2Xbm, &UpLeft2MaskXbm, upleft2_bits, upleft2_mask_bits, upleft2_width, upleft2_height },
    { &UpRight1GC, &UpRight1Xbm, &UpRight1MaskXbm, upright1_bits, upright1_mask_bits, upright1_width, upright1_height },
    { &UpRight2GC, &UpRight2Xbm, &UpRight2MaskXbm, upright2_bits, upright2_mask_bits, upright2_width, upright2_height },
    { &DownLeft1GC, &DownLeft1Xbm, &DownLeft1MaskXbm, dwleft1_bits, dwleft1_mask_bits, dwleft1_width, dwleft1_height },
    { &DownLeft2GC, &DownLeft2Xbm, &DownLeft2MaskXbm, dwleft2_bits, dwleft2_mask_bits, dwleft2_width, dwleft2_height },
    { &DownRight1GC, &DownRight1Xbm, &DownRight1MaskXbm, dwright1_bits, dwright1_mask_bits, dwright1_width, dwright1_height },
    { &DownRight2GC, &DownRight2Xbm, &DownRight2MaskXbm, dwright2_bits, dwright2_mask_bits, dwright2_width, dwright2_height },
    { &UpTogi1GC, &UpTogi1Xbm, &UpTogi1MaskXbm, utogi1_bits, utogi1_mask_bits, utogi1_width, utogi1_height },
    { &UpTogi2GC, &UpTogi2Xbm, &UpTogi2MaskXbm, utogi2_bits, utogi2_mask_bits, utogi2_width, utogi2_height },
    { &DownTogi1GC, &DownTogi1Xbm, &DownTogi1MaskXbm, dtogi1_bits, dtogi1_mask_bits, dtogi1_width, dtogi1_height },
    { &DownTogi2GC, &DownTogi2Xbm, &DownTogi2MaskXbm, dtogi2_bits, dtogi2_mask_bits, dtogi2_width, dtogi2_height },
    { &LeftTogi1GC, &LeftTogi1Xbm, &LeftTogi1MaskXbm, ltogi1_bits, ltogi1_mask_bits, ltogi1_width, ltogi1_height },
    { &LeftTogi2GC, &LeftTogi2Xbm, &LeftTogi2MaskXbm, ltogi2_bits, ltogi2_mask_bits, ltogi2_width, ltogi2_height },
    { &RightTogi1GC, &RightTogi1Xbm, &RightTogi1MaskXbm, rtogi1_bits, rtogi1_mask_bits, rtogi1_width, rtogi1_height },
    { &RightTogi2GC, &RightTogi2Xbm, &RightTogi2MaskXbm, rtogi2_bits, rtogi2_mask_bits, rtogi2_width, rtogi2_height },
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

typedef struct {
    GC		*TickEvenGCPtr;
    GC		*TickOddGCPtr;
} Animation;

Animation	AnimationPattern[] =
{
    { &Mati2GC, &Mati2GC },		/* NekoState == NEKO_STOP */
    { &Jare2GC, &Mati2GC },		/* NekoState == NEKO_JARE */
    { &Kaki1GC, &Kaki2GC },		/* NekoState == NEKO_KAKI */
    { &Mati3GC, &Mati3GC },		/* NekoState == NEKO_AKUBI */
    { &Sleep1GC, &Sleep2GC },		/* NekoState == NEKO_SLEEP */
    { &AwakeGC, &AwakeGC },		/* NekoState == NEKO_AWAKE */
    { &Up1GC, &Up2GC }	,		/* NekoState == NEKO_U_MOVE */
    { &Down1GC, &Down2GC },		/* NekoState == NEKO_D_MOVE */
    { &Left1GC, &Left2GC },		/* NekoState == NEKO_L_MOVE */
    { &Right1GC, &Right2GC },		/* NekoState == NEKO_R_MOVE */
    { &UpLeft1GC, &UpLeft2GC },		/* NekoState == NEKO_UL_MOVE */
    { &UpRight1GC, &UpRight2GC },	/* NekoState == NEKO_UR_MOVE */
    { &DownLeft1GC, &DownLeft2GC },	/* NekoState == NEKO_DL_MOVE */
    { &DownRight1GC, &DownRight2GC },	/* NekoState == NEKO_DR_MOVE */
    { &UpTogi1GC, &UpTogi2GC },		/* NekoState == NEKO_U_TOGI */
    { &DownTogi1GC, &DownTogi2GC },	/* NekoState == NEKO_D_TOGI */
    { &LeftTogi1GC, &LeftTogi2GC },	/* NekoState == NEKO_L_TOGI */
    { &RightTogi1GC, &RightTogi2GC },	/* NekoState == NEKO_R_TOGI */
};

/*
 * JH, define some variables 
 */
Window WmRootWindow();          /* routine to find root window id       */
Window root;
int cat_asleep_timer = 0;
Bool use_pause_window = False;
Bool use_root_window = False;
Bool auto_move_mouse = False;
# define CAT_ASLEEP_TIME 25

unsigned long		theWindowMask;
XSetWindowAttributes	theWindowAttributes;
XWMHints		theWMHints;
Pixmap			theCursorMask;

extern Window SmPauseWindow();

int InPausedState(dpy, root)
    Display *dpy;
    Window root;
{
    XWindowAttributes attr;
    int new_state;
    int a, b, c, d, e;

    if (sWindow == 0)
        {
        sWindow = SmPauseWindow(dpy, root);
        if (sWindow == 0)
	    return 0;
	}

    if (!XGetWindowAttributes (dpy, sWindow, &attr))
	{
	fprintf (stderr, "XGetWindowAttributes on session manager pause window failed.\n");
	exit(1);
	}

    new_state = (attr.map_state == IsViewable);
    return new_state;
}



/*--------------------------------------------------------------
 *
 *	�ӥåȥޥåץǡ�����GC �����
 *
 *--------------------------------------------------------------*/

void
InitBitmapAndGCs()
{
    BitmapGCData	*BitmapGCDataTablePtr;
    XGCValues		theGCValues;

    theGCValues.function = GXcopy;
    theGCValues.foreground = BlackPixel( theDisplay, theScreen );
    theGCValues.background = WhitePixel( theDisplay, theScreen );
    theGCValues.fill_style = FillTiled;

    for ( BitmapGCDataTablePtr = BitmapGCDataTable;
	  BitmapGCDataTablePtr->GCCreatePtr != NULL;
	  BitmapGCDataTablePtr++ ) {

	*(BitmapGCDataTablePtr->BitmapCreatePtr)
	    = XCreatePixmapFromBitmapData( theDisplay,
				     RootWindow( theDisplay, theScreen ),
				     BitmapGCDataTablePtr->PixelPattern,
				     BitmapGCDataTablePtr->PixelWidth,
				     BitmapGCDataTablePtr->PixelHeight,
				     BlackPixel(theDisplay, theScreen),
				     WhitePixel(theDisplay, theScreen),
				     DefaultDepth(theDisplay, theScreen));

	theGCValues.tile = *(BitmapGCDataTablePtr->BitmapCreatePtr);

	if (BitmapGCDataTablePtr->MaskCreatePtr != NULL)
	    {
	    *(BitmapGCDataTablePtr->MaskCreatePtr)
		= XCreateBitmapFromData( theDisplay,
					 RootWindow( theDisplay, theScreen ),
					 BitmapGCDataTablePtr->MaskPattern,
					 BitmapGCDataTablePtr->PixelWidth,
					 BitmapGCDataTablePtr->PixelHeight);
    
	    theGCValues.clip_mask = *(BitmapGCDataTablePtr->MaskCreatePtr);
    
	    *(BitmapGCDataTablePtr->GCCreatePtr)
		= XCreateGC( theDisplay, theWindow,
			     GCFunction | GCForeground | GCBackground |
			     GCTile | GCFillStyle | GCClipMask,
			     &theGCValues );
	    }
	else
	    *(BitmapGCDataTablePtr->GCCreatePtr)
		= XCreateGC( theDisplay, theWindow,
			     GCFunction | GCForeground | GCBackground |
			     GCTile | GCFillStyle,
			     &theGCValues );
    }

    XFlush(theDisplay);
}


/*--------------------------------------------------------------
 *
 *	�����꡼��Ķ������
 *
 *--------------------------------------------------------------*/

void
InitScreen( DisplayName, theGeometry, TitleName, iconicState )
    char	*DisplayName;
    char	*theGeometry;
    char	*TitleName;
    Bool	iconicState;
{
    int				GeometryStatus;
    Bool			OverrideRedirectStatus;
    XWindowAttributes		wa;
    XSizeHints			theSizeHints;
    Pixmap			theIconPixmap;
    Pixmap			theCursorSource;
    Window			theRoot;
    Colormap			theColormap;
    XColor			theWhiteColor, theBlackColor, theExactColor;

    if ( ( theDisplay = XOpenDisplay( DisplayName ) ) == NULL ) {
	fprintf( stderr, "%s: Can't open display", ProgramName );
	if ( DisplayName != NULL ) {
	    fprintf( stderr, " %s.\n", DisplayName );
	} else {
	    fprintf( stderr, ".\n" );
	}
	exit( 1 );
    }

    theScreen = DefaultScreen( theDisplay );
    theDepth = DefaultDepth( theDisplay, theScreen );

    theBlackPixel = BlackPixel( theDisplay, theScreen );
    theWhitePixel = WhitePixel( theDisplay, theScreen );

    GeometryStatus = XParseGeometry( theGeometry,
				     &WindowPointX, &WindowPointY,
				     &WindowWidth, &WindowHeight );

    if ( !( GeometryStatus & XValue ) ) {
	WindowPointX = DEFAULT_WIN_X;
    }
    if ( !( GeometryStatus & YValue ) ) {
	WindowPointY = DEFAULT_WIN_Y;
    }
    if ( !( GeometryStatus & WidthValue ) ) {
	WindowWidth = WINDOW_WIDTH;
    }
    if ( !( GeometryStatus & HeightValue ) ) {
	WindowHeight = WINDOW_HEIGHT;
    }

    if ( ( GeometryStatus & XValue )
	 && ( GeometryStatus & YValue )
	 && ( GeometryStatus & WidthValue )
	 && ( GeometryStatus & HeightValue ) ) {
	OverrideRedirectStatus = True;
    } else {
	OverrideRedirectStatus = False;
    }

    theCursorSource
	= XCreateBitmapFromData( theDisplay,
				 RootWindow( theDisplay, theScreen ),
				 cursor_bits,
				 cursor_width,
				 cursor_height );

    theCursorMask
	= XCreateBitmapFromData( theDisplay,
				 RootWindow( theDisplay, theScreen ),
				 cursor_mask_bits,
				 cursor_mask_width,
				 cursor_mask_height );

    theColormap = DefaultColormap( theDisplay, theScreen );

    if ( !XAllocNamedColor( theDisplay, theColormap,
			    "white", &theWhiteColor, &theExactColor ) ) {
	fprintf( stderr,
		 "%s: Can't XAllocNamedColor( \"white\" ).\n", ProgramName );
	exit( 1 );
    }

    if ( !XAllocNamedColor( theDisplay, theColormap,
			    "black", &theBlackColor, &theExactColor ) ) {
	fprintf( stderr,
		 "%s: Can't XAllocNamedColor( \"black\" ).\n", ProgramName );
	exit( 1 );
    }

    theCursor = XCreatePixmapCursor( theDisplay,
				     theCursorSource, theCursorMask,
				     &theBlackColor, &theWhiteColor,
				     cursor_x_hot, cursor_y_hot );

    theWindowAttributes.cursor = theCursor;
    theWindowAttributes.background_pixel = theWhitePixel;

    if (use_pause_window || use_root_window)
    {
	theWindowMask = CWCursor;
    }
    else
    {
	theWindowAttributes.border_pixel = theBlackPixel;
	theWindowAttributes.override_redirect = OverrideRedirectStatus;
	theWindowMask = CWBackPixel		|
		    CWBorderPixel	|
		    CWCursor		|
		    CWOverrideRedirect;
    }

    if (use_root_window)
    {
	theWindow = WmRootWindow(theDisplay, DefaultRootWindow(theDisplay));
        theWindow = WmRootWindow(theDisplay, theWindow);

	XChangeWindowAttributes(theDisplay, theWindow, theWindowMask,
	    &theWindowAttributes);
/*	XSetWindowBackground(theDisplay, theWindow, 1);
	XClearWindow(theDisplay, theWindow);
*/
    }
    else
    {
	if (use_pause_window)
	{
	    while( (theWindow = SmPauseWindow(theDisplay,
				    DefaultRootWindow(theDisplay))) == 0)
		sleep(5);
/*	    sWindow = SmPauseWindow(theDisplay, DefaultRootWindow(theDisplay));
	    while(sWindow == 0)
	    {
	    sleep(5);
	    sWindow = SmPauseWindow(theDisplay, DefaultRootWindow(theDisplay));
	    }
	    theWindow = sWindow;
*/
	    XChangeWindowAttributes(theDisplay, theWindow, theWindowMask,
		&theWindowAttributes);
/*	    XSetWindowBackground(theDisplay, theWindow, 1);
	    XClearWindow(theDisplay, theWindow);
*/
	}
	else
	{
	    theWindow = XCreateWindow( theDisplay,
			       RootWindow( theDisplay, theScreen ),
			       WindowPointX, WindowPointY,
			       WindowWidth, WindowHeight,
			       BorderWidth,
			       theDepth,
			       InputOutput,
			       CopyFromParent,
			       theWindowMask,
			       &theWindowAttributes );
	}
    }

    XGetWindowAttributes(theDisplay, theWindow, &wa);
    if (! use_pause_window && ! use_root_window)
    {
	theIconPixmap = XCreatePixmapFromBitmapData( theDisplay, theWindow,
				       icon_bits,
				       icon_width,
				       icon_height,
				       BlackPixel(theDisplay,theScreen),
				       WhitePixel(theDisplay,theScreen),
				       wa.depth);
	theWMHints.icon_pixmap = theIconPixmap;
	if ( iconicState ) {
	    theWMHints.initial_state = IconicState;
	} else {
	    theWMHints.initial_state = NormalState;
	}
	theWMHints.flags = IconPixmapHint | StateHint;

	XSetWMHints( theDisplay, theWindow, &theWMHints );

	theSizeHints.flags = PPosition | PSize;
	theSizeHints.x = WindowPointX;
	theSizeHints.y = WindowPointY;
	theSizeHints.width = WindowWidth;
	theSizeHints.height = WindowHeight;

	XSetNormalHints( theDisplay, theWindow, &theSizeHints );

	if ( strlen( TitleName ) >= 1 ) {
	    XStoreName( theDisplay, theWindow, TitleName );
	    XSetIconName( theDisplay, theWindow, TitleName );
	} else {
	    XStoreName( theDisplay, theWindow, ProgramName );
	    XSetIconName( theDisplay, theWindow, ProgramName );
	}

	XMapWindow( theDisplay, theWindow );
    }

    XFlush( theDisplay );

    XGetGeometry( theDisplay, theWindow,
		  &theRoot,
		  &WindowPointX, &WindowPointY,
		  &WindowWidth, &WindowHeight,
		  &BorderWidth, &theDepth );

    InitBitmapAndGCs();

    XSelectInput( theDisplay, theWindow, EVENT_MASK1 );

    XFlush( theDisplay );
}


/*--------------------------------------------------------------
 *
 *	���󥿡��Х�
 *
 *	�����δؿ���Ƥ֤ȡ��������λ����֤äƤ��ʤ��ʤ롣ǭ
 *	��ư����ߥ�Ĵ�������Ѥ��뤳�ȡ�
 *
 *--------------------------------------------------------------*/

void
Interval()
{
if (use_pause_window)
      {
      int new_pause = 0;

/*    while( !InPausedState(theDisplay, RootWindow(theDisplay, theScreen)) ) */
      while( (theWindow = SmPauseWindow(theDisplay,
			    DefaultRootWindow(theDisplay))) == 0)
	{
	new_pause = 1;
	sleep(5);
	}

      /* If there is a new pause window, we must set the cursor to a mouse
         again. */

      if (new_pause == 1)
	XChangeWindowAttributes(theDisplay, theWindow, theWindowMask,
	    &theWindowAttributes);
      }
#ifdef VMS
	sys$waitfr(neko$l_timer_efn);
	sys$setimr(neko$l_timer_efn,neko$q_interval,0,0);
#else
    pause();
#endif
}


/*--------------------------------------------------------------
 *
 *	�ƥ��å�������Ƚ���
 *
 *--------------------------------------------------------------*/

void
TickCount()
{
    if ( ++NekoTickCount >= MAX_TICK ) {
	NekoTickCount = 0;
    }

    if ( NekoTickCount % 2 == 0 ) {
	if ( NekoStateCount < MAX_TICK ) {
	    NekoStateCount++;
	}
    }
}


/*--------------------------------------------------------------
 *
 *	ǭ��������
 *
 *--------------------------------------------------------------*/

void
SetNekoState( SetValue )
    int		SetValue;
{
    NekoTickCount = 0;
    NekoStateCount = 0;

    NekoState = SetValue;

#ifdef	DEBUG
    switch ( NekoState ) {
    case NEKO_STOP:
    case NEKO_JARE:
    case NEKO_KAKI:
    case NEKO_AKUBI:
    case NEKO_SLEEP:
    case NEKO_U_TOGI:
    case NEKO_D_TOGI:
    case NEKO_L_TOGI:
    case NEKO_R_TOGI:
	NekoMoveDx = NekoMoveDy = 0;
	break;
    default:
	break;
    }
#endif
}


/*--------------------------------------------------------------
 *
 *	ǭ�������
 *
 *--------------------------------------------------------------*/

void
DrawNeko( x, y, DrawGC )
    int		x;
    int		y;
    GC		DrawGC;
{
    if ( EventState != DEBUG_LIST ) 
	XClearArea(theDisplay, theWindow, NekoLastX, NekoLastY,
	    BITMAP_WIDTH, BITMAP_HEIGHT, 0);

    XSetTSOrigin( theDisplay, DrawGC, x, y );
    XSetClipOrigin( theDisplay, DrawGC, x, y);

    XFillRectangle( theDisplay, theWindow, DrawGC,
		    x, y, BITMAP_WIDTH, BITMAP_HEIGHT );
    XFlush( theDisplay );

    NekoLastX = x;
    NekoLastY = y;

    NekoLastGC = DrawGC;
}


/*--------------------------------------------------------------
 *
 *	ǭ���������
 *
 *--------------------------------------------------------------*/

void
RedrawNeko()
{
    XFillRectangle( theDisplay, theWindow, NekoLastGC,
		    NekoLastX, NekoLastY,
		    BITMAP_WIDTH, BITMAP_HEIGHT );

    XFlush( theDisplay );
}


/*--------------------------------------------------------------
 *
 *	ǭ��ư��ˡ����
 *
 *--------------------------------------------------------------*/

void
NekoDirection()
{
    int			NewState;
    double		LargeX, LargeY;
    double		Length;
    double		SinTheta;

    if ( NekoMoveDx == 0 && NekoMoveDy == 0 ) {
	NewState = NEKO_STOP;
    } else {
	LargeX = (double)NekoMoveDx;
	LargeY = (double)(-NekoMoveDy);
	Length = sqrt( LargeX * LargeX + LargeY * LargeY );
	SinTheta = LargeY / Length;

	if ( NekoMoveDx > 0 ) {
	    if ( SinTheta > SinPiPer8Times3 ) {
		NewState = NEKO_U_MOVE;
	    } else if ( ( SinTheta <= SinPiPer8Times3 )
			&& ( SinTheta > SinPiPer8 ) ) {
		NewState = NEKO_UR_MOVE;
	    } else if ( ( SinTheta <= SinPiPer8 )
			&& ( SinTheta > -( SinPiPer8 ) ) ) {
		NewState = NEKO_R_MOVE;
	    } else if ( ( SinTheta <= -( SinPiPer8 ) )
			&& ( SinTheta > -( SinPiPer8Times3 ) ) ) {
		NewState = NEKO_DR_MOVE;
	    } else {
		NewState = NEKO_D_MOVE;
	    }
	} else {
	    if ( SinTheta > SinPiPer8Times3 ) {
		NewState = NEKO_U_MOVE;
	    } else if ( ( SinTheta <= SinPiPer8Times3 )
			&& ( SinTheta > SinPiPer8 ) ) {
		NewState = NEKO_UL_MOVE;
	    } else if ( ( SinTheta <= SinPiPer8 )
			&& ( SinTheta > -( SinPiPer8 ) ) ) {
		NewState = NEKO_L_MOVE;
	    } else if ( ( SinTheta <= -( SinPiPer8 ) )
			&& ( SinTheta > -( SinPiPer8Times3 ) ) ) {
		NewState = NEKO_DL_MOVE;
	    } else {
		NewState = NEKO_D_MOVE;
	    }
	}
    }

    if ( NekoState != NewState ) {
	SetNekoState( NewState );
    }
}


/*--------------------------------------------------------------
 *
 *	ǭ�ɤ֤Ĥ���Ƚ��
 *
 *--------------------------------------------------------------*/

Bool
IsWindowOver()
{
    Bool	ReturnValue = False;

    if ( NekoY <= 0 ) {
	NekoY = 0;
	ReturnValue = True;
    } else if ( NekoY >= WindowHeight - BITMAP_HEIGHT ) {
	NekoY = WindowHeight - BITMAP_HEIGHT;
	ReturnValue = True;
    }
    if ( NekoX <= 0 ) {
	NekoX = 0;
	ReturnValue = True;
    } else if ( NekoX >= WindowWidth - BITMAP_WIDTH ) {
	NekoX = WindowWidth - BITMAP_WIDTH;
	ReturnValue = True;
    }

    return( ReturnValue );
}


/*--------------------------------------------------------------
 *
 *	ǭ��ư����Ƚ��
 *
 *--------------------------------------------------------------*/

Bool
IsNekoDontMove()
{
    if ( NekoX == NekoLastX && NekoY == NekoLastY ) {
	return( True );
    } else {
	return( False );
    }
}


/*--------------------------------------------------------------
 *
 *	ǭ��ư����Ƚ��
 *
 *--------------------------------------------------------------*/

Bool
IsNekoMoveStart()
{
#ifndef	DEBUG
    if ( PrevMouseX != MouseX || PrevMouseY != MouseY ) {
	return( True );
    } else {
	return( False );
    }
#else
    if ( NekoMoveDx == 0 && NekoMoveDy == 0 ) {
	return( False );
    } else {
	return( True );
    }
#endif
}


/*--------------------------------------------------------------
 *
 *	ǭ��ư dx, dy �׻�
 *
 *--------------------------------------------------------------*/

void
CalcDxDy()
{
    Window		QueryRoot, QueryChild;
    int			AbsoluteX, AbsoluteY;
    int			RelativeX, RelativeY;
    unsigned int	ModKeyMask;
    double		LargeX, LargeY;
    double		DoubleLength, Length;

    XQueryPointer( theDisplay, theWindow,
		   &QueryRoot, &QueryChild,
		   &AbsoluteX, &AbsoluteY,
		   &RelativeX, &RelativeY,
		   &ModKeyMask );

    PrevMouseX = MouseX;
    PrevMouseY = MouseY;

    MouseX = RelativeX;
    MouseY = RelativeY;

    LargeX = (double)( MouseX - NekoX - BITMAP_WIDTH / 2 );
    LargeY = (double)( MouseY - NekoY - BITMAP_HEIGHT );

    DoubleLength = LargeX * LargeX + LargeY * LargeY;

    if ( DoubleLength != (double)0 ) {
	Length = sqrt( DoubleLength );
	if ( Length <= NekoSpeed ) {
	    NekoMoveDx = (int)LargeX;
	    NekoMoveDy = (int)LargeY;
	} else {
	    NekoMoveDx = (int)( ( NekoSpeed * LargeX ) / Length );
	    NekoMoveDy = (int)( ( NekoSpeed * LargeY ) / Length );
	}
    } else {
	NekoMoveDx = NekoMoveDy = 0;
    }
}


/*--------------------------------------------------------------
 *
 *	ư�����ǭ�������
 *
 *--------------------------------------------------------------*/

void
NekoThinkDraw()
{
#ifndef	DEBUG
    CalcDxDy();
#endif

    if ( NekoState != NEKO_SLEEP ) {
	DrawNeko( NekoX, NekoY,
		  NekoTickCount % 2 == 0 ?
		  *(AnimationPattern[ NekoState ].TickEvenGCPtr) :
		  *(AnimationPattern[ NekoState ].TickOddGCPtr) );
    } else {
	DrawNeko( NekoX, NekoY,
		  NekoTickCount % 8 <= 3 ?
		  *(AnimationPattern[ NekoState ].TickEvenGCPtr) :
		  *(AnimationPattern[ NekoState ].TickOddGCPtr) );
    }

    TickCount();

    switch ( NekoState ) {
    case NEKO_STOP:
	if ( IsNekoMoveStart() ) {
	    SetNekoState( NEKO_AWAKE );
	    break;
	}
	if ( NekoStateCount < NEKO_STOP_TIME ) {
	    break;
	}
	if ( NekoMoveDx < 0 && NekoX <= 0 ) {
	    SetNekoState( NEKO_L_TOGI );
	} else if ( NekoMoveDx > 0 && NekoX >= WindowWidth - BITMAP_WIDTH ) {
	    SetNekoState( NEKO_R_TOGI );
	} else if ( NekoMoveDy < 0 && NekoY <= 0 ) {
	    SetNekoState( NEKO_U_TOGI );
	} else if ( NekoMoveDy > 0 && NekoY >= WindowHeight - BITMAP_HEIGHT ) {
	    SetNekoState( NEKO_D_TOGI );
	} else {
	    SetNekoState( NEKO_JARE );
	}
	break;
    case NEKO_JARE:
	if ( IsNekoMoveStart() ) {
	    SetNekoState( NEKO_AWAKE );
	    break;
	}
	if ( NekoStateCount < NEKO_JARE_TIME ) {
	    break;
	}
	SetNekoState( NEKO_KAKI );
	break;
    case NEKO_KAKI:
	if ( IsNekoMoveStart() ) {
	    SetNekoState( NEKO_AWAKE );
	    break;
	}
	if ( NekoStateCount < NEKO_KAKI_TIME ) {
	    break;
	}
	SetNekoState( NEKO_AKUBI );
	break;
    case NEKO_AKUBI:
	if ( IsNekoMoveStart() ) {
	    SetNekoState( NEKO_AWAKE );
	    break;
	}
	if ( NekoStateCount < NEKO_AKUBI_TIME ) {
	    break;
	}
	SetNekoState( NEKO_SLEEP );
	break;
    case NEKO_SLEEP:
	if ( IsNekoMoveStart() ) {
	    SetNekoState( NEKO_AWAKE );
	    break;
	}
	else
	{
	    if (auto_move_mouse)
	    {
		/* count to CAT_ASLEEP_TIME then move pointer */

		cat_asleep_timer++;
		if(cat_asleep_timer == CAT_ASLEEP_TIME)
		{
		    movepointer ();
		    cat_asleep_timer = 0;
		}
	    }
	}
	break;
    case NEKO_AWAKE:
	if ( NekoStateCount < NEKO_AWAKE_TIME ) {
	    break;
	}
	NekoDirection();	/* ǭ��ư����������� */
	break;
    case NEKO_U_MOVE:
    case NEKO_D_MOVE:
    case NEKO_L_MOVE:
    case NEKO_R_MOVE:
    case NEKO_UL_MOVE:
    case NEKO_UR_MOVE:
    case NEKO_DL_MOVE:
    case NEKO_DR_MOVE:
	NekoX += NekoMoveDx;
	NekoY += NekoMoveDy;
	NekoDirection();
	if ( IsWindowOver() ) {
	    if ( IsNekoDontMove() ) {
		SetNekoState( NEKO_STOP );
	    }
	}
	break;
    case NEKO_U_TOGI:
    case NEKO_D_TOGI:
    case NEKO_L_TOGI:
    case NEKO_R_TOGI:
	if ( IsNekoMoveStart() ) {
	    SetNekoState( NEKO_AWAKE );
	    break;
	}
	if ( NekoStateCount < NEKO_TOGI_TIME ) {
	    break;
	}
	SetNekoState( NEKO_KAKI );
	break;
    default:
	/* Internal Error */
	SetNekoState( NEKO_STOP );
	break;
    }

    Interval();
}


#ifdef	DEBUG

/*--------------------------------------------------------------
 *
 *	����饯��������ɽ���ʥǥХå��ѡ�
 *
 *--------------------------------------------------------------*/

void
DisplayCharacters()
{
    int		Index;
    int		x, y;

    for ( Index = 0, x = 0, y = 0;
	  BitmapGCDataTable[ Index ].GCCreatePtr != NULL; Index++ ) {

	DrawNeko( x, y, *(BitmapGCDataTable[ Index ].GCCreatePtr) );
	XFlush( theDisplay );

	x += BITMAP_WIDTH;

	if ( x > WindowWidth - BITMAP_WIDTH ) {
	    x = 0;
	    y += BITMAP_HEIGHT;
	    if ( y > WindowHeight - BITMAP_HEIGHT) {
		break;
	    }
	}
    }
}

#endif	/* DEBUG */


/*--------------------------------------------------------------
 *
 *	�������٥�Ƚ���
 *
 *--------------------------------------------------------------*/

Bool
ProcessKeyPress( theKeyEvent )
    XKeyEvent	*theKeyEvent;
{
    int			Length;
    int			theKeyBufferMaxLen = AVAIL_KEYBUF;
    char		theKeyBuffer[ AVAIL_KEYBUF + 1 ];
    KeySym		theKeySym;
    XComposeStatus	theComposeStatus;
    Bool		ReturnState;

    ReturnState = True;

    Length = XLookupString( theKeyEvent,
			    theKeyBuffer, theKeyBufferMaxLen,
			    &theKeySym, &theComposeStatus );

    if ( Length > 0 ) {
	switch ( theKeyBuffer[ 0 ] ) {
	case 'q':
	case 'Q':
	    if ( theKeyEvent->state & Mod1Mask ) {	/* META (Alt) ���� */
		ReturnState = False;
	    }
	    break;
	default:
	    break;
	}
    }

#ifdef	DEBUG
    if ( EventState == DEBUG_MOVE ) {
	switch ( theKeySym ) {
	case XK_KP_1:
	    NekoMoveDx = -(int)( NekoSpeed / sqrt( (double)2 ) );
	    NekoMoveDy = -NekoMoveDx;
	    break;
	case XK_KP_2:
	    NekoMoveDx = 0;
	    NekoMoveDy = (int)NekoSpeed;
	    break;
	case XK_KP_3:
	    NekoMoveDx = (int)( NekoSpeed / sqrt( (double)2 ) );
	    NekoMoveDy = NekoMoveDx;
	    break;
	case XK_KP_4:
	    NekoMoveDx = -(int)NekoSpeed;
	    NekoMoveDy = 0;
	    break;
	case XK_KP_5:
	    NekoMoveDx = 0;
	    NekoMoveDy = 0;
	    break;
	case XK_KP_6:
	    NekoMoveDx = (int)NekoSpeed;
	    NekoMoveDy = 0;
	    break;
	case XK_KP_7:
	    NekoMoveDx = -(int)( NekoSpeed / sqrt( (double)2 ) );
	    NekoMoveDy = NekoMoveDx;
	    break;
	case XK_KP_8:
	    NekoMoveDx = 0;
	    NekoMoveDy = -(int)NekoSpeed;
	    break;
	case XK_KP_9:
	    NekoMoveDx = (int)( NekoSpeed / sqrt( (double)2 ) );
	    NekoMoveDy = -NekoMoveDx;
	    break;
	}
    }
#endif

    return( ReturnState );
}


/*--------------------------------------------------------------
 *
 *	ǭ����Ĵ��
 *
 *--------------------------------------------------------------*/

void
NekoAdjust()
{
    if ( NekoX < 0 ) {
	NekoX = 0;
    } else if ( NekoX > WindowWidth - BITMAP_WIDTH ) {
	NekoX = WindowWidth - BITMAP_WIDTH;
    }

    if ( NekoY < 0 ) {
	NekoY = 0;
    } else if ( NekoY > WindowHeight - BITMAP_HEIGHT ) {
	NekoY = WindowHeight - BITMAP_HEIGHT;
    }
}


/*--------------------------------------------------------------
 *
 *	���٥�Ƚ���
 *
 *--------------------------------------------------------------*/

Bool
ProcessEvent()
{
    XEvent	theEvent;
    Bool	ContinueState = True;

    switch ( EventState ) {
    case NORMAL_STATE:
	while ( XCheckMaskEvent( theDisplay, EVENT_MASK1, &theEvent ) ) {
	    switch ( theEvent.type ) {
	    case ConfigureNotify:
		WindowWidth = theEvent.xconfigure.width;
		WindowHeight = theEvent.xconfigure.height;
		WindowPointX = theEvent.xconfigure.x;
		WindowPointY = theEvent.xconfigure.y;
		BorderWidth = theEvent.xconfigure.border_width;
		NekoAdjust();
		break;
	    case Expose:
		if ( theEvent.xexpose.count == 0 ) {
		    RedrawNeko();
		}
		break;
	    case MapNotify:
		RedrawNeko();
		break;
	    case KeyPress:
		ContinueState = ProcessKeyPress( &theEvent );
		if ( !ContinueState ) {
		    return( ContinueState );
		}
		break;
	    default:
		/* Unknown Event */
		break;
	    }
	}
	break;
#ifdef	DEBUG
    case DEBUG_LIST:
	XNextEvent( theDisplay, &theEvent );
	switch ( theEvent.type ) {
	case ConfigureNotify:
	    WindowWidth = theEvent.xconfigure.width;
	    WindowHeight = theEvent.xconfigure.height;
	    WindowPointX = theEvent.xconfigure.x;
	    WindowPointY = theEvent.xconfigure.y;
	    BorderWidth = theEvent.xconfigure.border_width;
	    break;
	case Expose:
	    if ( theEvent.xexpose.count == 0 ) {
		DisplayCharacters();
	    }
	    break;
	case MapNotify:
	    DisplayCharacters();
	    break;
	case KeyPress:
	    ContinueState = ProcessKeyPress( &theEvent );
	    break;
	default:
	    /* Unknown Event */
	    break;
	}
	break;
    case DEBUG_MOVE:
	while ( XCheckMaskEvent( theDisplay, EVENT_MASK1, &theEvent ) ) {
	    switch ( theEvent.type ) {
	    case ConfigureNotify:
		WindowWidth = theEvent.xconfigure.width;
		WindowHeight = theEvent.xconfigure.height;
		WindowPointX = theEvent.xconfigure.x;
		WindowPointY = theEvent.xconfigure.y;
		BorderWidth = theEvent.xconfigure.border_width;
		NekoAdjust();
		break;
	    case Expose:
		if ( theEvent.xexpose.count == 0 ) {
		    RedrawNeko();
		}
		break;
	    case MapNotify:
		RedrawNeko();
		break;
	    case KeyPress:
		ContinueState = ProcessKeyPress( &theEvent );
		if ( !ContinueState ) {
		    return( ContinueState );
		}
		break;
	    default:
		/* Unknown Event */
		break;
	    }
	}
	break;
#endif
    default:
	/* Internal Error */
	break;
    }

    return( ContinueState );
}


/*--------------------------------------------------------------
 *
 *	ǭ����
 *
 *--------------------------------------------------------------*/

void
ProcessNeko()
{
#ifndef VMS
    struct itimerval	Value;
#endif
    /* �Ķ��ν���� */

    EventState = NORMAL_STATE;

    /* ǭ�ν���� */

    NekoX = ( WindowWidth - BITMAP_WIDTH / 2 ) / 2;
    NekoY = ( WindowHeight - BITMAP_HEIGHT / 2 ) / 2;

    NekoLastX = NekoX;
    NekoLastY = NekoY;

    SetNekoState( NEKO_STOP );

    /* �����ޡ����� */
#ifdef VMS
    lib$get_ef(&neko$l_timer_efn);
    neko$q_interval[0] = -IntervalTime;
    neko$q_interval[1] = -1;
    sys$setimr(neko$l_timer_efn,neko$q_interval,0,0);
#else
    timerclear( &Value.it_interval );
    timerclear( &Value.it_value );

    Value.it_interval.tv_usec = IntervalTime;
    Value.it_value.tv_usec = IntervalTime;

    setitimer( ITIMER_REAL, &Value, 0 );
#endif
    /* �ᥤ����� */

    do {
	NekoThinkDraw();
    } while ( ProcessEvent() );
}


#ifdef	DEBUG

/*--------------------------------------------------------------
 *
 *	ǭ�����ʥǥХå��ѡ�
 *
 *--------------------------------------------------------------*/

void
NekoList()
{
    EventState = DEBUG_LIST;

    fprintf( stderr, "\n" );
    fprintf( stderr, "ǭ������ɽ�����ޤ���(Quit �� Alt-Q)\n" );
    fprintf( stderr, "\n" );

    XSelectInput( theDisplay, theWindow, EVENT_MASK2 );

    while ( ProcessEvent() );
}


/*--------------------------------------------------------------
 *
 *	ǭ��ư�ƥ��ȡʥǥХå��ѡ�
 *
 *--------------------------------------------------------------*/

void
NekoMoveTest()
{
#ifndef VMS
    struct itimerval	Value;
#endif
    /* �Ķ��ν���� */

    EventState = DEBUG_MOVE;

    /* ǭ�ν���� */

    NekoX = ( WindowWidth - BITMAP_WIDTH / 2 ) / 2;
    NekoY = ( WindowHeight - BITMAP_HEIGHT / 2 ) / 2;

    NekoLastX = NekoX;
    NekoLastY = NekoY;

    SetNekoState( NEKO_STOP );

    /* �����ޡ����� */
#ifdef VMS
    lib$get_ef(&neko$l_timer_efn);
    neko$q_interval[0] = -IntervalTime;
    neko$q_interval[1] = -1;
    sys$setimr(neko$l_timer_efn,neko$q_interval,0,0);
#else
    timerclear( &Value.it_interval );
    timerclear( &Value.it_value );

    Value.it_interval.tv_usec = IntervalTime;
    Value.it_value.tv_usec = IntervalTime;

    setitimer( ITIMER_REAL, &Value, 0 );
#endif
    /* �ᥤ����� */

    fprintf( stderr, "\n" );
    fprintf( stderr, "ǭ�ΰ�ư�ƥ��Ȥ�Ԥ��ޤ���(Quit �� Alt-Q)\n" );
    fprintf( stderr, "\n" );
    fprintf( stderr, "\t�����ѥåɾ�Υƥ󥭡���ǭ���ư�����Ʋ�������\n" );
    fprintf( stderr, "\t(ͭ���ʥ����ϣ������Ǥ���)\n" );
    fprintf( stderr, "\n" );

    do {
	NekoThinkDraw();
    } while ( ProcessEvent() );
}


/*--------------------------------------------------------------
 *
 *	��˥塼�����ʥǥХå��ѡ�
 *
 *--------------------------------------------------------------*/

void
ProcessDebugMenu()
{
    int		UserSelectNo = 0;
    char	UserAnswer[ BUFSIZ ];

    fprintf( stderr, "\n" );
    fprintf( stderr, "��xneko �ǥХå���˥塼��\n" );

    while ( !( UserSelectNo >= 1 && UserSelectNo <= 2 ) ) {
	fprintf( stderr, "\n" );
	fprintf( stderr, "\t1)��ǭ����饯��������ɽ��\n" );
	fprintf( stderr, "\t2)��ǭ��ư�ƥ���\n" );
	fprintf( stderr, "\n" );
	fprintf( stderr, "Select: " );

	fgets( UserAnswer, sizeof( UserAnswer ), stdin );

	UserSelectNo = atoi( UserAnswer );

	if ( !( UserSelectNo >= 1 && UserSelectNo <= 2 ) ) {
	    fprintf( stderr, "\n" );
	    fprintf( stderr, "�������ֹ�����򤷤Ʋ�������\n" );
	}
    }

    switch ( UserSelectNo ) {
    case 1:
	/* ǭ����饯��������ɽ�� */
	NekoList();
	break;
    case 2:
	/* ǭ��ư�ƥ��� */
	NekoMoveTest();
	break;
    default:
	/* Internal Error */
	break;
    }

    fprintf( stderr, "�ƥ��Ƚ�λ��\n" );
    fprintf( stderr, "\n" );
}

#endif	/* DEBUG */


/*--------------------------------------------------------------
 *
 *	SIGALRM �����ʥ����
 *
 *--------------------------------------------------------------*/

void
NullFunction()
{
    /* No Operation */
}


/*--------------------------------------------------------------
 *
 *	Usage
 *
 *--------------------------------------------------------------*/

void
Usage()
{
    /*
     * JH, added [-root] [-demo] [-pause] as option
     */

    fprintf( stderr, "\n");
    fprintf( stderr,"Usage: %s \n",ProgramName );
    fprintf( stderr, "\n");    
    fprintf( stderr,
            "       [-display <display>] [-geometry <geometry>] [-help]\n" );
    fprintf( stderr, 
	    "       [-title <title>] [-name <title>] [-iconic] [-pause]\n" );
    fprintf( stderr, 
            "       [-speed <speed>] [-time <time>] [-root] [-demo]\n" );
    fprintf( stderr, "\n");
}


/*--------------------------------------------------------------
 *
 *	�إѥ�᡼��ɾ��
 *
 *--------------------------------------------------------------*/

Bool
GetArguments( argc, argv, theDisplayName, theGeometry, theTitle,
	      NekoSpeed, IntervalTime )
    int		argc;
    char	*argv[];
    char	*theDisplayName;
    char	*theGeometry;
    char	*theTitle;
    double	*NekoSpeed;
    long	*IntervalTime;
{
    int		ArgCounter;
    Bool	iconicState;

    theDisplayName[ 0 ] = '\0';
    theGeometry[ 0 ] = '\0';
    theTitle[ 0 ] = '\0';

    iconicState = False;

    /*
     * JH, hack adds demo and root as options
     */

    for ( ArgCounter = 0; ArgCounter < argc; ArgCounter++ ) {

	if ( strncmp( argv[ ArgCounter ], "-h", 2 ) == 0 ) {
	    Usage();
	    exit( 0 );
	} else if ( strcmp( argv[ ArgCounter ], "-display" ) == 0 ) {
	    ArgCounter++;
	    if ( ArgCounter < argc ) {
		strcpy( theDisplayName, argv[ ArgCounter ] );
	    } else {
		fprintf( stderr, "%s: -display option error.\n", ProgramName );
		exit( 1 );
	    }
	} else if ( strncmp( argv[ ArgCounter ], "-geom", 5 ) == 0 ) {
	    ArgCounter++;
	    if ( ArgCounter < argc ) {
		strcpy( theGeometry, argv[ ArgCounter ] );
	    } else {
		fprintf( stderr,
			 "%s: -geometry option error.\n", ProgramName );
		exit( 1 );
	    }
	} else if ( ( strcmp( argv[ ArgCounter ], "-title" ) == 0 )
	     || ( strcmp( argv[ ArgCounter ], "-name" ) == 0 ) ) {
	    ArgCounter++;
	    if ( ArgCounter < argc ) {
		strcpy( theTitle, argv[ ArgCounter ] );
	    } else {
		fprintf( stderr, "%s: -title option error.\n", ProgramName );
		exit( 1 );
	    }
	} else if ( strcmp( argv[ ArgCounter ], "-iconic" ) == 0 ) {
	    iconicState = True;
	} else if ( strcmp( argv[ ArgCounter ], "-speed" ) == 0 ) {
	    ArgCounter++;
	    if ( ArgCounter < argc ) {
		*NekoSpeed = atof( argv[ ArgCounter ] );
	    } else {
		fprintf( stderr, "%s: -speed option error.\n", ProgramName );
		exit( 1 );
	    }
	} else if ( strcmp( argv[ ArgCounter ], "-time" ) == 0 ) {
	    ArgCounter++;
	    if ( ArgCounter < argc ) {
		*IntervalTime = atol( argv[ ArgCounter ] );
	    } else {
		fprintf( stderr, "%s: -time option error.\n", ProgramName );
		exit( 1 );
	    }
	} else if ( strcmp( argv[ ArgCounter ], "-root" ) == 0 ) {
	    use_root_window = True;
	} else if ( strcmp( argv[ ArgCounter ], "-demo" ) == 0 ) {
	    auto_move_mouse = True;
	} else if ( strcmp( argv[ ArgCounter ], "-pause" ) == 0 ) {
            use_pause_window = True;
	} else {
	    fprintf( stderr,
		     "%s: Unknown option \"%s\".\n", ProgramName,
						     argv[ ArgCounter ] );
	    Usage();
	    exit( 1 );
	}
    }

    if ( strlen( theDisplayName ) < 1 ) {
	theDisplayName = NULL;
    }

    if ( strlen( theGeometry ) < 1 ) {
	theGeometry = NULL;
    }

    return( iconicState );
}


/*--------------------------------------------------------------
 *
 *	�ᥤ��ؿ�
 *
 *--------------------------------------------------------------*/

int
main( argc, argv )
    int		argc;
    char	*argv[];
{
    Bool	iconicState;
    char	theDisplayName[ DIRNAMELEN ];
    char	theGeometry[ DIRNAMELEN ];
    char	theTitle[ DIRNAMELEN ];

    ProgramName = argv[ 0 ];

    argc--;
    argv++;

    iconicState = GetArguments( argc, argv,
				theDisplayName,
				theGeometry,
				theTitle,
				&NekoSpeed,
				&IntervalTime );

    if (theTitle[0] == 0) strcpy(theTitle, "XNeko");

    InitScreen( theDisplayName, theGeometry, theTitle, iconicState );
#ifndef VMS
    signal( SIGALRM, NullFunction );
#endif
    SinPiPer8Times3 = sin( PI_PER8 * (double)3 );
    SinPiPer8 = sin( PI_PER8 );

#ifndef	DEBUG
    ProcessNeko();
#else
    ProcessDebugMenu();
#endif

    exit( 0 );
}

/*
 * JH, function moves the cursor to some random value
 */
int movepointer ()
{
	int ptr_x, ptr_y;

	root = DefaultRootWindow(theDisplay);
	root = WmRootWindow(theDisplay, root);

	get_rand(&ptr_x);
	get_rand(&ptr_y);

	XWarpPointer(theDisplay, 0, root, 0, 0, 0, 0,
					ptr_x, ptr_y);
	XSync(theDisplay, False);
        return;
}

/*
 * JH, calculate a value between 0 - 1024
 */
int get_rand(
	int *value)
{
    short num;
    int val;

    val = rand();
    num = (short) rand ();
    num = num / 32;
    if (num <0) num = 1024+num;
    *value = (int) num;
    return;    
}

/*
 * JH, beats me..., i pinched it from xcrab, xaquarium
 */
Window WmRootWindow (theDisplay, root)
    Display *theDisplay;
    Window root;
{
    Window parent;
    Window *child;
    unsigned int nchildren;
    XWindowAttributes rootatt, childatt;
    if (!XGetWindowAttributes (theDisplay, root, &rootatt)) {
        fprintf (stderr, "XGetWindowAttributes on root failed.\n");
        exit(1);
    }

    if (XQueryTree (theDisplay, root, &root, &parent, &child, &nchildren)) {
        int i;
        for (i = 0; i < nchildren; i++) {
            if (!XGetWindowAttributes (theDisplay, child[i], &childatt)) {
                fprintf (stderr, "XGetWindowAttributes on child failed.\n");
                exit(1);
            }
            if ((rootatt.width == childatt.width) &&
                (rootatt.height == childatt.height))
                return child[i];
        }
        return root;
    } else {
        fprintf (stderr, "XQueryTree failed (window doesn't exist).\n");
        exit(1);
    }
}

