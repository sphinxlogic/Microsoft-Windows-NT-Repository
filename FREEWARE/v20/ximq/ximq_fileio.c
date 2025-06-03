/*
 * XIMQ - display NASA/PDS "IMQ" files on a X11 workstation
 *
 * Copyright © 1991, 1994 Digital Equipment Corporation
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 *
 * IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION.
 *
 * DIGITAL EQUIPMENT CORPORATION SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND DIGITAL EQUIPMENT CORPORATION HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Module:
 *
 *	XIMQ_FILEIO.C
 *
 * Author:
 *
 *    Frederick G. Kleinsorge, November 1991
 *
 * Environment:
 *
 *    OpenVMS AXP, VAX-C syntax
 *
 * Abstract:
 *
 *    This is just the file io access routines.  Since the PDS CD disks
 *    are really VMS files - but with no record info... and for speed...
 *    I map the files as private sections, and then simulate I/O.
 *
 * Modifications:
 *
 */ 
#include "ximq.h"

/*
 *  Routine:
 *
 *	close_file
 *
 *  Description:
 *
 *	Unmaps and closes a file.  Resets some of the file information to
 *	reduce problems with stale data.
 *
 *  Inputs:
 *
 *	address of a file information block
 *
 *  Outputs:
 *
 *	TRUE  == closed
 *	FALSE == error
 *
 */
int
close_file(FILEINFO *fin)

{
  int status = 0;

  if (state.debugging)
    printf("close_file\n");

  if (fin == 0) return FALSE;

  if (fin->input_file_mapped)
    {
      status = sys$deltva(&fin->start_addr, 0);
      if (!status & 1)
        {
	  printf("Error Unmapping section file %d\n", status);
        }      
      sys$close( &fin->fab);
     /*
      *  Make the filename invalid
      *
      */
      fin->input_filename[0] = 0;
    }

  fin->input_file_open = 0;
  fin->start_addr = 0;
  fin->end_addr   = 0;
  fin->input_file_mapped = 0;

  if ((fin->image_valid) && (fin->tree))
    {
      delete_node(fin->tree);
      fin->tree = 0;
    }

  return (TRUE);
}

/*
 *  Routine:
 *
 *	open_image_file
 *
 *  Description:
 *
 *	Open the file as a private section file mapped into the virtual
 *	address of the process.  It returns the file information into the
 *	file information block
 *
 *  Inputs:
 *
 *	address of a file information block
 *
 *  Outputs:
 *
 *	file information block is filled in and the file is open and mapped
 *
 *	TRUE  = success (file open and mapped)
 *	FALSE = failed (file is not open and not mapped)
 *
 */
int
open_image_file(fin)

FILEINFO *fin;

{
  int status, addr_base[2], use_P0[2] = {0x200, 0x200};

  if (state.debugging)
    printf("open_image_file\n");

 /*
  *  Open input file.
  *
  */
  fin->fab = cc$rms_fab;
  fin->fab.fab$l_fop = FAB$M_UFO;
  fin->fab.fab$b_fac = FAB$M_GET;
  fin->fab.fab$b_rtv = 0xFF;
  fin->fab.fab$l_fna = &fin->input_filename[0];
  fin->fab.fab$b_fns = strlen(&fin->input_filename[0]);
  fin->fab.fab$l_dna = DEFAULT_FILE;
  fin->fab.fab$b_dns = sizeof (DEFAULT_FILE - 1);

  status = sys$open( &fin->fab);

  if (status != RMS$_NORMAL)
    {
      printf("Error opening file <%s>\n", &fin->input_filename[0]);
      lib$signal( status);
      return;
    }

  fin->input_file_open = 1;

 /*
  *  Map the input file as a private section in P0 space, let VMS figure out
  *  where.
  *
  */
  status = sys$crmpsc( &use_P0,                     /* inadr */
                       &addr_base,                  /* retadr */
                       0,                           /* acmode */
                       SEC$M_EXPREG | SEC$M_CRF,    /* flags */
                       0,                           /* gsdnam */
                       0,                           /* ident */
                       0,                           /* relpag */
                       fin->fab.fab$l_stv,          /* chan */
                       0,                           /* pagcnt */
                       0,                           /* vbn */
                       0xEEEE,                      /* prot */
                       0);                          /* pfc */

  if (!(status & 1))
    {
      printf("Error mapping file <%s>\n", &fin->input_filename[0]);
      lib$signal( status);
      close_file(fin);
      return (FALSE);
    }

  if (state.debugging)
    {
      printf("File mapped, start %d, end %d\n", addr_base[0], addr_base[1]);
    }

  fin->input_file_mapped = 1;

 /*
  *  Get the base address into the structure pointer
  *
  */
  fin->record.length  = addr_base[0];
  fin->start_addr     = addr_base[0];
  fin->end_addr       = addr_base[1];
  fin->file_records   = 999;
  fin->label_records  = 999;
  fin->record_bytes   = 999;
  fin->current_record = 1;
  fin->tree           = 0;

  return (TRUE);
}

