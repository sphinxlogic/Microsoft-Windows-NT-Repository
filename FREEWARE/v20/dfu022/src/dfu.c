/******************************************************
*
*	(C)opyright Digital Equipment Corp. 1995
*	FREEWARE VERSION
*
*	Version : V2.2
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
#include smgdef
#include signal
#include tt2def
#include iodef
typedef unsigned long Boolean;

globalvalue dfu_tables;
/* Next global params needed for CTRL/C trapping */

int key_tab, status, cip, orig_colls;
static int orgttchar[3], ttchar[3];

extern char outbuf[255];
extern unsigned int ctrlc, tchan, disp_id, disp1_id, rows, colls,
                    disp2_id, status_id, paste_id, help_id, keyb_id;
extern Boolean smg$enable;

static FILE *xp;
int astrtn(), refresh() ; /* Ast routines called by CTRLC,Y and W */
unsigned int prim_hand(), brdcst_ast();
void singlemsg(),put_disp(), put_status();

main()

/*	MAIN 

	Purpose : 1 Get privilege mask
		  2 Setup SMG environment unless no VT or DFU$NOSMG is set
		  3 Get and Parse command (syntax only)
		  4 Dispatch to correct subroutine

	Inputs  : Command line (if specified through foreign command)

	Outputs : returns last status code to DCL in  case
		  of single command processing. In interactive mode
		  always returns SS$_NORMAL.
*/
{
   const rms_eof=98938,smg$_eof=1213442;
   struct { short status, count; 
           int extra ;} iosb;
   static char command_line[255], *e;
   unsigned int out_len,ret_len,prvmask;
   void reset_ctrl(), clean_ctrlc(), 
    prev_screen(), next_screen(), dump_screen(), toggle_width() ;
   int smg_flag, x, y, i, ttype;
   int cursor_on = SMG$M_CURSOR_ON;
   $DESCRIPTOR(input_line , command_line);
   $DESCRIPTOR(prompt,"DFU> ");
   $DESCRIPTOR(terminal,"SYS$COMMAND");
   $DESCRIPTOR(top_txt,"< DFU V2.2 (Freeware) >");
   $DESCRIPTOR(status_txt,"Statistics");
   $DESCRIPTOR(do_key,"DO");
   $DESCRIPTOR(pf2,"PF2");
   $DESCRIPTOR(pf4,"PF4");
   $DESCRIPTOR(prev,"PREV_SCREEN");
   $DESCRIPTOR(next,"NEXT_SCREEN");
   $DESCRIPTOR(select,"SELECT");
   $DESCRIPTOR(help,"HELP");

/* First find out how we got called ( by RUN, or a foreign command */
   ret_len = 0;
   status = lib$get_foreign(&input_line,0,&ret_len,0); 
   out_len = ret_len; 

   smg$enable = TRUE;
   key_tab = 0; disp2_id = 0; cip = 0;

/* Now create the SMG environment */
 
   colls=80;rows=24;
   SMG$CREATE_PASTEBOARD(&paste_id, 0, &rows, 
     &colls,&SMG$M_KEEP_CONTENTS,&ttype,0);
   if ((e = (char *) getenv("DFU$NOSMG")) && *e) smg$enable = FALSE;
    else
   { 
     if (ttype != SMG$K_VTTERMTABLE) smg$enable = FALSE;
     if (ttype != SMG$K_VTTERMTABLE) SMG$DELETE_PASTEBOARD(&paste_id,&0);
   }

   SMG$CREATE_VIRTUAL_KEYBOARD(&keyb_id,0,0,0,0);
   if (smg$enable)
/* Setup key table */
   { SMG$ERASE_PASTEBOARD(&paste_id);
     SMG$CREATE_KEY_TABLE(&key_tab); 
     colls -=2; orig_colls = colls;
     smg_flag = SMG$M_KEY_NOECHO + SMG$M_KEY_TERMINATE;
     SMG$ADD_KEY_DEF(&key_tab,&do_key,0, &smg_flag, &do_key,0);
     SMG$ADD_KEY_DEF(&key_tab,&pf4,0, &smg_flag,&pf4,0);
     SMG$ADD_KEY_DEF(&key_tab,&prev,0, &smg_flag, &prev,0);
     SMG$ADD_KEY_DEF(&key_tab,&next,0, &smg_flag, &next,0);
     SMG$ADD_KEY_DEF(&key_tab,&pf2,0, &smg_flag, &help,0); 
     SMG$ADD_KEY_DEF(&key_tab,&help,0, &smg_flag, &help,0); 
     SMG$ADD_KEY_DEF(&key_tab,&select,0, &smg_flag, &select,0); 
     SMG$CREATE_VIRTUAL_DISPLAY(&500, &colls , &disp1_id, &SMG$M_BORDER, 
        0, 0);
     x = 508 - rows; y = rows - 7;
     SMG$CREATE_VIEWPORT(&disp1_id,&x,&1,&y,&colls); 
     SMG$CREATE_VIRTUAL_DISPLAY(&2, &colls, &status_id, 0 , 0, 0);
     SMG$CREATE_VIRTUAL_DISPLAY(&2, &colls, &disp2_id, 0 , 0, 0);
     SMG$SET_BROADCAST_TRAPPING(&paste_id,brdcst_ast,0);
     SMG$LABEL_BORDER(&disp1_id, &top_txt, 0, 0,&SMG$M_BOLD);
     SMG$LABEL_BORDER(&status_id, &status_txt, 0, 0,&SMG$M_BOLD);
     SMG$PASTE_VIRTUAL_DISPLAY(&disp1_id, &paste_id, &2,&2,0);
     x = rows - 4;
     SMG$PASTE_VIRTUAL_DISPLAY(&status_id, &paste_id, &x,&2,0);
     x = rows - 1;
     SMG$PASTE_VIRTUAL_DISPLAY(&disp2_id, &paste_id, &x,&2,0);
     x = 508 - rows;
     SMG$SET_CURSOR_ABS(&disp1_id,&x,&1);  
     SMG$SET_CURSOR_ABS(&disp2_id,&1,&1);  
     SMG$BEGIN_PASTEBOARD_UPDATE(&paste_id);
   }

   sprintf(outbuf,"\n     Disk and File Utilities for OpenVMS DFU V2.2");
   put_disp();
   sprintf(outbuf,"     Freeware version");
   put_disp();
   sprintf(outbuf,"     Copyright � 1995 Digital Equipment Corporation\n");
   put_disp();

   if (smg$enable)
   { /* Enter additional info */
     sprintf(outbuf,"     DFU functions are : \n"); put_disp();
     sprintf(outbuf,"     DEFRAGMENT : Defragment files"); 
     put_disp();
     sprintf(outbuf,"     DELETE     : Delete files by File-ID; delete directory (trees)"); 
     put_disp();
     sprintf(outbuf,"     DIRECTORY  : Manipulate directories"); 
     put_disp();
     sprintf(outbuf,"     REPORT     : Generate a complete disk report"); 
     put_disp();
     sprintf(outbuf,"     SEARCH     : Fast file search"); 
     put_disp();
     sprintf(outbuf,"     SET        : Modify file attributes"); 
     put_disp();
     sprintf(outbuf,"     UNDELETE   : Recover deleted files"); 
     put_disp();
     sprintf(outbuf,"     VERIFY     : Check and repair disk structure"); 
     put_disp();
     SMG$END_PASTEBOARD_UPDATE(&paste_id);
   }  

   prvmask = 0;
   status = dfu_check_access(&prvmask);  /*Get the privilege mask */

/* Setup terminal channel for control purposes; get the terminal chars */
   status = SYS$ASSIGN(&terminal, &tchan, 0,0);
   status = SYS$QIOW(0,tchan, IO$_SENSEMODE,0,0,0,&orgttchar,12,0,0,0,0);
   for (i = 0; i < 3; i++) ttchar[i] = orgttchar[i];
   ttchar[2] &= ~TT2$M_EDITING; /* Clear line edit bit */
   clean_ctrlc(); /* Enable CTRL/W if needed */

   if (ret_len==0) 
   { if (smg$enable) 
       status = SMG$READ_COMPOSED_LINE(&keyb_id,&key_tab,&input_line,&prompt,
         &out_len,&disp2_id,0,0,0,0,0);
      else 
       status = SMG$READ_COMPOSED_LINE(&keyb_id,0,&input_line,&prompt,
         &out_len,0,0,0,0,0,0);
   }

   cip = 1;

/* Main loop starts here. Get a command and pasre it*/
   for (;;)
   { /* loop forever until EXIT is entered */
     if(status==smg$_eof) status = exit_command(prvmask);
     if ((status&1) != 1) goto endfor;
     if (out_len == 0) goto endfor;

/* First catch special screen commands */
     if (smg$enable)
     { status = strncmp(command_line, "PREV_SCREEN", 11);
       if (status == 0) 
       { prev_screen();
         goto endfor;
       }     
       status = strncmp(command_line, "DO",2);
       if (status == 0) 
       { status = spawn_command(prvmask);
         goto endfor;
       }     
       status = strncmp(command_line, "PF4",3);
       if (status == 0) 
       { dump_screen();
         goto endfor;
       }     
       status = strncmp(command_line, "NEXT_SCREEN", 11);
       if (status == 0) 
       { next_screen();
         goto endfor;
       }     
       status = strncmp(command_line, "SELECT", 6);
       if (status == 0)
       { toggle_width();
         goto endfor;
       }

       SMG$ERASE_DISPLAY(&disp1_id);
       SMG$ERASE_DISPLAY(&status_id);
       SMG$CHANGE_VIEWPORT(&disp1_id,&x,&1,&y,&colls);
       SMG$SET_CURSOR_ABS(&disp1_id,&x,&1);  
     }

/* Catch the CLI errors do avoid disrupting the SMG screen... */
     VAXC$ESTABLISH(prim_hand);
     status = CLI$DCL_PARSE(&input_line,dfu_tables,lib$get_input,0,&prompt);
     VAXC$ESTABLISH(NULL);
     if (status == CLI$_NOCOMD) singlemsg(0,status);
     if ((status & 1 ) != 1) goto endfor;
      else
/* Now dispatch if no errors */
     { reset_ctrl();
       CLI$DISPATCH(prvmask);
       clean_ctrlc();
       cip = 0;
       status = brdcst_ast();
       if (smg$enable) SMG$SET_CURSOR_MODE(&paste_id, &cursor_on);
     }
endfor:
     if (ret_len !=0) 
     { /* Single command processing , so exit here */
       status += 0x10000000; /* Do not echo the error on DCL level */
       if (smg$enable) 
       { if (colls != orig_colls) toggle_width();
         SMG$SET_CURSOR_ABS(&disp2_id,&2,&1);  
       }
       exit(status);
     }
     /* Get next line */
     cip = 0;
     if (smg$enable)
     { SMG$ERASE_LINE(&disp2_id, &1, &1);
       SMG$SET_CURSOR_ABS(&disp2_id,&1,&1);  
       status = SMG$READ_COMPOSED_LINE(&keyb_id,&key_tab,&input_line,
                &prompt,&out_len,&disp2_id,0,0,0,0,0); /*Get next command */
       cip = 1;
     }
      else
       status = SMG$READ_COMPOSED_LINE(&keyb_id,0,&input_line,
                &prompt,&out_len,0,0,0,0,0,0); /*Get next command */
   }
}  /* END of MAIN */
int help_command(int mask)
/*	HELP

	Purpose : call on-line help
	Output  : contents of DFUHLP helplib.
*/
{  unsigned int x,flag=1,tmp=0;
   static char help[80];
   $DESCRIPTOR(help_key ,help);
   $DESCRIPTOR(help_item, "helpkey");
   $DESCRIPTOR(help_lib,"dfuhlp");
   $DESCRIPTOR(item,"DFU ");
   $DESCRIPTOR(hlp_txt,"< DFU Help Screen >");

   /* Check if a help item was entered */
   if (cli$present(&help_item) == CLI$_PRESENT) 
     cli$get_value(&help_item,&help_key,&help_key);
    else
     help_key.dsc$w_length=0;
 
   /* Add 'DFU ' to the help key */
   help_key.dsc$w_length +=4;
   str$concat(&help_key,&item,&help_key);
   x = rows - 4;
/* Reenable line editing... */
   status = SYS$QIOW(0,tchan, IO$_SETMODE,0,0,0,&orgttchar,12,0,0,0,0);
   if (smg$enable)
   { SMG$CREATE_VIRTUAL_DISPLAY(&x, &colls, &help_id, 0 , 0, 0);
     SMG$LABEL_BORDER(&help_id, &hlp_txt, 0, 0,&SMG$M_BOLD);
     SMG$PASTE_VIRTUAL_DISPLAY(&help_id, &paste_id, &2,&2,0);
     status = SMG$PUT_HELP_TEXT(&help_id, &keyb_id, &help_key, &help_lib,0,0);
     SMG$UNPASTE_VIRTUAL_DISPLAY(&help_id, &paste_id);
     SMG$DELETE_VIRTUAL_DISPLAY(&help_id);
   }
    else
     status = lbr$output_help(lib$put_output,0,&help_key,
               &help_lib,&flag,lib$get_input);
   if (status != SS$_NORMAL)
   { 
     sprintf(outbuf,"%%DFU-E-HELPERR, Error opening help library,");
     put_disp(); singlemsg(0,status);
   }
   return(1);
} /*END help_command */

