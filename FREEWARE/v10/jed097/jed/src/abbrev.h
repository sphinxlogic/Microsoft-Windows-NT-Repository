#define MAX_ABBREVS 48
#define MAX_ABBREV_LEN 48
#define MAX_ABBREV_TABLES 10
typedef struct Abbrev_Table_Type
{
   int len;			       /* current length of abbrev */
   int empty;			       /* number of cell known to be empty */
   char abbrevs [MAX_ABBREVS][MAX_ABBREV_LEN];
				       /* formatted as abbrev\0expansion */
   char word_chars[256];	       /* word delimiters */
   char name[16];		       /* name of table */
}
Abbrev_Table_Type;

extern Abbrev_Table_Type *Global_Abbrev_Table;
extern void use_abbrev_table (char *);
extern void define_abbrev (char *, char *, char *);
extern void create_abbrev_table (char *, char *);
extern int expand_abbrev (unsigned char);
extern int abbrev_table_p (char *);
extern void dump_abbrev_table (char *);
extern char *what_abbrev_table (void);



