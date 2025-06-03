/*
 *	block - dispays block style banner one word at a time.
 *	Author:		Jae H. Lee, (Coyote on irc)
 *			State University of New York at Buffalo
 *			Department of Computer Science
 *	example:	block this is a test.
 *			block -i this is a test.  ("this" in italic)
 *			block -i this -i is -i a -i test. (all in italic)
 *			block this\ is a test.  ("this is" on same line)
 *
 */
#include <stdio.h>
#define	COLM	8
#define MAXCHR	10
#define HCHR    10
#define WCHR    10
#define MAXLINE 120
#define ITALIC	1


static char *block_char[] = {
"             !!       \"\" \"\"                 $$     %%%    %%  &&&&        ''    ",
"             !!       \"\" \"\"     ##  ##   $$$$$$$$ %% %%  %%  &&  &&      ''     ",
"             !!       \"\" \"\"     ##  ##  $$$$$$$$$$%% %% %%   &&  &&     ''      ",
"             !!       \"\" \"\"   ##########$$  $$     %%% %%     && &&             ",
"             !!                 ##  ##  $$$$$$$$$     %%       &&&              ",
"             !!                 ##  ##   $$$$$$$$$   %%       && &&             ",
"             !!               ##########    $$  $$  %%  %%%  &&   && &          ",
"                                ##  ##  $$$$$$$$$$ %%  %% %%&&     &&&          ",
"             !!                 ##  ##   $$$$$$$$ %%   %% %%&&     &&           ",
"             !!                             $$    %     %%%  &&&&&& &           ",
"       (    )                                                                  /",
"     ((      ))                   ++                                          //",
"    ((        ))     *  *  *      ++                                         // ",
"   ((          ))     * * *       ++                                        //  ",
"   ((          ))      ***    ++++++++++          ----------               //   ",
"   ((          ))   ********* ++++++++++          ----------              //    ",
"   ((          ))      ***        ++       ,,,                           //     ",
"    ((        ))      * * *       ++      ,,,,,                  ...    //      ",
"     ((      ))      *  *  *      ++       ,,,,                 .....  //       ",
"       (    )                                ,                   ...   /        ",
"   0000       11     22222222 3333333333     444  5555555555 66666666 7777777777",
"  000000    1111    2222222222333333333     4444  555555555566666666667777777777",
" 00    00  11 11    2       22      33     44 44  55        66       6       77 ",
"00      00    11            22     33     44  44  55        66              77  ",
"00      00    11           222    333    44   44  555555555 666666666      77   ",
"00      00    11         222        333 444444444455555555556666666666    77    ",
"00      00    11       222           33 4444444444        5566      66   77     ",
" 00    00     11      222     3       33      44  5       5566      66  77      ",
"  000000  111111111122222222223333333333      44  55555555556666666666 77       ",
"   0000   11111111112222222222 33333333       44   55555555  66666666 77        ",
" 88888888  99999999                                                    ???????? ",
"88888888889999999999                          <<              >>      ??????????",
"88      8899      99   :::       ;;;         <<                >>     ??      ??",
"88      8899      99   :::       ;;;        <<    ==========    >>            ??",
" 88888888 9999999999                       <<     ==========     >>        ???? ",
" 88888888  999999999                      <<                      >>      ??    ",
"88      88        99   :::       ;;;       <<     ==========     >>       ??    ",
"88      88        99   :::       ;;;        <<    ==========    >>              ",
"88888888889999999999              ;;         <<                >>         ??    ",
" 88888888  99999999               ;           <<              >>          ??    ",
" @@@@@@@@  AAAAAAAA BBBBBBBBB  CCCCCCCC DDDDDDDDD EEEEEEEEEEFFFFFFFFFF GGGGGGGG ",
"@@@@@@@@@@AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFFGGGGGGGGGG",
"@@      @@AA      AABB      BBCC       CDD      DDEE        FF        GG        ",
"@@  @@@ @@AA      AABB      BBCC        DD      DDEE        FF        GG        ",
"@@  @@@@@@AA      AABBBBBBBBB CC        DD      DDEEEEE     FFFFF     GG   GGGGG",
"@@  @@@@@@AAAAAAAAAABBBBBBBBB CC        DD      DDEEEEE     FFFFF     GG   GGGGG",
"@@   @@@@ AAAAAAAAAABB      BBCC        DD      DDEE        FF        GG      GG",
"@@        AA      AABB      BBCC       CDD      DDEE        FF        GG      GG",
"@@@@@@@@@@AA      AABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFF        GGGGGGGGGG",
" @@@@@@@@ AA      AABBBBBBBBB  CCCCCCCC DDDDDDDDD EEEEEEEEEEFF         GGGGGGGGG",
"HH      HHIIIIIIIIII  JJJJJJJJKK      KKLL        MM      MMNN      NN OOOOOOOO ",
"HH      HHIIIIIIIIII  JJJJJJJJKK     KK LL        MMMM  MMMMNNN     NNOOOOOOOOOO",
"HH      HH    II         JJ   KK   KK   LL        MM MMMM MMNNNN    NNOO     OOO",
"HH      HH    II         JJ   KK KK     LL        MM  MM  MMNN NN   NNOO    O OO",
"HHHHHHHHHH    II         JJ   KKKKK     LL        MM  MM  MMNN  NN  NNOO   O  OO",
"HHHHHHHHHH    II         JJ   KK  KK    LL        MM      MMNN  NN  NNOO  O   OO",
"HH      HH    II    JJ   JJ   KK   KK   LL        MM      MMNN   NN NNOO O    OO",
"HH      HH    II    JJ   JJ   KK    KK  LL        MM      MMNN    NNNNOOO     OO",
"HH      HHIIIIIIIIIIJJJJJJJ   KK     KK LLLLLLLLLLMM      MMNN     NNNOOOOOOOOOO",
"HH      HHIIIIIIIIII JJJJJ    KK      KKLLLLLLLLLLMM      MMNN      NN OOOOOOOO ",
"PPPPPPPPP  QQQQQQQQ RRRRRRRRR  SSSSSSSS TTTTTTTTTTUU      UUVV      VVWW      WW",
"PPPPPPPPPPQQQQQQQQQQRRRRRRRRRRSSSSSSSSSSTTTTTTTTTTUU      UUVV      VVWW      WW",
"PP      PPQQ      QQRR      RRSS       S    TT    UU      UU VV    VV WW      WW",
"PP      PPQQ      QQRR      RRSS            TT    UU      UU VV    VV WW      WW",
"PPPPPPPPPPQQ      QQRRRRRRRRRRSSSSSSSSS     TT    UU      UU VV    VV WW  WW  WW",
"PPPPPPPPP QQ      QQRRRRRRRRR  SSSSSSSSS    TT    UU      UU VV    VV WW  WW  WW",
"PP        QQ   QQ QQRR   RR           SS    TT    UU      UU  VV  VV  WW  WW  WW",
"PP        QQ    QQQQRR    RR  S       SS    TT    UU      UU  VV  VV  WW WWWW WW",
"PP        QQQQQQQQQ RR     RR SSSSSSSSSS    TT    UUUUUUUUUU   VVVV    WWW  WWW ",
"PP         QQQQQQ QQRR      RR SSSSSSSS     TT     UUUUUUUU     VV     WW    WW ",
"XX      XXYY      YYZZZZZZZZZZ  [[[[[    \\           ]]]]]                      ",
" XX    XX  YY    YY ZZZZZZZZZ   [[       \\\\             ]]      ^^              ",
"  XX  XX    YY  YY        ZZ    [[        \\\\            ]]     ^^^^             ",
"   XXXX      YYYY        ZZ     [[         \\\\           ]]    ^^  ^^            ",
"    XX        YY     ZZZZZZZZ   [[          \\\\          ]]   ^^    ^^           ",
"   XXXX       YY     ZZZZZZZZ   [[           \\\\         ]]  ^^      ^^          ",
"  XX  XX      YY       ZZ       [[            \\\\        ]]                      ",
" XX    XX     YY      ZZ        [[             \\\\       ]]                      ",
"XX      XX    YY     ZZZZZZZZZ  [[              \\\\      ]]            __________",
"XX      XX    YY    ZZZZZZZZZZ  [[[[[            \\   ]]]]]            __________",
"  ``       AAAAAAAA BBBBBBBBB  CCCCCCCC DDDDDDDDD EEEEEEEEEEFFFFFFFFFF GGGGGGGG ",
"   ``     AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFFGGGGGGGGGG",
"    ``    AA      AABB      BBCC       CDD      DDEE        FF        GG        ",
"          AA      AABB      BBCC        DD      DDEE        FF        GG        ",
"          AA      AABBBBBBBBB CC        DD      DDEEEEE     FFFFF     GG   GGGGG",
"          AAAAAAAAAABBBBBBBBB CC        DD      DDEEEEE     FFFFF     GG   GGGGG",
"          AAAAAAAAAABB      BBCC        DD      DDEE        FF        GG      GG",
"          AA      AABB      BBCC       CDD      DDEE        FF        GG      GG",
"          AA      AABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFF        GGGGGGGGGG",
"          AA      AABBBBBBBBB  CCCCCCCC DDDDDDDDD EEEEEEEEEEFF         GGGGGGGGG",
"HH      HHIIIIIIIIII  JJJJJJJJKK      KKLL        MM      MMNN      NN OOOOOOOO ",
"HH      HHIIIIIIIIII  JJJJJJJJKK     KK LL        MMMM  MMMMNNN     NNOOOOOOOOOO",
"HH      HH    II         JJ   KK   KK   LL        MM MMMM MMNNNN    NNOO     OOO",
"HH      HH    II         JJ   KK KK     LL        MM  MM  MMNN NN   NNOO    O OO",
"HHHHHHHHHH    II         JJ   KKKKK     LL        MM  MM  MMNN  NN  NNOO   O  OO",
"HHHHHHHHHH    II         JJ   KK  KK    LL        MM      MMNN  NN  NNOO  O   OO",
"HH      HH    II    JJ   JJ   KK   KK   LL        MM      MMNN   NN NNOO O    OO",
"HH      HH    II    JJ   JJ   KK    KK  LL        MM      MMNN    NNNNOOO     OO",
"HH      HHIIIIIIIIIIJJJJJJJ   KK     KK LLLLLLLLLLMM      MMNN     NNNOOOOOOOOOO",
"HH      HHIIIIIIIIII JJJJJ    KK      KKLLLLLLLLLLMM      MMNN      NN OOOOOOOO ",
"PPPPPPPPP  QQQQQQQQ RRRRRRRRR  SSSSSSSS TTTTTTTTTTUU      UUVV      VVWW      WW",
"PPPPPPPPPPQQQQQQQQQQRRRRRRRRRRSSSSSSSSSSTTTTTTTTTTUU      UUVV      VVWW      WW",
"PP      PPQQ      QQRR      RRSS       S    TT    UU      UU VV    VV WW      WW",
"PP      PPQQ      QQRR      RRSS            TT    UU      UU VV    VV WW      WW",
"PPPPPPPPPPQQ      QQRRRRRRRRRRSSSSSSSSS     TT    UU      UU VV    VV WW  WW  WW",
"PPPPPPPPP QQ      QQRRRRRRRRR  SSSSSSSSS    TT    UU      UU VV    VV WW  WW  WW",
"PP        QQ   QQ QQRR   RR           SS    TT    UU      UU  VV  VV  WW  WW  WW",
"PP        QQ    QQQQRR    RR  S       SS    TT    UU      UU  VV  VV  WW WWWW WW",
"PP        QQQQQQQQQ RR     RR SSSSSSSSSS    TT    UUUUUUUUUU   VVVV    WWW  WWW ",
"PP         QQQQQQ QQRR      RR SSSSSSSS     TT     UUUUUUUU     VV     WW    WW ",
"XX      XXYY      YYZZZZZZZZZZ   {{{{       ||      }}}}     ~~~                ",
" XX    XX  YY    YY ZZZZZZZZZ   {{          ||         }}   ~~ ~~    ~          ",
"  XX  XX    YY  YY        ZZ    {{          ||         }}   ~    ~~ ~~          ",
"   XXXX      YYYY        ZZ     {{          ||         }}         ~~~           ",
"    XX        YY     ZZZZZZZZ  {{                       }}                      ",
"   XXXX       YY     ZZZZZZZZ  {{                       }}                      ",
"  XX  XX      YY       ZZ       {{          ||         }}                       ",
" XX    XX     YY      ZZ        {{          ||         }}                       ",
"XX      XX    YY     ZZZZZZZZZ  {{          ||         }}                       ",
"XX      XX    YY    ZZZZZZZZZZ   {{{{       ||      }}}}                        ",
};

