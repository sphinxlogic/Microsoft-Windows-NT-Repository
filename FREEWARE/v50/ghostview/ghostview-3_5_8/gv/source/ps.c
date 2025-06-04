/*
 * ps.c -- Postscript scanning and copying routines.
 * Copyright (C) 1992  Timothy O. Theisen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Author: Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
*/

/*
 * Added the ps_io_*() routines, rewrote readline(), modified
 * pscopyuntil() and pscopydoc() and eliminated pscopy().
 * The modifications mainly aim at
 *    - elimination of the (line length <= 255) constraint since
 *      there are just too many documents ignoring this requirement. 
 *    - acceptance of '\r' as line terminator as suggested by
 *      Martin Buck (martin-2.buck@student.uni-ulm.de).
 * Johannes Plass, 04/96 (plass@thep.physik.uni-mainz.de)
 *
*/

/*
 * > Believe it or not--even Adobe doesn't know how to produce correct
 * > PS-files. Their Acrobat Reader sometimes starts including other files
 * > with %%BeginFile instead of %%BeginFile: and even more often ends them
 * > with just %%EOF instead of %%EndFile.
 * >    Martin Buck, martin-2.buck@student.uni-ulm.de
 *
 * Therefore we use Martin's ACROREAD_WORKAROUND (thanks for the patch, Martin).
 * ###jp### 04/96
 * 
 */
#define USE_ACROREAD_WORKAROUND

/*
#define MESSAGES
#define MESSAGE_NO_ESC
*/
#include "message.h"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef VMS
#   include <unixio.h>
#   if __VMS_VER < 70000000
#   include "strcasecmp.h"
#   endif
#   define unlink remove
#   define USE_FTELL_FOR_FILEPOS
#else
#   include <string.h>
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef BUFSIZ
#define BUFSIZ 1024
#endif
#include <ctype.h>

#include "paths.h"
#include INC_X11(Intrinsic.h)
#include INC_X11(Xos.h)                /* #includes the appropriate <string.h> */
#include "xfuncs.h"
/* RLD
#include INC_X11(Xfuncs.h)
*/

#include "types.h"
#include "file.h"
#include "misc.h"
#include "ps.h"
#include "note.h"
#include "d_memdebug.h"
extern Media *gv_medias;

#ifdef BSD4_2
#define memset(a,b,c) bzero(a,c)
#endif

/* length calculates string length at compile time */
/* can only be used with character constants */
#define length(a)       (sizeof((a))-1)
#define iscomment(a, b) (strncmp((a), (b), length((b))) == 0)
#define DSCcomment(a)   ((a)[0] == '%' && (a)[1] == '%')

/* list of standard paper sizes from Adobe's PPD. */

#if 0
struct documentmedia papersizes[] = {
    {"BBox",	         0,  0},
    {"Letter",		 612,  792},
    {"LetterSmall",	 612,  792},
    {"Legal",		 612, 1008},
    {"Statement",	 396,  612},
    {"Tabloid",		 792, 1224},
    {"Ledger",		1224,  792},
    {"Executive",	 540,  720},
    {"A0",		2384, 3370},
    {"A1",		1684, 2384},
    {"A2",		1191, 1684},
    {"A3",		 842, 1191},
    {"A4",		 595,  842},
    {"A4Small",		 595,  842},
    {"A5",		 420,  595},
    {"B4",		 729, 1032},
    {"B5",		 516,  729},
    {"Folio",		 612,  936},
    {"Quarto",		 610,  780},
    {"10x14",		 720, 1008},
    {  NULL,		   0,    0}
};
#endif


#if NeedFunctionPrototypes
#   define PT(aaa) aaa
#else 
#   define PT(aaa) ()
#endif

/*--------------------------------------------------*/
/* Declarations for ps_io_*() routines. */

typedef struct FileDataStruct_ *FileData;

typedef struct FileDataStruct_ {
   FILE *file;           /* file */
   int   file_desc;      /* file descriptor corresponding to file */
   int   filepos;        /* file position corresponding to the start of the line */
   char *buf;            /* buffer */
   int   buf_size;       /* size of buffer */
   int   buf_end;        /* last char in buffer given as offset to buf */
   int   line_begin;     /* start of the line given as offset to buf */
   int   line_end;       /* end of the line given as offset to buf */
   int   line_len;       /* length of line, i.e. (line_end-line_begin) */
   char  line_termchar;  /* char exchanged for a '\0' at end of line */
   int   status;         /* 0 = okay, 1 = failed */
} FileDataStruct;

static FileData ps_io_init PT((FILE *));
static void     ps_io_exit PT((FileData));
static char    *ps_io_fgetchars PT((FileData, int));
static int      ps_io_fseek PT((FileData, int));
static int      ps_io_ftell PT((FileData));

static char    *readline PT((FileData, char **, long *, unsigned int *));
static char    *gettextline PT((char *));
static char    *gettext PT((char *,char **));
static int      blank PT((char *));
static char    *pscopyuntil PT((FileData,FILE *,long,long,char *));

static char    *skipped_line = "% ps_io_fgetchars: skipped line";
static char    *empty_string = "";

/*--------------------------------------------------*/

/*
 *	psscan -- scan the PostScript file for document structuring comments.
 *
 *	This scanner is designed to retrieve the information necessary for
 *	the ghostview previewer.  It will scan files that conform to any
 *	version (1.0, 2.0, 2.1, or 3.0) of the document structuring conventions.
 *	It does not really care which version of comments the file contains.
 *	(The comments are largely upward compatible.)  It will scan a number
 *	of non-conforming documents.  (You could have part of the document
 *	conform to V2.0 and the rest conform to V3.0.  It would be similar
 *	to the DC-2 1/2+, it would look funny but it can still fly.)
 *
 *	This routine returns a pointer to the document structure.
 *	The structure contains the information relevant to previewing.
 *      These include EPSF flag (to tell if the file is a encapsulated figure),
 *      Page Media (for the Page Size), Bounding Box (to minimize backing
 *      pixmap size or determine window size for encapsulated PostScript), 
 *      Orientation of Paper (for default transformation matrix), and
 *      Page Order.  The title and CreationDate are also retrieved to
 *      help identify the document.
 *
 *      The following comments are examined:
 *
 *      Header section: 
 *      Must start with %!PS-Adobe-.  Version numbers ignored.
 *
 *      %!PS-Adobe-* [EPSF*] (changed EPSF-* to EPSF* to do XFig a favor ...###jp###)
 *      %%BoundingBox: <int> <int> <int> <int>|(atend)
 *      %%CreationDate: <textline>
 *      %%Orientation: Portrait|Landscape|(atend)
 *      %%Pages: <uint> [<int>]|(atend)
 *      %%PageOrder: Ascend|Descend|Special|(atend)
 *      %%Title: <textline>
 *      %%DocumentMedia: <text> <real> <real> <real> <text> <text>
 *      %%DocumentPaperSizes: <text>
 *      %%EndComments
 *
 *      Note: Either the 3.0 or 2.0 syntax for %%Pages is accepted.
 *            Also either the 2.0 %%DocumentPaperSizes or the 3.0
 *            %%DocumentMedia comments are accepted as well.
 *
 *      The header section ends either explicitly with %%EndComments or
 *      implicitly with any line that does not begin with %X where X is
 *      a not whitespace character.
 *
 *      If the file is encapsulated PostScript the optional Preview section
 *      is next:
 *
 *      %%BeginPreview
 *      %%EndPreview
 *
 *      This section explicitly begins and ends with the above comments.
 *
 *      Next the Defaults section for version 3 page defaults:
 *
 *      %%BeginDefaults
 *      %%PageBoundingBox: <int> <int> <int> <int>
 *      %%PageOrientation: Portrait|Landscape
 *      %%PageMedia: <text>
 *      %%EndDefaults
 *
 *      This section explicitly begins and ends with the above comments.
 *
 *      The prolog section either explicitly starts with %%BeginProlog or
 *      implicitly with any nonblank line.
 *
 *      %%BeginProlog
 *      %%EndProlog
 *
 *      The Prolog should end with %%EndProlog, however the proglog implicitly
 *      ends when %%BeginSetup, %%Page, %%Trailer or %%EOF are encountered.
 *
 *      The Setup section is where the version 2 page defaults are found.
 *      This section either explicitly begins with %%BeginSetup or implicitly
 *      with any nonblank line after the Prolog.
 *
 *      %%BeginSetup
 *      %%PageBoundingBox: <int> <int> <int> <int>
 *      %%PageOrientation: Portrait|Landscape
 *      %%PaperSize: <text>
 *      %%EndSetup
 *
 *      The Setup should end with %%EndSetup, however the setup implicitly
 *      ends when %%Page, %%Trailer or %%EOF are encountered.
 *
 *      Next each page starts explicitly with %%Page and ends implicitly with
 *      %%Page or %%Trailer or %%EOF.  The following comments are recognized:
 *
 *      %%Page: <text> <uint>
 *      %%PageBoundingBox: <int> <int> <int> <int>|(atend)
 *      %%PageOrientation: Portrait|Landscape
 *      %%PageMedia: <text>
 *      %%PaperSize: <text>
 *
 *      The trailer section start explicitly with %%Trailer and end with %%EOF.
 *      The following comment are examined with the proper (atend) notation
 *      was used in the header:
 *
 *      %%Trailer
 *      %%BoundingBox: <int> <int> <int> <int>|(atend)
 *      %%Orientation: Portrait|Landscape|(atend)
 *      %%Pages: <uint> [<int>]|(atend)
 *      %%PageOrder: Ascend|Descend|Special|(atend)
 *      %%EOF
 *
 *
 *  + A DC-3 received severe damage to one of its wings.  The wing was a total
 *    loss.  There was no replacement readily available, so the mechanic
 *    installed a wing from a DC-2.
 */

