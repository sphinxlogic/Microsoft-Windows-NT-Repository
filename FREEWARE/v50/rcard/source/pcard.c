#define module_name PCARD
#define module_ident "V1.2"
#define COPYRIGHT " Copyright © 1999, Hunter Goatley"
/*
 * Copyright © 1999, Hunter Goatley.   Permission is granted for
 * not-for-profit redistribution, provided all source and object code remain
 * unchanged from the original distribution, and that all copyright notices
 * remain intact.
 */
/*
 *
 *  Program:	PCARD
 *
 *  Author:	Hunter Goatley
 *
 *  Date:	May 29, 1998
 *
 *  Abstract:
 *
 *	This program will read a DECwindows CardFiler file and create
 *	a TeX source file to print the cards.
 *
 *	This program is based on my RCARD program, written 18-OCT-1991.
 *
 *      This programs works with the files produced by the DECwindows v5.4
 *      cardfiler and the Motif v1.0 and V1.1 cardfilers.
 *
 *  Modified by:
 *
 *	V1.2		Hunter Goatley		17-MAY-1999 17:35
 *		Bug fix to handle files with more than 127 records.
 *
 *	V1.1		Hunter Goatley		 5-NOV-1998 22:09
 *		Cleanup so it'll compile with DEC C without /STANDARD=VAXC.
 *
 *	V1.0		Hunter Goatley		29-MAY-1998 09:04
 *		Original version (from RCARD).
 */

#ifdef __DECC
#pragma module module_name module_ident
#else
#ifdef VAXC
#module module_name module_ident
#endif
#endif

#include <stdio.h>
#include <errno.h>
#include <ssdef.h>
#include <libdef.h>
#include <smgdef.h>
#include <descrip.h>
#include <stat.h>
#include <rmsdef.h>
#include <nam.h>
#include <fab.h>
#include <rab.h>
#include <lib$routines.h>
#include <stdlib.h>
#include <string.h>
#include <starlet.h>
#ifdef __DECC
#include <fcntl.h>
#include <unistd.h>
#else
#include <unixio.h>
#endif

/* Magic numbers based on observation only!! */
#define SUBJLEN 53		/* Length of each subject record */
#define MAXSUBJLEN 40		/* Maximum length of a subject */

#define PUT(rab,str) \
	rab.rab$w_rsz = strlen(str);\
	rab.rab$l_rbf = str;\
	sys$put(&rab,0,0);

/* Some globals to make life much easier */

unsigned long status;
int number_of_cards;

/* A card consists of a subject and text */
struct cards {
  char *subj;
  char *text;
} *card;


/* FORWARD ROUTINE */
unsigned long	read_card_file (int);
unsigned long	main (int, char *[]);

/* MACROS */
#define errchk(s) if (!(s & 1)) lib$signal(s);
#define call(s)  status = s; errchk(status);

#define init_ddesc(desc)			\
	desc.dsc$w_length = 0;			\
	desc.dsc$b_dtype = DSC$K_DTYPE_T;	\
	desc.dsc$b_class = DSC$K_CLASS_D;	\
	desc.dsc$a_pointer = 0;

#define strcpy_to_tex(dest,source) \
	{ char *dst, *src; \
	  for (dst = dest, src = source; *src != '\0'; src++) { \
	    if (*src == '$' || *src == '&' || *src == '#' || *src == '\\' \
		|| *src == '_') \
		*dst++ = '\\'; \
	    if (*src == '\t') \
		*dst++ = ' '; \
	    else \
		*dst++ = *src; \
	  } \
	  *dst = '\0'; \
	}

char *TeX_Header[] = {
	"\\ignoreunderfill",
	"\\parindent=0pt",
	"\\parskip=20pt",
	"\\parindent=10pt",
	"\\parskip=0pt",
	"\\eightpoint",
	"\\def\\today{\\ifcase\\month\\or",
	"	January\\or February\\or March\\or April\\or May\\or June\\or",
	"	July\\or August\\or September\\or October\\or November\\or December\\fi",
	"	\\space\\number\\day, \\number\\year}",
	"\\def\\Card#1{\\it\\vskip20pt\\hrule depth0pt height1pt\\vglue0pt",
	"        \\leftline{\\strut #1}\\vglue0pt\\obeylines\\tt",
	"        }",
	"\\newspage",
	0};

