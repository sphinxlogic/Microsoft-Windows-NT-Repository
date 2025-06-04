$ sav_verify = 'f$verify(0)
$!+
$! documentation is at the end of this command file
$!-
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Error_Exit
$ set noon
$ submit = "submit/noprint/keep"
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$ owner = user
$!
$ @vde$system:plmenu$$set_vde_library 'p1'
$ if .not. $status then GOTO Error_Exit
$ root = f$trnlnm("PLMENU$CMSROOT")
$ mnemonic = f$trnlnm("PLMENU$MNEMONIC")
$!
$ if p2 .eqs. ""
$ then
$   write sys$output "%CRESTRM-F-NOSTREAM, No stream name specified for parent"
$   GOTO Error_Exit
$ else
$   source = p2
$ endif
$!
$ if p3 .eqs. ""
$ then
$   write sys$output "%CRESTRM-F-NOSTREAM, No stream name specified for new stream"
$   GOTO Error_Exit
$ else
$   target = P3
$ endif
$! 
$ if p4 .eqs. ""
$ then
$   remark = "Build stream for ''SOURCE'"
$ else
$   remark = P4
$ endif
$!
$ if p5 .eqs. ""
$ then
$   batch_queue = "SYS$LOBUILD"
$ else
$   batch_queue = P5
$ endif
$!
$ if p6 .eqs. ""
$ then
$   work_dir = "SYS$LOGIN"
$ else
$   work_dir = P6
$ endif
$!
$ if p7 .eqs. ""
$ then
$   write sys$output "%CRESTRM-F-NOGENLEN, No generation length specified for new stream"
$   GOTO Error_Exit
$ else
$   variant = P7
$ endif
$!
$! See if the VDE$FETCH_METHOD logical is defined to VDE.
$! If the logical is defined to VDE, create and populate the
$! stream in the VDE database only.  If the logical is undefined,
$! the stream will be created in both VDE and CMS.
$!
$ stream_fetch_method = f$trnlnm("VDE$FETCH_METHOD")
$ stream_fetch_method = F$edit(stream_fetch_method,"Trim,UPCASE")
$!
$! Due to the base_level update mechanism for the VMS build,
$! the stream must be created in the SYS facility CMS library
$! 1/19/93 - DJS
$!
$ if stream_fetch_method .NES. "VDE"
$ then
$   GOSUB Create_CMS_Classes
$ else
$   if (mnemonic .eqs. "VMS") .or. (mnemonic .eqs. "EVMS")
$   then
$       write sys$output "%CRESTRM-I-ASSUME, assuming ''mnemonic' is an OpenVMS library"
$       write sys$output "%CRESTRM-I-CMSSYSCLS, adding class ''target' to [SYS] facility"
$	define/nolog CMS$LIB 'root':[SYS.CMS]
$	CMS Create class 'target' "''remark' [*DEL_HIST*]"
$   endif
$ endif
$!
$ GOSUB Create_VDE_Stream
$!
$ GOSUB Populate_Stream
$
$Main_Exit:
$ EXIT 1+0*'f$verify(sav_verify)
$!
$Error_Exit:
$ EXIT 0+0*'f$verify(sav_verify)
$!
$!
$!+----------------------------------------------------------------------------
$! perform the CMS command to create an empty(no generations) class in all the
$! non-archived, non nopropagate facilities.  Check if there exists a facility
$! with the same name as the source stream, if the facility exists and it is
$! no propagate, create the class in the facility.
$!
$! The loop through the letters of the alphabit is done to avoid encountering a
$! CMS virtual memory problem which occurs when processing all libraries in one
$! FACSERIAL command.
$!+----------------------------------------------------------------------------
$Create_CMS_Classes:
$!------------------
$library_loop:
$ library = f$search("''root':[*]cms.dir",1)
$ if library .eqs. "" then GOTO Check_Stream_facility
$ fac = f$parse(library,,,"DIRECTORY") - "[" - "]" - "<" - ">"
$!
$! skip archived & nopropagate facilities
$!
$ if f$search("''root':[''fac']VMSCMS$ARCHIVED_FACILITY.FLAG") .nes. ""
$ then
$   write sys$output -
   "%FAC-I-SKPARCH, Library ''root':[''fac'.cms] marked ARCHIVED, skipping it"
