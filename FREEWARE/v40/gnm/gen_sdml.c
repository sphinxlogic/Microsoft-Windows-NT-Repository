/*
 *  gen_msg.c
 *
 *  Lon S Hilde
 *  Written 18-jan-1994
 *
 *  This tool translates a GNM file into an SMDL file, allowing you to
 *  have a common source file that generates output suitable for the
 *  DECdocument tool.  Used in conjunction with GEN_MSG.
 *
 *  No claims to style or substance.
 *
 */
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 512
#define GENERAL_LINE_LIMIT 30

#define MSG_NAME_ARRAY_SIZE 20480

#define BLANK_LINE "\n"
#define SDML_TEXT_HDR "<MESSAGE_SECTION> <MESSAGE_TYPE>(textident)\n\n"
#define SDML_MSG_TEXT_1 "<MSG>"
#define SDML_MSG_TEXT_2 "<MSG_TEXT>(Facility)\n"
#define SDML_MSG_TEXT_3 "VDE, OpenVMS Development Environment\n"
#define SDML_MSG_TEXT_4 "<MSG_TEXT>(Explanation)\n"
#define SDML_MSG_TEXT_5 "<MSG_TEXT>(User Action)\n"
#define SDML_TEXT_END "<ENDMESSAGE_SECTION>\n"

typedef struct n_and_p {
    char *name;
    fpos_t fptr;
} name_and_fpos;

int copy_substr_but_single_space (char in_string[], char out_string[]);
int find_next_word_index (char in_string[]);
int copy_str_but_no_indent (char in_string[], char out_string[]);
int names_compare (const void *, const void *);
int copy_str_if_change_fao (char in_string[], char out_string[]);

