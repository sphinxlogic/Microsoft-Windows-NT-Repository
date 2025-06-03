#include <stdio.h>

#include "config.h"
#include "buffer.h"
#include "text.h"
#include "ledit.h"
#include "ins.h"
#include "cmds.h"
#include "misc.h"


#ifdef HAS_ABBREVS

int expand_abbrev (unsigned char ch)
{
   Abbrev_Table_Type *tbl;
   int len, i;
   unsigned char *p;
   char *abbrev;
   
   tbl = CBuf->abbrev_table;
   if (tbl == NULL) 
     {
	tbl = Global_Abbrev_Table;
	if (tbl == NULL) return 0;
     }
   
   if (tbl->len == 0) return 0;
   
   
   if (tbl->word_chars[(int) ch]) return 0;   /* not a delimiter */

   p = CLine->data + (Point - 1);
   
   while ((p >= CLine->data) && (tbl->word_chars[(int) *p]))
     {
	p--;
     }
   p++;
   
   len = (CLine->data + Point) - p;
   if ((len == 0) || (len >= MAX_ABBREV_LEN / 2)) return 0;
   /* It makes no sense to have an abbrev larger than its expansion! */
   
   ch = *p;
   for (i = 0; i < tbl->len; i++)
     {
	abbrev = tbl->abbrevs [i];
	if ((*abbrev == (char) ch) && (abbrev[len] == 0) && 
	    (0 == strncmp (abbrev, (char *) p, len)))
	  {
	     Point -= len;
	     abbrev += len + 1;
	     deln (&len);
	     len = strlen (abbrev);
	     if (CBuf->flags & OVERWRITE_MODE)
	       {
		  deln (&len);	       /* this does not delete across lines */
	       }
	     ins_chars ((unsigned char *) abbrev, len);
	     return 1;
	  }
     }
   return 0;
}

static Abbrev_Table_Type *Abbrev_Tables [MAX_ABBREV_TABLES];

static Abbrev_Table_Type *find_table (char *name, int *loc)
{
   Abbrev_Table_Type **tbl, **max;
   
   tbl = Abbrev_Tables;
   max = tbl + MAX_ABBREV_TABLES;
   
   while ((tbl < max) && (*tbl != NULL))
     {
	if (strcmp (name, (*tbl)->name) == 0)
	  {
	     break;
	  }
	tbl++;
     }
   *loc = (int) (tbl - Abbrev_Tables);
   if (tbl == max) return NULL;
   return *tbl;
}

Abbrev_Table_Type *Global_Abbrev_Table;

void create_abbrev_table (char *name, char *word_chars)
{
   Abbrev_Table_Type *tbl;
   int loc;
   
   tbl = find_table (name, &loc);
   if (tbl == NULL)
     {
	if (loc == MAX_ABBREV_TABLES) 
	  {
	     msg_error ("Abbrev Table Quota reached.");
	     return;
	  }
	
	tbl = (Abbrev_Table_Type *) SLMALLOC (sizeof (Abbrev_Table_Type));
	if (tbl == NULL)
	  {
	     msg_error ("Malloc error.");
	     return;
	  }
	
	Abbrev_Tables [loc] = tbl;
     }
   
   MEMSET ((char *) tbl, 0, sizeof (Abbrev_Table_Type));
   
   if (*word_chars == 0) word_chars = Jed_Word_Range;
   
   jed_make_lut ((unsigned char *) tbl->word_chars, (unsigned char *) word_chars, 0);
   
   strncpy (tbl->name, name, 15);      /* The null char is here because of
					* the MEMSET
					*/
   
   if (strcmp (name, "Global") == 0) Global_Abbrev_Table = tbl;
}

   
   
void define_abbrev (char *table, char *abbrev, char *expans)
{
   Abbrev_Table_Type *tbl;
   int len, lena;
   int i;
   char *a;
   
   tbl = find_table (table, &len);
   if (tbl == NULL) 
     {
	msg_error ("Table does not exist.");
	return;
     }
   
   if (tbl->len == MAX_ABBREVS) return; /* silently return */
   
   lena = strlen (abbrev);
   len = lena + strlen (expans) + 1;
   
   if (len >= MAX_ABBREV_LEN)
     {
	msg_error ("Expansion is too long.");
     }
   
   for (i = 0; i < MAX_ABBREVS; i++)
     {
	a = tbl->abbrevs[i];
	if ((*a == 0) 
	    || ((a[lena] == 0) && (0 == strcmp (a, abbrev))))
	  {
	     strcpy (a, abbrev);
	     strcpy (a + lena + 1, expans);
	     tbl->len++;
	     return;
	  }
     }
}


void use_abbrev_table (char *name)
{
   Abbrev_Table_Type *tbl;
   int loc;
   
   tbl = find_table (name, &loc);
   if (tbl == NULL) 
     {
	msg_error ("Table is not defined.");
	return;
     }
   CBuf->abbrev_table = tbl;
}

int abbrev_table_p (char *name)
{
   int loc;
   if (find_table (name, &loc) != NULL) return 1;
   return 0;
}

char *what_abbrev_table (void)
{
   Abbrev_Table_Type *tbl = CBuf->abbrev_table;
   
   if (tbl == NULL) tbl = Global_Abbrev_Table;
   if (tbl == NULL) return "";
   return tbl->name;
}

void dump_abbrev_table (char *name)
{
   Abbrev_Table_Type *tbl;
   char *abbrev;
   int len, i;
   
   if (NULL == (tbl = find_table (name, &len))) return;
   
   for (i = 0; i < tbl->len; i++)
     {
	abbrev = tbl->abbrevs [i];
	if (*abbrev)
	  {
	     insert_string (abbrev);
	     ins ('\t');
	     insert_string (abbrev + (1 + strlen (abbrev)));
	     newline ();
	  }
     }
}


	
	     

#endif   /* HAS_ABBREVS */
