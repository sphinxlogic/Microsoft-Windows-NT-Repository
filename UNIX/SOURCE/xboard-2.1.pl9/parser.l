%a 5000
%o 5000
%e 1500
%k 2000
%p 4000
%{
/*
 * parser.l -- lex parser of algebraic chess moves for XBoard
 *
 * Original author:  Chris Sears
 * Enhancements (Version 2.0):  Tim Mann
 *
 * XBoard borrows its colors, icon and piece bitmaps from XChess
 * which was written and is copyrighted by Wayne Christopher.
 *
 * Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts.
 * Enhancements Copyright 1992 Free Software Foundation, Inc.
 *
 * The following terms apply to Digital Equipment Corporation's copyright
 * interest in XBoard:
 * ------------------------------------------------------------------------
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * ------------------------------------------------------------------------
 *
 * The following terms apply to the enhanced version of XBoard distributed
 * by the Free Software Foundation:
 * ------------------------------------------------------------------------
 * This file is part of XBOARD.
 *
 * XBOARD is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY.  No author or distributor accepts responsibility to anyone for
 * the consequences of using it or for whether it serves any particular
 * purpose or works at all, unless he says so in writing.  Refer to the XBOARD
 * General Public License for full details.
 *
 * Everyone is granted permission to copy, modify and redistribute XBOARD, but
 * only under the conditions described in the XBOARD General Public License. A
 * copy of this license is supposed to have been given to you along with
 * XBOARD so you can know your rights and responsibilities.  It should be in a
 * file named COPYING.  Among other things, the copyright notice and this
 * notice must be preserved on all copies.
 * ------------------------------------------------------------------------
 *
 * This parser handles all forms of promotion.
 * The parser resolves ambiguous moves by searching and check-testing.
 * It also parses comments of the form [anything] or (anything).
 */
#include "xboard.h"
#include <ctype.h>
#include <string.h>

#define False 0
#define True 1
#define NO_CONSTRAINT	-1
#undef YYLMAX
#define YYLMAX			2048
#define UNPUT_BUF_SIZE		YYLMAX
#undef input
#undef output
#undef unput

#undef WHITE_ON_MOVE
#define WHITE_ON_MOVE		((int) ((yyboardindex % 2) == 0))

extern Board	boards[MAX_MOVES];
int		yyboardindex;
extern int      xboardDebug;
extern FILE	*gameFileFP;
char		currentMoveString[YYLMAX];
char		unputBuffer[UNPUT_BUF_SIZE];
int		unputCount = 0;

static int RookSearch P((int *rank, int *file, ChessSquare piece,
			 int whiteOnMove, Board b,
			 int canmove, int rconstraint, int fconstraint));
static int BishopSearch P((int *rank, int *file, ChessSquare piece,
			   int whiteOnMove, Board b,
			   int canmove, int rconstraint, int fconstraint));
static int KnightSearch P((int *rank, int *file, ChessSquare piece,
			   int whiteOnMove, Board b,
			   int canmove, int rconstraint, int fconstraint));
static int KingSearch P((int *rank, int *file, ChessSquare piece,
			 int whiteOnMove, Board b,
			 int canmove, int rconstraint, int fconstraint));
static int QueenSearch P((int *rank, int *file, ChessSquare piece,
			  int whiteOnMove, Board b,
			  int canmove, int rconstraint, int fconstraint));
static int CheckTest P((int whiteOnMove, Board board,
			int rf, int ff, int rt, int ft));
ChessMove LegalityTest P((int whiteOnMove, Board board,
			  int rf, int ff, int rt, int ft, int promoChar));
int yywrap P((void));
static int input P((void));
static void output P((int ch));
static void unput P((int ch));
extern void CopyBoard P((Board to, Board from));

%}
%%

\([0-9]+:[0-9][0-9]\) {
    /* elapsed time indication, e.g. (0:12) */ 
    return (int) ElapsedTime;
}

"[--"[^\]]*"--]" {
    /* position diagram enclosed in [-- --] */
    return (int) PositionDiagram;
}

\[[^\]]*\]	{        			/* anything in [] */
    return (int) Comment; 
}

\([^()]*(\([^()]*\))+[^()]*\)  { 	  	/* nested () */
    return (int) Comment; 
}

\([^)][^)]+\)   { 				/* >=2 chars in () */
    return (int) Comment; 
}       

[RrBbNnQqKkPp][/]?[a-h][1-8][xX:-]?[a-h][1-8](=?\(?[RrBbNnQq]\)?)? {
    /*
     * Fully-qualified algebraic move, possibly with promotion
     */
    int skip1 = 0, skip2 = 0;
    ChessSquare piece;
    ChessMove result;
    
    /*
     * remove the / 
     */
    if (yytext[1] == '/') skip1 = 1;
    
    /*
     * remove the [xX:-] 
     */
    if ((yytext[3+skip1] == 'x') || (yytext[3+skip1] == 'X') ||
	(yytext[3+skip1] == '-') || (yytext[3+skip1] == ':')) skip2 = 1;
    
    currentMoveString[0] = yytext[1+skip1];
    currentMoveString[1] = yytext[2+skip1];
    currentMoveString[2] = yytext[3+skip1+skip2];
    currentMoveString[3] = yytext[4+skip1+skip2];
    currentMoveString[4] = NULLCHAR;
    
    if (yyleng-skip1-skip2 > 5) {
	if (yytext[yyleng-1] == ')') {
	    currentMoveString[4] = ToLower(yytext[yyleng-2]);
	} else {
	    currentMoveString[4] = ToLower(yytext[yyleng-1]);
	}
	currentMoveString[5] = NULLCHAR;
    }

    piece = boards[yyboardindex]
      [currentMoveString[1] - '1'][currentMoveString[0] - 'a'];
    if (ToLower(yytext[0]) != ToLower(PieceToChar(piece)))
      return (int) BadMove;

    result = LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
			  currentMoveString[1] - '1',
			  currentMoveString[0] - 'a',
			  currentMoveString[3] - '1',
			  currentMoveString[2] - 'a',
			  currentMoveString[4]);

    if (currentMoveString[4] == NULLCHAR &&
	(result == WhitePromotionQueen || result == BlackPromotionQueen)) {
	currentMoveString[4] = 'q';
	currentMoveString[5] = NULLCHAR;
    }

    return (int) result;
}

