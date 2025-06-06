%{
/* $XFree86: mit/fonts/PEX/to_wfont.y,v 1.3 1993/05/18 15:36:27 dawes Exp $ */
/* $XConsortium: to_wfont.y,v 5.3 91/04/04 16:00:26 gildea Exp $ */

/*****************************************************************
Copyright (c) 1989,1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

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


#define YYMAXDEPTH 10000

#include <X11/Xos.h>
#include <stdio.h>
#ifndef L_SET
#define L_SET SEEK_SET
#endif
#include "wfont.h"

#ifdef X_NOT_STDC_ENV
FILE *fopen();
#endif

typedef struct {

        float           std_left,      /* NCGA standard left spacing */
                        std_wide,      /* character width            */  
                        std_rght;      /* Right spacing              */  
}               Standard;


char            fname[80];
Dispatch        *table;    /* dispatch table */
Standard	*sp_table; /* NCGA font spacings */
Path            *strokes;  /* strokes of each character */
Property        *property; /* property list */

struct {
	int path, point, props;
} count, expect;

Path_subpath   *current_path;

Font_header     head;		/* font header */
int             tableindex;	/* which character */
int             yyerrno;	/* error number */

%}

%union {
	int	nil;	/* void is reserved */
	int	ival;
	float	dval;
	char	*cval;
}

%start font

%token <dval> REAL
%token <ival> INTEGER
%token <cval> STRING

%token <nil> BOTTOM
%token <nil> CENTER
%token <nil> CLOSE
%token <nil> FONTNAME
%token <nil> PROPERTIES
%token <nil> NUM_CH
%token <nil> INDEX
%token <nil> L_SPACE
%token <nil> MAGIC
%token <nil> OPEN
%token <nil> RIGHT
%token <nil> R_SPACE
%token <nil> STROKE
%token <nil> TOP
%token <nil> VERTICES
%token <nil> BEARING
%token <nil> WIDTH

%type <cval> fontname
%type <ival> properties
%type <dval> top bottom center right
%type <ival> nstroke nvertice n_pts index num_ch
%type <ival> closeflag
%type <ival> counter
%type <dval> sp_left wide sp_right

%%

font : fontheader num_ch fontprops fontbody spacing { fini(); }|
	fontheader fontbody  { fini(); };

fontheader : fontname top bottom 
	{ wf_header($1, $2, $3); };

fontname : FONTNAME STRING
	{ $$ = $2; };

top : TOP REAL { $$ = $2;};

bottom : BOTTOM REAL { $$ = $2;};

num_ch: NUM_CH INTEGER { set_num_ch($2);};

fontprops : /* empty */ | properties;

properties : PROPERTIES INTEGER { init_properties ($2); } property_list
        { check_num_props (); }

property_list : /* empty */ | single_property property_list

single_property : STRING STRING { add_property($1, $2); };

fontbody : 	/* empty */ 
	| glyph fontbody;

glyph : glyph_header strokes
	{ check_nstroke(); };

glyph_header : index { tableindex = $1; } sym_headinfo;

sym_headinfo :  nstroke center right nvertice
	{ glyph_header($1, $2, $3, $4); };

index : INDEX INTEGER { check_num_ch(); $$ = $2; };

nstroke : STROKE INTEGER { $$ = $2; expect.path = $2; };

nvertice: {$$ = 0;} | VERTICES INTEGER  { $$ = $2; }  ;

center : CENTER REAL{ $$ = $2; };

right : RIGHT REAL{ $$ = $2; };

strokes :	/* empty */ | path strokes;

path : closeflag n_pts { init_path($1, $2); } points
	{ check_npts(); }

points : 	/* empty */ | coord points;

closeflag : CLOSE { $$ = $1 == CLOSE; } | OPEN { $$ = $1 == CLOSE; } ;

n_pts : INTEGER { $$ = $1; };

coord : REAL REAL { add_point($1, $2); };

spacing : 	/* empty */ 
	| item spacing;

item : counter {tableindex = $1;} sp_left wide sp_right
	{ std_space($3, $4, $5); };

counter  : BEARING INTEGER {$$ = $2;};

sp_left: L_SPACE REAL {$$ = $2;};

wide :  WIDTH REAL {$$ = $2;};

sp_right: R_SPACE REAL {$$ = $2;};

%%

#define BYE(err)	yyerrno = (err), yyerror()

