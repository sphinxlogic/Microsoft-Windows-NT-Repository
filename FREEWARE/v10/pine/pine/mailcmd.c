#if !defined(lint) && !defined(DOS)
static char rcsid[] = "$Id: mailcmd.c,v 4.128 1993/12/06 20:08:15 mikes Exp $";
#endif
/*----------------------------------------------------------------------

            T H E    P I N E    M A I L   S Y S T E M

   Laurence Lundblade and Mike Seibel
   Networks and Distributed Computing
   Computing and Communications
   University of Washington
   Administration Builiding, AG-44
   Seattle, Washington, 98195, USA
   Internet: lgl@CAC.Washington.EDU
             mikes@CAC.Washington.EDU

   Please address all bugs and comments to "pine-bugs@cac.washington.edu"

   Copyright 1989-1993  University of Washington

    Permission to use, copy, modify, and distribute this software and its
   documentation for any purpose and without fee to the University of
   Washington is hereby granted, provided that the above copyright notice
   appears in all copies and that both the above copyright notice and this
   permission notice appear in supporting documentation, and that the name
   of the University of Washington not be used in advertising or publicity
   pertaining to distribution of the software without specific, written
   prior permission.  This software is made available "as is", and
   THE UNIVERSITY OF WASHINGTON DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
   WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION ALL IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND IN
   NO EVENT SHALL THE UNIVERSITY OF WASHINGTON BE LIABLE FOR ANY SPECIAL,
   INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, TORT
   (INCLUDING NEGLIGENCE) OR STRICT LIABILITY, ARISING OUT OF OR IN CONNECTION
   WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  
   Pine and Pico are trademarks of the University of Washington.
   No commercial use of these trademarks may be made without prior
   written permission of the University of Washington.

   Pine is in part based on The Elm Mail System:
    ***********************************************************************
    *  The Elm Mail System  -  Revision: 2.13                             *
    *                                                                     *
    * 			Copyright (c) 1986, 1987 Dave Taylor              *
    * 			Copyright (c) 1988, 1989 USENET Community Trust   *
    ***********************************************************************
 

  ----------------------------------------------------------------------*/

/*======================================================================
     mailcmd.c
     The meat and pototoes of mail processing here:
       - initial command processing and dispatch
       - save message
       - capture address off incoming mail
       - jump to specific numbered message
       - open (broach) a new folder
       - search message headers (where is) command
  ====*/

#include "headers.h"
#ifdef HEBREW
extern char compose_heb;
#endif


#ifdef ANSI
MESSAGECACHE      *closest_not_deleted(MAILSTREAM *, long, long);
int                save(char *, CONTEXT_S *, MESSAGECACHE *, long);
void               select_sort(int);
struct attachment *attachments(int, long);
char              *mcdate2ctime(MESSAGECACHE *);
void               run_viewer(char *, char *);
int                display_attachment(long, struct attachment *);
void               save_attachment(int, long, struct attachment *);
void		   display_text_att(long, struct attachment *);

#else
MESSAGECACHE       *closest_not_deleted();
int                 save();
void                select_sort();
struct attachment *attachments();
void                save_attachment();
int                 display_attachment();
void                run_viewer();
char               *mcdate2ctime();
void		   display_text_att();

#endif


 
/*----------------------------------------------------------------------
         The giant switch on the commands for index and viewing

  Input:  command  -- The command char/code
          in_index -- flag indicating command is from index, rather than view
          orig_command -- The original command typed before pre-processing
  Output: force_mailchk -- Set to tell caller to force call to new_mail().

  Result: Manifold

          Returns 1 if the message number or attachments to shown changed 
 ---*/
