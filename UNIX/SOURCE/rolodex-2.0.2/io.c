/* io.c */
#include <stdio.h>
#ifdef UNIX
#include <sys/types.h>
#include <sys/stat.h>
#ifdef BSD
#include <sys/file.h>
#else
#include <fcntl.h>
#endif
#endif

#ifdef VMS
#include <types.h>
#include <stat.h>
#include <file.h>
#endif

#ifdef MSDOS
#	ifdef MSC
#	include <sys/types.h>
#	endif	/* MSC */
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include <ctype.h>

#ifdef TMC
#include <ctools.h>
#else
#include "ctools.h"
#endif

#include "rolofilz.h"
#include "datadef.h"


char *Field_Names[N_BASIC_FIELDS] = {
        
        "Name: ", "Work Phone: ", "Home Phone: ", "Company: ",
        "Work Address: ", "Home Address: ", "Remarks: ", "Date Updated: "
        
     };

Ptr_Rolo_List Begin_Rlist = 0;
Ptr_Rolo_List End_Rlist = 0;
Ptr_Rolo_List Current_Entry = 0;

static char *rolofiledata;

/*
 * The following has been added to accomplish two goals:
 *
 * 1)  Document all help files expected in the system, to avoid/identify
 *     lost elements in distribution.
 *
 * 2)  Allow a flexible, easily-modified way of changing filenames as
 *     necessary/desired to comply with different operating systems.
 */
char *hlpfiles[] = {
	"addhelp.hlp",	/* ADDHELP		*/
	"addinfo.hlp",	/* ADDINFO		*/
	"confirm.hlp",	/* CONFIRMHELP		*/
	"entrymnu.hlp",	/* ENTRYMENU		*/
	"escan.hlp",	/* ESCANHELP		*/
	"esearch.hlp",	/* ESEARCHHELP	  	*/
	"fldsrch.hlp",	/* FIELDSEARCHHELP  	*/
#ifdef UNIX
	"lockinfo.unx",	/* LOCKINFO	    	*/
#endif
#ifdef VMS
	"lockinfo.vms",	/* LOCKINFO	    	*/
#endif
#ifdef MSDOS
	"lockinfo.dos",	/* LOCKINFO	    	*/
#endif
	"mainmenu.hlp",	/* MAINMENU		*/
	"mnymtch.hlp",	/* MANYMATCHHELP     	*/
	"moption.hlp",	/* MOPTIONHELP	    	*/
	"moptions.hlp",	/* MOPTIONSHELP	    	*/
	"moreflds.hlp",	/* MOREFIELDSHELP    	*/
	"newadd.hlp",	/* NEWADDHELP	     	*/
	"otherfmt.hlp",	/* OTHERFORMATHELP 	*/
	"pkentry.hlp",	/* PICKENTRYHELP     	*/
	"pkntmenu.hlp",	/* PICKENTRYMENU	*/
	"poptmenu.hlp",	/* POPTIONMENU	     	*/
	"poptions.hlp",	/* POPTIONSHELP	     	*/
	"srchstr.hlp",	/* SEARCHSTRINGHELP  	*/
	"update.hlp",	/* UPDATEHELP	    	*/
	"updatmnu.hlp",	/* UPDATEMENU	   	*/
	"usrfld.hlp",	/* USERFIELDHELP     	*/
};


read_rolodex (fd) int fd;
                                             