[a-h][1-8][xX:-]?[a-h][1-8](=?\(?[RrBbNnQq]\)?)?	{
    /*
     * Simple algebraic move, possibly with promotion
     */
    int skip = 0;
    ChessMove result;

    /*
     * remove the [xX:-] 
     */
    if ((yytext[2] == 'x') || (yytext[2] == 'X') ||
	(yytext[2] == '-') || (yytext[2] == ':')) skip = 1;

    currentMoveString[0] = yytext[0];
    currentMoveString[1] = yytext[1];
    currentMoveString[2] = yytext[2+skip];
    currentMoveString[3] = yytext[3+skip];
    currentMoveString[4] = NULLCHAR;

    if (yyleng-skip > 4) {
	if (yytext[yyleng-1] == ')') {
	    currentMoveString[4] = ToLower(yytext[yyleng-2]);
	} else {
	    currentMoveString[4] = ToLower(yytext[yyleng-1]);
	}
	currentMoveString[5] = NULLCHAR;
    }

    result = LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
			  currentMoveString[1] - '1',
			  currentMoveString[0] - 'a',
			  currentMoveString[3] - '1',
			  currentMoveString[2] - 'a',
			  currentMoveString[4]);

    if (currentMoveString[4] == NULLCHAR &&
	(result == WhitePromotionQueen || result == BlackPromotionQueen)) {
	currentMoveString[4] = 'q';
	currentMoveString[5] = NULLCHAR;
    }

    return (int) result;
}

[a-h][1-8](=?\(?[RrBbNnQq]\)?)?	{
    /*
     * Pawn move, possibly with promotion
     */
    int rank, file;
    ChessMove result;

    currentMoveString[0] = yytext[0];
    currentMoveString[1] = yytext[1];
    currentMoveString[2] = yytext[0];
    currentMoveString[3] = yytext[1];
    currentMoveString[4] = NULLCHAR;

    rank = yytext[1] - '1';
    file = yytext[0] - 'a';

    if (WHITE_ON_MOVE) {
	if (rank <= 0) return (int) BadMove;
	if (rank == 3 && boards[yyboardindex][2][file] == EmptySquare)
	  currentMoveString[1] -= 2;
	else
	  currentMoveString[1]--;
    } else {
	if (rank >= 7) return (int) BadMove;
	if (rank == 4 && boards[yyboardindex][5][file] == EmptySquare)
	  currentMoveString[1] += 2;
	else
	  currentMoveString[1]++;
    }

    if (yyleng > 2) {
	if (yytext[yyleng-1] == ')') {
	    currentMoveString[4] = ToLower(yytext[yyleng-2]);
	} else {
	    currentMoveString[4] = ToLower(yytext[yyleng-1]);
	}
	currentMoveString[5] = NULLCHAR;
    }

    result = LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
			  currentMoveString[1] - '1',
			  currentMoveString[0] - 'a',
			  currentMoveString[3] - '1',
			  currentMoveString[2] - 'a',
			  currentMoveString[4]);

    if (currentMoveString[4] == NULLCHAR &&
	(result == WhitePromotionQueen || result == BlackPromotionQueen)) {
	currentMoveString[4] = 'q';
	currentMoveString[5] = NULLCHAR;
    }

    return (int) result;
}


(ab|bc|cd|de|ef|fg|gh|hg|gf|fe|ed|dc|cb|ba|([a-h][xX:-][a-h]))(=?\(?[RrBbNnQq]\)?)? {
    /*
     * Pawn capture, possibly with promotion, possibly ambiguous
     */
    int i, file0, file1, skip = 0, found = 0;
    ChessMove result;

    /*
     * remove the [xX:-]
     */
    if ((yytext[1] == 'x') || (yytext[1] == 'X')
	|| (yytext[1] == ':') || (yytext[1] == '-')) skip = 1;

    file0 = yytext[0] - 'a';
    file1 = yytext[1+skip] - 'a';

    currentMoveString[0] = yytext[0];
    currentMoveString[2] = yytext[1+skip];
    if (yyleng-skip > 2) {
	if (yytext[yyleng-1] == ')')
	  currentMoveString[4] = ToLower(yytext[yyleng-2]);
	else
	  currentMoveString[4] = ToLower(yytext[yyleng-1]);
	currentMoveString[5] = NULLCHAR;
    } else {
	currentMoveString[4] = NULLCHAR;
    }

    if (WHITE_ON_MOVE) {
	for (i = 1; i < BOARD_SIZE - 1; i++) {
	    if (boards[yyboardindex][i][file0] == WhitePawn) {
		result = LegalityTest(True, boards[yyboardindex],
				      i, file0, i + 1, file1,
				      currentMoveString[4]);
		if (result != BadMove) {
		    found++;
		    if (found > 1) return (int) AmbiguousMove;
		    currentMoveString[1] = '1' + i;
		    currentMoveString[3] = '1' + i + 1;
		}
	    }
	}
    } else {
	for (i = 1; i < BOARD_SIZE - 1; i++) {
	    if (boards[yyboardindex][i][file0] == BlackPawn) {
		result = LegalityTest(False, boards[yyboardindex],
				      i, file0, i - 1, file1,
				      currentMoveString[4]);
		if (result != BadMove) {
		    found++;
		    if (found > 1) return (int) AmbiguousMove;
		    currentMoveString[1] = '1' + i;
		    currentMoveString[3] = '1' + i - 1;
		}
	    }
	}
    }

    if (currentMoveString[4] == NULLCHAR &&
	(result == WhitePromotionQueen || result == BlackPromotionQueen)) {
	currentMoveString[4] = 'q';
	currentMoveString[5] = NULLCHAR;
    }

    return (int) result;
}

