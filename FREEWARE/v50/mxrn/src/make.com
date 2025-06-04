$ SAVE_VERIFY=F$VERIFY(f$trnlnm("MXRN_DEBUG"))
$!
$! MAKE.COM for MXRN/DXRN
$! Contributed by Patrick L. Mahan, mahan@tgv.com
$! Various updates for Compaq C, TCP/IP Services, etc, by Stephen Hoffman
$!
$!
$! This DCL command procedure consolidates both make-dxrn.com and 
$! make-mxrn.com together into one package.  This make.com supports
$! the following options:
$!
$!	$ @MAKE	DXRN	! will (try to) build DXRN
$! 	$ @MAKE MOTIF	! will build MXRN
$!	$ @MAKE CLEAN	! cleans out *.LIS, *.OBJ files
$!	$ @MAKE CLOBBER	! cleans out *.exe then does a clean
$!
$!
$!
$! Process the command options...
$!
$ popt = f$edit(p1,"COLLAPSE,UPCASE")
$!
$ if f$length(popt) .eq. 0 then popt = "MXRN"
$!
$ if popt .Eqs. "CLEAN" then goto clean
$!
$ if popt .Eqs. "CLOBBER" then goto clobber
$!
$ if popt .Eqs. "DXRN" 
$ Then
$   !
$   !  Check to see if we are running above OpenVMS v6.0, or if we
$   !  are on an OpenVMS Alpha system.  If so, always build MXRN.
$   !
$   vers = F$Extract(1,999,F$GetSYI("VERSION"))
$   majvers = F$Element(0,".",vers)
$   If f$integer(majvers) .ge. 6 
$   Then 
$     Write sys$output "Unable to build DXRN (DECwindows XUI) on OpenVMS Alpha"
$     Write sys$output "Will build MXRN (DECwindows Motif) instead."
$     popt = "MXRN"
$   EndIf
$   If F$GetSYI("CPU") .eq. 128
$   Then 
$     Write sys$output "Unable to build DXRN (DECwindows XUI) on OpenVMS Alpha"
$     Write sys$output "Will build MXRN (DECwindows Motif) instead."
$     popt = "MXRN"
$   EndIf
$   program = "DXRN"
$   interface = "DECwindows XUI"
$ Endif
$!
$ if popt .Eqs. "MXRN" 
$ Then
$   program = "MXRN"
$   interface = "DECwindows Motif (or CDE)"
$   define/nolog X11 DECW$INCLUDE
$   define/nolog XM  DECW$INCLUDE
$ Endif
$!
$!
$! Compile the XRN with the user requested Interface
$!
$!  Get the compiler options via the logical name COPTS
$!
$ TCPIP_STACK = "UCX"
$
$ If program .Eqs. "MXRN"
$ Then
$   cc_options = f$trnlnm("COPTS") + "/define=(MOTIF, ''TCPIP_STACK)"
$ Else
$   cc_options = f$trnlnm("COPTS") + "/define=(''TCPIP_STACK')
$ Endif
$!
$!  Get the linker options via the logical name LOPTS
$!
$ link_options = f$trnlnm("LOPTS")
$!
$!
$!  Compile the "C" files
$!
$! procedure target	command 			depends upon
$! CALL MAKE FILE.OBJ	"CC ''cc_options' FILE.C"	FILE.C
$ write sys$output "Making XRN with ''interface' interface"
$ CALL MAKE AVL.OBJ  		"CC ''cc_options' AVL"		AVL.C
$ CALL MAKE BUTTONS.OBJ  	"CC ''cc_options' BUTTONS"	BUTTONS.C
$ CALL MAKE CLIENTLIB.OBJ  	"CC ''cc_options' CLIENTLIB"	CLIENTLIB.C
$ CALL MAKE COMPOSE.OBJ  	"CC ''cc_options' COMPOSE"	COMPOSE.C
$ CALL MAKE CURSOR.OBJ  	"CC ''cc_options' CURSOR"	CURSOR.C
$ CALL MAKE DIALOGS.OBJ  	"CC ''cc_options' DIALOGS"	DIALOGS.C
$ CALL MAKE ERROR_HNDS.OBJ  	"CC ''cc_options' ERROR_HNDS"	ERROR_HNDS.C
$ CALL MAKE INTERNALS.OBJ  	"CC ''cc_options' INTERNALS"	INTERNALS.C
$ CALL MAKE KILLFILE.OBJ  	"CC ''cc_options' KILLFILE"	KILLFILE.C
$ CALL MAKE MENUS.OBJ  		"CC ''cc_options' MENUS"	MENUS.C
$ CALL MAKE MESG.OBJ  		"CC ''cc_options' MESG"		MESG.C
$ CALL MAKE NEWSRCFILE.OBJ  	"CC ''cc_options' NEWSRCFILE"	NEWSRCFILE.C
$ CALL MAKE NEWSRC_PARSE.OBJ	"CC ''cc_options' NEWSRC_PARSE"	NEWSRC_PARSE.C
$ If program .Nes. "MXRN" 
$ then 
$   CALL MAKE PANE.OBJ  	"CC ''cc_options' PANE"		PANE.C
$ endif
$ CALL MAKE RESOURCES.OBJ  	"CC ''cc_options' RESOURCES"	RESOURCES.C
$ CALL MAKE SAVE.OBJ  		"CC ''cc_options' SAVE"		SAVE.C
$ CALL MAKE SERVER.OBJ  	"CC ''cc_options' SERVER"	SERVER.C
$ CALL MAKE SLIST.OBJ  		"CC ''cc_options' SLIST"	SLIST.C
$ CALL MAKE T6.OBJ  		"CC ''cc_options' T6"		T6.C
$ CALL MAKE T7.OBJ  		"CC ''cc_options' T7"		T7.C
$ CALL MAKE UTILS.OBJ  		"CC ''cc_options' UTILS"	UTILS.C
$ CALL MAKE XMISC.OBJ  		"CC ''cc_options' XMISC"	XMISC.C
$ CALL MAKE XRN.OBJ  		"CC ''cc_options' XRN"		XRN.C
$ CALL MAKE XTHELPER.OBJ  	"CC ''cc_options' XTHELPER"	XTHELPER.C
$!
$! If you are building XRN to use with MultiNet on an Alpha then you
$! will need to uncomment the following lines.  This is because we
$! need to tell DEC C to only prefix ANSI routines with DECC$ in order
$! to prevent the common socket routines (socket, accept, connect, etc)
$! from being prefix with DECC$.  This allows us to use the MultiNet
$! socket library instead.  However, a side effect is that certain
$! routines will not get the DECC$ prefix and the linker will complain.
$! To get around this we create Alpha transfer vectors that will allow
$! us to access the proper DEC C routines.
$!
$ If F$GetSYI("CPU") .eq. 128
$ Then
$   if f$search("SYS$SYSROOT:[MULTINET]START_MULTINET.COM") .nes. ""
$   then
$     ! *WARNING, also uncomment the line "!TRANSFER.OBJ" from the
$     ! MXRN.OPT file in order to include this file in the link.
$     CALL MAKE TRANSFER.OBJ	"MACRO/ALPHA/NOLIST TRANSFER.M64" TRANSFER.M64
$   EndIf
$ EndIf
$!
$!
$!
$ write sys$output "Building ''program'"
$ CALL MAKE 'program'.EXE	"LINK ''link_options' ''program'/OPT" *.OBJ
$!
$ exit
$!
$!
$! Delete executables, Purge directory and clean up object files and listings
$!
$Clobber:
$ Delete/noconfirm/log *.exe;*
$!
$!
$! Purge directory, clean up object files and listings
$!
$Clean:
$ Purge
$ Delete/noconfirm/log *.lis;*
$ Delete/noconfirm/log *.obj;*
$ Delete/noconfirm/log *.dia;*
$!
$ exit
$!
$MAKE: SUBROUTINE   !SUBROUTINE TO CHECK DEPENDENCIES
$ V = 'F$Verify(0)
$! P1 = What we are trying to make
$! P2 = Command to make it
$! P3 - P8  What it depends on
$
$ If F$Search(P1) .Eqs. "" Then Goto Makeit
$ Time = F$CvTime(F$File(P1,"RDT"))
$arg=3
$Loop:
$	Argument = P'arg
$	If Argument .Eqs. "" Then Goto Exit
$	El=0
$Loop2:
$	File = F$Element(El," ",Argument)
$	If File .Eqs. " " Then Goto Endl
$	AFile = ""
$Loop3:
$	OFile = AFile
$	AFile = F$Search(File)
$	If AFile .Eqs. "" .Or. AFile .Eqs. OFile Then Goto NextEl
$	If F$CvTime(F$File(AFile,"RDT")) .Ges. Time Then Goto Makeit
$	Goto Loop3
$NextEL:
$	El = El + 1
$	Goto Loop2
$EndL:
$ arg=arg+1
$ If arg .Le. 8 Then Goto Loop
$ Goto Exit
$
$Makeit:
$ Set Verify
$ 'P2
$ VV='F$Verify(0)
$Exit:
$ If V Then Set Verify
$ENDSUBROUTINE
