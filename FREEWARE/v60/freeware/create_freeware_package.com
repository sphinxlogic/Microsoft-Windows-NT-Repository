$! Copyright 2003 Hewlett-Packard Company
$! Copyright 2001 Compaq Computer Corporation
$!
$! Put together various parts of the OpenVMS Freeware CD-ROM.
$! The Freeware V6.0 edition is a two-volume CD-ROM set
$!
$!
$! Procedure written by Stephen Hoffman for Freeware V4.0.
$! Updated by Stephen Hoffman for Freeware V5.0 and V6.0.
$!
$!
$! Change Notice: Some of what is processed, created and used here is also
$! discussed over in the OpenVMS CD-ROM Cover Letter -- changes made here
$! to constructs such as the volume label(s) or to the number of Freeware
$! volumes will need to be reflected within the Cover Letter text used.
$!
$! This procedure presently expects two Freeware volumes, and expects both
$! of the Freeware staging masters to be mounted.  (The LD tool is used for
$! staging, and CDRTOOLS/DVDRTOOLS is used to generate the master CD media.)
$!
$! The Freeware staging disks must be created with INITIALIZE/ERASE/SYSTEM,
$! and should specify /STRUCTURE=2 (ODS2) and the appropriate volume label.
$!
$
$ set noon
$ set control=(y,t)
$
$ fwvversdot == "V6.0"
$ fwvvers == "V60"
$ fwvers == "60"
$ fwfaq  == "60"
$
$
$ TYPE SYS$INPUT

  When creating a new version of the OpenVMS Freeware CD-ROM set, 
  remember to invoke INTOUCH with BUILD_MENU_INFO.INT as the target 
  file for the menu system.  (Or use BUILD_MENU_INFOALL.INT, to look 
  for the FREEWARE_README.TXT files anywhere on the Freeware disk.)

  The INTOUCH command used to build the menu must be separately 
  invoked on each CD-ROM in the Freeware CD-ROM set.  

  The INTOUCH command required looks like this:

    $ @DISK$FREEWARE%%_%:[tti_intouch]intouch.com
    $ define freeware$cd DISK$FREEWARE%%_%
    $ set default DISK$FREEWARE%%_%:[FREEWARE]
    $ intouch/source BUILD_MENU_INFO.int

  The number of CD-ROM volumes and the volume labels used here are
  documented in the CD-ROM cover letter for OpenVMS -- updates made
  here must be passed along to the writer updating the cover letter.

  Changes made to the category list here must be passed along to the
  OpenVMS webmaster for inclusion on the Freeware Submission webpage.

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
            "UTILITIES|AFFINITY|WEB_TOOLS|PROGRAMMING|FREEWARE|"
$ bel[0,7] = 7
$
$ close/nolog infile
$ close/nolog outfile
$
$ call missing_readme_files DISK$FREEWARE'fwvers'_1:[000000]*.DIR
$ call missing_readme_files DISK$FREEWARE'fwvers'_2:[000000]*.DIR
$
$ call scan_the_readme_files DISK$FREEWARE'fwvers'_1:[*]freeware_readme.txt
$ call scan_the_readme_files DISK$FREEWARE'fwvers'_2:[*]freeware_readme.txt
$
$ if f$search("sys$scratch:fw1rm.txt;*") .nes. "" then delete sys$scratch:fw1rm.txt;*
$ if f$search("sys$scratch:fw1ab.txt;*") .nes. "" then delete sys$scratch:fw1ab.txt;*
$ copy/write nla0: sys$scratch:fw1rm.txt
$ copy/write nla0: sys$scratch:fw1ab.txt
$ call gather_the_readme_files -
  DISK$FREEWARE'fwvers'_1:[*]freeware_readme.txt -
  sys$scratch:fw1rm.txt sys$scratch:fw1ab.txt DISK$FREEWARE'fwvers'_1
$
$ if f$search("sys$scratch:fw2rm.txt;*") .nes. "" then delete sys$scratch:fw2rm.txt;*
$ if f$search("sys$scratch:fw2ab.txt;*") .nes. "" then delete sys$scratch:fw2ab.txt;*
$ copy/write nla0: sys$scratch:fw2rm.txt
$ copy/write nla0: sys$scratch:fw2ab.txt
$ call gather_the_readme_files -
  DISK$FREEWARE'fwvers'_2:[*]freeware_readme.txt -
  sys$scratch:fw2rm.txt sys$scratch:fw2ab.txt DISK$FREEWARE'fwvers'_2
