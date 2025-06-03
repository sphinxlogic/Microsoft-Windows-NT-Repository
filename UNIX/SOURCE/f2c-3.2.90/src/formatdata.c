/****************************************************************
Copyright 1990 by AT&T Bell Laboratories and Bellcore.

Permission to use, copy, modify, and distribute this software
and its documentation for any purpose and without fee is hereby
granted, provided that the above copyright notice appear in all
copies and that both that the copyright notice and this
permission notice and warranty disclaimer appear in supporting
documentation, and that the names of AT&T Bell Laboratories or
Bellcore or any of their entities not be used in advertising or
publicity pertaining to distribution of the software without
specific, written prior permission.

AT&T and Bellcore disclaim all warranties with regard to this
software, including all implied warranties of merchantability
and fitness.  In no event shall AT&T or Bellcore be liable for
any special, indirect or consequential damages or any damages
whatsoever resulting from loss of use, data or profits, whether
in an action of contract, negligence or other tortious action,
arising out of or in connection with the use or performance of
this software.
****************************************************************/

#include "ctype.h"		/* For isspace() */
#include "defs.h"
#include "output.h"
#include "names.h"
#include "format.h"

#define MAX_INIT_LINE 100
#define NAME_MAX 64

static int memno2info();

extern int in_string;
extern char *str_fmt[];
extern void define_start();

void list_init_data(Infile, Inname, outfile)
 FILE **Infile, *outfile;
 char *Inname;
{
    char buf[200];
    FILE *sortfp;
    int status;

    fclose(*Infile);
    *Infile = 0;

    sprintf (buf, "sort %s > %s", Inname, sortfname);
    if (status = system (buf) >> 8)
	fatali ("System call to sort failed, status %d", status);

    if ((sortfp = fopen (sortfname, "r")) == NULL)
	Fatal("Couldn't open sorted initialization data");

    do_init_data(outfile, sortfp);
    fclose(sortfp);

/* Insert a blank line after any initialized data */

	nice_printf (outfile, "\n");

    if (debugflag && infname)
	 /* don't back block data file up -- it won't be overwritten */
	backup_filename (initfname);
} /* list_init_data */



/* do_init_data -- returns YES when at least one declaration has been
   written */

int do_init_data(outfile, infile)
FILE *outfile, *infile;
{
    char varname[NAME_MAX], ovarname[NAME_MAX];
    ftnint offset;
    ftnint type;
    int vargroup;	/* 0 --> init, 1 --> equiv, 2 --> common */
    int did_one = 0;		/* True when one has been output */
    chainp values = CHNULL;	/* Actual data values */
    int keepit = 0;
    Namep np;

    ovarname[0] = '\0';

    while (rdname (infile, &vargroup, varname) && rdlong (infile, &offset)
	    && rdlong (infile, &type)) {
	if (strcmp (varname, ovarname)) {

	/* If this is a new variable name, the old initialization has been
	   completed */

		write_one_init(outfile, ovarname, &values, keepit);

		strcpy (ovarname, varname);
		values = CHNULL;
		if (vargroup == 0) {
			if (memno2info(atoi(varname+2), &np)) {
				if (((Addrp)np)->uname_tag != UNAM_NAME) {
					err("do_init_data: expected NAME");
					goto Keep;
					}
				np = ((Addrp)np)->user.name;
				}
			if (!(keepit = np->visused) && !np->vimpldovar)
				warn1("local variable %s never used",
					np->fvarname);
			}
		else {
 Keep:
			keepit = 1;
			}
		if (keepit && !did_one) {
			nice_printf (outfile, "/* Initialized data */\n\n");
			did_one = YES;
			}
	} /* if strcmp */

	values = mkchain((char *)data_value(infile, offset, (int)type), values);
    } /* while */

/* Write out the last declaration */

    write_one_init (outfile, ovarname, &values, keepit);

    return did_one;
} /* do_init_data */


 void
