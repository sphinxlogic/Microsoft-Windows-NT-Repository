17-Feb-86 14:37:31-MST,2686;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-SMOKE.ARPA by SIMTEL20.ARPA with TCP; Mon 17 Feb 86 14:37:25-MST
Received: from brl-aos.arpa by SMOKE.BRL.ARPA id a013663; 15 Feb 86 21:42 EST
Received: from ames-nas-gw.arpa by AOS.BRL.ARPA id a012214; 15 Feb 86 21:32 EST
From: Marty <fouts@ames-nas.arpa>
Message-Id: <8602160233.AA11868@ames-nas.ARPA>
Received: by ames-nas.ARPA; Sat, 15 Feb 86 18:33:58 pst
Date: 15 Feb 1986 1833-PST (Saturday)
To: unix-sources@BRL.ARPA
Cc: fouts@ames-nas.arpa
Subject: How many bits and where they go.


     Attached is a simple minded program which gives a printout of various
odd bits of information about the way a machine does arithmetic in C.

     It works on a number of unix implementations (I. E. gives the same
answers as the local C manual,) and I would be very interested in hearing
from anyone who tries it on other implementations.

     (So far, I've tried it on SVR2 and BSD4.2 Vaxen, SGI Irises, Amdahl
UTS, and Cray UniCos.)

     Compile in the usual way.

----- Cut here and compile.  THIS IS NOT A SHELL SCRIPT -----

#define checkbits(N,CP,L) { \
    N = 1; \
    L = 0; \
    while (N != 0) { \
	N <<= 1; \
	L++; \
    } \
    printf("%s   %4d   %6d    ", CP, L, sizeof(N)); \
    N = 1; \
    N = ~N; \
    switch (N) { \
        case -1 : printf("1's c.\n"); break; \
	case -2 : printf("2's c.\n"); break; \
	default : if (N < 0) { \
	    printf("  mag.\n"); \
	} else { \
	    printf("unsign\n"); \
	} \
    } \
}

#define printbytes(CP,N,L) { \
    long temp = (long) N; \
    printf("%s ",CP); \
    for (i = 0; i < ((L) / bits); i++) { \
        printf("%3d",(int) (temp & 0xFFL)); \
	temp >>= bits; \
    } \
    printf("\n"); \
}

char C;
short S;
int I;
long L;
int icl, isl, iil, ill;

union u {
    char C;
    short S;
    int I;
    long L;
    char FILLER[sizeof(L)];
} U;


main()
{
    int i;
    int bits;
    for (i = 0; i < sizeof(U.L); i++) U.FILLER[i] = (char) (i + 1);
    printf(" Type   Bits   Sizeof   Format\n");
    checkbits(C," Char",icl);
    bits = icl;
    checkbits(S,"Short",isl);
    checkbits(I,"  Int",iil);
    checkbits(L," Long",ill);
    printf("\nPosition ");
    for (i = 0; i < sizeof(U.L); i++) printf("%3d",i);
    printf("\n");
    printbytes("    Char",U.C,icl);
    printbytes("   Short",U.S,isl);
    printbytes("     Int",U.I,iil);
    printbytes("    Long",U.L,ill);
    printf("\n");
    if ( (char) U.S == U.S ) printf("Char == Short\n");
    if ( (short) U.I == U.I ) printf("Short == Int\n");
    if ( (long)((int) (U.L)) == U.L ) printf("Int == Long\n");
}