$
$ if f$search("DISK$FREEWARE''fwvers'_1:[000000]AAAREADME.''fwvvers';*") .nes. "" then delete DISK$FREEWARE'fwvers'_1:[000000]AAAREADME.'fwvvers';*
$ if f$search("DISK$FREEWARE''fwvers'_2:[000000]AAAREADME.''fwvvers';*") .nes. "" then delete DISK$FREEWARE'fwvers'_2:[000000]AAAREADME.'fwvvers';*
$
$ append sys$scratch:fw1rm.txt,fw2rm.txt -
    DISK$FREEWARE'fwvers'_1:[000000]AAAREADME.'fwvvers'/NEW
$ append sys$scratch:fw1rm.txt,fw2rm.txt -
    DISK$FREEWARE'fwvers'_2:[000000]AAAREADME.'fwvvers'/NEW
$ if f$search("sys$scratch:fw1rm.txt;*") .nes. "" then delete sys$scratch:fw1rm.txt;*
$ if f$search("sys$scratch:fw2rm.txt;*") .nes. "" then delete sys$scratch:fw2rm.txt;*
$
$ close/nolog abfile
$ if f$search("sys$scratch:fw0ab.txt;*") .nes. "" then delete sys$scratch:fw0ab.txt;*
$ copy/write nla0: sys$scratch:fw0ab.txt    ! creates RMS seq. variable CR carriage control file format
$ open/append abfile sys$scratch:fw0ab.txt
$ write abfile "OpenVMS Freeware ''fwvversdot' Abstracts"
$ write abfile " "
$ close/nolog abfile 
$
$ if f$search("DISK$FREEWARE''fwvers'_1:[000000]00FREEWARE_ABSTRACT.TXT;*") .nes. "" then delete DISK$FREEWARE'fwvers'_1:[000000]00FREEWARE_ABSTRACT.TXT;*
$ if f$search("DISK$FREEWARE''fwvers'_2:[000000]00FREEWARE_ABSTRACT.TXT;*") .nes. "" then delete DISK$FREEWARE'fwvers'_2:[000000]00FREEWARE_ABSTRACT.TXT;*
$ if f$search("DISK$FREEWARE''fwvers'_1:[FREEWARE]00FREEWARE_ABSTRACT.TXT;*") .nes. "" then delete DISK$FREEWARE'fwvers'_1:[FREEWARE]00FREEWARE_ABSTRACT.TXT;*
$ if f$search("DISK$FREEWARE''fwvers'_2:[FREEWARE]00FREEWARE_ABSTRACT.TXT;*") .nes. "" then delete DISK$FREEWARE'fwvers'_2:[FREEWARE]00FREEWARE_ABSTRACT.TXT;*
$
$ append sys$scratch:fw0ab.txt,fw1ab.txt,fw2ab.txt -
    DISK$FREEWARE'fwvers'_1:[000000]00FREEWARE_ABSTRACT.TXT/NEW
$ append sys$scratch:fw0ab.txt,fw1ab.txt,fw2ab.txt -
    DISK$FREEWARE'fwvers'_2:[000000]00FREEWARE_ABSTRACT.TXT/NEW
$ append sys$scratch:fw0ab.txt,fw1ab.txt,fw2ab.txt -
    DISK$FREEWARE'fwvers'_1:[FREEWARE]00FREEWARE_ABSTRACT.TXT/NEW
$ append sys$scratch:fw0ab.txt,fw1ab.txt,fw2ab.txt -
    DISK$FREEWARE'fwvers'_2:[FREEWARE]00FREEWARE_ABSTRACT.TXT/NEW
