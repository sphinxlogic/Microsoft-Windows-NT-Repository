/*------------------------------------------------------------------------
      decode_64.c
      Author:   Sohail Gani
      Dept:     Mail Interchange Group
      Date:     30-10-91
------------------------------------------------------------------------*/


#include <stdio.h>

/*------------------------------------------------------------------------
      This is the look up table used for the base decoding
------------------------------------------------------------------------*/
char  lut[] = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '=', ','
};

FILE *fpi, *fpo;

void
convert (FILE *fp, int data[])
{
int   i, j, total;


      i = total = 0;
      for (j = 128; j >= 1; j /= 2)
	    total += data[i++] * j;

      fprintf (fp, "%c", total);

      return;
}


void
process (FILE *fp,char seg[])
{
int   cnt_a, num;
int   j, k;
int   which = 0;
int   index = 0;
int   reg[3][8];

      for (cnt_a = 0; cnt_a < 4; cnt_a++) {
	    if (seg[cnt_a] == '=')
		  break;
	    for (num = 0; num < 64; num++) {
		  if (lut[num] == seg[cnt_a]) {
/*------------------------------------------------------------------------
      Use base64 look up table to extract 8-bit binary and convert
------------------------------------------------------------------------*/
			for (j = 32; j >= 1; j /= 2) {
			      if (num >= j) {
				    num = num % j;
				    reg[which][index] = 1;
			      }
			      else {
				    reg[which][index] = 0;
			      }
			      index++;
			      if (index >= 8) {
				    convert (fp, reg[which]);
				    index = 0;
				    which = (which + 1) % 3;
				    for (k = 0; k < 8; k++)
					  reg[which][k] = 0;
			      }
			}
			break;
		  }
	    }
      }
      return;
}


int
main ( int argc, char *argv[] )
{
int   i;
char  literal;
char  segment[4];

/*------------------------------------------------------------------------
      Is there a file to process or is stdin going to be used
------------------------------------------------------------------------*/
      if (argc >= 2) {
	    fpi = fopen (argv[1], "r");
      }
      else
	    fpi = stdin;

/*------------------------------------------------------------------------
      Is there a file to write out to or is stdout going to be used
------------------------------------------------------------------------*/
      if (argc == 3) {
	    fpo = fopen (argv[2], "w");
      }
      else
	    fpo = stdout;


      i = 0;

/*------------------------------------------------------------------------
      Process the input, ignore "\n" and deal with eol (i.e. ",")
------------------------------------------------------------------------*/
      while (fscanf (fpi, "%c", &literal) != EOF) {
	    if (literal == '\n') {
		  continue;
	    }
	    if (literal == ',') {
		  fprintf (fpo, "\n");
		  continue;
	    }
	    segment[i++] = literal;
	    if (i >= 4) {
		  process (fpo,segment);
		  i = 0;
	    }
      }

/*------------------------------------------------------------------------
      Let's tidy up
------------------------------------------------------------------------*/
      fprintf (fpo, "");
      if (fpi != stdin)
	    fclose (fpi);
      if (fpo != stdout)
	    fclose (fpo);
      return 0;
}


