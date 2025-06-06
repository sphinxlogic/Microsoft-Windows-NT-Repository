/* $XFree86: mit/extensions/server/PEX/ospex/osPexFont.c,v 1.2 1993/04/09 11:26:04 dawes Exp $ */
/* $XConsortium: osPexFont.c,v 5.6 91/06/04 16:35:40 hersh Exp $ */

/***********************************************************
Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include <X11/Xos.h>
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
extern char *getenv();
#endif

#include <stdio.h>
#include "mipex.h"
#include "miFont.h"
#include "PEXErr.h"
#define XK_LATIN1
#include "keysymdef.h"

#ifndef PEX_DEFAULT_FONTPATH
#define PEX_DEFAULT_FONTPATH "/usr/lib/X11/fonts/PEX"
#endif

#ifndef X_NOT_POSIX
#include <dirent.h>
#else
#ifdef SYSV
#include <dirent.h>
#else
#ifdef USG
#include <dirent.h>
#else
#include <sys/dir.h>
#ifndef dirent
#define dirent direct
#endif
#endif
#endif
#endif


/* A convenient shorthand. */
typedef struct dirent	 ENTRY;

extern void CopyISOLatin1Lowered();
extern int get_lowered_truncated_entry();

void ClosePEXFontFile();
void SetPEXFontFilePtr();

extern  diFontHandle defaultPEXFont;

/*
 * Unless an environment variable named PEX_FONTPATH is set before the
 * server is started up, PEX will look in the path defined in the
 * PEX_DEFAULT_FONTPATH compiler constant defined in miFont.h for PEX fonts.
 * If environment variable PEX_FONTPATH is defined, then this will
 * be used as the path to the fonts .
 */
 
static char *
pex_get_font_directory_path()
{
    static int	 already_determined = 0;
    static char *font_dir_path = NULL;
    
    if (!already_determined) {
	if (getenv("PEX_FONTPATH")) {
	    font_dir_path = 
	       (char *)Xalloc((unsigned long)(1+strlen(getenv("PEX_FONTPATH"))));
	    strcpy(font_dir_path, getenv("PEX_FONTPATH"));
	} else {
	    char *xwinhome;

	    if ((xwinhome = getenv("XWINHOME")) != NULL) {
		font_dir_path =
		    (char *)Xalloc((unsigned long)(strlen(xwinhome) + 1 +
				   strlen("/lib/X11/fonts/PEX")));
		sprintf(font_dir_path, "%s/lib/X11/fonts/PEX", xwinhome);
	    } else {
		font_dir_path =
		    (char *)Xalloc((unsigned long)(1+
				   strlen(PEX_DEFAULT_FONTPATH)));
		strcpy(font_dir_path, PEX_DEFAULT_FONTPATH);
	    }
	}
	already_determined = 1;
    }
    
    return (font_dir_path);
}


/*
 * The next two functions (pex_setup_wild_match() and pex_is_matching()) are 
 * stolen (and slightly modified) from MIT X11R4 fonts/mkfontdir/fontdir.c.
 * pex_setup_wild_match() sets up some state about the pattern to match, which
 * pex_is_matching() then uses.
 */
 
 
/* results of this function are used by pex_is_matching() */

static void
pex_setup_wild_match(pat, phead, ptail, plen)
char	*pat;			/* in */
int	*phead, *ptail, *plen;	/* out */
{
    register int head, tail;
    register char c, *firstWild;

    *plen = tail = strlen(pat);
    for (   firstWild = pat; 
	    ((c = *firstWild) && !((c == XK_asterisk) || (c == XK_question)));
            firstWild++)
	;
	    
    head = firstWild - pat;

    while ((c = pat[head]) && (c != XK_asterisk))
        head++;
    if (head < tail)
    {
        while (pat[tail-1] != XK_asterisk)
            tail--;
    }
    *phead = head;
    *ptail = tail;
}

/* returns value greater than 0 if successful.  head, tail, and plen
 * come from a previous call to pex_setup_wild_match 
 */