int
process_cmd(command, in_index, orig_command, force_mailchk)
        int command, in_index, orig_command;
	int *force_mailchk;
{
    int                mango, question_line, a_changed, is_unread;
    long               current_sorted_msgno, new_sorted_mess_num, del_count,
                       old_sorted_msgno;
    char              *new_folder, sequence[5], prompt[80+MAXFOLDER];
    MESSAGECACHE      *mc;
    ENVELOPE          *e;
    BODY              *b;
    SourceType         src;
    static  int        bump_end_count = 0;
    CONTEXT_S         *tc;
    struct attachment *a;
#ifdef	DOS
    extern long coreleft();
#endif

    dprint(4, (debugfile, "\n - process_cmd((%d)%c) -\n",
                                                 command, (char)command));

    question_line             = -3;
    ps_global->status_changed = 0;
    ps_global->mangled_screen = 0;
    ps_global->mangled_footer = 0;
    ps_global->mangled_header = 0;
    ps_global->next_screen    = SCREEN_FUN_NULL;
    current_sorted_msgno      = ps_global->current_sorted_msgno;
    old_sorted_msgno          = current_sorted_msgno;
    mc                        = NULL;
    a_changed                 = 0;
   *force_mailchk             = 0;

    if(command  !=  KEY_DOWN && command != PF6 && command != ctrl('N') &&
       command  !=  'n' && command != '\t')
      bump_end_count = 0;

    switch (command)
      {
          /*------------- Help --------*/
        case PF1:
        case OPF1:
        case OOPF1:
        case '?':
        case ctrl('G'):
          if(ps_global->nr_mode) {
              q_status_message(1, 3, 5, "No help text currently available");
              break;
          }
	  /*
	   * We're not using the h_mail_view portion of this right now because
	   * that call is being handled in scrolltool() before it gets
	   * here.  Leave it in case we change how it works.
	   */
          helper(in_index ? h_mail_index : h_mail_view,
      in_index ? "HELP FOR FOLDER INDEX VIEW": "HELP FOR MESSAGE VIEW", 0);
          dprint(2, (debugfile,"MAIL_CMD: did help command\n"));
          ps_global->mangled_screen = 1;
          break;


          /*--------- Return to main menu ------------*/
        case PF3: 
        case 'm':
          if(ps_global->nr_mode && command == 'm')
            goto bogus;
          if(ps_global->nr_mode && command == PF3)
	    goto do_quit;
          ps_global->next_screen = main_menu_screen;
#ifdef	DOS
	  flush_index_cache();		/* save room on PC */
#endif
          dprint(2, (debugfile,"MAIL_CMD: going back to main menu"));
          break;


          /*------- View mail or attachment --------*/
        case ctrl('M'):
        case ctrl('J'):
	  if(!in_index) {
	    q_status_message(0, 0, 2,
		    "\007No default command in View Message Screen");
	    break;
	  }
	  /* fall through */
        case PF4:
        case 'v':
	  if(in_index) {
            if(ps_global->max_msgno <= 0) {
              q_status_message(0, 1, 3, "\007No message to view");
            }else {
              ps_global->next_screen = mail_view_screen;
#ifdef	DOS
	      flush_index_cache();		/* save room on PC */
#endif
            }
	  }else {

            /*------- View Attachment -----------*/
            a = attachments(question_line, current_sorted_msgno);
            ps_global->mangled_footer = 1;
            if(a != NULL) {
              char *labels[3];
	      int ret;

              labels[0] = "Save";
              labels[1] = "View";
              labels[2] = NULL;
	      sprintf(prompt, "Save or View attachment %s ? (s/v) [s] ",
		                                                a->number);
              ret = radio_buttons(prompt,
                               question_line, 0, "sv", labels, 's', 'x',
                               NO_HELP, 0);
	      switch(ret) {
		case 's':
                  save_attachment(question_line, 
                                  ps_global->sort[current_sorted_msgno], a); 
		  break;
		case 'v':
                  a_changed = display_attachment(
                                  ps_global->sort[current_sorted_msgno], a);
		  break;
		/* ^C */
		case 'x':
		  q_status_message(0, 0, 2, "\007Attachment cancelled");
		  break;
	      }
            }
	  }
          break;


          /*---------- Previous message ----------*/
        case PF5: 
        case KEY_UP:
        case 'p':
        case ctrl('P'):		    
          if(ps_global->max_msgno == 0) {
              q_status_message(0, 1, 3, "\007No message in folder");
          }else  if(current_sorted_msgno > 1 ){
              current_sorted_msgno--;
              ps_global->current_sorted_msgno = current_sorted_msgno;
          }else {
              q_status_message(0, 0, 1, "Already on first message");
          }
          break;


          /*---------- Next Message ----------*/
        case PF6:
        case KEY_DOWN:
        case 'n':
        case ctrl('N'):
	  if(ps_global->max_msgno > 0
	     && current_sorted_msgno < ps_global->max_msgno) {
              current_sorted_msgno++;
              ps_global->current_sorted_msgno = current_sorted_msgno;
          }else{
	      sprintf(prompt, "\007No m%s",
		      (ps_global->max_msgno > 0) ? "ore messages" 
						 : "essages in folder");

	      if(!ps_global->nr_mode &&
		 (IS_NEWS || (ps_global->context_current->use&CNTXT_INCMNG))){
		  char nextfolder[MAXPATH];

		  strcpy(nextfolder, ps_global->cur_folder);
		  if(next_folder(nextfolder, nextfolder,
				 ps_global->context_current))
		    strcat(prompt, ".  Press TAB for next folder.");
		  else
		    strcat(prompt, ".  No more folders to TAB to.");
	      }

	      q_status_message(0, 0, 1, prompt);

	      if(!IS_NEWS && bump_end_count++ > 3) {
		  *force_mailchk = 1;
		  bump_end_count = 0;
	      }
	  }
          break;


          /*---------- Delete message ----------*/
        case PF9: 
        case 'd':
        case KEY_DEL: 
          if(ps_global->nr_mode) {
	    if(command == PF9)
	      goto do_forward;
	    else
              goto bogus;
	  }
          dprint(4, (debugfile, "\n - delete message -\n"));
          if(ps_global->max_msgno <= 0L) {
              q_status_message(0, 1, 3,"\007No message in folder");
              break;
          }

          if(READONLY_FOLDER){
              q_status_message(1, 1, 3,
                               "Can't delete message. Folder is read-only");
              break;
          }

          if(ps_global->io_error_on_stream) {
              ps_global->io_error_on_stream = 0;
              mail_check(ps_global->mail_stream); /* forces write */
          }

          e = mail_fetchstructure(ps_global->mail_stream,
                                 ps_global->sort[current_sorted_msgno],
				 NULL);
          mc = mail_elt(ps_global->mail_stream,
                        ps_global->sort[current_sorted_msgno]);
          if(e == NULL || mc == NULL || ps_global->dead_stream) {
              q_status_message(1, 1, 3,
                               "Can't delete message. Error accessing folder");
              break;
          }
          dprint(3,(debugfile, "Deleted: %s\n",
                    e != NULL && e->message_id != NULL ? e->message_id : ""));

	  if(current_sorted_msgno >= ps_global->max_msgno)
	    strcpy(prompt, "Last message ");
	  else
	    prompt[0] = '\0';

	  if(!mc->deleted) {
              sprintf(sequence, "%ld", ps_global->sort[current_sorted_msgno]);
              mail_setflag(ps_global->mail_stream, sequence, "\\DELETED");
              update_titlebar_status(mc);
              ps_global->status_changed = 1;
              clear_index_cache_ent(current_sorted_msgno);
              check_point_change();
	  }
	  else{
	      if(prompt[0] == '\0')
		sprintf(prompt, "Message %ld ",current_sorted_msgno);

	      strcat(prompt, "already ");
	  }

	  is_unread = 1;
	  if(F_ON(F_DEL_SKIPS_DEL,ps_global)
	     || (IS_NEWS || (ps_global->context_current->use & CNTXT_INCMNG)))
	    new_sorted_mess_num = next_sorted_un(ps_global->mail_stream,
						 current_sorted_msgno + 1,
						 &is_unread, 1);

	  if(F_OFF(F_DEL_SKIPS_DEL,ps_global) 
	     && current_sorted_msgno < ps_global->max_msgno){
	      current_sorted_msgno++;
	      ps_global->current_sorted_msgno = current_sorted_msgno;
	  }
	  else if(F_ON(F_DEL_SKIPS_DEL,ps_global) && is_unread){
	      /* more interesting mail, goto next msg */
	      ps_global->current_sorted_msgno = new_sorted_mess_num;
	      current_sorted_msgno            = new_sorted_mess_num;
	  }

	  if(prompt[0])
	    strcat(prompt, "deleted");

	  if(!ps_global->nr_mode 
	     && (IS_NEWS || (ps_global->context_current->use & CNTXT_INCMNG))
	     && !is_unread){
	      char nextfolder[MAXPATH];

	      if(prompt[0] == '\0')
		strcat(prompt, "Last undeleted message");

	      strcpy(nextfolder, ps_global->cur_folder);
	      if(next_folder(nextfolder,nextfolder,ps_global->context_current))
		strcat(prompt, ".  Press TAB for next folder.");
	      else
		strcat(prompt, ".  No more folders to TAB to.");
	  }

	  if(prompt[0])
	    q_status_message(0,1,3, prompt);

          break;
          

          /*---------- Undelete message ----------*/
        case PF10:
        case 'u':
          if(ps_global->nr_mode) {
	    if(command == PF10)
	      goto do_jump;
	    else
              goto bogus;
	  }
          dprint(4, (debugfile, "\n - undelete -\n"));
          if(ps_global->max_msgno == 0) {
              q_status_message(0, 1, 3, "\007No message in folder");
              break;
          }
          if(READONLY_FOLDER){
              q_status_message(1, 1, 3,
                              "Can't undelete message. Folder is read-only");
              break;
          }
          e  = mail_fetchstructure(ps_global->mail_stream,
                                  ps_global->sort[current_sorted_msgno],
				  NULL); 
          mc = mail_elt(ps_global->mail_stream,
                        ps_global->sort[current_sorted_msgno]);
          if(e == NULL || mc == NULL || ps_global->dead_stream) {
              q_status_message(1, 1, 4,
                      "Can't undelete message. Error accessing folder");
              break;
          }

          dprint(3,(debugfile, "Undeleted: %s\n", e->message_id));
          if (ps_global->max_msgno < 1) {
             q_status_message(0, 0, 2, "No message to undelete");
          }else if(!mc->deleted) {
              q_status_message(0, 1, 2, 
                           "\007Can't undelete a message that isn't deleted");
          }else {
              sprintf(sequence, "%ld", ps_global->sort[current_sorted_msgno]);
              mail_clearflag(ps_global->mail_stream, sequence, "\\DELETED");
              update_titlebar_status(mc);
              ps_global->status_changed = 1;
              clear_index_cache_ent(current_sorted_msgno);
              q_status_message(0, 1, 3,
                           "Deletion mark removed, message won't be deleted");
              if(ps_global->io_error_on_stream) {
                  ps_global->io_error_on_stream = 0;
                  mail_check(ps_global->mail_stream); /* forces write */
              }else {
                  check_point_change();
              }
          }
          break;


          /*---------- Reply to message ----------*/
        case PF11: 
        case 'r':
#ifdef HEBREW
	case ctrl('R'):
#endif
          if(ps_global->anonymous && command == PF11) {
	    if(in_index)
	      goto do_sortindex;
	    else
	      goto do_index;
	  }
          if(ps_global->nr_mode && command == PF11)
	    goto do_print;
          if(ps_global->nr_mode)
            goto bogus;
          if(ps_global->max_msgno == 0) {
              q_status_message(0, 0, 2, "\007No message in folder");
              break;
          }
          e  = mail_fetchstructure(ps_global->mail_stream,
                                  ps_global->sort[current_sorted_msgno],
				  &b);
          mc = mail_elt(ps_global->mail_stream,
                        ps_global->sort[current_sorted_msgno]);
          if(mc == NULL || e == NULL || b == NULL) {
              q_status_message(1, 2, 4,
                  "\007Can't reply to message. Error accessing folder");
              break;
          }
          if(ps_global->max_msgno <= 0) {
              q_status_message(0, 0, 2, "\007No message to reply to");
          }else {
#ifdef HEBREW
	     if(command==ctrl('R'))compose_heb=1;
	     else compose_heb=0;
#endif
#ifdef	DOS
	      flush_index_cache();		/* save room on PC */
#endif
              reply(ps_global, mc->msgno, e, b);
              ps_global->mangled_screen = 1;
          }
          break;


          /*---------- Forward message ----------*/
        case PF12: 
        case 'f':
#ifdef HEBREW
	case ctrl('F'):
#endif
do_forward:
          if(command == PF12) {
	    if(ps_global->anonymous)
              goto bogus;
	    if(ps_global->nr_mode)
	      goto do_save;
	  }
          if(ps_global->max_msgno == 0) {
              q_status_message(0, 0, 2, "\007No message in folder to forward");
              break;
          }
          e  = mail_fetchstructure(ps_global->mail_stream,
                                  ps_global->sort[current_sorted_msgno],
				  &b);
          mc = mail_elt(ps_global->mail_stream,
                        ps_global->sort[current_sorted_msgno]);
          if(mc == NULL || e == NULL || b == NULL) {
              q_status_message(1, 2, 4,
                  "\007Can't forward message. Error accessing folder");
              break;
          }
          if(ps_global->max_msgno <= 0) {
              q_status_message(0, 0, 2,
                               "\007Folder is empty. No message to forward");
          }else {
#ifdef HEBREW
	     if(command==ctrl('F'))compose_heb=1;
	     else compose_heb=0;
#endif
#ifdef	DOS
	      flush_index_cache();		/* save room on PC */
#endif
              forward(ps_global, mc->msgno, e, b);
              if(ps_global->anonymous)
                ps_global->mangled_footer = 1;
              else
                ps_global->mangled_screen = 1;
          }
          break;


          /*---------- Quit pine ------------*/
        case 'q':
        case OPF3:
          if(ps_global->nr_mode && command == OPF3)
            goto do_export;
do_quit:
	  ps_global->next_screen = quit_screen;
	  dprint(1, (debugfile,"MAIL_CMD: quit\n"));		    
          break;


          /*---------- Compose message ----------*/
        case OPF4:		    
        case 'c':
#ifdef HEBREW
	case ctrl('B'):
#endif
          if(ps_global->anonymous)
            goto bogus;
          ps_global->prev_screen = in_index ? mail_index_screen :
                                        mail_view_screen;
#ifdef HEBREW
	  if(command==ctrl('B'))compose_heb=1;
	  else compose_heb=0;
#endif
#ifdef	DOS
	  flush_index_cache();		/* save room on PC */
#endif
          compose_screen(ps_global);
          ps_global->mangled_screen = 1;
          break;


          /*--------- Folders menu ------------*/
	case OPF5: 
        case 'l':
          if(ps_global->anonymous)
	    goto bogus;
          if(ps_global->nr_mode) {
	    if(command == OPF5)
	      goto do_sortindex;
	    goto bogus;
	  }
          ps_global->next_screen = folder_screen;
#ifdef	DOS
	  flush_index_cache();		/* save room on PC */
#endif
          dprint(2, (debugfile,"MAIL_CMD: going to folder/collection menu"));
          break;


          /*---------- Open specific new folder ----------*/
        case OPF6:
        case 'g':
          if(ps_global->nr_mode)
            goto bogus;

	  tc = (ps_global->context_last
		      && !(ps_global->context_current->type & FTYPE_BBOARD)) 
                       ? ps_global->context_last : ps_global->context_current;

          new_folder = broach_folder(question_line, 1, &tc);
#ifdef	DOS
	  if(new_folder && *new_folder == '{' && coreleft() < 20000){
	      q_status_message(0, 0, 2,
			       "\007Not enough memory to open IMAP folder");
	      new_folder = NULL;
	  }
#endif
          if(new_folder != NULL) {
              dprint(9, (debugfile, "do_broach_folder (%s, %s)\n",
                         new_folder, tc->context));
              if(do_broach_folder(new_folder, tc) > 0)
                  /* so we go out and come back into the index */
                  ps_global->next_screen = mail_index_screen;
          }
          break;
    	  
    	    
          /*------- Go to index (or zoom) ----------*/
        case OPF7:
        case 'i':
do_index:
          if(!in_index) {
#ifdef	DOS
	      flush_index_cache();		/* save room on PC */
#endif
              ps_global->next_screen = mail_index_screen;
          }else {
	      if(command == OPF7)
		  goto do_zoom;
	      else
                  q_status_message(0, 1, 3, "\007Already in Index");
          }
          break;


          /*------- Zoom -----------*/
do_zoom:
        case 'z':
	  if(F_OFF(F_ENABLE_ZOOM,ps_global))
	    goto bogus;

          q_status_message(1, 2, 3, "\007Zoom command not implemented yet");
          break;


          /*---------- Search (where is command) AND Jump To ----------*/
       case '0':
       case '1':
       case '2':
       case '3':
       case '4':
       case '5':
       case '6':
       case '7':
       case '8':
       case '9':
          if(F_OFF(F_ENABLE_JUMP,ps_global))
	      goto bogus;

       case OPF8:
       case 'w':		/* Note, whereis only effective in Index */
       case OOPF7:
       case 'j':
do_jump:
          if(ps_global->max_msgno <= 0L) {
              q_status_message(0, 0, 2,
			       (command == 'j' || command == OOPF7 
				|| (command >= '0' && command <= '9'))
			       ? "\007No messages to jump to" 
			       : "\007No messages to search");
              break;
          }

          new_sorted_mess_num = (command == 'j' || command == OOPF7 
	    || command == PF10 || (command >= '0' && command <= '9'))
	      ? jump_to(question_line,
			(command >= '0' && command <= '9') ? command : '\0')
	      : search_headers(question_line, current_sorted_msgno,
			     ps_global->max_msgno);
          ps_global->mangled_footer = 1;
          if(new_sorted_mess_num > 0) {
              ps_global->current_sorted_msgno = new_sorted_mess_num;
              current_sorted_msgno            = new_sorted_mess_num;
          }
          break;


          /*---------- print message on paper ----------*/
        case OPF9:
        case 'y':
do_print:
          if(ps_global->anonymous || (ps_global->nr_mode && command == OPF9))
            goto bogus;
          if(ps_global->max_msgno == 0)
              q_status_message(0, 1, 3, "\007No message in folder");
          else
              print_message(ps_global->current_sorted_msgno);
          break;


          /*---------- Yank Address ----------*/
        case OPF10:
        case 't':
          if(ps_global->nr_mode)
            goto bogus;
          if(ps_global->max_msgno == 0) {
              q_status_message(0, 0, 2, "\007No message in folder");
              break;
          }
          e  = mail_fetchstructure(ps_global->mail_stream,
                                  ps_global->sort[current_sorted_msgno],
				  NULL);
          if(e == NULL) {
              q_status_message(1, 2, 4,
      "\007Can't yank address into address book. Error accessing folder");
              break;
          }
          grab_addr_cmd(e, question_line);
          ps_global->mangled_footer = 1;
          break;


          /*---------- Save Message ----------*/
        case OPF11: 
        case 's':
          if(ps_global->anonymous || (ps_global->nr_mode && command == OPF11))
            goto bogus;
do_save:
          if(ps_global->max_msgno == 0) {
              q_status_message(0, 0, 2, "\007No message in folder");
              break;
          }
          e  = mail_fetchstructure(ps_global->mail_stream,
                                  ps_global->sort[current_sorted_msgno],
				  NULL);
          mc = mail_elt(ps_global->mail_stream,
                        ps_global->sort[current_sorted_msgno]);
          if(mc == NULL || e == NULL) {
              q_status_message(1, 2, 4,
                  "\007Can't save message. Error accessing folder");
              break;
          }
          mango = 0;
          if(!save_msg(question_line, mc, &mango, current_sorted_msgno)) {
              if(mango)
		ps_global->mangled_header = 1;
	      ps_global->mangled_footer = 1;
              break; /* No save done */
          }
          if(mango) 
            ps_global->mangled_screen = 1;
          else 
            ps_global->mangled_footer = 1;
#ifdef X_NEVER  /* Don't set seen flag on a save */
          if(!mc->seen) {
              sprintf(sequence, "%ld", ps_global->sort[current_sorted_msgno]);
              mail_setflag(ps_global->mail_stream,sequence,"\\SEEN");
              ps_global->status_changed = 1;
              clear_index_cache_ent(current_sorted_msgno);
          } 
#endif            
          if(!mc->deleted && !READONLY_FOLDER){
              sprintf(sequence, "%ld", ps_global->sort[current_sorted_msgno]);
              mail_setflag(ps_global->mail_stream, sequence, "\\DELETED");
              update_titlebar_status(mc);
              ps_global->status_changed = 1;
              clear_index_cache_ent(current_sorted_msgno);
	      check_point_change();
          }
          break;


          /*---------- Export message ----------*/
        case OPF12:
        case 'e':
          if(ps_global->anonymous || (ps_global->nr_mode && command == OPF12))
            goto bogus;
do_export:
          if(ps_global->max_msgno == 0) {
              q_status_message(0, 0, 2, "\007No message in folder");
              break;
          }
          export_message(question_line, current_sorted_msgno);
          ps_global->mangled_footer = 1;
          break;


          /*---------- Expunge ----------*/
        case OOPF3:
        case 'x':
          if(ps_global->nr_mode || !in_index)
            goto bogus;
          dprint(2, (debugfile, "\n - expunge -\n"));
	  if(IS_NEWS){
              q_status_message(1, 2, 4,
	                      "eXclude of deleted news not implemented yet.");
              break;
	  } else if(READONLY_FOLDER){
              q_status_message(1, 2, 4, "Can't expunge. Folder is read-only");
              break;
          }

	  if((del_count = count_deleted(ps_global->mail_stream)) == 0){
	     q_status_message(1, 2, 4, 
		"Nothing to Expunge!  No messages marked \"Deleted\".");
	     break;
	  }

	  if(F_OFF(F_AUTO_EXPUNGE,ps_global)) {
            sprintf(prompt, "Expunge %ld message%s from %s", del_count,
                  plural(del_count), pretty_fn(ps_global->cur_folder));
            ps_global->mangled_footer = 1;
	    {int ret;
              ret = want_to(prompt, 'y', 'x', NO_HELP, 0);
	      if(ret == 'n') {
	        break;
	      /* ^C */
	      }else if(ret == 'x') {
	        Writechar('\007', 0);
	        break;
	      }
	    }
	  }
          mc = closest_not_deleted(ps_global->mail_stream,
              		     current_sorted_msgno,
              		     ps_global->max_msgno);
          dprint(8,(debugfile, "Expunge max:%ld cur:l%d kill:%d ncur:%ld\n",
                    ps_global->max_msgno, ps_global->current_sorted_msgno,
                    del_count, mc == NULL ? 0 : mc->msgno));
          StartInverse();
          PutLine0(0,0,"**");/*Show something on the screen to indicate delay*/
	  MoveCursor(ps_global->ttyo->screen_rows -3, 0);
          fflush(stdout);

          mail_expunge(ps_global->mail_stream);
#ifdef X_FINISH
          {
              dprint(1, (debugfile, "**** expunge failed ****\n"));
              q_status_message(1, 3, 5 "\007Expunge failed!");
              break;
          }
#endif
          dprint(2,(debugfile,"expunge complete cur:%ld max:%ld\n",
                    mc==NULL ? 0 : mc->msgno, ps_global->max_msgno));
          /* mm_exists and mm_expunge take care of updating max_msgno*/
          clear_index_cache();
          reset_check_point();
          sort_current_folder();
          if(mc != NULL) {
              long j;
              for(j = 1L; j < ps_global->max_msgno; j++)
                if(ps_global->sort[j] == mc->msgno)
                  break;
              if(j < ps_global->max_msgno)
                ps_global->current_sorted_msgno = j;
              else
                ps_global->current_sorted_msgno = ps_global->max_msgno;
          }else {
             ps_global->current_sorted_msgno = -1;
         }
          current_sorted_msgno         = ps_global->current_sorted_msgno;
          ps_global->mangled_screen    = 1; /* force redraw in index */
          PutLine0(0, 1, "  ");
          EndInverse();
          fflush(stdout);
          if(ps_global->expunge_count > 0) {
              q_status_message3(0, 0, 3,
                        "Expunged %s message%s from folder \"%s\"",
                         long2string(ps_global->expunge_count),
                         plural(ps_global->expunge_count),
			 pretty_fn(ps_global->cur_folder));
              ps_global->expunge_count = 0;
          }else if(F_ON(F_AUTO_EXPUNGE,ps_global)) {
              q_status_message1(0, 0, 3,
                        "No messages to expunge from folder \"%s\"",
			 pretty_fn(ps_global->cur_folder));
	  }
          break;


          /*------- Unexclude -----------*/
	case OOPF4:
        case '&':
	  if(!in_index)
	    goto bogus;
	  else
            q_status_message(1, 2, 3,
		IS_NEWS ? "\007Unexclude command not implemented yet"
			: "\007Unexclude not available for mail folders");
          break;


          /*------- Apply -----------*/
	case OOPF5:
        case 'a':
	  if(!in_index || F_OFF(F_ENABLE_APPLY,ps_global))
	    goto bogus;

	  q_status_message(1, 2, 3,
			   "\007Apply command not implemented yet");
          break;


          /*-------- Sort command -------*/
	case OOPF6:
        case '$':
do_sortindex:
	  if(!in_index)
	      goto bogus;
	  dprint(1, (debugfile,"MAIL_CMD: sort\n"));		    
	  select_sort(question_line);
          /*
	   * Setting mail_box_changed will cause the call to new_mail()
	   * back in mail_index_screen() to call sort_current_folder().
	   */
          ps_global->mail_box_changed = 1;
	  ps_global->mangled_body = 1;
	  ps_global->mangled_footer = 1;
	  clear_index_cache();
          break;


          /*------- Skip to next interesting -----------*/
        case OOPF8:
        case '\t':
          if(ps_global->max_msgno > 0) {
              new_sorted_mess_num = next_sorted_un(ps_global->mail_stream,
                                                   current_sorted_msgno+1,
                                                   &is_unread, IS_NEWS);
              if(is_unread){
                  if(new_sorted_mess_num > 0){
                      ps_global->current_sorted_msgno = new_sorted_mess_num;
                      current_sorted_msgno            = new_sorted_mess_num;
                  }
	      }
	  }

	  /*
	   * If there weren't any unread messages left, OR there
	   * aren't any messages at all, we may want to offer  to
	   * go on to the next folder...
	   */
	  if(!is_unread || ps_global->max_msgno <= 0){
	      char ret = 'n';

	      if(!ps_global->nr_mode
		 && (IS_NEWS || (ps_global->context_current->use&CNTXT_INCMNG))
		 && context_isambig(ps_global->cur_folder)){
		  char nextfolder[MAXPATH];
		  int  another = 0;

		  strcpy(nextfolder, ps_global->cur_folder);
		  while(next_folder(nextfolder, nextfolder,
				    ps_global->context_current)){

		      sprintf(prompt, "%sView next %s \"%s\"",
			      (another++) ? "" : "No more messages.  ",
			      (IS_NEWS) ? "news group" : "folder",
			      nextfolder);

		      if((ret=want_to(prompt,'y','x',NO_HELP,0)) == 'y'){
			  if(do_broach_folder(nextfolder,
					      ps_global->context_current) > 0)
			    ps_global->next_screen = mail_index_screen;
			  break;
			  }

		      if(ret == 'x'){
			  q_status_message(0,0,1,"\007Command cancelled");
			  break;
		      }
		  }

		  if(another == 0 || ret == 'n')
		    q_status_message1(0, 0, 1, "No more %ss",
				      IS_NEWS ? "news group" : "folder");
	      }
	      else{
		  if(ps_global->max_msgno > 0)
		    q_status_message1(0, 0, 1, "No more %s messages",
				    IS_NEWS ? "undeleted" : "new");
		  else
		    q_status_message(0, 1, 3, "\007No message in folder");
	      }
	  }

          break;


          /*------- Toggle Full Headers -----------*/
	case OOPF9: 
        case 'h':
          if(F_OFF(F_ENABLE_FULL_HDR,ps_global))
            goto bogus;
          ps_global->full_header = !ps_global->full_header;
          q_status_message1(0, 0, 2, "Display of full headers is now o%s",
                           ps_global->full_header ? "n" : "ff");
          a_changed = 1;
          break;


          /*------- Bounce -----------*/
	case OOPF10:
        case 'b':
          if(F_OFF(F_ENABLE_BOUNCE,ps_global))
            goto bogus;

          q_status_message(1, 2, 3, "\007Bounce command not implemented yet");
          break;


          /*------- Flag -----------*/
	case OOPF11:
        case '*':
	  if(F_OFF(F_ENABLE_FLAG,ps_global))
	    goto bogus;

	  q_status_message(1, 2, 3, "\007Flag command not implemented yet");
          break;


#ifndef DOS
          /*------- Pipe message -----------*/
	case OOPF12:
        case '|':
	  if(F_ON(F_ENABLE_PIPE,ps_global)) {
            q_status_message(1, 2, 3,
		"\007Piping message into UNIX command not implemented yet");
            break;
	  }
#endif


          /*--------- Default, unknown command ----------*/
        default:
        bogus:
          q_status_message2(0, 0, 2,
	      "\007Command \"%s\" not defined for this screen. Use %s for help",
                          (void *)pretty_command(orig_command),
			  F_ON(F_USE_FK,ps_global) ? "F1" : "?");
          break;
      }

    return(current_sorted_msgno != old_sorted_msgno || a_changed);
}




