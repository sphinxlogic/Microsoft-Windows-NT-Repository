%!% variable that controls the location of the mailbox for new mail
variable Rmail_Spool_Dir = "/usr/spool/mail";
$1 = getenv("MAIL"); if (strlen($1)) Rmail_Spool_Dir = $1;

%!% where the JED mail reader should put the mail
!if (is_defined("Rmail_Directory"))
{
   variable Rmail_Directory = dircat(getenv("HOME"), "Mail");
}

%!% temporary mailbox where newmail is moved to before parsed and converted.
variable Rmail_Tmp_Mbox = dircat(Rmail_Directory, "#NewMail#");

%!% name to use to indicate that this directory is locked.  This way
%!% another jed rmail reader cannot read this now.
variable Rmail_Lock_File_Name = "___LOCKED___";

variable Rmail_Master_Index_File = dircat(Rmail_Directory, "_Root_Index.index");
variable Rmail_Folder_Buffer = Null_String;
variable Rmail_Folder_Name = Null_String;
variable Rmail_Newmail_Folder = "New-Mail";
variable Rmail_Root_Buffer = "Rmail: Folder List";
variable Rmail_Mail_Buffer_Name = Null_String;

!if (is_defined("Rmail_Dont_Reply_To"))
{
   % comma separated list of name to not reply to.   
   variable Rmail_Dont_Reply_To;
   $1 = getenv("USER");
   $2 = getenv("HOST");
   variable Rmail_Dont_Reply_To = $1;
   if (strlen($1) and strlen ($2))
     {
	Rmail_Dont_Reply_To = Sprintf("%s,%s@%s", $1, $1, $2, 3);
     }
}

Rmail_Dont_Reply_To = strlow (Rmail_Dont_Reply_To);

variable Rmail_Message_Number = Null_String;   %/* this is a string!! */


% check for existence of Rmail directory.  Create it if it does not exist.
!if (file_status(Rmail_Directory)) 
{
   !if (mkdir(Rmail_Directory)) error(strcat ("Unable to create ", Rmail_Directory));
   if (chmod(Rmail_Directory, 0700)) error ("chmod failed!");
}

define rmail_buffer_name(folder)
{
   strcat ("Rmail: ", folder);
}


define rmail_folder_file(folder)
{
   strcat (dircat (Rmail_Directory, folder), ".index");
}
   
% set buffer flags so that autosave is off but save anyway is on.
% This way it gets saved in case of a power failure or something.

define rmail_set_buffer_flags()
{
   variable flags = getbuf_info();
   flags = (flags | 0x80) & ~(0x2);    % turn off autosave, turn on save
   flags = flags & ~(0x20);            % turn off undo
   flags = flags | 0x100;              % No backup file.
   setbuf_info(flags);
}

define rmail_save_buffer_secure()
{
   variable dir, file, flags;
   flags = getbuf_info(); pop(); =dir; =file;
   
   !if ((flags & 1) and strlen(file)) return;
   file = dircat(dir, file);
   write_buffer(file); pop();
   chmod(file, 0600); pop();
}

   
define rmail_update_master_index (folder, n, total, n_unread)
{
   variable cbuf = whatbuf(), f, fline;
   
   fline = Sprintf("Folder:\t%20s\t(%d/%d) messages. %d unread.\n", 
		   folder, n, total, n_unread, 4);
   
   variable flags;
   read_file(Rmail_Master_Index_File); pop();
   rmail_set_buffer_flags();
   push_spot();
   bob();
   % only save if necessary.
   !if (bol_fsearch(fline))
     {
	!if (bol_fsearch(Sprintf("Folder:\t%20s\t(", folder, 1))) eob();
	set_readonly(0);
	delete_line();
	insert(fline);
	set_readonly(1);
     }
   rmail_save_buffer_secure();  % only if modified.
   pop_spot();
   setbuf(cbuf);
}


