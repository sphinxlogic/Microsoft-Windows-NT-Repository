/*
 * File:	expr.h
 * Purpose:	
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef hy_exprh
#define hy_exprh

#include <math.h>
#include <stdlib.h>

#ifdef __hpux
#define alloca malloc
#endif

/* Rename all YACC/LEX stuff or we'll conflict with other
 * applications
 */

#define yyback PROIO_yyback
#define yylook PROIO_yylook
#define yywrap PROIO_yywrap
#define yyoutput PROIO_yyoutput
#define yylex PROIO_yylex
#define yyerror PROIO_yyerror
#define input PROIO_input
#define unput PROIO_unput

#define yyleng PROIO_yyleng
#define yytext PROIO_yytext
#define yymorfg PROIO_yymorfg
#define yylineno PROIO_yylineno
#define yytchar PROIO_yytchar
#define yyin PROIO_yyin
#define yyout PROIO_yyout
#define yysvf PROIO_yysvf
#define yyestate PROIO_yyestate
#define yysvec PROIO_yysvec
#define yybgin PROIO_yybgin
#define yyprevious PROIO_yyprevious
#define yylhs PROIO_yylhs
#define yylen PROIO_yylen
#define yydefred PROIO_yydefred
#define yydgoto PROIO_yydgoto
#define yysindex PROIO_yysindex
#define yyrindex PROIO_yyrindex
#define yygindex PROIO_yygindex
#define yytable PROIO_yytable
#define yycheck PROIO_yycheck
#define yyname PROIO_yyname
#define yyrule PROIO_yyrule
#define yydebug PROIO_yydebug
#define yynerrs PROIO_yynerrs
#define yyerrflag PROIO_yyerrflag
#define yychar PROIO_yychar
#define yyvsp PROIO_yyvsp
#define yyssp PROIO_yyssp
#define yyval PROIO_yyval
#define yylval PROIO_yylval
#define yyss PROIO_yyss
#define yyvs PROIO_yyvs
#define yyparse PROIO_yyparse

/* +++steve162e: more defines necessary */
#define yy_init_buffer PROIO_yy_init_buffer
#define yy_create_buffer PROIO_yy_create_buffer
#define yy_load_buffer_state PROIO_yy_load_buffer_state
#define yyrestart PROIO_yyrestart
#define yy_switch_to_buffer PROIO_yy_switch_to_buffer
#define yy_delete_buffer PROIO_yy_delete_buffer
/* ---steve162e */

/* WG 1/96: still more for flex 2.5 */
#define yy_scan_buffer PROIO_scan_buffer
#define yy_scan_string PROIO_scan_string
#define yy_scan_bytes PROIO_scan_bytes
#define yy_flex_debug PROIO_flex_debug
#define yy_flush_buffer PROIO_flush_buffer
#define yyleng PROIO_yyleng
#define yytext PROIO_yytext

#ifdef __cplusplus
extern "C" {
char *proio_cons(char *, char *);
char * make_integer(char *);
char * make_word(char *);
char * make_string(char *);
char * make_real(char *, char *);
char * make_exp(char *, char *);
char * make_exp2(char *, char *, char*);
void add_expr(char *);
void process_command(char *);
void syntax_error(char *);
}
#else
#if __BORLANDC__
char *proio_cons(char *, char *);
char * make_integer(char *);
char * make_word(char *);
char * make_string(char *);
char * make_real(char *, char *);
char * make_exp(char *, char *);
char * make_exp2(char *, char *, char*);
void add_expr(char *);
void process_command(char *);
void syntax_error(char *);
#else
char *proio_cons();
char * make_integer();
char * make_word();
char * make_string();
char * make_real();
char * make_exp();
char * make_exp2();

void add_expr();
void process_command();
void syntax_error();
#endif
#endif

#endif /* hy_exprh */
