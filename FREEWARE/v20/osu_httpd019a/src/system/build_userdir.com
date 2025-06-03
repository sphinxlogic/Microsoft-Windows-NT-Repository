$! This procedure creates a www userdirectory in a specified user's
$! home directory tree.  It adds ACL entries to the created files to allow
$! access.
$!
$! Create www.dir directory and grant it read access to the www_server account.
$!
$! Parameters: P1	Login directory, default SYS$LOGIN.	
$! 	       P2	Username, default process username.
$!	       P3	Owner field, no default
$!
$! The symbol www_account must be set to the username of the www_server
$! acount.
$ www_account = "www_server"
$ if f$type(www_account) .eqs. ""
$ then
$    write sys$output "www_account symbol not defined, please edit this file"
$    exit
$ endif
$!
$! Get parameters
$!
$ home = P1
$ if home .eqs. "" then home = f$trnlnm("SYS$LOGIN")
$ if p2 .eqs. "" then p2 = f$getjpi("0","USERNAME")
$ if p3 .eqs. "" then read/end=abort/prompt="Personal name: " sys$command p3
$!
$! Create directory and add ACL
$!
$ wwwdir = f$extract(0,f$length(home)-1,home) + ".WWW]"
$ create/directory/log 'wwwdir'
$ set acl 'home'www.dir;1/log -
	/acl=(IDENTIFIER='www_account',OPTIONS=DEFAULT,ACCESS=READ+EXECUTE)
$ set acl 'home'www.dir;1/log -
	/acl=(IDENTIFIER='www_account',ACCESS=READ+EXECUTE)
$!
$! set ACL entry on login directory itself.
$!
$ login_dir = home
$ i = f$length(login_dir)
$ back_scan:
$   i = i - 1
$   if i .le. 0 then goto scan_done
$   char = f$extract(i,1,login_dir)
$   if char .nes. "." .and. char .nes. "[" then goto back_scan
$ scan_done:
$   label = f$element(0,"]",f$extract(i+1,255,login_dir))
$   base = f$extract(0,i,login_dir)
$   if f$extract(i,1,home) .eqs. "[" then base = base + "[000000"
$   login_dir = base + "]" + label + ".DIR;1"
$ write sys$output "login_dir: ",login_dir,"  label: ", label
$ set acl 'login_dir'/log -
	/acl=(IDENTIFIER='www_account',OPTIONS=NOPROPAGATE,ACCESS=EXECUTE)
$!
$! if f$search(wwwdir+"index.html") .nes. "" then exit
$! if f$search(home+"finger.pln") .eqs. "" then exit
$ write sys$output "Creating ''wwwdir'index.html"
$ create 'wwwdir'index.html
<HTML>
<!-- This file is in HTML format for processing by WWW (World Wide Web)
     client programs. -->
<HEAD>
$ open/append ifile 'wwwdir'index.html
$ on error then goto rundown
$ on control_y then goto rundown
$ write ifile "<TITLE>User ",P2," on KCGL1</TITLE>"
$ write ifile "</HEAD><BODY>"
$ write ifile "<H2>",P3,"</H2>"
$ write ifile ""
$ write ifile "<!-- Replace '/www/userpic.gif' with filename of your own image -->
$ write ifile "<IMG SRC=""/www/userpic.gif"">"
$ write ifile ""
$!
$! Append finger.pln if found.
$!
$ if f$search(home+"finger.pln") .eqs. "" then goto rundown
$ write sys$output "Found finger.pln file in sys$login"
$ write ifile "<!-- A finger.pln file was found in this account's login directory.
$ write ifile "     It is being included as pre-formatted text. -->
$ write ifile "<HR><!--Separator line -->"
$ open/read ffile 'home'Finger.pln
$ on error then goto rundown_f
$ on control_y then goto rundown_f
$ write ifile "<H3>finger plan</H3><PRE>"
$ append_line:
$    read/end=rundown_f ffile line
$ fixup_line_lt:
$    lt = f$locate("<",line)
$    if lt .ge. f$length(line) then goto fixup_line_gt
$    line = f$extract(0,lt,line) + "&lt;" + f$extract(lt+1,256,line)
$    goto fixup_line_lt
$!
$ fixup_line_gt:
$    gt = f$locate("<",line)
$    if gt .ge. f$length(line) then goto output_line
$    line = f$extract(0,gt,line) + "&gt;" + f$extract(gt+1,256,line)
$    goto fixup_line_lt
$!
$ output_line:
$    write/symbol ifile line
$    goto append_line
$ rundown_f:
$ write ifile "</PRE>"
$ close ffile
$ rundown:
$ write ifile "</BODY></HTML>"
$ close ifile
$ abort:
$ exit