int exit_command(int mask)
/*  EXIT

	Purpose : Exit Program with SS$_NORMAL
	Output  : Return to DCL

*/
{  void clean_ctrlc(), toggle_width();

   clean_ctrlc(); 
   if (smg$enable)
   { SMG$DISABLE_BROADCAST_TRAPPING(&paste_id);
     if (colls != orig_colls) toggle_width();
     SMG$SET_CURSOR_ABS(&disp2_id,&2,&1);  
     SMG$DELETE_PASTEBOARD(&paste_id, &0);
   }
   exit(1);
   return(1);
} /*END exit_command */

int spawn_command(int mask)
{
  /* Saves the screen and spawns a subprocess */

  int save_id;
  $DESCRIPTOR(prompt,"DFU_sub$ ");

  if (smg$enable)
  { SMG$SAVE_PHYSICAL_SCREEN(&paste_id, &save_id);
    SMG$DISABLE_BROADCAST_TRAPPING(&paste_id);
  }
  printf("%%DFU-I-SPAWN, Creating subprocess...\n");
  status = lib$spawn(0,0,0,0,0,0,0,0,0,0,&prompt,0,0);
/* Next call is a workaround for a bug in TTdriver which may inadvertently
   reset the terminal width */
  if (colls > 80) printf("\x1b[?3h");      
	else printf("\x1b[?3l");
  if (smg$enable)
  { SMG$RESTORE_PHYSICAL_SCREEN(&paste_id, &save_id);
    SMG$SET_BROADCAST_TRAPPING(&paste_id,brdcst_ast,0);
  }
  if ((status & 1) !=1) singlemsg(0,status);
  return (1);
}

