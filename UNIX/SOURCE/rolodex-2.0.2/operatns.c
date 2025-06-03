/* operatns.c */
#include <stdio.h>
#include <ctype.h>

#ifdef TMC
#include <ctools.h>
#else
#include "ctools.h"
#endif
#include "args.h"
#include "menu.h"
#include "mem.h"

#include "rolofilz.h"
#include "rolodefs.h"
#include "datadef.h"
#include "choices.h"

#define index strchr 
char *index();

extern char *ctime();

Ptr_Rolo_List create_entry (basicdata,otherdata) char **basicdata, **otherdata;
{
  Ptr_Rolo_List newlink;        
  Ptr_Rolo_Entry newentry;
  int i,j;
  newlink = new_link_with_entry();        
  newentry = get_entry(newlink);
  for (j = 0; j < N_BASIC_FIELDS; j++) {
      set_basic_rolo_field(j,newentry,basicdata[j]);
  }
  j = 0;
  while (otherdata[j] != 0) j++;
  set_n_others(newentry,j);
  if (j > 0) {
     newentry -> other_fields = (char **) rolo_emalloc(j*sizeof(char *));
     for (i = 0; i < j; i++) {
         set_other_field(i,newentry,otherdata[i]);
     }
  }
  else newentry -> other_fields = 0;
  return(newlink);
}  
  

other_fields () 
{
  int rval;
  rval = rolo_menu_yes_no (
             "Additional fields? ",DEFAULT_NO,1,
             MOREFIELDSHELP, "additional fields"
          );
  return(rval == MENU_YES);
}


add_the_entry ()
{
  return(MENU_YES == rolo_menu_yes_no (
              "Add new entry to rolodex? ",DEFAULT_YES,1,
              NEWADDHELP, "adding newly entered entry"
           ));
}


rolo_add () 

{
  int i,j,k,rval,menuval;
  char *response;
  char *basicdata[N_BASIC_FIELDS], *otherdata[100];
  Ptr_Rolo_List rlink;

  if(read_only)
  {
	printf("Readonly mode: cannot add entries.\n");
	sleep(2);
	return(1);
  }
        
  for (j = 0; j < 100; j++) otherdata[j] = (char *)NULL;
  for (j = 0; j < N_BASIC_FIELDS; j++) basicdata[j] = (char *)NULL;
  cathelpfile(ADDINFO,(char *)NULL,1);
  
  /* 'k' and 'kludge' are are kludge to allow us to back up from entering */
  /* user defined fields to go an correct wrong basic field information. */
  
  k = 0;
  
  kludge :
  
  for (j = k; j < N_BASIC_FIELDS - 1; j++) {
        
      redo :
        
      rval = menu_match (
           &menuval,&response,
           Field_Names[j],
           0,0,0,1,5,
           "\\",A_ABORT_ADD,
           "^",A_BACKUP,
           "!",A_FILL_IN_REST,
           "?",A_HELP,
           "",A_NO_DATA
        );
        
      switch (rval) {
        
        case MENU_NO_MATCH :
          basicdata[j] = copystr(response);
          if (j == 0 && strlen(basicdata[j]) == 0) {
             printf("Each entry must have a name!\n\n");
	     sleep(2);
             goto redo;
          }
          break;

        case MENU_MATCH :
          switch (menuval) {
            case A_BACKUP :
              if (j == 0) return(0);
              j--;
              goto redo;
              /* break; */
            case A_ABORT_ADD :
              return(0);
              /* break; */
            case A_FILL_IN_REST :
              if (j == 0) {
                 fprintf(stderr,"You must enter at least a name...\n\n");
		 sleep(2);
                 goto redo;
              }
              goto add_entry;
              /* break; */
            case A_HELP :
              cathelpfile(ADDHELP,"adding entries",1);
              any_char_to_continue();
              clear_the_screen();
              cathelpfile(ADDINFO,(char *)NULL,0);
              for (i = 0; i < j; i++) {
                  printf("%s%s\n",Field_Names[i],basicdata[i]);
              }
              goto redo;                            
              /* break; */
            case A_NO_DATA :
              if (j == 0) {
                 fprintf(stderr,"You must enter at least a name...\n\n");
		 sleep(2);
                 goto redo;
              }

              if (basicdata[j] != (char *)NULL)
			basicdata[j][0] = '\0';
	      else
			basicdata[j] = "";
              break;
            default :
              fprintf(stderr,"Impossible rval from rolo_add menu_match\n\n");
              save_and_exit(-1);
              break;
          }
          break;

        case MENU_EOF :
          user_eof();
          break;
          
        case MENU_ERROR :
        case MENU_AMBIGUOUS :
        default :
          fprintf(stderr,"Impossible return from rolo_add menu_match\n");
          save_and_exit(-1);
          break;

      }

  }

  if (other_fields()) {
     for (j = 0; j < 100; j++) {
         redo_other :
         rval = menu_match (
              &menuval,&response,
              "Enter <name>: <data> (type RETURN to quit) : ",
              0,0,0,0,5,
              "\\",O_ABORT,
              "?",O_HELP,
              "Help",O_HELP,
              "^",O_BACKUP,
              "",O_DONE_OTHERS
           );
         switch (rval) {
           case MENU_MATCH :
             switch (menuval) {
               case O_DONE_OTHERS :
                 goto add_entry;
                 /* break; */
               case O_BACKUP :
                 if (j == 0) {
                    k = N_BASIC_FIELDS - 2;
                    goto kludge;
                 }
                 else {
                    j--;
                    printf("Deleting %s\n",otherdata[j]);
                    goto redo_other;
                 }
                 /* break; */
               case O_ABORT :
                 return(0);
                 /* break; */
               case O_HELP :
                 cathelpfile(OTHERFORMATHELP,"user-item format",1);
                 any_char_to_continue();
                 goto redo_other;
              }   
             break;
           case MENU_NO_MATCH :
             if ((index(response,':') == (char *)NULL) ||
		 (*response == ':')) {
                printf("No field name provided -- separate with a ':'.\n");
                goto redo_other;
             }
             otherdata[j] = copystr(response);
             break;
           case MENU_EOF :
             user_eof();
             break;
           case MENU_AMBIGUOUS :
           case MENU_ERROR :
           default :
             fprintf(stderr,"Impossible rval from add_other menu_match\n");
             save_and_exit(-1);
         }
     }
  }

  add_entry :   

  basicdata[N_BASIC_FIELDS - 1] = timestring();
  
  rlink = create_entry(basicdata,otherdata);
  clear_the_screen();
  display_entry(get_entry(rlink));
  if (add_the_entry()) {
     printf (
         "Adding entry for %s to rolodex\n",
         get_basic_rolo_field((int) R_NAME,get_entry(rlink))
      );
     rolo_insert(rlink,compare_links);
     changed = 1;
     sleep(2);
  }
  else {
     return(0);
  }
  return(0);	/* Should never hit this... */
}