/*
 *  Routine:
 *
 *	process_label
 *
 *  Description:
 *
 *	Reads the file label and verifies that the file is an image file.
 *	The image and histogram record pointers are found and optionally
 *	some of the label information is loaded into the global variables.
 *
 *  Inputs:
 *
 *	address of a file information block
 *	flag indicating if the global variables need to be loaded.
 *
 *  Outputs:
 *
 *	TRUE  = success
 *	FALSE = failure (the file is not valid, or no END statement was found)
 *
 */
int
process_label(fin, load_variables)

FILEINFO *fin;
int load_variables;

{
  unsigned char ibuf[2048];
  int current_record_pointer = 1, last_record_pointer = 1;
  int *num_records = &fin->image_record;

  union {
    char  *c;
    short *s;
    } test;

  int length, nlen, line, i, j, end_found = 0;

  if (state.debugging)
    printf("process_label\n");

 /*
  *  Do a little test to make sure that this is a valid file...
  *
  *  First.  The first word must be a record count, and the size must
  *  be > 0 and < 2k.
  *
  *  Next.  The first characters must be "NJPL1I00PDS100".
  *
  *  Next.  This line must have a "SDFU_LABEL" value
  *
  *  If it passes, then we are all set.
  *
  */
  test.s = fin->start_addr;
  length = *test.s++;

  if ((length <= 0) || (length > 2048))
    {
     /*
      *  See if this is actually a CRLF formatted file instead of a
      *  VMS variable length file!
      *
      */
      fin->record_format = 0;
      test.s = fin->start_addr;

      if (strncmp(test.c,"NJPL1I00PDS100",14) != 0)
	{
          printf ("Invalid record format for image\n");
          return FALSE;
	}
    }
  else fin->record_format = 1;

  if (strncmp(test.c,"NJPL1I00PDS100",14) == 0)
    {
      i = parse_string(test.c, length, &ibuf[0]);
      if (strncmp(&ibuf[0],"SFDU_LABEL",10) != 0)
        {
	  printf ("Initial record did not have SFDU_LABEL value\n");
	  printf ("[%*.*s]\n",i,i,&ibuf[0]);
	  return FALSE;
        }
    }
  else
    {
      printf ("Initial record did not start NJPL1I00PDS100...\n");
      printf ("[%*.*s]\n",length,length,&ibuf[0]);
      return FALSE;
    }

 /*
  *  Now loop through the header until we find an END statement
  *
  */
  do {

      length = read_var(fin, ibuf);

      if (ibuf[0] == 94)
        {
          current_record_pointer = parse_value(ibuf, length);
          *num_records = (current_record_pointer - last_record_pointer);
          last_record_pointer = current_record_pointer;

          if (i = strncmp(ibuf,"^IMAGE_HISTOGRAM",16) == 0)
            {
              fin->image_histogram_record = current_record_pointer;
	      num_records = &fin->image_histogram_length;
            }
          else if (i = strncmp(ibuf,"^ENCODING_HISTOGRAM",19) == 0)
            {
              fin->encoding_histogram_record = current_record_pointer;
	      num_records = &fin->encoding_histogram_length;
            }
          else if (i = strncmp(ibuf,"^ENGINEERING_TABLE",18) == 0)
            {
              fin->engineering_table_record = current_record_pointer;
              num_records = &fin->engineering_table_length;
            }
          else if (i = strncmp(ibuf,"^IMAGE",6) == 0)
            {
              fin->image_record = current_record_pointer;
              num_records = &fin->image_record;
            }
        }

      if ((i = strncmp(ibuf,"END",3)) == 0 && length == 3)
	{
	  end_found = 1;
	  break;
	}

      if (load_variables)
	{
 	  if (i = strncmp(ibuf,"FILTER_NAME",11) == 0)
	    parse_string(ibuf, length, &txt.filter_name[0]);
	  else if (i = strncmp(ibuf,"FILTER_NUMBER",13) == 0)
	    parse_string(ibuf, length, &txt.filter_number[0]);
	  else if (i = strncmp(ibuf,"RECORD_BYTES",12) == 0)
	    fin->record_bytes = parse_value(ibuf, length);
	  else if (i = strncmp(ibuf,"FILE_RECORDS",12) == 0)
	    fin->file_records = parse_value(ibuf, length);
	  else if (i = strncmp(ibuf,"LABEL_RECORDS",13) == 0)
	    fin->label_records = parse_value(ibuf, length);
	  else if (i = strncmp(ibuf,"TARGET_NAME",11) == 0)
	    parse_string(ibuf, length, &txt.target_name[0]);
	  else if (i = strncmp(ibuf,"SPACECRAFT_NAME",15) == 0)
	    parse_string(ibuf, length, &txt.spacecraft_name[0]);
	  else if (i = strncmp(ibuf,"IMAGE_ID",8) == 0)
	    parse_string(ibuf, length, &txt.image_id[0]);
	  else if (i = strncmp(ibuf,"IMAGE_NUMBER",12) == 0)
	    parse_string(ibuf, length, &txt.image_number[0]);
	  else if (i = strncmp(ibuf,"IMAGE_TIME",10) == 0)
	    parse_string(ibuf, length, &txt.image_time[0]);
	  else if (i = strncmp(ibuf,"SCAN_MODE",9) == 0)
	    parse_string(ibuf, length, &txt.scan_mode[0]);
	  else if (i = strncmp(ibuf,"NOTE",4) == 0)
	    parse_string(ibuf, length, &txt.note_data[0]);
	  else if (ibuf[0] == 32)
	    {
	      for (j = 0; j < length; j += 1)
		{
		  if (ibuf[j] != 32) break;
		}

	      if (j < (length-1))
		{
		  if (i = strncmp(&ibuf[j],"LINES",5) == 0)
		    state.num_scanlines = parse_value(&ibuf[j], length-j);
		  else if (i = strncmp(&ibuf[j],"LINE_SAMPLES",12) == 0)
		    state.samples_per_line = parse_value(&ibuf[j], length-j);
		}
	    }
	}
    } while (length > 0);

  if ((state.debugging) && (load_variables))
    {    
      printf("Image histogram record %d, count = %d\n",
		fin->image_histogram_record, fin->image_histogram_length);

      printf("Encoding histogram record %d, count = %d\n",
		fin->encoding_histogram_record, fin->encoding_histogram_length);

      printf("Engineering table record %d, count = %d\n",
		fin->engineering_table_record, fin->engineering_table_length);

      printf("Image record %d\n",
		fin->image_record);

      printf("Record bytes %d, File records %d, label records %d\n",
		fin->record_bytes, fin->file_records, fin->label_records);
    }

  return (end_found);
}