/*----------------------------------------------------------------------
   Execute save message command: prompt for folder and call function to save

  Args: screen_line    --  Line on the screen to prompt on
        message        --  The MESSAGECACHE entry of message to save
        screen_mangled --  pointer to flag to set if the screen gets mangled

 Result: The folder lister can be called to make selection; mangled screen set
         Returns 0 if unsuccessful
                 1 if saved

   This does the prompting for the folder name to save to, possibly calling 
 up the folder display for selection of folder by user.                 
 ----*/
save_msg(screen_line, message, screen_mangled, index_sorted_msgno)
     int           screen_line, *screen_mangled ;
     long          index_sorted_msgno;
     MESSAGECACHE *message;
{
    char       new_folder[MAXFOLDER+1], prompt[MAXFOLDER+80], *p;
    HelpType   help;
    int        rc, saveable_count = 0;
    static     char folder[MAXFOLDER+1] = {'\0'};
    static     CONTEXT_S *last_context = NULL;
    CONTEXT_S *cntxt = NULL, *tc;
    ESCKEY_S   ekey[5];
    ENVELOPE  *env;

    dprint(4, (debugfile, "\n - saving message -\n"));

    if(message == NULL) {
        q_status_message(0, 0, 2, "\007No Messages to Save");
        return(0);
    }

    /* start with the default save context */
    if((cntxt = default_save_context(ps_global->context_list)) == NULL)
       cntxt = ps_global->context_list;

    if((ps_global->save_by_from || ps_global->save_by_sender ||
	ps_global->save_by_recipient) && 
       (env=mail_fetchstructure(ps_global->mail_stream,message->msgno,NULL))){
	int        i, start, end;
	char      *folder_name;

	if(ps_global->save_by_from)
	  folder_name = env->from ? env->from->mailbox 
				  : env->sender ? env->sender->mailbox : NULL;
	else if(ps_global->save_by_sender)
	  folder_name = env->sender ? env->sender->mailbox
				    : env->from ? env->from->mailbox : NULL;
	else
	  folder_name = env->to ? env->to->mailbox
			    : (ps_global->context_current->type&FTYPE_BBOARD)
	                    ? ps_global->cur_folder : NULL;

	/*
	 * Now fish out the user's name from the mailbox portion of the
	 * address.  Using elm's get_return_name as a template, take 
	 * the left most portion up to the first '%', ':' or '@' and to
	 * right of any '!' (as if c-client would give us such a mailbox).
	 */
	if(folder_name){
	    end = strlen(folder_name) - 1;
	    for(start = end; folder_name[start] != '!' && start > -1; start--)
	      if(strindex("%:@", folder_name[start]))
		end = start - 1;

	    start++;			/* compensate for either case above */
	    for(i = start; i <= end; i++) /* copy name */
	      folder[i - start] = isupper(folder_name[i]) 
		               ? folder_name[i] - 'A' + 'a' : folder_name[i];

	    folder[i - start] = '\0';	/* tie it off */
	}
	else
	  strcpy(folder, DEFAULT_SAVE);
    }
    else if(ps_global->save_by_last_saved){
	if(last_context)
	  cntxt = last_context;
    }
    else
      strcpy(folder, DEFAULT_SAVE);

    /* how many context's can be saved to... */
    for(tc = ps_global->context_list; tc; tc = tc->next)
      if(!(tc->type&FTYPE_BBOARD))
        saveable_count++;

    /* set up extra command option keys */
    rc = 0;
    ekey[rc].ch      = (F_ON(F_USE_FK,ps_global)) ? PF4 + rc : ctrl('T');
    ekey[rc].rval    =  2;
    ekey[rc].name    = "^T";
    ekey[rc++].label = "To Fldrs";

    if(saveable_count > 1){
	ekey[rc].ch      =  (F_ON(F_USE_FK,ps_global)) ? PF4 + rc : ctrl('P');
	ekey[rc].rval    =  10;
	ekey[rc].name    = "^P";
	ekey[rc++].label = "Prev Collection";

	ekey[rc].ch      = (F_ON(F_USE_FK,ps_global)) ? PF4 + rc : ctrl('N');
	ekey[rc].rval    =  11;
	ekey[rc].name    = "^N";
	ekey[rc++].label = "Next Collection";
    }

    if(F_ON(F_ENABLE_TAB_COMPLETE,ps_global)){
	ekey[rc].ch      = (F_ON(F_USE_FK,ps_global)) ? PF4 + rc : TAB;
	ekey[rc].rval    =  12;
	ekey[rc].name    = "TAB";
	ekey[rc++].label = "Complete";
    }

    ekey[rc].ch = -1;

    *new_folder = '\0';
    help = NO_HELP;
    while(1) {
	/* only show collection number if more than one available */
	if(ps_global->context_list->next){
	    sprintf(prompt, "SAVE to folder in <%.25s> [%s] : ",
		    cntxt->label[0], folder);
	}
	else
	  sprintf(prompt, "SAVE to folder [%s] : ", folder);

        rc = optionally_enter(new_folder, screen_line, 0, MAXFOLDER, 1, 0,
                              prompt, ekey, help, 0);

	if(rc == -1){
	    q_status_message(1, 1, 3, "\007Error reading folder name");
	    return(0);
	}
	else if(rc == 1){
	    q_status_message(0, 0, 2, "\007Save message cancelled");
	    return(0);
	}
	else if(rc == 2) {
	    void (*redraw)() = ps_global->redrawer;

	    push_titlebar_state();
	    rc = folder_lister(ps_global,SaveMessage,cntxt,&cntxt,new_folder);
            ClearScreen();
	    pop_titlebar_state();
	    redraw_titlebar();
            if(ps_global->redrawer = redraw) /* reset old value, and test */
              (*ps_global->redrawer)();
	}
	else if(rc == 3){
            help = (help == NO_HELP) ? h_oe_save : NO_HELP;
	}
	else if(rc == 10){			/* Previous collection */
	    CONTEXT_S *start;
	    start = cntxt;
	    tc    = cntxt;

	    while(1){
		if((tc = tc->next) == NULL)
		  tc = ps_global->context_list;

		if(tc == start)
		  break;

		if((tc->type&FTYPE_BBOARD) == 0)
		  cntxt = tc;
	    }
	}
	else if(rc == 11){			/* Next collection */
	    tc = cntxt;

	    do
	      if((cntxt = cntxt->next) == NULL)
		cntxt = ps_global->context_list;
	    while((cntxt->type&FTYPE_BBOARD) && cntxt != tc);
	}
	else if(rc == 12){			/* file name completion! */
	    if(!folder_complete(cntxt, new_folder))
	      Writechar('\007', 0);

	}
	else if(rc != 4)
          break;
    }

    dprint(9, (debugfile, "rc = %d, \"%s\"  \"%s\"\n", rc, new_folder,folder));
    if(rc == 1 || (!*new_folder && !*folder) || (rc == 2 && !*new_folder)) {
        q_status_message(0, 0, 2 ,
			 "\007No folder named; save message cancelled");
        return(0);
    }

    removing_trailing_white_space(new_folder);
    removing_leading_white_space(new_folder);

    last_context = cntxt;		/* remember for next time */
    if(!*new_folder)
      strcpy(new_folder, folder);
    else
      strcpy(folder, new_folder);

    /* is it a nickname? */
    if(context_isambig(new_folder)
       && (p = folder_is_nick(new_folder, cntxt->folders))){
	strcpy(new_folder, p);
    }

    if(rc = save(new_folder, cntxt, message, index_sorted_msgno)){
	if(ps_global->context_list->next && context_isambig(new_folder)){
	    sprintf(tmp_20k_buf, 
		    "Message %s copied to \"%.15s%s\" in <%.15s%s>",
		    long2string(index_sorted_msgno), new_folder,
		    (strlen(new_folder) > 15) ? "..." : "",
		    cntxt->label[0],
		    (strlen(cntxt->label[0]) > 15) ? "..." : "");
	}
	else
	  sprintf(tmp_20k_buf, "Message %s copied to folder \"%.27s%s\"",
		  long2string(index_sorted_msgno), new_folder,
		  (strlen(new_folder) > 27) ? "..." : "");

	if(!READONLY_FOLDER)
	  strcat(tmp_20k_buf, " and marked deleted");

	q_status_message(0, 1, 3, tmp_20k_buf);
    }

    return(rc);
}