main (int argc, char *argv[])
{
    FILE *ostream, *istream;
    char *prog = argv[0], *infile = argv[1], *outfile = argv[2], *status;
    char buf1[MAX_LINE_LENGTH], buf2[MAX_LINE_LENGTH], buf3[MAX_LINE_LENGTH];
    int l, index1, index2, failsafe;
    name_and_fpos *names[MSG_NAME_ARRAY_SIZE];
    int names_index, names_count, name_length, changed;
    fpos_t fptr;

    /* open the files we need for input and output
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

    /* I'm going to walk through the input file and build name
     * structures in dymnamic memory.  This will allow me to sort them
     * before outputing the whole sdml message text to the output file.
     */
    names_index = 0;
    while (fgets (&buf1[0], MAX_LINE_LENGTH, istream) == &buf1[0])
    {
	if ((strncmp (&buf1[0], ".NAME", 5) == 0) ||
	    (strncmp (&buf1[0], ".name", 5) == 0))
	{
	    /* allocate space for another sdml_message structure
             */
	    names[names_index] = malloc (sizeof (*names[0]));

	   /* find the actual name text and move it into memory (and into
	    * the a name array element).
	    */
	    index1 = find_next_word_index (&buf1[0]);
	    name_length = strlen (&buf1[index1]);
	    names[names_index]->name = malloc (name_length+1);
	    strcpy (names[names_index]->name, &buf1[index1]);

	    /* Also record the file position.  We will use that later when
	     * we go after the message explanation and user_action text.
	     */
	    if (fgetpos (istream, &(names[names_index]->fptr)) != 0)
	    {
		fprintf (stderr, "%s error, unable to get current istream file position\n", prog);
		exit (3);
	    }

	    names_index++;
	}
    }
    names_count = names_index;

    /* Okay, let's sort the names array.
     */
    qsort ((char *)names, names_count, sizeof (names[0]), names_compare);

    /* Okay, let's build the output file.
     *
     * First, put in the header.
     */
    if (fputs (SDML_TEXT_HDR, ostream) == EOF)
    {
	fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
	exit (4);
    }
     
    /* Then, build one message at a time and line by line.
     */
    for (names_index = 0; names_index < names_count; names_index++)
    {
	/* start with the first standard MSG_TEXT and the NAME
         */
	index2 = copy_substr_but_single_space (SDML_MSG_TEXT_1, &buf2[0]);
	buf2 [index2++] = '(';
	l = copy_substr_but_single_space (names[names_index]->name, &buf2[index2]);
	index2 = index2 + l;
	buf2 [index2++] = '\\';

	/* set the file position to the line we left off on when we found the name.
	 * This should be the MESSAGE lines.
	 */
	if (fsetpos (istream, &(names[names_index]->fptr)) != 0)
	{
	    fprintf (stderr, "%s error, unable to set current istream file position\n", prog);
	    exit (5);
	}

	/* tack on the MESSAGE text lines to the NAME text
	 */
	failsafe = 0;
	while (failsafe < GENERAL_LINE_LIMIT)
	{
	    if ((status = fgets (&buf1[0], MAX_LINE_LENGTH, istream)) == NULL)
	    {
		buf2[index2++] = '\0';
		fprintf (stderr, "%s error, message not terminated, %s\n", prog, &buf2[0]);
		exit (6);
	    }
	    if ((strncmp (&buf1[0], ".MESSAGE", 8) == 0) ||
		(strncmp (&buf1[0], ".message", 8) == 0))
		index1 = find_next_word_index (&buf1[0]);
	    else
	    {
		if ((buf1[0] == '.') ||
		    (buf1[0] == '!'))
		    break;
		else
		    index1 = 0;
	    }
	    l = copy_str_but_no_indent (&buf1[index1], &buf2[index2]);
	    index2 = index2 + l;
	    while (isgraph (buf2[--index2]) == 0);
	    buf2[++index2] = ' ';
	    buf2[++index2] = '\n';
	    index2++;
	    failsafe++;
	}
	if (failsafe >= GENERAL_LINE_LIMIT)
	{
	    buf2[index2++] = '\0';
	    fprintf (stderr, "%s error, MESSAGE not terminated, %s\n", prog, &buf2[0]);
	    exit (7);
	}
	while (isgraph (buf2[--index2]) == 0);
	buf2[++index2] = ' ';
	buf2[++index2] = ')';
	buf2[++index2] = '\n';
	buf2[++index2] = '\0';

	/* Okay, we've built the whole "<MSG>.. " line.  Now copy it over one more
	 * time and change the fao stuff, if any.
	 */
	changed = copy_str_if_change_fao (&buf2[0], &buf3[0]);

	/* and, finally, output it
	 */
	if (fputs (((changed) ? &buf3[0] : &buf2[0]), ostream) == EOF)
	{
	    fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
	    exit (8);
	}

	/* Now put in the next MSG_TEXT lines (facility) text
	 */
	if (fputs (SDML_MSG_TEXT_2, ostream) == EOF)
	{
	    fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
	    exit (9);
	}
	if (fputs (SDML_MSG_TEXT_3, ostream) == EOF)
	{
	    fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
	    exit (10);
	}

	/* Now start the EXPLANATION lines, if there are any.
	 */
	if ((strncmp (&buf1[0], ".EXPLANATION", 12) == 0) ||
	    (strncmp (&buf1[0], ".explanation", 12) == 0))
	{
	    if (fputs (SDML_MSG_TEXT_4, ostream) == EOF)
	    {
		fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
		exit (11);
	    }
	    index1 = find_next_word_index (&buf1[0]);
	    index1 = copy_str_but_no_indent (&buf1[index1], &buf2[0]);

	    /* skip blank lines
	     */
	    if (strcmp (BLANK_LINE, &buf2[0]) != 0)
	    {
		if (fputs (&buf2[0], ostream) == EOF)
		{
		    fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
		    exit (12);
		}
	    }
	    failsafe = 0;
	    while (failsafe < GENERAL_LINE_LIMIT)
	    {
		if ((status = fgets (&buf1[0], MAX_LINE_LENGTH, istream)) == NULL)
		{
		    buf2[index2++] = '\0';
		    fprintf (stderr, "%s error, message not terminated, %s\n", prog, &buf2[0]);
		    exit (13);
		}
		if ((buf1[0] == '.') ||
		    (buf1[0] == '!'))
		    break;
		index1 = copy_str_but_no_indent (&buf1[0], &buf2[0]);

		/* skip blank lines
	         */
		if (strcmp (BLANK_LINE, &buf2[0]) != 0)
		{
		    if (fputs (&buf2[0], ostream) == EOF)
		    {
			fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
			exit (14);
		    }
		}
		failsafe++;
	    }
	    if (failsafe >= GENERAL_LINE_LIMIT)
	    {
		fprintf (stderr, "%s error, EXPLANATION not terminated, %s\n", prog, &buf2[0]);
		exit (15);
	    }
	}

	/* Finally, the USER_ACTION, if any.
         */
	if ((strncmp (&buf1[0], ".USER_ACTION", 12) == 0) ||
	    (strncmp (&buf1[0], ".user_action", 12) == 0))
	{
	    if (fputs (SDML_MSG_TEXT_5, ostream) == EOF)
	    {
		fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
		exit (16);
	    }
	    index1 = find_next_word_index (&buf1[0]);
	    index1 = copy_str_but_no_indent (&buf1[index1], &buf2[0]);
	    if (strcmp (BLANK_LINE, &buf2[0]) != 0)
	    {
		if (fputs (&buf2[0], ostream) == EOF)
		{
		    fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
		    exit (17);
		}
	    }
	    failsafe = 0;
	    while (failsafe < GENERAL_LINE_LIMIT)
	    {
		if ((status = fgets (&buf1[0], MAX_LINE_LENGTH, istream)) == NULL)
		{
		    buf2[index2++] = '\0';
		    fprintf (stderr, "%s error, message not terminated, %s\n", prog, &buf2[0]);
		    exit (18);
		}
		if ((buf1[0] == '.') ||
		    (buf1[0] == '!'))
		    break;
		index1 = copy_str_but_no_indent (&buf1[0], &buf2[0]);

		/* skip blank lines
	         */
		if (strcmp (BLANK_LINE, &buf2[0]) != 0)
		{
		    if (fputs (&buf2[0], ostream) == EOF)
		    {
			fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
			exit (19);
		    }
		}
		failsafe++;
	    }
	    if (failsafe >= GENERAL_LINE_LIMIT)
	    {
		fprintf (stderr, "%s error, EXPLANATION not terminated, %s\n", prog, &buf2[0]);
		exit (20);
	     }
	}

	/* One final nit.  End with a blank line.
	 */
	if (fputs (BLANK_LINE, ostream) == EOF)
	{
	    fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
	    exit (23);
	}
    }    /* for names loop */

    /* Finally, terminate the file with the END text and close the output file.
     */
    if (fputs (SDML_TEXT_END, ostream) == EOF)
    {
	fprintf (stderr, "%s error, write failure to %s\n", prog, outfile);
	exit (21);
    }
    if (fclose (ostream) == EOF)
    {
	fprintf (stderr, "%s error, closure failure to %s\n", prog, outfile);
	exit (22);
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

int copy_str_but_no_indent (char in_string[], char out_string[])
{
    int i = 0, j = 0;

    while (((in_string[i] == ' ') ||
	    (in_string[i] == '\t')) &&
	   (in_string[i] != '\0'))
	i++;
 
    while (in_string[i] != '\0')
    {
	if (in_string[i] == '\f')
	    i++;
	else if (in_string[i] == '\t')
	{
	    out_string[j++] = ' ';
	    i++;
	}
	else
	    out_string[j++] = in_string[i++];
    }
    out_string[j] = '\0';

    return j;
}

int names_compare (const void *ptr1, const void *ptr2)
{
    int status;
    char *name1, *name2;

    name1 = (*(name_and_fpos **)(ptr1))->name;
    name2 = (*(name_and_fpos **)(ptr2))->name;

    return strcmp (name1, name2);
}

int copy_str_if_change_fao (char in_string[], char out_string[])
{
    char *cptr1=NULL, *cptr2=NULL, *cptr3=NULL, *cptr4=NULL, *in_cptr, *out_cptr;
    int m, fao_found;

    in_cptr = &in_string[0];
    out_cptr = &out_string[0];

    /* lets, see if we can locate a set of fao delimiters in the input string
     */
    fao_found = 0;
    cptr4 = &in_string[0];
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
	    fao_found = 1;
	    break;
	}
    }

    if (fao_found == 0)
	return 0;

    while (fao_found)
    {
	/* copy over the in string up to the start of the fao stuff
	 */
	for (; in_cptr < cptr1; in_cptr++, out_cptr++)
	    *out_cptr = *in_cptr;

	/* skip the "<" and any spaces
	 */
	in_cptr++;
	while (isspace (*in_cptr))
	    in_cptr++;

	/* is there a parameter name?
	 */
	if (in_cptr < cptr2)
	{
	    /* found a parameter name, so copy in a "<EMPHASIS>(" string
	     */
	    m = copy_substr_but_single_space ("<EMPHASIS>(", out_cptr);
	    out_cptr = out_cptr + m;

	    /* copy over the parameter name
	     */
	    while (in_cptr < cptr2)
		*(out_cptr++) = *(in_cptr++);

	    /* close with a ")"
	     */
	    *(out_cptr++) = ')';
	}

	/* skip the rest of the fao stuff
	 */
	in_cptr = ++cptr4;

 	/* but there might be more, so try again
	 */
	fao_found = 0;
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
		fao_found = 1;
		break;
	    }
	}
    }

    /* copy over the rest of the in string
     */
    while (*in_cptr != '\0')
	*(out_cptr++) = *(in_cptr++);
    *(out_cptr) = '\0';
    return 1;
}

