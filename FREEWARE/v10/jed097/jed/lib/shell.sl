%% system specific routines


variable Shell_Last_Shell_Command = Null_String;

#ifdef VMS
. ( 
.   [cmd] =cmd
.   [cfile file cmd]
.   "vms_shell.com" expand_jedlib_file =cfile
.   cfile strlen {"Unable to open vms_shell.com" error} !if
.   "@" cfile strcat "/output=" strcat =cfile
  
.   "sys$login:_jed_shell.cmd_" =file
.   cfile file strcat =cfile
  
.   " \"" cmd strcat "\"" strcat =cmd
.   "starting process..." flush
.   cfile cmd strcat system pop
.   file insert_file pop
.   file delete_file
. ) shell_cmd
#endif

#ifdef MSDOS
. (
.   [cmd] =cmd
.   [file] "_jed_shll.cmd" =file
.   cmd " &> " strcat file strcat =cmd
.   "opening pipe..." flush
.   cmd system 0 < {"system failed." error} if
.   file insert_file pop
.   file delete_file
. ) shell_cmd

#endif 

#ifdef MSDOS VMS OS2
#ifdef MSDOS OS2
. (
.   [dir n] =dir
  % this bit will remove final slash on DOS unless it is root dir.
.   dir strlen =n
.   dir n 1 substr "\\" strcmp n 3 <= or
.     { 
.       dir 1 n 1 - substr =dir
.     } !if
.   dir 
. ) msdos_fix_dir
#endif
#ifndef OS2
. (
.   [cmd n] 
.   "Shell Command:"  Null_String Shell_Last_Shell_Command read_mini =cmd 
.   cmd strlen {return} !if
.   cmd =Shell_Last_Shell_Command
.   getbuf_info pop pop exch pop
#ifdef MSDOS
.   msdos_fix_dir
#endif
.   change_default_dir {"Unable to chdir!" error} if

.   "*shell-output*" pop2buf erase_buffer
.   cmd shell_cmd
.   bob
. ) do_shell_cmd
#endif OS2
#endif MSDOS VMS

#ifdef UNIX OS2
. (
.    [dir]
.    [msg shbuf cmd] "*shell-output*" =shbuf "opening pipe..." =msg
.    "Shell Command:"  Null_String Shell_Last_Shell_Command read_mini =cmd
.    cmd strlen { return } !if
.    cmd =Shell_Last_Shell_Command   
.    "(" cmd strcat ")" strcat =cmd
.    cmd " 2>&1" strcat =cmd
   
.    getbuf_info pop pop exch pop =dir
.    dir change_default_dir {"Unable to chdir!" error} if
.    shbuf pop2buf erase_buffer
.    msg flush
.    cmd shell_cmd bob
.    msg "done" strcat flush
. ) do_shell_cmd 
#endif UNIX OS2

. (
.   [km dir]  "*shell*" =km

.   km keymap_p { 
.     km make_keymap
.     "shell_input" "^M" km definekey
.   } !if
.   getbuf_info pop pop =dir pop
#ifdef MSDOS OS2
.   dir msdos_fix_dir =dir
#endif
.   dir change_default_dir {"Unable to chdir!" error} if
  
  
.   km pop2buf
.   km use_keymap
.   "Current Directory is " insert dir insert newline
#ifdef UNIX
.    "% " insert
#endif
#ifdef MSDOS OS2
.    "> " insert
#endif
#ifdef VMS
.    "$ " insert
#endif
. ) shell


. (
.    [cmd tmp prompt]
#ifdef UNIX
.    "% " =prompt "/tmp/_jed_shell_" make_tmp_file =tmp
#endif
#ifdef MSDOS OS2
.    "> " =prompt "jedshell." make_tmp_file =tmp
#endif
#ifdef VMS
.    "$ " =prompt "sys$login:_jed_shell_." make_tmp_file =tmp
#endif
  
   
.    bol prompt skip_chars
.    push_mark eol bufsubstr =cmd
.    eob bol prompt skip_chars
.    cmd looking_at {cmd insert} !if
.    eol newline
.    0 update  %% Update now so user see that things are ok
   
.    cmd strlen {
.                 getbuf_info pop pop exch pop
#ifdef MSDOS OS2
.                 msdos_fix_dir
#endif
. 		change_default_dir {"Unable to chdir!" error} if

#ifdef UNIX
.                 "( " cmd strcat ")" strcat 
. 		" > " strcat tmp strcat " 2>&1" strcat
#endif
#ifdef MSDOS
.                 cmd " &> " strcat tmp strcat
#endif
#ifdef OS2
.                 cmd " > " strcat tmp strcat 
#endif
#ifdef VMS
.                "vms_shell.com" expand_jedlib_file dup
. 	       strlen {pop "Unable to find vms_shell.com" error} !if
.                "@" exch strcat 
. 	       "/output=" strcat tmp strcat
. 	       " " strcat cmd strcat 
#endif
.      system pop
.      tmp insert_file pop
.      tmp delete_file pop
.    } if
   
.    bolp {prompt}{"\n" prompt strcat} else insert
#ifdef OS2
.  1 update %% Update due to problems with Borland C
#endif

. ) shell_input