static int 
pex_is_matching(string, pat, head, tail, plen)
register char       *string;		/* in */
register char       *pat;		/* in */
int                 head, tail, plen;	/* in */
{
    register int i, l;
    int j, m, res;
    register char cp, cs;
 
    res = -1;
    for (i = 0; i < head; i++)
    {
        cp = pat[i];
        if (cp == XK_question)
        {
            if (!string[i])
                return res;
            res = 0;
        }
        else if (cp != string[i])
            return res;
    }
    if (head == plen)
        return (string[head] ? res : 1);
    l = head;
    while (++i < tail)
    {
        /* we just skipped an asterisk */
        j = i;
        m = l;
        while ((cp = pat[i]) != XK_asterisk)
        {
            if (!(cs = string[l]))
                return 0;
            if ((cp != cs) && (cp != XK_question))
            {
                m++;
                cp = pat[j];
                if (cp == XK_asterisk)
                {
                    if (!string[m])
                        return 0;
                }
                else
                {
                    while ((cs = string[m]) != cp)
                    {
                        if (!cs)
                            return 0;
                        m++;
                    }
                }
                l = m;
                i = j;
            }
            l++;
            i++;
        }
    }
    m = strlen(&string[l]);
    j = plen - tail;
    if (m < j)
        return 0;
    l = (l + m) - j;
    while (cp = pat[i])
    {
        if ((cp != string[l]) && (cp != XK_question))
            return 0;
        l++;
        i++;
    }
    return 1;
}

/*
 * Caller is responsible for freeing contents of buffer and buffer when
 * done with it.
 */
#define ABSOLUTE_MAX_NAMES 200

int
pex_get_matching_names(patLen, pPattern, maxNames, numNames, names)
ddUSHORT   patLen;		/* in */
ddUCHAR	  *pPattern;		/* in */
ddUSHORT   maxNames;		/* in */
ddULONG   *numNames;		/* out - number of names found */
char    ***names;		/* out - pointer to list of strings */
{
    DIR		    *fontdir;
    ENTRY           *dir_entry;
    char	     pattern[100];
    char	     entry[100];
    int		     i, head, tail, len, total = 0;
    
    CopyISOLatin1Lowered(pattern, pPattern, patLen);
    
    if (!(*names = (char **)Xalloc((unsigned long)(ABSOLUTE_MAX_NAMES * sizeof(char *)))))
	return 0;
    
    if (!(fontdir = opendir(pex_get_font_directory_path())))
	return 0;

    pex_setup_wild_match(pattern, &head, &tail, &len);
    
    do {
	dir_entry = readdir(fontdir);
	if (dir_entry) {
	
	    if (!get_lowered_truncated_entry(dir_entry->d_name, entry))
		continue;

	    if (pex_is_matching(entry, pattern, head, tail, len) > 0) {
	    
		if (!( (*names)[total] = (char *)Xalloc((unsigned long)(1 + strlen(entry))))) {
		    for (i = 0; i < total; i++)
			Xfree((*names)[i]);
		    Xfree(*names);
		    return 0;
		}
		
		strcpy((*names)[total], entry);
		total++;
	    }
	}
    } while (dir_entry && total < maxNames);

    closedir(fontdir);
    
    *numNames = total;
    
    return 1;
}

/*
 * get_stroke(stroke, fp) extracts the definition of characters
 * from the font file.  It return -1 if anything goes wrong, 0 if
 * everything is OK.  
 */

static int
get_stroke(stroke, fp)
    Ch_stroke_data     *stroke;
    FILE               *fp;
{
    listofddPoint	   *spath;
    register int	    i;
    unsigned long	    closed;	/* placeholder, really unused */
    register ddULONG	    npath;
    register miListHeader  *hdr = &(stroke->strokes);

    stroke->n_vertices = 0;
    npath = hdr->maxLists = hdr->numLists;
    hdr->type = DD_2D_POINT;
    hdr->ddList = spath = (listofddPoint *)
	Xalloc((unsigned long)(sizeof(listofddPoint) * npath));
	
    if (spath == NULL)
	return -1;

    for (i = 0; i < npath; i++, spath++)
	spath->pts.p2Dpt = NULL;

    for (i = 0, spath = hdr->ddList; i < npath; i++, spath++) {
    
	/* for each subpath of the character definition ... */
	
	if (fread((char *) &spath->numPoints,
		  sizeof(spath->numPoints), 1, fp) != 1 ||
	    fread((char *) &closed, sizeof(closed), 1, fp) != 1)
	    return -1;

	if (spath->numPoints <= 0)
	    continue;

	spath->maxData = sizeof(ddCoord2D) * spath->numPoints;
	
	if (!(spath->pts.p2Dpt = (ddCoord2D *) Xalloc((unsigned long)(spath->maxData))))
	    return -1;
	    
	if (fread((char *)spath->pts.p2Dpt, sizeof(ddCoord2D), 
		  spath->numPoints, fp) != spath->numPoints)
	    return -1;
	    
	stroke->n_vertices += spath->numPoints;
    }
    return 0;
}