void prev_screen(void)
{
  int row_start, cnt;

  SMG$GET_VIEWPORT_CHAR(&disp1_id, &row_start);
  cnt = row_start - 1;
  if (cnt <= 1)
     SMG$RING_BELL(&disp1_id);
   else
    if (cnt < (rows - 10))
    { SMG$SCROLL_VIEWPORT(&disp1_id, &SMG$M_DOWN, &cnt);
    }
     else
    { cnt = (rows - 10);
      SMG$SCROLL_VIEWPORT(&disp1_id, &SMG$M_DOWN, &cnt);
    }
}

void next_screen(void)
{
  int row_start, cnt;

  SMG$GET_VIEWPORT_CHAR(&disp1_id, &row_start);
  cnt = 508 - rows - row_start;
  if (cnt <= 1)
     SMG$RING_BELL(&disp1_id);
   else
    if (cnt < (rows - 10))
    { SMG$SCROLL_VIEWPORT(&disp1_id, &SMG$M_UP, &cnt);
    }
     else
    { cnt = (rows - 10);
      SMG$SCROLL_VIEWPORT(&disp1_id, &SMG$M_UP, &cnt);
    }
}

int dump_actn(struct dsc$descriptor *out_str, int user_arg)
{  
  fprintf(xp,"%.*s\n",out_str->dsc$w_length, out_str->dsc$a_pointer);
  return(1);
}