#ifndef VMS
#  define ps_system system
#else

/*############################################################
 * ps_system        (VMS only, otherwise same as system())
 * Similar to system(), except it allows I/O redirection with <in >out
 * at the end of the line, and returns a more Unix-compatible status (0=OK).
 * The I/O redirection is very rudimentary. >> and | are NOT supported.
 * No error checking is performed, though hopefully the command will return
 * a reasonable error message when it can't access the file.
 * < or > in the command (eg. in directory spec), unless followed by another
 * < or >, will always be interpreted as I/O redirection, so be careful!
 *############################################################*/

int ps_system(cmd)
  char *cmd;
{
  char *inpf=NULL,*outf=NULL,*cf=NULL,*s,*t;
  int status;

  if (!cmd) cmd="";
  cmd = PS_XtNewString(cmd);
  s = strrchr(cmd,'>'); if (s) { outf = s+1; *s='\n'; }
  s = strrchr(cmd,'<'); if (s) { inpf = s+1; *s='\n'; }
  while ((s=strrchr(cmd,'\n'))) *s='\0';
  if (outf) {
    for (s=t=outf; *s; s++) if (!isspace(*s)) *t++=*s; 
    *t='\0';
    if (!(*outf)) outf=NULL;
  }
  if (inpf) {
    for (s=t=inpf; *s; s++) if (!isspace(*s)) *t++=*s;
    *t='\0';
    if (!(*inpf)) inpf=NULL;
  }
  if (outf || inpf) {
    FILE *com;
    cf=file_getTmpFilename (NULL, outf ? outf : inpf);
    com=fopen(cf, "w");
    if (!com) return 1;
    if (inpf) fprintf(com, "$DEFINE_/NOLOG SYS$INPUT  %s\n",inpf);
    if (outf) fprintf(com, "$DEFINE_/NOLOG SYS$OUTPUT %s\n",outf);
    fprintf(com,"$%s\n",cmd);
    fclose(com);
    PS_XtFree(cmd);
    cmd = PS_XtMalloc((1+strlen(cf)+1)*sizeof(char));
    sprintf(cmd,"@%s",cf);
  }
  status=system(cmd);
  if (cf) unlink(cf);
  PS_XtFree(cmd);
  return((status&7)==1 ? 0 : status);
}
#endif

/*-----------------------------------------------------------*/

static void ps_dynMemExhaust()
{
   fprintf(stderr,"Fatal Error: Dynamic memory exhausted.\n");
   exit(EXIT_STATUS_FATAL);
}
#define CHECK_MALLOCED(aaa)  if (!(aaa)) ps_dynMemExhaust()

/*###########################################################*/
/* psscan */
/*###########################################################*/

