$!
$! TYPCHK_DIFF.COM - is a command file to DIFF warnings on a new and old version of a TYPCHK file to sense if any new
$! warnings have been eliminated or introduced. Uses scratch files with File Extension of *.TYPCHK_(N/O)TEMP(/1/2).
$!
$ set nover
$ if p1 .eqs. "" then goto OUT_SCR
$ delete/sym/all
$ out_scr = 0
$ goto OS1
$OUT_SCR:
$ delete/sym/all
$ out_scr = 1
$OS1:
$ line_pos = 115
$ ff = ""
$ old_file_name = ""
$ new_file_name = ""
$ on error then goto done
$ on warning then goto done
$ on control_y then goto done
$ blanks = "                                                                                                                      " 
$ write sys$output ""
$ write sys$output "DIFF New and Old .TYPCHK files to determine if warnings eliminated or introduced."
$ write sys$output ""
$GET_NEW:
$ write sys$output ""
$ inquire new "Enter NEW  .TYPCHK File "
$ if new .eqs. "" then goto EXIT
$ i = f$elem(1,"]",new)
$ if i .eqs. "]" then i = new
$ if f$loc(".",i) .eq. f$len(i) then new = new + ".TYPCHK"
$ new_name = f$search(new)
$ if new_name .nes. "" then goto GET_OLD_FILE
$ write sys$output ""
$ write sys$output "Error - No such File: ",new
$ goto GET_NEW

$GET_OLD_FILE:
$ write sys$output ""
$ cnt = 0
$ last_file = ""
$ old_name := 'f$ele(0,";",new_name)";*"
$GO10:
$ file = f$sear(old_name)
$ if file .eqs. new_name then goto GO10
$ if file .eqs. "" then goto GET_OLD
$ cnt = cnt + 1
$ nam'cnt' = file
$ if last_file .eqs. "" then last_file = file
$ write sys$output cnt,") ",file,"	",f$file(file,"EOF"),"	",f$file(file,"CDT")
$ goto GO10

$GET_OLD:
$ write sys$output ""
$ i := "Enter OLD  .TYPCHK File"
$ if CNT .GT. 0 then i = i + " or List Number [ " + last_file + " ] "
$ i = """" + i + """"
$ inquire old 'i'
$ if old .nes. "" then goto GETO10
$ if cnt .le. 0 then goto GET_NEW
$ old_name = last_file
$ goto DO_COMP

$GETO10:
$ if (f$type(old) .nes. "INTEGER") .or. (cnt .le. 0) then goto GETO20
$ if (old .lt. 1) .or. (old .gt. cnt) then goto GETO15
$ old_name = nam'old'
$ goto DO_COMP

$GETO15:
$ write sys$output ""
$ write sys$output "Error - No such List Number: ",old
$ goto GET_OLD

$GETO20:
$ i = f$elem(1,"]",old)
$ if i .eqs. "]" then i = old
$ if f$loc(".",i) .eq. f$len(i) then old = old + ".TYPCHK"
$ old_name = f$search(old)
$ if old_name .nes. "" then goto DO_COMP
$ write sys$output ""
$ write sys$output "Error - No such File: ",old
$ goto GET_OLD