[a-h][xX:]?[a-h][1-8](=?\(?[RrBbNnQq]\)?)?	{
    /*
     * unambiguously abbreviated Pawn capture, possibly with promotion
     */
    int skip = 0;
    ChessMove result;

    /*
     * remove the [xX:-]
     */
    if ((yytext[1] == 'x') || (yytext[1] == 'X')
	|| (yytext[1] == ':') || (yytext[1] == '-')) skip = 1;

    currentMoveString[0] = yytext[0];
    currentMoveString[2] = yytext[1+skip];
    currentMoveString[3] = yytext[2+skip];
    if (WHITE_ON_MOVE) {
	if (yytext[2+skip] == '1') return (int) BadMove;
	currentMoveString[1] = yytext[2+skip] - 1;
    } else {
	if (yytext[2+skip] == '8') return (int) BadMove;
	currentMoveString[1] = yytext[2+skip] + 1;
    }
    if (yyleng-skip > 3) {
	if (yytext[yyleng-1] == ')')
	  currentMoveString[4] = ToLower(yytext[yyleng-2]);
	else
	  currentMoveString[4] = ToLower(yytext[yyleng-1]);
	currentMoveString[5] = NULLCHAR;
    } else {
	currentMoveString[4] = NULLCHAR;
    }

    result = LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
			  currentMoveString[1] - '1',
			  currentMoveString[0] - 'a',
			  currentMoveString[3] - '1',
			  currentMoveString[2] - 'a',
			  currentMoveString[4]);

    if (currentMoveString[4] == NULLCHAR &&
	(result == WhitePromotionQueen || result == BlackPromotionQueen)) {
	currentMoveString[4] = 'q';
	currentMoveString[5] = NULLCHAR;
    }

    if (result != BadMove) return (int) result;

    /* Special case: improperly written en passant capture */
    if (WHITE_ON_MOVE) {
	if (currentMoveString[3] == '5') {
	    currentMoveString[1] = '5';
	    currentMoveString[3] = '6';
	} else {
	    return (int) BadMove;
	}
    } else {
	if (currentMoveString[3] == '4') {
	    currentMoveString[1] = '4';
	    currentMoveString[3] = '3';
	} else {
	    return (int) BadMove;
	}
    }

    result = LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
			  currentMoveString[1] - '1',
			  currentMoveString[0] - 'a',
			  currentMoveString[3] - '1',
			  currentMoveString[2] - 'a',
			  currentMoveString[4]);

    if (result == WhiteCapturesEnPassant || result == BlackCapturesEnPassant)
      return (int) result;
    else
      return (int) BadMove;
}

[RrBbNnQqKk][xX:-]?[a-h][1-8]  {
    /*
     * piece move, possibly ambiguous
     */
    int rank, file, found;
    ChessMove r, result = NormalMove;
    ChessSquare piece;

    if ((yytext[1] == 'x') || (yytext[1] == 'X')
	|| (yytext[1] == ':') || (yytext[1] == '-')) {
	currentMoveString[2] = yytext[2];
	currentMoveString[3] = yytext[3];
    } else {
	currentMoveString[2] = yytext[1];
	currentMoveString[3] = yytext[2];
    }
    currentMoveString[4] = NULLCHAR;

    rank = currentMoveString[3] - '1';
    file = currentMoveString[2] - 'a';

    switch (yytext[0]) {
      case 'R': case 'r':
	piece = WHITE_ON_MOVE ? WhiteRook : BlackRook;
	found = RookSearch(&rank, &file, piece,
			   WHITE_ON_MOVE, boards[yyboardindex],
			   True, NO_CONSTRAINT, NO_CONSTRAINT);
	break;
      case 'B': case 'b':
	piece = WHITE_ON_MOVE ? WhiteBishop : BlackBishop;
	found = BishopSearch(&rank, &file, piece,
			     WHITE_ON_MOVE, boards[yyboardindex],
			     True, NO_CONSTRAINT, NO_CONSTRAINT);
	break;
      case 'N': case 'n':
	piece = WHITE_ON_MOVE ? WhiteKnight : BlackKnight;
	found = KnightSearch(&rank, &file, piece,
			     WHITE_ON_MOVE, boards[yyboardindex],
			     True, NO_CONSTRAINT, NO_CONSTRAINT);
	break;
      case 'Q': case 'q':
	piece = WHITE_ON_MOVE ? WhiteQueen : BlackQueen;
	found = QueenSearch(&rank, &file, piece,
			    WHITE_ON_MOVE, boards[yyboardindex],
			    True, NO_CONSTRAINT, NO_CONSTRAINT);
	break;
      case 'K': case 'k':
	piece = WHITE_ON_MOVE ? WhiteKing : BlackKing;
	found = KingSearch(&rank, &file, piece,
			   WHITE_ON_MOVE, boards[yyboardindex],
			   True, NO_CONSTRAINT, NO_CONSTRAINT);
	if (found == 0 && (rank == 0 || rank == 7) &&
	    (file == 2 || file == 6)) {
	    /* Try castling */
	    if (boards[yyboardindex][rank][4] == piece) {
	        r = LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
				 rank, 4, rank, file, NULLCHAR);
		if (r != BadMove) {
		    found++;
		    result = r;
		    file = 4;
		}
	    }
	}
	break;
    }

    switch (found) {
      case 1: 
	currentMoveString[0] = file + 'a';
	currentMoveString[1] = rank + '1';
	return (int) result;
      case 0:
	return (int) BadMove;
      default:
	return (int) AmbiguousMove;
    }
}