struct document *
psscan(fileP,filename,filename_raw,filename_dscP,cmd_scan_pdf,filename_uncP,cmd_uncompress,scanstyle)
    FILE **fileP;
    char *filename;
    char *filename_raw;
    char **filename_dscP;
    char *cmd_scan_pdf;
    char **filename_uncP;
    char *cmd_uncompress;
    int scanstyle;
{
    FILE *file;
    struct document *doc;
    int bb_set = NONE;
    int pages_set = NONE;
    int page_order_set = NONE;
    int orientation_set = NONE;
    int page_bb_set = NONE;
    int page_media_set = NONE;
    int preread;		/* flag which tells the readline isn't needed */
    int i;
    unsigned int maxpages = 0;
    unsigned int nextpage = 1;	/* Next expected page */
    unsigned int thispage;
    int ignore = 0;		/* whether to ignore page ordinals */
    char *label;
    char *line;
                           	/* 255 characters + 1 newline + 1 NULL */
    char text[PSLINELENGTH];	/* Temporary storage for text */
    long position;		/* Position of the current line */
    long beginsection;		/* Position of the beginning of the section */
    unsigned int line_len; 	/* Length of the current line */
    unsigned int section_len;	/* Place to accumulate the section length */
    char *next_char;		/* 1st char after text returned by gettext() */
    char *cp;
    Media dmp;
    FileData fd;
    int respect_eof;            /* Derived from the scanstyle argument.
                                   If set to 0 EOF comments will be ignored,
                                   if set to 1 they will be taken seriously.
                                   Purpose; Out there are many documents which 
                                   include other DSC conforming documents without
                                   without enclosing them by 'BeginDocument' and
                                   'EndDocument' comments. This may cause fake EOF 
				   comments to appear in the body of a page.
				   Similarly, if respect_eof is set to false
				   'Trailer' comments are ignored except of the
				   last one found in the document.
				*/
    int ignore_dsc;             /* Derived from scanstyle.
				   If set the document structure will be ignored.
				*/

    BEGINMESSAGE(psscan)

    if (cmd_uncompress) {
      char b[2];
      if (!(fread(b, sizeof(char),2, *fileP) == 2)
	  || b[0] != '\037' || (b[1] != '\235' && b[1] != '\213')) {
        rewind(*fileP);
	cmd_uncompress=NULL;
      }
    }
    if (cmd_uncompress) {
      struct document *retval = NULL;
      FILE *tmpfile = (FILE*)NULL;
      char *filename_unc;
      char cmd[512];
      char s[512];
      filename_unc=file_getTmpFilename(NULL,filename_raw);
      sprintf(cmd,cmd_uncompress,filename,filename_unc);
      INFMESSAGE(is compressed)
      INFSMESSAGE(uncompress command,cmd)
      if (ps_system(cmd) || file_fileIsNotUseful(filename_unc)) {
	INFMESSAGE(uncompressing failed)
unc_exec_failed:
	sprintf(s,"Execution of\n%s\nfailed.",cmd);
#if 0
unc_failed:
#endif
	NotePopupShowMessage(s);
	if (tmpfile) fclose(tmpfile);
	unlink(filename_unc);
unc_ok:
	GV_XtFree(filename_unc);
	ENDMESSAGE(psscan)
        return(retval);
      }
      tmpfile = fopen(filename_unc, "r");
      if (!tmpfile) goto unc_exec_failed;
      fclose(*fileP);
      *fileP = tmpfile;
      retval = psscan(fileP,filename_unc,filename_raw,filename_dscP,cmd_scan_pdf,NULL,NULL,scanstyle);
#if 0
      if (!retval) {
	sprintf(s,"333 Scanning\n%s\nfailed.",filename_unc);
	goto unc_failed;
      }
#endif
      *filename_uncP = (char*)GV_XtNewString(filename_unc);
      goto unc_ok;
    }

    respect_eof = (scanstyle & SCANSTYLE_IGNORE_EOF) ? 0 : 1;
    ignore_dsc = (scanstyle & SCANSTYLE_IGNORE_DSC) ? 1 : 0;
    file = *fileP;

    if (ignore_dsc) {
      INFMESSAGE(ignoring DSC)
      ENDMESSAGE(psscan)
      return(NULL);
    }

    fd =  ps_io_init(file);
    if (!readline(fd,&line, &position, &line_len)) {
	fprintf(stderr, "Warning: empty file.\n");
        ENDMESSAGE(psscan)
        ps_io_exit(fd);
	return(NULL);
    }

    /* Header comments */

    if (line_len>1 && (iscomment(line,"%!PS-Adobe-") || iscomment(line + 1,"%!PS-Adobe-"))) {
      INFMESSAGE(found "PS-Adobe-" comment)
      doc = (struct document *) PS_malloc(sizeof(struct document));
      CHECK_MALLOCED(doc);
      memset(doc, 0, sizeof(struct document));
      sscanf(line, "%*s %s", text);
      /*###jp###*/
      /*doc->epsf = iscomment(text, "EPSF-");*/
      doc->epsf = iscomment(text, "EPSF");
      doc->beginheader = position;
      section_len = line_len;
    } else if (iscomment(line,"%PDF-") && cmd_scan_pdf) {
      struct document *retval = NULL;
      FILE *tmpfile = (FILE*)NULL;
      char *filename_dsc;
      char cmd[512];
      char s[512];
      filename_dsc=file_getTmpFilename(NULL,filename_raw);
      sprintf(cmd,cmd_scan_pdf,filename,filename_dsc);
      INFMESSAGE(is PDF)
      INFSMESSAGE(scan command,cmd)
#ifdef VMS
      if ((system(cmd)&7)!=1 || file_fileIsNotUseful(filename_dsc)) {
#else
      if (system(cmd) || file_fileIsNotUseful(filename_dsc)) {
#endif
	INFMESSAGE(scan subprocess failed)
scan_exec_failed:
	sprintf(s,"Execution of\n%s\nfailed.",cmd);
scan_failed:
	NotePopupShowMessage(s);
	if (tmpfile) fclose(tmpfile);
	unlink(filename_dsc);
scan_ok:
	GV_XtFree(filename_dsc);
        ps_io_exit(fd);
	ENDMESSAGE(psscan)
        return(retval);
      }
      tmpfile = fopen(filename_dsc, "r");
      if (!tmpfile) goto scan_exec_failed;
      fclose(*fileP);
      *fileP = tmpfile;
      retval = psscan(fileP,filename_dsc,filename_raw,filename_dscP,cmd_scan_pdf,NULL,NULL,scanstyle);
      if (!retval) {
	sprintf(s,"Scanning\n%s\nfailed.",filename_dsc);
	goto scan_failed;
      }
      *filename_dscP = (char*)GV_XtNewString(filename_dsc);
      goto scan_ok;
    } else {
      INFMESSAGE(unable to classify document)
      ENDMESSAGE(psscan)
      ps_io_exit(fd);
      return(NULL);
    }
    
    preread = 0;
    while (preread || readline(fd, &line, &position, &line_len)) {
	if (!preread) section_len += line_len;
	preread = 0;
	if (line[0] != '%' ||
	    iscomment(line+1, "%EndComments") ||
	    line[1] == ' ' || line[1] == '\t' || line[1] == '\n' ||
	    !isprint(line[1])) {
	    break;
	} else if (line[1] != '%') {
	    /* Do nothing */
	} else if (doc->title == NULL && iscomment(line+2, "Title:")) {
	    doc->title = gettextline(line+length("%%Title:"));
	} else if (doc->date == NULL && iscomment(line+2, "CreationDate:")) {
	    doc->date = gettextline(line+length("%%CreationDate:"));
	} else if (bb_set == NONE && iscomment(line+2, "BoundingBox:")) {
	    sscanf(line+length("%%BoundingBox:"), "%s", text);
	    if (strcmp(text, "(atend)") == 0) {
		bb_set = ATEND;
	    } else {
		if (sscanf(line+length("%%BoundingBox:"), "%d %d %d %d",
			   &(doc->boundingbox[LLX]),
			   &(doc->boundingbox[LLY]),
			   &(doc->boundingbox[URX]),
			   &(doc->boundingbox[URY])) == 4)
		    bb_set = 1;
		else {
		    float fllx, flly, furx, fury;
		    if (sscanf(line+length("%%BoundingBox:"), "%f %f %f %f",
			       &fllx, &flly, &furx, &fury) == 4) {
			bb_set = 1;
			doc->boundingbox[LLX] = fllx;
			doc->boundingbox[LLY] = flly;
			doc->boundingbox[URX] = furx;
			doc->boundingbox[URY] = fury;
			if (fllx < doc->boundingbox[LLX])
			    doc->boundingbox[LLX]--;
			if (flly < doc->boundingbox[LLY])
			    doc->boundingbox[LLY]--;
			if (furx > doc->boundingbox[URX])
			    doc->boundingbox[URX]++;
			if (fury > doc->boundingbox[URY])
			    doc->boundingbox[URY]++;
		    }
		}
	    }
	} else if (orientation_set == NONE &&
		   iscomment(line+2, "Orientation:")) {
	    sscanf(line+length("%%Orientation:"), "%s", text);
	    if (strcmp(text, "(atend)") == 0) {
		orientation_set = ATEND;
	    } else if (strcmp(text, "Portrait") == 0) {
		doc->orientation = PORTRAIT;
		orientation_set = 1;
	    } else if (strcmp(text, "Landscape") == 0) {
		doc->orientation = LANDSCAPE;
		orientation_set = 1;
	    }
	} else if (page_order_set == NONE && iscomment(line+2, "PageOrder:")) {
	    sscanf(line+length("%%PageOrder:"), "%s", text);
	    if (strcmp(text, "(atend)") == 0) {
		page_order_set = ATEND;
	    } else if (strcmp(text, "Ascend") == 0) {
		doc->pageorder = ASCEND;
		page_order_set = 1;
	    } else if (strcmp(text, "Descend") == 0) {
		doc->pageorder = DESCEND;
		page_order_set = 1;
	    } else if (strcmp(text, "Special") == 0) {
		doc->pageorder = SPECIAL;
		page_order_set = 1;
	    }
	} else if (pages_set == NONE && iscomment(line+2, "Pages:")) {
	    sscanf(line+length("%%Pages:"), "%s", text);
	    if (strcmp(text, "(atend)") == 0) {
		pages_set = ATEND;
	    } else {
		switch (sscanf(line+length("%%Pages:"), "%d %d",
			       &maxpages, &i)) {
		    case 2:
			if (page_order_set == NONE) {
			    if (i == -1) {
				doc->pageorder = DESCEND;
				page_order_set = 1;
			    } else if (i == 0) {
				doc->pageorder = SPECIAL;
				page_order_set = 1;
			    } else if (i == 1) {
				doc->pageorder = ASCEND;
				page_order_set = 1;
			    }
			}
		    case 1:
			if (maxpages > 0) {
			    doc->pages = (struct page *) PS_calloc(maxpages,
							   sizeof(struct page));
                            CHECK_MALLOCED(doc->pages);
			}
		}
	    }
	} else if (doc->nummedia == NONE &&
		   iscomment(line+2, "DocumentMedia:")) {
	    float w, h;
	    doc->media = (Media)
			 PS_malloc(sizeof (MediaStruct));
            CHECK_MALLOCED(doc->media);
	    doc->media[0].name = gettext(line+length("%%DocumentMedia:"),
					 &next_char);
	    if (doc->media[0].name != NULL) {
		if (sscanf(next_char, "%f %f", &w, &h) == 2) {
		    doc->media[0].width = w + 0.5;
		    doc->media[0].height = h + 0.5;
		}
		if (doc->media[0].width != 0 && doc->media[0].height != 0)
		    doc->nummedia = 1;
		else
		    PS_free(doc->media[0].name);
	    }
	    preread=1;
	    while (readline(fd, &line, &position, &line_len) &&
		   DSCcomment(line) && iscomment(line+2, "+")) {
		section_len += line_len;
		doc->media = (Media)
			     PS_realloc(doc->media,
				     (doc->nummedia+1)*
				     sizeof (MediaStruct));
                CHECK_MALLOCED(doc->media);
		doc->media[doc->nummedia].name = gettext(line+length("%%+"),
							 &next_char);
		if (doc->media[doc->nummedia].name != NULL) {
		    if (sscanf(next_char, "%f %f", &w, &h) == 2) {
			doc->media[doc->nummedia].width = w + 0.5;
			doc->media[doc->nummedia].height = h + 0.5;
		    }
		    if (doc->media[doc->nummedia].width != 0 &&
			doc->media[doc->nummedia].height != 0) doc->nummedia++;
		    else
			PS_free(doc->media[doc->nummedia].name);
		}
	    }
	    section_len += line_len;
	    if (doc->nummedia != 0) doc->default_page_media = doc->media;
	} else if (doc->nummedia == NONE &&
		   iscomment(line+2, "DocumentPaperSizes:")) {

	    doc->media = (Media)
			 PS_malloc(sizeof (MediaStruct));
            CHECK_MALLOCED(doc->media);
	    doc->media[0].name = gettext(line+length("%%DocumentPaperSizes:"),
					 &next_char);
	    if (doc->media[0].name != NULL) {
		doc->media[0].width = 0;
		doc->media[0].height = 0;
		for (i=0; gv_medias[i]; i++) {
 		    dmp = gv_medias[i];
		    /* Note: Paper size comment uses down cased paper size
		     * name.  Case insensitive compares are only used for
		     * PaperSize comments.
		     */
		    if (strcasecmp(doc->media[0].name, dmp->name) == 0) {
			PS_free(doc->media[0].name);
			doc->media[0].name = (char *)PS_malloc(strlen(dmp->name)+1);
                        CHECK_MALLOCED(doc->media[0].name);
			strcpy(doc->media[0].name, dmp->name);
			doc->media[0].width = dmp->width;
			doc->media[0].height = dmp->height;
			break;
		    }
		}
		if (doc->media[0].width != 0 && doc->media[0].height != 0)
		    doc->nummedia = 1;
		else
		    PS_free(doc->media[0].name);
	    }
	    while ((cp = gettext(next_char, &next_char))) {
		doc->media = (Media)
			     PS_realloc(doc->media,
				     (doc->nummedia+1)*
				     sizeof (MediaStruct));
                CHECK_MALLOCED(doc->media);
		doc->media[doc->nummedia].name = cp;
		doc->media[doc->nummedia].width = 0;
		doc->media[doc->nummedia].height = 0;
		for (i=0; gv_medias[i]; i++) {
		   dmp = gv_medias[i];
		    /* Note: Paper size comment uses down cased paper size
		     * name.  Case insensitive compares are only used for
		     * PaperSize comments.
		     */
		    if (strcasecmp(doc->media[doc->nummedia].name,
			       dmp->name) == 0) {
			PS_free(doc->media[doc->nummedia].name);
			doc->media[doc->nummedia].name =
				(char *)PS_malloc(strlen(dmp->name)+1);
                        CHECK_MALLOCED(doc->media[doc->nummedia].name);
			strcpy(doc->media[doc->nummedia].name, dmp->name);
			doc->media[doc->nummedia].width = dmp->width;
			doc->media[doc->nummedia].height = dmp->height;
			break;
		    }
		}
		if (doc->media[doc->nummedia].width != 0 &&
		    doc->media[doc->nummedia].height != 0) doc->nummedia++;
		else
		    PS_free(doc->media[doc->nummedia].name);
	    }
	    preread=1;
	    while (readline(fd, &line, &position, &line_len) &&
		   DSCcomment(line) && iscomment(line+2, "+")) {
		section_len += line_len;
		next_char = line + length("%%+");
		while ((cp = gettext(next_char, &next_char))) {
		    doc->media = (Media)
				 PS_realloc(doc->media,
					 (doc->nummedia+1)*
					 sizeof (MediaStruct));
                    CHECK_MALLOCED(doc->media);
		    doc->media[doc->nummedia].name = cp;
		    doc->media[doc->nummedia].width = 0;
		    doc->media[doc->nummedia].height = 0;
		    for (i=0; gv_medias[i]; i++) {
    		        dmp = gv_medias[i];
			/* Note: Paper size comment uses down cased paper size
			 * name.  Case insensitive compares are only used for
			 * PaperSize comments.
			 */
			if (strcasecmp(doc->media[doc->nummedia].name,
				   dmp->name) == 0) {
			    doc->media[doc->nummedia].width = dmp->width;
			    doc->media[doc->nummedia].height = dmp->height;
			    break;
			}
		    }
		    if (doc->media[doc->nummedia].width != 0 &&
			doc->media[doc->nummedia].height != 0) doc->nummedia++;
		    else
			PS_free(doc->media[doc->nummedia].name);
		}
	    }
	    section_len += line_len;
	    if (doc->nummedia != 0) doc->default_page_media = doc->media;
	}
    }

    if (DSCcomment(line) && iscomment(line+2, "EndComments")) {
	readline(fd, &line, &position, &line_len);
	section_len += line_len;
    }
    doc->endheader = position;
    doc->lenheader = section_len - line_len;

    /* Optional Preview comments for encapsulated PostScript files */ 

    beginsection = position;
    section_len = line_len;
    while (blank(line) && readline(fd, &line, &position, &line_len)) {
	section_len += line_len;
    }

    if (doc->epsf && DSCcomment(line) && iscomment(line+2, "BeginPreview")) {
	doc->beginpreview = beginsection;
	beginsection = 0;
	while (readline(fd, &line, &position, &line_len) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndPreview"))) {
	    section_len += line_len;
	}
	section_len += line_len;
	readline(fd, &line, &position, &line_len);
	section_len += line_len;
	doc->endpreview = position;
	doc->lenpreview = section_len - line_len;
    }

    /* Page Defaults for Version 3.0 files */

    if (beginsection == 0) {
	beginsection = position;
	section_len = line_len;
    }
    while (blank(line) && readline(fd, &line, &position, &line_len)) {
	section_len += line_len;
    }

    if (DSCcomment(line) && iscomment(line+2, "BeginDefaults")) {
	doc->begindefaults = beginsection;
	beginsection = 0;
	while (readline(fd, &line, &position, &line_len) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndDefaults"))) {
	    section_len += line_len;
	    if (!DSCcomment(line)) {
		/* Do nothing */
	    } else if (doc->default_page_orientation == NONE &&
		iscomment(line+2, "PageOrientation:")) {
		sscanf(line+length("%%PageOrientation:"), "%s", text);
		if (strcmp(text, "Portrait") == 0) {
		    doc->default_page_orientation = PORTRAIT;
		} else if (strcmp(text, "Landscape") == 0) {
		    doc->default_page_orientation = LANDSCAPE;
		}
	    } else if (page_media_set == NONE &&
		       iscomment(line+2, "PageMedia:")) {
		cp = gettext(line+length("%%PageMedia:"), NULL);
		for (dmp = doc->media, i=0; i<doc->nummedia; i++, dmp++) {
		    if (strcmp(cp, dmp->name) == 0) {
			doc->default_page_media = dmp;
			page_media_set = 1;
			break;
		    }
		}
		PS_free(cp);
	    } else if (page_bb_set == NONE &&
		       iscomment(line+2, "PageBoundingBox:")) {
		if (sscanf(line+length("%%PageBoundingBox:"), "%d %d %d %d",
			   &(doc->default_page_boundingbox[LLX]),
			   &(doc->default_page_boundingbox[LLY]),
			   &(doc->default_page_boundingbox[URX]),
			   &(doc->default_page_boundingbox[URY])) == 4)
		    page_bb_set = 1;
		else {
		    float fllx, flly, furx, fury;
		    if (sscanf(line+length("%%PageBoundingBox:"), "%f %f %f %f",
			       &fllx, &flly, &furx, &fury) == 4) {
			page_bb_set = 1;
			doc->default_page_boundingbox[LLX] = fllx;
			doc->default_page_boundingbox[LLY] = flly;
			doc->default_page_boundingbox[URX] = furx;
			doc->default_page_boundingbox[URY] = fury;
			if (fllx < doc->default_page_boundingbox[LLX])
			    doc->default_page_boundingbox[LLX]--;
			if (flly < doc->default_page_boundingbox[LLY])
			    doc->default_page_boundingbox[LLY]--;
			if (furx > doc->default_page_boundingbox[URX])
			    doc->default_page_boundingbox[URX]++;
			if (fury > doc->default_page_boundingbox[URY])
			    doc->default_page_boundingbox[URY]++;
		    }
		}
	    }
	}
	section_len += line_len;
	readline(fd, &line, &position, &line_len);
	section_len += line_len;
	doc->enddefaults = position;
	doc->lendefaults = section_len - line_len;
    }

    /* Document Prolog */

    if (beginsection == 0) {
	beginsection = position;
	section_len = line_len;
    }
    while (blank(line) && readline(fd, &line, &position, &line_len)) {
	section_len += line_len;
    }

    if (!(DSCcomment(line) &&
	  (iscomment(line+2, "BeginSetup") ||
	   iscomment(line+2, "Page:") ||
	   iscomment(line+2, "Trailer") ||
	   iscomment(line+2, "EOF")))) {
	doc->beginprolog = beginsection;
	beginsection = 0;
	preread = 1;

	while ((preread ||
		readline(fd, &line, &position, &line_len)) &&
	       !(DSCcomment(line) &&
	         (iscomment(line+2, "EndProlog") ||
	          iscomment(line+2, "BeginSetup") ||
	          iscomment(line+2, "Page:") ||
	          iscomment(line+2, "Trailer") ||
	          iscomment(line+2, "EOF")))) {
	    if (!preread) section_len += line_len;
	    preread = 0;
	}
	section_len += line_len;
	if (DSCcomment(line) && iscomment(line+2, "EndProlog")) {
	    readline(fd, &line, &position, &line_len);
	    section_len += line_len;
	}
	doc->endprolog = position;
	doc->lenprolog = section_len - line_len;
    }

    /* Document Setup,  Page Defaults found here for Version 2 files */

    if (beginsection == 0) {
	beginsection = position;
	section_len = line_len;
    }
    while (blank(line) && readline(fd, &line, &position, &line_len)) {
	section_len += line_len;
    }

    if (!(DSCcomment(line) &&
	  (iscomment(line+2, "Page:") ||
	   iscomment(line+2, "Trailer") ||
           (respect_eof && iscomment(line+2, "EOF"))))) {
	doc->beginsetup = beginsection;
	beginsection = 0;
	preread = 1;
	while ((preread ||
		readline(fd, &line, &position, &line_len)) &&
	       !(DSCcomment(line) &&
	         (iscomment(line+2, "EndSetup") ||
	          iscomment(line+2, "Page:") ||
	          iscomment(line+2, "Trailer") ||
	          (respect_eof && iscomment(line+2, "EOF"))))) {
	    if (!preread) section_len += line_len;
	    preread = 0;
	    if (!DSCcomment(line)) {
		/* Do nothing */
	    } else if (doc->default_page_orientation == NONE &&
		iscomment(line+2, "PageOrientation:")) {
		sscanf(line+length("%%PageOrientation:"), "%s", text);
		if (strcmp(text, "Portrait") == 0) {
		    doc->default_page_orientation = PORTRAIT;
		} else if (strcmp(text, "Landscape") == 0) {
		    doc->default_page_orientation = LANDSCAPE;
		}
	    } else if (page_media_set == NONE &&
		       iscomment(line+2, "PaperSize:")) {
		cp = gettext(line+length("%%PaperSize:"), NULL);
		for (dmp = doc->media, i=0; i<doc->nummedia; i++, dmp++) {
		    /* Note: Paper size comment uses down cased paper size
		     * name.  Case insensitive compares are only used for
		     * PaperSize comments.
		     */
		    if (strcasecmp(cp, dmp->name) == 0) {
			doc->default_page_media = dmp;
			page_media_set = 1;
			break;
		    }
		}
		PS_free(cp);
	    } else if (page_bb_set == NONE &&
		       iscomment(line+2, "PageBoundingBox:")) {
		if (sscanf(line+length("%%PageBoundingBox:"), "%d %d %d %d",
			   &(doc->default_page_boundingbox[LLX]),
			   &(doc->default_page_boundingbox[LLY]),
			   &(doc->default_page_boundingbox[URX]),
			   &(doc->default_page_boundingbox[URY])) == 4)
		    page_bb_set = 1;
		else {
		    float fllx, flly, furx, fury;
		    if (sscanf(line+length("%%PageBoundingBox:"), "%f %f %f %f",
			       &fllx, &flly, &furx, &fury) == 4) {
			page_bb_set = 1;
			doc->default_page_boundingbox[LLX] = fllx;
			doc->default_page_boundingbox[LLY] = flly;
			doc->default_page_boundingbox[URX] = furx;
			doc->default_page_boundingbox[URY] = fury;
			if (fllx < doc->default_page_boundingbox[LLX])
			    doc->default_page_boundingbox[LLX]--;
			if (flly < doc->default_page_boundingbox[LLY])
			    doc->default_page_boundingbox[LLY]--;
			if (furx > doc->default_page_boundingbox[URX])
			    doc->default_page_boundingbox[URX]++;
			if (fury > doc->default_page_boundingbox[URY])
			    doc->default_page_boundingbox[URY]++;
		    }
		}
	    }
	}
	section_len += line_len;
	if (DSCcomment(line) && iscomment(line+2, "EndSetup")) {
	    readline(fd, &line, &position, &line_len);
	    section_len += line_len;
	}
	doc->endsetup = position;
	doc->lensetup = section_len - line_len;
    }

    /* BEGIN Windows NT fix ###jp###
       Mark Pfeifer (pfeiferm%ppddev@comet.cmis.abbott.com) told me
       about problems when viewing Windows NT 3.51 generated postscript
       files with gv. He found that the relevant postscript files
       show important postscript code after the '%%EndSetup' and before
       the first page comment '%%Page: x y'.
    */
    if (doc->beginsetup) {
      while (!(DSCcomment(line) &&
	      (iscomment(line+2, "EndSetup") ||
	      (iscomment(line+2, "Page:") ||
	       iscomment(line+2, "Trailer") ||
	       (respect_eof && iscomment(line+2, "EOF"))))) &&
             (readline(fd, &line, &position, &line_len))) {
        section_len += line_len;
        doc->lensetup = section_len - line_len;
	doc->endsetup = position;
      }
    }
    /* END Windows NT fix ###jp##*/

    /* Individual Pages */

    if (beginsection == 0) {
	beginsection = position;
	section_len = line_len;
    }
    while (blank(line) && readline(fd, &line, &position, &line_len)) {
	section_len += line_len;
    }


newpage:
    while (DSCcomment(line) && iscomment(line+2, "Page:")) {
	if (maxpages == 0) {
	    maxpages = 1;
	    doc->pages = (struct page *) PS_calloc(maxpages, sizeof(struct page));
            CHECK_MALLOCED(doc->pages);
	}
	label = gettext(line+length("%%Page:"), &next_char);
	if (sscanf(next_char, "%d", &thispage) != 1) thispage = 0;
	if (nextpage == 1) {
	    ignore = thispage != 1;
	}
	if (!ignore && thispage != nextpage) {
	    PS_free(label);
	    doc->numpages--;
	    goto continuepage;
	}
	nextpage++;
	if (doc->numpages == maxpages) {
	    maxpages++;
	    doc->pages = (struct page *)
			 PS_realloc(doc->pages, maxpages*sizeof (struct page));
            CHECK_MALLOCED(doc->pages);

	}
	memset(&(doc->pages[doc->numpages]), 0, sizeof(struct page));
	page_bb_set = NONE;
	doc->pages[doc->numpages].label = label;
	if (beginsection) {
	    doc->pages[doc->numpages].begin = beginsection;
	    beginsection = 0;
	} else {
	    doc->pages[doc->numpages].begin = position;
	    section_len = line_len;
	}
continuepage:
	while (readline(fd, &line, &position, &line_len) &&
	       !(DSCcomment(line) &&
	         (iscomment(line+2, "Page:") ||
	          iscomment(line+2, "Trailer") ||
	          (respect_eof && iscomment(line+2, "EOF"))))) {
	    section_len += line_len;
	    if (!DSCcomment(line)) {
		/* Do nothing */
	    } else if (doc->pages[doc->numpages].orientation == NONE &&
		iscomment(line+2, "PageOrientation:")) {
		sscanf(line+length("%%PageOrientation:"), "%s", text);
		if (strcmp(text, "Portrait") == 0) {
		    doc->pages[doc->numpages].orientation = PORTRAIT;
		} else if (strcmp(text, "Landscape") == 0) {
		    doc->pages[doc->numpages].orientation = LANDSCAPE;
		}
	    } else if (doc->pages[doc->numpages].media == NULL &&
		       iscomment(line+2, "PageMedia:")) {
		cp = gettext(line+length("%%PageMedia:"), NULL);
		for (dmp = doc->media, i=0; i<doc->nummedia; i++, dmp++) {
		    if (strcmp(cp, dmp->name) == 0) {
			doc->pages[doc->numpages].media = dmp;
			break;
		    }
		}
		PS_free(cp);
	    } else if (doc->pages[doc->numpages].media == NULL &&
		       iscomment(line+2, "PaperSize:")) {
		cp = gettext(line+length("%%PaperSize:"), NULL);
		for (dmp = doc->media, i=0; i<doc->nummedia; i++, dmp++) {
		    /* Note: Paper size comment uses down cased paper size
		     * name.  Case insensitive compares are only used for
		     * PaperSize comments.
		     */
		    if (strcasecmp(cp, dmp->name) == 0) {
			doc->pages[doc->numpages].media = dmp;
			break;
		    }
		}
		PS_free(cp);
	    } else if ((page_bb_set == NONE || page_bb_set == ATEND) &&
		       iscomment(line+2, "PageBoundingBox:")) {
		sscanf(line+length("%%PageBoundingBox:"), "%s", text);
		if (strcmp(text, "(atend)") == 0) {
		    page_bb_set = ATEND;
		} else {
		    if (sscanf(line+length("%%PageBoundingBox:"), "%d %d %d %d",
			    &(doc->pages[doc->numpages].boundingbox[LLX]),
			    &(doc->pages[doc->numpages].boundingbox[LLY]),
			    &(doc->pages[doc->numpages].boundingbox[URX]),
			    &(doc->pages[doc->numpages].boundingbox[URY])) == 4)
			if (page_bb_set == NONE) page_bb_set = 1;
		    else {
			float fllx, flly, furx, fury;
			if (sscanf(line+length("%%PageBoundingBox:"),
				   "%f %f %f %f",
				   &fllx, &flly, &furx, &fury) == 4) {
			    if (page_bb_set == NONE) page_bb_set = 1;
			    doc->pages[doc->numpages].boundingbox[LLX] = fllx;
			    doc->pages[doc->numpages].boundingbox[LLY] = flly;
			    doc->pages[doc->numpages].boundingbox[URX] = furx;
			    doc->pages[doc->numpages].boundingbox[URY] = fury;
			    if (fllx <
				    doc->pages[doc->numpages].boundingbox[LLX])
				doc->pages[doc->numpages].boundingbox[LLX]--;
			    if (flly <
				    doc->pages[doc->numpages].boundingbox[LLY])
				doc->pages[doc->numpages].boundingbox[LLY]--;
			    if (furx >
				    doc->pages[doc->numpages].boundingbox[URX])
				doc->pages[doc->numpages].boundingbox[URX]++;
			    if (fury >
				    doc->pages[doc->numpages].boundingbox[URY])
				doc->pages[doc->numpages].boundingbox[URY]++;
			}
		    }
		}
	    }
	}
	section_len += line_len;
	doc->pages[doc->numpages].end = position;
	doc->pages[doc->numpages].len = section_len - line_len;
	doc->numpages++;
    }

    /* Document Trailer */

    if (beginsection) {
	doc->begintrailer = beginsection;
	beginsection = 0;
    } else {
	doc->begintrailer = position;
	section_len = line_len;
    }

    preread = 1;
    while ((preread ||
	    readline(fd, &line, &position, &line_len)) &&
 	   !(respect_eof && DSCcomment(line) && iscomment(line+2, "EOF"))) {
	if (!preread) section_len += line_len;
	preread = 0;
	if (!DSCcomment(line)) {
	    /* Do nothing */
	} else if (iscomment(line+2, "Page:")) {
	    PS_free(gettext(line+length("%%Page:"), &next_char));
	    if (sscanf(next_char, "%d", &thispage) != 1) thispage = 0;
	    if (!ignore && thispage == nextpage) {
		if (doc->numpages > 0) {
		    doc->pages[doc->numpages-1].end = position;
		    doc->pages[doc->numpages-1].len += section_len - line_len;
		} else {
		    if (doc->endsetup) {
			doc->endsetup = position;
			doc->endsetup += section_len - line_len;
		    } else if (doc->endprolog) {
			doc->endprolog = position;
			doc->endprolog += section_len - line_len;
		    }
		}
		goto newpage;
	    }
	} else if (!respect_eof && iscomment(line+2, "Trailer")) {
	    /* What we thought was the start of the trailer was really */
	    /* the trailer of an EPS on the page. */
	    /* Set the end of the page to this trailer and keep scanning. */
	    if (doc->numpages > 0) {
		doc->pages[ doc->numpages-1 ].end = position;
		doc->pages[ doc->numpages-1 ].len += section_len - line_len;
	    }
	    doc->begintrailer = position;
	    section_len = line_len;
	} else if (bb_set == ATEND && iscomment(line+2, "BoundingBox:")) {
	    if (sscanf(line+length("%%BoundingBox:"), "%d %d %d %d",
		       &(doc->boundingbox[LLX]),
		       &(doc->boundingbox[LLY]),
		       &(doc->boundingbox[URX]),
		       &(doc->boundingbox[URY])) != 4) {
		float fllx, flly, furx, fury;
		if (sscanf(line+length("%%BoundingBox:"), "%f %f %f %f",
			   &fllx, &flly, &furx, &fury) == 4) {
		    doc->boundingbox[LLX] = fllx;
		    doc->boundingbox[LLY] = flly;
		    doc->boundingbox[URX] = furx;
		    doc->boundingbox[URY] = fury;
		    if (fllx < doc->boundingbox[LLX])
			doc->boundingbox[LLX]--;
		    if (flly < doc->boundingbox[LLY])
			doc->boundingbox[LLY]--;
		    if (furx > doc->boundingbox[URX])
			doc->boundingbox[URX]++;
		    if (fury > doc->boundingbox[URY])
			doc->boundingbox[URY]++;
		}
	    }
	} else if (orientation_set == ATEND &&
		   iscomment(line+2, "Orientation:")) {
	    sscanf(line+length("%%Orientation:"), "%s", text);
	    if (strcmp(text, "Portrait") == 0) {
		doc->orientation = PORTRAIT;
	    } else if (strcmp(text, "Landscape") == 0) {
		doc->orientation = LANDSCAPE;
	    }
	} else if (page_order_set == ATEND && iscomment(line+2, "PageOrder:")) {
	    sscanf(line+length("%%PageOrder:"), "%s", text);
	    if (strcmp(text, "Ascend") == 0) {
		doc->pageorder = ASCEND;
	    } else if (strcmp(text, "Descend") == 0) {
		doc->pageorder = DESCEND;
	    } else if (strcmp(text, "Special") == 0) {
		doc->pageorder = SPECIAL;
	    }
	} else if (pages_set == ATEND && iscomment(line+2, "Pages:")) {
	    if (sscanf(line+length("%%Pages:"), "%*u %d", &i) == 1) {
		if (page_order_set == NONE) {
		    if (i == -1) doc->pageorder = DESCEND;
		    else if (i == 0) doc->pageorder = SPECIAL;
		    else if (i == 1) doc->pageorder = ASCEND;
		}
	    }
	}
    }
    section_len += line_len;
    if (DSCcomment(line) && iscomment(line+2, "EOF")) {
	readline(fd, &line, &position, &line_len);
	section_len += line_len;
    }
    doc->endtrailer = position;
    doc->lentrailer = section_len - line_len;

#if 0
    section_len = line_len;
    preread = 1;
    while (preread ||
	   readline(fd, &line, &position, &line_len)) {
	if (!preread) section_len += line_len;
	preread = 0;
	if (DSCcomment(line) && iscomment(line+2, "Page:")) {
	    PS_free(gettext(line+length("%%Page:"), &next_char));
	    if (sscanf(next_char, "%d", &thispage) != 1) thispage = 0;
	    if (!ignore && thispage == nextpage) {
		if (doc->numpages > 0) {
		    doc->pages[doc->numpages-1].end = position;
		    doc->pages[doc->numpages-1].len += doc->lentrailer +
						       section_len - line_len;
		} else {
		    if (doc->endsetup) {
			doc->endsetup = position;
			doc->endsetup += doc->lentrailer +
					 section_len - line_len;
		    } else if (doc->endprolog) {
			doc->endprolog = position;
			doc->endprolog += doc->lentrailer +
					  section_len - line_len;
		    }
		}
		goto newpage;
	    }
	}
    }
#endif
    ENDMESSAGE(psscan)
    ps_io_exit(fd);
    return doc;
}

/*###########################################################*/
/*
 *	psfree -- free dynamic storage associated with document structure.
 */
/*###########################################################*/

void
psfree(doc)
    struct document *doc;
{
    int i;

    BEGINMESSAGE(psfree)
    if (doc) {
	for (i=0; i<doc->numpages; i++) {
	    if (doc->pages[i].label) PS_free(doc->pages[i].label);
	}
	for (i=0; i<doc->nummedia; i++) {
	    if (doc->media[i].name) PS_free(doc->media[i].name);
	}
	if (doc->title) PS_free(doc->title);
	if (doc->date) PS_free(doc->date);
	if (doc->pages) PS_free(doc->pages);
	if (doc->media) PS_free(doc->media);
	PS_free(doc);
    }
    ENDMESSAGE(psfree)
}

/*----------------------------------------------------------*/
/*
 * gettextline -- skip over white space and return the rest of the line.
 *               If the text begins with '(' return the text string
 *		 using gettext().
 */
/*----------------------------------------------------------*/

static char *
gettextline(line)
    char *line;
{
    char *cp;

    BEGINMESSAGE(gettextline)
    while (*line && (*line == ' ' || *line == '\t')) line++;
    if (*line == '(') {
        ENDMESSAGE(gettextline)
	return gettext(line, NULL);
    } else {
	if (strlen(line) == 0) {ENDMESSAGE(gettextline) return NULL;}
	cp = (char *) PS_malloc(strlen(line));
	CHECK_MALLOCED(cp);
	strncpy(cp, line, strlen(line)-1);
	cp[strlen(line)-1] = '\0';
        ENDMESSAGE(gettextline)
	return cp;
    }
}

/*----------------------------------------------------------*/
/*
 *	gettext -- return the next text string on the line.
 *		   return NULL if nothing is present.
 */
/*----------------------------------------------------------*/

static char *
gettext(line, next_char)
    char *line;
    char **next_char;
{
    char text[PSLINELENGTH];	/* Temporary storage for text */
    char *cp;
    int quoted=0;

    BEGINMESSAGE(gettext)
    while (*line && (*line == ' ' || *line == '\t')) line++;
    cp = text;
    if (*line == '(') {
	int level = 0;
	quoted=1;
	line++;
	while (*line && !(*line == ')' && level == 0 )) {
	    if (*line == '\\') {
		if (*(line+1) == 'n') {
		    *cp++ = '\n';
		    line += 2;
		} else if (*(line+1) == 'r') {
		    *cp++ = '\r';
		    line += 2;
		} else if (*(line+1) == 't') {
		    *cp++ = '\t';
		    line += 2;
		} else if (*(line+1) == 'b') {
		    *cp++ = '\b';
		    line += 2;
		} else if (*(line+1) == 'f') {
		    *cp++ = '\f';
		    line += 2;
		} else if (*(line+1) == '\\') {
		    *cp++ = '\\';
		    line += 2;
		} else if (*(line+1) == '(') {
		    *cp++ = '(';
		    line += 2;
		} else if (*(line+1) == ')') {
		    *cp++ = ')';
		    line += 2;
		} else if (*(line+1) >= '0' && *(line+1) <= '9') {
		    if (*(line+2) >= '0' && *(line+2) <= '9') {
			if (*(line+3) >= '0' && *(line+3) <= '9') {
			    *cp++ = ((*(line+1) - '0')*8 + *(line+2) - '0')*8 +
				    *(line+3) - '0';
			    line += 4;
			} else {
			    *cp++ = (*(line+1) - '0')*8 + *(line+2) - '0';
			    line += 3;
			}
		    } else {
			*cp++ = *(line+1) - '0';
			line += 2;
		    }
		} else {
		    line++;
		    *cp++ = *line++;
		}
	    } else if (*line == '(') {
		level++;
		*cp++ = *line++;
	    } else if (*line == ')') {
		level--;
		*cp++ = *line++;
	    } else {
		*cp++ = *line++;
	    }
	}
    } else {
	while (*line && !(*line == ' ' || *line == '\t' || *line == '\n'))
	    *cp++ = *line++;
    }
    *cp = '\0';
    if (next_char) *next_char = line;
    if (!quoted && strlen(text) == 0) {ENDMESSAGE(gettext) return NULL;}
    cp = (char *) PS_malloc(strlen(text)+1);
    CHECK_MALLOCED(cp);
    strcpy(cp, text);
    ENDMESSAGE(gettext)
    return cp;
}

/*----------------------------------------------------------*/
/* ps_io_init */
/*----------------------------------------------------------*/

#define FD_FILE             (fd->file)
#define FD_FILE_DESC        (fd->file_desc)
#define FD_FILEPOS	    (fd->filepos)
#define FD_LINE_BEGIN       (fd->line_begin)
#define FD_LINE_END	    (fd->line_end)
#define FD_LINE_LEN	    (fd->line_len)
#define FD_LINE_TERMCHAR    (fd->line_termchar)
#define FD_BUF		    (fd->buf)
#define FD_BUF_END	    (fd->buf_end)
#define FD_BUF_SIZE	    (fd->buf_size)
#define FD_STATUS	    (fd->status)

#define FD_STATUS_OKAY        0
#define FD_STATUS_BUFTOOLARGE 1
#define FD_STATUS_NOMORECHARS 2

#define LINE_CHUNK_SIZE     4096
#define MAX_PS_IO_FGETCHARS_BUF_SIZE 57344
#define BREAK_PS_IO_FGETCHARS_BUF_SIZE 49152

static FileData ps_io_init(file)
   FILE *file;
{
   FileData fd;
   Cardinal size = sizeof(FileDataStruct);

   BEGINMESSAGE(ps_io_init)

   fd = (FileData) PS_XtMalloc(size);
   memset((void*) fd ,0,(size_t)size);

   rewind(file);
   FD_FILE      = file;
   FD_FILE_DESC = fileno(file);
   FD_FILEPOS   = ftell(file);
   FD_BUF_SIZE  = (2*LINE_CHUNK_SIZE)+1;
   FD_BUF       = PS_XtMalloc(FD_BUF_SIZE);
   FD_BUF[0]    = '\0';
   ENDMESSAGE(ps_io_init)
   return(fd);
}

/*----------------------------------------------------------*/
/* ps_io_exit */
/*----------------------------------------------------------*/

static void
ps_io_exit(fd)
   FileData fd;
{
   BEGINMESSAGE(ps_io_exit)
   PS_XtFree(FD_BUF);
   PS_XtFree(fd);
   ENDMESSAGE(ps_io_exit)
}

/*----------------------------------------------------------*/
/* ps_io_fseek */
/*----------------------------------------------------------*/

static int
ps_io_fseek(fd,offset)
   FileData fd;
   int offset;
{
   int status;
   BEGINMESSAGE(ps_io_fseek)
   status=fseek(FD_FILE,(long)offset,SEEK_SET);
   FD_BUF_END = FD_LINE_BEGIN = FD_LINE_END = FD_LINE_LEN = 0;
   FD_FILEPOS = offset;
   FD_STATUS  = FD_STATUS_OKAY;
   ENDMESSAGE(ps_io_fseek)
   return(status);
}

/*----------------------------------------------------------*/
/* ps_io_ftell */
/*----------------------------------------------------------*/

static int
ps_io_ftell(fd)
   FileData fd;
{
   BEGINMESSAGE(ps_io_ftell)
   IMESSAGE(FD_FILEPOS)
   ENDMESSAGE(ps_io_ftell)
   return(FD_FILEPOS);
}

/*----------------------------------------------------------*/
/* ps_io_fgetchars */
/*----------------------------------------------------------*/

#ifdef USE_MEMMOVE_CODE
static void ps_memmove (d, s, l)
  char *d;
  const char *s;
  unsigned l;
{
  if (s < d) for (s += l, d += l; l; --l) *--d = *--s;
  else if (s != d) for (; l; --l)         *d++ = *s++;
}
#else
#   define ps_memmove memmove
#endif

static char * ps_io_fgetchars(fd,num)
   FileData fd;
   int num;
{
   char *eol=NULL,*tmp;
   size_t size_of_char = sizeof(char);

   BEGINMESSAGE(ps_io_fgetchars)

   if (FD_STATUS != FD_STATUS_OKAY) {
      INFMESSAGE(aborting since status not okay)
      ENDMESSAGE(ps_io_fgetchars)
      return(NULL);
   }

   FD_BUF[FD_LINE_END] = FD_LINE_TERMCHAR; /* restoring char previously exchanged against '\0' */
   FD_LINE_BEGIN       = FD_LINE_END;

#if 0
   {
      int fp = (int)(ftell(FD_FILE));
      if (num<0)  { INFMESSAGE(reading line) }
      else        { INFMESSAGE(reading specified num of chars) }
      IIMESSAGE(FD_BUF_SIZE,FD_BUF_END)
      IIMESSAGE(FD_LINE_BEGIN,FD_LINE_END)
      INFIMESSAGE(unparsed:,FD_BUF_END-FD_LINE_END)
      IMESSAGE(fp)
      IIMESSAGE(FD_FILEPOS,fp-(FD_BUF_END-FD_LINE_END))
   }
#endif /* 0 */

   do {
      if (num<0) { /* reading whole line */
         if (FD_BUF_END-FD_LINE_END) {
 	    /* strpbrk is faster but fails on lines with embedded NULLs 
              eol = strpbrk(FD_BUF+FD_LINE_END,"\n\r");
            */
	    tmp = FD_BUF + FD_BUF_END;
	    eol = FD_BUF + FD_LINE_END;
	    while (eol < tmp && *eol != '\n' && *eol != '\r') eol++;
	    if (eol >= tmp) eol = NULL;
            if (eol) {
               if (*eol=='\r' && *(eol+1)=='\n') eol += 2;
               else eol++;
               break;
            }
         }
      } else { /* reading specified num of chars */
	 if (FD_BUF_END >= FD_LINE_BEGIN+num) {
            eol = FD_BUF+FD_LINE_BEGIN+num;
            break;
         }
      }

      INFMESSAGE(no end of line yet)

      if (FD_BUF_END - FD_LINE_BEGIN > BREAK_PS_IO_FGETCHARS_BUF_SIZE) {
	INFMESSAGE(breaking line artificially)
	eol = FD_BUF + FD_BUF_END - 1;
	break;
      }

      while (FD_BUF_SIZE < FD_BUF_END+LINE_CHUNK_SIZE+1) {
         if (FD_BUF_SIZE > MAX_PS_IO_FGETCHARS_BUF_SIZE) {
	   /* we should never get here, since the line is broken
             artificially after BREAK_PS_IO_FGETCHARS_BUF_SIZE bytes. */
            INFMESSAGE(buffer became to large)
            ENDMESSAGE(ps_io_fgetchars)
	    fprintf(stderr, "gv: ps_io_fgetchars: Fatal Error: buffer became too large.\n");
	    exit(-1);
         }
         if (FD_LINE_BEGIN) {
            INFMESSAGE(moving line to begin of buffer)
            ps_memmove((void*)FD_BUF,(void*)(FD_BUF+FD_LINE_BEGIN),
                    ((size_t)(FD_BUF_END-FD_LINE_BEGIN+1))*size_of_char);
            FD_BUF_END    -= FD_LINE_BEGIN; 
            FD_LINE_BEGIN  = 0;
         } else {
            INFMESSAGE(enlarging buffer)
	    /*
              FD_BUF_SIZE    = FD_BUF_END+LINE_CHUNK_SIZE+1;
	    */
            FD_BUF_SIZE    = FD_BUF_SIZE+LINE_CHUNK_SIZE+1;
            IMESSAGE(FD_BUF_SIZE)
            FD_BUF         = PS_XtRealloc(FD_BUF,FD_BUF_SIZE);
         }
      }

      FD_LINE_END = FD_BUF_END;
#ifdef VMS
      /* different existing VMS file formats require that we use read here ###jp###,10/12/96 */ 
      if (num<0) FD_BUF_END += read(FD_FILE_DESC,FD_BUF+FD_BUF_END,LINE_CHUNK_SIZE);
      else       FD_BUF_END += fread(FD_BUF+FD_BUF_END,size_of_char,LINE_CHUNK_SIZE,FD_FILE);
#else
      /* read() seems to fail sometimes (? ? ?) so we always use fread ###jp###,07/31/96*/
      FD_BUF_END += fread(FD_BUF+FD_BUF_END,size_of_char,LINE_CHUNK_SIZE,FD_FILE);
#endif

      FD_BUF[FD_BUF_END] = '\0';
      if (FD_BUF_END-FD_LINE_END == 0) {
         INFMESSAGE(failed to read more chars)
         ENDMESSAGE(ps_io_fgetchars)
         FD_STATUS = FD_STATUS_NOMORECHARS;
         return(NULL);
      }
   }
   while (1);

   FD_LINE_END          = eol - FD_BUF;
   FD_LINE_LEN          = FD_LINE_END - FD_LINE_BEGIN;
   FD_LINE_TERMCHAR     = FD_BUF[FD_LINE_END];
   FD_BUF[FD_LINE_END]  = '\0';
#ifdef USE_FTELL_FOR_FILEPOS
   if (FD_LINE_END==FD_BUF_END) {
      INFMESSAGE(### using ftell to get FD_FILEPOS)
      /*
      For VMS we cannot assume that the record is FD_LINE_LEN bytes long
      on the disk. For stream_lf and stream_cr that is true, but not for
      other formats, since VAXC/DECC converts the formatting into a single \n.
      eg. variable format files have a 2-byte length and padding to an even
      number of characters. So, we use ftell for each record.
      This still will not work if we need to fseek to a \n or \r inside a
      variable record (ftell always returns the start of the record in this
      case).
      (Tim Adye, adye@v2.rl.ac.uk)
      */
      FD_FILEPOS         = ftell(FD_FILE);
   } else
#endif /* USE_FTELL_FOR_FILEPOS */
      FD_FILEPOS        += FD_LINE_LEN;

#if 0
   SMESSAGE(FD_BUF+FD_LINE_BEGIN)
   IIMESSAGE(FD_LINE_BEGIN,FD_LINE_END)
   IIMESSAGE(FD_BUF_END,FD_LINE_LEN)
#endif

   ENDMESSAGE(ps_io_fgetchars)
   return(FD_BUF+FD_LINE_BEGIN);
}

/*----------------------------------------------------------*/
/*
   readline()
   Read the next line in the postscript file.
   Automatically skip over data (as indicated by
   %%BeginBinary/%%EndBinary or %%BeginData/%%EndData
   comments.)
   Also, skip over included documents (as indicated by
   %%BeginDocument/%%EndDocument comments.)
*/
/*----------------------------------------------------------*/

static char * readline (fd, lineP, positionP, line_lenP)
   FileData fd;
   char **lineP;
   long *positionP;
   unsigned int *line_lenP;
{
   unsigned int nbytes=0;
   int skipped=0;
   char *line;

   BEGINMESSAGE(readline)

   if (positionP) *positionP = FD_FILEPOS;
   line = ps_io_fgetchars(fd,-1);
   if (!line) {
      INFMESSAGE(could not get line)
      *line_lenP = 0;
      *lineP     = empty_string;
      ENDMESSAGE(readline)
      return(NULL); 
   }

   *line_lenP = FD_LINE_LEN;

#define IS_COMMENT(comment)				\
           (DSCcomment(line) && iscomment(line+2,(comment)))
#define IS_BEGIN(comment)				\
           (iscomment(line+7,(comment)))
#define SKIP_WHILE(cond)				\
	   while (readline(fd, &line, NULL, &nbytes) && (cond)) *line_lenP += nbytes;\
           skipped=1;
#define SKIP_UNTIL_1(comment) {				\
           INFMESSAGE(skipping until comment)		\
           SKIP_WHILE((!IS_COMMENT(comment)))		\
           INFMESSAGE(completed skipping until comment)	\
        }
#define SKIP_UNTIL_2(comment1,comment2) {		\
           INFMESSAGE(skipping until comment1 or comment2)\
           SKIP_WHILE((!IS_COMMENT(comment1) && !IS_COMMENT(comment2)))\
           INFMESSAGE(completed skipping until comment1 or comment2)\
        }

#if 0
   if ((scanstyle&SCANSTYLE_MISSING_BEGINDOCUMENT) &&
       (line[0] == '%') &&
       (*line_lenP > 11) &&
       (iscomment(line,"%!PS-Adobe-") || iscomment(line + 1,"%!PS-Adobe-"))) {
     char *c=line+11;
     while (*c && !isspace(*c)) c++;
     if (isspace(*c)) while (*c && isspace(*c)) c++;
     /* don't skip EPSF files */
       printf("line in question: %s\n",line);
     if (strncmp(c,"EPSF",4)) {
       printf("skipping starts here: %s\n",line);
       SKIP_UNTIL_1("EOF")
       *line_lenP += nbytes;
       readline(fd, &line, NULL, &nbytes);
       printf("skipping ends here: %s\n",line);
     }
   }
   else
#endif
   if  (!IS_COMMENT("Begin"))     {} /* Do nothing */
   else if IS_BEGIN("Document:")  SKIP_UNTIL_1("EndDocument")
   else if IS_BEGIN("Feature:")   SKIP_UNTIL_1("EndFeature")
#ifdef USE_ACROREAD_WORKAROUND
   else if IS_BEGIN("File")       SKIP_UNTIL_2("EndFile","EOF")
#else
   else if IS_BEGIN("File")       SKIP_UNTIL_1("EndFile")
#endif
   else if IS_BEGIN("Font")       SKIP_UNTIL_1("EndFont")
   else if IS_BEGIN("ProcSet")    SKIP_UNTIL_1("EndProcSet")
   else if IS_BEGIN("Resource")   SKIP_UNTIL_1("EndResource")
   else if IS_BEGIN("Data:")      {
      int  num;
      char text[101];
      INFMESSAGE(encountered "BeginData:")
      if (FD_LINE_LEN > 100) FD_BUF[100] = '\0';
      text[0] = '\0';
      if (sscanf(line+length("%%BeginData:"), "%d %*s %s", &num, text) >= 1) {
         if (strcmp(text, "Lines") == 0) {
            INFIMESSAGE(number of lines to skip:,num)
            while (num) {
               line = ps_io_fgetchars(fd,-1);
               if (line) *line_lenP += FD_LINE_LEN;
               num--;
            }
         } else {
            int read_chunk_size = LINE_CHUNK_SIZE;
            INFIMESSAGE(number of chars to skip:,num)
            while (num>0) {
               if (num <= LINE_CHUNK_SIZE) read_chunk_size=num;
               line = ps_io_fgetchars(fd,read_chunk_size);
               if (line) *line_lenP += FD_LINE_LEN;
               num -= read_chunk_size;
            }
         }
      }
      SKIP_UNTIL_1("EndData")
   }
   else if IS_BEGIN("Binary:") {
      int  num;
      INFMESSAGE(encountered "BeginBinary:")
      if (sscanf(line+length("%%BeginBinary:"), "%d", &num) == 1) {
         int read_chunk_size = LINE_CHUNK_SIZE;
         INFIMESSAGE(number of chars to skip:,num)
         while (num>0) {
            if (num <= LINE_CHUNK_SIZE) read_chunk_size=num;
            line = ps_io_fgetchars(fd,read_chunk_size);
            if (line) *line_lenP += FD_LINE_LEN;
            num -= read_chunk_size;
         }
         SKIP_UNTIL_1("EndBinary")
      }
   }

   if (skipped) {
      INFMESSAGE(skipped lines)
      *line_lenP += nbytes;
      *lineP = skipped_line;      
   } else {
      *lineP = FD_BUF+FD_LINE_BEGIN;
   }

   ENDMESSAGE(readline)
   return(FD_BUF+FD_LINE_BEGIN);
}