define rmail_save_folder (folder, delit)
{
   !if (bufferp(Rmail_Folder_Buffer)) return;
   
   variable file = rmail_folder_file(folder), n;
   variable total, n_ok, n_unread;
   
   setbuf(Rmail_Folder_Buffer);
   set_readonly(0);
   n = string(whatline());
   
   % This is a good place to update the master index.
   % calculate total messages and number of deleted ones.
   eob(); bskip_chars(" \t\n");
   total = whatline();
   
   bob();
   n_ok = total;
   n_unread = 0;
   forever 
     {
	if (looking_at("D")) n_ok--; else if (looking_at("-")) n_unread++;
	!if (down(1)) break;
	bol();
     }
   
   rmail_update_master_index(folder, n_ok, total, n_unread);

   widen();
   bob();
   go_down(1);
   bol(); push_mark(); eol(); 
   if (strcmp(bufsubstr(), n))
     {
	delete_line();
	insert(n);
	newline();

     }
   rmail_save_buffer_secure();
   
   if (delit) 
     {
	delbuf(whatbuf());
	return;
     }
   % 
   % narrow it back
   % 
   bob();
   bol_fsearch("\x1F\xC\n"); pop(); go_down(1);
   push_mark(); eob(); narrow();
}

define rmail_find_folder (folder)
{
   variable file = rmail_folder_file(folder);
   rmail_save_folder(Rmail_Folder_Name, 1);
   Rmail_Folder_Buffer = rmail_buffer_name(folder);
   find_file(file); pop();
   rmail_set_buffer_flags();

   rename_buffer(Rmail_Folder_Buffer);
   Rmail_Folder_Name = folder;

   % make sure buffer is properly formatted.  Routines expect this to be
   % widened.
   
   widen();
   bob();
   if (eobp())
     {
	% new, format it
	insert("0\n1\n\x1F\xC\n")
     }   
}


define rmail_get_header(h)
{
   bob();
   !if (strcmp(h, "Date: "))
     {
	% special treatment for date.  It always looks like:
	% Wed, 11 jun 1993  
	% The day and , are somtimes missing and 1993 may be just 93.
	!if (re_fsearch("^Date:[ \t]+[a-zA-Z,]* *0?\\([1-3]?[0-9]\\) +\\([a-zA-Z]+\\) +")) return Null_String;
	return Sprintf("%s-%s", regexp_nth_match(1), regexp_nth_match(2), 2);
     }

   !if (bol_fsearch(h)) return Null_String;
   go_right(strlen(h));
   skip_white();
   push_mark(); eol(); trim(); bufsubstr();
}

   
define rmail_extract_headers()
{
   variable from = Null_String, date = Null_String, subject = Null_String;
   
   push_spot();
   bob(); push_mark();
   push_mark();  skip_chars("\n \t");  del_region(); newline();
   
   !if (bol_fsearch("\n")) eob();
   narrow();
   
   from = rmail_get_header("From ");
   !if (strlen(from)) from = rmail_get_header("From: ");
   
   date = rmail_get_header("Date: ");
   subject = rmail_get_header("Subject: ");
   
   widen();
   pop_spot();
   %                RAFE  (flags, the - means it has not been read.)
   Sprintf(" %6s       %25.25s  %s\n", date, from, subject, 3);
   
}


% This routine assumes that the buffer is narrow. 
define rmail_make_filename()
{
   variable cbuf = whatbuf(), n;
   setbuf(Rmail_Folder_Buffer);
   bob();
   push_mark(); push_mark();
   eol();
   n = bufsubstr();
   del_region();
   if (strlen(n)) n = integer(n); else n = 0;
   ++n;
   n = string(n);
   bob();
   insert(n); 
   if (eobp())
     {
	insert("\n1\n\x1F\xC\n");
     }
   
   setbuf(cbuf);
   return n;
}

   
   

define rmail_create_folder(folder)
{
   variable dir = dircat(Rmail_Directory, folder);
   if (file_status(dir)) return dir;
   !if (mkdir(dir)) error ("Unable to create folder directory.");
   chmod(dir, 0700); pop();
   rmail_update_master_index(folder, 0, 0, 0);
   return (dir);
}


define rmail_output_newmail()
{
   variable headers = rmail_extract_headers();
   variable cbuf = whatbuf();
   variable n = rmail_make_filename(), file;
   
   file = rmail_create_folder (Rmail_Folder_Name);
   file = dircat(file, n);
   bob(); push_mark(); skip_chars("\n \t"); del_region();
   push_mark(); eob();
   write_region_to_file(file); pop();
   setbuf(Rmail_Folder_Buffer);
   eob();
   % If this format is changed, change it in xpunge as well.
   insert(Sprintf("-%3s", n, 1)); insert(headers);
   setbuf(cbuf);
}