char *TeX_Header2[] = {
	"\\centerline{\\tenrm \\today}",
	"",
	"\\begincolumns{2}",
	"",
	0};

char *TeX_End[] = {
	"",
	"\\endcolumns\\bye",
	0};



int print_array (struct RAB *rab, char *array[])
{
  char **ppLine;
  for (ppLine = array; *ppLine != NULL; ppLine++) {
	rab->rab$w_rsz = strlen(*ppLine);
	rab->rab$l_rbf = *ppLine;
	sys$put (rab,0,0);
  }
  return (1);
}


/*
 *  Routine:	read_card_file
 *
 *  Functional description:
 *
 *	This routine reads the .CARD file into memory and sets up the card[]
 *	subj and text pointers.
 *
 *  Formal parameters:
 *
 *      fd - the file descriptor for the open file to read
 *
 *  Implicit inputs:
 *
 *      card[], number_of_cards
 *
 *  Returns:
 *
 *      Unsigned long as status
 *
 */
unsigned long
read_card_file (int fd)
{
   struct stat cardstat;
   unsigned short card_length;
   char *buffer;
   char *ptr;
   
   int i;

   printf("Reading file into memory....\n");
   if (fstat(fd,&cardstat) != 0)
   {
     printf("Error getting file size");
     return(SS$_ABORT);
   }

  /*  cardstat.st_size is the size in bytes */

   if ((buffer = (char *) malloc (cardstat.st_size)) == 0) {
     perror("Memory exhausted");
     return(SS$_ABORT);
   }
   /*  Now read the whole file into memory  */
   if ((i = read (fd, buffer, cardstat.st_size)) < 0) {
     perror("Read");
     return(SS$_ABORT);
   }
#ifdef DEBUG
   printf("Read %d bytes\n",i);
#endif

   if (strncmp("mgc",buffer,3) != 0) {
     printf("Magic value not found---not a Cardfiler file\n");
     return(SS$_ABORT);
   }

   number_of_cards = * (unsigned short *) &buffer[3];

   /*
     Allocate the memory for card[] and subj[]
     */

   if ((card = (struct cards *) calloc (number_of_cards,
					(sizeof (struct cards)))) == 0) {
     perror("Memory exhausted (card)");
     return(SS$_ABORT);
   }

   /*  Now point to the first card title */
   ptr = &buffer[5];

   /*
     Fill in the card[].subj array with the addresses of each subject in the
     card file.

     The card titles are all SUBJLEN bytes long.  The title itself begins
     at offset 12 from the beginning of each "record."

     Under DECwindows, each record began with a "w", but the Motif cardfiler
     changed it to some other value (usually the number of the card, but
     not always).
     */

   for (i = 0; i < number_of_cards; i++, ptr += SUBJLEN)
     card[i].subj = ptr + 12;

   /*
     Skip over the non-null character.  Then skip over all the nulls.
     When we've found a non-null, we need to back up two places (actually
     three, because the auto-increment put us one character too far).
     */
   ptr++;
   while (!(*ptr++));
   ptr -= 3;

   /* Now we're ready to process the cards themselves */

   for (i = 0; (ptr[0] == '\0') && (i < number_of_cards); i++) {
     card_length = *((unsigned short *) (ptr+2));
     /*  This section should handle images, but I couldn't figure out how
	 they were stored.  */
     /*
     if ((long) ptr[4] == 0) {
       ptr += 4;
       ptr += (card_length + 5);
       card_length = *((unsigned short *) (ptr+2));
     }
     */
     card[i].text = ptr + 4;
     ptr += (card_length + 4);
   }

#ifdef DEBUG
   for (i=0; i < number_of_cards; i++)
     {
       printf("------------------------------------------\n");
       printf("Subject: %s\n", card[i].subj);
       printf("Text:    %s\n", card[i].text);
     }
#endif

   return(SS$_NORMAL);

}


/*
 * 
 *  Main routine
 *
 */
