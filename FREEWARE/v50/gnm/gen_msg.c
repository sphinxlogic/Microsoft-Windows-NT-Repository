/*
 *  gen_msg.c
 *
 *  Lon S Hilde
 *  Written 18-jan-1994
 *
 *  This tool translates a GNM file into a MSG file, allowing you to
 *  have a common source file that generates output suitable for the
 *  MESSAGE compiler.  Used in comjunction with GEN_SDML.
 *
 *  Added a braces-to-angle-brackets tweak, to allow a way to get angle
 *  brackets through past the converter - 13-Mar-1998 - Steve Hoffman
 *
 *  No claims to style or substance.
 *
 */
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 512
#define MESSAGE_LINE_LIMIT 10
#define BLANK_LINE "\n"

int copy_substr_but_single_space (char in_string[], char out_string[]);
int find_next_word_index (char in_string[]);
void strip_fao_and_check_quotes (char string[], char **fao_found, char **quote_found);
void lower_case_directive (char string[]);

main (int argc, char *argv[])
{
    FILE *ostream, *istream;
    char *prog = argv[0], *infile = argv[1], *outfile = argv[2], *fao_found, *quote_found;
    char buf1[MAX_LINE_LENGTH], buf2[MAX_LINE_LENGTH], buf3[MAX_LINE_LENGTH];
    int index1, index2, index3, l, failsafe;
    fpos_t fptr;

    /* open the input and output files
     */
    istream = fopen (infile, "r");
    if (istream == NULL)
    {
	fprintf (stderr, "%s error, failed to open %s\n", prog, infile);
	exit (1);
    }

    ostream = fopen (outfile, "w");
    if (ostream == NULL)
    {
	fprintf (stderr, "%s error, failed to open %s\n", prog, outfile);
	exit (2);
    }

    /* read in the input file lines one by one
     */
    while (fgets (&buf1[0], MAX_LINE_LENGTH, istream) == &buf1[0])
    {
	/* pass through all the comment lines
	 */
	if (buf1[0] == '!')
	{
	    if (fputs (&buf1[0], ostream) == EOF)
	    {
		fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
		exit (3);
	    }
	    continue;
	}

	/* pass through some of the the "directive" lines (but single space).
	 * Also, lower case the directives
	 * I may also need to reverse the order of the .base and .severity lines?
	 */
	if ((strncmp (&buf1[0], ".TITLE", 6) == 0) ||
	    (strncmp (&buf1[0], ".IDENT", 6) == 0) ||
	    (strncmp (&buf1[0], ".FACILITY", 9) == 0) ||
	    (strncmp (&buf1[0], ".LITERAL", 8) == 0) ||
	    (strncmp (&buf1[0], ".PAGE", 5) == 0) ||
	    (strncmp (&buf1[0], ".BASE", 5) == 0) ||
	    (strncmp (&buf1[0], ".SEVERITY", 9) == 0) ||
	    (strncmp (&buf1[0], ".END", 4) == 0) ||
	    (strncmp (&buf1[0], ".title", 6) == 0) ||
	    (strncmp (&buf1[0], ".ident", 6) == 0) ||
	    (strncmp (&buf1[0], ".facility", 9) == 0) ||
	    (strncmp (&buf1[0], ".literal", 8) == 0) ||
	    (strncmp (&buf1[0], ".page", 5) == 0) ||
	    (strncmp (&buf1[0], ".base", 5) == 0) ||
	    (strncmp (&buf1[0], ".severity", 9) == 0) ||
	    (strncmp (&buf1[0], ".end", 4) == 0))
 	{
	    /* I want a blank line preceeding each page and the final end
	     */
	    if ((strncmp (&buf1[0], ".PAGE", 5) == 0) ||
	        (strncmp (&buf1[0], ".END", 4) == 0) ||
	       (strncmp (&buf1[0], ".page", 5) == 0) ||
	        (strncmp (&buf1[0], ".end", 4) == 0))
	    {
		if (fputs (BLANK_LINE, ostream) == EOF)
		{
		    fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
		    exit (4);
		}
	    }
	    index2 = copy_substr_but_single_space (&buf1[0], &buf2[0]);
	    buf2 [index2++] = '\n';
	    buf2 [index2++] = '\0';
	    lower_case_directive (&buf2[0]);
	    if (fputs (&buf2[0], ostream) == EOF)
	    {
		fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
		exit (5);
	    }
	    continue;
	}

	/* the only other things that interests me is the NAME and MESSAGE lines.
	 * this is where the hard work is done.
	 */
	if ( ((strncmp (&buf1[0], ".NAME", 5) == 0)) ||
	    ((strncmp (&buf1[0], ".name", 5) == 0)))
	{
	    /* strip off the .NAME directive and start with the actual name.
	     * (buf2 will build the desired string, buf1 and buf3 are temporaries.)
	     */
	    index1 = find_next_word_index (&buf1[0]);
	    index2 = copy_substr_but_single_space (&buf1[index1], &buf2[0]);

	    /* buffer read in the message lines
	     */
	    failsafe = 0;
	    index3 = 0;
	    while (failsafe < MESSAGE_LINE_LIMIT)
	    {
		/* save the current file position in case we go "to far"
		 */
		if (fgetpos (istream, &fptr) != 0)
		{
		    fprintf (stderr, "%s error, unable to get current istream file position\n", prog);
		    exit (6);
		}
		
		if (fgets (&buf1[0], MAX_LINE_LENGTH, istream) == NULL)
		{
		    buf2[index2++] = '\0';
		    fprintf (stderr, "%s error, message not terminated, %s\n", prog, &buf2[0]);
		    exit (7);
		}
		if (((strncmp (&buf1[0], ".MESSAGE", 8) == 0)) ||
		    ((strncmp (&buf1[0], ".message", 8) == 0)))
		    index1 = find_next_word_index (&buf1[0]);
		else
		{
		    if ((buf1[0] == '.') ||
			(buf1[0] == '!'))
		    {
			/* we went too far, back up so that we don't miss this next
			 * directive or comment.
			 */
			if (fsetpos (istream, &fptr) != 0)
			{
			    fprintf (stderr, "%s error, unable to set current istream file position\n", prog);
			    exit (8);
			}
			break;
		    }
		    else
			index1 = 0;
		}
		l = copy_substr_but_single_space (&buf1[index1], &buf3[index3]);
		index3 = index3 + l;
		failsafe++;
	    }
	    buf3[index3] = '\0';

	    /* copy the message string to buf1 and deal with fao directives
	     * and double-quotes
	     */
	    strip_fao_and_check_quotes (&buf3[0], &fao_found, &quote_found);
	
	    if (fao_found != NULL)
	    {
		l = copy_substr_but_single_space ("/fao=1", &buf2[index2]);
		index2 = l + index2;
	    }

	    if (index2 < 8)
		buf2 [index2++] = '\t';
	    if (index2 < 16)
		buf2 [index2++] = '\t';
	    buf2 [index2++] = '\t';

	    if (quote_found != NULL)
		buf2 [index2++] = '<';
	    else
		buf2 [index2++] = '"';

	    l = copy_substr_but_single_space (&buf3[0], &buf2[index2]);
	    index2 = index2 +l;

	    if (quote_found != NULL)
		buf2 [index2++] = '>';
	    else
		buf2 [index2++] = '"';

	    buf2[index2++] = '\n';
	    buf2[index2++] = '\0';
	    if (fputs (&buf2[0], ostream) == EOF)
	    {
		fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
		exit (9);
	    }
	    continue;
	}
    }
    exit (0);
}

