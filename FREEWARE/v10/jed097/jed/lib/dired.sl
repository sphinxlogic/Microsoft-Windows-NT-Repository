%
%  Simple dired mode for JED.
%
%
% To invoke Dired, do `C-x d' or `M-x dired'.
%
%  Moving around
%  =============
%
%  All the usual motion commands plus some extras:
%
%  `C-n' `n' SPC
%       move point to the next line (at the beginning of the file name)
%
%  `C-p' `p'
%       move point to the previous line (at the beginning of the file name)
%
%  DEL
%       move up and unflag
%
%  `^K'
%	dired_kill_line - removes a line from the dired buffer
%
%  ==============
%
%     The primary use of Dired is to "flag" files for deletion and then
%  delete the previously flagged files.
%
%  `d'
%       Flag this file for deletion.
%
%  `u'
%       Remove deletion flag on this line.
%
%  `DEL'
%       Move point to previous line and remove the deletion flag on that
%       line.
%
%  `~'
%       Flag all backup files (files whose names end with `~') for deletion
%
%  `g'
%       Update the entire contents of the Dired buffer
%
%  Deleting Files
%  ==============
%
%  `x'
%       expunge all flagged files.  Displays a list of all the file names
%       flagged for deletion, and requests confirmation with `yes'.
%       After confirmation, all the flagged files are deleted and then
%       their lines are deleted from the Dired buffer.
%
%  File Manipulations
%  ==================
%
%  `f'
%       Visit the file described on the current line, like typing `C-x C-f'
%       and supplying that file name
%
%  `v'
%       View the file described on the current line
%
%  `r'
%       rename - prompts for a new name for the file on the current line
%
%  'm'
%       move - move a group of flagged files to an new directory
%
% `M-x dired_search'
%       use fsearch with the contents of LAST_SEARCH to perform a search 
%       through the files listed in the dired buffer from the current point 
%       forward.  Since it stops in the file where the search string is
%       encountered, it is structured so that `M-x dired_search' from the 
%       visited file will revert back to the dired buffer and continue the 
%       dired_search from the next file in the list.
%

.  "DiredMap" keymap_p 
.     {
.	"DiredMap" make_keymap
.	"dired_find"		"f"	"DiredMap"	definekey
.	"dired_find"		"F"	"DiredMap"	definekey
.	"dired_view"		"v"	"DiredMap"	definekey
.	"dired_view"		"V"	"DiredMap"	definekey
.	"dired_tag"		"d"	"DiredMap"	definekey
.	"dired_tag"		"D"	"DiredMap"	definekey
.	"dired_untag"		"u"	"DiredMap"	definekey
.	"dired_untag"		"U"	"DiredMap"	definekey
.	"dired_move"		"M"	"DiredMap"	definekey
.	"dired_move"		"m"	"DiredMap"	definekey
.	"dired_delete"		"X"	"DiredMap"	definekey
.	"dired_delete"		"x"	"DiredMap"	definekey
.	"dired_next_file"	"^N"	"DiredMap"	definekey
.	"dired_next_file"	"N"	"DiredMap"	definekey
.	"dired_next_file"	"n"	"DiredMap"	definekey
.	"dired_next_file"	" "	"DiredMap"	definekey
.	"dired_prev_file"	"^P"	"DiredMap"	definekey
.	"dired_prev_file"	"P"	"DiredMap"	definekey
.	"dired_prev_file"	"p"	"DiredMap"	definekey
#ifdef MSDOS
.	"dired_untag_last"	"^@S"	"DiredMap"	definekey
#endif   
#ifdef UNIX
.	"dired_untag_last"	"^?"	"DiredMap"	definekey % DEL key
#endif   
.	"dired_flag_backup"	"~"	"DiredMap"	definekey
undefinekey ("^K", "DiredMap");
.	"dired_kill_line"	"^K"	"DiredMap"	definekey
.	"dired_rename"		"R"	"DiredMap"	definekey
.	"dired_rename"		"r"	"DiredMap"	definekey
.	"dired_reread_dir"	"g"	"DiredMap"	definekey
.	"dired_reread_dir"	"G"	"DiredMap"	definekey
.	"describe_mode"		"H"	"DiredMap"	definekey
.	"describe_mode"		"h"	"DiredMap"	definekey
.	"dired_quick_help"	"?"	"DiredMap"	definekey
.     } !if


. [Dired_Buffer] "*dired*" =Dired_Buffer
. [Dired_Current_Directory]

#ifdef VMS
% takes somthing like dev:[a.b.c] and converts it to dev:[a.b]c.dir
. ( [dir] =dir
.   [cdir last this]  Null_String =cdir  Null_String =last
.   [n file ch]
.   dir strlen =n
.   dir n 1 substr int ']' != {dir return} if
  
.   { --n n {break} !if
.     dir n 1 substr int =ch
.     ch '.' == ch '[' == or {break} if
.   } forever
.   ch '[' == 
.     {
.        dir 1 n substr 
.        "000000]" strcat
.     }
.     {
.        dir 1 n 1 - substr
.        "]" strcat
.     } 
.    else =cdir
   
.   dir n 1 + dir strlen substr =file
.   file 1 file strlen 1 - substr ".dir" strcat =file
.   cdir file strcat
. ) dired_convert_vms_dir
#endif


. (
.   [dir] =dir
.   [ls flags]
.   [kmap] "DiredMap" =kmap
.   [spaces] "  " =spaces

  %
  %  put dir in canonical form--- using trick
  %
#ifdef MSDOS OS2
.  dir expand_filename =dir
#ifdef MSDOS
.  dir msdos_fixup_dirspec =dir
#endif
#endif

#ifdef VMS
.   dir dired_convert_vms_dir =dir
#endif
.   dir file_status 2 == {
.      dir kmap dircat 
.      parse_filename pop =Dired_Current_Directory
#ifdef VMS
.      Dired_Current_Directory =dir
#endif
.   }{
.      dir parse_filename =dir =Dired_Current_Directory
.   } else
.   Dired_Current_Directory change_default_dir {"Failed to chdir." error} if

.   Dired_Buffer sw2buf 
.   getbuf_info =flags exch pop Dired_Current_Directory exch flags setbuf_info 
  
.   " DIRED: %b   (%m%n)  (%p)  |  press '?' for help." 0 set_status_line
.   0 set_readonly erase_buffer
.   kmap use_keymap
.   "dired" 0 setmode  
#ifdef UNIX
.   "ls -al "  =ls
#endif
#ifdef MSDOS
.   "dir/l/ogne " =ls
#endif
#ifdef OS2
.   "dir /n " =ls
#endif
#ifdef VMS
.   "directory/size/date/prot/notrail " =ls
#endif

%"Getting listing..." flush
.   ls dir strcat shell_cmd
.   bob
#ifdef OS2
   % kill 4 lines of header junk and 2 lines of trailer junk
.   push_mark 4 down pop
.   eol what_column 1 == {1 down pop} if
.   bol del_region
.   eob push_mark 3 up pop eol del_region
.   bob
#endif
#ifdef MSDOS
.   push_mark 4 down pop bol del_region		% kill 4 lines of header junk
.   eob "bytes free" bsearch {bol push_mark eob del_region} if
.   bob
#endif
#ifdef VMS
.   push_mark 3 down pop bol del_region
#endif

.   {bol spaces insert 1 down}{} while
.   bob
#ifdef UNIX
.   "  total " looking_at
.     {push_mark eol 1 right pop del_region} if
#endif
.   "This Directory: " insert Dired_Current_Directory insert newline     
.   0 set_buffer_modified_flag 1 set_readonly
.   spaces message
. ) dired_read_dir

. [Dired_Quick_Help]
. "d:tag file, u:untag, x:delete tagged files, r:rename, h:more help, ?:this help"
. =Dired_Quick_Help

. (
.   Dired_Quick_Help message
. ) dired_quick_help


. ( Dired_Current_Directory dired_read_dir
. ) dired_reread_dir

. (
#ifdef UNIX OS2
.    eol " " bfind right pop
#endif
#ifdef VMS MSDOS
.    bol 2 right pop
#endif
. )  dired_point_file
  
. (
.   1 down pop
.   dired_point_file
. )  dired_next_file
  
. (
.   1 up pop
.   dired_point_file
. )  dired_prev_file

#ifndef VMS
. (
.   whatline 1 == {	  % don't remove top line!
.     0 set_readonly
.     bol push_mark 1 down pop del_region
.     0 set_buffer_modified_flag 1 set_readonly
.   } !if
. ) dired_kill_line
#endif

. (
.   0 set_readonly
#ifdef UNIX
.   bol "This " looking_at
.   "  d" looking_at	or	  % do not tag a directory
#endif
#ifdef MSDOS OS2
.   bol "This " looking_at
.   "<DIR>" ffind or		  % do not tag a directory
#endif
#ifdef VMS
.   bol ".DIR" ffind
.   bol skip_white what_column 4 > or
#endif
.   bol skip_white eolp or
.   {
.     bol 'D' insert_char del
.   } !if
.   1 down pop bol
.   dired_point_file
.   0 set_buffer_modified_flag 1 set_readonly
. ) dired_tag

. (
.   0 set_readonly
.   bol 
.   'D' looking_at_char {insert_single_space del} if
.   1 down pop bol
.   dired_point_file
.   0 set_buffer_modified_flag 1 set_readonly
. ) dired_untag


. (
.   dired_point_file
.   push_mark
#ifdef MSDOS
.   12 goto_column " " bskip_chars bufsubstr 
.   skip_white 
.   what_column 14 > { "." strcat push_mark " " ffind pop bufsubstr strcat} !if
#endif
#ifdef UNIX OS2
.   eol bufsubstr
#endif
#ifdef VMS
.   ";" ffind pop ";0-9" skip_chars bufsubstr
#endif  
. ) dired_extract_file



% perform operation on tagged files--- 4 parameters
. (
.   [op_function] =op_function
.   [msg_str] =msg_str
.   [op_string] =op_string
  
.   [lbuf]  " *Deletions*" =lbuf
.   [stack n fails]  Null_String =fails
.   [file]
  
.   Dired_Buffer setbuf push_spot
.   bob
  
.   _stkdepth =stack
.   { _stkdepth stack - {pop} loop
.     Dired_Buffer sw2buf
.     0 set_readonly
.     bob
.     { "D%" bol_fsearch}{del del "D " insert} while
.     pop_spot
.     0 set_buffer_modified_flag 1 set_readonly
.   } ERROR_BLOCK
  
.   0 set_readonly

.   { "D" bol_fsearch }
.   {
.     "D%" insert del del
.     dired_extract_file
.   } while

.   _stkdepth stack - =n
.   n {"No tags!" error} !if
  
.   lbuf sw2buf erase_buffer
.   n { insert newline } loop    % tagged files on stack
.   bob
.   buffer_format_in_columns
.   op_string get_yes_no {
.     Dired_Buffer sw2buf bob
.     {"D%" bol_fsearch}{
.       dired_extract_file =file 
. 	bol
.       Dired_Current_Directory file dircat =file
. 	file op_function {
. 	  msg_str file strcat flush
. 	  push_mark
. 	  eol 1 right pop
. 	  del_region 1 left pop
. 	}{
. 	  fails " " strcat file strcat =fails
. 	  del del "  " insert
.       } else
.     } while
.   } if
.   EXECUTE_ERROR_BLOCK
.   fails strlen {"Operation Failed:" fails strcat message } if
. ) dired_xop_tagged_files

. (
.   "Delete these files" "Deleted " &delete_file dired_xop_tagged_files
. ) dired_delete

. [Dired_Move_Target_Dir]

. (
.   [file] =file
.   [name] file extract_filename =name
  
.   file Dired_Move_Target_Dir name dircat rename_file not
. ) dired_do_move

. (
.   [dir]
.   "Move to dir" read_file_from_mini =dir
.   dir file_status 2 != {"Expecting directory name" error } if
.   dir =Dired_Move_Target_Dir

.   "Move these to " dir strcat "Moved " &dired_do_move dired_xop_tagged_files
. ) dired_move

. (
.   1 up {"Top of Buffer."  error} !if
.   dired_untag
.   1 up pop 
.   dired_point_file
. ) dired_untag_last

#ifndef VMS
. (
.   [f]
.   push_spot bob
.   0 set_readonly
.   {"~" fsearch} {
.      dired_extract_file =f
.      f "~" f strlen string_match {
.         bol 'D' insert_char del	       % is a backup file
.      } if
.      eol
.   } while
.   pop_spot
.   0 set_buffer_modified_flag 1 set_readonly
. ) dired_flag_backup
#endif


. (
.   [oldf f n nf nd gone od status]
.   dired_extract_file =oldf
.   "Rename " oldf strcat " to" strcat read_file_from_mini =n
  %
  %  Lets be smart about this.  If new name is a dir, move it to the dir
  %  with oldname.  If file is not a directory and exists, signal error.
  %
.   n file_status =status
.   status 1 == { "File already exists.  Not renamed." error} if
.   status 2 == { n oldf dircat =n } if

  %
  %  Check to see if rename to new directory  -- gone = 0 if so.
  %
.   n parse_filename =nf =nd
.   Dired_Current_Directory oldf dircat =f 
.   f parse_filename pop =od
#ifdef UNIX
.   od nd strcmp =gone
#endif
#ifdef VMS MSDOS OS2
.   od strup nd strup strcmp =gone
#endif
  
.   f n rename_file
.     { "Operation Failed!" error } if
.   0 set_readonly
.   gone {
.     bol push_mark eol 1 right pop del_region
.   }{
#ifdef MSDOS
.	 [len]	   
.        nf strlen =len
.        12 len - =len
.        dired_point_file
.        push_mark 12 right pop del_region
.        nf insert
.	 1 len 1 {" " insert} _for
.        1 down pop
#else
.        dired_point_file 
.        push_mark oldf strlen right pop del_region
.        nf insert 1 down pop
#endif
.    } else
.    dired_point_file
.    0 set_buffer_modified_flag 1 set_readonly
. ) dired_rename

%!%Mode designed for maintaining and editing a directory.
%!%
%!%To invoke Dired, do `M-x dired' or `C-x d' (emacs)
%!%
%!%Dired will prompt for a directory name and get a listing of files in the
%!%requested directory.
%!%
%!%The primary use of Dired is to "flag" files for deletion and then delete
%!%the previously flagged files.
%!%
%!%'d'		Flag this file for deletion.
%!%'u'		Remove deletion flag on this line.
%!%DEL		Move point to previous line and remove deletion flag.
%!%'~'		Flag all backup files for deletion.
%!%
%!%'x'		eXpunge all flagged files.  Dired will show a list of the
%!%	files tagged for deletion and ask for confirmation before actually 
%!%	deleting the files.
%!%
%!%'r'		Rename file on the current line; prompts for a newname
%!%'m'		Move tagged files to a new dir; prompts for dir name
%!%
%!%`g'		Update the entire contents of the Dired buffer
%!%
%!%`f'		Visit the file described on the current line, like typing
%!%	`M-x find_file' and supplying that file name.  If current line is a
%!%	directory, runs dired on the directory and the old buffer is killed.
%!%
%!%`v'		View the file described on the current line in MOST mode.
%!%
%!%`M-x dired_search'
%!%	use fsearch to perform a search through the files listed in the
%!%	dired buffer from the current point forward.  `M-x dired_search' 
%!%	from the visited file will revert to the dired buffer and continue 
%!%	the search from the next file in the list.
%!%
%!%all the usual motion commands plus some extras:
%!%
%!%`C-n' `n' SPC
%!%	move point to the next line (at the beginning of the file name)
%!%
%!%`C-p' `p'
%!%	move point to the previous line (at the beginning of the file name)
%!%
%!%`M-x dired_kill_line'	`^K' (emacs) 
%!%	removes a line from the dired buffer
define dired ()
{
.   "Directory:"  read_file_from_mini dired_read_dir
.   dired_quick_help
.   "dired_hook" runhooks   
}