write_char_len(outfile, dimp, n, extra1)
 FILE *outfile;
 int n;
 struct Dimblock *dimp;
 int extra1;
{
	int i, nd;
	expptr e;

	if (!dimp) {
		nice_printf (outfile, extra1 ? "[%d+1]" : "[%d]", n);
		return;
		}
	nice_printf(outfile, "[%d", n);
	nd = dimp->ndim;
	for(i = 0; i < nd; i++) {
		e = dimp->dims[i].dimsize;
		if (!ISICON (e))
			err ("write_char_len:  nonconstant array size");
		else
			nice_printf(outfile, "*%d", e->constblock.Const.ci);
		}
	/* extra1 allows for stupid C compilers that complain about
	 * too many initializers in
	 *	char x[2] = "ab";
	 */
	nice_printf(outfile, extra1 ? "+1]" : "]");
	}

 static int ch_ar_dim = -1; /* length of each element of char string array */
 static int eqvmemno;	/* kludge */

 static void
write_char_init(outfile, Values, namep)
 FILE *outfile;
 chainp *Values;
 Namep namep;
{
	struct Equivblock *eqv;
	long size;
	struct Dimblock *dimp;
	int i, nd, type;
	expptr ds;

	if (!namep)
		return;
	if(nequiv >= maxequiv)
		many("equivalences", 'q', maxequiv);
	eqv = &eqvclass[nequiv];
	eqv->eqvbottom = 0;
	type = namep->vtype;
	size = type == TYCHAR
		? namep->vleng->constblock.Const.ci
		: typesize[type];
	if (dimp = namep->vdim)
		for(i = 0, nd = dimp->ndim; i < nd; i++) {
			ds = dimp->dims[i].dimsize;
			if (!ISICON(ds))
				err("write_char_values: nonconstant array size");
			else
				size *= ds->constblock.Const.ci;
			}
	*Values = revchain(*Values);
	eqv->eqvtop = size;
	eqvmemno = ++lastvarno;
	eqv->eqvtype = type;
	write_equiv_init(outfile, nequiv, Values, 0);
	define_start(outfile, namep->cvarname, CNULL, "");
	if (type == TYCHAR)
		indent_printf(0, outfile, "((char *)&equiv_%d)\n\n", eqvmemno);
	else
		indent_printf(0, outfile, dimp
			? "((%s *)&equiv_%d)\n\n" : "(*(%s *)&equiv_%d)\n\n",
			c_type_decl(type,0), eqvmemno);
	}

/* write_one_init -- outputs the initialization of the variable pointed to
   by   info.   When   is_addr   is true,   info   is an Addrp; otherwise,
   treat it as a Namep */