/*###########################################################*/
/*
 *	pscopyuntil -- copy lines of Postscript from a section of one file
 *		       to another file until a particular comment is reached.
 *                     Automatically switch to binary copying whenever
 *                     %%BeginBinary/%%EndBinary or %%BeginData/%%EndData
 *		       comments are encountered.
 */
/*###########################################################*/

char *
pscopyuntil(fd, to, begin, end, comment)
   FileData fd;
   FILE *to;
   long begin;			/* set negative to avoid initial seek */
   long end;
   char *comment;
{
   char *line;
   int comment_length;

   BEGINMESSAGE(pscopyuntil)
   if (comment) {
      INFSMESSAGE(will copy until,comment)
      comment_length = strlen(comment);
   }
   else {
      INFMESSAGE(will copy until specified file position)
      comment_length = 0;
   }
   if (begin >= 0) ps_io_fseek(fd, begin);
   while (ps_io_ftell(fd) < end) {
      line = ps_io_fgetchars(fd,-1);
      if (!line) break;
      if (comment && strncmp(line, comment, comment_length) == 0) {
         char *cp = (char *) PS_malloc(strlen(line)+1);
         INFSMESSAGE(encountered specified,comment)
         CHECK_MALLOCED(cp);
         strcpy(cp, line);
         ENDMESSAGE(pscopyuntil)
         return cp;
      }
      fputs(line, to);
      if  (!IS_COMMENT("Begin"))     {} /* Do nothing */
      else if IS_BEGIN("Data:")      {
         int  num;
         char text[101];
         INFMESSAGE(encountered "BeginData:")
         if (FD_LINE_LEN > 100) FD_BUF[100] = '\0';
         text[0] = '\0';
         if (sscanf(line+length("%%BeginData:"), "%d %*s %s", &num, text) >= 1) {
            if (strcmp(text, "Lines") == 0) {
               INFIMESSAGE(number of lines:,num)
               while (num) {
                  line = ps_io_fgetchars(fd,-1);
                  if (line) fputs(line,to);
                  num--;
               }
            } else {
               int read_chunk_size = LINE_CHUNK_SIZE;
               INFIMESSAGE(number of chars:,num)
               while (num>0) {
                  if (num <= LINE_CHUNK_SIZE) read_chunk_size=num;
                  line = ps_io_fgetchars(fd,read_chunk_size);
                  if (line) fwrite(line,sizeof(char),FD_LINE_LEN, to);
                  num -= read_chunk_size;
               }
            }
         }
      }
      else if IS_BEGIN("Binary:") {
         int  num;
         INFMESSAGE(encountered "BeginBinary:")
         if (sscanf(line+length("%%BeginBinary:"), "%d", &num) == 1) {
            int read_chunk_size = LINE_CHUNK_SIZE;
            INFIMESSAGE(number of chars:,num)
            while (num>0) {
               if (num <= LINE_CHUNK_SIZE) read_chunk_size=num;
               line = ps_io_fgetchars(fd,read_chunk_size);
               if (line) fwrite(line, sizeof (char),FD_LINE_LEN, to);
               num -= read_chunk_size;
            }
         }
      }
   }
   ENDMESSAGE(pscopyuntil)
   return NULL;
}