void dump_screen(void)
/* Routine to dump the screen contents to a file */
{ char fname[]="DFU_SCREEN.TXT";

  xp = fopen(fname,"w","mrs=132","rfm=var","ctx=rec","rat=cr","rop=WBH");
  status = SMG$PUT_PASTEBOARD(&paste_id, dump_actn, 0 , 0);
  if ((status & 1) !=1)
  { sprintf(outbuf,"%%DFU-E-DUMPERR, Error dumping screen contents,");
    put_disp(); singlemsg(0, status);
  }
   else
  { sprintf(outbuf,"%%DFU-I-DUMPDONE, Screen contents saved in DFU_SCREEN.TXT");
    put_disp(); 
  }
  fclose(xp);
}

void toggle_width(void)
/* Routine to change terminal width */
{
  if (colls > 80)
    SMG$CHANGE_PBD_CHARACTERISTICS(&paste_id, &80, &colls);
   else           
    SMG$CHANGE_PBD_CHARACTERISTICS(&paste_id, &132, &colls);
  colls -=2;
  SMG$CHANGE_VIRTUAL_DISPLAY(&disp1_id,0,&colls);
  SMG$CHANGE_VIEWPORT(&disp1_id,0,0,0,&colls);
  SMG$CHANGE_VIRTUAL_DISPLAY(&status_id,0,&colls);
  SMG$CHANGE_VIRTUAL_DISPLAY(&disp2_id,0,&colls);
}
  
