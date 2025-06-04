$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$ on error then GOTO Error_Exit
$ on warning then continue
$!
$!
$ if p1 .eqs. ""
$ then
$   write sys$output "%RENSTRM-E-Missing, No stream name to rename specified in P1!"
$   goto Error_exit
$ endif
$!
$ if p2 .eqs. ""
$ then
$   write sys$output "%RENSTRM-E-Missing, No new stream name specified in P2!"
$   goto Error_exit
$ endif
$!
$ if p3 .eqs. ""
$ then
$   remark_qual = ""
$ else
$   remark_qual = "/REMARK=" + """" + "''p3'" + """"
$ endif
$!
$ mnemonic = f$edit(p4,"collapse,upcase")
$ if mnemonic .eqs. ""
$ then 
$   mnemonic = "''f$trnlnm(""VDE$LIBRARY_DEFAULTS_MNEMONICS"",,0)'"
$   mnemonic = f$edit(mnemonic,"collapse,upcase")
$ endif
$!
$ @vde$system:plmenu$$set_vde_library 'mnemonic'
$ root = f$trnlnm("PLMENU$CMSROOT")
$ mnemonic = f$trnlnm("PLMENU$MNEMONIC")
$!
$! See if the vde$fetch_method logical is defined to VDE.
$! If the logical is defined to VDE, rename the
$! stream in the VDE database only.  If the logical is undefined,
$! the stream will be renamed in both VDE and CMS.
$!
$ stream_fetch_method = f$trnlnm("vde$fetch_method")
$ stream_fetch_method = F$edit(stream_fetch_method,"Trim,UPCASE")
$!
$! rename stream in SYS CMS facility if a VDE only stream
$! required for BASE_LEVEL.MAR updates
$!  
$ if stream_fetch_method .EQS. "VDE"
$ then
$   if mnemonic .eqs. "VMS" .or. mnemonic .eqs. EVMS
$   then
$	set command vde$system:plmenu$$facrename
$	facrename/log=library'remark_qual' 'root':[SYS.cms]  'p1'  'p2'
$   endif       
$   GOTO DO_VDE_Rename
$ endif
$!
$! the loop through the letters of the alphabet is done to avoid virtual memory
$! problems encountered with CMS when processing all libraries in one FACRENAME
$! command invocation.
$!
$ set command vde$system:plmenu$$facrename
$ letters = "A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z"
$ index = 0
$letter_loop:
$ letter = f$element(index,",",letters)
$ if letter .eqs. "," then GOTO DO_VDE_Rename
$ fac = letter + "*"
$ if f$search("''root':[000000]''fac'.dir") .nes. "" then -
  facrename/log=library'remark_qual' -
  'root':['fac'.cms]  'p1'        'p2'
$!		'old_name'  'new_name'
$ index = index + 1
$ goto letter_loop
$!
$DO_VDE_Rename:
$! perform the VDE modify stream/name='new_name' 'old_name' 
$!
$  VDE Set priv MODSTRM; Modify stream'remark_qual'/name='p2'  'p1'
$!
$ GOTO MAIN_EXIT
$!
$ERROR_EXIT:
$ EXIT 0+0*'f$verify(sav_verify)
$!
$MAIN_EXIT:
$ EXIT 1+0*'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$!
$! Performs a rename stream command in the VDE database for the mnemonic
$! specified via p4.  The stream specified in P1 is renamed to the name
$! specified in P2.  This job is submitted by the rename_stream job.
$!
$! INPUTS:
$!	P1 = stream to rename
$!	P2 = new stream name
$!	P3 = new remark
$!	p4 = VDE library mnemonic
$!
$! VDE$FETCH_METHOD logical
$!
$!+-----------------------------------------------------------------------------
