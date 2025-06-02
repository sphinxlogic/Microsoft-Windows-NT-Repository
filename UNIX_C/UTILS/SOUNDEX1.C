Article 7 of alt.sources:
Path: mit-eddie!husc6!ncsuvx!lll-winken!ohlone!lll-tis!ptsfa!jmc
From: jmc@ptsfa.UUCP (Jerry Carlin)
Newsgroups: alt.sources
Subject: Soundex
Date: 9 Jul 87 21:02:16 GMT

Here is a short program I wrote a few years ago illustrating the use
of the "soundex" algorithm. It is used to generate the same token for
similarly pronounced names (like Smith and Smythe).

/* soundex algorithm from 1918 patent */

#include <stdio.h>
#include <ctype.h>

#define MAXNAME 30      /* max name length */
#define SOUNDLEN 6      /* max len soundex string */

main(argc,argv)
int argc;
char *argv[];
{
        char name[MAXNAME];     /* input string for surname */
        char sound[SOUNDLEN];   /* SOUNDLEN -1 length */
        int namelen;            /* length of entered name */
        int i;

        if (argc > 1)
        {
                for (i = 1; i < argc; i++)
                {
                        getsound(argv[i],sound);
                        printf("%s\n",sound);
                }
                exit(0);
        }
        gets(name);
        if (name[0] == '\0')
                exit(0);
        getsound(name,sound);
        printf("%s\n",sound);
        exit(0);
}

/***************************************************************************/
/* soundex algorhithm - format of result is annnn                          */
/***************************************************************************/

getsound(name,sound)
char *name;
char *sound;
{
        int i;
        int j;
        int val;        /* char value */
        int oldval;
        char tempc;
        int namelen;

        namelen = strlen(name);
        j = 1;          /* start 2nd pos */
        oldval = 0;
        tempc = name[0];
        sound[0] = isupper(tempc) ? tolower(tempc) : tempc;
        for (i = 1; (i <= namelen) && (j < SOUNDLEN - 1); i++)
        {
                tempc = name[i];
                if (isupper(tempc))
                        tempc = tolower(tempc);
                switch (tempc)
                {
                case 'b':
                case 'f':
                case 'p':
                case 'v':
                        val = 1;
                        break;
                case 'c':
                case 'g':
                case 'j':
                case 'k':
                case 'q':
                case 's':
                case 'x':
                case 'z':
                        val = 2;
                        break;
                case 'd':
                case 't':
                        val = 3;
                        break;
                case 'l':
                        val = 4;
                        break;
                case 'm':
                case 'n':
                        val = 5;
                        break;
                case 'r':
                        val = 6;
                        break;
                default:

                        val = 0;
                        break;
                }
                if (val != 0 && val != oldval)
                        sound[j++] = val + '0';
                oldval = val;
        }
        while (j < SOUNDLEN-1)
                sound[j++] = '0';
        sound[SOUNDLEN] = '\0';
}

-- 
voice: (415) 823-2441	uucp: {ihnp4,lll-crg,ames,qantel,pyramid}!ptsfa!jmc
Where am I? In the village. Whose side are you on? That would be telling.


