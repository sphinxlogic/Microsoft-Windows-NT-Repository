/********************************************************/
/*							*/
/*	SEMI's BANNER ( Ver 1.21- ) :			*/
/*							*/
/*		made	by Koh,BongGyun			*/
/*			in '91.4 ( Ver 1.0- )		*/
/*			   '91.5 ( Ver 1.11- )		*/
/*			   '91.9			*/
/*							*/
/*		e-mail to e910017@xserver.kaist.ac.kr	*/
/*							*/
/********************************************************/

#include <stdio.h>

#define	END	'\0'
#define	NO	-1
#define DATFILE	"kbandat.c"

#include DATFILE

char	*data;				/* data inputed */
short	font[ _WIDTH+2 ][ MAX_LEN ];	/* font data to output */
	/* font[ _WIDTH ] : width of letter, font[ _WIDTH+1 ] : cmode */

short	count;	/* pass1: now reading count-th character in data */
		/* output: now reading count-th font */
short	digit;	/* pass1: now making digit-th font */
		/* output: total digits of font */

main( argc, argv )
	short	argc;
	char	*argv[];
{
	int	i;
	void	pass1(), output();

	if ( argc == 1 )	puts( ":)" );	/* refuse if no strings */
	else	for ( i=1 ; i < argc ; ++i ) {
			data = argv[ i ];	/* data is the i-th string */
			count = digit = 0;

			pass1();	/* making font */
			output();	/* output font with compression */
		}
}

void	pass1()		/* making font following data */
{
	short	kor =NO, i;
	void	make_eng(), make_kor();

	while ( data[ count ] != END )
		if ( data[ count ] == CMD_CHR )
			if ( data[ ++count ] == CMD_CHR )	/* double CMD_CHR is one normal character */
				make_eng();
			else
			switch ( instr( mod_chr, data[ count ] ) + 1 ) {
			  case 0:	/* toggle Korean mode */
				kor = -kor;	break;
			  case 1:	/* change choice char's */
				if ( ( i = data[ ++count ] - '0' ) < 0 || i >= N_CMODE )
					cmode = 0;
				else {	cmode = i;		++count; }
				break;
			  case 2:	/* change English letter types */
				if ( ( i = data[ ++count ] - '0' ) < 0 || i >= N_MODE )
					mode = 0;
				else {	mode = i;		++count; }
			}
		else if ( kor == NO )	make_eng();
		else			make_kor();
}

void	output()	/* output font with compression */
{
	short	i;
	void	output1();

	for ( i=0 ; i < WIDTH ; i+=2 ) {
		output1( font[ i ], font[ i+1 ] );	/* compress two lines into one line and output it */
		putchar( '\n' );
	}
}

void	output1( font1, font2 )		/* compress two lines and output */
	short	*font1, *font2;
{
	short	i;
	void	output2();

	for ( i=0 ; i < digit ; ++i )
		output2( font1[ i ], font2[ i ], font[ _WIDTH ][ i ], font[ _WIDTH+1 ][ i ] );	/* output one digit font in two lines with compression */
}

void	output2( f1, f2, len, cmode )		/* output a digit font with compression */
	short	f1, f2;
	short	len;
	short	cmode;
{
	short	choice, i;

	for ( i=stepspace ; i > 0 ; --i )
		putchar( font_choice[ cmode ][ 0 ] );
	while ( ( len /= 2 ) > 0 ) {
		choice = ( f1 / len ) * 2 + ( f2 / len );	/* get the choice determining the character to output */
		putchar( font_choice[ cmode ][ choice ] );	/* output a bit character */
		f1 %= len; f2 %= len;
	}
}

void	make_eng()	/* making font in English mode */
{
	short	i, in;

	if ( ( in = instr( eng_chr, data[ count++ ] ) ) != NO ) {
		for ( i=0 ; i < WIDTH ; ++i )
			font[ i ][ digit ] = eng_fnt[ mode ][ in ][ i ];	/* read font */
		font[ _WIDTH ][ digit ] = eng_fnt[ mode ][ in ][ i ];		/* read the width */
		font[ _WIDTH+1 ][ digit++ ] = cmode;
	}
}

void	make_kor()	/* making font in Korean mode */
{
	short	fir, sec, thi, i;
	short	att_f =0, att_s =0;

	if ( ( fir = instr( kor_key[ 0 ], data[ count ] ) ) != NO )
		++count;
	if ( ( sec = instr( kor_key[ 1 ], data[ count ] ) ) != NO ) {
		if ( sec >= 9 ) {
			++att_f;
			switch ( instr( "hnm", data[ count++ ] ) + 1 ) {
			  case 1:
				if ( ( i = instr( "kol", data[ count ] ) ) != NO ) {
					sec += i+1;	++count;	++att_f;
				}
				break;
			  case 2:
				if ( ( i = instr( "jpl", data[ count ] ) ) != NO ) {
					sec += i+1;	++count;	++att_f;
				}
				break;
			  case 3:
				if ( data[ count ] == 'l' ) {
					++sec;		++count;	++att_f;
				}
			}
		}
		else	++count;
		if ( ( thi = instr( kor_key[ 2 ], data[ count ] ) ) != NO &&
			 instr( kor_key[ 1 ], data[ count+1 ] ) == NO ) {
			att_f += 3;	att_s = 1;
			if ( ( i = instr( "rsfq", data[ count++ ] ) ) != NO &&
				 instr( kor_key[ 1 ], data[ count+1 ] ) == NO ) {
				switch ( i ) {
				  case 0:
					if ( data[ count ] == 't' ) {
						++thi;		++count;
					}
					break;
				  case 1:
					if ( ( i = instr( "wg", data[ count ] ) ) != NO ) {
						thi += i+1;	++count;
					}
					break;
				  case 2:
					if ( ( i = instr( "raqtxvg", data[ count ] ) ) != NO ) {
						thi += i+1;	++count;
					}
					break;
				  case 3:
					if ( data[ count ] == 't' ) {
						++thi;		++count;
					}
				}
			}
		}
	}

	if ( fir == NO && sec == NO )
		make_eng();	/* if not Korean letter, deal it as English */
	else {
		for ( i=0 ; i < WIDTH ; ++i ) {
			font[ i ][ digit ] = ( fir != NO ) ? fir_fnt[ att_f ][ fir ][ i ] : 0;
			if ( sec != NO ) {
				font[ i ][ digit ] |= sec_fnt[ att_s ][ sec ][ i ];
				if ( att_s != 0 )	font[ i ][ digit ] |= thi_fnt[ 0 ][ thi ][ i ];
			}
		}
		font[ _WIDTH ][ digit ] = KOR_LEN;
		font[ _WIDTH+1 ][ digit++ ] = cmode;
	}
}

instr( str, ch )	/* return the position of ch in str, if no position, NO */
	char	*str;
	char	ch;
{
	short	i =0;

	while ( ch != str[ i ] && str[ i ] != END )	++i;
	if ( str[ i ] == END )	i = NO;		/* if no position, return NO */
	return( i );
}

