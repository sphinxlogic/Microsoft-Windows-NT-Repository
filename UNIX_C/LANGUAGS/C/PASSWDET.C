25-Nov-85 20:37:22-MST,13340;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Mon 25 Nov 85 20:36:54-MST
Received: from usenet by TGR.BRL.ARPA id a029511; 25 Nov 85 21:54 EST
From: jbn@wdl1.uucp
Newsgroups: net.sources
Subject: Obvious password detector (reposted)
Message-ID: <879@wdl1.UUCP>
Date: 23 Nov 85 06:25:37 GMT
Sender: notes@wdl1.uucp
Nf-ID: #N:wdl1:64200002:000:12602
Nf-From: wdl1!jbn    Nov 22 18:41:00 1985
To:       unix-sources@BRL-TGR.ARPA

/*
	Obvious password detection subroutine.

	Call:

		char word[];
		char *obvious(word);

	Returns 0 if password is acceptable.
	Returns a pointer to a static message if the password is unacceptable.

	The algorithm used requires that the length of the password be
	within configurable length limits, and that the password not
	have triplet statistics similar to those associated with words
	in the English language.  This is an inversion of a technique
	used to find spelling errors without a full dictionary.  No
	word in the UNIX spelling dictionary will pass this algorithm.

	Users should be advised to pick a password composed of random
	letters and numbers.  Eight randomly chosen letters will
	pass the algorithm over 95% of the time.  A word prefaced
	by a digit will not pass the algorithm, although a word
	with a digit in the middle usually will.  Two words run
	together will often pass.

	In the interest of greater network security, this algorithm
	is offered to the ARPANET community as a proposed standard technique
	for eliminating obvious passwords.

				John Nagle
				Ford Aerospace and Communications Corporation
				Western Development Laboratories
				3939 Fabian Way
				Palo Alto, CA  94303
				
				1/16/84
*/
/*
		Table of triple usage in text

		24511 words were used to make this table.
		The words came from the files:
			/usr/dict/words
			obvpats.lp

		The table is 30 percent populated.
*/
long obvtab[27][27] =
{
    {	0X00100001, 0X00040000, 0X00040000, 0X00009000, 0X00808020, 0X00140000, 
	0X00000000, 0X00080000, 0X00000002, 0X00000000, 0X00000000, 0X00000000, 
	0X00001020, 0X00000000, 0X00000010, 0X00000020, 0X00000000, 0X00000000, 
	0X00000030, 0X00300100, 0X00000100, 0X00000000, 0X00000020, 0X00000000, 
	0X00000000, 0X00000000, 0X00000000 },
    {	0X00090000, 0X000c708a, 0X022cd73e, 0X023ffbbe, 0X02fffffe, 0X002cd0da, 
	0X023cdae2, 0X02adf3b6, 0X0024f222, 0X00dd7efa, 0X00008022, 0X02b5937a, 
	0X06fdfbfe, 0X02bdf37e, 0X07ffdfff, 0X003c3248, 0X023dfb76, 0X00200000, 
	0X07fffffe, 0X02bbfbbe, 0X06bcfb6f, 0X055f7dfc, 0X02609232, 0X021cfbf6, 
	0X02999222, 0X00bdf3ff, 0X06308232 },
    {	0X00080000, 0X07fdfbfc, 0X022c9226, 0X00008012, 0X00248222, 0X07dffbfe, 
	0X00200000, 0X00000000, 0X00208020, 0X047ffdfe, 0X00000020, 0X00000000, 
	0X02208222, 0X00800220, 0X0000a022, 0X03fdfffe, 0X00048000, 0X00000000, 
	0X02208222, 0X00308228, 0X00049062, 0X071efefc, 0X00000220, 0X00000020, 
	0X00000000, 0X049d11a2, 0X00000000 },
    {	0X00080080, 0X02fffffe, 0X000c0020, 0X0024932a, 0X0000802a, 0X06bdf3fe, 
	0X00000002, 0X00248220, 0X02bdf3fe, 0X007df0fe, 0X00000000, 0X02bdfffe, 
	0X02208222, 0X00200222, 0X00008222, 0X07fffffe, 0X00000102, 0X00200000, 
	0X02308222, 0X00188200, 0X02aca262, 0X003df27e, 0X00000004, 0X00000000, 
	0X00000000, 0X001dd08e, 0X00000022 },
    {	0X00088022, 0X06fd7bfc, 0X02249222, 0X00209102, 0X02249332, 0X07fff7fe, 
	0X00249282, 0X02a43a22, 0X00008222, 0X05fdf3fe, 0X00208022, 0X00000220, 
	0X02208222, 0X00208222, 0X00008223, 0X07fdf3fe, 0X00049322, 0X00200000, 
	0X02208222, 0X0091bb2a, 0X00008302, 0X043dfafe, 0X00008222, 0X02048222, 
	0X00000000, 0X00987aef, 0X00200000 },
    {	0X004c1030, 0X04ff79dc, 0X023c9226, 0X023c9b3a, 0X02bdf3fe, 0X04ddfbfe, 
	0X023c92fa, 0X0224f3e6, 0X0224a222, 0X045d7afa, 0X00208022, 0X0218ca36, 
	0X02fdfbfe, 0X02a9f22e, 0X06ffffff, 0X05fd73d8, 0X02bdb32e, 0X00200000, 
	0X07fffffe, 0X02fbfbbe, 0X06bdfb6e, 0X005d78bc, 0X066c8222, 0X029df37e, 
	0X0333832a, 0X008df97f, 0X06648222 },
    {	0X00080000, 0X07bc7bfe, 0X00000222, 0X0000000a, 0X00008002, 0X02fc7abe, 
	0X022cb366, 0X00000102, 0X00000022, 0X055c70fe, 0X00008000, 0X00000002, 
	0X02208222, 0X0000000a, 0X00000200, 0X03edf2be, 0X00000008, 0X00000000, 
	0X02208222, 0X00118922, 0X028ca32e, 0X041c74b8, 0X00000000, 0X00000002, 
	0X00000000, 0X00000000, 0X00000000 },
    {	0X00080000, 0X06fdf3fc, 0X00008222, 0X00000000, 0X00008022, 0X02fdf3fe, 
	0X00240220, 0X020d93a2, 0X02bddffe, 0X047df0fe, 0X00000002, 0X00008200, 
	0X02208222, 0X02300222, 0X00318226, 0X02fdf3fe, 0X00009220, 0X00000000, 
	0X02208222, 0X00919b06, 0X00048302, 0X061cf2ee, 0X00000000, 0X02048022, 
	0X00000000, 0X00056002, 0X00000002 },
    {	0X00080000, 0X06fdfbfe, 0X02249222, 0X00048102, 0X00048202, 0X07fff2ff, 
	0X00208202, 0X00008022, 0X00008122, 0X047dfcfe, 0X00000800, 0X00008220, 
	0X02248222, 0X0008a222, 0X0028c222, 0X06fdfeff, 0X00209202, 0X00200000, 
	0X02249232, 0X00108302, 0X02bdf3fa, 0X071d71fe, 0X00000200, 0X00008322, 
	0X00000000, 0X011d32be, 0X00000000 },
    {	0X00483010, 0X05dd7bbc, 0X022cf226, 0X023cdb2a, 0X02bdf6b6, 0X04fd79fc, 
	0X0234d262, 0X063df3a2, 0X00000222, 0X00000202, 0X00208002, 0X02208a22, 
	0X02fdbbfe, 0X022df36e, 0X07ffffff, 0X05fd70be, 0X02bdb766, 0X00200000, 
	0X02fdfafe, 0X02ffffff, 0X06bdfb6f, 0X000c7008, 0X02248222, 0X00008202, 
	0X023082e2, 0X00000002, 0X04608022 },
    {	0X00080000, 0X07ed7bbc, 0X00000000, 0X00000000, 0X00000000, 0X009cd57a, 
	0X00000000, 0X00000000, 0X00000000, 0X005070c4, 0X00000400, 0X00001000, 
	0X00000000, 0X00000000, 0X00000000, 0X02fc5bae, 0X00000000, 0X00000000, 
	0X00000000, 0X00000000, 0X00000000, 0X015d7e96, 0X00000000, 0X00000000, 
	0X00000000, 0X00000000, 0X00000000 },
    {	0X00080000, 0X07fdfff4, 0X00248222, 0X00040302, 0X00048022, 0X02fdf3f6, 
	0X00208222, 0X00048000, 0X0014a022, 0X00ddf8fe, 0X00000002, 0X0000ca22, 
	0X0220a222, 0X00000022, 0X00208222, 0X01fdfb7a, 0X00009202, 0X00000000, 
	0X02208222, 0X02b1abae, 0X00048302, 0X029d7110, 0X00000000, 0X00008322, 
	0X00000000, 0X00ac9522, 0X00000000 },
    {	0X00088000, 0X07fffbfe, 0X02249222, 0X0234b322, 0X02bdf36e, 0X07fdfffe, 
	0X00bc93e6, 0X00048222, 0X00008222, 0X05fffdfe, 0X00008000, 0X0288d322, 
	0X02bdb7fe, 0X0228e332, 0X00208020, 0X07fffffe, 0X021ca362, 0X00000000, 
	0X02208022, 0X0299ab26, 0X06eca326, 0X055dfefe, 0X00008222, 0X02008322, 
	0X00000000, 0X00bdfbfe, 0X00000022 },
    {	0X00080000, 0X07fdffbc, 0X002dd236, 0X0021fbfe, 0X00008020, 0X07ddfbfe, 
	0X00209220, 0X00000000, 0X00208020, 0X053efcfa, 0X00000000, 0X00000000, 
	0X00008222, 0X0220a222, 0X02208222, 0X07fdfbfe, 0X023c9be6, 0X00200000, 
	0X00088002, 0X0239a37c, 0X00048120, 0X041dfaf8, 0X00000020, 0X00008222, 
	0X00000000, 0X001cd0a8, 0X00000000 },
    {	0X00180000, 0X06fdfbfe, 0X002c922a, 0X02349b22, 0X02bffffe, 0X07fff3ff, 
	0X00249222, 0X02bdbb76, 0X02208222, 0X057ffbfe, 0X00208022, 0X021d33ea, 
	0X02008222, 0X00008222, 0X0228c222, 0X07fdf2fe, 0X00209220, 0X00200000, 
	0X0220822a, 0X02fdfbfe, 0X06adf3ee, 0X043dfafe, 0X02208222, 0X00048322, 
	0X00100200, 0X01ffb11e, 0X02808222 },
    {	0X0008113c, 0X017f79da, 0X02fcd77e, 0X02349b2a, 0X02adfbfe, 0X037ef35c, 
	0X0234b262, 0X02bcf7f6, 0X0204f222, 0X011c58fe, 0X00008222, 0X02a99b2e, 
	0X06fdfbfe, 0X02aff36e, 0X07ffffff, 0X045df9dc, 0X02bffbae, 0X00200000, 
	0X06fffbfe, 0X02fbbbae, 0X02bdf3ee, 0X055f73ff, 0X02008a22, 0X06bdfb7e, 
	0X02d043ea, 0X001dd37e, 0X06008222 },
    {	0X00080000, 0X07fffabc, 0X002c9022, 0X00008100, 0X00040002, 0X07fdfefe, 
	0X00241220, 0X00040002, 0X023cd232, 0X057ff8fa, 0X00000002, 0X00000220, 
	0X02208222, 0X00208022, 0X00008020, 0X03fdfafa, 0X022db322, 0X00040000, 
	0X02208222, 0X02b0bb2a, 0X0224832a, 0X041d7afe, 0X00000008, 0X00048202, 
	0X00000000, 0X00949b86, 0X00000000 },
    {	0X00080000, 0X00100000, 0X00000000, 0X00000000, 0X00000000, 0X00000010, 
	0X00000000, 0X00000000, 0X00000000, 0X00000000, 0X00000000, 0X00000000, 
	0X00000000, 0X00000000, 0X00000000, 0X00000000, 0X00000000, 0X00020000, 
	0X00080000, 0X00000000, 0X00000000, 0X02008222, 0X00000000, 0X00000020, 
	0X00000000, 0X00000000, 0X00000000 },
    {	0X00080010, 0X07fffffc, 0X022c9222, 0X023c9b2a, 0X02bdd366, 0X07ffffff, 
	0X00249222, 0X022cb322, 0X02208222, 0X07fbfdfe, 0X00208020, 0X029df336, 
	0X02b98236, 0X0239936e, 0X02b9ab66, 0X07fffffe, 0X023cb322, 0X00200000, 
	0X02248322, 0X02f3fb7e, 0X06bcf3ee, 0X01ddf2fe, 0X02208222, 0X00008222, 
	0X00000020, 0X0099f0be, 0X00008222 },
    {	0X00081000, 0X03fdfffe, 0X02248222, 0X0224b323, 0X00248242, 0X07fffbfe, 
	0X02208222, 0X002c8220, 0X02f5fa6e, 0X057ff8fe, 0X00200000, 0X0224a222, 
	0X02208222, 0X02208222, 0X02208223, 0X06f5f7fe, 0X022c9326, 0X00200000, 
	0X00288222, 0X02bdf36e, 0X02edf7ff, 0X041df2fe, 0X00010220, 0X00248222, 
	0X00000000, 0X001d78bc, 0X00000000 },
    {	0X001c1010, 0X03fdfbfc, 0X00248222, 0X02249102, 0X00048000, 0X03fdfbfe, 
	0X00249222, 0X00248022, 0X02bffbfe, 0X047ff8fe, 0X00000000, 0X02000222, 
	0X02208222, 0X00208222, 0X00308223, 0X07fdfafe, 0X00049222, 0X00000000, 
	0X02a08222, 0X02f1ab7e, 0X027c9be2, 0X031df2fe, 0X00000202, 0X01208322, 
	0X00000000, 0X01ad328e, 0X000db2a2 },
    {	0X004c1010, 0X02dd7bdc, 0X027db736, 0X023c9b2a, 0X06ac92b6, 0X063df5f6, 
	0X0030b042, 0X002cf3a2, 0X00004002, 0X057dd0be, 0X00208202, 0X00048a30, 
	0X06fdfafe, 0X007bf36e, 0X02bdfbfe, 0X023d5210, 0X02bdbbfe, 0X00200000, 
	0X06fffbfe, 0X023bdbbe, 0X06bcf3be, 0X00202000, 0X00840220, 0X00000002, 
	0X00309220, 0X00601022, 0X04208022 },
    {	0X00080000, 0X003d7fbc, 0X00004000, 0X00000000, 0X00000002, 0X039dfbfe, 
	0X00000000, 0X00000000, 0X00000000, 0X057cdafe, 0X00000000, 0X00000100, 
	0X00008002, 0X00000000, 0X00000000, 0X02bcfa88, 0X00000000, 0X00000000, 
	0X00008020, 0X00000800, 0X00000000, 0X000c1000, 0X02400220, 0X01000000, 
	0X00000000, 0X00800200, 0X00008000 },
    {	0X00080000, 0X077d7bde, 0X00248222, 0X00208102, 0X02248220, 0X025d53bf, 
	0X00209202, 0X00000200, 0X02208222, 0X055d70f8, 0X00000000, 0X02804200, 
	0X02180222, 0X00000222, 0X02bd03fc, 0X00fdf864, 0X00209222, 0X00000000, 
	0X02008222, 0X02bdbb2c, 0X00208322, 0X00057100, 0X00000000, 0X00808002, 
	0X02000000, 0X0000f022, 0X02000000 },
    {	0X00080000, 0X005870d8, 0X00000000, 0X00649322, 0X00000000, 0X021c7298, 
	0X00008000, 0X00201000, 0X00208202, 0X0058f0fe, 0X00000000, 0X00000000, 
	0X00000020, 0X00000000, 0X00000002, 0X001d6090, 0X00249222, 0X00200000, 
	0X00000000, 0X00000020, 0X02248326, 0X00051016, 0X00000200, 0X00008020, 
	0X01008000, 0X04041080, 0X00000000 },
    {	0X004c1010, 0X0097f998, 0X00248232, 0X00009322, 0X0004c226, 0X00ddf0f6, 
	0X00201222, 0X0224f230, 0X02048020, 0X00094030, 0X00000002, 0X00004420, 
	0X00699a22, 0X0221e22e, 0X0310c3ba, 0X04fd78d8, 0X023dd322, 0X00200000, 
	0X0234c272, 0X0039b32e, 0X02108322, 0X00095b88, 0X00000022, 0X0004832a, 
	0X00000200, 0X02000000, 0X00000002 },
    {	0X00080000, 0X00557b8e, 0X00000000, 0X00000000, 0X00000002, 0X00bc5a3e, 
	0X00000000, 0X00000020, 0X00000000, 0X003df0ae, 0X00000000, 0X00000000, 
	0X02008020, 0X00000002, 0X00000000, 0X0065e2b2, 0X00000002, 0X00000000, 
	0X00008002, 0X00000008, 0X00000020, 0X00040820, 0X00008020, 0X00008020, 
	0X00000008, 0X00002084, 0X06809222 } };
