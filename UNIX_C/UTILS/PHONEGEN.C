18-Dec-85 20:39:55-MST,1483;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Wed 18 Dec 85 20:39:50-MST
Received: from usenet by TGR.BRL.ARPA id a005480; 18 Dec 85 21:51 EST
From: Liudvikas Bukys <bukys@rochester.uucp>
Newsgroups: net.sources
Subject: phone_gen -- a simple efficient general phone number letterizer
Message-ID: <14090@rochester.UUCP>
Date: 18 Dec 85 17:36:54 GMT
Keywords: telephone number permutation
To:       unix-sources@BRL-TGR.ARPA

Here is a little program which does just what the recently-posted
"telno" program does, except that it is more general (no restrictions
on phone number format, no compiled-in array sizes), more efficient
(no big arrays, no calls to qsort()), and a lot simpler.

-------

/*
 * generate all alphabetic strings corresponding to a phone number
 *
 * n.b.: assumes I can modify argv[][] in place!
 */

int main(argc, argv)
int argc;
char **argv;
	{
	for (;  --argc > 0;  printf("\n"))
		phone_gen(*++argv, 0);

	return (0);
	}

char *table[] = { "abc", "def", "ghi", "jkl", "mno", "prs", "tuv", "wxy" };

phone_gen(s, i)
char *s;
int i;
	{
	char c, *s2;
	
	c = s[i];

	if (c == '\0')
		printf("%s\n", s);
	else if ('2' <= c && c <= '9')
		for (s2= table[c-'2'];  s[i]= *s2++;)
			phone_gen(s, i+1);
	else
		phone_gen(s, i+1);

	s[i] = c;
	}

-------

I guess it also assumes that the characters '2' through '9' are
contiguous in your character set.
