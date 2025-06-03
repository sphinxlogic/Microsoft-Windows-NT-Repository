%
%  This should be used to byte compile files in JED_ROOT/lib for
%  fast loading.  This may be performed in batch mode as:
%
%      jed -batch -n -l bytecomp.sl
%
%

!if (is_defined ("Preprocess_Only"))
{
   variable Preprocess_Only = 0;
}

Null_String;             % a marker

% list of file to byte compile:

#ifdef UNIX VMS
. "mail.sl" "iso-latin.sl" 
#endif
#ifdef UNIX
. "rmail.sl"  
#endif
#ifdef MSDOS OS2
. "dos437.sl"  "dos850.sl" 
#endif
#ifdef XWINDOWS
. "mouse.sl"
#endif
. "mousex.sl"
. "dabbrev.sl" "mutekeys.sl" "bookmark.sl" "replace.sl"
. "tex.sl" "binary.sl" "compile.sl" "ctags.sl" "isearch.sl"  "jedhelp.sl"
. "rot13.sl"  "tabs.sl"  "untab.sl"  "jedhelp.sl"  "ctags.sl"  "compile.sl"
. "menu.sl" "dired.sl" "util.sl" "tmisc.sl" "cmisc.sl" "ispell.sl" "misc.sl"
. "help.sl" "cal.sl" "man.sl" "fortran.sl" "dcl.sl" "shell.sl" "most.sl"
. "info.sl" "ispell.sl" "sort.sl" "regexp.sl" "wordstar.sl" "buf.sl"
. "emacsmsc.sl"
. "indent.sl" "search.sl" "linux.sl" "mini.sl" "edt.sl" "emacs.sl" "site.sl"

define jed_byte_compile_file(f, method)
{
   variable file;
   
   file = expand_jedlib_file(f);
   if (strlen(file)) 
     {
	flush(strcat("Processing ", file));
	byte_compile_file(file, method);
     }
   else flush(strcat(f, " not found-- Skipped."));
}

while (=$1, strlen($1)) jed_byte_compile_file($1, Preprocess_Only);
exit_jed();


   