#include <stdio.h>
#define SPACE ' '

main   (argc, argv)
int	argc;
char	**argv;
{
	struct  {
		int row;
		int colm;
	} pos[MAXCHR];
        int	i, j, k;
	int	wordlen;
	int	c_type;
	char   *word;
	char	line[MAXLINE];

	for (i = 1; --argc > 0; i++) {
	if (!strcmp (argv[i], "-i")) {
		c_type = ITALIC;
		continue;
	} 
	word = argv[i];
	wordlen = strlen (word);
	for (j=0; j < wordlen && j < MAXCHR; j++) {
		if (word[j] < SPACE) word[j] = '?';
		pos[j].row  = (int)(word[j] - SPACE) / 8;
		pos[j].colm = (int)(word[j] - SPACE) % 8;
	}
	for (k=0; k<HCHR; k++) {
		bzero (line, MAXLINE);
		wordlen = strlen (word);
		if (c_type == ITALIC) strncat (line, "              ", HCHR-k-1);
		for (j=0; j < wordlen && j < MAXCHR; j++) {
			(void) strncat (line, &block_char[pos[j].row*HCHR+k][pos[j].colm*WCHR], WCHR);
			(void) strcat (line, "  ");
		}
		(void) printf ("%s\n", line);
	}
	printf (" \n \n");
	c_type--;
	}
}
