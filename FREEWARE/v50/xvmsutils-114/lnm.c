/*
 *    Tranlate (VMS) logical name from "in" to "out".
 */
#include <string.h>
#include <lnmdef.h>
#include <descrip.h>
#include <starlet.h>
#include <stdio.h>

static struct dsc$descriptor_s create_descriptor (string)
char *string;
{
   struct dsc$descriptor_s descrip_string;

   /* build it */

   descrip_string.dsc$b_dtype	= DSC$K_DTYPE_T;
   descrip_string.dsc$b_class	= DSC$K_CLASS_S;
   descrip_string.dsc$w_length	= strlen(string);
   descrip_string.dsc$a_pointer	= string;

   /* return it */

   return (descrip_string);
}

int GetLogicalTab(Logical_Name, Table_Name, outbuf)
char *Logical_Name, *Table_Name, *outbuf;
{
	static short len;
	int istatus;
	int i;
	static char retbuf[256];
	char *rptr;
        struct dsc$descriptor_s search_list = create_descriptor(Table_Name);
        struct dsc$descriptor_s logical_name = create_descriptor(Logical_Name);
	struct {
	    short len;
	    short code;
	    char *buf;
	    short *retlen;
	} ItmLst[] = { {255, LNM$_STRING, retbuf, &len},
		       {0, 0, (char *)0, 0}
	};

	static long attr=LNM$M_CASE_BLIND;

	istatus = sys$trnlnm(&attr,
			     &search_list,
			     &logical_name,
			     0,
			     ItmLst);

	if (!(istatus&1)) {
	    return(istatus);
	} else {
	    retbuf[len] = '\0';
	    rptr = retbuf;
	    if (*rptr == 27)	/* process permanent file */
               rptr += 4;	/* skip past it the iff */
	    strcpy (outbuf, rptr);
	    return(1);
	}
}

int GetLogical(Logical_Name, outbuf)
char *Logical_Name, *outbuf;
{
	int istatus;
	istatus = GetLogicalTab(Logical_Name, "LNM$FILE_DEV", outbuf);

	return(istatus);
}

/*
 *	Routine to get number of translation strings for logical names
 */


int GetLogicalInd(Logical_Name)
char *Logical_Name;
{
	static short len;
	int istatus;
	static char retbuf[256];
        struct dsc$descriptor_s search_list =
        create_descriptor("LNM$FILE_DEV");
        struct dsc$descriptor_s logical_name =
        create_descriptor(Logical_Name);
	struct {
	    short len;
	    short code;
	    char *buf;
	    short *retlen;
	} ItmLst[] = { {255, LNM$_MAX_INDEX, retbuf, &len},
		       {0, 0, (char *)0, 0}
	};

	static long attr=LNM$M_CASE_BLIND;

	istatus = sys$trnlnm(&attr,
			     &search_list,
			     &logical_name,
			     0,
			     ItmLst);

	if (!(istatus&1)) {
	    return(-1);
	} else {
	    return((int)retbuf[0]);
	}
}

CreateLogical(name,table,definition, accmode)
char *name, *table, *definition;
int  accmode;
{
	struct dsc$descriptor_s table_name = create_descriptor (table);
	struct dsc$descriptor_s logical_name = create_descriptor (name);
	struct {
	    short len;
	    short item;
	    char *buffer;
	    short *buflen;
	} itmlst[2];
	itmlst[0].len = strlen(definition);
	itmlst[0].item = LNM$_STRING;
	itmlst[0].buffer = definition;
	itmlst[0].buflen = 0;
	itmlst[1].len = itmlst[1].item = 0;

	(void) sys$crelnm(0, &table_name,
			  &logical_name,
			  &accmode, itmlst);
}

/*
 *	Routine to delete logical names
 */

DeleteLogical(name,table)
char *name, *table;
{
	struct dsc$descriptor_s table_name = create_descriptor (table);
	struct dsc$descriptor_s logical_name = create_descriptor (name);
	(void) sys$dellnm(&table_name,
			  &logical_name,
			  0);
}

