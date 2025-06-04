#define module_name RCARD
#define module_ident "V1.3"
#define COPYRIGHT " Copyright © 1991, 1993, MadGoat Software "
/*
 * Copyright © 1991, 1993, MadGoat Software.   Permission is granted for
 * not-for-profit redistribution, provided all source and object code remain
 * unchanged from the original distribution, and that all copyright notices
 * remain intact.
 */
/*
 *
 *  Program:	CARD
 *
 *  Author:	Hunter Goatley
 *
 *  Date:	October 18, 1991
 *
 *  Abstract:
 *
 *	This program will read a DECwindows CardFiler file and display it
 *      on a character-cell terminal (VT100, etc.).  It was written based
 *      on observations of how the data is stored; I didn't have access to
 *      the Cardfiler sources, so the program is not capable of writing to
 *      the file.
 *
 *      This programs works with the files produced by the DECwindows v5.4
 *      cardfiler and the Motif v1.0 and V1.1 cardfilers.
 *
 *  Modified by:
 *
 *	V1.3		Hunter Goatley		21-OCT-1993 10:26
 *		Modified to support screens larger than 24 rows.  Added
 *		copyright to display.  Added support for CTRL-W to
 *		repaint the screen.
 *
 *	01-002		Hunter Goatley		 3-JUL-1993 00:56
 *		Added __alpha check.
 *
 *	01-001		Hunter Goatley		18-OCT-1991 08:40
 *		Genesis.
 */

#ifdef __alpha
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

/* Magic numbers based on observation only!! */
#define SUBJLEN 53		/* Length of each subject record */
#define MAXSUBJLEN 40		/* Maximum length of a subject */

#define MAXLINELEN 46		/* Width of the card virtual display */
#define MVIEWROWS 6		/* Number of rows in subject viewport */
#define CVIEWROWS 10		/* Number of rows in card viewport */
#define CVDSIZE 4000  		/* Size of the card virtual display */

/* Some globals to make life much easier */

unsigned long pb;				/* Pasteboard */
unsigned long mvd;				/* Contents virtual display */
unsigned long cvd;				/* Card virtual display */
unsigned long kb;                               /* Virtual keyboard */
unsigned long number_of_cards;			/* Number of cards in file */
unsigned long status;				/* Status of routine calls */
unsigned long pb_rows;				/* Number of rows in pboard */
unsigned long pb_cols;				/* Number of cols in pboard */
unsigned long copyvd;				/* Copyright virtual display */

/* A card consists of a subject and text */
struct cards {
  char *subj;
  char *text;
} *card;

/* Constants passed in to SMG$ routines (GNU C doesn't like &1, etc.) */
static const unsigned long one			= 1;
static const unsigned long three		= 3;
static const unsigned long twelve		= 12;
static const unsigned long smg_m_up		= SMG$M_UP;
static const unsigned long smg_m_down		= SMG$M_DOWN;
static const unsigned long smg_m_erase_pbd	= SMG$M_ERASE_PBD;
static const unsigned long smg_m_wrap_word	= SMG$M_WRAP_WORD;
static const unsigned long smg_m_normal		= SMG$M_NORMAL;
static const unsigned long smg_m_reverse	= SMG$M_REVERSE;
static const unsigned long smg_m_cursor_off	= SMG$M_CURSOR_OFF;
static const unsigned long smg_m_cursor_on	= SMG$M_CURSOR_ON;
static const unsigned long smg_m_border		= SMG$M_BORDER;
static const unsigned long smg_k_top		= SMG$K_TOP;
static const unsigned long maxsubjlen		= MAXSUBJLEN;
static const unsigned long maxlinelen		= MAXLINELEN;
static const unsigned long cvdsize		= CVDSIZE;

static unsigned long mviewrows		= MVIEWROWS;
static unsigned long cviewrows		= CVIEWROWS;

/* FORWARD ROUTINE */
void		setup_screen (char *);
unsigned long	read_card_file (int);
void		select_card ();
long		display_card (unsigned long);
void		scroll_view (unsigned long *, long);
unsigned long	main (int, char *[]);


