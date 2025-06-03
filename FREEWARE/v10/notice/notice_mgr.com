$ save_verify = 'f$verify(0)'
$!
$!  File:	NOTICE.COM
$!
$!  Author:	Hunter Goatley
$!		Copyright © 1992--1994, MadGoat Software.  All rights reserved.
$!
$!  Date:	February 24, 1992
$!
$!  Abstract:
$!
$!	This command procedure is used to maintain the NOTICE database
$!	(a text library).
$!
$!  Modified by:
$!
$!	01-003		Hunter Goatley		14-APR-1994 09:01
$!		Add rename option, don't force /SYSTEM when looking up
$!		NOTICE_LIBRARY.
$!
$!	01-002		Hunter Goatley		 6-MAY-1992 09:23
$!		Added option to create a new library.
$!
$!	01-001		Hunter Goatley		23-APR-1992 15:17
$!		Added ability to touch and edit a module.
$!
$!	01-000		Hunter Goatley		24-FEB-1992 07:27
$!		Genesis.
$!
$ say = "write sys$output"
$ the_library = f$trnlnm("NOTICE_LIBRARY")
$ if the_library .eqs. ""
$ then	say "NOTICE library logical NOTICE_LIBRARY has not been defined"
$	goto common_exit
$ endif
$ library = "library"
$ libcmd = "LIBRARY/TEXT/LOG"
$ delete = "delete/nolog/noconfirm"
$!
$ if f$search(the_library) .eqs.""
$ then	say "Library ''the_library' not found!"
$	exit
$ endif
$ on contrl_y then goto common_exit
$!
$!  Ask for the option desired
$!
$ menu_loop:
$	on error then goto common_exit
$	say ""
$	say "NOTICE library maintenance"
$	say ""
$	say "  1.  List the library contents"
$	say "  2.  Add/replace a module to/in the library"
$	say "  3.  Touch a module in the library (update date)"
$	say "  4.  Edit a module in the library"
$	say "  5.  Rename a module in the library"
$	say "  6.  Delete a module from the library"
$	say "  7.  Create a new library"
$	say "  0.  Exit"
$	say ""
$	read/error=common_exit sys$command choice/prompt="Choice <0>: "
$	say ""
$	if choice .eqs. "" .or. choice.eqs."0" then goto common_exit
$	if choice .eqs. "1" then goto List_Contents
$	if choice .eqs. "2" then goto Add_Module
$	if choice .eqs. "3" then goto Touch_Module
$	if choice .eqs. "4" then goto Edit_Module
$	if choice .eqs. "5" then goto Rename_Module
$	if choice .eqs. "6" then goto Delete_Module
$	if choice .eqs. "7" then goto Create_Library
$	say "Invalid choice"
$	goto menu_loop
$ List_Contents:
$	'libcmd'/LIST/FULL 'the_library'
$	say ""
$	read/error=common_exit sys$command choice-
		/prompt="Press RETURN to continue"
$	goto menu_loop
$ Add_Module:
$	read/error=Menu_Loop sys$command filename/prompt="Source file: "
$	if filename.eqs."" then goto Add_Module
$	filename = f$edit(filename,"UPCASE")
$	file = f$search(filename)
$	choice = ""
$	if file .eqs. ""
$	then	read/error=Menu_Loop sys$command choice -
		/prompt="File ''filename' doesn't exist.  Create it <Y>? "
$		if choice.eqs."" then choice = "Y"
$		if choice
$		then define/user sys$input sys$command:
$		     edit/tpu 'filename'
$		     file = f$search(filename)
$		     if file.eqs.""
$		     then say "No module created or added"
$			  goto Menu_Loop
$		     endif
$		else goto Menu_Loop
$		endif
$	endif
$	filename = f$parse(file,"","","NAME")
$ add_get_modname:
$	read/error=Menu_Loop sys$command module-
		/prompt="Module name <''filename'>: "
