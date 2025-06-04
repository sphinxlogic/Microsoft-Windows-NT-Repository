/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: ttrace.h,v 1.1 1995/05/28 19:55:19 tom Exp $
 *
 * interface of ttrace.c
 */
#ifndef TTRACE_H
#define TTRACE_H

extern	void	trace(char *format, ...);
extern	void	ttrace(char *format, ...);

#endif /* TTRACE_H */
