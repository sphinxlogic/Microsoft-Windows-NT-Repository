/*
 *	$XConsortium: VTPrsTbl.c,v 1.10 95/06/09 22:17:06 gildea Exp $
 *	$XFree86: xc/programs/xterm/VTPrsTbl.c,v 3.20 1999/10/13 04:21:40 dawes Exp $
 */

/*
 *
 * Copyright 1999 by Thomas E. Dickey <dickey@clark.net>
 *
 *                         All Rights Reserved
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE ABOVE LISTED COPYRIGHT HOLDER(S) BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name(s) of the above copyright
 * holders shall not be used in advertising or otherwise to promote the
 * sale, use or other dealings in this Software without prior written
 * authorization.
 *
 *
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include <xtermcfg.h>
#endif

#ifndef OPT_VT52_MODE
#define OPT_VT52_MODE   1 /* true if xterm supports VT52 emulation */
#endif

#ifndef OPT_WIDE_CHARS
#define OPT_WIDE_CHARS  0 /* true if xterm supports 16-bit characters */
#endif

#include <VTparse.h>

#if !OPT_DEC_LOCATOR
#undef  CASE_CSI_TICK_STATE
#define CASE_CSI_TICK_STATE CASE_ESC_IGNORE
#endif

#if !OPT_WIDE_CHARS
#undef  CASE_ESC_PERCENT
#define CASE_ESC_PERCENT CASE_ESC_IGNORE
#endif

/*
 * Stupid Apollo C preprocessor can't handle long lines.  So... To keep
 * it happy, we put each onto a separate line....  Sigh...
 */

Const PARSE_T ansi_table[] =
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	EOT		ENQ		ACK		BEL	*/
CASE_PRINT,
CASE_ENQ,
CASE_PRINT,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	DC4		NAK		SYN		ETB	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	CAN		EM		SUB		ESC	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	SP		!		"		#	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	$		%		&		'	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	(		)		*		+	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	,		-		.		/	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	0		1		2		3	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	4		5		6		7	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	8		9		:		;	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	<		=		>		?	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	@		A		B		C	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	D		E		F		G	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	H		I		J		K	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	L		M		N		O	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	P		Q		R		S	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	T		U		V		W	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	X		Y		Z		[	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	\		]		^		_	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	`		a		b		c	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	d		e		f		g	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	h		i		j		k	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	l		m		n		o	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	p		q		r		s	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	t		u		v		w	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	x		y		z		{	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	|		}		~		DEL	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      0x84            0x85            0x86            0x87    */
CASE_IND,
CASE_NEL,
CASE_PRINT,
CASE_PRINT,
/*      0x88            0x89            0x8a            0x8b    */
CASE_HTS,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_PRINT,
CASE_RI,
CASE_SS2,
CASE_SS3,
/*      0x90            0x91            0x92            0x93    */
CASE_DCS,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      0x94            0x95            0x96            0x97    */
CASE_PRINT,
CASE_PRINT,
CASE_SPA,
CASE_EPA,
/*      0x98            0x99            0x9a            0x9b    */
CASE_SOS,
CASE_PRINT,
CASE_DECID,
CASE_CSI_STATE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_ST,
CASE_OSC,
CASE_PM,
CASE_APC,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      currency        yen             brokenbar       section         */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      notsign         hyphen          registered      macron          */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      acute           mu              paragraph       periodcentered  */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      agrave          aacute          acircumflex     atilde          */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      eth             ntilde          ograve          oacute          */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
};

