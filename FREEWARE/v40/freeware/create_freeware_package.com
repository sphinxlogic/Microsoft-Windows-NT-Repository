$! Put together various parts of the OpenVMS Freeware CD-ROM...
$! Freeware V4.0 edition: two volume CD-ROM set
$
$ set noon
$ set control=(y,t)
$
$ TYPE SYS$INPUT

  When creating a new version of the OpenVMS Freeware CD-ROM set, 
  remember to invoke INTOUCH with BUILD_MENU_INFO.INT as the target 
  file for the menu system.  (Or use BUILD_MENU_INFOALL.INT, to look 
  for the FREEWARE_README.TXT files anywhere on the Freeware disk.)

  The INTOUCH command to build the menu must be invoked on each CD-ROM 
  in the Freeware CD-ROM set.  The command used looks like this:

    $ set default DISK$FREEWAREV%_%:[FREEWARE]
    $ intouch/source BUILD_MENU_INFO.int

$
$
$ catlist = "|GAMES|GRAPHICS|LANGUAGES|MISCELLANEOUS|MOTIF_TOYS|NETWORKING|" +-
            "SOFTWARE|SYSTEM_MANAGEMENT|STORAGE_MANAGEMENT|BUILD_TOOLS|" +-
            "UTILITIES|AFFINITY|WEB_TOOLS|"
$ bel[0,7] = 7
$
$ close/nolog infile
$ close/nolog outfile
$
$!$ call scan_the_readme_files DISK$FREEWAREV4_1:[*]freeware_readme.txt
$!$ call scan_the_readme_files DISK$FREEWAREV4_2:[*]freeware_readme.txt
$
$ delete sys$scratch:fw1rm.txt;*
$ call gather_the_readme_files -
  DISK$FREEWAREV4_1:[*]freeware_readme.txt -
  sys$scratch:fw1rm.txt
$
$ delete sys$scratch:fw2rm.txt;*
$ call gather_the_readme_files -
  DISK$FREEWAREV4_2:[*]freeware_readme.txt -
  sys$scratch:fw2rm.txt
$
$ delete DISK$FREEWAREV4_1:[000000]AAAREADME.V4;*
$ delete DISK$FREEWAREV4_2:[000000]AAAREADME.V4;*
$ copy sys$scratch:fw1rm.txt,fw2rm.txt DISK$FREEWAREV4_1:[000000]AAAREADME.V4
$ copy sys$scratch:fw1rm.txt,fw2rm.txt DISK$FREEWAREV4_2:[000000]AAAREADME.V4
$ delete sys$scratch:fw1rm.txt;*,sys$scratch:fw2rm.txt;*
$
$ write sys$output "Cleaning up the directories ..."
$
$ PURGE DISK$FREEWAREV4_1:[000000]*.*
$ PURGE DISK$FREEWAREV4_2:[000000]*.*
$
$ RENAME DISK$FREEWAREV4_1:[000000]*.TXT;* *.*;1
$ RENAME DISK$FREEWAREV4_2:[000000]*.TXT;* *.*;1
$ RENAME DISK$FREEWAREV4_1:[000000]*.V%;* *.*;1
$ RENAME DISK$FREEWAREV4_2:[000000]*.V%;* *.*;1
$
$ PURGE DISK$FREEWAREV4_1:[FREEWARE]*.*
$ PURGE DISK$FREEWAREV4_2:[FREEWARE]*.*
$ RENAME DISK$FREEWAREV4_1:[FREEWARE]*.*;* DISK$FREEWAREV4_1:[FREEWARE]*.*;1
$ RENAME DISK$FREEWAREV4_2:[FREEWARE]*.*;* DISK$FREEWAREV4_2:[FREEWARE]*.*;1
$ PURGE DISK$FREEWAREV4_1:[000TOOLS]*.*
$ PURGE DISK$FREEWAREV4_2:[000TOOLS]*.*
$ RENAME DISK$FREEWAREV4_1:[000TOOLS]*.*;* DISK$FREEWAREV4_1:[000TOOLS]*.*;1
$ RENAME DISK$FREEWAREV4_2:[000TOOLS]*.*;* DISK$FREEWAREV4_2:[000TOOLS]*.*;1
$
$ write sys$output "Checking the abstracts for differences ..."
$
$ DIFF DISK$FREEWAREV4_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWAREV4_2:[000000]00FREEWARE_ABSTRACT.TXT
$ DIFF DISK$FREEWAREV4_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWAREV4_1:[FREEWARE]00FREEWARE_ABSTRACT.TXT
$ DIFF DISK$FREEWAREV4_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWAREV4_2:[FREEWARE]00FREEWARE_ABSTRACT.TXT
$
$ write sys$output "Setting the security attributes ..."
$
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] -
  DISK$FREEWAREV4_1:[000000]*.*;*
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] -
  DISK$FREEWAREV4_2:[000000]*.*;*
