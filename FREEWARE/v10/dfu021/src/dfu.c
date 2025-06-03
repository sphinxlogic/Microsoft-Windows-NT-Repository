/******************************************************
*
*	(C)opyright Digital Equipment Corp. 1994,1995
*	FREEWARE CD Version
*
*	Disk and File Utilities for OpenVMS VAX/AXP
*	Version : V2.1
*	Date    : February 1995
*
*******************************************************/

#ifndef VAXC
#pragma message disable(GLOBALEXT)
#endif

#include clidef         
#include climsgdef
#include descrip
#include stdio
#include lib$routines
#include ssdef
#include jpidef
#include iodef

typedef unsigned long Boolean;

globalvalue dfu_tables;
/* Next global params needed for CTRL/C trapping */

int status;
static int oldmask, newmask;
extern unsigned int ctrlc, tchan, disp_id, keyb_id;
extern int astrtn(); /* Ast routine called by CTRLC */

main()

/*	MAIN 

	Purpose : 1 Get privilege mask
		  2 Get and Parse command (syntax only)
		  3 Dispatch to correct subroutine

	Inputs  : Command line (if specified through foreign command)

	Outputs : returns last status code to DCL in  case
		  of single command processing. In interactive mode
		  always returns SS$_NORMAL.
*/
{
   const rms_eof=98938,smg$_eof=1213442;
   struct { short status, count; 
           int extra ;} iosb;
   static char command_line[255];
   unsigned int out_len,ret_len,prvmask;
   void singlemsg(), reset_ctrl(), clean_ctrlc();
   $DESCRIPTOR(input_line , command_line);
   $DESCRIPTOR(prompt,"DFU> ");
   $DESCRIPTOR(terminal,"SYS$COMMAND");

/* Use SMG to set up a virtual keyboard; this enables us to recall 
        commands. */
   SMG$CREATE_VIRTUAL_KEYBOARD(&keyb_id,0,0,0,0);
   printf("\n");
   printf("     Disk and File Utilities for OpenVMS DFU V2.1\n");
   printf("     Copyright © 1994, 1995 Digital Equipment Corporation\n");

   prvmask = 0;
   status = dfu_check_access(&prvmask);  /*Get the privilege mask */
   status=1; ret_len=0; 

/* Check for empty command line; this means an interactive DFU session */
   status = lib$get_foreign(&input_line,0,&ret_len,0); out_len=ret_len;

   if (ret_len==0) 
   { printf("     Type HELP for on-line help\n\n");
     status = SMG$READ_COMPOSED_LINE(&keyb_id,0,&input_line,&prompt,&out_len,
		0,0,0,0,0,0);
   }
    else printf("\n");

/* Setup terminal channel for control purposes */
   SYS$ASSIGN(&terminal, &tchan, 0,0);
   
   for (;;) { /* loop forever until EXIT is entered */
     if(status==smg$_eof) exit(1);
     if ((status&1) != 1) exit(status);
/* Now parse command line. */
     status = CLI$DCL_PARSE(&input_line,dfu_tables,lib$get_input,0,&prompt);
/* Now dispatch if no errors */
     if ((status&1) == 1) 
     { reset_ctrl(0);
       CLI$DISPATCH(prvmask);
       printf("\n");
       clean_ctrlc();
     }
      else
       if (status != CLI$_NOCOMD) printf("\n");
     if (ret_len !=0) 
      { /* Single command processing , so exit here */
       status += 0x10000000; /* Do not echo the error on DCL level */
       exit(status);
      }
     /* Get next line */
     status = SMG$READ_COMPOSED_LINE(&keyb_id,0,&input_line,&prompt,&out_len,
		0,0,0,0,0,0); /*Get next command */
     }
}  /* END of MAIN */
int help_command(int mask)
/*	HELP

	Purpose : call on-line help
	Output  : contents of DFUHLP helplib.
		  call LBR$OUTPUT_HELP to get DCL-style help.
*/
{  void clean_ctrlc();
   unsigned int flag=1,tmp=0;
   int lbr$output_help();
   static char help[80];
   $DESCRIPTOR(help_key ,help);
   $DESCRIPTOR(help_item, "helpkey");
   $DESCRIPTOR(help_lib,"dfuhlp");
   $DESCRIPTOR(item,"DFU ");
   void singlemsg();

   /* Check if a help item was entered */
   if (cli$present(&help_item) == CLI$_PRESENT) 
     cli$get_value(&help_item,&help_key,&help_key);
    else
     help_key.dsc$w_length=0;
 
   /* Add 'DFU ' to the help key */
   help_key.dsc$w_length +=4;
   str$concat(&help_key,&item,&help_key);
   status = lbr$output_help(lib$put_output,0,&help_key,
               &help_lib,&flag,lib$get_input);
   if (status != SS$_NORMAL)
   { 
     printf("%%DFU-E-HELPERR, Error opening help library,\n");
     singlemsg(status);
   }
   return(1);
} /*END help_command */

