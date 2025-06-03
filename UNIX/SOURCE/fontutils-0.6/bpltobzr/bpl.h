typedef union
{
  real_bounding_box_type *bbval;
  int intval;
  spline_list_type *outlineval;		/* A single cyclic path.  */
  real realval;
  spline_list_array_type *shapeval;	/* A sequence of outlines.  */
  spline_type splineval;		/* A single line or cubic.  */
  string strval;
} YYSTYPE;
#define	TR_BB	258
#define	TR_CHAR	259
#define	TR_DESIGNSIZE	260
#define	TR_FONTBB	261
#define	TR_FONTCOMMENT	262
#define	TR_FONTFILE	263
#define	TR_LINE	264
#define	TR_NCHARS	265
#define	TR_OUTLINE	266
#define	TR_SPLINE	267
#define	TR_WIDTH	268
#define	T_INTEGER	269
#define	T_REAL	270
#define	T_REALSTR	271
#define	T_STRING	272


extern YYSTYPE yylval;
