$! Copyright 2001 Compaq Computer Corporation
$!
$! Put together various parts of the OpenVMS Freeware CD-ROM...
$! Freeware V5.0 edition: two volume CD-ROM set
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

    $ DISK$FREEWARE%%_%:[tti_intouch]intouch.com
    $ set default DISK$FREEWARE%%_%:[FREEWARE]
    $ intouch/source BUILD_MENU_INFO.int

$
$ a=f$environment("PROCEDURE")
$ dev    = f$parse(a,,,"DEVICE",)
$ devdir = f$parse(a,,,"DEVICE",) + f$parse(a,,,"DIRECTORY",)
$
$ define/process/nolog freeware$cd  'dev'
$ define/process/nolog freeware$loc 'devdir'
$
$ catlist = "|GAMES|GRAPHICS|LANGUAGES|MISCELLANEOUS|MOTIF_TOYS|NETWORKING|" +-
            "SOFTWARE|SYSTEM_MGMT|STORAGE_MGMT|BUILD_TOOLS|" +-
            "UTILITIES|AFFINITY|WEB_TOOLS|PROGRAMMING|"
$ bel[0,7] = 7
$
$ close/nolog infile
$ close/nolog outfile
$
$!$ call scan_the_readme_files DISK$FREEWARE50_1:[*]freeware_readme.txt
$!$ call scan_the_readme_files DISK$FREEWARE50_2:[*]freeware_readme.txt
$
$ delete sys$scratch:fw1rm.txt;*,fw1ab.txt;*
$ call gather_the_readme_files -
  DISK$FREEWARE50_1:[*]freeware_readme.txt -
  sys$scratch:fw1rm.txt sys$scratch:fw1ab.txt DISK$FREEWARE50_1
$
$ delete sys$scratch:fw2rm.txt;*
$ call gather_the_readme_files -
  DISK$FREEWARE50_2:[*]freeware_readme.txt -
  sys$scratch:fw2rm.txt sys$scratch:fw2ab.txt DISK$FREEWARE50_2
$
$ delete DISK$FREEWARE50_1:[000000]AAAREADME.V5;*
$ delete DISK$FREEWARE50_2:[000000]AAAREADME.V5;*
$ append sys$scratch:fw1rm.txt,fw2rm.txt -
    DISK$FREEWARE50_1:[000000]AAAREADME.V5/NEW
$ append sys$scratch:fw1rm.txt,fw2rm.txt -
    DISK$FREEWARE50_2:[000000]AAAREADME.V5/NEW
$ delete sys$scratch:fw1rm.txt;*,sys$scratch:fw2rm.txt;*
$
$ close/nolog abfile
$ delete sys$scratch:fw0ab.txt;*
$ open/write abfile sys$scratch:fw0ab.txt
$ write abfile "OpenVMS Freeware Abstracts"
$ close/nolog abfile 
$
$ append sys$scratch:fw0ab.txt,fw1ab.txt,fw2ab.txt -
    DISK$FREEWARE50_1:[000000]00FREEWARE_ABSTRACT.TXT/NEW
$ append sys$scratch:fw0ab.txt,fw1ab.txt,fw2ab.txt -
    DISK$FREEWARE50_2:[000000]00FREEWARE_ABSTRACT.TXT/NEW
$ append sys$scratch:fw0ab.txt,fw1ab.txt,fw2ab.txt -
    DISK$FREEWARE50_1:[FREEWARE]00FREEWARE_ABSTRACT.TXT/NEW
$ append sys$scratch:fw0ab.txt,fw1ab.txt,fw2ab.txt -
    DISK$FREEWARE50_2:[FREEWARE]00FREEWARE_ABSTRACT.TXT/NEW