int exit_command(int mask)
/*  EXIT

	Purpose : Exit Program with SS$_NORMAL
	Output  : Return to DCL

*/
{  exit(1);
   return(1);
} /*END exit_command */

int dfu_check_access(int *mask)
/*
    Check if the user has DFU_ALLPRIV identifier granted.
    
*/

{ int i,j, rights_size, ret_addr, pages;
  unsigned short len;
  int jpi$_rights_size , jpi$_process_rights;
  struct _itm { short buflen, itemcode;
           int *bufadr, *retlen; } item_list[2];
  struct _rt {int rights[2]; } *rights_list;
  static char ascii_id[255]; 
  $DESCRIPTOR(ascii_descr , ascii_id);
  Boolean found;
  void singlemsg();

  jpi$_rights_size = 817; jpi$_process_rights = 814;
  *mask = 0; /*Assume identifier not granted */
  item_list[0].buflen = 4;
  item_list[0].itemcode = jpi$_rights_size;
  item_list[0].bufadr = &rights_size;
  item_list[0].retlen = 0;

  item_list[1].buflen = 0;
  item_list[1].itemcode = 0; /* End of list */
  status = SYS$GETJPIW(0,0,0,&item_list,0,0,0);
  if ((status & 1) !=1) 
  { singlemsg(status);
    return(status);
  }
/* Rightssize is returned in bytes, so rightssize/8 -1 is
   number of rights. Now allocate enough dynamic space to get
   all the rights in. */

  j = rights_size/8 - 1;
/* Add some 10 percent extra space */
  j = (110*j)/100;
  ret_addr = malloc(j*8);
  rights_list = (struct _rt *) ret_addr;

/* Now get the rights list */
  item_list[0].buflen = j*8 ;
  item_list[0].itemcode = jpi$_process_rights;
  item_list[0].bufadr = (int *) rights_list; 
  item_list[0].retlen = 0;
   
  item_list[1].buflen = 0;
  item_list[1].itemcode = 0; /* End of list */

  status = SYS$GETJPIW(0,0,0,&item_list,0,0,0);
  if ((status & 1) !=1)
  { singlemsg(status);
    return(status);
  }
   
  i = 1; found = FALSE; 
  while (!found)
  { status = SYS$IDTOASC(rights_list->rights[0], &len, &ascii_descr, 0,0,0);
    status = strncmp(ascii_id, "DFU_ALLPRIV", len);
    if (status == 0) 
    { *mask = -1;
      found = TRUE;
    }
    else 
    { i++; rights_list++;
      if (i > j) found = TRUE;
    }
  }
  free(ret_addr);
  return(1);
}
  
void singlemsg(int stat)
/* Gets a system message and displays it.
   Unlike LIB$SIGNAL it will not terminate the
   program on fatal errors 
*/
{
   int len;
   static char mesg[80];
   $DESCRIPTOR(mesg_desc , mesg);

   SYS$GETMSG(stat,&mesg_desc.dsc$w_length, &mesg_desc,0,0);
   mesg[mesg_desc.dsc$w_length]='\0';
   printf("%s\n",mesg);
}

void reset_ctrl(int f)
/* Reset CTRLC/Y on the terminal channel */

{
  int stat, func;
  struct { short status, count; 
           int extra ;} iosb;


/* Enable CTRLC/Y trapping */
  if (f == 0)
  { ctrlc = 0;
    newmask = 0x02000000;
    stat = lib$disable_ctrl(&newmask,&oldmask);
  }
  func = IO$_SETMODE | IO$M_CTRLCAST;
  stat = SYS$QIOW(0,tchan,func,&iosb,0,0,astrtn,0,0,0,0,0);
  func = IO$_SETMODE | IO$M_CTRLYAST;
  stat = SYS$QIOW(0,tchan,func,&iosb,0,0,astrtn,0,0,0,0,0);
}

int astrtn()
/* Ast routine called by CTRLC/Y 
   Set CTRLC to 1 and reset the Trapping */

{
  ctrlc = 1;
  reset_ctrl(1);
  return(1);
}

void clean_ctrlc()
/* Cleanup CTRLC channel */

{ SYS$CANCEL(tchan);
  lib$enable_ctrl(&oldmask);
}


int strindex(char *input_string, char *symbol, int len)
/*
   Give the location of the SYMBOL in INPUT_STRING
*/

{ int i ;
  
  i=0;
  while ((i < len) && (input_string[i] != *symbol)) i++;
  return(i < len ) ? i : -1; 
}