#define ERR_BASE	1000
#define OPEN_ERROR 	1001
#define WRITE_ERROR 	1002
#define WRONG_NAME 	1003
#define NO_MEMORY 	1004
#define EXCEED_PATH 	1005
#define EXCEED_POINT 	1006
#define PATH_MISMATCH	1007
#define POINT_MISMATCH	1008
#define PROP_MISMATCH   1009
#define EXCEED_PROPS 	1010

char	*prog;

main(argc, argv)
	int             argc;
	char           *argv[];

{
	/* Usage : to_wfont [-o outfile] [infile] */
	char           *s;

	fname[0] = 0;
	tableindex = 0;
	head.num_ch = -1;

	prog = *argv;
	while (--argc > 0 && *++argv != NULL) {
		s = *argv;
		if (*s++ == '-')
			switch (*s) {
			case 'o':
				if (*++argv != NULL)
				{
					--argc;
					(void) strcpy(fname, *argv);
				}
				break;
			default:      /* ignore other options */
				;
			}
		else {
			FILE           *fp;

			/* standard input redirection */
			fp = fopen(*argv, "r");
			if (fp != NULL) {
				if (close(fileno(stdin)) < 0)
				{
					perror(prog);
					return;
				}
				if (dup(fileno(fp)) < 0)
				{
					perror(prog);
					return;
				}
				(void) fclose(fp);
			}
		}
	}
	return (yyparse());
}

/* set number of characters */
set_num_ch(num_ch)
int num_ch;
{
	yyerrno = 0;
	head.num_ch = num_ch;
	if (num_ch > 0) {
	  table    = (Dispatch *) malloc(num_ch * sizeof(Dispatch));
	  sp_table = (Standard *) malloc(num_ch * sizeof(Standard));
	  strokes  = (Path *)     malloc(num_ch * sizeof(Path));
	}

	for (tableindex = 0; tableindex < num_ch; tableindex++) {
		table[tableindex].center = 0.0;
		table[tableindex].right = 0.0;
		table[tableindex].offset = 0;

		sp_table[tableindex].std_left = 0.0;
		sp_table[tableindex].std_wide = 0.0;
		sp_table[tableindex].std_rght = 0.0;

		strokes[tableindex].n_subpaths = 0;
		strokes[tableindex].n_vertices = 0;
		strokes[tableindex].subpaths = NULL;
	}
}

/* initialize the property info in the header */
init_properties(num_props)
	int             num_props;
{
	if (num_props > 0) {
	  head.properties = (Property *) 
	                      malloc (num_props * sizeof (Property));
	  head.num_props = expect.props = num_props;
	}
	else {
	  head.properties = NULL;
	  head.num_props = expect.props = 0;
	}
	count.props = -1;
	property = head.properties;  /* initialize the list pointer */
}

check_num_props()
{
        count.props++;
        if (count.props != expect.props)
	  BYE (PROP_MISMATCH);
}

/* add individual property info into the buffer */
add_property(name, value)
	char            *name,
			*value;
{
        /* check if the property exceeds allocated space */

        if (++count.props >= head.num_props)
	     BYE(EXCEED_PROPS);

	/* copy the strings into the buffer */

	(void) strcpy (property->propname, name);
	(void) strcpy (property->propvalue, value);

	/* increment the property pointer */

	property++;
}

check_num_ch()
{

  if (head.num_ch == -1)
	set_num_ch(128);

}

yyerror()
{
#if defined(linux) || defined(__386BSD__)
	int      yylineno;
#else
	extern int      yylineno;
#endif
#	define ERR_SIZE (sizeof(err_string) / sizeof(char *))
	static char    *err_string[] = {
		"Cannot open file",
		"Write fails",
		"Illegal font name",
		"Memory allocation failure",
		"Specified number of strokes exceeded",
		"Specified number of points exceeded",
		"Number of strokes do not match",
		"Number of points do not match",
		"Number of properties do not match",
		"Specified number of properties exceeded",
	0};
	char           *str;

	yyerrno -= ERR_BASE;
	if (yyerrno > 0 && yyerrno < ERR_SIZE)
		str = err_string[yyerrno-1];
	else
		str = "Syntax error";
		fprintf(stderr, "line %d: %s.\n", yylineno, str);
	freeall();
	(void) unlink(fname);
	exit(1);
}