int copy_substr_but_single_space (char in_string[], char out_string[])
{
    int i = 0, j = 0;

    while (in_string[i] != '\0')
    {
	if (in_string[i] == '\f')
	    i++;
	else if (isspace (in_string[i]))
	{
	    out_string[j++] = ' ';
	    while ((isspace (in_string[i])) &&
		   (in_string[i] != '\0'))
		i++;
	}
	else
	    out_string[j++] = in_string[i++];
    }
    if (out_string[j-1] == ' ')
	j--;

    return j;
}

int find_next_word_index (char in_string[])
{
    int i = 0;

   while (!(isspace (in_string[i])) &&
	   (in_string[i] != '\0'))
	i++;

    while ((isspace (in_string[i])) &&
	   (in_string[i] != '\n') &&
	   (in_string[i] != '\0'))
	i++;

   return i;
}

void strip_fao_and_check_quotes (char string[], char **fao_found, char **quote_found)
{
    char *cptr1=NULL, *cptr2=NULL, *cptr3=NULL, *cptr4=NULL, *local_fao_start;
    int fao_found_boolean;
    int x;

    /* any double quotes in this string?
     */
    *quote_found = strchr (&string[0], '"');

    /* lets, see if we can locate a set of fao delimiters in the input string
     */
    fao_found_boolean = 0;
    cptr4 = &string[0];
    while ((cptr1 = strchr (cptr4, '<')) != NULL)
    {
	if ((cptr4 = strchr (cptr1, '>')) == NULL)
	    break;
	if ((cptr2 = strchr (cptr1, ',')) == NULL)
	    break;
	if ((cptr3 = strchr (cptr2, '!')) == NULL)
	    break;

	if (cptr3 < cptr4)
	{
	    fao_found_boolean = 1;
	    break;
	}
    }

    if (fao_found_boolean != 0)
	*fao_found = cptr1;
    else
    {
	*fao_found = NULL;
	return;
    }

    while (fao_found_boolean)
    {
	local_fao_start = cptr1;

	/* Move forward from the "," to the next non space character
	 */
	while (isspace (*(++cptr2)));

	/* So...remove the text between cptr1 and cptr2...
	 * don't forget to skip the ">".
	 */
	while (*cptr2 != '\0')
	{
	    if (cptr2 == cptr4)
		cptr2++;
	    else
		*(cptr1++) = *(cptr2++);
	}
	*cptr1 = '\0';

 	/* but there might be more, so try again
	 */
	fao_found_boolean = 0;
	while ((cptr1 = strchr (local_fao_start, '<')) != NULL)
	{
	    if ((cptr4 = strchr (cptr1, '>')) == NULL)
		break;
	    if ((cptr2 = strchr (cptr1, ',')) == NULL)
		break;
	    if ((cptr3 = strchr (cptr2, '!')) == NULL)
		break;

	    if (cptr3 < cptr4)
	    {
		fao_found_boolean = 1;
		break;
	    }
	}
    }
    /*
     *  the following tweak allows us to get angle brackets into the 
     *  message file by passing in braces...
    */
    for ( x = 0; x < strlen( string ); x++ )
     {
     if ( string[x] == '{' ) string[x] = '<';
     if ( string[x] == '}' ) string[x] = '>';
     }
}

void lower_case_directive (char string[])
{
    int i = 1;

    if (string[0] != '.')
	return;

    while (isalpha (string[i]))
	{
	string[i] = tolower (string[i]);
	i++;
	}

    if ((strncmp (&string[0], ".severity", 9)) == 0)
    {
	while (isspace (string[i]))
	    i++;

	while (isalpha (string[i]))
	    {
	    string[i] = tolower (string[i]);
	    i++;
	    }
    }
}