void write_one_init (outfile, varname, Values, keepit)
FILE *outfile;
char *varname;
chainp *Values;
int keepit;
{
    static int memno;
    static union {
	Namep name;
	Addrp addr;
    } info;
    Namep namep;
    int is_addr, size;
    ftnint last, loc;
    int is_scalar = 0;
    char *array_comment = NULL;
    chainp cp, values;
    extern char datachar[];
    static int e1[3] = {1, 0, 1};

    if (!keepit)
	goto done;
    if (varname == NULL || varname[1] != '.')
	goto badvar;

/* Get back to a meaningful representation; find the given   memno in one
   of the appropriate tables (user-generated variables in the hash table,
   system-generated variables in a separate list */

    memno = atoi(varname + 2);
    switch(varname[0]) {
	case 'q':
		/* Must subtract eqvstart when the source file
		 * contains more than one procedure.
		 */
		write_equiv_init(outfile, eqvmemno = memno - eqvstart, Values, 0);
		goto done;
	case 'Q':
		/* COMMON initialization (BLOCK DATA) */
		write_equiv_init(outfile, memno, Values, 1);
		goto done;
	case 'v':
		break;
	default:
 badvar:
		errstr("write_one_init:  unknown variable name '%s'", varname);
		goto done;
	}

    is_addr = memno2info (memno, &info.name);
    if (info.name == (Namep) NULL) {
	err ("write_one_init -- unknown variable");
	return;
	}
    if (is_addr) {
	if (info.addr -> uname_tag != UNAM_NAME) {
	    erri ("write_one_init -- couldn't get name pointer; tag is %d",
		    info.addr -> uname_tag);
	    namep = (Namep) NULL;
	    nice_printf (outfile, " /* bad init data */");
	} else
	    namep = info.addr -> user.name;
    } else
	namep = info.name;

	/* check for character initialization */

    *Values = values = revchain(*Values);
    if (info.name->vtype == TYCHAR) {
	for(last = 1; values; values = values->nextp) {
		cp = (chainp)values->datap;
		loc = (ftnint)cp->datap;
		if (loc > last) {
			write_char_init(outfile, Values, namep);
			goto done;
			}
		last = (int)cp->nextp->datap == TYBLANK
			? loc + (int)cp->nextp->nextp->datap
			: loc + 1;
		}
	}
    else {
	size = typesize[info.name->vtype];
	loc = 0;
	for(; values; values = values->nextp) {
		if ((int)((chainp)values->datap)->nextp->datap == TYCHAR) {
			write_char_init(outfile, Values, namep);
			goto done;
			}
		last = ((long) ((chainp) values->datap)->datap) / size;
		if (last - loc > 4) {
			write_char_init(outfile, Values, namep);
			goto done;
			}
		loc = last;
		}
	}
    values = *Values;

    nice_printf (outfile, "static %s ", c_type_decl (info.name -> vtype, 0));

    if (is_addr)
	write_nv_ident (outfile, info.addr);
    else
	output_name (outfile, info.name);

    if (namep)
	is_scalar = namep -> vdim == (struct Dimblock *) NULL;

    if (namep && !is_scalar)
	array_comment = info.name->vtype == TYCHAR
		? 0 : write_array_decls(outfile, namep->vdim, 1);

    if (info.name -> vtype == TYCHAR)
	if (ISICON (info.name -> vleng))

/* We'll make single strings one character longer, so that we can use the
   standard C initialization.  All this does is pad an extra zero onto the
   end of the string */
		write_char_len(outfile, namep->vdim, ch_ar_dim =
			info.name -> vleng -> constblock.Const.ci, e1[Ansi]);
	else
		err ("variable length character initialization");

    if (array_comment)
	nice_printf (outfile, "%s", array_comment);

    nice_printf (outfile, " = ");
    write_output_values (outfile, namep, values);
    ch_ar_dim = -1;
    nice_printf (outfile, ";\n");
 done:
    frchain(Values);
} /* write_one_init */




chainp data_value (infile, offset, type)
FILE *infile;
ftnint offset;
int type;
{
    char line[MAX_INIT_LINE + 1], *pointer;
    chainp vals, prev_val;
    long atol();
    char *newval;

    if (fgets (line, MAX_INIT_LINE, infile) == NULL) {
	err ("data_value:  error reading from intermediate file");
	return CHNULL;
    } /* if fgets */

/* Get rid of the trailing newline */

    if (line[0])
	line[strlen (line) - 1] = '\0';

#define iswhite(x) (isspace (x) || (x) == ',')

    pointer = line;
    prev_val = vals = CHNULL;

    while (*pointer) {
	register char *end_ptr, old_val;

/* Move   pointer   to the start of the next word */

	while (*pointer && iswhite (*pointer))
	    pointer++;
	if (*pointer == '\0')
	    break;

/* Move   end_ptr   to the end of the current word */

	for (end_ptr = pointer + 1; *end_ptr && !iswhite (*end_ptr);
		end_ptr++)
	    ;

	old_val = *end_ptr;
	*end_ptr = '\0';

/* Add this value to the end of the list */

	if (ONEOF(type, MSKREAL|MSKCOMPLEX))
		newval = cpstring(pointer);
	else
		newval = (char *)atol(pointer);
	if (vals) {
	    prev_val->nextp = mkchain(newval, CHNULL);
	    prev_val = prev_val -> nextp;
	} else
	    prev_val = vals = mkchain(newval, CHNULL);
	*end_ptr = old_val;
	pointer = end_ptr;
    } /* while *pointer */

    return mkchain((char *)offset, mkchain((char *)type, vals));
} /* data_value */

 static union Constant *make_one_const();
 static long charlen;

