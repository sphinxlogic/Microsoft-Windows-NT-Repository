/* menuaux.c */
#include <stdio.h>
#include <ctype.h>

#ifdef TMC
#include <ctools.h>
#else
#include "ctools.h"
#endif
#include "args.h"
#include "menu.h"

#include "rolofilz.h"
#include "rolodefs.h"
#include "datadef.h"


rolo_menu_yes_no (prompt,rtn_default,help_allowed,helpidx,subject)

  char *prompt;
  int rtn_default;
  int help_allowed;
  int helpidx;
  char *subject;
  
{
  int rval;
  reask :
  rval = menu_yes_no_abort_or_help (
              prompt,ABORTSTRING,help_allowed,rtn_default
           );
  switch (rval) {
    case MENU_EOF :
      user_eof();
      break;
    case MENU_HELP : 
      cathelpfile(helpidx,subject,1);
      goto reask;
      /* break; */
    default :
      return(rval);
      /* break; */
  }

  return(0);
}
  

rolo_menu_data_help_or_abort (prompt,helpidx,subject,ptr_response)

  char *prompt;
  int helpidx;
  char *subject;
  char **ptr_response;
  
{ 
  int rval;
  reask :
  rval = menu_data_help_or_abort(prompt,ABORTSTRING,ptr_response);
  if (rval == MENU_EOF) user_eof();
  if (rval == MENU_HELP) {
     cathelpfile(helpidx,subject,1);
     goto reask;
  }     
  return(rval);
}
     

rolo_menu_number_help_or_abort (prompt,low,high,ptr_ival)

  char *prompt;
  int low,high,*ptr_ival;
  
{  
  int rval;
  if (MENU_EOF == (rval = menu_number_help_or_abort (
                               prompt,ABORTSTRING,low,high,ptr_ival
                           )))
     user_eof();
  return(rval);
}