. (
.   bol bobp 2 right pop eobp or {return} if
#ifdef UNIX
.   'd' looking_at_char
#endif
#ifdef MSDOS OS2
.   "<DIR>" ffind
#endif
#ifdef VMS
.   ".DIR" ffind
#endif
.     {
.       Dired_Current_Directory
.       dired_extract_file
.       dircat dired_read_dir
.       return
.     } if
.   Dired_Current_Directory  
.   dired_extract_file
.   dircat read_file {"Unable to read file." error } !if
.   whatbuf pop2buf
. ) dired_find

. (
.   bol bobp
#ifdef UNIX
.   2 right pop 'd' looking_at_char
#endif
#ifdef MSDOS OS2
.   "<DIR>" ffind
#endif
#ifdef VMS
.   ".DIR" ffind
#endif
.   or {return} if
.   Dired_Current_Directory
.   dired_extract_file
.   dircat read_file {"Unable to read file." error } !if
.   whatbuf pop2buf
.   most_mode
. ) dired_view


#ifndef VMS
. (
.   [obuf]
.   LAST_SEARCH looking_at { 1 right } if
.   LAST_SEARCH fsearch {
.      1				% found - return 1
.   }{
.      buffer_modified { "buffer has been modified" message
.        1				% cannot continue - return 1
.      }{
.        whatbuf =obuf
.        Dired_Buffer pop2buf
.        obuf delbuf
.	 0				% okay - return 0
.      } else
.   } else
. ) dired_search_files

. (
.   [dirstr]
.   Dired_Buffer bufferp {
.      LAST_SEARCH =dirstr
.      Dired_Buffer whatbuf strcmp {	% continue last search
.         dirstr strlen {"No specified search string" message return} !if
.         dired_search_files {return} if
.         1 down pop
.      }{
.         Dired_Buffer pop2buf
.         "dired_search:" LAST_SEARCH Null_String read_mini =dirstr
.         dirstr strlen {"Specify search string" message return} !if
.         dirstr save_search_string
.      } else
.      {
.	  bol
#ifdef UNIX
.         "  d" looking_at
#endif
#ifdef MSDOS OS2
.         "<DIR>" ffind
#endif
#ifdef VMS
.         ".DIR"  ffind
#endif
.         { 
.            Dired_Current_Directory 
.            dired_extract_file
.            dircat read_file {"Unable to read file." error } !if
.            dired_search_files { whatbuf pop2buf return} if
.         } !if				%  don't attempt directories !
.         1 down {return} !if
.      } forever
.   } if
. ) dired_search
#endif