$
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] -
  DISK$FREEWAREV4_1:[*...]*.*;*
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] -
  DISK$FREEWAREV4_2:[*...]*.*;*
$
$
$ write sys$output "Done!"
$
$ EXIT
$
$gather_the_readme_files: subroutine
$ write sys$output "Creating the freeware abstract ..."
$ close/nolog infile
$ close/nolog outfile
$ open/write outfile 'p2'
$ ff[0,7] = 12
$gtrf_loop1:
$ close/nolog infile
$ nextfile = f$search("''p1'",1)
$ nextfilelen = f$length(nextfile) 
$ if nextfilelen .eq. 0 then goto done
$ delimiter = f$fao("!#*+",nextfilelen)
$ write sys$output "Now processing ''nextfile' ..."
$ open/read infile 'nextfile'
$ write outfile ff
$ write outfile delimiter
$ write outfile nextfile
$ write outfile delimiter
$ write outfile " "
$ category = ""
$gtrf_loop2:
$ read/end=gtrf_loop1/error=oops infile textline
$ if category .eqs. ""
$ then
$   category = f$edit(f$element(1,",",textline),"UPCASE,COLLAPSE")
$   category2 = f$edit(f$element(1,",",textline),"UPCASE,TRIM")
$   if category2 .nes. category
$   then
$     write sys$output "** Category space in ''nextfile'" + bel
$   endif
$   if f$locate("|''category'|",catlist) .eq. f$length(catlist)
$   then
$     write sys$output "** Category error in ''nextfile'" + bel
$   endif
$ endif
$ write outfile textline
$ goto gtrf_loop2
$oops:
$ close/nolog infile
$ close/nolog outfile
$ write sys$output "The tub is filled with brightly colored hammers."
$ write sys$output "(Unexpected abstract creation results.)"
$ write sys$output "Freeware abstract creation terminating."
$ exit
$done:
$ close/nolog infile
$ close/nolog outfile
$ write sys$output "Freeware abstract completed."
$ exit
$ endsubroutine
$
$
$scan_the_readme_files: subroutine
$ write sys$output "Scanning the freeware abstracts ..."
$ close/nolog outfile
$strf_loop1:
$ close/nolog infile
$ nextfile = f$search("''p1'",1)
$ nextfilelen = f$length(nextfile) 
$ if nextfilelen .eq. 0 then goto done
$ delimiter = f$fao("!#*+",nextfilelen)
$ write sys$output "Now processing ''nextfile' ..."
$ open/read infile 'nextfile'
$ category = ""
$strf_loop2:
$ read/end=strf_loop1/error=oops infile textline
$ if category .eqs. ""
$ then
$   category = f$edit(f$element(1,",",textline),"UPCASE,COLLAPSE")
$   category2 = f$edit(f$element(1,",",textline),"UPCASE,TRIM")
$   if category2 .nes. category
$   then
$     write sys$output "** Category space in ''nextfile'" + bel
$   endif
$   if f$locate("|''category'|",catlist) .eq. f$length(catlist)
$   then
$     write sys$output "** Category error in ''nextfile'" + bel
$   endif
$   name = f$edit(f$element( 0,",",textline),"TRIM")
$   desc = f$edit(f$element( 2,",",textline),"TRIM")
$   write sys$output "Product Name : ''name'"
$   write sys$output "Category     : ''category'"
$   write sys$output "Description  : ''desc'"
$   commacheck1 = textline - "," - "," - "," 
$   commacheck2 = textline - "," - ","
$   if commacheck1 .nes. commacheck2
$   then
$     write sys$output "** Spurious comma in ''nextfile'" + bel
$   endif
$   write sys$output " "
$ endif
$ goto strf_loop2
$oops:
$ close/nolog infile
$ write sys$output "The red cabinet tastes very green today."
$ write sys$output "(Unexpected abstract creation results.)"
$ write sys$output "Scan of freeware abstracts terminating."
$ exit
$done:
$ close/nolog infile
$ write sys$output "Scan of freeware abstracts completed."
$ exit
$ endsubroutine