[RrBbNnQqKk][a-h1-8][xX:-]?[a-h][1-8]	{
    /*
     * piece move with rank or file disambiguator
     */
    int rc, fc, rank, file, found;
    ChessMove r, result = NormalMove;
    ChessSquare piece;

    if ((yytext[2] == 'x') || (yytext[2] == 'X')
	|| (yytext[2] == ':') || (yytext[2] == '-')) {
	currentMoveString[2] = yytext[3];
	currentMoveString[3] = yytext[4];
	rank = yytext[4] - '1'; file = yytext[3] - 'a';
    } else {
	currentMoveString[2] = yytext[2];
	currentMoveString[3] = yytext[3];
	rank = yytext[3] - '1'; file = yytext[2] - 'a';
    }

    currentMoveString[4] = NULLCHAR;

    if (isalpha(yytext[1])) {
	fc = yytext[1] - 'a';
	rc = NO_CONSTRAINT;
    } else {
	fc = NO_CONSTRAINT;
	rc = yytext[1] - '1';
    }

    switch (yytext[0]) {
      case 'R': case 'r':
	piece = WHITE_ON_MOVE ? WhiteRook : BlackRook;
	found = RookSearch(&rank, &file, piece, WHITE_ON_MOVE,
			   boards[yyboardindex], True, rc, fc);
	break;
      case 'B': case 'b':
	piece = WHITE_ON_MOVE ? WhiteBishop : BlackBishop;
	found = BishopSearch(&rank, &file, piece, WHITE_ON_MOVE,
			     boards[yyboardindex], True, rc, fc);
	break;
      case 'N': case 'n':
	piece = WHITE_ON_MOVE ? WhiteKnight : BlackKnight;
	found = KnightSearch(&rank, &file, piece, WHITE_ON_MOVE,
			     boards[yyboardindex], True, rc, fc);
	break;
      case 'Q': case 'q':
	piece = WHITE_ON_MOVE ? WhiteQueen : BlackQueen;
	found = QueenSearch(&rank, &file, piece, WHITE_ON_MOVE,
			    boards[yyboardindex], True, rc, fc);
	break;
      case 'K': case 'k':
	piece = WHITE_ON_MOVE ? WhiteKing : BlackKing;
	found = KingSearch(&rank, &file, piece, WHITE_ON_MOVE,
			   boards[yyboardindex], True, rc, fc);
	if (found == 0 && (rank == 0 || rank == 7) &&
	    (file == 2 || file == 6)) {
	    /* Try castling */
	    if ((rc == NO_CONSTRAINT || rc == rank) &&
		(fc == NO_CONSTRAINT || fc == 4) &&
		(boards[yyboardindex][rank][4] == piece)) {
	        r = LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
				 rank, 4, rank, file, NULLCHAR);
		if (r != BadMove) {
		    found++;
		    result = r;
		    file = 4;
		}
	    }
	}
	break;
    }

    switch (found) {
      case 1: 
	currentMoveString[0] = file + 'a';
	currentMoveString[1] = rank + '1';
	return (int) result;
      case 0:
	return (int) BadMove;
      default:
	return (int) AmbiguousMove;
    }
}

000|0-0-0|ooo|OOO|o-o-o|O-O-O	{
    int rf, ff, rt, ft;

    if (WHITE_ON_MOVE) {
	strcpy(currentMoveString, "e1c1");
	rf = 0;
	ff = 4;
	rt = 0;
	ft = 2;
    } else{ 
	strcpy(currentMoveString, "e8c8");
	rf = 7;
	ff = 4;
	rt = 7;
	ft = 2;
    }
    return (int) LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
			      rf, ff, rt, ft, NULLCHAR);
}

00|0-0|oo|OO|o-o|O-O	{
    int rf, ff, rt, ft;

    if (WHITE_ON_MOVE) {
	strcpy(currentMoveString, "e1g1");
	rf = 0;
	ff = 4;
	rt = 0;
	ft = 6;
    } else {
	strcpy(currentMoveString, "e8g8");
	rf = 7;
	ff = 4;
	rt = 7;
	ft = 6;
    }
    return (int) LegalityTest(WHITE_ON_MOVE, boards[yyboardindex],
			      rf, ff, rt, ft, NULLCHAR);
}

[Rr](esign|ESIGN)([Ss]|[Ee][Dd])?  {
    if (WHITE_ON_MOVE)
      return (int) BlackWins;
    else
      return (int) WhiteWins;
}

[Ww](hite|HITE)?" "[Rr](esign|ESIGN)([Ss]|[Ee][Dd])?  {
    return (int) BlackWins;
}

[Bb](lack|LACK)?" "[Rr](esign|ESIGN)([Ss]|[Ee][Dd])?  {
    return (int) WhiteWins;
}

[Ww](hite|HITE)?" "[Aa](sserts|SSERTS)" "([aA]" ")?[Ww](in|IN)  {
    return (int) WhiteWins;
}

[Bb](lack|LACK)?" "[Aa](sserts|SSERTS)" "([aA]" ")?[Ww](in|IN)  {
    return (int) BlackWins;
}

[Ss](talemate|TALEMATE)  {
    return (int) GameIsDrawn;
}

([Cc](heck|HECK))?[Mm](ate|ATE)  {
    if (WHITE_ON_MOVE)
      return (int) WhiteWins;
    else
      return (int) BlackWins;
}

([Bb](lack|LACK)?|[Ww](hite|HITE)?)" "[Oo](ffers|FFERS)" "[Dd](raw|RAW)[Nn]?  {
    return (int) GameIsDrawn;
}

