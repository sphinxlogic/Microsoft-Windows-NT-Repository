# include "stdio.h"
# define U(x) ((unsigned char)(x))
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# ifndef YYLMAX
# define YYLMAX 200
# endif
# define output(c) (void)putc(c,yyout)
#if defined(__cplusplus) || defined(__STDC__)
	int yyback(int *, int);
	int yyinput(void);
	int yylook(void);
	void yyoutput(int);
	int yyracc(int);
	int yyreject(void);
	void yyunput(int);

#ifndef __STDC__
#ifndef yyless
	void yyless(int);
#endif
#ifndef yywrap
	int yywrap(void);
#endif
#endif

#endif
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO (void)fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;

# line 3 "lev_comp.l"
/*	SCCS Id: @(#)lev_lex.c	3.1	93/06/17	*/

# line 4 "lev_comp.l"
/*	Copyright (c) 1989 by Jean-Christophe Collet */

# line 5 "lev_comp.l"
/* NetHack may be freely redistributed.  See license for details. */

#define LEV_LEX_C

#include "hack.h"
#include "lev_comp.h"
#include "sp_lev.h"


# line 13 "lev_comp.l"
/* Most of these don't exist in flex, yywrap is macro and
 * yyunput is properly declared in flex.skel.
 */
#if !defined(FLEX_SCANNER) && !defined(FLEXHACK_SCANNER)
int FDECL(yyback, (int *, int));
int NDECL(yylook);
int NDECL(yyinput);
int NDECL(yywrap);
int NDECL(yylex);
	/* Traditional lexes let yyunput() and yyoutput() default to int;
	 * newer ones may declare them as void since they don't return
	 * values.  For even more fun, the lex supplied as part of the
	 * newer unbundled compiler for SunOS 4.x adds the void declarations
	 * (under __STDC__ or _cplusplus ifdefs -- otherwise they remain
	 * int) while the bundled lex and the one with the older unbundled
	 * compiler do not.  To detect this, we need help from outside --
	 * sys/unix/Makefile.utl.
	 */
# if defined(NeXT) || defined(SVR4)
#  define VOIDYYPUT
# endif
# if !defined(VOIDYYPUT)
#  if defined(POSIX_TYPES) && !defined(BOS) && !defined(HISX) && !defined(_M_UNIX)
#   define VOIDYYPUT
#  endif
# endif
# if !defined(VOIDYYPUT) && defined(WEIRD_LEX)
#  if defined(SUNOS4) && defined(__STDC__) && (WEIRD_LEX > 1) 
#   define VOIDYYPUT
#  endif
# endif
# ifdef VOIDYYPUT
void FDECL(yyunput, (int));
void FDECL(yyoutput, (int));
# else
int FDECL(yyunput, (int));
int FDECL(yyoutput, (int));
# endif
#endif	/* !FLEX_SCANNER && !FLEXHACK_SCANNER */

void FDECL(init_yyin, (FILE *));
void FDECL(init_yyout, (FILE *));

#ifdef MICRO
#undef exit
extern void FDECL(exit, (int));
#endif


# line 61 "lev_comp.l"
/*
 * This doesn't always get put in lev_comp.h
 * (esp. when using older versions of bison).
 */
extern YYSTYPE yylval;

int line_number = 1, colon_line_number = 1;


# line 69 "lev_comp.l"
/*
 * This is *** UGLY *** but I can't think a better way to do it;
 * I really need a huge buffer to scan maps...
 * (This should probably be `#ifndef FLEX_SCANNER' since it's lex-specific.)
 */
#ifdef YYLMAX
#undef YYLMAX
#endif
#define YYLMAX	2048


# line 79 "lev_comp.l"
/*
 *	This is a hack required by Michael Hamel to get things
 *	working on the Mac.
 */
#if defined(applec) && !defined(FLEX_SCANNER) && !defined(FLEXHACK_SCANNER)
#undef input
#undef unput
#define unput(c) { yytchar = (c); if (yytchar == 10) yylineno--; *yysptr++ = yytchar; }
# ifndef YYNEWLINE
# define YYNEWLINE 10
# endif

char
input()		/* Under MPW \n is chr(13)! Compensate for this. */
{
	if (yysptr > yysbuf) return(*--yysptr);
	else {
		yytchar = getc(yyin);
		if (yytchar == '\n') {
		    yylineno++;
		    return(YYNEWLINE);
		}
		if (yytchar == EOF) return(0);
		else		    return(yytchar);
	}
}
#endif	/* applec && !FLEX_SCANNER && !FLEXHACK_SCANNER */

# define MAPC 2
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:

# line 112 "lev_comp.l"
{
#ifdef FLEX23_BUG
		  /*
		   * There is a bug in Flex 2.3 patch level < 6
		   * (absent in previous versions)
		   * that results in the following behaviour :
		   * Once you enter an yymore(), you never exit from it.
		   * This should do the trick!
		   */
		  extern int yy_more_len;

		  yy_more_len = 0;
#endif	/* FLEX23_BUG */
		  BEGIN(INITIAL);
		  yylval.map = (char *) alloc(yyleng-5);
		  strncpy(yylval.map, yytext,yyleng-6);
		  yylval.map[yyleng-6] = 0;
		  return MAP_ID;
		}
break;
case 2:

# line 131 "lev_comp.l"
{ line_number++; yymore(); }
break;
case 3:

# line 132 "lev_comp.l"
	{ line_number++; }
break;
case 4:

# line 133 "lev_comp.l"
	{ colon_line_number = line_number; return ':'; }
break;
case 5:

# line 134 "lev_comp.l"
	return MESSAGE_ID;
break;
case 6:

# line 135 "lev_comp.l"
	return MAZE_ID;
break;
case 7:

# line 136 "lev_comp.l"
	return NOMAP_ID;
break;
case 8:

# line 137 "lev_comp.l"
	return LEVEL_ID;
break;
case 9:

# line 138 "lev_comp.l"
return LEV_INIT_ID;
break;
case 10:

# line 139 "lev_comp.l"
	return FLAGS_ID;
break;
case 11:

# line 140 "lev_comp.l"
return GEOMETRY_ID;
break;
case 12:

# line 141 "lev_comp.l"
	{ BEGIN(MAPC); line_number++; }
break;
case 13:

# line 142 "lev_comp.l"
	return OBJECT_ID;
break;
case 14:

# line 143 "lev_comp.l"
	return MONSTER_ID;
break;
case 15:

# line 144 "lev_comp.l"
	return TRAP_ID;
break;
case 16:

# line 145 "lev_comp.l"
	return DOOR_ID;
break;
case 17:

# line 146 "lev_comp.l"
return DRAWBRIDGE_ID;
break;
case 18:

# line 147 "lev_comp.l"
return MAZEWALK_ID;
break;
case 19:

# line 148 "lev_comp.l"
	return WALLIFY_ID;