$DO_COMP:
$ write sys$output ""
$ write sys$output ""
$ write sys$output "Comparing New File:  ",New_name
$ write sys$output "       to Old File:  ",old_name
$ write sys$output ""
$ write sys$output ""
$!
$! determine if VAX or Alpha
$!
$ open mar_file 'new_name
$ read/end=DONE mar_file line
$ close mar_file
$ alpha=""
$ if f$loc(" VAX MACRO ",line) .ne. f$len(line) then alpha = 0
$ if f$loc(" AMAC ",line) .ne. f$len(line) then alpha = 1
$ if alpha .eqs. "" then goto NOT_MACRO_FILE
$!
$ old_file_name = old_name
$ if f$loc("]",old_file_name) .ne. f$len(old_file_name) then tmp = f$ele(1,"]",old_file_name)
$ old_file_name = f$ele(0,".",tmp)
$ tmp = f$ele(1,".",tmp)
$ old_file_name = old_file_name + "_" + f$ele(0,";",tmp) + "_" + f$ele(1,";",tmp)
$ search/number/win=(1,0)/out='old_file_name'.TYPCHK_OTEMP 'old_name'  "WARN: "
$!
$ new_file_name = new_name
$ if f$loc("]",new_file_name) .ne. f$len(new_file_name) then tmp = f$ele(1,"]",new_file_name)
$ new_file_name = f$ele(0,".",tmp)
$ save_file_name = new_file_name
$ tmp = f$ele(1,".",tmp)
$ new_file_name = new_file_name + "_" + f$ele(0,";",tmp) + "_" + f$ele(1,";",tmp)
$ search/number/win=(1,0)/out='new_file_name'.TYPCHK_NTEMP 'new_name'  "WARN: "
$!
$! Reformat new search file, Strip out normal variable characters like the listing line number, or the comment from the instr
$! Place search line numbers at end of record both instruction and warning records then do DIFF
$! Note if two diffs in a row then replace the first diff by the last instruction found.
$! A wrinkle is that the warnings could be on MACRO expansions.
$!
$ open sear_file  'new_file_name'.TYPCHK_NTEMP
$ open/write diff_file  'new_file_name'.TYPCHK_NTEMP1
$NO_LI:
$ last_inst = ""
$GET_NEXT_WARN:
$ read/end_of_file=DO_OLD sear_file line
$ if f$extr(0,6,line) .eqs. "******" then goto NO_LI ! ignore separator lines
$ if f$extr(7,6,line) .eqs. "WARN: " then goto PROC_WARN
$!
$! Process instruction
$!
$ line1 = f$ele(0,";",line)
$ line1 = f$extr(7,f$len(line1)-7,line1)
$ if alpha then str = 39
$ if alpha .and. (f$extr(31,1,line1) .eqs. "	") then str = 31 ! if alpha macro expansion
$ if .not. alpha then str = 48
$ if .not. alpha .and. (f$extr(42,5,line1) .eqs. "     ") then str = 42 ! if VAX macro expansion
$ fill = str - 1
$ if alpha then fill = str
$ inst = f$extr(0,fill,blanks) + f$extr(str,f$len(line1),line1)
$ goto GET_NEXT_WARN

$!
$! process warning
$! 
$PROC_WARN:
$ line1 = f$extr(7,f$len(line)-7,line)
$ i = 0
$ if f$len(line1) .lt. line_pos then i = line_pos-f$len(line1)
$ if f$len(line1) .gt. line_pos then line1 = f$extr(0,line_pos,line1)
$ write diff_file line1,f$extr(0,i,blanks),inst,FF,f$ele(1,"]",new_name)," Line: ",+f$extr(0,6,line)
$ goto GET_NEXT_WARN
$!
$DO_OLD:
$ close sear_file
$ close diff_file
$!
$! reformat old search file
$!
$ open sear_file  'old_file_name'.TYPCHK_OTEMP
$ open/write diff_file  'old_file_name'.TYPCHK_OTEMP1
$NO_LI1:
$ last_inst = ""
$GET_NEXT_WARN1:
$ read/end_of_file=DO_DIFF sear_file line
$ if f$extr(0,6,line) .eqs. "******" then goto NO_LI1 ! ignore separator lines
$ if f$extr(7,6,line) .eqs. "WARN: " then goto PROC_WARN1
$!
$! Process instruction
$!
$ line1 = f$ele(0,";",line)
$ line1 = f$extr(7,f$len(line1)-7,line1)
$ if alpha then str = 39
$ if alpha .and. (f$extr(31,1,line1) .eqs. "	") then str = 31 ! if alpha macro expansion
$ if .not. alpha then str = 48
$ if .not. alpha .and. (f$extr(42,5,line1) .eqs. "     ") then str = 42 ! if VAX macro expansion
$ fill = str - 1
$ if alpha then fill = str
$ inst = f$extr(0,fill,blanks) + f$extr(str,f$len(line1),line1)
$ goto GET_NEXT_WARN1