define rmail_process_newmail (mbox)
{
   variable n = 0, from = "^\\cFrom ";
   
   rmail_find_folder(Rmail_Newmail_Folder);
   eob(); push_spot();
   setbuf(" *rmail-newmail*");
   erase_buffer();

   if (insert_file(mbox) <= 0) error ("File not inserted.");
   

   bob(); push_mark();
   !if (re_fsearch(from)) error ("New mail with no messages!");
   del_region();
   forever
     {
	push_mark();
	eol();
	if (re_fsearch(from)) go_up(1); else eob();
	narrow();
	++n;
	rmail_output_newmail();
	erase_buffer();
	widen();
	if (eobp()) break;
	del();
     }
   
   % pop back to beginning of the newest messages in the folder.
   % in addition, save_folder expects the buffer to be narrowed.
   whatbuf();   
   setbuf(Rmail_Folder_Buffer);
   bob();
   bol_fsearch("\x1F\xC\n"); pop();
   go_down(1); push_mark(); eob(); narrow();
   pop_spot();
   
   setbuf(());
   
   rmail_save_folder(Rmail_Newmail_Folder, 1);
   return n;
}
   

define rmail_get_newmail()
{
   variable file, n;
   variable nomail, cmd;
   variable exec_file;
   
   nomail = "No new mail.";
   
   file = getenv("MAIL");
   
   !if (strlen(file)) file = dircat(Rmail_Spool_Dir, getenv("USER"));
   
   if (stat_file(file)) 
     {
	message(nomail);
	return (0);
     }
   
   !if (stat_struct("size"))
     {
	message(nomail);
	return (0);
     }
   
   flush("Getting new mail...");
   exec_file = dircat(Jed_Bin_Dir, "getmail");
   
   if (file_status(exec_file) != 1)
     {
	error (strcat (exec_file, " not found!"));
     }
   
   cmd = Sprintf("%s %s %s", exec_file, file, Rmail_Tmp_Mbox, 3);
   push_mark(); pipe_region(cmd); pop();
   
   n = rmail_process_newmail (Rmail_Tmp_Mbox);
   % delete_file(Rmail_Tmp_Mbox);
   return n;
}

define rmail_folder_mode()
{
   variable flags;
   find_file(Rmail_Master_Index_File); pop();
   set_readonly(1);
   rmail_set_buffer_flags();
   rename_buffer(Rmail_Root_Buffer);
   
   bob();
   use_keymap("Rmail-Folder");
}

define rmail_exit_folder ()
{
   rmail_save_folder(Rmail_Folder_Name, 1);
   if (bufferp(Rmail_Root_Buffer)) pop2buf(Rmail_Root_Buffer); 
   else rmail_folder_mode();
   
   onewindow();
   if (bufferp(Rmail_Mail_Buffer_Name))
     {
	delbuf(Rmail_Mail_Buffer_Name);
	Rmail_Message_Number = Null_String;
     }
   Rmail_Mail_Buffer_Name = Null_String;
   message(Null_String);
}

variable Rmail_Headers_Hidden = 0;

define rmail_unhide_headers ()
{
   variable mark = "\x1F\xC\n";

   Rmail_Headers_Hidden = 0;
   set_readonly(0);
   widen();
   bob(); push_mark();
   !if (bol_fsearch("\n")) eob();
   narrow();
   bob();
   
   % make a mark if not found
   if (bol_fsearch(mark))
     {
	go_down(1);  push_spot();
   
	% delete visible headers
	push_mark(); eob(); del_region();

	% restore old ones
	bob(); push_mark(); bol_fsearch(mark); pop();
	bufsubstr(); eob(); insert();
	pop_spot();
     }
   else
     {
	insert(mark);
	push_mark();
	eob();
	bskip_chars("\n");
	bufsubstr();
	bob();
	insert();
	newline();
	go_down(2);
     }
   push_mark(); widen(); eob(); narrow();
   bob(); set_readonly(1);
}