int dfu_check_access(int *mask)
/*
    Check if the user has DFU_ALLPRIV identifier granted.
*/

{ int i,j, rights_size, ret_addr, pages;
  unsigned short len;
  int jpi$_rights_size , jpi$_process_rights;
  struct _itm { short buflen, itemcode;
           int *bufadr, *retlen; } item_list[3];
  struct _rt {int rights[2]; } *rights_list;
  static char ascii_id[255]; 
  $DESCRIPTOR(ascii_descr , ascii_id);
  Boolean found;

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
  { singlemsg(0,status);
    return(status);
  }
/* Rightssize is returned in bytes, so rightssize/8 -1 is
   number of rights. Now allocate enough dynamic space to get
   all the rights in. */

  j = (rights_size/8) + 1;
  ret_addr = malloc(j*8);
  rights_list = (struct _rt *) ret_addr;
   
  item_list[0].buflen = j*8 ;
  item_list[0].itemcode = jpi$_process_rights;
  item_list[0].bufadr = (int *) rights_list; 
  item_list[0].retlen = 0;
   
  item_list[1].buflen = 0;
  item_list[1].itemcode = 0; /* End of list */

  status = SYS$GETJPIW(0,0,0,&item_list,0,0,0);
  if ((status & 1) !=1)
  { singlemsg(0,status);
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
  
void reset_ctrl(void)
/* Reset CTRLC/Y on the terminal channel */

{
  int stat, func;
  struct { short status, count; 
           int extra ;} iosb;
  unsigned int t_mask[2];

/* Disable the F6 key */
  status = SYS$QIOW(0,tchan, IO$_SETMODE,0,0,0,&ttchar,12,0,0,0,0);
  ctrlc = 0;
/* Enable CTRLC/Y trapping by setting CTRL C and Y as out-of-band chars*/
  t_mask[0] = 0; t_mask[1] = 1 << 3;
  t_mask[1] += (1 << 25); 
  status = SYS$QIOW(0,tchan,(IO$_SETMODE+IO$M_OUTBAND),&iosb,
    0,0,astrtn,&t_mask,0,0,0,0); 
}

int astrtn()
/* Ast routine called by CTRLC/Y 
   Set CTRLC to 1 and reset the Trapping */

{ 
  ctrlc = 1;
  sprintf(outbuf,"     Current command ABORTING...");
  put_status(2);
  return(1);
}

void clean_ctrlc()
/* Cleanup CTRLC channel */

{ struct { short status, count; 
           int extra ;} iosb;
  unsigned int t_mask[2];

  status = SYS$QIOW(0,tchan, IO$_SETMODE,0,0,0,&orgttchar,12,0,0,0,0); 
  if (!smg$enable)
    status = SYS$QIOW(0,tchan,(IO$_SETMODE+IO$M_OUTBAND),&iosb,
      0,0,0,0,0,0,0,0); 
   else
  { t_mask[0] = 0; t_mask[1] = 1 << 23;
    status = SYS$QIOW(0,tchan,(IO$_SETMODE+IO$M_OUTBAND),&iosb,
      0,0,refresh,&t_mask,0,0,0,0); 
  }
  status = 1;
}

int refresh()
{
  status = SMG$CANCEL_INPUT(&keyb_id);
  status = SMG$REPAINT_SCREEN(&paste_id);
  return(1);
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

void put_status(int x)
/* Display statistics info */

{
  $DESCRIPTOR(stat_descr, outbuf);
  if (smg$enable)
  { stat_descr.dsc$w_length = strlen(outbuf);
    SMG$PUT_CHARS(&status_id, &stat_descr, &x, &2, &SMG$M_ERASE_LINE);
  }
}

void put_disp(void)
{
  $DESCRIPTOR(to_disp, outbuf);

  if (smg$enable)
  { to_disp.dsc$w_length = strlen(outbuf);
    SMG$PUT_LINE(&disp1_id, &to_disp,0,0,0,&SMG$M_WRAP_CHAR,0,0);
  }
   else printf("%s\n",outbuf);
}

void singlemsg(int stat1, int stat)
/* Gets a system message and displays it.
   Unlike LIB$SIGNAL it will not terminate the
   program on fatal errors 
*/
{
   int len;                                               
   $DESCRIPTOR(mesg_desc , outbuf);

   if (stat1 !=0)
   { SYS$GETMSG(stat1,&mesg_desc.dsc$w_length, &mesg_desc,0,0);
     outbuf[mesg_desc.dsc$w_length]='\0';
     put_disp(); mesg_desc.dsc$w_length = 255;
   }
   SYS$GETMSG(stat,&mesg_desc.dsc$w_length, &mesg_desc,0,0);
   outbuf[mesg_desc.dsc$w_length]='\0';
   put_disp(); mesg_desc.dsc$w_length = 255;
}

int actrtn(struct dsc$descriptor *out_str, int user_arg)
/* user action routine to catch SYS$PUTMSG */
{ 
  sprintf(outbuf,"%.*s",out_str->dsc$w_length, out_str->dsc$a_pointer);
  put_disp();
  return(0);
}

unsigned int prim_hand(signal, mech)
int signal[], mech[];
{ /* Use the signal array to pass the error to PUTMSG
     PUTMSG in turn will call ACTRTN which will use SMG output
     in stead of just dumping SYS$OUTPUT/ERROR
  */

int params;
signal[0] -= 2;
SYS$PUTMSG(signal, actrtn,0,params);
return SS$_CONTINUE;
}

unsigned int brdcst_ast()
/* Displays the broadcast messages */
{ static char msg[160];
  int len;
  $DESCRIPTOR(brdcst_msg,msg);

  if (cip == 1) return(1); /* exit if DFU is busy */
  status = SMG$CANCEL_INPUT(&keyb_id);
  SMG$SET_CURSOR_ABS(&status_id,&2,&2);  
  status = SMG$GET_BROADCAST_MESSAGE(&paste_id, &brdcst_msg, &len,0);
  while (status == SS$_NORMAL)
  { brdcst_msg.dsc$w_length = len;
    SMG$PUT_LINE(&disp1_id, &brdcst_msg, 0, 0 ,0 ,&SMG$M_WRAP_CHAR, 0, 0);
    brdcst_msg.dsc$w_length = 160;
    status = SMG$GET_BROADCAST_MESSAGE(&paste_id, &brdcst_msg, &len,0);
  }
  return(1);
}