void write_output_values (outfile, namep, values)
FILE *outfile;
Namep namep;
chainp values;
{
    int type = TYUNKNOWN;

    if (namep)
	type = namep -> vtype;

/* Handle array initializations away from scalars */

    if (namep && namep -> vdim) {
	write_array_init (outfile, namep -> vtype, values);
    } else {
	struct Constblock Const;
	static expptr Vlen;

	make_one_const(type, &Const.Const, values);
	Const.vtype = type;
	Const.vstg = ONEOF(type, MSKREAL|MSKCOMPLEX);
	if (type== TYCHAR) {
		if (!Vlen)
			Vlen = ICON(0);
		Const.vleng = Vlen;
		Vlen->constblock.Const.ci = charlen;
		output_const (outfile, &Const);
		free (Const.Const.ccp);
		}
	else
		output_const (outfile, &Const);
    } /* else */
} /* write_output_values */


write_array_init (outfile, type, values)
FILE *outfile;
int type;
chainp values;
{
    int size = typesize[type];
    long index, main_index = 0;
    int k;

    if (type == TYCHAR) {
	nice_printf(outfile, "\"");
	in_string = 1;
	k = 0;
	if (Ansi != 1)
		ch_ar_dim = -1;
	}
    else
	nice_printf (outfile, "{ ");
    while (values) {
	struct Constblock Const;

	index = ((long) ((chainp) values->datap)->datap) / size;
	while (index > main_index) {

/* Fill with zeros.  The structure shorthand works because the compiler
   will expand the "0" in braces to fill the size of the entire structure
   */

	    switch (type) {
	        case TYREAL:
		case TYDREAL:
		    nice_printf (outfile, "0.0,");
		    break;
		case TYCOMPLEX:
		case TYDCOMPLEX:
		    nice_printf (outfile, "{0},");
		    break;
		case TYCHAR:
			nice_printf(outfile, " ");
			break;
		default:
		    nice_printf (outfile, "0,");
		    break;
	    } /* switch */
	    main_index++;
	} /* while index > main_index */

	switch (type) {
	    case TYCHAR:
		{ int this_char;

		if (k == ch_ar_dim) {
			nice_printf(outfile, "\"");
			in_string = 0;
			nice_printf(outfile, " \"");
			in_string = 1;
			k = 0;
			}
		this_char = (int) ((chainp) values->datap)->
				nextp->nextp->datap;
		if ((int)((chainp)values->datap)->nextp->datap == TYBLANK) {
			main_index += this_char;
			k += this_char;
			while(--this_char >= 0)
				nice_printf(outfile, " ");
			values = values -> nextp;
			continue;
			}
		nice_printf(outfile,
			str_fmt[this_char & 0x7f],
			this_char);
		k++;
		} /* case TYCHAR */
	        break;

	    case TYSHORT:
	    case TYLONG:
	    case TYREAL:
	    case TYDREAL:
	    case TYLOGICAL:
	    case TYCOMPLEX:
	    case TYDCOMPLEX:
		make_one_const(type, &Const.Const, values);
		Const.vtype = type;
		Const.vstg = ONEOF(type, MSKREAL|MSKCOMPLEX);
		output_const(outfile, &Const);
	        break;
	    default:
	        erri("write_array_init: bad type '%d'", type);
	        break;
	} /* switch */
	values = values->nextp;

	main_index++;
	if (values && type != TYCHAR)
	    nice_printf (outfile, ",");
    } /* while values */

    if (type == TYCHAR) {
	nice_printf(outfile, "\"");
	in_string = 0;
	}
    else
	nice_printf (outfile, " }");
} /* write_array_init */


 static union Constant *