/*----------------------------------------------------------------------
        Do the work of actually saving message in a folder

    Args: folder  -- The folder to save the message in
          message -- The MESSAGECACHE entry of message to save

  Result: Returns 0 if message couldn't be saved
          returns 1 if it was saved

Just gather the message pieces and let c-client handle the rest...
 ----*/
#ifndef VMS
int
save(folder, context, message, index_sorted_msgno)
     char         *folder;
     CONTEXT_S    *context;
     MESSAGECACHE *message;
     long          index_sorted_msgno;
{
    int         rc, rv;
    char       *tmp, *save_folder;
    STORE_S    *so;
    STRING      msg;
    MAILSTREAM *save_stream = NULL;
    SourceType  src;
#ifdef	DOS
    struct {			/* hack! stolen from dawz.c */
	int fd;
	unsigned long pos;
    } d;
    extern STRINGDRIVER dawz_string;

    src = FileStar;
#else
    src = CharStar;
#endif
    /*
     * time to start depending on c-client for this!  Use context_append
     * and context_create to do the dirty work, but first, set up
     * storage object...
     */
    if((so = so_get(src, NULL, WRITE_ACCESS)) == NULL){
	q_status_message(1, 2, 4, 
			 "\007Problem creating space for message text.");
	return(0);
    }

#ifdef	DOS
    /* set append file and dos_gets */
    mailgets    = dos_gets;
    append_file = (FILE *) so_text(so);
#endif

    if((tmp=mail_fetchheader(ps_global->mail_stream, message->msgno))==NULL){
	so_give(&so);
	return(0);
    }
#ifndef	DOS
    else
      so_puts(so, tmp);
#endif

    if((tmp=mail_fetchtext(ps_global->mail_stream, message->msgno)) == NULL){
	so_give(&so);
	return(0);
    }
#ifndef	DOS
    else
      so_puts(so, tmp);
#endif

    so_seek(so, 0L, 0);				/* just in case */

    /*
     * What's really needed is a way to pipe this crap right into
     * context_append...
     */
    /* set up string driver */
#ifdef	DOS
    d.fd  = fileno((FILE *)so_text(so));
    d.pos = 0L;
    INIT(&msg, dawz_string, (void *)&d, filelength(d.fd));
#else
    INIT(&msg, mail_string, (void *)so_text(so), strlen((char *)so_text(so)));
#endif

    save_folder = (strucmp(folder, ps_global->inbox_name) == 0)
                   ? ps_global->VAR_INBOX_PATH : folder;

    save_stream = context_same_stream(context->context,
				      save_folder,
				      ps_global->mail_stream);

    if(!save_stream && ps_global->mail_stream != ps_global->inbox_stream)
      save_stream = context_same_stream(context->context,
					save_folder,
					ps_global->inbox_stream);



    ps_global->try_to_create = 0; 	/* set in mm_notify? */
    rv = rc = 0;
    while(!(rv = context_append(context->context, save_stream,
				save_folder, &msg))){
	if(rc++ || !ps_global->try_to_create)   /* abysmal failure! */
	  break;				/* c-client returned error? */

	/*
	 * already exists?
	 */
	if(ps_global->context_list->next && context_isambig(save_folder)){
	    sprintf(tmp_20k_buf,
		    "Folder \"%.15s%s\" in <%.15s%s> doesn't exist. Create",
		    save_folder, (strlen(save_folder) > 15) ? "..." : "",
		    context->label[0],
		    (strlen(context->label[0]) > 15) ? "..." : "");
	}
	else
	  sprintf(tmp_20k_buf,"Folder \"%.40s\" doesn't exist.  Create",
		  save_folder);

        if(want_to(tmp_20k_buf, 'y', 'n', NO_HELP, 0) != 'y'){
            q_status_message(0, 1, 3, "\007Save message cancelled");
	    break;
        }

	if(!context_create(context->context, 
			  create_proto(save_stream, context, save_folder),
			  save_folder))
	  break;				/* c-client returned error? */

/* BUG: add folder to list in context if ambiguous!! */
	SETPOS((&msg), 0L);			/* reset string driver */
    }

    ps_global->try_to_create = 0;		/* reset for next time */
#ifdef	DOS
    append_file = NULL;				/* reset DOS hacks */
    mailgets    = NULL;				/* reset DOS hacks */
    mail_gc(ps_global->mail_stream, GC_TEXTS);
#endif
    so_give(&so);
    return(rv);
}
#else /* !VMS */
int
save(folder, context, message, index_sorted_msgno)
     char         *folder;
     CONTEXT_S    *context;
     MESSAGECACHE *message;
     long          index_sorted_msgno;
{
   return (int) vms_move_folder(folder, context, message);
}
#endif /* !vms */

/*----------------------------------------------------------------------
    Export a message to a plain file in users home directory

   Args:  q_line -- screen line to prompt on
         message -- MESSAGECACHE enrty of message to export

 Result: 
 ----*/
