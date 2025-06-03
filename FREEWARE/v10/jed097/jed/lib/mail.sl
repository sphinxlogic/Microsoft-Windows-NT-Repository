% Mail for VMS and Unix systems
%
% On VMS, this uses callable mail interface.  
% For Unix, uses /usr/ucb/mail
%
% Calls mailhook if defined.  "mail_hook" can either load a file
% to override some of the functions here.  This is because one cannot
% define a function from within a function unless one writes the function
% as a string and evals the string.  For example:
%
%  define create_sum_function ()
%  { 
%     eval ("define sum (x, y) { x + y; }");
%  }
%


#ifdef UNIX
%%% University of Calififornia, Berkeley mail program
%%% "/usr/ucb/mail" on most unix, "/usr/sbin/Mail" on others

!if (is_defined("UCB_Mailer"))
{
   variable UCB_Mailer;
   UCB_Mailer = "/usr/ucb/mail";
   if (1 != file_status(UCB_Mailer))
     {
	UCB_Mailer = "/usr/sbin/Mail";
	if (1 != file_status (UCB_Mailer))
	  {
	     error ("UCB Mail program not found!");
	  }
     }
}

define unix_send_mail (to, cc, subj)
{
   subj = str_quote_string (subj, "'", '\'');
   % This is a total crock.  For ucb mail, we need to 'quote' lines beginning
   % with '~'.  Lets do it now:
   check_region (0);
   dupmark();  pop();
   narrow ();
   bob();
   while (bol_fsearch("~")) insert (" ");
   bob ();
   if (strlen(cc))
     {
	push_spot ();
	insert (Sprintf("~c %s\n", cc, 1));
     }
   eob();
   widen();
   pipe_region (Sprintf("%s -s '%s' '%s'", UCB_Mailer, subj, to, 3));
   if (strlen(cc))
     {
	pop_spot ();
	delete_line();
     }
   
}

#endif


variable Mail_Previous_Buffer = Null_String;
variable Mail_Previous_Windows = 1;

define send ()
{
   variable mail_cmd, to = Null_String, subj = Null_String;
   variable dir, file, cc = Null_String, sent;
  
   push_spot ();
   bob ();
   
   !if (bol_fsearch ("---text follows this line---"))
     {
	pop_spot ();
	error ("Failed to find text divider.");
     }
   
   push_mark ();
   bob (); narrow ();
   if (re_fsearch("^To:[ \t]*\\(.*\\)"))
     {
	to = regexp_nth_match (1);
     }
   if (re_fsearch("^Cc:[ \t]*\\(.*\\)"))
     {
	cc = regexp_nth_match (1);
     }
   if (re_fsearch("^Subject:[ \t]*\\(.*\\)"))
     {
	subj = regexp_nth_match (1);
     }
   
   eob();
   widen ();

   !if (strlen(to))
     {
	pop_spot ();
	error ("Bad To: field.");
     }
#ifdef VMS
   if (strlen(cc))
     {
	to = strcat (strcat(to, ","), cc);
     }
#endif
      
   !if (down(1))
     {
	pop_spot();
	error ("No message!");
     }
   
   push_mark ();
   eob ();
   flush("Sending...");
   
#ifdef UNIX
   sent = unix_send_mail (to, cc, subj);
#endif
#ifdef VMS
   narrow();
   sent = vms_send_mail (to, subj);
   widen ();
#endif
   if (sent) 
     {
	flush ("Sending...done");
	set_buffer_modified_flag (0);
	getbuf_info(); pop(); pop(); =dir; =file;
	delete_file (make_autosave_filename(dir, file)); pop();
     } 
   else
     {
	message ("No message sent.");
     } 
   
   pop_spot ();
   whatbuf();
   if (bufferp(Mail_Previous_Buffer)) sw2buf (Mail_Previous_Buffer);
   if (1 == Mail_Previous_Windows) onewindow();
   bury_buffer(());
}

add_completion("send");

define mail_format_buffer ()
{
   variable mail_map = "mail_map";
   erase_buffer();
   text_mode();
   insert("To: \nCc: \nSubject: \n---text follows this line---\n");
   bob(); eol();
   set_buffer_modified_flag(0);
   set_buffer_undo(1);
   !if (keymap_p(mail_map)) make_keymap(mail_map);
   use_keymap(mail_map);
}

define mail()
{
   variable mail_buf, old, status;
   mail_buf = "*mail*";
   variable file, dir;
      
   old = bufferp(mail_buf);
   Mail_Previous_Windows = nwindows();
   Mail_Previous_Buffer = pop2buf_whatbuf(mail_buf);
   
   %% if buffer is not old, turn autosave on
   if (old == 0)
     {
#ifdef VMS
	dir = "sys$login:";
#else
	dir = dircat(getenv("HOME"), Null_String);
#endif
	file = "__jed_mail__";
	setbuf_info(file, dir, mail_buf, 2);
     }
   else
     {
	status = getbuf_info() & 1; pop(); pop(); pop();
	if (status) return();
     }
   
   mail_format_buffer ();
   runhooks("mail_hook");
}