/*
	Configuration parameters
*/
#define MINLENGTH 5			/* minimum password length */
#define MAXLENGTH 8			/* maximum password length */
#define MINNOFIND 2			/* minimum unusual triples */
short unusual;				/* count of unusual triples */
/*
	obvious  --  test word for obviousness as password

	Words are rejected for being too short, too long, 
	or looking too much like English words.
*/
char *obvious(word)			/* returns msg or zero if OK */
char word[];				/* word to try */
{
	register int i = 0;		/* for length */
	while (word[i]) i++;		/* compute length */
	if (i < MINLENGTH) return("too short");
	if (i > MAXLENGTH) return("too long");
	unusual = 0;			/* no unusual triples yet */
	obvword(word);			/* try the word */
	if (unusual<MINNOFIND) return("too obvious");	/* too obvious */
	return(0);			/* success */
}
/*
	dotriple  --  called by obvword

	If this triple is not used by any word used to build the table,
	we tally that fact.
*/
dotriple(m1,m2,m3)
short m1,m2,m3;				/* all in 0..26 */
{
	if (!(obvtab[m1][m2] & (1L << m3))) unusual++; /* check for triple */
}
/*
	obvword  --  do one word

	dotriple is called on each 3-character triple in the word,
	using a mapped value of the character into the range 0..26,
	where letters map into 1..26 regardless of case and everything
	else maps to zero.
*/
obvword(word)
char word[];				/* word to do, max 20 chars */
{	register int i;			/* for loops */
	register int patcnt = 0;	/* count in word */
	register char ch;		/* working char */
	short pat[21];			/* pattern of mapped values */
	for (i=0; word[i] && (i < sizeof(pat)); i++)	/* scan until null */
	{ 	patcnt = i;		/* max value */
		ch = word[i];		/* get character */
		if ((ch >= 'a') && (ch <= 'z')) pat[i] = ch + 1 - 'a';
	   else if ((ch >= 'A') && (ch <= 'Z')) pat[i] = ch + 1 - 'A';
	   else pat[i] = 0;		/* map into 0..26 */
	}
	for (i=0; i < patcnt - 1; i++)	/* for all triples */
	{	dotriple(pat[i],pat[i+1],pat[i+2]); /* do the triple */
	}
}