[Dd](raw|RAW)[Nn]?(" "[Bb][Yy])?(" "[Rr](epetition|EPETITION)|" "[Aa](gree|GREE)([Dd]|(ment|MENT))?) {
    return (int) GameIsDrawn;
}

[Dd](raw|RAW)[Nn]?(" (".*")")?  {
    return (int) GameIsDrawn;
}

[Ww](hite|HITE)?(" "[Ww][IiOo][Nn][Ss]?(" "[Oo][Nn]" "[Tt](ime|IME))?(" (".*")")?|" "[Mm](ates|ATES))? { 
    return (int) WhiteWins;
}

[Bb](lack|LACK)?(" "[Ww][IiOo][Nn][Ss]?(" "[Oo][Nn]" "[Tt](ime|IME))?(" (".*")")?|" "[Mm](ates|ATES))? { 
    return (int) BlackWins;
}

[Ww](hite|HITE)?" "[Ll][Oo][Ss]([Tt]|[Es][Ss])(" "[Oo][Nn]" "[Tt](ime|IME))? { 
    return (int) BlackWins;
}

[Bb](lack|LACK)?" "[Ll][Oo][Ss]([Tt]|[Es][Ss])(" "[Oo][Nn]" "[Tt](ime|IME))? { 
    return (int) WhiteWins;
}

1-0|"1 - 0"|"1/0"|"1 / 0"|"1:0"|"1 : 0"	{ 
    return (int) WhiteWins;
}
0-1|"0 - 1"|"0/1"|"0 / 1"|"0:1"|"0 : 1"	{ 
    return (int) BlackWins;
}
("1/2"|"1 / 2")(" "?[-:]" "?("1/2"|"1 / 2"))? {
    return (int) GameIsDrawn;
}

[1-9][0-9]*/[. \t\n]*[a-hNnPp]    {
    /* move numbers */
    /* note: above regular expression assumes game starts from
       conventional opening position, so that only pawn and
       knight moves are legal. */
    
    if ((yyleng == 1) && (yytext[0] == '1'))
      return (int) StartGame;
}

[Gg](nu|NU)" "?[Cc](hess|HESS).*[Gg](ame|AME) {
    return (int) StartGame;
}

[a-zA-Z0-9'-]+			{
        /* Skip random words */
}

.|"\n"				{
        /* Skip everything else */
}

%%


/*
    Test whether a piece of the given type attacks the square (*rank, *file)
    on board b, assuming it moves as a rook.  If canmove is True, the
    piece must be able to move legally to the square; otherwise we test
    only if it attacks the square.  rconstraint is either NO_CONSTRAINT or
    the rank the piece must start on; fconstraint is either NO_CONSTRAINT
    or the file the piece must start on.  Upon return, *rank and *file
    are modified to point to one of the pieces found, if any, and the
    function return value is the number of pieces found.
*/

static int RookSearch(rank, file, piece,
                      whiteOnMove, b, canmove, rconstraint, fconstraint)
     int *rank, *file, whiteOnMove, canmove, rconstraint, fconstraint;
     ChessSquare piece;
     Board b;
{
    int i, r = *rank, f = *file, found = 0;
    
    /*
     * Start from the target space and work outwards towards the piece.
     * This is necessary for check testing.
     */
    for (i = f + 1;; i++) {
	if (i >= BOARD_SIZE)
	  break;
	if ((b[r][i] != EmptySquare) && (b[r][i] != piece))
	  break;
	if (fconstraint != NO_CONSTRAINT && i != fconstraint)
	  continue;
	if (rconstraint != NO_CONSTRAINT && (r != rconstraint))
	  break;
	if (canmove)
	  if (CheckTest(whiteOnMove, b, r, i, r, f))
	    continue;
	if (b[r][i] == piece) {
	    *file = i;
	    found++;
	    break;
	}
    }
    
    for (i = f - 1;; i--) {
	if (i < 0)
	  break;
	if ((b[r][i] != EmptySquare) && (b[r][i] != piece))
	  break;
	if (fconstraint != NO_CONSTRAINT && (i != fconstraint))
	  continue;
	if (rconstraint != NO_CONSTRAINT && (r != rconstraint))
	  break;
	if (canmove)
	  if (CheckTest(whiteOnMove, b, r, i, r, f))
	    continue;
	if (b[r][i] == piece) {
	    *file = i;
	    found++;
	    break;
	}
    }
    
    for (i = r + 1;; i++) {
	if (i >= BOARD_SIZE)
	  break;
	if ((b[i][f] != EmptySquare) && (b[i][f] != piece))
	  break;
	if (fconstraint != NO_CONSTRAINT && (f != fconstraint))
	  break;
	if (rconstraint != NO_CONSTRAINT && (i != rconstraint))
	  continue;
	if (canmove)
	  if (CheckTest(whiteOnMove, b, i, f, r, f))
	    continue;
	if (b[i][f] == piece) {
	    *rank = i;
	    found++;
	    break;
	}
    }
    
    for (i = r - 1;; i--) {
	if (i < 0)
	  break;
	if ((b[i][f] != EmptySquare) && (b[i][f] != piece))
	  break;
	if (fconstraint != NO_CONSTRAINT && (f != fconstraint))
	  break;
	if (rconstraint != NO_CONSTRAINT && (i != rconstraint))
	  continue;
	if (canmove)
	  if (CheckTest(whiteOnMove, b, i, f, r, f))
	    continue;
	if (b[i][f] == piece) {
	    *rank = i;
	    found++;
	    break;
	}
    }
    
    return found;
}

/*
    Test whether a piece of the given type attacks the square (*rank, *file)
    on board b, assuming it moves as a bishop.  If canmove is True, the
    piece must be able to move legally to the square; otherwise we test
    only if it attacks the square.  rconstraint is either NO_CONSTRAINT or
    the rank the piece must start on; fconstraint is either NO_CONSTRAINT
    or the file the piece must start on.  Upon return, *rank and *file
    are modified to point to one of the pieces found, if any, and the
    function return value is the number of pieces found.
*/