/*----------------------------------------------------------*/
/* blank */
/* Check whether the line contains nothing but white space. */
/*----------------------------------------------------------*/

static int blank(line)
   char *line;
{
   char *cp = line;

   BEGINMESSAGE(blank)
   while (*cp == ' ' || *cp == '\t') cp++;
   ENDMESSAGE(blank)
   return *cp == '\n' || *cp== '\r' || (*cp == '%' && (line[0] != '%' || line[1] != '%'));
}

/*##########################################################*/
/* pscopydoc */
/* Copy the headers, marked pages, and trailer to fp */
/*##########################################################*/

void
pscopydoc(dest_file,src_filename,d,pagelist)
    FILE *dest_file;
    char *src_filename;
    Document d;
    char *pagelist;
{
    FILE *src_file;
    char text[PSLINELENGTH];
    char *comment;
    Boolean pages_written = False;
    Boolean pages_atend = False;
    int pages;
    int page = 1;
    int i, j;
    int here;
    FileData fd;

    BEGINMESSAGE(pscopydoc)

    INFSMESSAGE(copying from file, src_filename)
    src_file = fopen(src_filename, "r");
    fd = ps_io_init(src_file);

    i=0;
    pages=0;
    while (pagelist[i]) { if (pagelist[i]=='*') pages++; i++; }
    INFIMESSAGE(number of pages to be copied,pages)

    here = d->beginheader;
    while ((comment=pscopyuntil(fd,dest_file,here,d->endheader,"%%Pages:"))) {
       SMESSAGE(comment)
       here = ps_io_ftell(fd);
       if (pages_written || pages_atend) {
          PS_free(comment);
          continue;
       }
       sscanf(comment+length("%%Pages:"), "%s", text);
       if (strcmp(text, "(atend)") == 0) {
          fputs(comment, dest_file);
          pages_atend = True;
       } else {
          switch (sscanf(comment+length("%%Pages:"), "%*d %d", &i)) {
             case 1:
                fprintf(dest_file, "%%%%Pages: %d %d\n", pages, i);
                break;
             default:
                fprintf(dest_file, "%%%%Pages: %d\n", pages);
                break;
          }
          pages_written = True;
       }
       PS_free(comment);
   }
   pscopyuntil(fd, dest_file, d->beginpreview, d->endpreview,NULL);
   pscopyuntil(fd, dest_file, d->begindefaults, d->enddefaults,NULL);
   pscopyuntil(fd, dest_file, d->beginprolog, d->endprolog,NULL);
   pscopyuntil(fd, dest_file, d->beginsetup, d->endsetup,NULL);

   for (i = 0; i < d->numpages; i++) {
      if (d->pageorder == DESCEND) j = (d->numpages - 1) - i;
      else                         j = i;
      if (pagelist[j]=='*') {
          comment = pscopyuntil(fd,dest_file,d->pages[i].begin,d->pages[i].end, "%%Page:");
          fprintf(dest_file, "%%%%Page: %s %d\n",d->pages[i].label, page++);
          PS_free(comment);
          pscopyuntil(fd, dest_file, -1, d->pages[i].end,NULL);
      }
   }

   here = d->begintrailer;
   while ((comment = pscopyuntil(fd, dest_file, here, d->endtrailer, "%%Pages:"))) {
      here = ps_io_ftell(fd);
      if (pages_written) {
         PS_free(comment);
         continue;
      }
      switch (sscanf(comment+length("%%Pages:"), "%*d %d", &i)) {
         case 1:
            fprintf(dest_file, "%%%%Pages: %d %d\n", pages, i);
            break;
         default:
            fprintf(dest_file, "%%%%Pages: %d\n", pages);
            break;
      }
      pages_written = True;
      PS_free(comment);
   }
   fclose(src_file);
   ps_io_exit(fd);

   ENDMESSAGE(pscopydoc)
}
#undef length