$ delete sys$scratch:fw0ab.txt,fw1ab.txt;*,sys$scratch:fw2ab.txt;*
$
$ write sys$output "Cleaning up the directories ..."
$
$ PURGE DISK$FREEWARE50_1:[000000]*.*
$ PURGE DISK$FREEWARE50_2:[000000]*.*
$
$ RENAME DISK$FREEWARE50_1:[000000]*.TXT;* *.*;1
$ RENAME DISK$FREEWARE50_2:[000000]*.TXT;* *.*;1
$ RENAME DISK$FREEWARE50_1:[000000]*.V%;* *.*;1
$ RENAME DISK$FREEWARE50_2:[000000]*.V%;* *.*;1
$
$ PURGE DISK$FREEWARE50_1:[FREEWARE]*.*
$ PURGE DISK$FREEWARE50_2:[FREEWARE]*.*
$ RENAME DISK$FREEWARE50_1:[FREEWARE]*.*;* DISK$FREEWARE50_1:[FREEWARE]*.*;1
$ RENAME DISK$FREEWARE50_2:[FREEWARE]*.*;* DISK$FREEWARE50_2:[FREEWARE]*.*;1
$ PURGE DISK$FREEWARE50_1:[000TOOLS]*.*
$ PURGE DISK$FREEWARE50_2:[000TOOLS]*.*
$ RENAME DISK$FREEWARE50_1:[000TOOLS]*.*;* DISK$FREEWARE50_1:[000TOOLS]*.*;1
$ RENAME DISK$FREEWARE50_2:[000TOOLS]*.*;* DISK$FREEWARE50_2:[000TOOLS]*.*;1
$
$ write sys$output "Checking the abstracts for differences ..."
$
$ DIFF DISK$FREEWARE50_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWARE50_2:[000000]00FREEWARE_ABSTRACT.TXT
$ DIFF DISK$FREEWARE50_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWARE50_1:[FREEWARE]00FREEWARE_ABSTRACT.TXT
$ DIFF DISK$FREEWARE50_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWARE50_2:[FREEWARE]00FREEWARE_ABSTRACT.TXT
$
$ write sys$output "Setting the security attributes ..."
$
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] -
  DISK$FREEWARE50_1:[000000]*.*;*
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] -
  DISK$FREEWARE50_2:[000000]*.*;*
$
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] -
  DISK$FREEWARE50_1:[*...]*.*;*
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] -
  DISK$FREEWARE50_2:[*...]*.*;*
$
$ write sys$output "Checking for any missing copies of the (current?) OpenVMS FAQ..."
$ if f$search("DISK$FREEWARE50_1:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "  Missing: DISK$FREEWARE50_1:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT"
$ endif
$ if f$search("DISK$FREEWARE50_1:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "  Missing: DISK$FREEWARE50_1:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT"
$ endif
$ if f$search("DISK$FREEWARE50_2:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "  Missing: DISK$FREEWARE50_2:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT"
$ endif
$ if f$search("DISK$FREEWARE50_2:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "  Missing: DISK$FREEWARE50_2:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT"
$ endif
$ write sys$output "Check for missing FAQs completed."
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
$ close/nolog abfile
$ open/write outfile 'p2'
$ open/write abfile 'p3'
$ write abfile " "
$ write abfile "Abstracts for Volume ''p4'"
$ write abfile " "
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
$   name = f$edit(f$element(0,",",textline),"COLLAPSE")
$   name2 = f$edit(f$element(0,",",textline),"TRIM")
$   if name2 .nes. name
$   then
$     write sys$output "** Space in kit name ''nextfile'" + bel
$   endif
$   if f$length(name) .gt. 15
$   then
$     write sys$output "** Kit name too long ''nextfile'" + bel
$   endif
$   category = f$edit(f$element(1,",",textline),"UPCASE,COLLAPSE")
$   category2 = f$edit(f$element(1,",",textline),"UPCASE,TRIM")
$   if category2 .nes. category
$   then
$     write sys$output "** Space in category name in ''nextfile'" + bel
$   endif
$   if f$locate("|''category'|",catlist) .eq. f$length(catlist)
$   then
$     write sys$output "** Category name mismatch in ''nextfile'" + bel
$   endif
$   if f$length(category) .gt. 15
$   then
$     write sys$output "** Category name too long ''nextfile'" + bel
$   endif
$   commacheck1 = textline - "," - "," - "," 
$   commacheck2 = textline - "," - ","
$   if commacheck1 .nes. commacheck2
$   then
$     write sys$output "** Spurious comma in ''nextfile'" + bel
$   endif
$   name = f$edit(f$element( 0,",",textline),"TRIM")
$   desc = f$edit(f$element( 2,",",textline),"TRIM,COMPRESS")
$   name = "[''name']                                                                  "
$   desc = "''desc'                                                                  "
$   name = f$extract(0,24,name)
$   desc = f$extract(0,54,desc)
$   write abfile "''name'''desc'"
$ endif
$ write outfile textline
$ goto gtrf_loop2
$oops:
$ close/nolog infile
$ close/nolog outfile
$ write abfile " "
$ close/nolog abfile
$ write sys$output "The tub is filled with brightly colored hammers."
$ write sys$output "(Unexpected abstract creation results.)"
$ write sys$output "Freeware abstract creation terminating."
$ exit
$done:
$ close/nolog infile
$ close/nolog outfile
$ write abfile " "
$ close/nolog abfile
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
$ write sys$output "The red wine tastes very wrench-like today."
$ write sys$output "(Unexpected abstract creation results.)"
$ write sys$output "Scan of freeware abstracts terminating."
$ exit
$done:
$ close/nolog infile
$ write sys$output "Scan of freeware abstracts completed."
$ exit
$ endsubroutine