$ delete sys$scratch:fw0ab.txt;*,fw1ab.txt;*,sys$scratch:fw2ab.txt;*
$
$
$ Call Just_The_FAQs
$
$ write sys$output "Cleaning up the directories ..."
$
$ PURGE DISK$FREEWARE'fwvers'_1:[000000]*.*
$ PURGE DISK$FREEWARE'fwvers'_2:[000000]*.*
$
$ RENAME DISK$FREEWARE'fwvers'_1:[000000]*.TXT;* *.*;1
$ RENAME DISK$FREEWARE'fwvers'_2:[000000]*.TXT;* *.*;1
$ RENAME DISK$FREEWARE'fwvers'_1:[000000]*.'fwvvers';* *.*;1
$ RENAME DISK$FREEWARE'fwvers'_2:[000000]*.'fwvvers';* *.*;1
$
$ PURGE DISK$FREEWARE'fwvers'_1:[FREEWARE]*.*
$ PURGE DISK$FREEWARE'fwvers'_2:[FREEWARE]*.*
$ RENAME DISK$FREEWARE'fwvers'_1:[FREEWARE]*.*;* DISK$FREEWARE'fwvers'_1:[FREEWARE]*.*;1
$ RENAME DISK$FREEWARE'fwvers'_2:[FREEWARE]*.*;* DISK$FREEWARE'fwvers'_2:[FREEWARE]*.*;1
$ PURGE DISK$FREEWARE'fwvers'_1:[000TOOLS]*.*
$ PURGE DISK$FREEWARE'fwvers'_2:[000TOOLS]*.*
$ RENAME DISK$FREEWARE'fwvers'_1:[000TOOLS]*.*;* DISK$FREEWARE'fwvers'_1:[000TOOLS]*.*;1
$ RENAME DISK$FREEWARE'fwvers'_2:[000TOOLS]*.*;* DISK$FREEWARE'fwvers'_2:[000TOOLS]*.*;1
$
$ write sys$output "Checking the abstracts for differences ..."
$
$ DIFF DISK$FREEWARE'fwvers'_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWARE'fwvers'_2:[000000]00FREEWARE_ABSTRACT.TXT
$ DIFF DISK$FREEWARE'fwvers'_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWARE'fwvers'_1:[FREEWARE]00FREEWARE_ABSTRACT.TXT
$ DIFF DISK$FREEWARE'fwvers'_1:[000000]00FREEWARE_ABSTRACT.TXT,-
  DISK$FREEWARE'fwvers'_2:[FREEWARE]00FREEWARE_ABSTRACT.TXT
$
$
$
$
$ write sys$output "Resetting appropriate security attributes ..."
$
$ set security/acl/delete=all DISK$FREEWARE'fwvers'_1:[000000]*.*;*
$ set security/acl/delete=all DISK$FREEWARE'fwvers'_2:[000000]*.*;*
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] DISK$FREEWARE'fwvers'_1:[*...]*.*;*
$ set security/prot=(s:re,g:re,w:re,o:re)/owner=[system] DISK$FREEWARE'fwvers'_2:[*...]*.*;*
$
$
$ TYPE SYS$INPUT

  The INTOUCH command used to build the menu must be separately 
  invoked on each CD-ROM in the Freeware CD-ROM set.  

  The INTOUCH command required looks like this:

    $ @DISK$FREEWARE%%_%:[tti_intouch]intouch.com
    $ define freeware$cd DISK$FREEWARE%%_%
    $ set default DISK$FREEWARE%%_%:[FREEWARE]
    $ intouch/source BUILD_MENU_INFO.int