static int BishopSearch(rank, file, piece, whiteOnMove, b,
			canmove, rconstraint, fconstraint)
     int *rank, *file, whiteOnMove, canmove, rconstraint, fconstraint;
     ChessSquare piece;
     Board b;
{
    int i, j, ii, jj, r = *rank, f = *file, found = 0;
    
    for (ii = -1; ii <= 1; ii += 2)
      for (jj = -1; jj <= 1; jj += 2) {
	  /*
	   * Start from the target space and work outwards towards the piece.
	   * This is necessary for check testing.
	   */
	  for (i = r + ii, j = f + jj;; i += ii, j += jj) {
	      if ((i < 0) || (i >= BOARD_SIZE) || (j < 0) || (j >= BOARD_SIZE))
		break;
	      if ((b[i][j] != EmptySquare) && (b[i][j] != piece))
		break;
	      if (fconstraint != NO_CONSTRAINT && (j != fconstraint))
		continue;
	      if (rconstraint != NO_CONSTRAINT && (i != rconstraint))
		continue;
	      if (canmove)
		if (CheckTest(whiteOnMove, b, i, j, r, f)) {
		    continue;
		}
	      if (b[i][j] == piece) {
		  *rank = i;
		  *file = j;
		  found++;
		  break;
	      }
	  }
      }	  

    return found;
}

/*
    Test whether a piece of the given type attacks the square (*rank, *file)
    on board b, assuming it moves as a knight.  If canmove is True, the
    piece must be able to move legally to the square; otherwise we test
    only if it attacks the square.  rconstraint is either NO_CONSTRAINT or
    the rank the piece must start on; fconstraint is either NO_CONSTRAINT
    or the file the piece must start on.  Upon return, *rank and *file
    are modified to point to one of the pieces found, if any, and the
    function return value is the number of pieces found.
*/

static int KnightSearch(rank, file, piece,
			whiteOnMove, b, canmove, rconstraint, fconstraint)
     int *rank, *file, whiteOnMove, canmove, rconstraint, fconstraint;
     ChessSquare piece;
     Board b;
{
    int i, j, s, rr, ff, r = *rank, f = *file, found = 0;

    for (i = -1; i <= 1; i += 2)
      for (j = -1; j <= 1; j += 2)
	for (s = 1; s <= 2; s++) {
	    rr = r + i*s;
	    ff = f + j*(3-s);
	    if (rr < 0 || rr > 7 || ff < 0 || ff > 7) continue;
	    if (rconstraint != NO_CONSTRAINT && rr != rconstraint) continue;
	    if (fconstraint != NO_CONSTRAINT && ff != fconstraint) continue;
	    if (b[rr][ff] == piece &&
		!(canmove && CheckTest(whiteOnMove, b, rr, ff, r, f))) {
		*rank = rr;
		*file = ff;
		found++;
	    }
	}
    return found;
}


/*
    Test whether a piece of the given type attacks the square (*rank, *file)
    on board b, assuming it moves as a king.  If canmove is True, the
    piece must be able to move legally to the square; otherwise we test
    only if it attacks the square.  rconstraint is either NO_CONSTRAINT or
    the rank the piece must start on; fconstraint is either NO_CONSTRAINT
    or the file the piece must start on.  Upon return, *rank and *file
    are modified to point to one of the pieces found, if any, and the
    function return value is the number of pieces found.
*/

static int KingSearch(rank, file, piece,
		      whiteOnMove, b, canmove, rconstraint, fconstraint)
     int *rank, *file, whiteOnMove, canmove, rconstraint, fconstraint;
     ChessSquare piece;
     Board b;
{
    int i, j, rr, ff, r = *rank, f = *file, found = 0;

    for (i = -1; i <= 1; i++)
      for (j = -1; j <= 1; j++) {
	  if (i == 0 && j == 0) continue;
	  rr = r + i;
	  ff = f + j;
	  if (rr < 0 || rr > 7 || ff < 0 || ff > 7) continue;
	  if (rconstraint != NO_CONSTRAINT && rr != rconstraint) continue;
	  if (fconstraint != NO_CONSTRAINT && ff != fconstraint) continue;
	  if (b[rr][ff] == piece &&
	      !(canmove && CheckTest(whiteOnMove, b, rr, ff, r, f))) {
	      *rank = rr;
	      *file = ff;
	      found++;
	  }
      }
    return found;
}


/*
    Test whether a piece of the given type attacks the square (*rank, *file)
    on board b, assuming it moves as a queen.  If canmove is True, the
    piece must be able to move legally to the square; otherwise we test
    only if it attacks the square.  rconstraint is either NO_CONSTRAINT or
    the rank the piece must start on; fconstraint is either NO_CONSTRAINT
    or the file the piece must start on.  Upon return, *rank and *file
    are modified to point to one of the pieces found, if any, and the
    function return value is the number of pieces found.
*/

static int QueenSearch(rank, file, piece,
		       whiteOnMove, b, canmove, rconstraint, fconstraint)
     int *rank, *file, whiteOnMove, canmove, rconstraint, fconstraint;
     ChessSquare piece;
     Board b;
{
    int rrook = *rank, frook = *file, nrook;
    int rbishop = *rank, fbishop = *file, nbishop;
    
    nrook = RookSearch(&rrook, &frook, piece,
		       whiteOnMove, b, canmove, rconstraint, fconstraint);
    nbishop = BishopSearch(&rbishop, &fbishop, piece,
			   whiteOnMove, b, canmove, rconstraint, fconstraint);
    if (nrook > 0) {
	*rank = rrook;
	*file = frook;
    } else if (nbishop > 0) {
	*rank = rbishop;
	*file = fbishop;
    }

    return nrook + nbishop;
}