$	if module .eqs. "" then module = filename
$	module = f$edit(module,"UPCASE,TRIM")
$	if f$length(module).gt.9
$	then	say "Module name is too long (maximum length is 9 characters)"
$		goto add_get_modname
$	endif
$	read/error=Menu_Loop sys$command choice-
		/prompt="Add file ''file' as module ''module' <Y>? "
$	on error then goto menu_loop
$	if choice .or. (choice.eqs."") then -
		'libcmd'/replace 'the_library' 'file'/module='module'
$	if choice
$	then	read/error=Menu_Loop sys$command choice-
		/prompt="Delete temporary file ''file' <Y>? "
$		if choice.eqs."" then choice = "Y"
$		if choice then delete 'file'
$	endif
$	goto menu_loop
$ Delete_Module:
$	read/error=Menu_Loop sys$command module/prompt="Module to delete: "
$	if module.eqs."" then goto Delete_Module
$	module = f$edit(module,"UPCASE")
$	read/error=Menu_Loop sys$command choice-
		/prompt="Delete module ''module' <Y>? "
$	on error then goto menu_loop
$	if choice .or. choice.eqs."" then -
		'libcmd'/DELETE='module' 'the_library'
$	goto menu_loop
$ Rename_Module:
$	rename_it = 1
$	edit_it = 0
$	goto Do_Module_Update
$ Edit_Module:
$	rename_it = 0
$	edit_it = 1
$	goto Do_Module_Update
$ Touch_Module:
$	rename_it = 0
$	edit_it = 0
$!$	goto Do_Module_Update
$ Do_Module_Update:
$	temp_file = "SYS$SCRATCH:NOTICE''f$pid(0)'.TMP"
$	if edit_it
$	then which_update = "edit"
$	else if rename_it
$	     then which_update = "rename"
$	     else which_update = "touch"
$	     endif
$	endif
$  Do_Module_Update_Prompt:
$	read/error=Menu_Loop sys$command module-
		/prompt="Module to ''which_update': "
$	if module.eqs."" then goto Do_Module_Update_Prompt
$	module = f$edit(module,"UPCASE")
$	read/error=Menu_Loop sys$command choice-
		/prompt="Really ''which_update' module ''module' <Y>? "
$	on error then goto menu_loop
$	if choice .or. choice.eqs.""
$	then	'libcmd'/EXTRACT='module'/out='temp_file' 'the_library'
$		if rename_it
$		then	defname = ""
$			if f$extract(0,1,module).eqs."$" then -
					defname = f$extract(1,255,module)
$ rename_get_modname:
$			read/error=Menu_Loop sys$command newname-
				/prompt="New module name <''defname'>: "
$			if f$length(newname).gt.9
$	then	say "Module name is too long (maximum length is 9 characters)"
$				goto rename_get_modname
$			endif
$			if newname.eqs."" then newname = defname
$			if newname.eqs.""
$			then	say "Module not renamed"
$				goto Menu_Loop
$			endif
$			'libcmd'/DELETE='module' 'the_library'
$			module = newname
$		endif
$		if edit_it
$		then	define/user sys$input sys$command
$			edit/tpu 'temp_file'
$		endif
$		if f$search(temp_file).nes.""
$		then  'libcmd'/REPLACE 'the_library' 'temp_file'/MODULE='module' 
$			delete 'temp_file';*
$		endif
$		say "Module ''module' ''which_update'ed"
$	else	say "Module ''module' not ''which_update'ed"
$	endif
$	goto Menu_Loop
$ Create_Library:
$ read/error=Menu_Loop sys$command choice-
		/prompt="Really create a new library <N>? "
$ if choice.eqs."" then choice = 0
$ if choice
$ then	read/error=Menu_Loop sys$command libname -
		/prompt="Filename for new library: "
$	if libname .nes. ""
$	then	libname = f$parse(libname,".TLB")
$		library/text/create/log 'libname'
$		set file/prot=w:r 'libname'
$		define/system/exec notice_library 'libname'
$		say "NOTICE_LIBRARY defined as ''libname'"
$	else	say "No library created"
$	endif
$ else	say "No library created"
$ endif
$ goto Menu_Loop
$ common_exit:
$	exit f$verify(save_verify).or.1
