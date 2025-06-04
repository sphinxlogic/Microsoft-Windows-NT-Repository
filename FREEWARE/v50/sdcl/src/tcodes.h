
/*
 *	The nextstate and output tables for the DFA scanner are 
 *	declared and initialized in this file.  This file
 *	is used exclusively by scan_lex.c and hence is
 *	only included by that C source file.  I have included
 *	descriptions of the state that each row of the 2-D array
 *	corresponds to.   The columns correspond to the following
 *	character classes:
 *
 *        L   D   O   S   S   W   E   D   E   E   S
 *        E   I   N   L   T   H   O   Q   N   R   U
 *        T   G   E   A   A   I   L   U   D   R   B
 *        T   I   C   S   R   T       O   F       S
 *        E   T   H   H       E       T   I       T
 *        R       A           S       E   L       I
 *                R           P           E       T
 *                            A                   U
 *                            C                   T
 *                            E                   E
 *
 */

static char nextstate[][11] = {
	/* state 0, START STATE */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13, 16},

	/* state 1, ID */
	{ 1,  1,  3,  4,  3,  8,  9, 11, 10, 13, 16},

	/* state 2, INTEGER */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13, 16},

	/* state 3, SingleChar */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13, 16},

	/* state 4, Comment */
	{ 1,  2,  3,  4,  5,  8,  9, 11, 10, 13, 3},

	/* state 5, Comment */
	{ 5,  5,  5,  5,  6,  5,  5,  5, 13,  5, 5},

	/* state 6, Comment */
	{ 5,  5,  5,  7,  6,  5,  5,  5, 13,  5, 5},

	/* state 7, Comment */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13, 16},

	/* state 8, WhiteSpace */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13, 16},

	/* state 9, Newline */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13, 16},

	/* state  10, EOF */
	{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},

	/* state 11, String */
	{11, 11, 11, 11, 11, 11, 13, 12, 13, 13, 14},

	/* state 12, String */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13,  3},

	/* state 13, ERROR */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13,  3},

	/* state 14, One ' in quoted string */
	{11, 11, 11, 11, 11, 11, 13, 12, 13, 13, 15},

	/* state 15, Two 's in quoted string */
	{15, 15, 15, 15, 15, 15, 13, 15, 13, 13, 11},

	/* state 16, One 's outside of quoted string */
	{17, 17,  3,  4,  3,  8,  9, 11, 10, 13, 16},

	/* state 17, Inside a 's outside of quoted string */
	{17, 17,  3,  4,  3,  8,  9, 11, 10, 13,  18},

	/* state 18, Terminal ' */
	{ 1,  2,  3,  4,  3,  8,  9, 11, 10, 13, 3},
};


/*
 *
 *        L   D   O   S   S   W   E   D   E   E   S
 *        E   I   N   L   T   H   O   Q   N   R   U
 *        T   G   E   A   A   I   L   U   D   R   B
 *        T   I   C   S   R   T       O   F       S
 *        E   T   H   H       E       T   I       T
 *        R       A           S       E   L       I
 *                R           P           E       T
 *                            A                   U
 *                            C                   T
 *                            E                   E
 *
 */

static char output[][11] = {
	/* state 0, START STATE */
	{ 0,  0,  0,  0,  0,  0,  0,  0, 15,  0,  0},

	/* state 1, ID */
	{ 0,  0,  6,  6,  6,  6,  6,  6,  6,  6,  6},

	/* state 2, INTEGER */
	{18,  0, 18, 18, 18, 18, 18, 18, 18, 18, 18},

	/* state 3, SingleChar */
	{11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11},

	/* state 4, Comment */
	{11, 11, 11, 11,  0, 11, 11, 11,  0, 11, 11},

	/* state 5, Comment */
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},

	/* state 6, Comment */
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},

	/* state 7, Comment */
	{12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12},

	/* state 8, WhiteSpace */
	{13, 13, 13, 13, 13,  0, 13, 13, 13, 13, 13},

	/* state 9, Newline */
	{14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14},

	/* state  10, EOF */
	{15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},

	/* state 11, String */
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},

	/* state 12, String */
	{16, 16, 16, 16, 16, 16, 16,  0, 16, 16, 16},

	/* state 13, ERROR */
	{17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17},

	/* state 14, One ' in quoted string */
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},

	/* state 15, Two 's in quoted string */
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},

	/* state 16, One 's outside quoted string */
	{ 0,  0, 39, 39, 39, 39, 39, 39, 39, 39,  0},

	/* state 17, Inside a 's outside quoted string */
	{ 0,  0, 39, 39, 39, 39, 39, 39, 39, 39,  0},

	/* state 18, terminal ' */
	{39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39}
    };