make_one_const(type, storage, values)
 int type;
 union Constant *storage;
 chainp values;
{
    union Constant *Const = (union Constant *) NULL;
    register char **L;

    if (type == TYCHAR) {
	char *str, *str_ptr, *Malloc ();
	chainp v, prev;
	int b = 0, k, main_index = 0;

/* Find the max length of init string, by finding the highest offset
   value stored in the list of initial values */

	for(k = 1, prev = CHNULL, v = values; v; prev = v, v = v->nextp)
	    ;
	if (prev != CHNULL)
	    k = ((int) (((chainp) prev->datap)->datap)) + 2;
		/* + 2 above for null char at end */
	str = Malloc (k);
	for (str_ptr = str; values; str_ptr++) {
	    int index = (int) (((chainp) values->datap)->datap);

	    while (index - main_index++ > 0)
		*str_ptr++ = ' ';

		k = (int) (((chainp) values->datap)->nextp->nextp->datap);
		if ((int)((chainp)values->datap)->nextp->datap == TYBLANK) {
			b = k;
			break;
			}
		*str_ptr = k;
		values = values -> nextp;
	} /* for str_ptr */
	*str_ptr = '\0';
	Const = storage;
	Const -> ccp = str;
	Const -> ccp1.blanks = b;
	charlen = str_ptr - str;
    } else {
	int i = 0;
	chainp vals;

	vals = ((chainp)values->datap)->nextp->nextp;
	if (vals) {
		L = (char **)(Const = storage);
		do L[i++] = vals->datap;
			while(vals = vals->nextp);
		}

    } /* else */

    return Const;
} /* make_one_const */



rdname (infile, vargroupp, name)
FILE *infile;
int *vargroupp;
char *name;
{
    register int i, c;

    c = getc (infile);

    if (feof (infile))
	return NO;

    *vargroupp = c - '0';
    for (i = 1;; i++) {
	if (i >= NAME_MAX)
		Fatal("rdname: oversize name");
	c = getc (infile);
	if (feof (infile))
	    return NO;
	if (c == '\t')
		break;
	*name++ = c;
    }
    *name = 0;
    return YES;
} /* rdname */

rdlong (infile, n)
FILE *infile;
ftnint *n;
{
    register int c;

    for (c = getc (infile); !feof (infile) && isspace (c); c = getc (infile))
	;

    if (feof (infile))
	return NO;

    for (*n = 0; isdigit (c); c = getc (infile))
	*n = 10 * (*n) + c - '0';
    return YES;
} /* rdlong */


 static int
memno2info (memno, info)
 int memno;
 Namep *info;
{
    chainp this_var;
    extern chainp new_vars;
    extern struct Hashentry *hashtab, *lasthash;
    struct Hashentry *entry;

    for (this_var = new_vars; this_var; this_var = this_var -> nextp) {
	Addrp var = (Addrp) this_var->datap;

	if (var == (Addrp) NULL)
	    Fatal("memno2info:  null variable");
	else if (var -> tag != TADDR)
	    Fatal("memno2info:  bad tag");
	if (memno == var -> memno) {
	    *info = (Namep) var;
	    return 1;
	} /* if memno == var -> memno */
    } /* for this_var = new_vars */

    for (entry = hashtab; entry < lasthash; ++entry) {
	Namep var = entry -> varp;

	if (var && var -> vardesc.varno == memno && var -> vstg == STGINIT) {
	    *info = (Namep) var;
	    return 0;
	} /* if entry -> vardesc.varno == memno */
    } /* for entry = hashtab */

    Fatal("memno2info:  couldn't find memno");
    return 0;
} /* memno2info */

 static chainp
do_string(outfile, v, nloc)
 FILEP outfile;
 register chainp v;
 ftnint *nloc;
{
	register chainp cp, v0;
	ftnint dloc, k, loc;
	extern char *chr_fmt[];
	unsigned long uk;
	char buf[8], *comma;

	nice_printf(outfile, "{");
	cp = (chainp)v->datap;
	loc = (ftnint)cp->datap;
	comma = "";
	for(v0 = v;;) {
		switch((int)cp->nextp->datap) {
			case TYBLANK:
				k = (ftnint)cp->nextp->nextp->datap;
				loc += k;
				while(--k >= 0) {
					nice_printf(outfile, "%s' '", comma);
					comma = ", ";
					}
				break;
			case TYCHAR:
				uk = (ftnint)cp->nextp->nextp->datap;
				sprintf(buf, chr_fmt[uk < 0x7f ? uk : 0x7f], uk);
				nice_printf(outfile, "%s'%s'", comma, buf);
				comma = ", ";
				loc++;
				break;
			default:
				goto done;
			}
		v0 = v;
		if (!(v = v->nextp))
			break;
		cp = (chainp)v->datap;
		dloc = (ftnint)cp->datap;
		if (loc != dloc)
			break;
		}
 done:
	nice_printf(outfile, "}");
	*nloc = loc;
	return v0;
	}

 static chainp
