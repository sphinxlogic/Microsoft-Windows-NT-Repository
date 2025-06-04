$ ! Procedure:	MANGLE.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$
$err: subroutine
$ set noon
$ severity = f$edit(P1,"COLLAPSE,UPCASE")
$ identification = f$edit(P2,"COLLAPSE,UPCASE")
$ text = f$edit(P3,"TRIM")
$ continuation = (f$edit(P4,"COLLAPSE,UPCASE") .nes. "")
$ percent = "%"
$ if (continuation) then percent = "-"
$ if (severity .eqs. "")
$ then say "%''facility'-F-NOSEVER, severity missing in call to ERR"
$ else if (identification .eqs. "")
$ then say "%''facility'-F-NOIDENT, identification missing in call to ERR"
$ else if (text .eqs. "")
$ then say "%''facility'-F-NOTEXT, text missing in call to ERR"
$ else say "''percent'''facility'-''severity'-''identification', ''text'"
$ endif !(text .eqs. "")
$ endif !(identification .eqs. "")
$ endif !(severity .eqs. "")
$exit_err:
$ exitt 1
$ endsubroutine
$
$ procedure = f$element(0,";",f$environment("PROCEDURE"))
$ procedure_name = f$parse(procedure,,,"NAME")
$ facility = procedure_name
$ location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
		+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$ set noon
$ on control_y then goto bail_out
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ say = "write sys$output"
$ err = "call err"
$
$ valid_options = "ADD-S/AFTER-N/AT-N/BEFORE-N/CHOP-N/DELETE-N/" -
		+ "DIRECTORY-S/FILES-S/INSERT-S/ITEM-S/PREFIX-S/" -
		+ "REMOVE-S/REPLACE-S/SWAP/TEST/TRIM-N/WITH-S"
$
$ error_issued = 0
$ smash_add = ""
$ smash_after = 0
$ smash_at = 0
$ smash_before = 0
$ smash_chop = 0
$ smash_delete = 0
$ smash_directory = ""
$ smash_insert = ""
$ smash_item = ""
$ smash_files = ""
$ smash_prefix = ""
$ smash_remove = ""
$ smash_replace = ""
$ smash_swap = 0
$ smash_test = 0
$ smash_trim = 0
$ smash_with = ""
$ count = 0
$loop0:
$   count = count + 1
$   if (count .gt. 8) then goto end_loop0
$   parameter = f$edit(P'count',"COLLAPSE,UPCASE")
$   if (smash_test) then err I PARAM "P''count' = <''parameter'>"
$   option = f$element(0,"=",parameter)
$   option_len = f$length(option)
$   if (option_len .eq. 0) then goto loop0
$   value = parameter - option - "="
$   value_len = f$length(value)
$   ocount = 0
$   valid_found = 0
$loop1:
$     v_option = f$element(ocount,"/",valid_options)
$     ocount = ocount + 1
$     if (v_option .eqs. "") then goto loop1
$     if (v_option .eqs. "/") then goto end_loop1
$     oname = f$element(0,"-",v_option)
$     otype = f$element(1,"-",v_option)
$     if (otype .eqs. "-") then otype = ""
$     if (option .eqs. f$extract(0,option_len,oname))
$       then
$       valid_found = 1
$       if ((otype .nes. "") .and. (value_len .eq. 0))
$         then err E BADVALUE "no value supplied with ''oname'"
$         error_issued = 1
$       else
$       if (otype .eqs. "")
$         then smash_'oname' = 1
$       else
$       if (otype .eqs. "S")
$         then smash_'oname' = value
$       else
$       if (otype .eqs. "N")
$         then
$         if (value .nes. f$integer(value))
$           then err E NOTNUMBER "''oname' requires a numeric value"
$           error_issued = 1
$         else smash_'oname' = f$integer(value)
$         endif
$       endif
$       endif
$       endif
$       endif
$     endif
$    if (.not. valid_found) then goto loop1
$end_loop1:
$   if (.not. valid_found)
$     then err E BADPARAM "parameter <''parameter'> is invalid"
$     error_issued = 1
$   endif
$  goto loop0
$end_loop0:
$ if (error_issued) then goto bail_out
$
$ if (smash_item .eqs. f$extract(0,f$length(smash_item),"NAME"))
$   then smash_item = "NAME"
$ else
$ if (smash_item .eqs. f$extract(0,f$length(smash_item),"TYPE"))
$   then smash_item = "TYPE"
$ else
$ err E BADITEM "item must be NAME or TYPE"
$ error_issued = 1
$ endif
$ endif
$ if (smash_files .eqs. "")
$   then err E NOFILES "no files were selected for processing"
$   error_issued = 1
$ endif
$ if ((smash_replace .nes. "") .and. (smash_with .eqs. ""))
$   then err E NOWITH "REPLACE used but no WITH"
$   error_issued = 1
$ endif
$ if ((smash_replace .eqs. "") .and. (smash_with .nes. ""))
$   then err E NOREPLACE "WITH used but no REPLACE"
$   error_issued = 1
$ endif
$ if ((smash_insert .nes. "") .and. -
	(smash_after .eq. 0) .and (smash_before .eq. 0))
$   then err E NOAFTER "INSERT used but no BEFORE or AFTER"
$   error_issued = 1
$ endif
$ if ((smash_before .ne. 0) .and. (smash_after .ne. 0))
$   then err E CHOOSE "you cannot use both BEFORE and AFTER"
$   error_issued = 1
$ endif
$ if ((smash_insert .eqs. "") .and. -
	((smash_after .ne. 0) .or. (smash_before .ne. 0)))