Const PARSE_T csi_table[] =		/* CSI */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_CSI_EX_STATE,
CASE_CSI_QUOTE_STATE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_CSI_TICK_STATE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	4		5		6		7	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	8		9		:		;	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_IGNORE,
CASE_ESC_SEMI,
/*	<		=		>		?	*/
CASE_IGNORE,
CASE_DEC3_STATE,
CASE_DEC2_STATE,
CASE_DEC_STATE,
/*	@		A		B		C	*/
CASE_ICH,
CASE_CUU,
CASE_CUD,
CASE_CUF,
/*	D		E		F		G	*/
CASE_CUB,
CASE_CNL,
CASE_CPL,
CASE_HPA,
/*	H		I		J		K	*/
CASE_CUP,
CASE_CHT,
CASE_ED,
CASE_EL,
/*	L		M		N		O	*/
CASE_IL,
CASE_DL,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_DCH,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_SU,
/*	T		U		V		W	*/
CASE_TRACK_MOUSE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_ECH,
CASE_GROUND_STATE,
CASE_CBT,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_HPA,
CASE_GROUND_STATE,
CASE_REP,
CASE_DA1,
/*	d		e		f		g	*/
CASE_VPA,
CASE_GROUND_STATE,
CASE_CUP,
CASE_TBC,
/*	h		i		j		k	*/
CASE_SET,
CASE_MC,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_RST,
CASE_SGR,
CASE_CPR,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECSTBM,
CASE_DECSC,
/*	t		u		v		w	*/
CASE_XTERM_WINOPS,
CASE_DECRC,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_DECREQTPARM,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T csi_ex_table[] =		/* CSI ! */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	4		5		6		7	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	8		9		:		;	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_IGNORE_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_DECSTR,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T csi_quo_table[] =		/* CSI ... " */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	4		5		6		7	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	8		9		:		;	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_IGNORE,
CASE_ESC_SEMI,
/*	<		=		>		?	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	@		A		B		C	*/
CASE_ESC_IGNORE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_DECSCL,
CASE_DECSCA,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

#if OPT_DEC_LOCATOR
Const PARSE_T csi_tick_table[] =	/* CSI ... ' */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	4		5		6		7	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	8		9		:		;	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_IGNORE_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECEFR,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECELR,
CASE_DECSLE,
/*	|		}		~		DEL	*/
CASE_DECRQLP,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	0x80		0x81		0x82		0x83	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0x84		0x85		0x86		0x87	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0x88		0x89		0x8a		0x8b	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0x8c		0x8d		0x8e		0x8f	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0x90		0x91		0x92		0x93	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0x94		0x95		0x96		0x97	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0x98		0x99		0x9a		0x9b	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0x9c		0x9d		0x9e		0x9f	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	nobreakspace	exclamdown	cent		sterling	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	currency	yen		brokenbar	section		*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	diaeresis	copyright	ordfeminine	guillemotleft	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	notsign		hyphen		registered	macron		*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	degree		plusminus	twosuperior	threesuperior	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	acute		mu		paragraph	periodcentered	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	cedilla		onesuperior	masculine	guillemotright	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	onequarter	onehalf		threequarters	questiondown	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	Agrave		Aacute		Acircumflex	Atilde		*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	Adiaeresis	Aring		AE		Ccedilla	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	Egrave		Eacute		Ecircumflex	Ediaeresis	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	Igrave		Iacute		Icircumflex	Idiaeresis	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	Eth		Ntilde		Ograve		Oacute		*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	Ocircumflex	Otilde		Odiaeresis	multiply	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	Ooblique	Ugrave		Uacute		Ucircumflex	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	Udiaeresis	Yacute		Thorn		ssharp		*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	agrave		aacute		acircumflex	atilde		*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	adiaeresis	aring		ae		ccedilla	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	egrave		eacute		ecircumflex	ediaeresis	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	igrave		iacute		icircumflex	idiaeresis	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	eth		ntilde		ograve		oacute		*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	ocircumflex	otilde		odiaeresis	division	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	oslash		ugrave		uacute		ucircumflex	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	udiaeresis	yacute		thorn		ydiaeresis	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};
#endif	/* OPT_DEC_LOCATOR */