define rmail_hide_headers()
{
   variable ok_headers = "From,Subj,Cc,Organ,In-Reply,Date,Reply-To,To", header;
   variable n;
   
   rmail_unhide_headers();
   Rmail_Headers_Hidden = 1;
   widen(); bob(); push_mark();
   !if (bol_fsearch("\n")) eob();
   narrow();
   bob();
   !if (bol_fsearch("\x1F\xC\n")) error ("Unable to find marker.");
   set_readonly(0);
   push_spot();
   
   n = 0; 
   % mark ok lines
   while (header = extract_element(ok_headers, n, ','), strlen(header))
     {
	n++;
	while (bol_fsearch(header))
	  {
	     insert("\xFF");
	  }
	
	pop_spot();
	push_spot();
     }
   pop_spot();
   eol(); go_right(1);
   % delete unmarked lines
   while(not(eobp()))
     {
	bol();
	if (looking_at("\xFF"))
	  {
	     del();
	     eol(); go_right(1);
	     continue;
	  }
	
	push_mark(); eol(); skip_chars("\n"); del_region();
	eol();
     }
   %newline();
   widen();
   bob();
   bol_fsearch("\x1F\xC\n"); pop();
   go_down(1);
   push_mark();  eob();   narrow();
   bob();
}


% This routines returns the file number of current summary line as a string.
% If the line is invalid, it returns the null string.
define rmail_extract_file_number()
{
   bol();
   go_right(1); skip_white();
   push_mark();
   skip_chars("0-9");
   bufsubstr();
}

define rmail_add_flag(flag)
{
   variable n = 13, flags = "RFE";
   
   flag = char(flag);
   n += is_substr(flags, flag);
   goto_column(n);
   !if (looking_at(flag))
     {
	insert(flag);
	del();
     }
}


% this routine returns 0 if line does not contain a valid message
% 1 if the message is already in the window, 2 if not but it exists, -1
% if it had to be read in.
define rmail_get_message(flag)
{
   variable buf, file, n, n1, pop_buf, ret;
   
   !if (bufferp(Rmail_Folder_Buffer)) return (0);
   pop2buf(Rmail_Folder_Buffer);
   
   file = rmail_extract_file_number();
   !if (strlen(file)) return 0;
   Rmail_Message_Number = file;
   set_readonly(0);
   bol(); if (looking_at("-")) 
     {
	del(); insert_single_space ();
     }
   
   if (flag) rmail_add_flag(flag);
   
   set_readonly(1);
   
   buf = Sprintf("Rmail: %s(%s)", Rmail_Folder_Name, file, 2);
   pop_buf = buf;
   ret = 2;

   if (strcmp(buf, Rmail_Mail_Buffer_Name) or not(bufferp(buf)))
     {
	%
	% we have to read it in.
	%
	ret = -1;
	if (bufferp(Rmail_Mail_Buffer_Name)) 
	  {
	     pop_buf = Rmail_Mail_Buffer_Name;
	  }
     }
   
   if (buffer_visible(pop_buf))
     {
	pop2buf(pop_buf);
	if (ret == 2) ret = 1;
     }
   else
     {
	% I want summary window at top. There is no nice way to do it
	% at present.
	onewindow();
	n = window_info('r');
	pop2buf(pop_buf);
	if (TOP_WINDOW_ROW == window_info('t')) 
	  {
	     pop2buf(Rmail_Folder_Buffer);
	     sw2buf(pop_buf);
	  }
	n1 = window_info('r');	
	%  we want n to be 4 lines
	%  now it is:
	n -= n1 + 1;
	loop (n - 4) enlargewin();
	pop2buf(Rmail_Folder_Buffer);
	pop2buf(pop_buf);
     }
   
   if (ret != -1) return ret;

   set_readonly(0);
   widen();
   erase_buffer();
   Rmail_Mail_Buffer_Name = buf;
   file = dircat(dircat(Rmail_Directory, Rmail_Folder_Name), file);
   n = insert_file(file);
   
   ERROR_BLOCK
     {
	set_readonly(1);
	set_buffer_modified_flag(0);
     }
   
   if (n < 0)
     {
	bob ();
	push_mark ();
	eob ();
	write_region_to_file (file); pop ();
	error ("File not found!");
     }
   

   bob();
   rename_buffer(buf);
   rmail_hide_headers();
   
   EXECUTE_ERROR_BLOCK;
   
   return (ret);
}

  
define rmail_toggle_headers()
{
   !if (rmail_get_message(0)) return;

   if (Rmail_Headers_Hidden)
     {
	rmail_unhide_headers();
     }
   else rmail_hide_headers();
   set_buffer_modified_flag(0);
   set_readonly(1);
   pop2buf(Rmail_Folder_Buffer);
}

