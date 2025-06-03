typedef union
{
  real realval;
  symval_type symval;
  string strval;
} YYSTYPE;
#define	TR_CHAR	258
#define	TR_CHAR_WIDTH	259
#define	TR_CODINGSCHEME	260
#define	TR_DEFINE	261
#define	TR_FONTDIMEN	262
#define	TR_KERN	263
#define	T_IDENTIFIER	264
#define	T_STRING	265
#define	T_REAL	266


extern YYSTYPE yylval;
