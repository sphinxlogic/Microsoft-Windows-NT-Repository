typedef union
{
  int int_val;
  real real_val;
  dimen_type dimen_val;
  charname_type charname_val;
  ccc_cmd_type cmd_val;
  ccc_cmd_list_type *cmd_list_val;
} YYSTYPE;
#define	TR_DEFINE	258
#define	TR_END	259
#define	TR_SETCHAR	260
#define	TR_SETCHARBB	261
#define	TR_HMOVE	262
#define	TR_VMOVE	263
#define	T_REAL	264
#define	TR_PT	265
#define	TR_XHEIGHT	266
#define	TR_CAPHEIGHT	267
#define	TR_FONTDEPTH	268
#define	TR_DESIGNSIZE	269
#define	TR_EM	270
#define	TR_HEIGHT	271
#define	TR_WIDTH	272
#define	TR_DEPTH	273
#define	TR_BBHEIGHT	274
#define	TR_BBWIDTH	275
#define	T_CHARNAME	276


extern YYSTYPE yylval;