define rmail_scroll_forward()
{
   ERROR_BLOCK 
     {
	pop2buf(Rmail_Folder_Buffer);
     }
   if (rmail_get_message(0) == 1) 
     {
	update(0);
	call("page_down");
     }

   EXECUTE_ERROR_BLOCK;
}

define rmail_scroll_backward()
{
   ERROR_BLOCK 
     {
	   pop2buf(Rmail_Folder_Buffer);
     }
   if (rmail_get_message(0) == 1) 
     {
	update(0);
	call("page_up");
     }

   EXECUTE_ERROR_BLOCK;
}

define rmail_forward_message()
{
   variable cbuf;
   !if (rmail_get_message('F'))
     {
	beep();
	return;
     }
   
   cbuf = whatbuf();
   mail();
   eob();
   insbuf(cbuf);
   bob(); eol();
}

define rmail_complex_get_from (from)
{
   variable f;
   f = strtrim(str_uncomment_string (regexp_nth_match (1), "\"'(", "\"')"));
   
   !if (strlen(f)) return from;
   
   % add to list 
   if (strlen(from))
     {
	!if (is_list_element(Rmail_Dont_Reply_To, f, ','))
	  from = strcat (strcat (from, ","),  f);
	f = from;
     }
   f;
}

define rmail_reply()
{
   !if (rmail_get_message('R'))
     {
	beep();
	return;
     }
   
   variable subj = Null_String, from = Null_String, cbuf = whatbuf();
   variable cc = Null_String;
   

   push_spot();
   bob();  push_mark();
   !if (bol_fsearch("\n")) eob();
   narrow();

   
   if (orelse 
	 {re_bsearch("^\\CFrom:.*<\\(.+\\)>");}
	 {re_bsearch("^\\CReply-To: *\\([^ ]+\\) *");}
	 {re_bsearch("^\\CFrom:.*<\\(.+\\)>");}
	 {re_bsearch("^\\CFrom: *\\([^ ]+\\) *");}
	 {re_bsearch("^\\cFrom +\\([^ ]+\\) *");}
       )
     {
	from = rmail_complex_get_from(from);
     }
   
   eob();
   
   if (re_bsearch("^Subject: *\\(.*\\)$"))
     {
	subj = regexp_nth_match(1);
     }
   bob();
   
   if (orelse
	 {re_fsearch("^\\CCC:.*<\\(.+\\)>");}
	 {re_fsearch("^\\CCC: *\\(.*\\)$");}
       )
     {
	cc = rmail_complex_get_from(cc);
     }

   bob();
   
   if (orelse 
	 {re_fsearch("^\\CTo:.*<\\(.+\\)>");}
	 {re_fsearch("^\\CTo: *\\(.*\\)$");}
       )
     {
	from = rmail_complex_get_from(from);
     }
   
   widen();
   pop_spot();
   mail();
   onewindow();
   
   if (buffer_modified ())
     {
	!if (get_yes_no("Mail already being composed.  Erase it")) return;
     }

   mail_format_buffer ();
   bob();
   do
     {
	if (looking_at("To:")) 
	  {
	     eol ();
	     insert(from);
	  }
	else if (looking_at("Cc:")) 
	  {
	     eol (); 
	     insert (cc);
	  }
	else if (looking_at("Subject:"))
	  {
	     eol ();
	     insert (subj);
	     bol ();
	     ffind(":"); pop(); go_right(1);  skip_white();
	     !if (looking_at("RE:")) insert ("RE: ");
	  }
     }
   while ((down(1), bol()) and not (looking_at("-")));
   
   eob();
   push_mark();
   narrow();
   % This trick allows a single undo to remove the later insertion:
   erase_buffer();
   
   insbuf(cbuf);
   bob();
   do
     {
	bol();
	insert(">");
     }
   while (down(1));
   newline();
   bob();
   widen();
   
   !if (strlen(from)) 
     {
	bob(); eol();
     }
}


define rmail_find_next_message()
{
   push_mark();
   while (down(1), bol(), () and looking_at("D"));
   if (eobp())
     {
	pop_mark(1);
	message("No more undeleted messages.");
	return(0);
     }
   pop_mark(0);
   1;
}

