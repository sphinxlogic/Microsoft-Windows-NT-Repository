/*************************************************************

  COPYRIGHT (C) DIGITAL EQUIPMENT CORPORATION, 1991
  ALL RIGHTS RESERVED.  UNPUBLISHED RIGHTS RESERVED
  UNDER THE COPYRIGHT LAWS OF THE UNITED STATES.

  RESTRICTED RIGHTS LEGEND   USE, DUPLICATION, OR 
  DISCLOSURE BY THE U.S. GOVERNMENT IS SUBJECT TO
  RESTRICTIONS AS SET FORTH IN SUBPARAGRAPH (C)(1)(II)
  OF DFARS 252.227-7013, OR IN FAR 52.227-19, OR IN
  FAR 52.227-14 ALT. III, AS APPLICABLE.
  
  THIS SOFTWARE IS PROPRIETARY TO AND EMBODIES CONFIDENTIAL
  TECHNOLOGY OF DIGITAL EQUIPMENT CORPORATION.  POSSESSION,
  USE, OR COPYING OF THE SOFTWARE AND MEDIA IS AUTHORIZED
  ONLY PURSUANT TO A VALID WRITTEN LICENSE FROM DIGITAL
  EQUIPMENT CORPORATION.

*************************************************************/

/*
 * $XConsortium: Xfuncs.h,v 1.8 91/04/17 09:27:52 rws Exp $
 * 
 * Copyright 1990 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. M.I.T. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 */

#ifndef _XFUNCS_H_
#define _XFUNCS_H_

#include <X11/Xosdefs.h>

#ifdef X_USEBFUNCS
void bcopy();
void bzero();
int bcmp();
#else
#if (__STDC__ && !defined(X_NOT_STDC_ENV) && !defined(sun) && !defined(macII)) || defined(SVR4) || defined(hpux) || defined(_IBMR2) || defined(ALPHA)
#include <string.h>
/* RLD
#define bcopy(b1,b2,len) memmove(b2, b1, (size_t)(len))
#define bzero(b,len) memset(b, 0, (size_t)(len))
#define bcmp(b1,b2,len) memcmp(b1, b2, (size_t)(len))
 */
#else
#ifdef sgi
#include <bstring.h>
#else
#ifdef SYSV
#include <memory.h>
#if defined(_XBCOPYFUNC) && !defined(macII)
#define bcopy _XBCOPYFUNC
#define _XNEEDBCOPYFUNC
#endif
void bcopy();
#define bzero(b,len) memset(b, 0, len)
#define bcmp(b1,b2,len) memcmp(b1, b2, len)
#else /* bsd */
void bcopy();
void bzero();
int bcmp();
#endif /* SYSV */
#endif /* sgi */
#endif /* __STDC__ and relatives */
#endif /* X_USEBFUNCS */

#endif /* _XFUNCS_H_ */