$!
$! process warning
$! 
$PROC_WARN1:
$ line1 = f$extr(7,f$len(line)-7,line)
$ i = 0
$ if f$len(line1) .lt. line_pos then i = line_pos-f$len(line1)
$ if f$len(line1) .gt. line_pos then line1 = f$extr(0,line_pos,line1)
$ write diff_file line1,f$extr(0,i,blanks),inst,FF,f$ele(1,"]",old_name)," Line: ",+f$extr(0,6,line)
$ goto GET_NEXT_WARN1

$!
$! Diff reformated files to scratch file
$!
$DO_DIFF:
$ close sear_file
$ close diff_file
$ diff/nonumber/merge=0/match=1/width=264/ignore=(comment,exact)/comment=form_feed/out='new_file_name'.TYPCHK_TEMP2 'new_file_name'.TYPCHK_NTEMP1 'old_file_name'.TYPCHK_OTEMP1
$!
$! Now reformat the diff file in a readable manner to the screen or to a save file if P1 was set
$!
$ open diff_file  'new_file_name'.TYPCHK_TEMP2
$ if out_scr then goto OUT1
$ write sys$output "DIFFs being placed in file: ",save_file_name,".TYPCHK_DIFF" 
$ write sys$output ""
$ define sys$output  'save_file_name'.TYPCHK_DIFF 
$ write sys$output ""
$ write sys$output "	TYPCHK_DIFF - TYPCHK files differences command file output."
$ write sys$output ""
$ write sys$output ""
$ write sys$output "Comparing New File:  ",New_name
$ write sys$output "       to Old File:  ",old_name
$ write sys$output ""
$ write sys$output ""
$ write sys$output "WARNINGs are identified by the FILENAME of filespec and by the line number at end of each Warning line."
$ write sys$output ""
$ write sys$output ""
$ goto GET_NEXT_DIFF
$OUT1:
$ write sys$output "WARNINGs are identified by the FILENAME of filespec and by the line number at end of each Warning line."
$ write sys$output ""
$ write sys$output ""
$ goto GET_NEXT_DIFF
$OUT_LINE:
$ if f$extr(0,36,line) .eqs. "Number of difference records found: " then goto DIFF_DONE
$ write sys$output line
$GET_NEXT_DIFF:
$ read/end=diff_done diff_file line
$ if f$extr(0,6,line) .nes. "WARN: " then goto out_line
$ warn = f$extr(0,line_pos,line)
$ inst = f$extr(line_pos-1,f$len(line),line)
$ line_num = f$ele(1,"",inst)
$ inst = f$ele(0,"",inst)
$ write sys$output inst
$ write sys$output warn,f$ele(1," ",line_num)," ",f$ele(2," ",line_num)
$ goto GET_NEXT_DIFF

$NOT_MACRO_FILE:
$ write sys$output ""
$ write sys$output "Error - Not a Macro Listing file: ",new_name
$ write sys$output ""
$ goto exit

$DIFF_DONE:
$ write sys$output "Number of different warnings found:",f$ele(1,":",line)
$ write sys$output ""
$ if .not. out_scr then deass sys$output 
$DONE:
$ close/nolog mar_file
$ close/nolog sear_file
$ close/nolog diff_file
$ set mess/nofac/noident/notext/nosever
$ set noon
$ if old_file_name .nes. "" then delete 'old_file_name'.TYPCHK_OTEMP.
$ if old_file_name .nes. "" then delete 'old_file_name'.TYPCHK_OTEMP1.
$ if new_file_name .nes. "" then delete 'new_file_name'.TYPCHK_NTEMP.     
$ if new_file_name .nes. "" then delete 'new_file_name'.TYPCHK_NTEMP1.
$ if new_file_name .nes. "" then delete 'new_file_name'.TYPCHK_TEMP2.
$ set mess/fac/ident/text/sever
$ set on
$!
$EXIT:
$ exit
$!
$! DIFF_WARN.COM - is a command file to DIFF warnings on a new and old version of a TYPCHK file to sense if any new
$! warnings have been eliminated or introduced. Uses scratch files with File Extension of *.TYPCHK_(N/O)TEMP(/1/2). 
$!
$! X-1		PJH		Paul J. Houlihan 		2-MAR-1991
$!		Created Command file.
$!