$ write sys$output " "
$ write sys$output "CREATE_FREEWARE_PACKAGE is done!"
$ write sys$output " "
$
$
$Main_Exit:
$
$
$
$ EXIT
$
$
$
$gather_the_readme_files: subroutine
$ write sys$output "Creating the freeware abstract ..."
$ close/nolog infile
$ close/nolog outfile
$ close/nolog abfile
$ open/append outfile 'p2'
$ open/append abfile 'p3'
$ write abfile " "
$ write abfile "Abstracts for Volume ''p4'"
$ write abfile " "
$ ff[0,7] = 12
$ nextfile = f$search("SYS$LOGIN:LOGIN.COM",1)
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
$   if category .eqs. "FREEWARE"
$   then
$     write sys$output "** Submission uses reserved FREEWARE category" + bel
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
$ nextfile = f$search("SYS$LOGIN:LOGIN.COM",1)
$strf_loop1:
$ close/nolog infile
$ nextfile = f$search("''p1'",1)
$ nextfilelen = f$length(nextfile) 
$ if nextfilelen .eq. 0 then goto done
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
$ write sys$output "The beer tastes very wrench-like today."
$ write sys$output "(Unexpected abstract creation results.)"
$ write sys$output "Scan of freeware abstracts terminating."
$ exit
$done:
$ close/nolog infile
$ write sys$output " "
$ write sys$output "Scan of freeware abstract contents completed ..."
$ write sys$output " "
$ exit
$ endsubroutine
$
$
$missing_readme_files: subroutine
$ write sys$output "Scanning for missing FREEWARE_README.TXT files ..."
$ nextfile = f$search("SYS$LOGIN:LOGIN.COM",1)
$ nextfile = f$search("SYS$LOGIN:LOGIN.COM",2)
$mrmf_loop1:
$ nextdir = f$search("''p1'",1)
$ nextdirlen = f$length(nextdir) 
$ if nextdirlen .eq. 0 then goto done
$ nextdirspec = f$parse(nextdir,,,"DIRECTORY") - "][" - "><" - "]" - ">" - "[" - "<"
$ nextdirnamspec = f$parse(nextdir,,,"NAME") 
$ if nextdirnamspec .eqs. "000000" then goto mrmf_loop1
$ nextabstract = f$parse(nextdir,,,"DEVICE") + "[" + nextdirspec + "." + nextdirnamspec + "]FREEWARE_README.TXT"
$ write sys$output "Now seeking ''nextabstract' ..."
$ if f$search("''nextabstract'",2) .eqs. ""
$ then
$   write sys$output "** Missing ''nextabstract'"
$ endif
$ goto mrmf_loop1
$done:
$ write sys$output " "
$ write sys$output "Scan for missing FREEWARE_README.TXT files completed ..."
$ write sys$output " "
$ exit
$ endsubroutine
$
$
$Just_The_FAQs:  subroutine
$!
$ write sys$output "Checking for missing copies of the OpenVMS FAQ ..."
$
$ copy DISK$FREEWARE'fwvers'_1:[VMSFAQ]VMSFAQ.TXT DISK$FREEWARE'fwvers'_1:[VMSFAQ]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT
$ copy DISK$FREEWARE'fwvers'_1:[VMSFAQ]VMSFAQ.TXT DISK$FREEWARE'fwvers'_1:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT
$ copy DISK$FREEWARE'fwvers'_1:[VMSFAQ]VMSFAQ.TXT DISK$FREEWARE'fwvers'_1:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT
$ copy DISK$FREEWARE'fwvers'_1:[VMSFAQ]VMSFAQ.TXT DISK$FREEWARE'fwvers'_2:[VMSFAQ]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT
$ copy DISK$FREEWARE'fwvers'_1:[VMSFAQ]VMSFAQ.TXT DISK$FREEWARE'fwvers'_2:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT
$ copy DISK$FREEWARE'fwvers'_1:[VMSFAQ]VMSFAQ.TXT DISK$FREEWARE'fwvers'_2:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT
$
$!
$! These can be file aliases, if you are looking to save storage space on the disks ...
$!
$ bel[0,7] = 7
$ if f$search("DISK$FREEWARE''fwvers'_1:[VMSFAQ]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "** Missing: DISK$FREEWARE''fwvers'_1:[VMSFAQ]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT" + bel
$ endif
$ if f$search("DISK$FREEWARE''fwvers'_2:[VMSFAQ]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "** Missing: DISK$FREEWARE''fwvers'_2:[VMSFAQ]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT" + bel
$ endif
$ if f$search("DISK$FREEWARE''fwvers'_1:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "** Missing: DISK$FREEWARE''fwvers'_1:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT" + bel
$ endif
$ if f$search("DISK$FREEWARE''fwvers'_1:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "** Missing: DISK$FREEWARE''fwvers'_1:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT" + bel
$ endif
$ if f$search("DISK$FREEWARE''fwvers'_2:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. "" 
$ then 
$   write sys$output "** Missing: DISK$FREEWARE''fwvers'_2:[000000]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT" + bel
$ endif
$ if f$search("DISK$FREEWARE''fwvers'_2:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT",1) .eqs. ""
$ then 
$   write sys$output "** Missing: DISK$FREEWARE''fwvers'_2:[FREEWARE]OPENVMS_FREQUENTLY_ASKED_QUESTIONS_FAQ.TXT" + bel
$ endif
$ write sys$output "Check for missing FAQs completed ..."
$
$ endsubroutine