{
  struct stat statdata;
  long filesize;
  int i,j,k,start_of_others,warning_given;
  Ptr_Rolo_Entry newentry;
  Ptr_Rolo_List newlink,rptr;
  char *next_field,*next_other;
  char **other_pointers;
  int n_entries = 0;

  /* find out how many bytes are in the file */

  fstat(fd,&statdata);
  if ((filesize = statdata.st_size) == 0) {
     return(0);
  }

  /* create an array of characters that big */
  
  rolofiledata = rolo_emalloc(filesize);

  /* read them all in at once for efficiency */
  
#ifdef MSDOS
  /*
   * Unlike Unix, MS-DOS compilers make a distinction between text and
   * binary files.  Unfortunately, this means that in text mode, the file
   * size is reported by the stat call won't necessarily match the value
   * reported by the read, since there is CR/LF character translation.
   * So, the best we can hope for here is that a failed read will give a
   * zero or negative return value...
   */
  if ((filesize = read(fd,rolofiledata,filesize)) <= 0) {
#else
  if (filesize != read(fd,rolofiledata,filesize)) {
#endif
     fprintf(stderr,"rolodex read failed\n");
     exit(-1);
  }

  j = 0;
  
  /* for each entry in the rolodex file */
  
  while (j < filesize) {

      n_entries++;
        
      /* create the link and space for the data entry */
        
      newlink = new_link_with_entry();
      newentry = get_entry(newlink);
      if (j == 0) {
         Begin_Rlist = newlink;
         set_prev_link(newlink,0);
         set_next_link(newlink,0);
      }
      else {
          set_next_link(End_Rlist,newlink);
          set_prev_link(newlink,End_Rlist);
          set_next_link(newlink,0);
      }
      End_Rlist = newlink;

      /* locate each required field in the character array and change */
      /* the ending line feed to a null.  Insert a pointer to the */
      /* beginning of the field into the data entry */

      for (i = 0; i < N_BASIC_FIELDS; i++) {
          next_field = rolofiledata + j;
          while (rolofiledata[j] != '\n') {
            j++;
          }
          rolofiledata[j] = '\0';
          j++;
          set_basic_rolo_field(i,newentry,next_field);
      }

      /* the end of an entry is indicated by two adjacent newlines */

      if (rolofiledata[j] == '\n') {
         j++;
         newentry -> other_fields = 0;
         continue;
      }

      /* there must be additional, user-inserted fields. Find out how many. */

      start_of_others = j;
      while (1) {
        while (rolofiledata[j] != '\n') {
          j++;
        }
        incr_n_others(newentry);
        j++;
        if (rolofiledata[j] == '\n') {
           j++;
           break;
        }
     }

     /* allocate an array of character pointers to hold these fields */

     other_pointers = (char **)rolo_emalloc(get_n_others(newentry)*sizeof(char *));

     /* separate each field and insert a pointer to it in the char array */

     k = start_of_others;
     for (i = 0; i < get_n_others(newentry); i++) {
         next_other = rolofiledata + k;
         while (rolofiledata[k] != '\n') {
           k++;
         }
         rolofiledata[k] = '\0';
         other_pointers[i] = next_other;
         k++;
     }

     /* insert the pointer to this character array into the data entry */

     newentry -> other_fields = other_pointers;

  }

  /* check that all the entries are in alphabetical order by name */
  
  warning_given = 0;
  rptr = get_next_link(Begin_Rlist);
  while (rptr != 0) {
    if (1 == compare_links(get_prev_link(rptr),rptr)) {
       if (!warning_given) fprintf(stderr,"Warning, rolodex out of order\n");
       warning_given = 1;
       reorder_file = 1;
    }
    rptr = get_next_link(rptr);
  }    
    
  return(n_entries);
  
}


write_rolo_list (fp) FILE *fp; 

/* write the entire in-core rolodex to a file */

{

  Ptr_Rolo_List rptr;
  Ptr_Rolo_Entry lentry;
  int j;

  rptr = Begin_Rlist;

  while (rptr != 0) {
    lentry = get_entry(rptr);
    for (j = 0; j < N_BASIC_FIELDS; j++) {
        fprintf(fp,"%s\n",get_basic_rolo_field(j,lentry));
    }
    for (j = 0; j < get_n_others(lentry); j++) {
        fprintf(fp,"%s\n",get_other_field(j,lentry));
    }
    fprintf(fp,"\n");
    rptr = get_next_link(rptr);
  }

}


write_rolo (fp1,fp2) FILE *fp1; FILE *fp2;

{
  write_rolo_list(fp1);
  write_rolo_list(fp2);
}


display_basic_field (name,value,show,up) char *name; char *value; int show,up;
{
  int i;
  if ((value == (char *)NULL) || (all_whitespace(value) && !show)) return;
  printf("%-25s",name);
  while (*value != '\0') {
    if (*value == ';') {
       while (*++value == ' '); 
       putchar('\n');
       for (i = 0; i < (up ? 28 : 25); i++) putchar(' ');
    }
    else {
       putchar(*value++);
    }
  }
  putchar('\n');
}


display_other_field (fieldstring) char *fieldstring;
{
  int already_put_sep = 0;        
  int count = 0;
  int i;
  while (*fieldstring != '\0') {
    if (*fieldstring == ';' && already_put_sep) {
       while (*++fieldstring == ' ');
       putchar('\n');
       for (i = 0; i < 25; i++) putchar(' ');
       continue;
    }
    putchar(*fieldstring);
    count++;
    if (*fieldstring == ':' && !already_put_sep) {
       for (i = count; i < 24; i++) putchar(' ');
       already_put_sep = 1;
    }
    fieldstring++;
  }
  putchar('\n');
}


summarize_entry_list (rlist,ss) Ptr_Rolo_List rlist; char *ss;

/* print out the Name field for each entry that is tagged as matched */
/* and number each entry. */

{
  int count = 1;
  clear_the_screen();
  printf("Entries that match '%s' :\n\n",ss);
  while (rlist != 0) {
    if (get_matched(rlist)) {
       printf (
          "%d. \t%s\n",
          count++,
          get_basic_rolo_field((int) R_NAME,get_entry(rlist))
       );
    }
    rlist = get_next_link(rlist);    
  }
  putchar('\n');
}


display_field_names ()

/* display and number each standard field name. */

{
  int j;
  char *name;
  clear_the_screen();
  for (j = 0; j < N_BASIC_FIELDS - 1; j++) {        
      name = Field_Names[j];        
      printf("%d. ",j+1);
      while (*name != ':') putchar(*name++);
      putchar('\n');
  }
  printf("%d. ",N_BASIC_FIELDS);
  printf("A user created item name\n\n");
}  
  
display_entry (lentry) Ptr_Rolo_Entry lentry;
{
  int j,n_others;
  char *string;
  
  clear_the_screen();
  
  /* display the standard fields other than Date Updated */
  
  for (j = 0; j < N_BASIC_FIELDS - 1; j++) {
      string = get_basic_rolo_field(j,lentry);
      display_basic_field(Field_Names[j],string,0,0);
  }        
      
  /* display any additional fields the user has defined for this entry */
  
  n_others = get_n_others(lentry);
  for (j = 0; j < n_others; j++) {
      string = get_other_field(j,lentry);
      display_other_field(string);
   }

   /* display the Date Updated field */
   
   j = N_BASIC_FIELDS - 1;
   display_basic_field(Field_Names[j],get_basic_rolo_field(j,lentry),0,0);
   fprintf(stdout,"\n");

}


display_entry_for_update (lentry) Ptr_Rolo_Entry lentry;

/* same as display_entry, except each item is numbered and the Date Updated */
/* item is not displayed */

{
  int j,n_others;
  char *string;
  int count = 1;
  
  clear_the_screen();
  
  for (j = 0; j < N_BASIC_FIELDS - 1; j++) {
      string = get_basic_rolo_field(j,lentry);
      printf("%d. ",count++);
      display_basic_field(Field_Names[j],string,1,1);
  }        
      
  n_others = get_n_others(lentry);
  for (j = 0; j < n_others; j++) {
      string = get_other_field(j,lentry);
      printf("%d. ",count++);
      display_other_field(string);
  }
  
  printf("%d. Add a new user defined field\n",count);

  fprintf(stdout,"\n");

}


int cathelpfile (fileidx,helptopic,clear)

  int fileidx;
  char *helptopic; 
  int clear;

{
  register char *filepath;
  FILE *fp;
  char buffer[MAXLINELEN];

  if(fileidx > LAST_HELP) {
	fprintf(stderr,
		"INTERNAL ERROR:  Error file index, max: %d, requested: %d\n",
		LAST_HELP,fileidx);
	return;
  }else
  	filepath = libdir(hlpfiles[fileidx]);
	
  if (clear) clear_the_screen();
  if (NULL == (fp = fopen(filepath,"r"))) {
     if (helptopic) {
        printf("No help available on %s, sorry.\n\n",helptopic); 
     }
     else {
        fprintf(stderr,"Fatal error, can't open %s\n",filepath);
        exit(-1);
     }
     return;
  }
  while (NULL != fgets(buffer,MAXLINELEN,fp)) printf("%s",buffer);  
  printf("\n");
  fclose(fp);
  return;
}


any_char_to_continue ()
{
  char buffer[80];
  printf("RETURN to continue: ");
  fgets(buffer,80,stdin);
  return;
}
