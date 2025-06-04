/******************************************************************************
 * Copyright 1995 by Thomas E. Dickey.  All Rights Reserved.                  *
 *                                                                            *
 * You may freely copy or redistribute this software, so long as there is no  *
 * profit made from its use, sale trade or reproduction. You may not change   *
 * this copyright notice, and it must be included in any copy made.           *
 ******************************************************************************/
/* $Id: dspc.h,v 1.1 1995/02/19 02:27:25 tom Exp $ */

/* dspc.c */
void dspc_init(char *scale, int coladj, int col);
void dspc_move(char *scale, int col, int line);