/*
    read in the pex font
 */
ErrorCode
LoadPEXFontFile(length, fontname, pFont)
    unsigned	    length;
    char *	    fontname;
    diFontHandle    pFont;
{
    char                fname[100];
    FILE               *fp;
    Font_file_header    header;
    Property           *properties = 0;
    Dispatch           *table = 0;
    miFontHeader       *font = (miFontHeader *)(pFont->deviceData);
    int                 found_first, found_it = 0, err = Success, numChars, np;
    char		name_to_match[100];
    char		lowered_entry[100];
    DIR		       *fontdir;
    ENTRY              *dir_entry;
    register int        i;
    register Ch_stroke_data **ch_font, *ch_stroke = 0;
    register Dispatch  *tblptr = 0;
    register Property  *propptr = 0;
    register pexFontProp *fpptr = 0;

    CopyISOLatin1Lowered(name_to_match, fontname, length);

    /* open up the font directory and look for matching file names */
    if (!(fontdir = opendir(pex_get_font_directory_path())))
	return (PEXERR(PEXFontError));

    do {
	dir_entry = readdir(fontdir);
	if (dir_entry) {
	    /* strip off .phont and make all lower case */
	    if (!get_lowered_truncated_entry(dir_entry->d_name, lowered_entry))
		continue;
		
	    /* does this match what got passed in? */
	    if (strcmp(lowered_entry, name_to_match) == 0)
		found_it = 1;
	}
    } while (dir_entry && !found_it);
    
    if (!found_it)
	return (PEXERR(PEXFontError));
    
    (void) strcpy(fname, pex_get_font_directory_path());
    (void) strcat(fname, "/");
    (void) strcat(fname, dir_entry->d_name);
    
    closedir(fontdir);

    if ((fp = fopen(fname, "r")) == NULL)
	return (PEXERR(PEXFontError));

    /*
     * read in the file header.  The file header has fields containing the
     * num of characters in the font, the extreme values, and number of font
     * properties defined, if any.
     */

    tblptr = 0;
    if (fread((char *) &header, sizeof(header), 1, fp) != 1) {
	(void) ClosePEXFontFile(fp);
	return (PEXERR(PEXFontError)); }
    
    /* Initialize font structure */
    (void) strcpy(font->name, header.name);
    font->font_type = FONT_POLYLINES;
    font->top = header.top;
    font->bottom = header.bottom;
    font->num_ch = header.num_ch;
    font->font_info.numProps = (CARD32)header.num_props;
    font->max_width = header.max_width;

    /* read in the font properties, if any, into font data area */
    if (header.num_props > 0) {

	(void) SetPEXFontFilePtr(fp, START_PROPS);   /* Get to props position */
	properties = (Property *) Xalloc(header.num_props * sizeof(Property));
	if (properties == NULL) {
	    (void) ClosePEXFontFile(fp);
	    return (BadAlloc); }

	if (fread((char *) properties, sizeof(Property), 
		  header.num_props, fp) != header.num_props) {
	    Xfree((char *) properties);
	    (void) ClosePEXFontFile(fp);
	    return (PEXERR(PEXFontError)); }
    
	/* Create space for font properties in the font data area */

	font->properties =
	    (pexFontProp *) Xalloc( (unsigned long)(header.num_props
				    * sizeof(pexFontProp)));
	if (font->properties == NULL) {
	    Xfree((char *) properties);
	    (void) ClosePEXFontFile(fp);
	    return (BadAlloc); }

	np = header.num_props;
	for (	i=0, propptr = properties, fpptr = font->properties;
		i < np;
		i++, propptr++, fpptr++) {

	    if (propptr->propname == NULL) {
		(header.num_props)--;
		continue; }

	    fpptr->name = MakeAtom( (char *)propptr->propname,
				    strlen(propptr->propname), 1);

	    if (propptr->propvalue != NULL)
		fpptr->value = MakeAtom((char *)propptr->propvalue,
					strlen(propptr->propvalue), 1);
	    else fpptr->value = 0;
	}

	/* free up local storage allocated for properties */
	 Xfree((char *) properties);
    }

    /* position file pointer to dispatch data */
    (void) SetPEXFontFilePtr(fp, (long) START_DISPATCH(header.num_props));

    /*
     * read in the distable font, use the offset to see if the
     * character is defined or not.  The strokes are defined in Phigs style.
     * The "center" of the character is not the physical center. It is the
     * center defined by the font designer.  The actual center is half the
     * "right" value.
     */

    table = (Dispatch *)Xalloc((unsigned long)(sizeof(Dispatch) *font->num_ch));
    
    if (table == NULL) {
	(void) ClosePEXFontFile(fp);
	return (BadAlloc); }
    
    if (fread((char *) table, sizeof(Dispatch), font->num_ch, fp)
	    != font->num_ch) {
	Xfree((char *) table);
	(void) ClosePEXFontFile(fp);
	return (PEXERR(PEXFontError)); }
    
    font->ch_data =
	(Ch_stroke_data **) Xalloc((unsigned long)(sizeof(Ch_stroke_data *) * 
						    font->num_ch));
    if (font->ch_data == NULL) {
	Xfree((char *) table);
	(void) ClosePEXFontFile(fp);
	return (BadAlloc); }
    
    /* The next loop initializes all ch_data pointers to null; essential
	for non-crashing during font clean-up in case of failed font file
	read.  Also count the number of non-blank chars.
     */
    for (   i = 0, ch_font = font->ch_data, tblptr = table, numChars = 0;
	    i < font->num_ch;
	    i++, ch_font++, tblptr++) {
	*ch_font = NULL;
	if (tblptr->offset != 0) numChars++; }

    ch_stroke = (Ch_stroke_data *)Xalloc((unsigned long)(numChars *
						    sizeof(Ch_stroke_data)));
    if (!ch_stroke) {
	err = BadAlloc;
	goto disaster;
    }

    /* read in the char data  (the font file format should be changed
	so that the allocation can be done outside this loop--this
	method is inefficient)
     */
    for (   i = 0, ch_font = font->ch_data, tblptr = table, found_first = 0;
	    i < font->num_ch;
	    i++, ch_font++, tblptr++) {
	if (tblptr->offset != 0) {
	    (*ch_font) = ch_stroke++;
	    (*ch_font)->strokes.ddList = NULL;
	    (*ch_font)->center = table[i].center;
	    (*ch_font)->right = table[i].right;
	    
	    (void) SetPEXFontFilePtr(fp, tblptr->offset);

	    /* read in the type, number of subpaths, and n_vertices fields */
	    if (    (fread(&((*ch_font)->type),
			   sizeof(Font_path_type), 1, fp) != 1) 
		||  (fread(&((*ch_font)->strokes.numLists),
			   sizeof(ddULONG),1,fp) != 1)
		||  (fread(&((*ch_font)->n_vertices),
			   sizeof(ddULONG), 1, fp) != 1) )
		{		  
		    err = PEXERR(PEXFontError);
		    goto disaster;
		}
	    
	    (*ch_font)->strokes.maxLists = (*ch_font)->strokes.numLists;
	    if ((*ch_font)->strokes.numLists > 0) {
	    
		if (get_stroke(*ch_font, fp)) {
		    err = BadAlloc;
		    goto disaster; }
	    
		if (!found_first) {
		    font->font_info.firstGlyph = i;
		    found_first = 1; }

		font->font_info.lastGlyph = i; }
	    }
    }

    Xfree((char *)table);

    (void) ClosePEXFontFile(fp);
    
    return (Success);

disaster:
    (void) ClosePEXFontFile(fp);
    if (table) Xfree(table);
    if (pFont == defaultPEXFont) defaultPEXFont = 0;	/* force free */
    FreePEXFont((diFontHandle) pFont, pFont->id);
    return (err);
    
}
void
ClosePEXFontFile(fp)
    FILE *fp;
{
    fclose (fp);
}

void
SetPEXFontFilePtr(fp,where)
    FILE *fp;
    long where;
{
    (void) fseek(fp, where, SEEK_SET);	    /* set pointer at "where" bytes
						from the beginning of the file */
}