/*
 *  Routine:
 *
 *	parse_value
 *
 *  Description:
 *
 *	This routine is called from the label processing routine to extract
 *	a decimal numeric number from the input line.  The line has the
 *	format FOO         = NNNN
 *
 *  Inputs:
 *
 *	address of string
 *	max length of string
 *
 *  Outputs:
 *
 *	value
 *
 */
int
parse_value(input_data, max_length)
unsigned char *input_data;
int max_length;
{
  unsigned char *scanit;
  int i;

  scanit = input_data;

  for (i = 0; i < max_length; i += 1)
    {
      if (*scanit++ == 61)
        {
          while (*scanit == 32) *scanit++;
          return (atoi(scanit));
        }
    }
  return (FALSE);
}

/*
 *  Routine:
 *
 *	parse_string
 *
 *  Description:
 *
 *	Called from process label to extract a string from the current
 *	line of the format: FOO     = string
 *	The string may be quoted, and a /* terminates the string if present
 *	as a comment delimiter.
 *
 *  Inputs:
 *
 *	address of string
 *	max length of input
 *	address to place result string
 *
 *  Outputs:
 *
 *	a null terminated string will be returned
 *	the return value will be the length of the returned string.
 *
 */
int
parse_string(input_data, max_length, result_data)
unsigned char *input_data;
unsigned char *result_data;
int max_length;
{
  union {
	  unsigned char  *c;
	  short *w;
	} scanit;

  int i = 0, dblq = 0, scanning = 1, looking = 1, j = 0;

  scanit.c = input_data;

 /*
  *  Main loop, continue until the end of the string is reached...
  *
  */
  while (i < max_length)
    {
     /*
      *  We are parsing a string that looks like:
      *
      *  FOO                = BAR
      *
      *  and we want to find BAR, so we scan until we hit the "=" in the
      *  string
      *
      */
      if (scanning)
        {
          if (*scanit.c++ == 61)
	    scanning = 0;
	}
     /*
      *  If we are past the "="...
      *
      */
      else
	{
         /*
          *  We are looking for the start of the string.  We expect to see
          *  spaces - which we will ignore, double quotes - which start a
          *  string that we will not touch, a single quote, which starts a
          *  literal, or even a number...
          *
          */
	  if (looking)
	    {
	     /*
	      *  Ignore spaces
	      *
	      */
	      if (*scanit.c == 32)
		{
		  *scanit.c++;
		}
	     /*
	      *  If it's a double quote " then start the string...
	      *
	      */
	      else if (*scanit.c == '\"')
		{
		  dblq = 1;
		  *scanit.c++;
		}
	     /*
	      *  If it's a single quote then start the string.
	      *
	      */
	      else if (*scanit.c == '\'')
		{
		  *scanit.c++;
		}
	     /*
	      *  If it's any other character, back up a character, change
	      *  the mode and start the string.
	      *
	      */
	      else
		{
		  looking = 0;
		  i -= 1;
		}
            }
         /*
          *  We have started the string and are now checking for the start
          *  of a comment "/*", double or single quotes to terminate the
          *  string.  Otherwise we will simply copy the character into the
          *  output buffer.
          *
          */
	  else
	    {
	     /*
	      *  We have a word pointer to look at two characters to see if
	      *  a comment is starting.  The comment terminates the string
	      *  unless the string is quoted.
	      *
	      */
	      if ((*scanit.w == '/*') && (!dblq))
		{
		  *result_data++ = 0;
		  return (j);
		}
	     /*
	      *  A double quote character also terminates the string.
	      *
	      */
	      else if (*scanit.c == '\"')
		{
		  *result_data++ = 0;
		  return (j);
		}
	     /*
	      *  A single quote terminates the string unless it's inside
	      *  a quoted string.
	      *
	      */
	      else if ((*scanit.c == '\'') && (!dblq))
		{
		  *result_data++ = 0;
		  return (j);
		}
	     /*
	      *  If it's any other character, just copy it.
	      *
	      */
	      else
		{
	          j += 1;
		  *result_data++ = *scanit.c++;
		}
	    }
	}

      if (j >= TEXT_MAX)
        return(j);

      i += 1;
    }

  *result_data++ = 0;

  return (j);
}