/*
    Return True if moving from (rf, ff) to (rt, ft) is illegal
    because it would leave the player on move in check.
*/
static int CheckTest(whiteOnMove, board, rf, ff, rt, ft)
     int whiteOnMove;
     Board board;
     int rf, ff, rt, ft;
{
    int rk, fk, rank, file;
    Board b;
    
    CopyBoard(b, board);
    b[rt][ft] = b[rf][ff];
    b[rf][ff] = EmptySquare;
    
    for (rk = 0; rk < BOARD_SIZE; rk++) {
	for (fk = 0; fk < BOARD_SIZE; fk++) {
	    if (b[rk][fk] == (whiteOnMove ? WhiteKing : BlackKing)) {
		if (whiteOnMove) {
		    if (rk+1 <= 7) {
			if (fk-1 >= 0 && b[rk+1][fk-1] == BlackPawn)
			  return True;
			if (fk+1 <= 7 && b[rk+1][fk+1] == BlackPawn)
			  return True;
		    }
		} else {
		    if (rk-1 >= 0) {
			if (fk-1 >= 0 && b[rk-1][fk-1] == WhitePawn)
			  return True;
			if (fk+1 <= 7 && b[rk-1][fk+1] == WhitePawn)
			  return True;
		    }
		}
		rank = rk; file = fk;
		if (KnightSearch(&rank, &file,
				 whiteOnMove ? BlackKnight : WhiteKnight,
				 whiteOnMove, b, False,
				 NO_CONSTRAINT, NO_CONSTRAINT))
		  return True;
		rank = rk; file = fk;
		if (BishopSearch(&rank, &file,
				 whiteOnMove ? BlackBishop : WhiteBishop,
				 whiteOnMove, b, False,
				 NO_CONSTRAINT, NO_CONSTRAINT))
		  return True;
		rank = rk; file = fk;
		if (RookSearch(&rank, &file,
			       whiteOnMove ? BlackRook : WhiteRook,
			       whiteOnMove, b, False, 
			       NO_CONSTRAINT, NO_CONSTRAINT))
		  return True;
		rank = rk; file = fk;
		if (QueenSearch(&rank, &file,
				whiteOnMove ? BlackQueen : WhiteQueen,
				whiteOnMove, b, False, 
				NO_CONSTRAINT, NO_CONSTRAINT))
		  return True;
		rank = rk; file = fk;
		if (KingSearch(&rank, &file,
			       whiteOnMove ? BlackKing : WhiteKing,
			       whiteOnMove, b, False,
			       NO_CONSTRAINT, NO_CONSTRAINT))
		  return True;
	    }
	}
    }
    
    return False;
}