$   then err E NOINSERT "BEFORE or AFTER used but no INSERT"
$   error_issued = 1
$ endif
$ if ((smash_insert .nes. "") .and. (smash_prefix .nes. ""))
$   then err E CONFLICT "conflicting user of AFTER and PREFIX"
$   error_issued = 1
$ endif
$ if ((smash_delete .ne. 0) .and. (smash_at .eq. 0))
$   then err E NOAT "DELETE used but no AT"
$   error_issued = 1
$ endif
$ if ((smash_delete .eq. 0) .and. (smash_at .ne. 0))
$   then err E NODELETE "AT used but no DELETE"
$   error_issued = 1
$ endif
$ if (error_issued) then goto bail_out
$
$ searchname = f$parse(smash_files,,,"NAME")
$ searchtype = f$parse(smash_files,,,"TYPE") - "."
$ searchspec = searchname + "." + searchtype
$ if (smash_test) then err I SEARCH "searching for <''searchspec'>"
$ if ((smash_item .eqs. "NAME") .and. (searchname .eqs. "*"))
$   then err E LOOPALERT "use of * as filename will lead to a loop"
$   error_issued = 1
$ endif
$ if ((smash_item .eqs. "TYPE") .and. (searchtype .eqs. "*"))
$   then err E LOOPALERT "use of * as filetype will lead to a loop"
$   error_issued = 1
$ endif
$ if (error_issued) then goto bail_out
$
$s_loop:
$   filespec = f$search(searchspec,81723)
$   if (filespec .eqs. "") then goto end_s_loop
$   inputname = f$parse(filespec,,,"NAME")
$   inputtype = f$parse(filespec,,,"TYPE") - "."
$   outputname = inputname
$   outputtype = inputtype
$   if (smash_test) then err I INPUT "from ''inputname'.''inputtype'"
$   if (smash_item .eqs. "NAME")
$     then work = inputname
$   else work = inputtype
$   endif
$   if (smash_chop .ne. 0) then work = f$extract(smash_chop,9999,work)
$   if (smash_delete .ne. 0)
$     then
$     if (smash_at .lt. f$length(work))
$       then
$       work = f$extract(0,smash_at,work) -
		+ f$extract(smash_at+smash_delete,9999,work)
$     endif
$   endif
$   if (smash_insert .nes. "")
$     then
$     if (smash_after .ne. 0)
$       then
$       if (f$length(work) .ge. smash_after)
$         then
$         work = f$extract(0,smash_after,work) -
		+ smash_insert -
		+ f$extract(smash_after,9999,work)
$       endif
$     else
$     if (smash_before .ne. 0)
$       then
$       if (f$length(work) .ge. smash_before)
$         then
          work = f$extract(0,f$length(work)-smash_before,work) -
		+ smash_insert -
		+ f$extract(f$length(work)-smash_before,9999,work)
$       endif
$     endif
$     endif
$   endif
$   if (smash_trim .ne. 0) then -
$     work = f$extract(0,f$length(work)-smash_trim,work)
$   if (smash_remove .nes. "") then work = work - smash_remove
$   if (smash_replace .nes. "")
$     then
$     where = f$locate(smash_replace,work)
$     if (where .ne. f$length(work))
$       then
$       work = f$extract(0,where,work) -
		+ smash_with -
		+ f$extract(where+f$length(smash_replace),9999,work)
$     endif
$   endif
$   work = smash_prefix + work + smash_add
$   if (smash_item .eqs. "NAME")
$     then outputname = work
$   else outputtype = work
$   endif
$   if (f$length(work) .gt. 39) then -
$     err W TOOBIG "filename too big with ''inputname'.''inputtype'"
$   if (f$length(work) .gt. 39) then goto s_loop
$   if (smash_swap)
$     then
$     work = outputtype
$     outputtype = outputname
$     outputname = work
$   endif
$   if (smash_test)
$     then err I OUTPUT "to  ''smash_directory'''outputname'.''outputtype'" Y
$   else
$   if ((outputname .eqs. inputname) .and. (outputtype .eqs. inputtype))
$     then err I NOMANGLE "file ''outputname'.''outputtype' not mangled"
$   else
$   renamee/log 'inputname'.'inputtype' -
		'smash_directory''outputname'.'outputtype'
$   endif
$   endif
$  goto s_loop
$end_s_loop:
$
$bail_out:
$ exitt 1.or.f$verify(__vfy_saved)
$ !+==========================================================================
$ !
$ ! Procedure:	MANGLE.COM
$ !
$ ! Purpose:	
$ !
$ ! Parameters:
$ !		FILES={filespec}
$ !		ITEM={NAME|TYPE}
$ !		CHOP={n}  chops n characters from the front
$ !		TRIM={n}  trims n characters from the end
$ !		REMOVE={string}
$ !		ADD={string}
$ !		PREFIX={string}
$ !		REPLACE={string}
$ !		  WITH={string}
$ !		INSERT={string}
$ !		  AFTER={n}
$ !		  BEFORE={n}
$ !		DELETE={m}
$ !		  AT={n}
$ !		DIRECTORY={string}
$ !		SWAP
$ !		TEST
$ !
$ !		NO spaces when specifying arguments e.g. FILE=*.EXE is ok
$ !		but FILE = *.EXE will result in an error.
$ !
$ ! History:
$ !		03-Dec-1999, DBS; Version V1-001
$ !	001 -	Original version.
$ !		15-Jun-2000, DBS; Version V1-002
$ !	002 -	Added DIRECTORY option to allow renaming to another location.
$ !		24-Jul-2000, DBS; Version V1-003
$ !	003 -	Added SWAP option to swap the filename and filetype as the
$ !		last step before the rename.
$ !-==========================================================================