/*
 *  Routine:
 *
 *	read_var
 *
 *  Description:
 *
 *	Simulates the read of a variable length record at the current file
 *	position.
 *
 *  Inputs:
 *
 *	address of a file information block
 *	address of a buffer to return the record into
 *
 *  Outputs:
 *
 *	Returns the length of the record (0 - EOF or error)
 *
 */
int
read_var(fin, ibuf)

char  *ibuf;
FILEINFO *fin;

{
int   length,nlen;
char  *to, *from;

  if (state.debugging) printf("read_var\n");

 /*
  *  This emulates the normal variable length record read.  It copies the
  *  variable length record into the buffer provided and returns it's length.
  *  It makes the large assumption that the caller provided a buffer that is
  *  large enough.
  *
  */

  if (fin->record_format)
    {
     /*
      *  Check for EOF
      *
      */
      if ((fin->record.data + *fin->record.length + 2 > fin->end_addr) ||
          (fin->current_record > fin->file_records))
        {

          if (state.debugging) printf("EOF after Record %d, Max Record %d\n",
			fin->current_record, fin->file_records);

          if (state.debugging) printf("    last record address %d, max file addr %d\n",
			fin->record.length, fin->end_addr);

          return (FALSE);
        }

      length = *fin->record.length++;

    }
  else
    {
      from = fin->record.data;

      for (length = 0; length < 10000; length += 1)
        {
	  if (from++ == 0xD)
	    {
	      if (from++ == 0xA) break;
	    }
        }
    }

  if (length > fin->record_bytes)
    {
      if (state.debugging)
      printf("Record %d, length (%d) exceeds max specified in label (%d)\n",
		fin->current_record, length, fin->record_bytes);
    }

  from   = fin->record.data;
  to     = ibuf;

  for (nlen = length; nlen > 0; nlen -= 1)
    {
      *to++ = *from++;
    }

 /*
  *  Point to next record after read
  *
  */
  fin->current_record +=1;
  if (fin->record_format)
    {
      fin->record.data += length + (1*length%2);
    }
  else
      fin->record.data += length + 2;

  return(length);
}