export_message(q_line, sorted_msgno)
     int  q_line;
     long sorted_msgno;
{
    HelpType	   help;
    char           filename[MAXPATH+1], full_filename[MAXPATH+1],*ill,
                   from[MAX_ADDRESS+1];
    int            rc, new_file;
    ENVELOPE      *env;
    BODY          *b;
    MESSAGECACHE  *mc;
    long           now, raw_msgno;
    gf_io_t	   pc;
    STORE_S	   *store;

    raw_msgno = ps_global->sort[sorted_msgno];

    env  = mail_fetchstructure(ps_global->mail_stream, raw_msgno, &b);
    mc   = mail_elt(ps_global->mail_stream, raw_msgno);

    if(mc == NULL || env == NULL) {
        q_status_message(1, 2, 4,
                  "\007Can't export message. Error accessing mail folder");
         return(0);
    }

    if(ps_global->restricted){
	q_status_message(1, 1, 3, "Pine demo can't export messages to files");
	return(0);
    }

    help = NO_HELP;
    filename[0] = '\0';
    while(1) {
        rc = optionally_enter(filename, q_line, 0, MAXPATH, 1, 0,
#ifdef	DOS
                         "File to save message text in: ",
#else
                         "File (in home directory) to save message text in: ",
#endif
                          NULL, help,0);

        /*--- Help ----*/
        if(rc == 3) {
            help = (help == NO_HELP) ? h_oe_export : NO_HELP;
            continue;
        }

        removing_trailing_white_space(filename);
        removing_leading_white_space(filename);
        if(rc == 1 || filename[0] == '\0') {
            q_status_message(0, 0, 2, "\007Export message cancelled");
            return(0);
        }

        if(rc == 4)
          continue;


        /*-- check out and expand file name. give possible error messages --*/
        strcpy(full_filename, filename);
        if((ill = filter_filename(filename)) != NULL) {
            q_status_message1(1, 1, 3, "\007%s in file name", ill);
            display_message('x');
            sleep(3);
            continue;
        }
#ifndef	DOS
        if(full_filename[0] == '~') {
            if(fnexpand(full_filename, sizeof(full_filename)) == NULL) {
                char *p = strindex(full_filename, '/');
                if(p != NULL)
                  *p = '\0';
                q_status_message1(1, 1, 3,
                    "\007Error expanding file name: \"%s\" unknown user",
    	            full_filename);
                display_message('x');
                sleep(3);
                continue;
            }
        } else if(full_filename[0] != '/') {
#else
        if(full_filename[0] == '\\' || full_filename[1] == ':') {
	    fixpath(full_filename, MAXPATH);
	}
	else{
#endif
            build_path(full_filename, ps_global->home_dir, filename);
        }


        break; /* Must have got an OK file name */

    }


    /* ---- full_filename already contains the absolute path ---*/
    if(!can_access(full_filename, ACCESS_EXISTS)) {
        char wt, prompt[100];
        sprintf(prompt, "File \"%s\" already exists. Append message to it",
                filename);
        wt = want_to(prompt, 'y', 'n', NO_HELP, 0);
        if(wt == 'n') {
            q_status_message(0, 0, 2, "\007Export message cancelled");
            return(0);
        }
        new_file = 0;
    } else {
        new_file = 1;
    }

    dprint(5, (debugfile, "Opening file \"%s\" for export\n", full_filename));

    if((store = so_get(FileStar, full_filename, WRITE_ACCESS)) == NULL){
        q_status_message2(1, 2, 4,
                        "\007Error opening file \"%s\" to export message: %s",
                          full_filename, error_description(errno));
        return(0);
    }

    /*--------- The message separator -------*/
    if(env && env->from)
      sprintf(from, "%s%s%s",env->from->mailbox,
                           env->from->host == NULL ? "" : "@",
                           env->from->host == NULL ? "" : env->from->host);
    else
      strcpy(from, "the-concourse-on-high");

    now = time(0);

    sprintf(tmp_20k_buf, "%sFrom %s ", new_file ? "" : NEWLINE, from);
    so_puts(store, tmp_20k_buf);

    strcpy(tmp_20k_buf, ctime(&now)); 	/* have to put our own newline... */
    tmp_20k_buf[strlen(tmp_20k_buf) - 1] = '\0';
    so_puts(store, tmp_20k_buf);
    so_puts(store, NEWLINE);		/* ... on ctime since DOS stinks */
    gf_set_so_writec(&pc, store);

    if(!format_message(mc->msgno, env, b, (FM_NEW_MESS|FM_DO_PRINT), pc)){
	q_status_message2(0, 1, 3, 
			  "\007Error exporting to file \"%s\" : %s",
			  filename, error_description(errno));
	return(0);
    }
    else{
	q_status_message2(0, 1, 3, "Message %s exported to file \"%s\"",
			  long2string(sorted_msgno), filename);
    }

    so_give(&store);		/* release storage */
    return(0);
}





/*----------------------------------------------------------------------
     Execute command to take address out of message and put in the address book

   Args: message --  The MESSAGECACHE of message to take address from
         quest_line -- The screen line number to prompt on

  Result: -1 is returned on failure, 0 on success
          The entry is added to the incode address book and to the file
 ----*/     

grab_addr_cmd(env, quest_line)
     ENVELOPE *env;
     int quest_line;
{
    int rc;
    char nickname[MAX_NICKNAME+1], name[MAX_FULLNAME+1],
         address[MAX_ADDRESS + 1], *prompt;
    HelpType  help;
    ADDRESS *addr;


    dprint(4, (debugfile, "\n - taking address into address book - \n"));
    if(ps_global->max_msgno == 0) {
	q_status_message(0, 1, 3, "\007No messages to grab the address from");
	return(-1);
    }
    addr = env->from;

    strcpy(address, addr->mailbox); /* BUG, this could overflow address */
    if(addr->host != NULL && addr->host[0] != '\0') {
        strcat(address, "@");
	strcat(address, addr->host);
    }


    prompt = "Enter nick name :";
    help = NO_HELP;
    *nickname = '\0';
    while(1) {
        rc = optionally_enter(nickname, quest_line, 0, MAX_NICKNAME, 1, 0,
                              prompt, NULL, help, 0);
        if(rc == 3) {
            help = (help == NO_HELP) ? h_oe_takenick : NO_HELP;
            continue;
        }
        if(rc == 4)
          continue;

        if(rc == 1 || (rc == 0 && !*nickname))
           goto take_cancel;

        if(strindex(nickname, ' ') != NULL) {
            q_status_message(0, 1, 1,
			    "\007Blank spaces not allowed in nick names");
            display_message(NO_OP_COMMAND);
            sleep(3);
            continue;
        }

        if(addr_lookup(nickname) == NULL)
          break;

        q_status_message1(0, 1, 1, "\007Already an entry with nick name \"%s\"",
                         nickname);
        display_message(NO_OP_COMMAND);
        sleep(3);
    }


    /*-- reearrange full name ---*/
    if(addr->personal != NULL) {
        if(strindex(addr->personal, ',') != NULL) {
            name[0] = '"';
            strcpy(&name[1], addr->personal);
            strcat(name, "\"");
        } else if(strindex(addr->personal, ' ') == NULL) {
            strcpy(name, addr->personal);
        } else {
            char *p, *q, *r;
            p = addr->personal;
            while((q = strindex(p, ' ')) != NULL) p = q+1;
            for(q = p, r = name; *q; *r++ = *q++);
            *r++ = ',';
            *r++ = ' ';
            for(q = addr->personal; q < p; *r++ = *q++);
            *r = '\0';
        }
    } else {
        name[0] = '\0';
    }
            

    prompt = "Full Name :";
    help = NO_HELP;
    while(1) {
        rc = optionally_enter(name,quest_line, 0, MAX_FULLNAME, 1, 0, prompt,
                              NULL, help, 0);
        if(rc == 3) {
            help = (help == NO_HELP) ? h_oe_takename : NO_HELP;
            continue;
        }
        if(rc != 4)
          break;
    }
    if(rc == 1 || !*name)
      goto take_cancel;


    prompt = "Address :";
    help = NO_HELP;
    while(1) {
        rc = optionally_enter(address, quest_line, 0, MAX_ADDRESS, 1, 0,
                              "Address :", NULL, help, 0);
        if(rc == 3) {
            help = (help == NO_HELP) ? h_oe_takeaddr : NO_HELP;
            continue;
        }
        if(rc != 4)
          break;
    }
    if(rc == 1 || !*address)
      goto take_cancel;


    rc = addr_add(nickname, name, address);

    /* addr_add does a status message for us */

    return(0);
    
  take_cancel:
    q_status_message(0, 0, 2, "\007Address book addition cancelled");
    return(-1);
}



/*----------------------------------------------------------------------
      Execute command to jump to a given message number

    Args: qline -- Line to ask question on

  Result: returns -1 or the message number to jump to
          the mangled_footer flag is set
 ----*/
long
jump_to(qline, first_num)
     int qline;
     int first_num;
{
    char     jump_num_string[80], *j, prompt[70];
    HelpType help;
    int      rc;
    long     jump_num;

    dprint(4, (debugfile, "\n - jump_to -\n"));

    if(first_num){
	jump_num_string[0] = first_num;
	jump_num_string[1] = '\0';
    }
    else
      jump_num_string[0] = '\0';

    strcpy(prompt, "Message number to jump to : ");
    help = NO_HELP;
    while (1) {
        rc = optionally_enter(jump_num_string, qline, 0,
                              sizeof(jump_num_string) - 1, 1, 0, prompt,
                              NULL, help, 0);
        if(rc == 3) {
            help = help == NO_HELP ? h_oe_jump : NO_HELP;
            continue;
        }

        if(rc == 0 && *jump_num_string != '\0') {
	    removing_trailing_white_space(jump_num_string);
	    removing_leading_white_space(jump_num_string);
            for(j = jump_num_string; isdigit(*j) || *j == '-'; j++);
	    if(*j != '\0') {
	        q_status_message(0, 2, 2,
                           "\007Invalid number entered. Use only digits 0-9");
            } else {
                jump_num = atol(jump_num_string);
                if(jump_num < 1L) {
	            q_status_message1(1, 2, 2,
                              "\007Message number (%s) must be greater than 0",
                                      long2string(jump_num));
                } else if(jump_num > ps_global->max_msgno) {
                    q_status_message1(1, 2, 2,
	   "\007Message number must be no more than %s, the number of messages",
		    long2string(ps_global->max_msgno));
                } else {
                    return(jump_num);
                }
            }
            jump_num_string[0] = '\0';
            display_message(NO_OP_COMMAND);
            sleep (3);
            continue;
	}

        if(rc != 4)
          break;
    }
    return(-1L);
}
		


/*----------------------------------------------------------------------
     Prompt for folder name to open, expand the name and return it

   Args: qline      -- Screen line to prompt on
         allow_list -- if 1, allow ^T to bring up collection lister

 Result: returns the folder name or NULL
         pine structure mangled_footer flag is set
         may call the collection lister in which case mangled screen will be set

 This prompts the user for the folder to open, possibly calling up
the collection lister if the user types ^T.
----------------------------------------------------------------------*/
char *
broach_folder(qline, allow_list, context)
    int qline, allow_list;
    CONTEXT_S **context;
{
    HelpType	help;
    static char new_folder[MAXFOLDER+1];
    char        expanded[MAXPATH+1],
                prompt[MAXFOLDER+80],
               *last_folder;
    CONTEXT_S  *tc, *tc2;
    FOLDER_S   *f;
    ESCKEY_S    ekey[5];
    int         rc, inbox;

    /*
     * the idea is to provide a clue for the context the file name
     * will be saved in (if a non-imap names is typed), and to
     * only show the previous if it was also in the same context
     */
    help           = NO_HELP;
    *expanded      = '\0';
    *new_folder    = '\0';
    last_folder    = NULL;
    inbox          = strucmp(ps_global->cur_folder,ps_global->inbox_name) == 0;
    ps_global->mangled_footer = 1;

    /*
     * if we're given a valid context AND we don't have our current
     * folder open, go with what's given ELSE go with inbox collection
     */
    tc = (inbox) ? (context && *context) ? *context:ps_global->context_current
		 : ps_global->context_list;

    /* set up extra command option keys */
    rc = 0;
    ekey[rc].ch  = (allow_list) ? (F_ON(F_USE_FK,ps_global)) 
					 ? PF4 + rc : ctrl('T') : 0 ;
    ekey[rc].rval    = (allow_list) ? 2 : 0;
    ekey[rc].name    = (allow_list) ? "^T" : "";
    ekey[rc++].label = (allow_list) ? "ToFldrs" : "";

    if(ps_global->context_list->next){
	ekey[rc].ch      =  (F_ON(F_USE_FK,ps_global)) ? PF4 + rc : ctrl('P');
	ekey[rc].rval    =  10;
	ekey[rc].name    = "^P";
	ekey[rc++].label = "Prev Collection";

	ekey[rc].ch      = (F_ON(F_USE_FK,ps_global)) ? PF4 + rc : ctrl('N');
	ekey[rc].rval    =  11;
	ekey[rc].name    = "^N";
	ekey[rc++].label = "Next Collection";
    }

    if(F_ON(F_ENABLE_TAB_COMPLETE,ps_global)){
	ekey[rc].ch      = (F_ON(F_USE_FK,ps_global)) ? PF4 + rc : TAB;
	ekey[rc].rval    =  12;
	ekey[rc].name    = "TAB";
	ekey[rc++].label = "Complete";
    }

    ekey[rc].ch = -1;

    while(1) {
	/*
	 * Figure out next default value for this context.  The idea
	 * is that in each context the last folder opened is cached.
	 * It's up to pick it out and display it.  This is fine
	 * and dandy if we've currently got the inbox open, BUT
	 * if not, make the inbox the default the first time thru.
	 */
	if(allow_list){
	    if(tc->last_folder[0]){
		last_folder = (inbox) ? tc->last_folder: ps_global->inbox_name;
		sprintf(expanded, " [%s]", last_folder);
		inbox = 1;	/* pretend we're in inbox from here on  */
	    }
	    else{
		last_folder = NULL;
		*expanded = '\0';
	    }
	}

	/* only show collection number if more than one available */
	if(ps_global->context_list->next){
	    sprintf(prompt, "GOTO %s in <%.20s> %s%s: ",
		    (tc->type&FTYPE_BBOARD) ? "news group" : "folder",
		    tc->label[0], expanded, *expanded ? " " : "");
	}
	else
	  sprintf(prompt, "GOTO folder %s: ", expanded, *expanded ? " " : "");

        rc = optionally_enter(new_folder, qline, 0, MAXFOLDER, 1 ,0, prompt,
                              ekey, help, 0);

	if(rc == -1){
	    q_status_message(1, 1, 3, "\007Error reading folder name");
	    return(NULL);
	}
	else if(rc == 1){
	    q_status_message(0, 0, 2, "\007Open Folder cancelled");
	    return(NULL);
	}
	else if(rc == 2){
	    void (*redraw)() = ps_global->redrawer;

	    push_titlebar_state();
	    rc = folder_lister(ps_global, OpenFolder, tc, &tc, new_folder);
            ClearScreen();
	    pop_titlebar_state();
            redraw_titlebar();
            if(ps_global->redrawer = redraw) /* reset old value, and test */
              (*ps_global->redrawer)();
	}
	else if(rc == 3){
            help = help == NO_HELP ? h_oe_broach : NO_HELP;
	}
	else if(rc == 10){			/* Previous collection */
	    tc2 = ps_global->context_list;
	    while(tc2->next && tc2->next != tc)
	      tc2 = tc2->next;

	    tc = tc2;
	}
	else if(rc == 11){			/* Next collection */
	    tc = (tc->next) ? tc->next : ps_global->context_list;
	}
	else if(rc == 12){			/* file name completion! */
	    if(!folder_complete(tc, new_folder))
	      Writechar('\007', 0);
	}
	else if(rc != 4)
          break;
    }

    removing_trailing_white_space(new_folder);
    removing_leading_white_space(new_folder);

    if(*new_folder == '\0'  && last_folder == NULL) {
        q_status_message(0, 0, 2, "\007Open folder cancelled");
        return(NULL);
    }

    if(*new_folder == '\0')
      strcpy(new_folder, last_folder);

    dprint(2, (debugfile, "broach folder, name entered \"%s\"\n",new_folder));

    /*-- Just check that we can expand this. It gets done for real later --*/
    strcpy(expanded, new_folder);
    if (! expand_foldername(expanded)) {
        dprint(1, (debugfile,
                    "Error: Failed on expansion of filename %s (save)\n", 
    	  expanded));
        return(NULL);
    }

    *context = tc;
    return(new_folder);
}




/*----------------------------------------------------------------------
    Actually attempt to open given folder 

  Args: new_folder -- The folder name to open

 Result:  1 if the folder was successfully opened
          0 if the folder open failed and went back to old folder
         -1 if open failed and no folder is left open
      
  Attempt to open the folder name given. If the open of the new folder
  fails then the previously open folder will remain open, unless
  something really bad has happened. The designate inbox will always be
  kept open, and when a request to open it is made the already open
  stream will be used. Making a folder the current folder requires
  setting the following elements of struct pine: mail_stream, cur_folder,
  current_msgno, max_msgno. Attempting to reopen the current folder is a 
  no-op.

  The first time the inbox folder is opened, usually as Pine starts up,
  it will be actually opened.
  ----*/

do_broach_folder(new_folder, new_context) 
     char      *new_folder;
     CONTEXT_S *new_context;
{
    MAILSTREAM *m;
    int         open_inbox, rv;
    char        expanded_file[MAXPATH+1], *old_folder,*old_path, *p;
    long        openmode;
    CONTEXT_S  *tc;

#ifdef	DOS
    openmode = OP_SHORTCACHE;
#else
    openmode = 0L;
#endif
#ifdef	DEBUG
    if(debug > 8)
      openmode |= OP_DEBUG;
#endif
    dprint(1, (debugfile, "About to open folder \"%s\"    inbox: \"%s\"\n",
	       new_folder, ps_global->inbox_name));

    /*----- Little to do to if reopening same folder -----*/
    if(new_context == ps_global->context_current && ps_global->mail_stream
       && strcmp(new_folder, ps_global->cur_folder) == 0)
      return(1);			/* successful open of same folder! */

    if(strcmp(new_folder, POSTPONED_MAIL) == 0) {
        q_status_message1(1, 2, 4,
     "\007Can't open \"%s\". To continue postponed mail, give compose command",
                          POSTPONED_MAIL);
        return(0);
    }

    if(strcmp(new_folder, INTERRUPTED_MAIL) == 0) {
        q_status_message1(1, 2, 4, 
    "\007To continue interrupted mail, give compose command. (Can't open %s.)",
                          INTERRUPTED_MAIL);
        return(0);
    }
    
    /*--- Set flag that we're opening the inbox, a special case ---*/
    /*
     * We want to know if inbox is being opened either by name OR
     * fully qualified path...
     */
    if(open_inbox = strucmp(new_folder, ps_global->inbox_name) == 0
       || strcmp(new_folder, ps_global->VAR_INBOX_PATH) == 0){
	new_context = ps_global->context_list; /* restore first context */

	/*
	 * IF we're asked to open inbox AND it's already open AND
	 * the only stream, just return ELSE fall thru and close 
	 * mail_stream returning with inbox_stream as new stream...
	 */
	if(ps_global->inbox_stream 
	   && (ps_global->inbox_stream == ps_global->mail_stream))
	  return(1);
    }

    /*
     * If ambiguous foldername (not fully qualified), make sure it's
     * not a nickname for a folder in the given context...
     */
    strcpy(expanded_file, new_folder); 	/* might get reset below */
    if(!open_inbox && new_context && context_isambig(new_folder)){
	if (p = folder_is_nick(new_folder, new_context->folders)){
	    strcpy(expanded_file, p);
	    dprint(2, (debugfile, "broach_folder: nickname for %s is %s\n",
		       expanded_file, new_folder));
	}
	else if ((new_context->use & CNTXT_INCMNG)
		 && (folder_index(new_folder, new_context->folders) < 0)){
	    q_status_message1(1, 2, 4,
			    "\007Can't find Incoming Folder %s.", new_folder);
	    return(0);
	}
    }

    /*--- Opening inbox, inbox has been already opened, the easy case ---*/
    if(open_inbox && ps_global->inbox_stream != NULL ) {
#ifdef	OLDWAY
        MsgNo first_unseen;
#else
	long i;
#endif
        expunge_and_close(ps_global->mail_stream, ps_global->cur_folder);

	ps_global->mail_stream              = ps_global->inbox_stream;
	ps_global->max_msgno                = ps_global->inbox_max_msgno;
        ps_global->new_mail_count           = 0L;
        ps_global->expunge_count            = 0L;
        ps_global->last_msgno_flagged       = 0L;
        ps_global->mail_box_changed         = 0;
        ps_global->new_current_sorted_msgno = -1L;
        ps_global->noticed_dead_stream      = 0;
        ps_global->noticed_dead_inbox       = 0;
        ps_global->dead_stream              = 0;
        ps_global->dead_inbox               = 0;
        ps_global->current_sort_order       = ps_global->inbox_sort_order;
        ps_global->current_reverse_sort     = ps_global->reverse_sort;

	dprint(7, (debugfile, "%ld %ld %x\n", ps_global->current_sorted_msgno,
                   ps_global->max_msgno,
		   ps_global->mail_stream));
	/*
	 * remember last context and folder
	 */
	if(context_isambig(ps_global->cur_folder)){
	    ps_global->context_last = ps_global->context_current;
	    strcpy(ps_global->context_current->last_folder,
		 ps_global->cur_folder);
	}

	strcpy(ps_global->cur_folder, ps_global->inbox_name);
	ps_global->context_current = ps_global->context_list;
        clear_index_cache();
        sort_current_folder();		/* MUST sort before restoring msgno! */
	ps_global->current_sorted_msgno = sorted_msgno(ps_global,
						  ps_global->inbox_raw_msgno);
        q_status_message2(0, 1, 3, "Opened folder \"INBOX\" with %s message%s",
                          long2string(ps_global->max_msgno),
                          ps_global->max_msgno != 1 ? "s" : "" );
	return(1);
    }

    if(!new_context && ! expand_foldername(expanded_file))
      return(0);

    old_folder = NULL;
    old_path   = NULL;
    /*---- now close the old one we had open if there was one ----*/
    if(ps_global->mail_stream != NULL) {
        old_folder   = cpystr(ps_global->cur_folder);
        old_path     = cpystr(ps_global->mail_stream->mailbox);
	if(strcmp(ps_global->cur_folder, ps_global->inbox_name) == 0) {
	    /*-- don't close the inbox stream, save a bit of state --*/
	    ps_global->inbox_max_msgno  = ps_global->max_msgno;
	    ps_global->inbox_raw_msgno  = 
                             ps_global->sort[ps_global->current_sorted_msgno];
	    ps_global->inbox_sort_order = ps_global->current_sort_order;
	    dprint(2, (debugfile,
		     "Close - saved inbox state: real %ld, max %ld\n",
		     ps_global->inbox_raw_msgno, ps_global->inbox_max_msgno));
	} else {
#ifdef	LATER
	    /*
	     * Folder count prefix is turned off for now as it's too
	     * costly to open each newsgroup to get the unread count.
	     * See context_bboard().
	     */

            if(ps_global->context_current &&
               (ps_global->context_current->type & FTYPE_BBOARD))
              update_news_prefix(ps_global->mail_stream,
				 folder_entry(ps_global->context_index,
					 ps_global->context_current->folders));
#endif
            expunge_and_close(ps_global->mail_stream, ps_global->cur_folder);
	}
    }
    
    q_status_message1(0, 1, 1, "Opening \"%s\"...", pretty_fn(new_folder));
    display_message('x'); /* So they see the message */

    /* 
     * if requested, make access to folder readonly (only once)
     */
    if (ps_global->open_readonly_on_startup) {
	openmode |= OP_READONLY ;
	ps_global->open_readonly_on_startup = 0 ;
    }

    /*
     * The name "inbox" is special, so treat it so 
     * (used to by handled by expand_folder)...
     */
    if(ps_global->nr_mode)
      ps_global->noshow_warn = 1;

    m = context_open((new_context && !open_inbox) ? new_context->context:NULL,
		     NULL, 
		     (open_inbox) ? ps_global->VAR_INBOX_PATH : expanded_file,
		     openmode);

    if(ps_global->nr_mode)
      ps_global->noshow_warn = 0;

    dprint(8, (debugfile, "Opened folder %x \"%s\" (context: \"%s\")\n", m,
               m != NULL ? m->mailbox : "nil", 
	       (new_context) ? new_context->context : "nil"));

    /* Can get m != NULL if correct passwd for remote, but wrong name */
    if(m == NULL || ((p = strindex(m->mailbox, '<')) != NULL &&
                      strcmp(p + 1, "no_mailbox>") == 0)) {
	/*-- non-existent local mailbox, or wrong passwd for remote mailbox--*/
        /* fall back to currently open mailbox */
        rv = 0;
        dprint(8, (debugfile, "Old folder: \"%s\"\n",
               old_folder == NULL ? "" : old_folder));
        if(old_folder != NULL) {
            if(strcmp(old_folder, ps_global->inbox_name) == 0){
                ps_global->mail_stream          = ps_global->inbox_stream;
                ps_global->max_msgno            = ps_global->inbox_max_msgno;
		ps_global->current_sort_order   = ps_global->inbox_sort_order;
		ps_global->current_sorted_msgno = sorted_msgno(ps_global,
						  ps_global->inbox_raw_msgno);

                dprint(8, (debugfile, "Reactivate inbox %ld %ld %x\n",
                           ps_global->current_sorted_msgno,
                           ps_global->max_msgno,
                           ps_global->mail_stream));
                strcpy(ps_global->cur_folder, ps_global->inbox_name);
            } else {
                ps_global->mail_stream = mail_open(NULL, old_path, openmode);
                /* mm_log will take care of error message here */
                if(ps_global->mail_stream == NULL) {
                    rv = -1;
                } else {
                    ps_global->max_msgno = ps_global->mail_stream->nmsgs;
                    ps_global->expunge_count       = 0;
                    ps_global->new_mail_count      = 0;
                    ps_global->noticed_dead_stream = 0;
                    ps_global->dead_stream         = 0;
		    ps_global->last_msgno_flagged  = 0L;

                    reset_check_point();
                    q_status_message1(0, 1, 3, "Folder \"%s\" reopened",
                                      old_folder);
                }
            }

	    if(rv == 0)
	      ps_global->current_sorted_msgno = 
		min(ps_global->current_sorted_msgno, ps_global->max_msgno);

            fs_give((void **)&old_folder);
            fs_give((void **)&old_path);
        } else {
            rv = -1;
        }
        if(rv == -1) {
            q_status_message(0, 2, 4, "\007No folder opened");
            ps_global->max_msgno            = 0L;
            ps_global->current_sorted_msgno = -1L;
            strcpy(ps_global->cur_folder, "");
        }
        return(rv);
    } else {
        if(old_folder != NULL) {
            fs_give((void **)&old_folder);
            fs_give((void **)&old_path);
        }
    }

    /*----- success in opening the new folder ----*/
    dprint(2, (debugfile, "Opened folder \"%s\" with %ld messages\n",
	       m->mailbox, m->nmsgs));


    /*--- A Little house keeping ---*/
    ps_global->mail_stream          = m;
    ps_global->max_msgno            = m->nmsgs;
    ps_global->current_sorted_msgno = -1L;
    ps_global->expunge_count        = 0L;
    ps_global->new_mail_count       = 0L;
    ps_global->last_msgno_flagged   = 0L;
    ps_global->current_sort_order   = ps_global->sort_order;
    ps_global->current_reverse_sort = ps_global->reverse_sort;
    ps_global->noticed_dead_stream  = 0;
    ps_global->noticed_dead_inbox   = 0;
    ps_global->dead_stream          = 0;
    ps_global->dead_inbox           = 0;

#ifdef	DOS
    /*
     * Doubling the array size is too costly under DOS for large 
     * mailboxes.  Allocating a bit over what we need makes more sense.
     */
    if(ps_global->sort_allocated <= ps_global->max_msgno) {
	while(ps_global->sort_allocated <= ps_global->max_msgno)
	  ps_global->sort_allocated += 64;
#else
    if(ps_global->sort_allocated < ps_global->max_msgno * 2) {
        ps_global->sort_allocated = ps_global->max_msgno * 2 + 1;
#endif
        if(ps_global->sort)
          fs_resize((void **)&ps_global->sort,
                    (size_t)ps_global->sort_allocated * sizeof(long));
        else
          ps_global->sort = (long *)fs_get((size_t)ps_global->sort_allocated * 
                                                            sizeof(long));
    }

    /*
     * remember old folder and context...
     */
    if(context_isambig(ps_global->cur_folder))
      strcpy(ps_global->context_current->last_folder,ps_global->cur_folder);

    strcpy(ps_global->cur_folder, new_folder);
    if(new_context){
	ps_global->context_last = ps_global->context_current;
	ps_global->context_current = new_context;
    }

    /*
     * and set the right "save" context (the collection of folders
     * ambiguous names typed at a save prompt are to be associated
     * with)...
     */
    if(new_context && (new_context->type & FTYPE_BBOARD)){
	for(tc = ps_global->context_list; tc; tc = tc->next){
	    if(tc->use & CNTXT_PRIME){
		ps_global->context_write = new_context;
		break;
	    }
	}
    }
    else if(new_context)
      ps_global->context_write = new_context;

    reset_check_point();

    /* UWIN doesn't want to see this message */
    if(!ps_global->nr_mode)
      q_status_message7(0, 2, 4, "%s \"%s\" opened with %s message%s%s",
                      IS_NEWS ? "News group" : "Folder",
                      pretty_fn(new_folder),
                      comatose(ps_global->max_msgno),
                      plural(ps_global->max_msgno),
                      READONLY_FOLDER ?"\007 READONLY" :"",
                      NULL, NULL);

#ifdef X_MAXMESSAGES /* Used when there was a limit to the number of messages*/
    if(ps_global->max_msgno > (80 * MAXMESSAGES)/ 100) {
        /* warn at 80% of capacity. warn early -- he might go on vacation */
        char tmp[30];
        strcpy(tmp, comatose(MAXMESSAGES));
        q_status_message2(1, 3, 4,
           "\007Warning! Folder has %s messages. Maximum allowed is %s",
                         comatose(ps_global->max_msgno), tmp);
    }
#endif
                      
    clear_index_cache();

    ps_global->mail_box_changed = 0;

    sort_current_folder();

    if(ps_global->max_msgno > 0) {
        MsgNo first_unseen;
	if(ps_global->start_entry > 0) {
	  ps_global->current_sorted_msgno = min(ps_global->start_entry,
						      ps_global->max_msgno);
	  ps_global->start_entry = 0;
	/* if it is an incoming mailbox, goto first unseen */
        }else if((open_inbox ||
	           (context_isambig(ps_global->cur_folder) &&
		    (ps_global->context_current->use & CNTXT_INCMNG))) &&
			  (first_unseen = first_sorted_unread(m)) >= 0) {
          ps_global->current_sorted_msgno = first_unseen;
        }else {
          ps_global->current_sorted_msgno = ps_global->current_reverse_sort ?
                                                 (MsgNo) 1:
                                                 (MsgNo)ps_global->max_msgno;
	}

    } else {
        ps_global->current_sorted_msgno = -1L;
    }

    /*--- If we just opened the inbox remember it's special stream ---*/
    if(open_inbox && ps_global->inbox_stream == NULL) {
	ps_global->inbox_stream = ps_global->mail_stream;
    }
	
    return(1);
}


/*----------------------------------------------------------------------
      Expunge (if confirmed) and close a mail stream

    Args: stream   -- The MAILSTREAM * to close
          folder   -- The name the folder is know by 

   Result:  Mail box is expunged and closed. A message is displayed to
             say what happened
 ----*/
void
expunge_and_close(stream, folder)
     MAILSTREAM *stream;
     char *folder;
{
    long  delete_count, max_folder, seen_not_deleted_cnt, saved_msgs, i;
    char  prompt_b[MAX_SCREEN_COLS+1], short_folder_name[MAXFOLDER*2+1],
          temp[MAXFOLDER*2+1], sequence[10];
    CONTEXT_S *save_context;
    MESSAGECACHE *mc;
    struct variable *vars = ps_global->vars;
    int ret;

    if(stream != NULL) {
        dprint(2, (debugfile, "expunge and close mail stream \"%s\"\n",
                   stream->mailbox));

        if(!stream->readonly) {

            q_status_message1(0, 0, 1, "Closing \"%s\"...", folder);
	    display_message('x');

	    /* Save read messages? */
	    if(VAR_READ_MESSAGE_FOLDER && VAR_READ_MESSAGE_FOLDER[0] &&
		    stream == ps_global->inbox_stream &&
                    (seen_not_deleted_cnt = count_seen_not_deleted(stream))) {

	      if(F_OFF(F_AUTO_READ_MSGS,ps_global)) {
                sprintf(prompt_b,
                       "Save the %ld read message%s in \"%s\"",
                         seen_not_deleted_cnt,
                         plural(seen_not_deleted_cnt),
                         VAR_READ_MESSAGE_FOLDER);
		ret = want_to(prompt_b, 'y', 0, NO_HELP, 0);
	      }
	      if(F_ON(F_AUTO_READ_MSGS,ps_global) || ret == 'y') {
	        save_context = default_save_context(ps_global->context_list);
	        if(!save_context)
		  save_context = ps_global->context_list;

		FETCH_ALL_FLAGS(stream);
		saved_msgs = 0;
		for(i=1; i <= stream->nmsgs; i++) {
		  mc = mail_elt(stream, (long)i);
		  if(mc && mc->seen && !mc->deleted) {
		    /* saved, set deleted flag in inbox */
		    if(save(VAR_READ_MESSAGE_FOLDER, save_context, mc, i)) {
		      sprintf(sequence, "%ld", i);
		      mail_setflag(stream, sequence, "\\DELETED");
		      saved_msgs++;
		    }
		    /* failure */
		    else {
		      if(saved_msgs > 0) {
                        q_status_message3(0, 2, 3,
	       "Saved %ld of %ld read messages, aborted after error on msg %ld",
			   (void *)saved_msgs, (void *)seen_not_deleted_cnt,
			   (void *)(i+1L));
		      }else {
                        q_status_message2(0, 2, 3,
				"Unable to save %ld read messages to %s",
				   (void *)seen_not_deleted_cnt,
				   VAR_READ_MESSAGE_FOLDER);
		      }
		      break;
		    }
		  }
		}
	      }
	    }

            delete_count = count_deleted(stream);
            if(delete_count != 0L)  {
                max_folder =    ps_global->ttyo->screen_cols - 50;
                strcpy(temp, pretty_fn(folder));
                if(strlen(temp) > max_folder){
                   strcpy(short_folder_name, "...");
                   strcat(short_folder_name,temp + strlen(temp) -max_folder);
                } else {
                   strcpy(short_folder_name, temp);
                }
                            
		if(F_OFF(F_AUTO_EXPUNGE,ps_global)) {
                  sprintf(prompt_b,
                       "Expunge the %ld deleted message%s from \"%s\"",
                       delete_count,
                         delete_count == 1 ? "" : "s",
                         short_folder_name);
		  ret = want_to(prompt_b, 'y', 0, NO_HELP, 0);
		}
                if(F_ON(F_AUTO_EXPUNGE,ps_global) || ret == 'y'){
                    q_status_message4(0, 2, 3,
                       "Closing \"%s\". Keeping %s message%s and deleting %s",
                                      pretty_fn(folder),
                                      comatose((stream->nmsgs - delete_count)),
                                      plural(stream->nmsgs - delete_count),
                                      long2string(delete_count));
                    display_message('q'); /* So they see the message */
                    mail_expunge(stream);
                } else {
                    q_status_message3(0, 2, 3,
                             "Closing folder \"%s\". Keeping all %s message%s",
                                   pretty_fn(folder), comatose(stream->nmsgs),
                                      plural(stream->nmsgs));
                    display_message('q'); /* So they see the message */
                }
            } else {
                q_status_message3(0, 2, 3,
                            "Closing folder \"%s\". Keeping all %s message%s",
                             pretty_fn(folder), comatose(stream->nmsgs),
                                  plural(stream->nmsgs));
                display_message('q'); /* So they see the message */	
            }
        } else {
            if(ps_global->context_current->type & FTYPE_BBOARD)
              q_status_message1(0, 2, 3,
                       "Closing news group \"%s\" and saving status.",
                              pretty_fn(folder));
            else
              q_status_message1(0, 2, 3,
                       "Closing read-only folder \"%s\". No changes to save",
                              pretty_fn(folder));
            display_message('q');
        }
    }
    mail_close(stream);
}




/*----------------------------------------------------------------------
     Find the closest message in the list that is not marked for deletion

   Args:  stream -- The mail stream/folder to find the messages in
          cur    -- The current message number
          maxmn  -- The largest message number in folder/stream

  Result: Returns a pointer to the MESSAGECACHE of undeleted message
          or NULL
 ----*/

MESSAGECACHE *
closest_not_deleted(stream, cur_sorted_msgno, maxmn)
     MAILSTREAM *stream;
     long cur_sorted_msgno, maxmn;
{
    long          i;
    MESSAGECACHE *mc;

    if(maxmn == 0)
      return((MESSAGECACHE *)NULL);

    (void)mail_fetchstructure(stream, ps_global->sort[cur_sorted_msgno], NULL);
    mc = mail_elt(stream, ps_global->sort[cur_sorted_msgno]);
    if(mc == NULL || !mc->deleted)
      return(mc);

    for(i = 0; cur_sorted_msgno - i > 0 || cur_sorted_msgno + i < maxmn; i++) {
	if(i + cur_sorted_msgno < maxmn) {
            (void)mail_fetchstructure(stream,
                                 ps_global->sort[cur_sorted_msgno + i], NULL);
            mc = mail_elt(stream, ps_global->sort[cur_sorted_msgno + i]);
            if(mc == NULL)
              return(NULL);
	    if(!mc->deleted)
	      return(mc);
	}
        if(cur_sorted_msgno - i > 0) {
            (void)mail_fetchstructure(stream,
                                 ps_global->sort[cur_sorted_msgno - i], NULL);
            mc = mail_elt(stream, ps_global->sort[cur_sorted_msgno - i]);
            if(mc == NULL)
              return(NULL);
	    if(!mc->deleted)
	      return(mc);
	}
    }
    /*--- mailbox will be empty ---*/
    return(NULL);
}
	    
	    

/*----------------------------------------------------------------------
      Search the message headers as display in index
 
  Args: command_line -- The screen line to prompt on
        msg          -- The current message number to start searching at
        max_msg      -- The largest message number in the current folder

  The headers are searched exactly as they are displayed on the screen. The
search will wrap around to the beginning if not string is not found right 
away.
  ----*/
long 
search_headers(command_line, sorted_msg, max_msg)
     int  command_line;
     long sorted_msg, max_msg;
{
    int         rc;
    long        i;
    static char search_string[MAX_SEARCH+1] = { '\0' };
    char        prompt[MAX_SEARCH+50], new_string[MAX_SEARCH+1];
    HelpType	help;

    dprint(4, (debugfile, "\n - search headers - \n"));

    sprintf(prompt, "Word to search for [%s] : ", search_string);
    help = NO_HELP;
    new_string[0] = '\0';

    while(1) {
        rc = optionally_enter(new_string, command_line, 0, MAX_SEARCH, 1,
                             0, prompt, NULL, help, 0);

        if(rc == 3) {
            help = NO_HELP;
            continue;
        }

        if(rc != 4) /* redraw */
          break; /* redraw */
    }

    if(rc == 1 || (new_string[0] == '\0' && search_string[0] == '\0')) {
        q_status_message(0, 0, 2, "\007Search cancelled");
        return(0);
    }

    if(new_string[0] == '\0')
        strcpy(new_string, search_string);

    strcpy(search_string, new_string);

    for(i = sorted_msg + 1; i <= max_msg; i++) {
        if(srchstr(build_header_line(i), search_string) != NULL)
          goto found;
    }

    for(i = 1; i < sorted_msg; i ++) {
        if(srchstr(build_header_line(i), search_string) != NULL)
          goto found;
    }

    q_status_message(0, 1, 3, "\007Word not found");
    return(-1);

  found:
     q_status_message1(0, 1, 3, "Word found%s", i > sorted_msg ? "" :
                        ". Search wrapped to beginning"); 
    return(i);
}
    

/*----------------------------------------------------------------------
    Print message number "m"

  Args:  m - Message number to print.

 Filters the original header and sends stuff to printer
  ---*/
print_message(sorted_m)
     long sorted_m;
{
    char           prompt[30];
    ENVELOPE      *e;
    BODY	  *b;

    sprintf(prompt, "message %ld ", sorted_m);
    if(open_printer(prompt) == 0) {
        e    = mail_fetchstructure(ps_global->mail_stream,
                                  ps_global->sort[sorted_m],
				  &b);

        if(!format_message(ps_global->sort[sorted_m], e, b,
			   (FM_NEW_MESS|FM_DO_PRINT), print_char)){
	    q_status_message(0, 1, 3, "\007Error printing message");
	    return(0);
	}

        close_printer();
    }

    return(1);
}



/*----------------------------------------------------------------------
   Prompt the user for the type of sort he desires

Args: q1 -- Line to prompt on

  ----*/
void
select_sort(ql)
     int ql;
{
    char      prompt[200], tmp[40], first_chars[10], *fc, *sort_names[10];
    int       s, i;
    HelpType  help;
    int       deefault = 'a';


    /*----- String together the prompt ------*/
    strcpy(prompt, "Sort by ");
    fc = first_chars;

    for(i = 0; ps_global->sort_types[i] != EndofList; i++) {
        sort_names[i] = cpystr(sort_name(ps_global->sort_types[i]));
        sort_names[i][0] = toupper(sort_names[i][0]);
        if(i != 0)
          strcat(prompt, ",");
        if(ps_global->current_sort_order == ps_global->sort_types[i]) {
            strcat(prompt, "[");
            *fc++ = '[';
        }
        strcpy(tmp,sort_name(ps_global->sort_types[i]));
        if(strcmp(tmp, "size") == 0) {
            tmp[2] = toupper(tmp[2]);
            *fc++ = tmp[2];
        } else {
            tmp[0] = toupper(tmp[0]);
            *fc++  = tmp[0];
        }
        strcat(prompt, tmp);
        if(ps_global->current_sort_order == ps_global->sort_types[i]) {
            strcat(prompt, "]");
            deefault = *(fc - 1);
            *fc++ = ']';
        }
    }
    sort_names[i++] = cpystr("Reverse");
    sort_names[i]   = NULL;
    strcat(prompt,",Reverse");
    *fc = '\0';

    sprintf(prompt+strlen(prompt)," (type %s or R): ", first_chars);
    help = h_select_sort;
    s = radio_buttons(prompt, ql, 0, "safdzr", sort_names, deefault,
							    'x', help, 0);

    switch(s) {
      case 's':
        ps_global->current_sort_order = SortSubject;
        break;
      case 'z':
        ps_global->current_sort_order = SortSize;
        break;
      case 'f':
        ps_global->current_sort_order = SortFrom;
        break;
      case 'd':
        ps_global->current_sort_order = SortDate;
        break;
      case 'a':
        ps_global->current_sort_order = SortArrival;
        break;
      case 'r':
        ps_global->current_reverse_sort = !ps_global->current_reverse_sort;
        break;
      /* ^C */
      case 'x':
	Writechar('\007', 0);
        break;
    }

    blank_keymenu(ps_global->ttyo->screen_rows - 2, 0);
}



struct attachment *
attachments(qline, sorted_msgno)
     int           qline;
     long          sorted_msgno;
{
    char               prompt[100], section[100];
    HelpType	       help;
    int                r;
    struct attachment *a;
    
    for(a = ps_global->atmts; (a+1)->description != NULL; a++);

    sprintf(prompt, "Enter attachment number to view or save (%s - %s) : ",
            ps_global->atmts->number, a->number);
            
    section[0] = '\0';
    help       = NO_HELP;
    while(1) {
        r = optionally_enter(section, qline, 0, sizeof(section), 1, 0,
                             prompt, NULL, help, 0);
        if(r == 3) {
            help = NO_HELP;
            continue;
        }

        if(r == 1 || section[0] == '\0') {
            q_status_message(0, 0, 2, "\007Attachment cancelled");
            return(NULL);
        }
        removing_trailing_white_space(section);
        removing_leading_white_space(section);
        for(a = ps_global->atmts; a->description != NULL ; a++)
          if(strcmp(section, a->number) == 0)
            break;

        if(a->description == NULL) {
            q_status_message1(1, 2, 4,
             "\007No such attachment number \"%s\". Must be in displayed list",
                              section);
            display_message('x');
            sleep(3);
            continue;
        }

        if(r != 4) /* redraw */
          break; /* redraw */
    }

    return(a);
}


/*----------------------------------------------------------------------
  detach the given body part using the given encoding
  returns: NULL on success, error message otherwise
  ----*/
char *
detach(msg_no, body, part_no, len, pc, aux_filters)
     long       msg_no;			/* message number to deal with	  */
     BODY      *body;			/* body pointer 		  */
     char      *part_no;		/* part number of message 	  */
     long      *len;			/* returns bytes read in this arg */
     gf_io_t    pc;			/* where to put it		  */
     filter_t  *aux_filters;		/* null terminated array of filts */
{
    unsigned long  length, rv;
    char          *status, *contents;
    gf_io_t        gc;
    SourceType     src = CharStar;
    static char    err_string[100];
#ifdef	DOS
    extern unsigned char  *xlate_to_codepage;
#endif

    err_string[0] = '\0';

#ifdef	DOS
    if(body->size.bytes > MAX_MSG_INCORE
       || strcmp(ps_global->mail_stream->dtb->name, "nntp") == 0){
	src      = FileStar;
	mailgets = dos_gets;
    }
    else
      mailgets = NULL;
#endif	/* DOS */

    /*
     * go grab the requested body part
     */
    contents = mail_fetchbody(ps_global->mail_stream,msg_no,part_no,&length);
    if(contents == NULL) {
#ifdef	DOS
        mailgets = NULL;
#endif
	sprintf(err_string, "Unable to access body part %s", part_no);
	if (len != (long *)NULL)
	    *len = 0;
	return(err_string);
    }

    rv = (length) ? length : 1;

    gf_filter_init();			/* prepare to filter it! */

    switch(body->encoding) {		/* handle decoding */
      case ENC7BIT:
      case ENC8BIT:
      case ENCBINARY:
        break;

      case ENCBASE64:
	gf_link_filter(gf_b64_binary);
        break;

      case ENCQUOTEDPRINTABLE:
	gf_link_filter(gf_qp_8bit);
        break;

      case ENCOTHER:
        break;
    }

    while(aux_filters && *aux_filters)	/* apply externally provided filters */
      gf_link_filter(*aux_filters++);

    /*
     * Following canonical model, after decoding convert newlines from
     * crlf to local convention.
     */
    if(body->type == TYPETEXT){
	gf_link_filter(gf_nvtnl_local);
#ifdef	DOS
	/*
	 * When detaching a text part AND it's US-ASCII OR it
	 * matches what the user's defined as our charset,
	 * translate it...
	 */
	if(mime_can_display(body->type, body->subtype, body->parameter)
	   && xlate_to_codepage){
	    gf_translate_opt(xlate_to_codepage, 256);
	    gf_link_filter(gf_translate);
	}
#endif
    }

#ifdef	LATER
    dprint(9, (debugfile, "Attachment lengths: %ld (decoded)  %ud (encoded)\n",
               *decoded_len, length));
#endif

    gf_set_readc(&gc,
#ifdef	DOS
		 /*
		  * If we fetched this to a file, 
		  * make sure we set up the storage
		  * object's getchar function accordingly...
		  */
		 (src == FileStar) ? (char *)append_file :
#endif
		 contents,
		 length, src);

    if(status = gf_pipe(gc, pc)) {
	sprintf(err_string, "Formatting error: %s", status);
        rv = 0L;
    }

#ifdef	DOS
    /*
     * free up file descriptor, and return tmpfile opened by
     * dos_gets and returned via the global "append_file"...
     */
    if(src == FileStar){
	if(append_file)
	  fclose(append_file);

	append_file = NULL;
	mailgets    = NULL;
	mail_gc(ps_global->mail_stream, GC_TEXTS);
    }
#endif

    if (len != (long *)NULL)
	*len = rv;

    if (err_string[0] == '\0')
	return(NULL);
    else
	return(err_string);
}


/*----------------------------------------------------------------------
  ----*/
void
save_attachment(qline, msgno, a)
     int		qline;
     long               msgno;
     struct attachment *a;
{
    char	filename[MAXPATH+1], full_filename[MAXPATH+1], *ill, *prompt;
    HelpType	help;
    char       *l_string, prompt_buf[200];
    int         r, is_text;
    long        len;
    PARAMETER  *param;
    gf_io_t     pc;
    STORE_S    *store;
    char       *err;

#ifdef NEVER
    dprint(9, (debugfile, "save_attachments(length: %ld, name: %s)\n",
               len, suggested_name == NULL ? "NULL" : suggested_name));
#endif

    is_text = a->body->type == TYPETEXT;

    /*-------  Figure out suggested file name ----*/
    for(param = a->body->parameter; param != NULL; param = param->next) {
        if(strucmp(param->attribute, "name") == 0)
          break;
    }
    if(param != NULL && param->value != NULL) 
      strcpy(filename, param->value);
    else
      filename[0] = '\0';

    /*---------- Prompt the user for the file name -------------*/
    help = NO_HELP;
    while(1) {
        prompt = "File (in home directory) to save attachment in: ";
        r = optionally_enter(filename, qline, 0, MAXPATH, 1, 0, prompt,
                             NULL, help, 0);

        /*--- Help ----*/
        if(r == 3) {
            help = (help == NO_HELP) ? h_oe_export : NO_HELP;
            continue;
        }

        if(r == 1 || filename[0] == '\0') {
            q_status_message(0, 0, 2, "\007Save attachment cancelled");
            return;
        }

        if(r == 4)
          continue;

        /* check out and expand file name. give possible error messages */
        strcpy(full_filename, filename);
        removing_trailing_white_space(full_filename);
        removing_leading_white_space(full_filename);
        if((ill = filter_filename(filename)) != NULL) {
            q_status_message1(0, 1, 3, "\007%s in file name", ill);
            display_message('x');
            sleep(3);
            continue;
        }
#ifndef	DOS
        if(full_filename[0] == '~') {
            if(fnexpand(full_filename, sizeof(full_filename)) == NULL) {
                char *p = strindex(full_filename, '/');
                if(p != NULL)
                  *p = '\0';
                q_status_message1(1, 1, 3,
                    "\007Error expanding file name: \"%s\" unknown user",
                    full_filename);
                display_message('x');
                sleep(3);
                continue;
            }
        } else if(full_filename[0] != '/') {
#else
        if(full_filename[0] != '\\' && full_filename[1] != ':') {
#endif
            build_path(full_filename, ps_global->home_dir, filename);
        }
        break; /* Must have got an OK file name */
    }

    if(ps_global->restricted) {
        q_status_message(1, 2, 4, "\007Pine demo can't save attachments");
        return;
    }
     

    /*----------- Write the contents to the file -------------*/
    if(can_access(full_filename, ACCESS_EXISTS) == 0) {
        sprintf(prompt_buf, "File \"%s\" already exists. Overwrite it",
                full_filename);
        if(want_to(prompt_buf, 'n', 'n', NO_HELP, 0) == 'n') {
            q_status_message(0, 1, 3, "\007Save of attachment cancelled");
            return;
        }

	unlink(full_filename);		/* so_get only appends! */
    }

    if((store = so_get(FileStar, full_filename, WRITE_ACCESS)) == NULL){
	q_status_message2(1, 3, 5, "\007Error opening destination %s: %s",
			  full_filename, error_description(errno));
	return;
    }

    gf_set_so_writec(&pc, store);

    if((err = detach(msgno, a->body, a->number, &len, pc, NULL)) == NULL) {
        l_string = cpystr(byte_string(len));
        q_status_message7(0, 2, 4, "Part %s, %s%s written to \"%s\"%s%s%s",
                         a->number, 
                         is_text ?   comatose(a->body->size.lines) :
                                     l_string,
                         is_text ?   " lines" : "",
                         full_filename,
                         is_text || len == a->body->size.bytes ?
                                        "" : "(decoded from ",
                         is_text || len == a->body->size.bytes ?
                                        "" : byte_string(a->body->size.bytes),
                         is_text || len == a->body->size.bytes ?
                                        "" : ")");
        fs_give((void **)&l_string);
    }
    else{
      q_status_message2(1, 3, 5,"\007%s: Error writing attachment to \"%s\"",
                        err, full_filename);
    }

    so_give(&store);
}


/*----------------------------------------------------------------------

  ----*/        
int
display_attachment(msgno, a)
     long               msgno;
     struct attachment *a;
{
    char     filename[MAXPATH+1];
    STORE_S *store;
    gf_io_t  pc;
    filter_t aux_filter[2];
    char    *err;

    aux_filter[0] = NULL;
    if(!ps_global->VAR_IMAGE_VIEWER && a->body->type == TYPEIMAGE){
	q_status_message(0,2,4,"Can't display!  Set image-viewer in pinerc.");
	return(0);
    }

    /*------- Display the attachment -------*/
    if(!a->can_display) {
        /*----- Can't display this type ------*/
        q_status_message3(1, 3, 5,
                 "\007Don't know how to display attachment format %s%s%s",
                          body_type_names(a->body->type),
                          a->body->subtype != NULL ? "/" : "",
                          a->body->subtype != NULL ? a->body->subtype :"");
        return(0);
    }
    
    if(a->body->type == TYPETEXT) {
	display_text_att(msgno, a);
	ps_global->mangled_screen = 1;
        return(0);
    }

        
    /*------ Write the image to a temporary file ------*/
#ifdef	DOS
    strcpy(filename, temp_nam(NULL, "im"));
    if(a->body->subtype != NULL){
	strcat(filename, ".");
	strncat(filename, a->body->subtype, 3);
    }
#else
    if(a->body->type == TYPEAUDIO)  {
        strcpy(filename, "/dev/null");
        aux_filter[0] = gf_play_sound;
	aux_filter[1] = NULL;
    } else {
	char prefix[8];
	sprintf(prefix, "img-%.3s",
		(a->body->subtype) ? a->body->subtype : "unk");
        strcpy(filename, temp_nam(NULL, prefix));
    }
#endif

    if((store = so_get(FileStar, filename, WRITE_ACCESS)) == NULL){
        q_status_message2(1, 3, 5,
                          "\007Error \"%s\", Can't write file %s",
                          error_description(errno), filename);
        return(0);
    }

    gf_set_so_writec(&pc, store);

    if((err=detach(msgno, a->body, a->number, (long *)NULL, pc,
							aux_filter)) != NULL){
	q_status_message2(1, 3, 5,
		"\007%s: Error saving image to temp file %s", err, filename);
        return(0);
    }

    so_give(&store);

    /*----- Run the viewer process ----*/
#ifndef	DOS
    if(a->body->type == TYPEIMAGE)
#endif
        run_viewer(filename, type_desc(a->body->type, a->body->subtype,
                                   a->body->parameter, 1));

#ifdef	DOS
    unlink(filename);
    /* BUG? Clear c-client cache? */
#endif
    return(0);
}

void
display_text_att(msgno, a)
    long            msgno;
    struct attachment *a;
{
    STORE_S        *store;
    gf_io_t         pc;
    SourceType	    src = CharStar;

    clear_index_cache_ent(msgno);

    /* BUG, should check this return code */
    (void)mail_fetchstructure(ps_global->mail_stream, msgno, NULL);

    /* initialize a storage object */
#ifdef	DOS
    if(a->body->size.bytes > MAX_MSG_INCORE
       || strcmp(ps_global->mail_stream->dtb->name, "nntp") == 0){
	src = FileStar;
    }else {
        src = CharStar;
    }
#endif

    store = so_get(src, NULL, EDIT_ACCESS);
    gf_set_so_writec(&pc, store);

    if(!ps_global->full_header) {

	(void) decode_text(a, msgno, pc, QStatus, 1);

    } else {
#ifdef	DOS
	/*
	 * This'll save us having to copy files around.
	 * (see mailview.c)
	 */
	if(src == FileStar){
	    append_file = (FILE *)so_text(store);
	    mailgets    = dos_gets;
	    (void)mail_fetchheader(ps_global->mail_stream, msgno);
	    (void)mail_fetchtext(ps_global->mail_stream, msgno);
	    append_file = NULL;
	    mailgets    = NULL;
	    mail_gc(ps_global->mail_stream, GC_TEXTS);
	}
	else
#endif
	{
	    so_puts(store, mail_fetchheader(ps_global->mail_stream,msgno));
	    so_puts(store, mail_fetchtext(ps_global->mail_stream,msgno));
	}
    }

    scrolltool(so_text(store), "ATTACHMENT TEXT", (int *)NULL,
	       AttachText, src, a);

    so_give(&store);	/* free resources associated with store */

}


#ifdef	DOS
#include	<process.h>
#endif

/*----------------------------------------------------------------------
   Run xloadimage to display gif,pbm and other such files

Args: image_file -- The name of the file to pass to xloadimage

If xloadimage produces any output on the command line or the shell
produces output because it can't find or execute xloadimage the first
5 lines will be displayed on the status line one at a time.
 ----*/
void
run_viewer(image_file, description)
     char *image_file, *description;
{
#ifdef	DOS
#define	MAXARGS	10
    char     *args[MAXARGS], command[MAXPATH], *cp = command;
    int       i;

    /* build args */
/* MAJOR HACK JUST FOR NOW */
    if(struncmp(description, "audio", 5) == 0){
	if(getenv("PINEHOME"))
	  sprintf(command,"%s\\sound", getenv("PINEHOME"));
	else
	  strcpy(command, "c:\\pine\\sound");
	args[0] = command;
	args[1] = image_file;
	args[2] = "27";
	args[3] = NULL;
    }
    else {
	strncpy(command, ps_global->VAR_IMAGE_VIEWER, MAXPATH);
	for(i=0; *cp != '\0';i++){			/* build args array */
	    if(i < MAXARGS+2){
		args[i] = NULL;			/* in case we break out */
	    }
	    else{
		q_status_message1(1, 3, 5, "\007Too many args : %s",
				  ps_global->VAR_IMAGE_VIEWER);
		return;
	    }

	    while(isspace(*cp))
	      if(*cp != '\0')
		cp++;
	      else
		break;

	    args[i] = cp;

	    while(!isspace(*cp))
	      if(*cp != '\0')
		cp++;
	      else
		break;

	    if(*cp != '\0')
	      *cp++ = '\0';
	}
	args[i++] = image_file;
	args[i] = NULL;
    }

    /* actually display the sucker */
    if(spawnv(_P_WAIT, args[0], args) == -1){
	/* error spawning image viewer */
        q_status_message2(0, 4, 5,
                          "\007Error \"%s\" spawning %s",
                          error_description(errno),
			  ps_global->VAR_IMAGE_VIEWER); 
	return;
    }

    /* let main loop know we messed with the display */
    ps_global->mangled_screen = 1;
#else
#ifdef VMS
    char      command[512];
    
    sprintf(command, "%s %s", ps_global->VAR_IMAGE_VIEWER, image_file);
    system(command);
    delete(image_file);
#else /* VMS */
    char      command[512], result_file[100], buf[512];
    int       messages_queued, pid, r, status;
    FILE     *f;
    SigType (*hsig)(), (*isig)(), (*qsig)();
    
    sprintf(command, "(%s %s; rm -f %s) &\n", ps_global->VAR_IMAGE_VIEWER,
          image_file, image_file);

    strcpy(result_file, temp_nam(NULL, "pine_cmd"));

    if((pid = fork()) == 0) {
        int output = creat(result_file, 0777);
        dup2(output, 1); 
        dup2(output, 2);
        execl("/bin/sh", "sh", "-c", command, 0);
        exit(-1);
    }
    isig = signal(SIGINT, SIG_IGN);
    qsig = signal(SIGQUIT, SIG_IGN);
    hsig = signal(SIGHUP, SIG_IGN);
#ifdef HAVE_WAIT_UNION
    while((r=  wait((union wait *)&status)) && r != -1 && r != pid);
#else
    while((r=  wait(&status)) && r != -1 && r != pid);
#endif
    signal(SIGINT,  isig);
    signal(SIGHUP,  hsig);
    signal(SIGQUIT, qsig);

    if(file_size(result_file) == 0L) 
      sleep(3); /* Give the process time to do something */

    messages_queued = 0;
    f = fopen(result_file, "r");
    if(f != NULL){
        unlink(result_file);
        while(fgets(buf, sizeof(buf), f) != NULL && messages_queued < 5) {
            buf[strlen(buf) - 1] = '\0';
            if(strlen(buf) == 0)
              continue;
            q_status_message1(0, 2, 4, "Display result: %s", buf);
            messages_queued++;
        }
        fclose(f);
    }
    if(!messages_queued)
#endif	/* VMS */
#endif
      q_status_message1(0, 1, 4, "Display of %s complete", description);

    display_message('x');
}


/* This isn't quite ctime, because it doesn't have the \n */

char *
mcdate2ctime(m)
     MESSAGECACHE *m;
{
    static char date[30];

    sprintf(date, "Xxx %s %2d %02d:%02d:%02d %04d",
    month_abbrev(m->month), m->day, m->hours, m->minutes, m->seconds,
	    m->year + BASEYEAR);
    return(date);
}