break;
case 20:

# line 149 "lev_comp.l"
	return REGION_ID;
break;
case 21:

# line 150 "lev_comp.l"
return RANDOM_OBJECTS_ID;
break;
case 22:

# line 151 "lev_comp.l"
return RANDOM_MONSTERS_ID;
break;
case 23:

# line 152 "lev_comp.l"
return RANDOM_PLACES_ID;
break;
case 24:

# line 153 "lev_comp.l"
	return ALTAR_ID;
break;
case 25:

# line 154 "lev_comp.l"
	return LADDER_ID;
break;
case 26:

# line 155 "lev_comp.l"
	return STAIR_ID;
break;
case 27:

# line 156 "lev_comp.l"
	return PORTAL_ID;
break;
case 28:

# line 157 "lev_comp.l"
return TELEPRT_ID;
break;
case 29:

# line 158 "lev_comp.l"
	return BRANCH_ID;
break;
case 30:

# line 159 "lev_comp.l"
return FOUNTAIN_ID;
break;
case 31:

# line 160 "lev_comp.l"
	return SINK_ID;
break;
case 32:

# line 161 "lev_comp.l"
	return POOL_ID;
break;
case 33:

# line 162 "lev_comp.l"
return NON_DIGGABLE_ID;
break;
case 34:

# line 163 "lev_comp.l"
return NON_PASSWALL_ID;
break;
case 35:

# line 164 "lev_comp.l"
	return ROOM_ID;
break;
case 36:

# line 165 "lev_comp.l"
	return SUBROOM_ID;
break;
case 37:

# line 166 "lev_comp.l"
return RAND_CORRIDOR_ID;
break;
case 38:

# line 167 "lev_comp.l"
return CORRIDOR_ID;
break;
case 39:

# line 168 "lev_comp.l"
	return GOLD_ID;
break;
case 40:

# line 169 "lev_comp.l"
return ENGRAVING_ID;
break;
case 41:

# line 170 "lev_comp.l"
	return NAME_ID;
break;
case 42:

# line 171 "lev_comp.l"
	return CHANCE_ID;
break;
case 43:

# line 172 "lev_comp.l"
return LEV;
break;
case 44:

# line 173 "lev_comp.l"
	{ yylval.i=D_ISOPEN; return DOOR_STATE; }
break;
case 45:

# line 174 "lev_comp.l"
	{ yylval.i=D_CLOSED; return DOOR_STATE; }
break;
case 46:

# line 175 "lev_comp.l"
	{ yylval.i=D_LOCKED; return DOOR_STATE; }
break;
case 47:

# line 176 "lev_comp.l"
	{ yylval.i=D_NODOOR; return DOOR_STATE; }
break;
case 48:

# line 177 "lev_comp.l"
	{ yylval.i=D_BROKEN; return DOOR_STATE; }
break;
case 49:

# line 178 "lev_comp.l"
	{ yylval.i=W_NORTH; return DIRECTION; }
break;
case 50:

# line 179 "lev_comp.l"
	{ yylval.i=W_EAST; return DIRECTION; }
break;
case 51:

# line 180 "lev_comp.l"
	{ yylval.i=W_SOUTH; return DIRECTION; }
break;
case 52:

# line 181 "lev_comp.l"
	{ yylval.i=W_WEST; return DIRECTION; }
break;
case 53:

# line 182 "lev_comp.l"
	{ yylval.i = -1; return RANDOM_TYPE; }
break;
case 54:

# line 183 "lev_comp.l"
	{ yylval.i = -2; return NONE; }
break;
case 55:

# line 184 "lev_comp.l"
	return O_REGISTER;
break;
case 56:

# line 185 "lev_comp.l"
	return M_REGISTER;
break;
case 57:

# line 186 "lev_comp.l"
	return P_REGISTER;
break;
case 58:

# line 187 "lev_comp.l"
	return A_REGISTER;
break;
case 59:

# line 188 "lev_comp.l"
	{ yylval.i=1; return LEFT_OR_RIGHT; }
break;
case 60:

# line 189 "lev_comp.l"
{ yylval.i=2; return LEFT_OR_RIGHT; }
break;
case 61:

# line 190 "lev_comp.l"
	{ yylval.i=3; return CENTER; }
break;
case 62:

# line 191 "lev_comp.l"
{ yylval.i=4; return LEFT_OR_RIGHT; }
break;
case 63:

# line 192 "lev_comp.l"
	{ yylval.i=5; return LEFT_OR_RIGHT; }
break;
case 64:

# line 193 "lev_comp.l"
	{ yylval.i=1; return TOP_OR_BOT; }
break;
case 65:

# line 194 "lev_comp.l"
	{ yylval.i=5; return TOP_OR_BOT; }
break;
case 66:

# line 195 "lev_comp.l"
	{ yylval.i=1; return LIGHT_STATE; }
break;
case 67:

# line 196 "lev_comp.l"
	{ yylval.i=0; return LIGHT_STATE; }
break;
case 68:

# line 197 "lev_comp.l"
	{ yylval.i=0; return FILLING; }
break;
case 69:

# line 198 "lev_comp.l"
{ yylval.i=1; return FILLING; }
break;
case 70:

# line 199 "lev_comp.l"
	{ yylval.i= AM_NONE; return ALIGNMENT; }
break;
case 71:

# line 200 "lev_comp.l"
	{ yylval.i= AM_LAWFUL; return ALIGNMENT; }
break;
case 72:

# line 201 "lev_comp.l"
	{ yylval.i= AM_NEUTRAL; return ALIGNMENT; }
break;
case 73:

# line 202 "lev_comp.l"
	{ yylval.i= AM_CHAOTIC; return ALIGNMENT; }
break;
case 74:

# line 203 "lev_comp.l"
{ yylval.i=1; return MON_ATTITUDE; }
break;
case 75:

# line 204 "lev_comp.l"
	{ yylval.i=0; return MON_ATTITUDE; }
break;
case 76:

# line 205 "lev_comp.l"
	{ yylval.i=1; return MON_ALERTNESS; }
break;
case 77:

# line 206 "lev_comp.l"
	{ yylval.i=0; return MON_ALERTNESS; }
break;
case 78:

# line 207 "lev_comp.l"
{ yylval.i= M_AP_FURNITURE; return MON_APPEARANCE; }
break;
case 79:

# line 208 "lev_comp.l"
{ yylval.i= M_AP_MONSTER;   return MON_APPEARANCE; }
break;
case 80:

# line 209 "lev_comp.l"
{ yylval.i= M_AP_OBJECT;    return MON_APPEARANCE; }
break;
case 81:

# line 210 "lev_comp.l"
	{ yylval.i=2; return ALTAR_TYPE; }
break;
case 82:

# line 211 "lev_comp.l"
	{ yylval.i=1; return ALTAR_TYPE; }
break;
case 83:

# line 212 "lev_comp.l"
	{ yylval.i=0; return ALTAR_TYPE; }
break;
case 84:

# line 213 "lev_comp.l"
	{ yylval.i=1; return UP_OR_DOWN; }
break;
case 85:

# line 214 "lev_comp.l"
	{ yylval.i=0; return UP_OR_DOWN; }
break;
case 86:

# line 215 "lev_comp.l"
	{ yylval.i=0; return BOOLEAN; }
break;
case 87:

# line 216 "lev_comp.l"
	{ yylval.i=1; return BOOLEAN; }
break;
case 88:

# line 217 "lev_comp.l"
	{ yylval.i=DUST; return ENGRAVING_TYPE; }
break;
case 89:

# line 218 "lev_comp.l"
	{ yylval.i=ENGRAVE; return ENGRAVING_TYPE; }
break;
case 90:

# line 219 "lev_comp.l"
	{ yylval.i=BURN; return ENGRAVING_TYPE; }
break;
case 91:

# line 220 "lev_comp.l"
	{ yylval.i=MARK; return ENGRAVING_TYPE; }
break;
case 92:

# line 221 "lev_comp.l"
	{ yylval.i=1; return CURSE_TYPE; }
break;
case 93:

# line 222 "lev_comp.l"
{ yylval.i=2; return CURSE_TYPE; }
break;
case 94:

# line 223 "lev_comp.l"
	{ yylval.i=3; return CURSE_TYPE; }
break;
case 95:

# line 224 "lev_comp.l"
{ yylval.i=NOTELEPORT; return FLAG_TYPE; }
break;
case 96:

# line 225 "lev_comp.l"
{ yylval.i=HARDFLOOR; return FLAG_TYPE; }
break;
case 97:

# line 226 "lev_comp.l"
	{ yylval.i=NOMMAP; return FLAG_TYPE; }
break;
case 98:

# line 227 "lev_comp.l"
{ yylval.i=SHORTSIGHTED; return FLAG_TYPE; }
break;
case 99:

# line 228 "lev_comp.l"
{ yylval.i=atoi(yytext); return INTEGER; }
break;
case 100:

# line 229 "lev_comp.l"
{ yytext[yyleng-1] = 0; /* Discard the trailing \" */
		  yylval.map = (char *) alloc(strlen(yytext+1)+1);
		  strcpy(yylval.map, yytext+1); /* Discard the first \" */
		  return STRING; }
break;
case 101:

# line 233 "lev_comp.l"
	{ line_number++; }
break;
case 102:

# line 234 "lev_comp.l"
	;
break;
case 103:

# line 235 "lev_comp.l"
	{ yylval.i = yytext[1]; return CHAR; }
break;
case 104:

# line 236 "lev_comp.l"
	{ return yytext[0]; }
break;
case -1:
break;
default:
(void)fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
#ifdef	AMIGA
long *alloc(n)
	unsigned n;
{
	return ((long *)malloc (n));
}
#endif

/* routine to switch to another input file; needed for flex */
void init_yyin( input_f )
FILE *input_f;
{
#if defined(FLEX_SCANNER) || defined(FLEXHACK_SCANNER)
	if (yyin)
	    yyrestart(input_f);
	else
#endif
	    yyin = input_f;
}
/* analogous routine (for completeness) */
void init_yyout( output_f )
FILE *output_f;
{
	yyout = output_f;
}