/*
 *  Routine:
 *
 *	set_pos_var
 *
 *  Description:
 *
 *	This is equivalent to read_var, but returns the address of the
 *	record rather than copy it to a local buffer.
 *
 *  Inputs:
 *
 *	address of a file information block
 *	address of a string pointer to return the record buffer address to
 *
 *  Outputs:
 *
 *	Returns the length of the record (0 = EOF or error)
 *
 */
int
set_pos_var(fin, ibuf)

char  **ibuf;
FILEINFO *fin;

{
  int   length;

 /*
  *  Check for EOF, don't bother with partial records!
  *
  */
  if ((fin->record.data + *fin->record.length + 2 > fin->end_addr) ||
      (fin->current_record > fin->file_records))
    {
      if (state.debugging) printf("EOF after Record %d, Max Record %d\n",
			fin->current_record, fin->file_records);

      if (state.debugging) printf("    last record address %d, max file addr %d\n",
			fin->record.length, fin->end_addr);

      return (FALSE);
    }

  length = *fin->record.length++;

  if (length > fin->record_bytes)
    {
      if (state.debugging)
      printf("Record %d, length (%d) exceeds max specified in label (%d)\n",
		fin->current_record, length, fin->record_bytes);
    }

  *ibuf  = fin->record.data;
  fin->current_record +=1;
  fin->record.data += length + (1*length%2);

  return(length);
}

/*
 *  Routine:
 *
 *	seek_var
 *
 *  Description:
 *
 *	Positions the file pointer to the specified record.
 *
 *  Inputs:
 *
 *	address of a file information block
 *	record number to position to (starting with 1)
 *
 *  Outputs:
 *
 *	The record number of the current position after the seek
 *
 *	If 0 is returned, an error or a seek past EOF occured and
 *	the file is positioned to the first record.
 *
 */
int
seek_var(fin, new_position)

int   new_position;
FILEINFO *fin;

{

  int   length,nlen;

 /*
  *  Handle the case where we are already positioned, and where we are
  *  past the position.
  *
  */
  if (new_position == fin->current_record)
    return (fin->current_record);

  if (new_position < fin->current_record)
    {
     /*
      *  Reset to the initial record...
      *
      */
      fin->current_record = 1;
      fin->record.length = fin->start_addr;
    }

 /*
  *  Now, step through the records until we get there...
  */
  while (fin->current_record < new_position)
    {
      fin->current_record +=1;
      length = *fin->record.length++;

      if (length > fin->record_bytes)
        {
          if (state.debugging)
          printf("Record %d, length (%d) > max in label (%d) in SEEK\n",
    		fin->current_record, length, fin->record_bytes);
        }

      fin->record.data += length + (1*length%2);

     /*
      *  Check for EOF
      *
      */
      if ((fin->record.length >= fin->end_addr) ||
          (fin->current_record > fin->file_records))
        {

          if (state.debugging) printf("EOF after Record %d, Max Record %d\n",
			fin->current_record, fin->file_records);

          if (state.debugging) printf("    last record address %d, max file addr %d\n",
			fin->record.length, fin->end_addr);

          fin->current_record = 1;
          fin->record.length = fin->start_addr;
          return (FALSE);
        }
    }

  return(fin->current_record);
}

/*
 *  Routine:
 *
 *	rewind_file
 *
 *  Description:
 *
 *	Sets the file to values that indicate that it is at the
 *	start of the file, and makes the file image invalid.
 *
 *  Inputs:
 *
 *	address of a file information block
 *
 *  Outputs:
 *
 *	none
 *
 */
int
rewind_file(FILEINFO *fin)
{

 /*
  *  Get the base address into the structure pointer
  *
  */
  fin->record.length  = fin->start_addr;
  fin->current_record = 1;
  fin->file_records   = 999;
  fin->label_records  = 999;
  fin->record_bytes   = 999;
  fin->image_valid    = 0;
}