define rmail_find_prev_message()
{
   variable d = "D";
   push_mark();
   while (up(1), bol(), () and looking_at(d));
   if (bobp() and looking_at(d))
     {
	pop_mark(1);
	message("No more undeleted messages.");
	return(0);
     }
   pop_mark(0);
   1;
}

define rmail_delete_forward()
{
   bol();
   if (eolp()) return beep();
   !if (looking_at("D"))
     {
	set_readonly(0);
	del(); insert("D");
	set_readonly(1);
     }
   rmail_find_next_message(); pop();
}

define rmail_undelete ()
{
   push_mark();
   eol();
   if (bol_bsearch("D"))
     {
	set_readonly(0);
	del(); insert_single_space ();
	set_readonly(1);
	pop_mark(0);
	return;
     }
   pop_mark(1);
   beep();
}

define rmail_next_message()
{
   if (rmail_find_next_message())
     {
	rmail_scroll_forward();
     }
}

define rmail_prev_message()
{
   if (rmail_find_prev_message())
     {
	rmail_scroll_forward();
     }
}

define rmail_select_folder (folder)
{
   variable n;
   rmail_find_folder(folder);
   bob();
   go_down(1);
   bol(); push_mark(); eol(); n = integer(bufsubstr());
   bol_fsearch("\x1F\xC\n"); pop;
   go_down (1);
   push_mark();
   eob();
   narrow();
   pop2buf(whatbuf());
   goto_line(n); bol();
   use_keymap("Rmail-Read");
   set_status_line("Jed %b Folder.      (%p)  %t", 0);
   set_readonly(1);
   runhooks ("rmail_folder_hook");
}

define rmail_folder_newmail()
{
   variable n;
   
   if (-1 != prefix_argument (-1))
     {
	variable file = read_file_from_mini ("Mailbox:");
	!if (strlen (file)) return;
   
	n = rmail_process_newmail (file);
     }
   else 
     {
	n = rmail_get_newmail();
     }
   
   if (n)
     {
	flush(Sprintf("%d new messages.", n, 1));
	rmail_select_folder(Rmail_Newmail_Folder);
     }
   else flush("No new mail.");
}


variable Rmail_Last_Folder = Null_String;


% This routine deletes buffer containing message n if it exists
% it returns 1 if is exists and is visible otherwise it returns 0.
define rmail_validate_message ()
{
   variable vis = 0;
   variable buf = Sprintf("Rmail: %s(%s)", Rmail_Folder_Name, Rmail_Message_Number, 2);
   
   if (bufferp(buf))
     {
	vis = buffer_visible(buf);
	delbuf(buf);
     }
   return vis;
}

define rmail_update_folder_and_save ()
{
   variable new_n;
   
   eob ();
   bskip_chars ("\n \t");
   bol ();
   new_n = rmail_extract_file_number ();
   !if (strlen(new_n)) new_n = "1";
   widen(); 
   
   bob(); push_mark(); eol(); 
   if (strcmp(bufsubstr(), new_n))
     {
	delete_line();
	insert(new_n);
	newline();
     }
   bol_fsearch("\x1F\xC\n"); pop();
   go_down(1); push_mark(); eob(); narrow();
   
   pop_mark(1);
   rmail_save_folder(Rmail_Folder_Name, 0);
}


define rmail_resequence_folder ()
{
   variable n, file, new_file, dir, new_n;
   variable update_message = 0;

   ERROR_BLOCK
     {
	rmail_update_folder_and_save ();
	pop_spot ();
     }

   push_spot();
   set_readonly(0);
   flush("Resequencing folder...");
   dir = dircat(Rmail_Directory, Rmail_Folder_Name);

   n = 0;
   bob();   
   while (file = rmail_extract_file_number (), strlen(file))
     {
	n++;
	new_n = string(n); 
	if (strcmp(file, new_n))
	  {
	     file = dircat(dir, file);
	     new_file = dircat(dir, new_n);
	     if (file_status (new_file)) error (strcat("Error renaming ", file));
	     if (rename_file(file, new_file)) error (strcat("Error renaming ", file));
	     bol();
	     go_right(1);
	     push_mark();
	     skip_white();
	     skip_chars("0-9");
	     del_region();
	     insert(Sprintf("%3s", new_n, 1));
	     !if (strcmp(new_n, Rmail_Message_Number))
	       {
		 update_message = rmail_validate_message ();
	       }
	  }

	!if (down(1)) break;
     }

   % update the last file number
   rmail_update_folder_and_save ();
   
   pop_spot();
   eol (); bskip_chars("\n"); bol ();
   set_readonly(1);
   if (update_message or (integer(new_n) < integer(Rmail_Message_Number)))
     {
	Rmail_Message_Number = Null_String;
	rmail_scroll_forward ();
     }
   
   message("done.");
}

