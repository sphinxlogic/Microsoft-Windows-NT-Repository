/*
** Copyright 1989, 1992 by Lionel Fournigault
**
** Permission to use, copy, and distribute for non-commercial purposes,
** is hereby granted without fee, providing that the above copyright
** notice appear in all copies and that both the copyright notice and this
** permission notice appear in supporting documentation.
** The software may be modified for your own purposes, but modified versions
** may not be distributed.
** This software is provided "as is" without any expressed or implied warranty.
**
**
*/

#ifndef GLOBALPARSE_H
#define GLOBALPARSE_H


typedef struct {
  char  text[256];
  int   precedence;
  int   position;
  int   is_virtual;
} lex_info_type;

#define YYSTYPE lex_info_type


extern char yy_class_name[];

extern int line_count;


#endif  /*  GLOBALPARSE_H  */