Const PARSE_T dec_table[] =		/* CSI ? */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	4		5		6		7	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	8		9		:		;	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_IGNORE,
CASE_ESC_SEMI,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECSED,
CASE_DECSEL,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_DECSET,
CASE_DEC_MC,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_DECRST,
CASE_GROUND_STATE,
CASE_DSR,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_XTERM_RESTORE,
CASE_XTERM_SAVE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T dec2_table[] =		/* CSI > */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	4		5		6		7	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	8		9		:		;	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_IGNORE,
CASE_ESC_SEMI,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECSED,
CASE_DECSEL,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DA2,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T dec3_table[] =		/* CSI = */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	4		5		6		7	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
/*	8		9		:		;	*/
CASE_ESC_DIGIT,
CASE_ESC_DIGIT,
CASE_IGNORE,
CASE_ESC_SEMI,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECSED,
CASE_DECSEL,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECRPTUI,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T eigtable[] =		/* CASE_ESC_IGNORE */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	$		%		&		'	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	(		)		*		+	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	,		-		.		/	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0		1		2		3	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	4		5		6		7	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	8		9		:		;	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T esc_table[] =		/* ESC */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_SP_STATE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_SCR_STATE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_PERCENT,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_SCS0_STATE,
CASE_SCS1_STATE,
CASE_SCS2_STATE,
CASE_SCS3_STATE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	4		5		6		7	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECSC,
/*	8		9		:		;	*/
CASE_DECRC,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_DECKPAM,
CASE_DECKPNM,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_IND,
CASE_NEL,
CASE_HP_BUGGY_LL,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_HTS,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_RI,
CASE_SS2,
CASE_SS3,
/*	P		Q		R		S	*/
CASE_DCS,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_XTERM_TITLE,
CASE_GROUND_STATE,
CASE_SPA,
CASE_EPA,
/*	X		Y		Z		[	*/
CASE_SOS,
CASE_GROUND_STATE,
CASE_DECID,
CASE_CSI_STATE,
/*	\		]		^		_	*/
CASE_ST,
CASE_OSC,
CASE_PM,
CASE_APC,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_RIS,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_HP_MEM_LOCK,
CASE_HP_MEM_UNLOCK,
CASE_LS2,
CASE_LS3,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_LS3R,
CASE_LS2R,
CASE_LS1R,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T esc_sp_table[] =		/* ESC SP */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	4		5		6		7	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	8		9		:		;	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_S7C1T,
CASE_S8C1T,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	L		M		N		O	*/
CASE_ANSI_LEVEL_1,
CASE_ANSI_LEVEL_2,
CASE_ANSI_LEVEL_3,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_IGNORE_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T iestable[] =		/* CASE_IGNORE_ESC */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	BS		HT		NL		VT	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	NP		CR		SO		SI	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	FS		GS		RS		US	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	SP		!		"		#	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	$		%		&		'	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	(		)		*		+	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	,		-		.		/	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	0		1		2		3	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	4		5		6		7	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	8		9		:		;	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	<		=		>		?	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	@		A		B		C	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	D		E		F		G	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	H		I		J		K	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	L		M		N		O	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	P		Q		R		S	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	T		U		V		W	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	X		Y		Z		[	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	`		a		b		c	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	d		e		f		g	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	h		i		j		k	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	l		m		n		o	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	p		q		r		s	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	t		u		v		w	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	x		y		z		{	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*	|		}		~		DEL	*/
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
CASE_IGNORE_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T igntable[] =		/* CASE_IGNORE_STATE */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	BS		HT		NL		VT	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	NP		CR		SO		SI	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_GROUND_STATE,
CASE_IGNORE,
CASE_GROUND_STATE,
CASE_IGNORE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	$		%		&		'	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	(		)		*		+	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	,		-		.		/	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0		1		2		3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	4		5		6		7	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	8		9		:		;	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	<		=		>		?	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	@		A		B		C	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	D		E		F		G	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	H		I		J		K	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	L		M		N		O	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	P		Q		R		S	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	T		U		V		W	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	X		Y		Z		[	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	\		]		^		_	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	`		a		b		c	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	d		e		f		g	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	h		i		j		k	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	l		m		n		o	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	p		q		r		s	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	t		u		v		w	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	x		y		z		{	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	|		}		~		DEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T scrtable[] =		/* ESC # */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_DECDHL,
/*	4		5		6		7	*/
CASE_DECDHL,
CASE_DECSWL,
CASE_DECDWL,
CASE_GROUND_STATE,
/*	8		9		:		;	*/
CASE_DECALN,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