define rmail_output_to_folder()
{
   variable folder, header, old_n, new_n, new_file, old_file;
   variable old_folder = Rmail_Folder_Name;
   variable buf, vis;
   variable new_dir, old_dir = dircat(Rmail_Directory, Rmail_Folder_Name);
   
   old_n = rmail_extract_file_number ();
   !if (strlen(old_n)) return;
   
   folder = read_mini("Move to folder:", Rmail_Last_Folder, Null_String);
   !if (strlen(folder)) return;
   !if (strcmp(folder, Rmail_Folder_Name)) return;
   
   new_dir = dircat(Rmail_Directory, folder);
   if (file_status(new_dir) != 2)
     {
	if (1 != get_yes_no(Sprintf("Folder %s does not exist, create it", folder, 1)))
	  {
	     return;
	  }
	rmail_create_folder(folder); pop();
     }
   
   % lets get this header
   bol(); push_mark(); eol(); header = bufsubstr();
   
   rmail_find_folder (folder);

   %
   % generate a new filename
   %
   new_n = rmail_make_filename();
   new_file = dircat(new_dir, new_n);
   old_file = dircat(old_dir, old_n);
   
   if (rename_file(old_file, new_file))
     {
	rmail_select_folder(old_folder);
	error("Unable to rename file.");
     }
   eob();
   insert(header);
   bol();
   go_right(1);
   push_mark();
   skip_white();
   skip_chars("0-9");
   del_region();
   insert(Sprintf("%3s", new_n, 1));
   Rmail_Last_Folder = folder;
   
   % Now narrow it so next routine can process it
   bob();
   bol_fsearch("\x1F\xC\n"); pop;
   go_down (1);
   push_mark();
   eob();
   narrow();

   rmail_select_folder(old_folder);
   set_readonly(0);
   bob(); 
   bol_fsearch(header); pop();
   delete_line();
   rmail_resequence_folder();
}


% This function is assumed to be called from within a folder.  It REALLY
% deletes files marked with the D.

define rmail_xpunge_deletions ()
{
   variable file, dir, n, new_file;
   % variable old_n = Null_String;
   flush("Expunging messages...");
   push_mark();
   ERROR_BLOCK
     {
	rmail_resequence_folder ();
	pop_mark(0);
	set_readonly(1);
     }
   set_readonly(0);
   dir = dircat(Rmail_Directory, Rmail_Folder_Name);
   bob();
   while (bol_fsearch("D"))
     {
	file = rmail_extract_file_number();
	!if (strlen(file)) continue;
	% !if (strcmp(file, Rmail_Message_Number))
	%  {
	%     old_n = file;
	%  }
	
	file = dircat(dir, file);
	if (file_status(file) == 1)
	  {
	     !if (delete_file(file))
	       {
		  error (strcat("Unable to delete ", file));
	       }
	     delete_line();
	  }
	else 
	  {
	     error(strcat("Unable to access ", file));
	  }
     }
   pop_mark(1);
   rmail_resequence_folder ();
}