/*
    Test whether moving from (rf, ff) to (rt, ft) and promoting
    to promoChar is legal.  If the move is not a promotion, promoChar
    must be NULLCHAR.  If the move is a promotion, and promoChar is
    NULLCHAR, we assume the promotion is to a queen.
*/
ChessMove LegalityTest(whiteOnMove, board, rf, ff, rt, ft, promoChar)
     int whiteOnMove;
     Board board;
     int rf, ff, rt, ft;
     int promoChar;
{
    ChessSquare piece, dpiece;
    int rank, file, rookfile, dir;

    piece = board[rf][ff];
    if (whiteOnMove) {
	if ((int) piece < (int) WhitePawn || (int) piece > (int) WhiteKing)
	  return BadMove;
    } else {
	if ((int) piece < (int) BlackPawn || (int) piece > (int) BlackKing)
	  return BadMove;
    }

    switch (piece) {
      case WhitePawn:
	if (CheckTest(whiteOnMove, board, rf, ff, rt, ft)) return BadMove;
	if (ft == ff) {
	    /* Non-capture */
	    if (rt == rf + 1) {
		if (board[rt][ft] != EmptySquare) return BadMove;
	    } else {
		if ((rf != 1) || (rt != 3) ||
		    (board[rf + 1][ft] != EmptySquare) ||
		    (board[rt][ft] != EmptySquare)) return BadMove;
	    }
	} else {
	    /* Capture */
	    if ((ft != ff - 1) && (ft != ff + 1)) return BadMove;
	    if (rt != rf + 1) return BadMove;
	    dpiece = board[rt][ft];
	    if (dpiece == EmptySquare && rf == 4 &&
		board[rf][ft] == BlackPawn &&
		board[rt + 1][ft] == EmptySquare)
	      /* For now, don't check whether black just moved */
	      return WhiteCapturesEnPassant;
	    if ((int) dpiece < (int) BlackPawn ||
		(int) dpiece > (int) BlackKing)
	      return BadMove;
	}
	switch (promoChar) {
	  case NULLCHAR:
	    if (rt == 7) return WhitePromotionQueen;
	    return NormalMove;
	  case 'n':
	  case 'N':
	    if (rt != 7) return BadMove;
	    return WhitePromotionKnight;
	  case 'b':
	  case 'B':
	    if (rt != 7) return BadMove;
	    return WhitePromotionBishop;
	  case 'r':
	  case 'R':
	    if (rt != 7) return BadMove;
	    return WhitePromotionRook;
	  case 'q':
	  case 'Q':
	    if (rt != 7) return BadMove;
	    return WhitePromotionQueen;
	  default:
	    return BadMove;
	}
      case BlackPawn:
	if (CheckTest(whiteOnMove, board, rf, ff, rt, ft)) return BadMove;
	if (ft == ff) {
	    /* Non-capture */
	    if (rt == rf - 1) {
		if (board[rt][ft] != EmptySquare) return BadMove;
	    } else {
		if ((rf != 6) || (rt != 4) ||
		    (board[rf - 1][ft] != EmptySquare) ||
		    (board[rt][ft] != EmptySquare)) return BadMove;
	    }
	} else {
	    /* Capture */
	    if ((ft != ff - 1) && (ft != ff + 1)) return BadMove;
	    if (rt != rf - 1) return BadMove;
	    dpiece = board[rt][ft];
	    if (dpiece == EmptySquare && rf == 3 &&
		board[rf][ft] == WhitePawn &&
		board[rt - 1][ft] == EmptySquare)
	      /* For now, don't check whether white just moved */
	      return BlackCapturesEnPassant;
	    if ((int) dpiece < (int) WhitePawn ||
		(int) dpiece > (int) WhiteKing)
	      return BadMove;
	}
	switch (promoChar) {
	  case NULLCHAR:
	    if (rt == 0) return BlackPromotionQueen;
	    return NormalMove;
	  case 'n':
	  case 'N':
	    if (rt != 0) return BadMove;
	    return BlackPromotionKnight;
	  case 'b':
	  case 'B':
	    if (rt != 0) return BadMove;
	    return BlackPromotionBishop;
	  case 'r':
	  case 'R':
	    if (rt != 0) return BadMove;
	    return BlackPromotionRook;
	  case 'q':
	  case 'Q':
	    if (rt != 0) return BadMove;
	    return BlackPromotionQueen;
	  default:
	    return BadMove;
	}
      case WhiteKnight:
      case BlackKnight:
	rank = rt; file = ft;
	if (KnightSearch(&rank, &file, piece,
			 whiteOnMove, board, True, rf, ff) != 1)
	  return BadMove;
	break;
      case WhiteBishop:
      case BlackBishop:
	rank = rt; file = ft;
	if (BishopSearch(&rank, &file, piece,
			 whiteOnMove, board, True, rf, ff) != 1)
	  return BadMove;
	break;
      case WhiteRook:
      case BlackRook:
	rank = rt; file = ft;
	if (RookSearch(&rank, &file, piece,
		       whiteOnMove, board, True, rf, ff) != 1)
	  return BadMove;
	break;
      case WhiteQueen:
      case BlackQueen:
	rank = rt; file = ft;
	if (QueenSearch(&rank, &file, piece,
			whiteOnMove, board, True, rf, ff) != 1)
	  return BadMove;
	break;
      case WhiteKing:
      case BlackKing:
	if (ff == 4 && (ft == 6 || ft == 2)) {
	    /* Test for legal castling move */
	    if ((rt != rf) ||
		(whiteOnMove && rf != 0) ||
		(!whiteOnMove && rf != 7))
	      return BadMove;
	    if (ft == 2) {
		dir = -1;
		rookfile = 0;
	    } else {
		dir = 1;
		rookfile = 7;
	    }
	    file = ff + dir;
	    while (file != rookfile) {
		if (board[rt][file] != EmptySquare) return BadMove;
		file += dir;
	    }
	    if (board[rt][rookfile] != (whiteOnMove ? WhiteRook : BlackRook))
	      return BadMove;
	    if (CheckTest(whiteOnMove, board, rf, ff, rf, ff) ||
		CheckTest(whiteOnMove, board, rf, ff, rf, ff + dir) ||
		CheckTest(whiteOnMove, board, rf, ff, rt, ft))
	      return BadMove;
	    /* For now, we don't check if the king or rook has moved */
	    if (whiteOnMove) {
		if (ft == 2)
		  return WhiteQueenSideCastle;
		else
		  return WhiteKingSideCastle;
	    } else {
		if (ft == 2)
		  return BlackQueenSideCastle;
		else
		  return BlackKingSideCastle;
	    }
	}
	rank = rt; file = ft;
	if (KingSearch(&rank, &file, piece,
		       whiteOnMove, board, True, rf, ff) != 1)
	  return BadMove;
	break;
      case EmptySquare:
      default:
	return BadMove;
    }
    if (promoChar != NULLCHAR) return BadMove;
    dpiece = board[rt][ft];
    if (dpiece == EmptySquare) return NormalMove;
    if (whiteOnMove) {
	if ((int) dpiece < (int) BlackPawn || (int) dpiece > (int) BlackKing)
	  return BadMove;
    } else {
	if ((int) dpiece < (int) WhitePawn || (int) dpiece > (int) WhiteKing)
	  return BadMove;
    }	
    return NormalMove;
}


int yywrap()
{
    return True;
}

static char *StringToLex;

static int input()
{
    int ret;
    
    if (StringToLex != NULL) {
	ret = *StringToLex;
	if (ret == NULLCHAR)
	  ret = EOF;
	else
	  StringToLex++;
    } else if (unputCount > 0) {
	ret = unputBuffer[--unputCount];
    } else {
	ret = fgetc(gameFileFP);
    }    

    if (ret == EOF) 
      return 0;
    else
      return ret;
}

static void output(ch)
     int ch;
{
    fprintf(stderr, "PARSER BUG: unmatched character '%c' (0%o)\n",
	    ch, ch);
}

static void unput(ch)
     int ch;
{
    if (ch == 0) return;
    if (StringToLex != NULL) {
	StringToLex--;
    } else {
	if (unputCount >= UNPUT_BUF_SIZE)
	  fprintf(stderr, "PARSER BUG: unput buffer overflow '%c' (0%o)\n",
		  ch, ch);
	unputBuffer[unputCount++] = ch;
    }
}

void yynewfile()
{
    unputCount = 0;
}

/* Parse a move from the given string */
/* Return a pointer to the first unparsed character 
   in "next" if it is non-NULL */
ChessMove yylexstr(boardIndex, s, next)
     int boardIndex;
     char *s;
     char **next;
{
    ChessMove ret;
    
    StringToLex = s;
    yyboardindex = boardIndex;
    ret = (ChessMove) yylex();
    if (next != NULL) *next = StringToLex;
    StringToLex = NULL;
    return ret;
}
