$ sav_verify = 'f$verify(0)
$ write sys$output "%OBSFAC-I-BOJ, Begin of job at ''f$time()"
$!+
$! Documentation at end of command file
$!+
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$ context = ""
$!
$ set noon
$!
$!  validate the root and set to the correct VDE library
$!
$ root = P1
$ @vde$system:plmenu$$set_vde_library 'root'
$ if .not. $status then GOTO Error_Exit
$!
$ facility = P2
$ if f$search("''root':[000000]''facility'.DIR") .eqs. ""
$ then
$   write sys$output "%OBSFAC-F-NOTFOUND, ''ROOT':[''FACILITY'] not found"
$   GOTO Error_Exit
$ endif
$!
$ stream = P3
$ if stream .eqs. "" then GOTO Error_Exit
$!
$ option = P4
$ if option .eqs. "" then option = "OBSOLETE"
$!
$!+
$! Put the archived facility flag in the facility directory.  This flag prevents
$! the CMS library for this facility from being populated by the populate stream
$! tools and the create stream tools.  It also prevents the library from being
$! processed by most of the other tools.  
$!+
$!
$ if option .eqs. "ARCHIVE"
$ then
$   if f$search("''root':[''facility']VMSCMS$ARCHIVED_FACILITY.FLAG") .EQS. ""
$   then
$	CREATE 'root':['facility']VMSCMS$ARCHIVED_FACILITY.FLAG
$   endif
$ endif
$!
$ GOSUB Build_Stream_Succ_List
$!
$Remove_VDE_Generations:
$!----------------------
$ index = -1
$ PID = f$getjpi("","PID")
$!
$Remove_Gen_Loop:
$!---------------
$ index = index + 1
$ define/nolog cms$lib 'root':['facility'.CMS]
$ stream = f$element(index,",",stream_succ_list)
$ if stream .eqs. "," then GOTO Remove_Gen_Loop_End
$ if stream .eqs. "" then GOTO Remove_Gen_Loop_End
$ outfile = "''PID'_''stream'" - "." - "."
$ outfile = outfile + ".COM" 
$ cms show generation/output='outfile'/generation='stream' -
  /format="$ VDE/KEPT REMOVE GEN /REMARK=""Obsolete Facility [*DEL_HIST*]"" [''facility']#e/STREAM=''stream' "
$!
$! remove the generations from the class
$!
$ if f$search("''outfile'") .nes. ""
$ then
$   @'outfile'
$   delete/nolog 'outfile';*
$ endif
$!
$! delete the class
$!
$ define/user sys$error _nla0:
$ define/user sys$output _nla0:
$ CMS delete class 'stream' "Facility not active in ''stream' [*DEL_HIST*]"
$!
$ GOTO Remove_Gen_Loop 
$!
$Remove_Gen_Loop_End:
$
$!
$MAIN_EXIT:
$ if f$search("''pid'_*.com") .nes. "" then delete/nolog 'PID'_*.COM;*
$ if f$trnlnm("succ_file") .nes. "" then close succ_file
$ if f$search("''succ_PID'_streams.lis") .nes. "" -
  then delete/nolog 'succ_PID'_streams.lis;*
$ EXIT 1+0*'f$verify(sav_verify)
$!
$ERROR_EXIT:
$ if f$search("''pid'_*.com") .nes. "" then delete/nolog 'PID'_*.COM;*
$ if f$trnlnm("succ_file") .nes. "" then close succ_file
$ if f$search("''succ_PID'_streams.lis") .nes. "" -
  then delete/nolog 'succ_PID'_streams.lis;*
$ EXIT 0+0*'f$verify(sav_verify)
$!
$ 
$!+-----------------------------------------------------------------------------
$! Build_Stream_Succ_List -
$!
$!  This subroutine builds a list of a stream and its successor streams.  The
$! list produced is store in symbol stream_succ_list.
$!
$! INPUTS -
$!	logical VDE$LIBRARY must be defined to the VDE library to use
$!
$!	symbol stream should contain the stream name for which to begin the
$!	successor chain.  The list the stream passed in P3 and
$!	any of it's successor streams.
$!
$!+-----------------------------------------------------------------------------
$Build_Stream_Succ_List:
$!----------------------
$ succ_PID = f$getjpi("","PID")
$ stream_succ_list = ""
$ VDE/KEPT show stream/successor/out='succ_PID'_streams.lis  'stream' 
$!
$ open/read succ_file 'succ_PID'_streams.lis
$ if .not. $status then GOTO Error_Exit
$!
$! throw away the first two records
$!
$ read/end_of_file=EOF succ_file succ_rec
$ if .not. $status then GOTO Error_Exit
$ read/end_of_file=EOF succ_file succ_rec
$ if .not. $status then GOTO Error_Exit
$!
$! the remaining records should contain the successor information
$!
$Read_loop:
$ read/end_of_file=EOF succ_file succ_rec
$ if .not. $status then GOTO Error_Exit
$!
$ index = -1
$Succ_Loop:
$!---------
$ index = index + 1
$ succ_stream = f$edit(f$element(index, ">", succ_rec),"TRIM")
$ succ_stream = f$extract(0,f$locate(" ",succ_stream),succ_stream)
$ if succ_stream .eqs. "" then GOTO succ_loop
$ if succ_stream .eqs. ">" then GOTO Read_Loop
$ if succ_stream .eqs. "-" then GOTO succ_loop
$!
$ stream_succ_list = stream_succ_list + succ_stream + ","
$!
$ GOTO Succ_Loop
$!
$EOF:
$ if f$trnlnm("succ_file") .nes. "" then close succ_file
$ if f$search("''succ_PID'_streams.lis") .nes. "" -
  then delete/nolog 'succ_PID'_streams.lis;*
$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$!
$! OBSFAC.COM  - Obsolete Facility
$!
$!  Description:
$!
$!	Obsoletes the facility named in P2 in the root passed in P1 beginning at
$! the stream passed in P3.  
$!
$!	The file VMSCMS$ARCHIVED_FACILITY.FLAG is created in the facility
$! directory.  Generations for all modules in the facility are removed from the
$! stream passed in P3 and any of it's successor streams.
$!
$!  Modification History
$!
$!  May-1992	    DJS		Original Version
$!		    SRH         Integrated into VDE environment
$!
$!+-----------------------------------------------------------------------------