Const PARSE_T scstable[] =		/* ESC ( etc. */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_GSETS,
CASE_GSETS,
CASE_GSETS,
CASE_GROUND_STATE,
/*	4		5		6		7	*/
CASE_GSETS,
CASE_GSETS,
CASE_GSETS,
CASE_GSETS,
/*	8		9		:		;	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GSETS,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_GROUND_STATE,
CASE_GSETS,
CASE_GSETS,
CASE_GSETS,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GSETS,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	H		I		J		K	*/
CASE_GSETS,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GSETS,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_GROUND_STATE,
CASE_GSETS,
CASE_GSETS,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GSETS,
CASE_GSETS,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};

/*
 * This table is treated specially.  The CASE_IGNORE entries correspond to the
 * characters that can be accumulated for the string function (e.g., OSC).
 */
Const PARSE_T sos_table[] =		/* OSC, DCS, etc. */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_ENQ,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_GROUND_STATE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	$		%		&		'	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	(		)		*		+	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	,		-		.		/	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0		1		2		3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	4		5		6		7	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	8		9		:		;	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	<		=		>		?	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	@		A		B		C	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	D		E		F		G	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	H		I		J		K	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	L		M		N		O	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	P		Q		R		S	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	T		U		V		W	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	X		Y		Z		[	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	\		]		^		_	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	`		a		b		c	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	d		e		f		g	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	h		i		j		k	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	l		m		n		o	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	p		q		r		s	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	t		u		v		w	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	x		y		z		{	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	|		}		~		DEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IND,
CASE_NEL,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_HTS,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_RI,
CASE_SS2,
CASE_SS3,
/*      0x90            0x91            0x92            0x93    */
CASE_DCS,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_SPA,
CASE_EPA,
/*      0x98            0x99            0x9a            0x9b    */
CASE_SOS,
CASE_IGNORE,
CASE_DECID,
CASE_CSI_STATE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_ST,
CASE_OSC,
CASE_PM,
CASE_APC,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      currency        yen             brokenbar       section         */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      notsign         hyphen          registered      macron          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      acute           mu              paragraph       periodcentered  */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      eth             ntilde          ograve          oacute          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
};

#if OPT_VT52_MODE
Const PARSE_T vt52_table[] =
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_IGNORE,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_IGNORE,
CASE_IGNORE,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	$		%		&		'	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	(		)		*		+	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	,		-		.		/	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	0		1		2		3	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	4		5		6		7	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	8		9		:		;	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	<		=		>		?	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	@		A		B		C	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	D		E		F		G	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	H		I		J		K	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	L		M		N		O	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	P		Q		R		S	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	T		U		V		W	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	X		Y		Z		[	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	\		]		^		_	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	`		a		b		c	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	d		e		f		g	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	h		i		j		k	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	l		m		n		o	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	p		q		r		s	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	t		u		v		w	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	x		y		z		{	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
/*	|		}		~		DEL	*/
CASE_PRINT,
CASE_PRINT,
CASE_PRINT,
CASE_IGNORE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      currency        yen             brokenbar       section         */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      notsign         hyphen          registered      macron          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      acute           mu              paragraph       periodcentered  */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      eth             ntilde          ograve          oacute          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
};