entry_action (rlink) Ptr_Rolo_List rlink;

{
  static entry_menu_displayed = 0;
  int rval,menuval;
  char *response;
  
  if (!entry_menu_displayed) cathelpfile(ENTRYMENU,(char *)NULL,0);
#ifdef NEVERDEF
  entry_menu_displayed = 1;	/* This may be turned off to provide verbose help */
#endif

  redo :
  
  rval = menu_match (
       &menuval, &response,
       "Action (? for help) : ",
       0,1,1,1,7,
       "\\",E_ABORT,
       "?",E_HELP,
       "",E_CONTINUE,
       "-",E_DELETE,
       "+",E_UPDATE,
       "<",E_PREV,
       "%",E_SCAN
    );
    
  if (rval != MENU_MATCH) {
     if (rval == MENU_EOF) user_eof();
     fprintf(stderr,"Impossible return from entry_action menu_match\n");
     save_and_exit(-1);
  }

  switch (menuval) {
    case E_ABORT :
    case E_CONTINUE :
    case E_PREV :
      break;
    case E_SCAN :
      rolo_peruse_mode(get_next_link(rlink));
      break;
    case E_UPDATE :
      rolo_update_mode(rlink);
      break;
    case E_DELETE :
      if(!rolo_delete(rlink))
      	printf("Entry deleted\n");
      sleep(1);
      changed = 1;
      break;
    case E_HELP :
      cathelpfile (
          (in_search_mode ? ESEARCHHELP : ESCANHELP),
          "entry actions",
          1
       );
      any_char_to_continue();
      clear_the_screen();
      display_entry(get_entry(rlink));
      goto redo;
      /* break; */
    default :
      fprintf(stderr,"Impossible menuval in entry_action\n");
      save_and_exit(-1);
  }

  return(menuval);
  
}


display_list_of_entries (rlist) Ptr_Rolo_List rlist;

{
  Ptr_Rolo_List old,hold;

  if(rlist != 0)
	hold = get_next_link(rlist);	/* In case of change */
        
  while (rlist != 0) {        
    
    if (!get_matched(rlist)) goto next;
        
    loop :    

    display_entry(get_entry(rlist));
    
    switch (entry_action(rlist)) {
      case E_CONTINUE :
        break;
      case E_ABORT :
        return(0);
        /* break; */
      case E_PREV :
        old = rlist;
        find_prev_match :
        if (get_prev_link(rlist) == 0) {
           rlist = old;
           printf("No previous entry in scan list\n");
           sleep(2);
        }
        else {
           rlist = get_prev_link(rlist);
           if (!get_matched(rlist)) goto find_prev_match;
        }
        goto loop;
        /* break; */
      case E_UPDATE :
	if(name_changed)
	{
		printf("Continuing scan at next entry in scan list before change...\n");	
		sleep(1);
	 	rlist = hold;

		break;
	}
	/* Deliberate fall-thru */
      default :
        printf("Displaying next entry in scan list...\n");
        sleep(1);
        break;
    }
    
    next :
    
    if(!name_changed)
	    rlist = get_next_link(rlist);
    else
	name_changed = 0;

    if(rlist != 0)
	hold = get_next_link(rlist);	/* In case of change */
    
  }
  
  printf("No further entries to scan...\n");
  sleep(2);     
  return(0);
}
  

rolo_peruse_mode (first_rlink) Ptr_Rolo_List first_rlink;

{
  Ptr_Rolo_List rlist = first_rlink;
  if (0 == Begin_Rlist) {
     fprintf(stderr,"No further entries in rolodex...\n");
     sleep(2);
     return(0);
  }
  while (rlist != 0) {  
    set_matched(rlist);
    rlist = get_next_link(rlist);
  }    
  display_list_of_entries(first_rlink);
  rlist = first_rlink;
  while (rlist != 0) {  
    unset_matched(rlist);
    rlist = get_next_link(rlist);
  }    

  return(0);
}