$   GOTO library_loop
$ endif
$!
$ if f$search("''root':[''fac']VMSCMS$NO_PROPAGATE.FLAG") .nes. ""
$ then
$   if fac .nes. source
$   then
$	write sys$output -
	"%FAC-I-SKPNOPROP, Library ''root':[''fac'.cms] marked NOPROPAGATE, skipping it"
$	GOTO library_loop
$   endif
$ endif
$!
$ define/nolog cms$lib 'root':['fac'.cms]
$!
$! check if the parent class is present
$! if no parent skip this facility
$!
$ define/user/nolog sys$output _nla0:
$ define/user/nolog sys$error _nla0:
$ CMS Show class 'source'
$ if .not. $status
$ then
$   write sys$output -
    "%CRESTRM-I-NOPARENT, class ''source' not found in ''root':[''fac'.cms], skipping it"
$   GOTO Library_Loop
$ endif
$!
$ CMS Create class 'target' "''remark' [*DEL_HIST*]"
$!
$ goto library_loop
$!
$Check_Stream_facility:
$!
$ RETURN
$!
$!+----------------------------------------------------------------------------
$!  Create VDE Stream, perform the command to create an empty(no generations)
$! build stream in VDE set parent stream and remark
$!+----------------------------------------------------------------------------
$Create_VDE_Stream:
$!-----------------
$ write sys$output "Creating empty VDE stream for ''target'"
$!
$ if stream_fetch_method .NES. "VDE"
$ then
$   VDE Set priv CRESTRM; -
     Create Stream/CMS_CLASS/NOINIT_CMS_CLASS/FROZEN/NOGENERATION/DEFER -
     /OWNER='user' -
     /PARENT='source' -
     /REMARK="''remark'" -
     /'variant' -
     'target'
$ else
$    VDE Set priv CRESTRM; -
     Create Stream/NOCMS_CLASS/NOINIT_CMS_CLASS/FROZEN/NOGENERATION/DEFER -
     /OWNER='user' -
     /PARENT='source' -
     /REMARK="''remark'" -
     /'variant' -
     'target'
$ endif
$!
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$! execute the populate stream job to populate the newly created build stream
$! from it's parent development stream.  POPSTRM submits CMS populate jobs to
$! 'batch_queue'.  Clobber anything -- NOMERGE -- already in the target stream.
$!+-----------------------------------------------------------------------------
$Populate_Stream:
$!---------------
$!
$ @VDE$SYSTEM:PLMENU$$POPSTRM.COM 'mnemonic' 'target' 'source' 'batch_queue' 'work_dir' NOMERGE
$ if .not. $status then GOTO Error_Exit
$!
$ RETURN
$!
$!++
$! PLMENU$$CRESTRM.COM - Create a VMS build stream.
$!
$!  This is the main command procedure used to create a VDE
$! build stream.  The command procedure will create the stream
$! in VDE and in CMS if necessary.  The build stream is created
$! with no generations.
$!
$!  The VDE$SYSTEM:PLMENU$$POPSTRM.COM procedure is used to populate
$!  the newly created build stream.
$!
$! Inputs:    Description               (local symbol name)
$!            ------------------------- -------------------                                   
$!  P1	      product			(root)
$!  P2        parent stream		(source)
$!  P3        stream to create		(target)
$!  P4        remark for stream         (remark)
$!  P5	      queue for CMS jobs	(batch_queue)
$!  P6        work directory for logs   (work_dir)
$!  P7        long or short generations (variant)
$!
$!  VDE$FETCH_METHOD
$!	      a logical used to define	(stream_fetch_method)
$!	      the scope of the create
$!	      VDE or VDE and CMS
$!
$! FILES:
$!	All files are located in the directory passed in P5.
$!
$!	    CMS POPULATE STATUS FILE
$!	    format: 'target_stream'_POP.DAT
$!
$!	    CMS populate logfiles
$!          format: 'facility'$POPSTRM.LOG
$!
$!-