Const PARSE_T vt52_esc_table[] =
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	BS		HT		NL		VT	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	NP		CR		SO		SI	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	$		%		&		'	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	(		)		*		+	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	,		-		.		/	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	0		1		2		3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	4		5		6		7	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	8		9		:		;	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	<		=		>		?	*/
CASE_ANSI_LEVEL_1,
CASE_DECKPAM,
CASE_DECKPNM,
CASE_IGNORE,
/*	@		A		B		C	*/
CASE_IGNORE,
CASE_CUU,
CASE_CUD,
CASE_CUF,
/*	D		E		F		G	*/
CASE_CUB,
CASE_IGNORE,
CASE_SO,
CASE_SI,
/*	H		I		J		K	*/
CASE_CUP,
CASE_RI,
CASE_ED,
CASE_EL,
/*	L		M		N		O	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	P		Q		R		S	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	T		U		V		W	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	X		Y		Z		[	*/
CASE_IGNORE,
CASE_VT52_CUP,
CASE_DECID,
CASE_IGNORE,
/*	\		]		^		_	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	`		a		b		c	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	d		e		f		g	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	h		i		j		k	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	l		m		n		o	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	p		q		r		s	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	t		u		v		w	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	x		y		z		{	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	|		}		~		DEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      currency        yen             brokenbar       section         */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      notsign         hyphen          registered      macron          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      acute           mu              paragraph       periodcentered  */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      eth             ntilde          ograve          oacute          */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
};
#endif /* OPT_VT52_MODE */

#if OPT_WIDE_CHARS
Const PARSE_T esc_pct_table[] =		/* ESC % */
{
/*	NUL		SOH		STX		ETX	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	EOT		ENQ		ACK		BEL	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_BELL,
/*	BS		HT		NL		VT	*/
CASE_BS,
CASE_TAB,
CASE_VMOT,
CASE_VMOT,
/*	NP		CR		SO		SI	*/
CASE_VMOT,
CASE_CR,
CASE_SO,
CASE_SI,
/*	DLE		DC1		DC2		DC3	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	DC4		NAK		SYN		ETB	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	CAN		EM		SUB		ESC	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_ESC,
/*	FS		GS		RS		US	*/
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*	SP		!		"		#	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	$		%		&		'	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	(		)		*		+	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	,		-		.		/	*/
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
CASE_ESC_IGNORE,
/*	0		1		2		3	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	4		5		6		7	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	8		9		:		;	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	<		=		>		?	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	@		A		B		C	*/
CASE_UTF8,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	D		E		F		G	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_UTF8,
/*	H		I		J		K	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	L		M		N		O	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	P		Q		R		S	*/
CASE_IGNORE_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	T		U		V		W	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	X		Y		Z		[	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	\		]		^		_	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	`		a		b		c	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	d		e		f		g	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	h		i		j		k	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	l		m		n		o	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	p		q		r		s	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	t		u		v		w	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	x		y		z		{	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*	|		}		~		DEL	*/
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      0x80            0x81            0x82            0x83    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x84            0x85            0x86            0x87    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x88            0x89            0x8a            0x8b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x8c            0x8d            0x8e            0x8f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x90            0x91            0x92            0x93    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x94            0x95            0x96            0x97    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x98            0x99            0x9a            0x9b    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      0x9c            0x9d            0x9e            0x9f    */
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
CASE_IGNORE,
/*      nobreakspace    exclamdown      cent            sterling        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      currency        yen             brokenbar       section         */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      diaeresis       copyright       ordfeminine     guillemotleft   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      notsign         hyphen          registered      macron          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      degree          plusminus       twosuperior     threesuperior   */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      acute           mu              paragraph       periodcentered  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      cedilla         onesuperior     masculine       guillemotright  */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      onequarter      onehalf         threequarters   questiondown    */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Agrave          Aacute          Acircumflex     Atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Adiaeresis      Aring           AE              Ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Egrave          Eacute          Ecircumflex     Ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Igrave          Iacute          Icircumflex     Idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Eth             Ntilde          Ograve          Oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ocircumflex     Otilde          Odiaeresis      multiply        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Ooblique        Ugrave          Uacute          Ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      Udiaeresis      Yacute          Thorn           ssharp          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      agrave          aacute          acircumflex     atilde          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      adiaeresis      aring           ae              ccedilla        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      egrave          eacute          ecircumflex     ediaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      igrave          iacute          icircumflex     idiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      eth             ntilde          ograve          oacute          */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      ocircumflex     otilde          odiaeresis      division        */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      oslash          ugrave          uacute          ucircumflex     */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
/*      udiaeresis      yacute          thorn           ydiaeresis      */
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
CASE_GROUND_STATE,
};
#endif