Ado_string(outfile, v, nloc)
 FILEP outfile;
 register chainp v;
 ftnint *nloc;
{
	register chainp cp, v0;
	ftnint dloc, k, loc;

	nice_printf(outfile, "\"");
	in_string = 1;
	cp = (chainp)v->datap;
	loc = (ftnint)cp->datap;
	for(v0 = v;;) {
		switch((int)cp->nextp->datap) {
			case TYBLANK:
				k = (ftnint)cp->nextp->nextp->datap;
				loc += k;
				while(--k >= 0)
					nice_printf(outfile, " ");
				break;
			case TYCHAR:
				k = (ftnint)cp->nextp->nextp->datap;
				nice_printf(outfile, str_fmt[k & 0x7f], k);
				loc++;
				break;
			default:
				goto done;
			}
		v0 = v;
		if (!(v = v->nextp))
			break;
		cp = (chainp)v->datap;
		dloc = (ftnint)cp->datap;
		if (loc != dloc)
			break;
		}
 done:
	nice_printf(outfile, "\"");
	in_string = 0;
	*nloc = loc;
	return v0;
	}

 static char *
Len(L)
 long L;
{
	static char buf[24];
	if (L == 1)
		return "";
	sprintf(buf, "[%ld]", L);
	return buf;
	}