/* MACROS */
#define errchk(s) if (!(s & 1)) lib$signal(s);
#define call(s)  status = s; errchk(status);

#define init_ddesc(desc)			\
	desc.dsc$w_length = 0;			\
	desc.dsc$b_dtype = DSC$K_DTYPE_T;	\
	desc.dsc$b_class = DSC$K_CLASS_D;	\
	desc.dsc$a_pointer = 0;

/*
 *  Routine:	setup_screen
 *
 *  Functional description:
 *
 *	This routine creates the necessary SMG$ items (pasteboard, virtual
 *	displays, etc.).
 *
 *  Formal parameters:
 *
 *	title	- The title for the menu virtual display
 *
 *  Implicit inputs:
 *
 *	cvd, mvd, pb, kb, various constants
 *
 *  Returns:
 *
 *	Nothing.
 */
void
setup_screen(char *title)
{
   struct dsc$descriptor_d full_title;
   unsigned long status;
   unsigned long x;
   $DESCRIPTOR(copyright_title, COPYRIGHT);

   init_ddesc(full_title);		/* Initialize the descriptor */

   x = strlen(title);
   call(STR$COPY_R (&full_title, (x > MAXSUBJLEN) ? &maxsubjlen : &x, title));
   call(SMG$CREATE_PASTEBOARD (&pb, 0, &pb_rows, &pb_cols));
   cviewrows += (pb_rows - 24);
   mviewrows += (pb_rows - 24);
   call(SMG$SET_CURSOR_MODE (&pb, &smg_m_cursor_off));
   call(SMG$CREATE_VIRTUAL_DISPLAY(&number_of_cards, &maxsubjlen, &mvd,
				   &smg_m_border));
   call(SMG$CREATE_VIEWPORT (&mvd, &one, &one, &mviewrows, &maxsubjlen));
   call(SMG$CREATE_VIRTUAL_DISPLAY(&cvdsize, &maxlinelen, &cvd,
				   &smg_m_border));

   call(SMG$CREATE_VIRTUAL_DISPLAY(&one, &pb_cols, &copyvd, 0));
   x = (pb_cols - copyright_title.dsc$w_length)/2;
   if (x > 0) {
	call(SMG$PUT_CHARS(&copyvd, &copyright_title, &one, &x, 0,
			   &smg_m_reverse));
	call(SMG$PASTE_VIRTUAL_DISPLAY (&copyvd, &pb, &pb_rows, &one));
   }

   call(SMG$CREATE_VIEWPORT (&cvd, &one, &one, &cviewrows, &maxlinelen));
   call(SMG$LABEL_BORDER (&mvd, &full_title, &smg_k_top, 0, &smg_m_reverse));
   call(SMG$CREATE_VIRTUAL_KEYBOARD (&kb));

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

   printf("Reading file into memory....\r");
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

   number_of_cards = buffer[3];

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
 *  Routine:	select_card
 *
 *  Functional description:
 *
 *	This routine presents the menu of card titles and processes keystrokes
 *	from the user.  Valid keys are:
 *
 *		up- and down-arrow, NextScreen, PrevScreen, RETURN, Q, ^Z,
 *		P, N
 *
 *	When RETURN is pressed, the card text for the selected title is
 *	displayed by calling display_card().
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
 *      None.
 *
 */
void
select_card ()
{
  unsigned short key_pressed;
  unsigned short read_key;
  long lines;
  unsigned long current_row;
  static struct dsc$descriptor_s menu_line =
    {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, 0};

  current_row = 1;
  read_key = 1;
  call(SMG$SET_CURSOR_ABS(&mvd, &current_row, &one));
  for (;;)
    {
      if (read_key)
	status = SMG$READ_KEYSTROKE (&kb, &key_pressed);
      switch (key_pressed)
	{
	case SMG$K_TRM_LOWERCASE_P:
	case SMG$K_TRM_UPPERCASE_P:
	case SMG$K_TRM_UP:
	  lines = -1;
	  break;
	case SMG$K_TRM_LOWERCASE_N:
	case SMG$K_TRM_UPPERCASE_N:
	case SMG$K_TRM_DOWN:
	  lines = 1;
	  break;
	case SMG$K_TRM_NEXT_SCREEN:
	  lines = mviewrows;
	  break;
	case SMG$K_TRM_PREV_SCREEN:
	  lines = -mviewrows;
	  break;
	case SMG$K_TRM_CTRLZ:
	case SMG$K_TRM_LOWERCASE_Q:
	case SMG$K_TRM_UPPERCASE_Q:
	  return;
	  break;
	case SMG$K_TRM_CTRLW:
	  call(SMG$REPAINT_SCREEN(&pb));
	  lines = 0;
	  break;
	case SMG$K_TRM_CR:
	  lines = display_card (current_row - 1);
	  /* display_card() can return number of lines to scroll; handle
	     the scrolling if it does. */
	  read_key = lines ? 0 : 1;	/* Read keystroke?  Only if lines=0 */
	  call(SMG$SET_CURSOR_ABS(&mvd, &current_row, &one));
	  break;
	default:
	  lines=0;
	  break;
	}
      /*  If we are supposed to move, rewrite the current line as normal
	  text, scroll the appropriate amount (via scroll_view), and
	  write the new current line in reverse video. */

      if (lines != 0) {
	/* Write current line in normal mode */
	call(SMG$SET_CURSOR_ABS(&mvd, &current_row, &one));
	menu_line.dsc$w_length = strlen(card[current_row-1].subj);
	menu_line.dsc$a_pointer = card[current_row-1].subj;
	call(SMG$PUT_CHARS(&mvd, &menu_line, &current_row, &one, 0,
			   &smg_m_normal));
	call(SMG$SET_CURSOR_ABS(&mvd, &current_row, &one));

	scroll_view (&mvd, lines);

	current_row = SMG$CURSOR_ROW (&mvd);
	call(SMG$SET_CURSOR_ABS(&mvd, &current_row, &one));
	menu_line.dsc$w_length = strlen(card[current_row-1].subj);
	menu_line.dsc$a_pointer = card[current_row-1].subj;
	call(SMG$PUT_CHARS(&mvd, &menu_line, &current_row, &one,
			   0, &smg_m_reverse));
      }
    }
}

/*
 *  Routine:	display_card
 *
 *  Functional description:
 *
 *	This routine is called to display the text of a card in a second
 *	virtual display.  The user can press the following keys:
 *
 *		up- and down-arrow, NextScreen, PrevScreen, P, N, Q, ^Z,
 *
 *	When N or P is pressed, a 1 or -1 is returned to cause select_card
 *	to automatically move to the next card without making the user
 *	press any other key.  Q and ^Z remove the virtual display from the
 *	screen.
 *
 *  Formal parameters:
 *
 *      card_number - the array offset for the card to display
 *
 *  Implicit inputs:
 *
 *      card[], cvd, cvdsize, pb (,etc.)
 *
 *  Returns:
 *
 *      Long = number of lines to scroll in main menu (-1, 0, or 1)
 *
 */
long
display_card (unsigned long card_number)
{
  unsigned short key_pressed;
  unsigned short return_flag;
  long return_stat;
  unsigned long current_row;
  unsigned long number_of_lines;
  struct dsc$descriptor_s text_line;
  int card_size;
  char *ptr;
  char *ptr2;
  long lines;

  text_line.dsc$b_dtype = DSC$K_DTYPE_T;
  text_line.dsc$b_class = DSC$K_CLASS_S;

  /*  Set up the card virtual display  */

  call(SMG$BEGIN_DISPLAY_UPDATE(&cvd));
  call(SMG$CHANGE_VIRTUAL_DISPLAY(&cvd, &cvdsize));
  call(SMG$ERASE_DISPLAY (&cvd));
  text_line.dsc$w_length = strlen(card[card_number].subj);
  text_line.dsc$a_pointer = card[card_number].subj;
  call(SMG$LABEL_BORDER (&cvd, &text_line, &smg_k_top, 0, &smg_m_reverse));

  /*  Now write each line of the card to the virtual display, wrapping
      by words.  The total number of lines is determined by the position
      of the cursor (the row) after all lines have been written.  */

  ptr = card[card_number].text;
  card_size = strlen(ptr);
  while (card_size > 0) {
    if ((ptr2 = (char *) strchr(ptr, '\n')) == 0)
      text_line.dsc$w_length = card_size;
    else
      text_line.dsc$w_length = ptr2 - ptr;
    text_line.dsc$a_pointer = ptr;
    call(SMG$PUT_LINE(&cvd, &text_line, 0, 0, 0, &smg_m_wrap_word));
    ptr = ptr2 + 1;
    card_size -= (text_line.dsc$w_length + 1);
  }
  number_of_lines = SMG$CURSOR_ROW (&cvd);
  call(SMG$CHANGE_VIRTUAL_DISPLAY (&cvd,
				   (number_of_lines < cviewrows) ?
				   &cviewrows : &number_of_lines));

  card_size = (80 - MAXLINELEN) / 2;
  /* Move view port back up to top of menu */
  call(SMG$END_DISPLAY_UPDATE(&cvd));
  call(SMG$CHANGE_VIEWPORT(&cvd, &one, &one, &cviewrows));
  call(SMG$PASTE_VIRTUAL_DISPLAY (&cvd, &pb, &twelve, &card_size));

  call(SMG$SET_CURSOR_ABS(&cvd, &one, &one));
  call(SMG$SET_CURSOR_MODE (&pb, &smg_m_cursor_on));

  current_row = 1;
  return_flag = 0;
  for (;;)
    {
      lines = 0;
      status = SMG$READ_KEYSTROKE (&kb, &key_pressed);
      switch (key_pressed)
	{
	case SMG$K_TRM_UP:
	  lines = -1;
	  break;
	case SMG$K_TRM_DOWN:
	  lines = 1;
	  break;
	case SMG$K_TRM_NEXT_SCREEN:
	  lines = cviewrows;
	  break;
	case SMG$K_TRM_PREV_SCREEN:
	  lines = -cviewrows;
	  break;
	case SMG$K_TRM_UPPERCASE_N:
	case SMG$K_TRM_LOWERCASE_N:
	  return_stat = 1;
	  return_flag = 1;
	  break;
	case SMG$K_TRM_UPPERCASE_P:
	case SMG$K_TRM_LOWERCASE_P:
	  return_stat = -1;
	  return_flag = 1;
	  break;
	case SMG$K_TRM_CTRLZ:
	case SMG$K_TRM_UPPERCASE_Q:
	case SMG$K_TRM_LOWERCASE_Q:
	  return_flag = 1;
	  return_stat = 0;
	  break;
	case SMG$K_TRM_CTRLW:
	  call(SMG$REPAINT_SCREEN(&pb));
	  lines = 0;
	  break;
	}
	      
      if (return_flag) {
	call(SMG$SET_CURSOR_MODE (&pb, &smg_m_cursor_off));
	if (!return_stat)
	  call(SMG$UNPASTE_VIRTUAL_DISPLAY(&cvd, &pb));
	return(return_stat);
	}
      else
	scroll_view (&cvd, lines);
    }
}

/*
 *  Routine:	scroll_view
 *
 *  Functional description:
 *
 *	This routine scrolls a viewport a certain number of lines.  The
 *	number of lines is a signed quantity, where negative means scroll
 *	up the page (backwards) and positive means scroll down the page.
 *	Unlike the SMG$ routines for menus, this routine handles the
 *	automatic wrap-around of the display when the beginning or end is
 *	reached.
 *
 *	It is assumed that the virtual display has a viewport associated with
 *	it.
 *
 *  Formal parameters:
 *
 *      vd	- The virtual display to scroll (passed by reference!)
 *	lines	- The signed number of lines to scroll (passed by value)
 *
 *  Implicit inputs:
 *
 *      various constants
 *
 *  Returns:
 *
 *      None.
 *
 */
void
scroll_view (unsigned long *vd, long lines)
{
  long work_row;
  unsigned long currow;
  unsigned long vrows;
  unsigned long vstart;
  unsigned long vrow;
  unsigned long maxrows;

  call(SMG$GET_DISPLAY_ATTR (vd, &maxrows));
  /* Calculate the row within the viewport that the cursor is on */
  call(SMG$GET_VIEWPORT_CHAR (vd, &vstart, 0, &vrows, 0));
  currow = vrow = SMG$CURSOR_ROW (vd);
  vrow -= vstart;
  vrow++;

  if (lines != 0) {
    work_row = currow + lines;
    if (lines > 0) {
      /*
       *  If we are on the last menu item, scroll back to top
       */
      if (currow == maxrows) {
	
	/* Move view port back up to top of menu */
	call(SMG$CHANGE_VIEWPORT(vd, &one, &one, &vrows));
	
	/* Re-initialize rows */
	work_row = 1;
      }
      else {
	if (work_row >= maxrows) {
	  lines = (long) (maxrows - currow);
	  work_row = maxrows;
	}
	/*  If we need to scroll, do it */
	if ((vrow + lines) > vrows) {
	  if ((work_row > (maxrows - vrows))
	      && lines > 1) {
	    lines = (long) (maxrows - currow - vrows + vrow);
	  }
	  call(SMG$SCROLL_VIEWPORT(vd, &smg_m_up, &lines));
	}
      }
    }
    else {
      
      /* Move backwards through the menu */
      
      if (currow == 1) {
	/* Move view port back up to bottom of menu */
	if (maxrows > vrows) {
	  currow = maxrows - vrows + 1;
	  call(SMG$CHANGE_VIEWPORT(vd, &currow, &one, &vrows));
	}
	else {
	  currow = maxrows;
	  call(SMG$CHANGE_VIEWPORT(vd, &one, &one, &vrows));
	}
	
	/* Re-initialize rows */
	work_row = maxrows;
      }
      else {
	if (work_row < 1) {
	  lines = 1 - currow;
	  work_row = 1;
	}
	/* Moving backwards */
	if ((long) (vrow + lines) < 1) {
	  if (((currow - vrow) < vrows) && (lines < -1)) {
	    lines = (long) (currow-vrow); /* top of viewport */
	  }
	  lines = abs(lines);
	  call(SMG$SCROLL_VIEWPORT(vd, &smg_m_down, &lines));
	}
      }
    }
    call(SMG$SET_CURSOR_ABS(vd, &work_row, &one));
    currow = work_row;
  }
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
  char *default_card_file = "SYS$LOGIN:.CARD";
  char ess[NAM$C_MAXRSS+1];		/* Space for expanded name */
  char title[NAM$C_MAXRSS+1];		/* Space for the menu title */

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

  call(SYS$PARSE (&fab));

  /* Copy the file name for use as the title */
  x = nam.nam$b_name+nam.nam$b_type;
  strncpy(title,nam.nam$l_name,x);
  title[x] = '\0';
  ess[nam.nam$b_esl] = '\0';

  if ((fd=open(ess,0,0)) <= 0) {
    printf("Error opening input file %s\n",ess);
    exit(SS$_ABORT);
  }

  /* Read entire file into memory (storing in array of cards structure) */

  if ((status = read_card_file (fd)) != SS$_NORMAL)
    exit(status);

  close(fd);      /* Go on and close the file */

  setup_screen(title);		/* Create the virtual displays, etc. */

  /* Load up the virtual display with the card subjects */

  menu_line.dsc$b_dtype = DSC$K_DTYPE_T;
  menu_line.dsc$b_class = DSC$K_CLASS_S;
  for (x=0; x < number_of_cards; x++) {
    menu_line.dsc$w_length = strlen(card[x].subj);
    menu_line.dsc$a_pointer = card[x].subj;
    call(SMG$PUT_LINE(&mvd, &menu_line, 0, x ? 0 : &smg_m_reverse));
  }


  x = (80 - MAXSUBJLEN) / 2;
  call(SMG$PASTE_VIRTUAL_DISPLAY (&mvd, &pb, &three, &x));

  select_card();

  call(SMG$DELETE_VIRTUAL_DISPLAY(&mvd));
  call(SMG$DELETE_PASTEBOARD(&pb, &smg_m_erase_pbd));
}