unsigned long
main(int argc, char *argv[])
{
  FILE *fp;				/* Input file pointer */
  int  fd;				/* File descriptor for card file */
  int  x;				/* Work variable */
  struct dsc$descriptor_s menu_line;	/* Descriptor for menu lines */
  struct FAB fab;			/* RMS blocks for SYS$PARSE */
  struct NAM nam;
  struct FAB ofab;			/* Output file FAB & RAB */
  struct RAB orab;
  char *default_card_file = "SYS$LOGIN:.CARD";
  char *default_out_file = "SYS$DISK:[].TEX";
  char ess[NAM$C_MAXRSS+1];		/* Space for expanded name */
  char title[NAM$C_MAXRSS+1];		/* Space for the menu title */
  char workstr[1024];
  char workstr2[1024];
  char null[] = "";
  char *ptr, *ptr2;
  int len, card_size;

  --argc;				/* Don't count the program name */
  if (argc != 1)			/* If too many or too few args, */
      exit(LIB$_WRONUMARG);		/* ...  exit with error status  */

  /* Set up RMS FAB and NAM blocks for call to SYS$PARSE to provide for
     a default file spec and the title of the menu. */
  fab = cc$rms_fab;
  nam = cc$rms_nam;
  fab.fab$b_fns = strlen(argv[1]);
  fab.fab$l_fna = argv[1];
  fab.fab$b_dns = strlen(default_card_file);
  fab.fab$l_dna = default_card_file;
  fab.fab$l_nam = &nam;
  nam.nam$l_esa = ess;
  nam.nam$b_ess = NAM$C_MAXRSS;

  call(sys$parse (&fab));

  /* Copy the file name for use as the title */
  strncpy(title,nam.nam$l_name,nam.nam$b_name);
  title[nam.nam$b_name] = '\0';
  ess[nam.nam$b_esl] = '\0';

  if ((fd=open(ess,0,0)) <= 0) {
    printf("Error opening input file %s\n",ess);
    exit(SS$_ABORT);
  }

  /* Read entire file into memory (storing in array of cards structure) */

  if ((status = read_card_file (fd)) != SS$_NORMAL)
    exit(status);

  close(fd);      /* Go on and close the file */

  /* Set up RMS FAB and NAM blocks for call to SYS$PARSE to provide for
     a default file spec and the title of the menu. */

  memset(&ofab, 0, FAB$C_BLN);		/* Zero out FAB */
  ofab.fab$b_bid = FAB$C_BID;		/* Set block ID to FAB */
  ofab.fab$b_bln = FAB$C_BLN;		/* Set the length */
  ofab.fab$b_fac = FAB$M_GET | FAB$M_PUT;
  ofab.fab$b_rat = FAB$M_CR;
  ofab.fab$b_rfm = FAB$C_VAR;
  ofab.fab$b_org = FAB$C_SEQ;		/* Sequential access */

  ofab.fab$b_fns = strlen(title);
  ofab.fab$l_fna = title;
  ofab.fab$b_dns = strlen(default_out_file);
  ofab.fab$l_dna = default_out_file;

  memset(&orab, 0, RAB$C_BLN);		/* Zero out RAB */
  orab.rab$b_bid = RAB$C_BID;		/* Set RAB block ID */
  orab.rab$b_bln = RAB$C_BLN;		/* Set RAB block length */
  orab.rab$l_fab = &ofab;

  status = sys$create (&ofab, 0, 0);
  if (status & 1)
    status = sys$connect (&orab, 0, 0);
  if (!(status & 1))
	return(status);

  /*
   *  Print out the TeX header
   */
  print_array (&orab, TeX_Header);
  sprintf(workstr, "\\centerline{\\twelverm Cardfile: %s}", title);
  PUT (orab, workstr);
  print_array (&orab, TeX_Header2);


  /* Now loop through the cards, writing them to the output file */

  for (x=0; x < number_of_cards; x++) {
printf ("Writing out card #%d, %s\n", x, card[x].subj);
    PUT(orab, null);
    strcpy_to_tex (workstr, card[x].subj);
    sprintf(workstr2, "\\Card{%s}", workstr);
    PUT(orab, workstr2);

    ptr = card[x].text;
    card_size = strlen(ptr);
    while (card_size > 0) {
	if ((ptr2 = (char *) strchr(ptr, '\n')) == 0) {
	    len = card_size;
	    ptr2 = ptr;
	}
	else
	    len = ptr2 - ptr;
	*ptr2 = '\0';
	strcpy_to_tex (workstr, ptr);
	if (len == 0)
	   strcpy(workstr, "\\line{}");
	PUT (orab, workstr);
	ptr += len + 1;
	card_size -= (len + 1);
    }

  }

  print_array (&orab, TeX_End);

  sys$close (&ofab, 0, 0);

}