/* create wfont header */
wf_header(name, top, bottom)
	char           *name;
	float           top,
	                bottom;
{

	if (name == NULL)
		BYE(WRONG_NAME);
	head.top = (float) top;
	head.bottom = (float) bottom;
	head.magic = WFONT_MAGIC_PEX;
	(void) strcpy(head.name, name);
	free(name);
}

/* create header for each glyph */
glyph_header(npath, center, right, npts)
	int             npath,
	                npts;
	float           center,
	                right;
{
     {
	register Path  *strk = strokes + tableindex;
	
        if (npath > 0)     /* Don't allocate space unless the character
			      has strokes associated with it. */
	{
		strk->subpaths = (Path_subpath *)
			malloc(npath * sizeof (Path_subpath));

		if (strk->subpaths == NULL)
			BYE(NO_MEMORY);

		strk->type = PATH_2DF;
		strk->n_subpaths = npath;
		strk->n_vertices = npts;
	}
	else {            /* Just initialize the entry */
	        strk->subpaths = NULL;
		strk->type = PATH_2DF;
		strk->n_subpaths = 0;
		strk->n_vertices = 0;
	}
      }
      {
		register Dispatch *tbl = table + tableindex;

		tbl->offset = 0;
		tbl->center = center;
		tbl->right = right;
      }
	count.path = -1;	       /* initialize path counter, not to
				        * exceed n_subpath */
}

/* create standard spacing info for each glyph  */
std_space(l_bear, wide, r_bear)

	float l_bear,
	      wide,
	      r_bear;
{
	register Standard *tbl = sp_table +tableindex;
	tbl->std_left = l_bear;
	tbl->std_wide = wide;
	tbl->std_rght = r_bear;
}

/* initialize each sub_path */
init_path(close, n)
	int             close,
	                n;
{
	register Path_subpath *path;

	if (++count.path >= strokes[tableindex].n_subpaths)
		BYE(EXCEED_PATH);
	path = current_path = strokes[tableindex].subpaths + count.path;
	path->n_pts = n;
	path->closed = close;
	if (n > 0) 
	  path->pts.pt2df = (Path_point2df *) 
	                     malloc(n * sizeof (Path_point2df));
	if (path->pts.pt2df == NULL)
		BYE(NO_MEMORY);
	expect.point = path->n_pts;
	count.point = -1;	       /* initialize point counter, not to
				        * exceed n_pts */
}

/* accumulating points for each sub_path */
add_point(x, y)
	float           x,
	                y;
{
	register Path_subpath   *path;
	register Path_point2df	*pt_ptr;

	path = current_path;
	if (++count.point >= path->n_pts)
		BYE(EXCEED_POINT);
	pt_ptr = path->pts.pt2df + count.point;
	pt_ptr->x = x;
	pt_ptr->y = y;
}

/* Path_type + n_subpaths + n_vertices */
#define STROKE_HEAD (sizeof (Path_type) + sizeof (int) + sizeof (int))