$1 = "Rmail-Read";
!if (keymap_p($1))
{
   make_keymap($1);
   _for (' ', 127, 1) 
     {
	$2 = char(());
	undefinekey($2, $1);
     }
   definekey("rmail_delete_forward", "D", $1);
   definekey("rmail_delete_forward", "d", $1);
   definekey("rmail_exit_folder", "q", $1);
   definekey("rmail_exit_folder", "Q", $1);
   definekey("rmail_folder_newmail", "g", $1);
   definekey("rmail_folder_newmail", "G", $1);
   definekey("rmail_next_message", "n", $1);
   definekey("rmail_next_message", "N", $1);
   definekey("rmail_prev_message", "p", $1);
   definekey("rmail_prev_message", "P", $1);
   definekey("rmail_toggle_headers", "t", $1);
   definekey("rmail_toggle_headers", "T", $1);
   definekey("rmail_scroll_forward", " ", $1);
   definekey("rmail_scroll_forward", "\r", $1);
   definekey("rmail_scroll_backward", "^?", $1);
   definekey("rmail_xpunge_deletions", "x", $1);
   definekey("rmail_xpunge_deletions", "X", $1);
   definekey("rmail_reply", "r", $1);
   definekey("rmail_reply", "R", $1);
   definekey("rmail_forward_message", "f", $1);
   definekey("rmail_forward_message", "F", $1);
   definekey("rmail_output_to_folder", "o", $1);
   definekey("rmail_undelete", "u", $1);
   definekey("rmail_undelete", "U", $1);
   definekey("mail", "m", $1);
   definekey("mail", "M", $1);
}
   



define rmail_select_this_folder ()
{
   bol();
   !if (looking_at("Folder:\t")) return beep();
   ffind("\t"); pop();
   skip_white();
   push_mark(); 
   !if (ffind("\t("))
     {
	pop_mark();
	beep();
	return;
     }
   
   rmail_select_folder (bufsubstr());
}

	




$1 = "Rmail-Folder";
!if (keymap_p($1))
{
   make_keymap($1);
   _for (' ', 127, 1) 
     {
	$2 = char(());
	undefinekey($2, $1);
     }
   definekey("rmail_folder_newmail", "g", $1);
   definekey("rmail_folder_newmail", "G", $1);
   definekey("rmail_quit_rmail", "q", $1);
   definekey("rmail_quit_rmail", "Q", $1);
   definekey("rmail_select_this_folder", " ", $1);
   definekey("rmail_select_this_folder", "\r", $1);
   definekey("mail", "m", $1);
   definekey("mail", "M", $1);
}

% if parameter is 0, unlock it if it is ours.  Otherwise, lock or steal it.
% it returns 1 if the lock was made, 0 if not.
define rmail_check_lock_file(lck)
{
   variable lpid, this_pid = string(getpid()), do_del;
   variable lock_file = dircat (Rmail_Directory, Rmail_Lock_File_Name);
   variable ret = 0, write_it = 0, lbuf;
   variable errbuf;
   
   read_file(lock_file); pop();  message (Null_String);
   lbuf = whatbuf();
   
   ERROR_BLOCK
     {
	set_buffer_modified_flag(0);
	delbuf(lbuf);
     }
   
   % extract the pid, it will be null if file was created
   bob(); push_mark(); eol(); lpid = bufsubstr();
   delete_line();
   insert(this_pid);
   
   if (lck)
     {
	% check existence of pid
	if (unix_kill(integer (lpid), 0)) lpid = Null_String;
   
	if (orelse 
	      {not (strlen(lpid)) or not(strcmp(lpid, this_pid))}
	      {error (Sprintf("Pid %s is locking mail directory.  Sorry.", lpid, 1));}
	    )
	  {
	     ret = 1;
	     write_it = 1;
	  }
     }
   else !if (strcmp(lpid, this_pid))
     {
	delete_file(lock_file); pop();
     }
   
   % get_yes_no WILL change the buffer.  This is because read_file was used 
   % which is effectively a setbuf.  get_yes_no calls update which restores
   % top level buffer.
   
   setbuf(lbuf);
   if (write_it) save_buffer();
   EXECUTE_ERROR_BLOCK;
   return (ret);
}

define rmail_quit_rmail()
{
   rmail_exit_folder();
   % This puts us at the root again so save and get out.
   rmail_save_buffer_secure();
   delbuf(Rmail_Root_Buffer);
   rmail_check_lock_file(0); pop();   % deletes the lock file.
}
	

define rmail ()
{
   variable n;
   !if (rmail_check_lock_file(1)) return;

   rmail_save_folder(Rmail_Folder_Name, 1);
   Rmail_Folder_Buffer = Null_String;
   
   rmail_folder_mode();
	
   n = rmail_get_newmail();
   if (n)
     {
	flush(Sprintf("%d new messages.", n, 1));
	rmail_select_folder(Rmail_Newmail_Folder);
     }
   else
     {
	message("No new mail.");
     }
}

   
  