int yyvstop[] = {
0,

104,
0,

102,
104,
0,

101,
0,

104,
0,

104,
0,

104,
0,

99,
104,
0,

4,
104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

2,
101,
0,

102,
104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

104,
0,

102,
0,

100,
0,

99,
0,

84,
0,

3,
0,

2,
0,

102,
0,

2,
3,
0,

103,
0,

71,
0,

66,
0,

64,
0,

16,
0,

39,
0,

6,
0,

41,
0,

32,
0,

35,
0,

31,
0,

15,
0,

90,
0,

85,
0,

88,
0,

50,
0,

59,
0,

91,
0,

54,
0,

44,
0,

87,
0,

52,
0,

12,
0,

24,
0,

10,
0,

8,
0,

7,
0,

26,
0,

58,
0,

83,
0,

77,
0,

73,
0,

86,
0,

49,
0,

57,
0,

63,
0,

51,
0,

67,
0,

29,
0,

42,
0,

25,
0,

13,
0,

27,
0,

20,
0,

76,
0,

65,
0,

48,
0,

61,
0,

45,
0,

94,
0,

68,
0,

46,
0,

47,
0,

97,
0,

55,
0,

53,
0,

82,
0,

1,
0,

5,
0,

14,
0,

36,
0,

19,
0,

92,
0,

89,
0,

75,
0,

56,
0,

72,
0,

70,
0,

81,
0,

38,
0,

30,
0,

11,
0,

9,
0,

18,
0,

80,
0,

74,
0,

93,
0,

69,
0,

40,
0,

60,
0,

96,
0,

43,
0,

78,
0,

79,
0,

17,
0,

62,
0,

95,
0,

33,
0,

34,
0,

98,
0,

23,
0,

21,
0,

22,
0,

28,
0,

37,
0,
0};
# define YYTYPE int
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,5,	0,0,	
0,0,	0,0,	0,0,	0,0,	
8,64,	0,0,	1,6,	1,7,	
0,0,	0,0,	6,63,	0,0,	
8,64,	8,64,	0,0,	0,0,	
0,0,	0,0,	0,0,	62,152,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,6,	0,0,	1,8,	
1,5,	6,63,	0,0,	8,64,	
1,9,	8,65,	8,64,	0,0,	
1,10,	62,153,	0,0,	0,0,	
62,153,	1,11,	8,64,	66,154,	
0,0,	0,0,	0,0,	8,64,	
62,153,	0,0,	0,0,	1,12,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,13,	1,14,	
1,15,	1,16,	1,17,	1,18,	
1,19,	24,87,	1,20,	29,97,	
48,142,	1,21,	1,22,	1,23,	
1,24,	1,25,	13,68,	1,26,	
1,27,	1,28,	14,69,	15,70,	
1,29,	16,72,	17,74,	20,79,	
16,73,	18,75,	15,71,	25,88,	
18,76,	19,77,	1,30,	1,31,	
1,32,	1,33,	1,34,	1,35,	
21,80,	1,36,	28,95,	19,78,	
21,81,	1,37,	1,38,	1,39,	
1,40,	1,41,	2,5,	1,42,	
1,43,	1,44,	1,45,	28,96,	
1,46,	46,139,	2,6,	2,7,	
10,67,	10,67,	10,67,	10,67,	
10,67,	10,67,	10,67,	10,67,	
10,67,	10,67,	33,109,	22,82,	
23,85,	26,89,	35,113,	22,83,	
33,110,	26,90,	27,92,	53,146,	
34,111,	2,6,	35,114,	2,8,	
2,47,	22,84,	23,86,	26,91,	
2,9,	27,93,	27,94,	30,98,	
2,10,	34,112,	36,115,	2,5,	
40,126,	2,11,	30,99,	54,69,	
31,101,	39,124,	30,100,	31,102,	
56,148,	41,128,	31,103,	2,12,	
36,116,	31,104,	40,127,	39,125,	
41,129,	57,79,	2,13,	2,14,	
2,15,	2,16,	2,17,	2,18,	
2,19,	2,5,	2,20,	59,88,	
2,5,	2,21,	2,48,	2,23,	
2,24,	2,25,	37,117,	2,26,	
2,27,	2,28,	37,118,	42,130,	
2,29,	45,137,	37,119,	45,138,	
38,121,	2,5,	38,122,	42,131,	
37,120,	55,147,	2,30,	2,31,	
2,32,	2,33,	2,34,	2,35,	
55,71,	2,36,	3,49,	43,132,	
38,123,	2,37,	2,38,	2,39,	
2,40,	2,41,	43,133,	2,42,	
2,43,	2,44,	2,45,	44,135,	
2,46,	43,134,	44,136,	61,151,	
2,5,	2,5,	2,5,	32,105,	
3,50,	58,149,	32,106,	3,51,	
68,155,	58,81,	32,107,	3,9,	
69,156,	70,157,	71,158,	3,52,	
72,159,	73,160,	74,161,	32,108,	
52,67,	52,67,	52,67,	52,67,	
52,67,	52,67,	52,67,	52,67,	
52,67,	52,67,	3,12,	75,162,	
76,163,	77,164,	78,165,	60,150,	
79,166,	3,53,	3,54,	3,55,	
3,16,	3,56,	3,18,	3,19,	
80,167,	3,57,	60,93,	60,94,	
3,58,	3,22,	3,23,	3,24,	
3,59,	81,168,	3,26,	3,60,	
3,28,	82,169,	83,170,	3,61,	
84,171,	85,172,	86,173,	86,174,	
87,175,	89,178,	90,179,	91,180,	
4,49,	3,30,	3,31,	3,32,	
3,33,	3,34,	3,35,	88,176,	
3,36,	92,181,	88,177,	93,182,	
3,37,	3,38,	3,39,	3,40,	
3,41,	94,183,	3,42,	3,43,	
3,44,	3,45,	4,50,	3,46,	
95,184,	4,62,	96,185,	97,186,	
98,187,	4,9,	99,189,	100,190,	
101,191,	4,52,	102,192,	103,193,	
4,51,	104,194,	105,195,	98,188,	
106,196,	107,197,	108,198,	109,199,	
110,200,	111,201,	112,202,	113,203,	
4,12,	114,204,	116,207,	117,208,	
119,211,	120,212,	122,216,	4,53,	
4,54,	4,55,	4,16,	4,56,	
4,18,	4,19,	4,51,	4,57,	
118,209,	4,51,	4,58,	4,48,	
4,23,	4,24,	4,59,	123,217,	
4,26,	4,60,	4,28,	115,205,	
121,213,	4,61,	124,218,	126,225,	
118,210,	115,206,	4,51,	121,214,	
127,226,	121,215,	128,227,	4,30,	
4,31,	4,32,	4,33,	4,34,	
4,35,	129,228,	4,36,	130,229,	
131,230,	132,231,	4,37,	4,38,	
4,39,	4,40,	4,41,	9,66,	
4,42,	4,43,	4,44,	4,45,	
134,234,	4,46,	47,140,	9,66,	
9,0,	4,51,	4,51,	4,51,	
135,235,	133,232,	47,140,	47,141,	
133,233,	136,236,	139,240,	146,155,	
137,237,	50,63,	50,143,	137,238,	
142,241,	147,242,	149,167,	150,181,	
148,243,	137,239,	9,66,	148,161,	
9,66,	9,66,	142,169,	151,244,	
155,245,	47,140,	156,246,	47,140,	
47,140,	9,66,	157,247,	158,248,	
50,144,	159,249,	9,66,	50,145,	
47,140,	153,152,	160,250,	125,219,	
161,251,	47,140,	125,220,	50,145,	
162,252,	50,145,	50,145,	163,253,	
164,254,	51,143,	165,255,	125,221,	
125,222,	166,256,	167,257,	168,258,	
125,223,	169,259,	125,224,	153,153,	
170,260,	171,261,	153,153,	172,262,	
173,263,	50,145,	50,145,	50,145,	
174,264,	175,265,	153,153,	51,145,	
50,145,	50,145,	51,145,	50,145,	
50,145,	176,266,	177,267,	178,268,	
50,145,	179,269,	51,145,	50,145,	
51,145,	51,145,	180,270,	50,145,	
181,271,	182,272,	183,273,	184,274,	
50,145,	185,275,	186,276,	187,277,	
188,278,	189,279,	190,280,	191,281,	
192,282,	193,283,	194,284,	195,285,	
51,145,	51,145,	51,145,	196,286,	
197,287,	198,288,	199,289,	51,145,	
51,145,	200,290,	51,145,	51,145,	
201,291,	202,292,	203,293,	51,145,	
204,294,	205,295,	51,145,	50,145,	
50,145,	50,145,	51,145,	206,296,	
207,297,	209,298,	210,299,	51,145,	
212,300,	213,301,	214,302,	215,303,	
216,304,	217,305,	218,306,	219,307,	
220,308,	221,309,	222,310,	223,311,	
224,312,	225,313,	226,314,	227,315,	
228,316,	229,317,	230,318,	231,319,	
232,320,	233,321,	234,322,	236,323,	
237,324,	238,325,	239,326,	240,327,	
241,328,	242,247,	51,145,	51,145,	
51,145,	243,329,	244,330,	245,331,	
246,332,	247,333,	248,334,	250,335,	
251,336,	252,337,	253,338,	254,339,	
256,340,	257,341,	258,342,	259,343,	
260,344,	261,345,	263,346,	264,347,	
265,349,	267,350,	268,351,	269,352,	
272,353,	273,354,	274,355,	276,356,	
277,357,	278,358,	279,359,	264,348,	
280,360,	281,361,	282,362,	283,363,	
285,364,	286,365,	287,366,	288,367,	
292,368,	293,369,	294,370,	295,371,	
296,372,	297,373,	299,374,	300,375,	
301,376,	302,377,	303,378,	305,379,	
306,380,	307,381,	308,382,	309,383,	
311,384,	312,385,	313,386,	315,387,	
316,388,	317,389,	318,390,	319,391,	
320,392,	321,393,	322,394,	324,395,	
325,396,	326,397,	329,398,	330,399,	
332,400,	333,401,	334,402,	335,403,	
336,404,	338,405,	339,406,	340,407,	
341,408,	343,409,	344,410,	345,411,	
347,412,	348,413,	349,414,	350,415,	
351,416,	352,417,	354,418,	355,419,	
356,420,	359,421,	361,422,	362,423,	
363,424,	364,425,	366,426,	367,427,	
368,428,	370,429,	371,430,	372,432,	
373,433,	374,434,	375,435,	376,436,	
371,431,	377,437,	378,438,	379,439,	
380,440,	381,441,	382,442,	383,443,	
385,444,	386,445,	387,446,	389,447,	
391,448,	392,449,	393,450,	395,451,	
396,452,	398,453,	399,420,	402,454,	
403,455,	404,456,	405,457,	406,458,	
407,459,	409,460,	410,461,	411,462,	
412,463,	413,464,	416,465,	418,466,	
419,467,	420,468,	422,469,	428,470,	
430,471,	431,472,	432,473,	433,474,	
434,475,	436,476,	437,477,	438,478,	
439,479,	440,480,	441,481,	444,482,	
446,483,	448,484,	449,485,	451,486,	
452,487,	454,488,	455,489,	456,490,	
457,491,	458,492,	459,493,	460,494,	
463,495,	464,496,	465,497,	467,501,	
471,502,	472,503,	473,504,	475,505,	
476,506,	477,507,	478,508,	482,509,	
465,498,	483,510,	465,499,	465,500,	
485,511,	486,512,	487,513,	489,514,	
490,515,	495,516,	496,517,	497,518,	
498,519,	499,520,	500,521,	501,522,	
502,523,	503,524,	504,525,	505,526,	
506,527,	507,528,	509,529,	511,530,	
514,531,	516,532,	517,533,	518,534,	
519,535,	520,536,	521,537,	522,538,	
524,539,	529,540,	530,541,	532,542,	
533,543,	534,544,	535,545,	536,546,	
537,547,	538,548,	541,549,	542,550,	
543,551,	544,552,	545,553,	546,554,	
547,555,	548,556,	549,557,	552,558,	
553,559,	554,560,	555,561,	556,562,	
558,563,	559,564,	560,565,	562,566,	
563,567,	564,568,	566,569,	567,570,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-113,	0,		0,	
yycrank+-208,	yysvec+1,	0,	
yycrank+-294,	yysvec+2,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+5,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+-7,	0,		yyvstop+8,
yycrank+-406,	0,		yyvstop+10,
yycrank+76,	0,		yyvstop+12,
yycrank+0,	yysvec+10,	yyvstop+14,
yycrank+0,	0,		yyvstop+17,
yycrank+6,	0,		yyvstop+20,
yycrank+4,	0,		yyvstop+22,
yycrank+15,	0,		yyvstop+24,
yycrank+10,	0,		yyvstop+26,
yycrank+12,	0,		yyvstop+28,
yycrank+17,	0,		yyvstop+30,
yycrank+28,	0,		yyvstop+32,
yycrank+13,	0,		yyvstop+34,
yycrank+39,	0,		yyvstop+36,
yycrank+70,	0,		yyvstop+38,
yycrank+71,	0,		yyvstop+40,
yycrank+7,	0,		yyvstop+42,
yycrank+16,	0,		yyvstop+44,
yycrank+72,	0,		yyvstop+46,
yycrank+69,	0,		yyvstop+48,
yycrank+37,	0,		yyvstop+50,
yycrank+10,	0,		yyvstop+52,
yycrank+47,	0,		yyvstop+54,
yycrank+56,	0,		yyvstop+56,
yycrank+138,	0,		yyvstop+58,
yycrank+23,	0,		yyvstop+60,
yycrank+47,	0,		yyvstop+62,
yycrank+41,	0,		yyvstop+64,
yycrank+61,	0,		yyvstop+66,
yycrank+97,	0,		yyvstop+68,
yycrank+109,	0,		yyvstop+70,
yycrank+64,	0,		yyvstop+72,
yycrank+62,	0,		yyvstop+74,
yycrank+68,	0,		yyvstop+76,
yycrank+102,	0,		yyvstop+78,
yycrank+122,	0,		yyvstop+80,
yycrank+120,	0,		yyvstop+82,
yycrank+91,	0,		yyvstop+84,
yycrank+20,	0,		yyvstop+86,
yycrank+-413,	0,		yyvstop+88,
yycrank+11,	yysvec+22,	yyvstop+90,
yycrank+0,	0,		yyvstop+92,
yycrank+420,	0,		yyvstop+95,
yycrank+459,	0,		yyvstop+98,
yycrank+208,	yysvec+51,	yyvstop+100,
yycrank+67,	yysvec+51,	yyvstop+102,
yycrank+81,	yysvec+51,	yyvstop+104,
yycrank+137,	yysvec+51,	yyvstop+106,
yycrank+90,	0,		yyvstop+108,
yycrank+99,	yysvec+51,	yyvstop+110,
yycrank+176,	yysvec+51,	yyvstop+112,
yycrank+108,	yysvec+51,	yyvstop+114,
yycrank+198,	yysvec+51,	yyvstop+116,
yycrank+170,	yysvec+51,	yyvstop+118,
yycrank+-13,	yysvec+47,	yyvstop+120,
yycrank+0,	yysvec+6,	yyvstop+122,
yycrank+0,	yysvec+8,	0,	
yycrank+0,	0,		yyvstop+124,
yycrank+12,	0,		0,	
yycrank+0,	yysvec+10,	yyvstop+126,
yycrank+160,	0,		0,	
yycrank+183,	0,		0,	
yycrank+184,	0,		0,	
yycrank+168,	0,		0,	
yycrank+173,	0,		0,	
yycrank+188,	0,		0,	
yycrank+183,	0,		0,	
yycrank+202,	0,		0,	
yycrank+183,	0,		0,	
yycrank+190,	0,		0,	
yycrank+194,	0,		0,	
yycrank+199,	0,		0,	
yycrank+212,	0,		0,	
yycrank+203,	0,		0,	
yycrank+203,	0,		0,	
yycrank+211,	0,		0,	
yycrank+218,	0,		0,	
yycrank+220,	0,		0,	
yycrank+221,	0,		0,	
yycrank+226,	0,		0,	
yycrank+232,	0,		0,	
yycrank+223,	0,		0,	
yycrank+231,	0,		0,	
yycrank+224,	0,		0,	
yycrank+235,	0,		0,	
yycrank+250,	0,		0,	
yycrank+255,	0,		0,	
yycrank+252,	0,		0,	
yycrank+265,	0,		0,	
yycrank+255,	0,		0,	
yycrank+227,	0,		0,	
yycrank+226,	0,		0,	
yycrank+238,	0,		0,	
yycrank+235,	0,		0,	
yycrank+222,	0,		0,	
yycrank+228,	0,		0,	
yycrank+227,	0,		0,	
yycrank+232,	0,		0,	
yycrank+247,	0,		0,	
yycrank+234,	0,		0,	
yycrank+232,	0,		0,	
yycrank+228,	0,		0,	
yycrank+233,	0,		0,	
yycrank+234,	0,		0,	
yycrank+247,	0,		0,	
yycrank+243,	0,		0,	
yycrank+245,	0,		0,	
yycrank+271,	0,		0,	
yycrank+239,	0,		0,	
yycrank+236,	0,		0,	
yycrank+266,	0,		0,	
yycrank+240,	0,		0,	
yycrank+258,	0,		0,	
yycrank+278,	0,		0,	
yycrank+244,	0,		0,	
yycrank+265,	0,		0,	
yycrank+265,	0,		0,	
yycrank+362,	0,		0,	
yycrank+277,	0,		0,	
yycrank+287,	0,		0,	
yycrank+293,	0,		0,	
yycrank+300,	0,		0,	
yycrank+289,	0,		0,	
yycrank+297,	0,		0,	
yycrank+291,	0,		0,	
yycrank+310,	0,		0,	
yycrank+295,	0,		0,	
yycrank+308,	0,		0,	
yycrank+308,	0,		0,	
yycrank+329,	0,		0,	
yycrank+0,	0,		yyvstop+128,
yycrank+311,	0,		0,	
yycrank+0,	yysvec+47,	0,	
yycrank+0,	0,		yyvstop+130,
yycrank+352,	0,		0,	
yycrank+0,	0,		yyvstop+132,
yycrank+0,	yysvec+50,	yyvstop+134,
yycrank+0,	yysvec+51,	0,	
yycrank+343,	yysvec+51,	0,	
yycrank+368,	yysvec+51,	0,	
yycrank+368,	0,		0,	
yycrank+366,	yysvec+51,	0,	
yycrank+357,	yysvec+51,	0,	
yycrank+367,	yysvec+51,	0,	
yycrank+0,	0,		yyvstop+136,
yycrank+-447,	yysvec+47,	0,	
yycrank+0,	0,		yyvstop+139,
yycrank+379,	0,		0,	
yycrank+368,	0,		0,	
yycrank+372,	0,		0,	
yycrank+369,	0,		0,	
yycrank+371,	0,		0,	
yycrank+371,	0,		0,	
yycrank+378,	0,		0,	
yycrank+393,	0,		0,	
yycrank+389,	0,		0,	
yycrank+391,	0,		0,	
yycrank+402,	0,		0,	
yycrank+389,	0,		0,	
yycrank+406,	0,		0,	
yycrank+406,	0,		0,	
yycrank+408,	0,		0,	
yycrank+397,	0,		0,	
yycrank+398,	0,		0,	
yycrank+414,	0,		0,	
yycrank+419,	0,		0,	
yycrank+393,	0,		0,	
yycrank+420,	0,		0,	
yycrank+421,	0,		0,	
yycrank+414,	0,		0,	
yycrank+431,	0,		0,	
yycrank+428,	0,		0,	
yycrank+429,	0,		0,	
yycrank+433,	0,		0,	
yycrank+436,	0,		0,	
yycrank+428,	0,		0,	
yycrank+442,	0,		0,	
yycrank+433,	0,		0,	
yycrank+438,	0,		0,	
yycrank+412,	0,		0,	
yycrank+419,	0,		0,	
yycrank+416,	0,		0,	
yycrank+411,	0,		0,	
yycrank+404,	0,		0,	
yycrank+404,	0,		0,	
yycrank+414,	0,		0,	
yycrank+412,	0,		0,	
yycrank+407,	0,		0,	
yycrank+416,	0,		0,	
yycrank+413,	0,		0,	
yycrank+414,	0,		0,	
yycrank+420,	0,		0,	
yycrank+417,	0,		0,	
yycrank+420,	0,		0,	
yycrank+423,	0,		0,	
yycrank+423,	0,		0,	
yycrank+432,	0,		0,	
yycrank+439,	0,		0,	
yycrank+447,	0,		0,	
yycrank+432,	0,		0,	
yycrank+0,	0,		yyvstop+141,
yycrank+433,	0,		0,	
yycrank+436,	0,		0,	
yycrank+0,	0,		yyvstop+143,
yycrank+445,	0,		0,	
yycrank+452,	0,		0,	
yycrank+443,	0,		0,	
yycrank+457,	0,		0,	
yycrank+449,	0,		0,	
yycrank+442,	0,		0,	
yycrank+442,	0,		0,	
yycrank+451,	0,		0,	
yycrank+449,	0,		0,	
yycrank+452,	0,		0,	
yycrank+461,	0,		0,	
yycrank+447,	0,		0,	
yycrank+463,	0,		0,	
yycrank+464,	0,		0,	
yycrank+456,	0,		0,	
yycrank+468,	0,		0,	
yycrank+469,	0,		0,	
yycrank+469,	0,		0,	
yycrank+466,	0,		0,	
yycrank+472,	0,		0,	
yycrank+458,	0,		0,	
yycrank+468,	0,		0,	
yycrank+458,	0,		0,	
yycrank+0,	0,		yyvstop+145,
yycrank+474,	0,		0,	
yycrank+459,	0,		0,	
yycrank+472,	0,		0,	
yycrank+473,	0,		0,	
yycrank+463,	0,		0,	
yycrank+570,	0,		0,	
yycrank+503,	yysvec+51,	0,	
yycrank+508,	0,		0,	
yycrank+510,	yysvec+51,	0,	
yycrank+505,	0,		0,	
yycrank+521,	0,		0,	
yycrank+522,	0,		0,	
yycrank+517,	0,		0,	
yycrank+0,	0,		yyvstop+147,
yycrank+525,	0,		0,	
yycrank+527,	0,		0,	
yycrank+510,	0,		0,	
yycrank+510,	0,		0,	
yycrank+526,	0,		0,	
yycrank+0,	0,		yyvstop+149,
yycrank+501,	0,		0,	
yycrank+528,	0,		0,	
yycrank+522,	0,		0,	
yycrank+512,	0,		yyvstop+151,
yycrank+535,	0,		0,	
yycrank+517,	0,		0,	
yycrank+0,	0,		yyvstop+153,
yycrank+522,	0,		0,	
yycrank+535,	0,		0,	
yycrank+537,	0,		0,	
yycrank+0,	0,		yyvstop+155,
yycrank+540,	0,		0,	
yycrank+527,	0,		0,	
yycrank+528,	0,		0,	
yycrank+0,	0,		yyvstop+157,
yycrank+0,	0,		yyvstop+159,
yycrank+526,	0,		0,	
yycrank+530,	0,		0,	
yycrank+530,	0,		0,	
yycrank+0,	0,		yyvstop+161,
yycrank+538,	0,		0,	
yycrank+502,	0,		0,	
yycrank+499,	0,		0,	
yycrank+513,	0,		0,	
yycrank+515,	0,		0,	
yycrank+502,	0,		0,	
yycrank+507,	0,		0,	
yycrank+518,	0,		0,	
yycrank+0,	0,		yyvstop+163,
yycrank+519,	0,		0,	
yycrank+506,	0,		0,	
yycrank+521,	0,		0,	
yycrank+522,	0,		0,	
yycrank+0,	0,		yyvstop+165,
yycrank+0,	0,		yyvstop+167,
yycrank+0,	0,		yyvstop+169,
yycrank+527,	0,		0,	
yycrank+524,	0,		0,	
yycrank+525,	0,		0,	
yycrank+582,	0,		0,	
yycrank+526,	0,		0,	
yycrank+524,	0,		0,	
yycrank+0,	0,		yyvstop+171,
yycrank+529,	0,		0,	
yycrank+530,	0,		0,	
yycrank+535,	0,		0,	
yycrank+523,	0,		0,	
yycrank+528,	0,		0,	
yycrank+0,	0,		yyvstop+173,
yycrank+519,	0,		0,	
yycrank+522,	0,		0,	
yycrank+532,	0,		0,	
yycrank+527,	0,		0,	
yycrank+542,	0,		0,	
yycrank+0,	0,		yyvstop+175,
yycrank+536,	0,		0,	
yycrank+533,	0,		0,	
yycrank+543,	0,		0,	
yycrank+0,	0,		yyvstop+177,
yycrank+542,	0,		0,	
yycrank+543,	0,		0,	
yycrank+534,	0,		0,	
yycrank+530,	0,		0,	
yycrank+531,	0,		0,	
yycrank+532,	0,		0,	
yycrank+539,	0,		0,	
yycrank+546,	0,		0,	
yycrank+0,	0,		yyvstop+179,
yycrank+537,	0,		0,	
yycrank+544,	0,		0,	
yycrank+537,	0,		0,	
yycrank+0,	0,		yyvstop+181,
yycrank+0,	0,		yyvstop+183,
yycrank+589,	0,		0,	
yycrank+582,	yysvec+51,	0,	
yycrank+0,	0,		yyvstop+185,
yycrank+584,	0,		0,	
yycrank+588,	0,		0,	
yycrank+590,	0,		0,	
yycrank+577,	0,		0,	
yycrank+574,	0,		0,	
yycrank+0,	0,		yyvstop+187,
yycrank+596,	0,		0,	
yycrank+578,	0,		0,	
yycrank+586,	0,		0,	
yycrank+582,	0,		0,	
yycrank+0,	0,		yyvstop+189,
yycrank+600,	0,		0,	
yycrank+595,	0,		0,	
yycrank+598,	0,		0,	
yycrank+0,	0,		yyvstop+191,
yycrank+595,	0,		0,	
yycrank+604,	0,		0,	
yycrank+586,	0,		0,	
yycrank+595,	0,		0,	
yycrank+595,	0,		0,	
yycrank+595,	0,		0,	
yycrank+0,	0,		yyvstop+193,
yycrank+595,	0,		0,	
yycrank+596,	0,		0,	
yycrank+606,	0,		0,	
yycrank+0,	0,		yyvstop+195,
yycrank+0,	0,		yyvstop+197,
yycrank+565,	0,		0,	
yycrank+0,	0,		yyvstop+199,
yycrank+577,	0,		0,	
yycrank+570,	0,		0,	
yycrank+570,	0,		0,	
yycrank+567,	0,		0,	
yycrank+0,	0,		yyvstop+201,
yycrank+582,	0,		0,	
yycrank+583,	0,		0,	
yycrank+566,	0,		0,	
yycrank+0,	0,		yyvstop+203,
yycrank+585,	0,		0,	
yycrank+578,	0,		0,	
yycrank+579,	0,		0,	
yycrank+580,	0,		0,	
yycrank+586,	0,		0,	
yycrank+590,	0,		0,	
yycrank+575,	0,		0,	
yycrank+578,	0,		0,	
yycrank+593,	0,		0,	
yycrank+594,	0,		0,	
yycrank+599,	0,		0,	
yycrank+594,	0,		0,	
yycrank+584,	0,		0,	
yycrank+587,	0,		0,	
yycrank+0,	0,		yyvstop+205,
yycrank+599,	0,		0,	
yycrank+585,	0,		0,	
yycrank+600,	0,		0,	
yycrank+0,	0,		yyvstop+207,
yycrank+594,	0,		0,	
yycrank+0,	0,		yyvstop+209,
yycrank+587,	0,		0,	
yycrank+590,	0,		0,	
yycrank+605,	0,		0,	
yycrank+0,	0,		yyvstop+211,
yycrank+592,	0,		0,	
yycrank+600,	0,		0,	
yycrank+0,	0,		yyvstop+213,
yycrank+629,	0,		0,	
yycrank+640,	yysvec+51,	0,	
yycrank+0,	0,		yyvstop+215,
yycrank+0,	0,		yyvstop+217,
yycrank+632,	0,		0,	
yycrank+639,	0,		0,	
yycrank+640,	0,		0,	
yycrank+641,	0,		0,	
yycrank+633,	0,		0,	
yycrank+651,	0,		0,	
yycrank+0,	0,		yyvstop+219,
yycrank+641,	0,		0,	
yycrank+649,	0,		0,	
yycrank+637,	0,		0,	
yycrank+649,	0,		0,	
yycrank+638,	0,		0,	
yycrank+0,	0,		yyvstop+221,
yycrank+0,	0,		yyvstop+223,
yycrank+627,	0,		0,	
yycrank+0,	0,		yyvstop+225,
yycrank+646,	0,		0,	
yycrank+642,	0,		0,	
yycrank+636,	0,		0,	
yycrank+0,	0,		yyvstop+227,
yycrank+626,	0,		0,	
yycrank+0,	0,		yyvstop+229,
yycrank+0,	0,		yyvstop+231,
yycrank+0,	0,		yyvstop+233,
yycrank+0,	0,		yyvstop+235,
yycrank+0,	0,		yyvstop+237,
yycrank+626,	0,		0,	
yycrank+0,	0,		yyvstop+239,
yycrank+627,	0,		0,	
yycrank+624,	0,		0,	
yycrank+619,	0,		0,	
yycrank+630,	0,		0,	
yycrank+627,	0,		0,	
yycrank+0,	0,		yyvstop+241,
yycrank+616,	0,		0,	
yycrank+618,	0,		0,	
yycrank+636,	0,		0,	
yycrank+622,	0,		0,	
yycrank+629,	0,		0,	
yycrank+628,	0,		0,	
yycrank+0,	0,		yyvstop+243,
yycrank+0,	0,		yyvstop+245,
yycrank+627,	0,		0,	
yycrank+0,	0,		yyvstop+247,
yycrank+623,	0,		0,	
yycrank+0,	0,		yyvstop+249,
yycrank+632,	0,		0,	
yycrank+637,	0,		0,	
yycrank+0,	0,		yyvstop+251,
yycrank+642,	0,		0,	
yycrank+643,	0,		0,	
yycrank+0,	0,		yyvstop+253,
yycrank+663,	0,		0,	
yycrank+678,	0,		0,	
yycrank+669,	0,		0,	
yycrank+670,	0,		0,	
yycrank+660,	0,		0,	
yycrank+670,	0,		0,	
yycrank+676,	0,		0,	
yycrank+0,	0,		yyvstop+255,
yycrank+0,	0,		yyvstop+257,
yycrank+681,	0,		0,	
yycrank+670,	0,		0,	
yycrank+687,	0,		0,	
yycrank+0,	0,		yyvstop+259,
yycrank+671,	0,		0,	
yycrank+0,	0,		yyvstop+261,
yycrank+0,	0,		yyvstop+263,
yycrank+0,	0,		yyvstop+265,
yycrank+654,	0,		0,	
yycrank+654,	0,		0,	
yycrank+647,	0,		0,	
yycrank+0,	0,		yyvstop+267,
yycrank+648,	0,		0,	
yycrank+646,	0,		0,	
yycrank+660,	0,		0,	
yycrank+646,	0,		0,	
yycrank+0,	0,		yyvstop+269,
yycrank+0,	0,		yyvstop+271,
yycrank+0,	0,		yyvstop+273,
yycrank+652,	0,		0,	
yycrank+657,	0,		0,	
yycrank+0,	0,		yyvstop+275,
yycrank+665,	0,		0,	
yycrank+669,	0,		0,	
yycrank+670,	0,		0,	
yycrank+0,	0,		yyvstop+277,
yycrank+700,	0,		0,	
yycrank+701,	0,		0,	
yycrank+0,	0,		yyvstop+279,
yycrank+0,	0,		yyvstop+281,
yycrank+0,	0,		yyvstop+283,
yycrank+0,	0,		yyvstop+285,
yycrank+708,	0,		0,	
yycrank+687,	0,		0,	
yycrank+696,	0,		0,	
yycrank+697,	0,		0,	
yycrank+711,	0,		0,	
yycrank+702,	0,		0,	
yycrank+684,	0,		0,	
yycrank+664,	0,		0,	
yycrank+677,	0,		0,	
yycrank+668,	0,		0,	
yycrank+673,	0,		0,	
yycrank+683,	0,		0,	
yycrank+671,	0,		0,	
yycrank+0,	0,		yyvstop+287,
yycrank+672,	0,		0,	
yycrank+0,	0,		yyvstop+289,
yycrank+683,	0,		0,	
yycrank+0,	0,		yyvstop+291,
yycrank+0,	0,		yyvstop+293,
yycrank+719,	0,		0,	
yycrank+0,	0,		yyvstop+295,
yycrank+723,	0,		0,	
yycrank+725,	0,		0,	
yycrank+709,	0,		0,	
yycrank+714,	0,		0,	
yycrank+719,	0,		0,	
yycrank+729,	0,		0,	
yycrank+713,	0,		0,	
yycrank+0,	0,		yyvstop+297,
yycrank+680,	0,		0,	
yycrank+0,	0,		yyvstop+299,
yycrank+0,	0,		yyvstop+301,
yycrank+0,	0,		yyvstop+303,
yycrank+0,	0,		yyvstop+305,
yycrank+681,	0,		0,	
yycrank+682,	0,		0,	
yycrank+0,	0,		yyvstop+307,
yycrank+723,	0,		0,	
yycrank+724,	0,		0,	
yycrank+719,	0,		0,	
yycrank+719,	0,		0,	
yycrank+734,	0,		0,	
yycrank+737,	0,		0,	
yycrank+736,	0,		0,	
yycrank+0,	0,		yyvstop+309,
yycrank+0,	0,		yyvstop+311,
yycrank+705,	0,		0,	
yycrank+738,	0,		0,	
yycrank+732,	0,		0,	
yycrank+736,	0,		0,	
yycrank+726,	0,		0,	
yycrank+744,	0,		0,	
yycrank+743,	0,		0,	
yycrank+742,	0,		0,	
yycrank+714,	0,		0,	
yycrank+0,	0,		yyvstop+313,
yycrank+0,	0,		yyvstop+315,
yycrank+747,	0,		0,	
yycrank+747,	0,		0,	
yycrank+733,	0,		0,	
yycrank+735,	0,		0,	
yycrank+746,	0,		0,	
yycrank+0,	0,		yyvstop+317,
yycrank+741,	0,		0,	
yycrank+739,	0,		0,	
yycrank+739,	0,		0,	
yycrank+0,	0,		yyvstop+319,
yycrank+744,	0,		0,	
yycrank+742,	0,		0,	
yycrank+742,	0,		0,	
yycrank+0,	0,		yyvstop+321,
yycrank+748,	0,		0,	
yycrank+744,	0,		0,	
yycrank+0,	0,		yyvstop+323,
yycrank+0,	0,		yyvstop+325,
yycrank+0,	0,		yyvstop+327,
0,	0,	0};
struct yywork *yytop = yycrank+827;
struct yysvf *yybgin = yysvec+1;
unsigned char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
040 ,01  ,'"' ,'#' ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,'+' ,01  ,'+' ,'#' ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'#' ,'#' ,'#' ,01  ,01  ,01  ,01  ,
'#' ,'#' ,01  ,'#' ,'#' ,01  ,01  ,01  ,
'#' ,01  ,01  ,'#' ,01  ,01  ,01  ,'#' ,
01  ,01  ,01  ,01  ,'#' ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,'#' ,'#' ,'#' ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ident	"@(#)/usr/ccs/lib/lex/ncform.sl 1.1 1.0 10/31/92 45302 Univel"
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
#if defined(__cplusplus) || defined(__STDC__)
int yylook(void)
#else
yylook()
#endif
{
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			if(yylastch > &yytext[YYLMAX]) {
				fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
				exit(1);
			}
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					if(lsp > &yylstate[YYLMAX]) {
						fprintf(yyout,"Input string too long, limit %d\n",YYLMAX);
						exit(1);
					}
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
#if defined(__cplusplus) || defined(__STDC__)
int yyback(int *p, int m)
#else
yyback(p, m)
	int *p;
#endif
{
	if (p==0) return(0);
	while (*p) {
		if (*p++ == m)
			return(1);
	}
	return(0);
}
	/* the following are only used in the lex library */
#if defined(__cplusplus) || defined(__STDC__)
int yyinput(void)
#else
yyinput()
#endif
{
	return(input());
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyoutput(int c)
#else
yyoutput(c)
  int c; 
#endif
{
	output(c);
	}
#if defined(__cplusplus) || defined(__STDC__)
void yyunput(int c)
#else
yyunput(c)
   int c; 
#endif
{
	unput(c);
	}