/* write out file, close everything, free everything */
fini()
{
	static long     zero = 0;

	/* pointers used to walk the arrays */
	register Path_subpath *spath;
	register Dispatch *tbl_ptr;
	register Path  *strptr;
	register Property *prop_ptr;

	FILE           *fp;
	long            offset;
	int             npath;
	register int    i,
	                j,
			k;
	Standard	*sp_ptr;
	Path_point2df	*pt;

	if (fname[0] == 0) {	       /* default output file name */
		(void) strcpy(fname, head.name);
		(void) strcat(fname, ".phont");
	}

	if ((fp = fopen(fname, "w")) == NULL)
		BYE(OPEN_ERROR);

#	define BY_BYE(err) fclose(fp), BYE(err)

	/* adjust the characters for spacing, note max char width */
	head.max_width = 0.0;
	for (i = 0, tbl_ptr = table, strptr = strokes, sp_ptr = sp_table;
             i < head.num_ch; i++, tbl_ptr++, strptr++, sp_ptr++) {
		tbl_ptr->center += sp_ptr->std_left;
		tbl_ptr->right += sp_ptr->std_left + sp_ptr->std_rght;
		if (tbl_ptr->right > head.max_width)
			head.max_width = tbl_ptr->right;
		npath = strptr->n_subpaths;
		if (npath > 0 || tbl_ptr->center != 0.0 ||
                    tbl_ptr->right != 0.0) {
			for (j = 0, spath = strptr->subpaths;
                             j < npath; j++, spath++) {
				for(k=0, pt = spath->pts.pt2df;
				     k<spath->n_pts; k++, pt++) {
					pt->x += sp_ptr->std_left;
				}
			}
		}
	}
	/* write the header info */
	if (fwrite(&head, sizeof (head), 1, fp) != 1)
		BY_BYE(WRITE_ERROR);

	(void) fseek(fp, START_PROPS, L_SET);

	/* write the property info, if any */
	if (head.num_props > 0) {
	  for (i=0, prop_ptr = head.properties; i < head.num_props;
	       i++, prop_ptr++) {

	       /* write the property name */
	       if (fwrite(prop_ptr->propname, 80, 1, fp) != 1)
		  BY_BYE(WRITE_ERROR);

	       /* write the property value */
	       if (fwrite(prop_ptr->propvalue, 80, 1, fp) != 1)
		  BY_BYE(WRITE_ERROR);
	     }
	}

	(void) fseek(fp, START_DISPATCH(head.num_props), L_SET);
	offset = START_PATH(head.num_ch, head.num_props);


	/* write the dispatch table */
	for (i = 0, tbl_ptr = table, strptr = strokes;
	     i < head.num_ch; i++, tbl_ptr++, strptr++) {
		if (fwrite(&tbl_ptr->center, sizeof (float), 1, fp) != 1 ||
		    fwrite(&tbl_ptr->right, sizeof (float), 1, fp) != 1)
			BY_BYE(WRITE_ERROR);
		npath = strptr->n_subpaths;
		if (npath > 0 || tbl_ptr->center != 0.0 ||
		    tbl_ptr->right != 0.0) {
			if (fwrite(&offset, sizeof (offset), 1, fp) != 1)
				BY_BYE(WRITE_ERROR);
			offset += STROKE_HEAD + npath * 2 * sizeof (int);
			for (j = 0, spath = strptr->subpaths;
			     j < npath; j++, spath++)
				offset += spath->n_pts * sizeof (Path_point2df);
		}
		else if (fwrite(&zero, sizeof (long), 1, fp) != 1)
			BY_BYE(WRITE_ERROR);
	}

	(void) fseek(fp, START_PATH(head.num_ch, head.num_props), L_SET);

	/* write the stroke table */
	for (i = 0, tbl_ptr = table, strptr = strokes;
	     i < head.num_ch; i++, tbl_ptr++, strptr++) {
		if (strptr->n_subpaths > 0 ||
		    tbl_ptr->center != 0.0 ||
		    tbl_ptr->right != 0.0) {
			if (fwrite(strptr, STROKE_HEAD, 1, fp) != 1)
				BY_BYE(WRITE_ERROR);

			for (j = 0, spath = strptr->subpaths;
			     j < strptr->n_subpaths; j++, spath++) {

				if (fwrite(&spath->n_pts,
				       sizeof (spath->n_pts), 1, fp) != 1 ||
				    fwrite(&spath->closed,
				      sizeof (spath->closed), 1, fp) != 1 ||
				    fwrite(spath->pts.pt2df,
					   sizeof (Path_point2df),
					   spath->n_pts, fp) != spath->n_pts)
					BY_BYE(WRITE_ERROR);
			}
		}
	}

	(void) fclose(fp);
	freeall();
#	undef BY_BYE
}

freeall()
{
	register Path  *path;
	register Path_subpath *spath;
	register int    i,
	                j,
	                n;

	path = strokes;
	for (i = 0; i < head.num_ch; i++, path++) {
		n = path->n_subpaths;
		if (n <= 0)
			continue;
		spath = path->subpaths;
		for (j = 0; j < n; j++, spath++)
			if (spath->pts.pt2df != NULL)
				free((char *) spath->pts.pt2df);
		if (path->subpaths != NULL)
			free((char *) path->subpaths);
	}
	free(table);
	free(sp_table);
	free(strokes);
	for (i=0; i < head.num_props; i++, head.properties++) {
	  if (head.properties != NULL)
	    free((char *) head.properties);
	}
}

check_nstroke()
{
	count.path++;
	if (expect.path != count.path)
		BYE(PATH_MISMATCH);
}

check_npts()
{
	count.point++;
	if (expect.point != count.point)
		BYE(POINT_MISMATCH);
}