write_equiv_init(outfile, memno, Values, iscomm)
 FILE *outfile;
 int memno;
 chainp *Values;
 int iscomm;
{
	struct Equivblock *eqv;
	char *equiv_name ();
	int curtype, dtype, filltype, filltype1, j, k, wasblank, xtype;
	static char Blank[] = "";
	register char *comma = Blank;
	register chainp cp, v;
	chainp sentinel, values, v1;
	ftnint L, L1, dL, dloc, loc, loc0;
	union Constant Const;
	char imag_buf[50], real_buf[50];
	int szshort = typesize[TYSHORT];
	static char typepref[] = {0, 0, TYSHORT, TYLONG, TYREAL, TYDREAL,
				  TYREAL, TYDREAL, TYLOGICAL, TYCHAR};
	char *z;

	/* add sentinel */
	if (iscomm) {
		L = extsymtab[memno].maxleng;
		xtype = extsymtab[memno].extype;
		}
	else {
		eqv = &eqvclass[memno];
		L = eqv->eqvtop - eqv->eqvbottom;
		xtype = eqv->eqvtype;
		}

	if (xtype != TYCHAR) {

		/* unless the data include a value of the appropriate
		 * type, we add an extra element in an attempt
		 * to force correct alignment */

		for(v = *Values;;v = v->nextp) {
			if (!v) {
				dtype = typepref[xtype];
				z = ISREAL(dtype) ? cpstring("0.") : (char *)0;
				k = typesize[dtype];
				if (j = L % k)
					L += k - j;
				v = mkchain((char *)L,
					mkchain((char *)dtype,
						mkchain(z, CHNULL)));
				*Values = mkchain((char *)v, *Values);
				L += k;
				break;
				}
			if ((int)((chainp)v->datap)->nextp->datap == xtype)
				break;
			}
		}

	sentinel = mkchain((char *)L, mkchain((char *)TYERROR,CHNULL));
	*Values = values = revchain(mkchain((char *)sentinel, *Values));

	/* use doublereal fillers only if there are doublereal values */

	k = TYLONG;
	for(v = values; v; v = v->nextp)
		if (ONEOF((int)((chainp)v->datap)->nextp->datap,
				M(TYDREAL)|M(TYDCOMPLEX))) {
			k = TYDREAL;
			break;
			}
	type_choice[0] = k;

	nice_printf(outfile, "%sstruct {\n", iscomm ? "" : "static ");
	next_tab(outfile);
	loc = loc0 = k = 0;
	curtype = -1;
	for(v = values; v; v = v->nextp) {
		cp = (chainp)v->datap;
		dloc = (ftnint)cp->datap;
		L = dloc - loc;
		if (L < 0) {
			extern char *filename0;
			fprintf(stderr, "Error");
			if (filename0)
				fprintf(stderr, " in file %s", filename0);
			fprintf(stderr, ": overlapping initializations\n");
			nerr++;
			v1 = cp;
			frchain(&v1);
			v->datap = 0;
			continue;
			}
		dtype = (int)cp->nextp->datap;
		if (dtype == TYBLANK) {
			dtype = TYCHAR;
			wasblank = 1;
			}
		else
			wasblank = 0;
		if (curtype != dtype || L > 0) {
			if (curtype != -1) {
				L1 = (loc - loc0)/dL;
				nice_printf(outfile, "%s e_%d%s;\n",
					typename[curtype], ++k, Len(L1));
				}
			curtype = dtype;
			loc0 = dloc;
			}
		if (L > 0) {
			if (xtype == TYCHAR)
				filltype = TYCHAR;
			else {
				filltype = L % szshort ? TYCHAR
						: type_choice[L/szshort % 4];
				filltype1 = loc % szshort ? TYCHAR
						: type_choice[loc/szshort % 4];
				if (typesize[filltype] > typesize[filltype1])
					filltype = filltype1;
				}
			nice_printf(outfile, "struct { %s filler%s; } e_%d;\n",
				typename[filltype],
				Len(L/typesize[filltype]), ++k);
			loc = dloc;
			}
		if (wasblank) {
			loc += (ftnint)cp->nextp->nextp->datap;
			dL = 1;
			}
		else {
			dL = typesize[dtype];
			loc += dL;
			}
		}
	nice_printf(outfile, "} %s = { ", iscomm
		? extsymtab[memno].cextname
		: equiv_name(eqvmemno, CNULL));
	loc = 0;
	for(v = values; ; v = v->nextp) {
		cp = (chainp)v->datap;
		if (!cp)
			continue;
		dtype = (int)cp->nextp->datap;
		if (dtype == TYERROR)
			break;
		dloc = (ftnint)cp->datap;
		if (dloc > loc) {
			nice_printf(outfile, "%s{0}", comma);
			comma = ", ";
			loc = dloc;
			}
		if (comma != Blank)
			nice_printf(outfile, ", ");
		comma = ", ";
		if (dtype == TYCHAR || dtype == TYBLANK) {
			v =  Ansi == 1  ? Ado_string(outfile, v, &loc)
					:  do_string(outfile, v, &loc);
			continue;
			}
		make_one_const(dtype, &Const, v);
		switch(dtype) {
			case TYLOGICAL:
				if (Const.ci < 0 || Const.ci > 1)
					errl(
			  "write_equiv_init: unexpected logical value %ld",
						Const.ci);
				nice_printf(outfile,
					Const.ci ? "TRUE_" : "FALSE_");
				break;
			case TYSHORT:
			case TYLONG:
				nice_printf(outfile, "%ld", Const.ci);
				break;
			case TYREAL:
				nice_printf(outfile, "%s",
					flconst(real_buf, Const.cds[0]));
				break;
			case TYDREAL:
				nice_printf(outfile, "%s", Const.cds[0]);
				break;
			case TYCOMPLEX:
				nice_printf(outfile, "%s, %s",
					flconst(real_buf, Const.cds[0]),
					flconst(imag_buf, Const.cds[1]));
				break;
			case TYDCOMPLEX:
				nice_printf(outfile, "%s, %s",
					Const.cds[0], Const.cds[1]);
				break;
			default:
				erri("unexpected type %d in write_equiv_init",
					dtype);
			}
		loc += typesize[dtype];
		}
	nice_printf(outfile, " };\n\n");
	prev_tab(outfile);
	frchain(&sentinel);
	}
