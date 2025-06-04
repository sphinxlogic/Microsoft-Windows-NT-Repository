$ sav_verify = f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ set noon
$!+
$! if a default source stream is not set then check the user name to see if it
$! is(most likely) a build account.  If the username contains "BLD" default
$! the source stream to the string prefixing the "BLD"
$!+
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$ source = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ if source .eqs. ""
$ then
$   if f$locate("BLD",user) .lt. f$length(user)
$   then
$     source = f$extract(0,f$locate("BLD",user),user)
$     if source .nes. "" then define/process PLMENU$DEFAULT_SOURCE_STREAM 'source'
$   endif
$ endif
$!
$ if f$trnlnm("PLMENU$DEFAULT_PRODUCT") .EQS. ""
$ then
$   define/process/nolog PLMENU$DEFAULT_PRODUCT "''F$TrnLnm(""VDE$LIBRARY_DEFAULTS_MNEMONICS"",,0)'"
$   define/process/nolog PLMENU$LABEL           "''F$TrnLnm(""VDE$LIBRARY_DEFAULTS_LABELS"",,0)'"
$ endif
$!
$! get the current value of the VDE$LIBRARY logical
$!
$ old_vdelib = f$trnlnm("VDE$LIBRARY")
$
$ action = ""
$!
$ type sys$input

OpenVMS Project Leader Menu (PLMENU) Version V1.3

Enter ? or "HELP" at any prompt to obtain help.
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately

$!
$Display_Menu:
$!------------
$!
$ type sys$input

     Choose an action from the list below:

          (1) Create a new Stream
          (2) Populate a Stream/Class
          (3) Rename a Stream
          (4) Set/Show PLMENU logicals
          (5) Create a baselevel stream from a build stream
          (6) Compare VDE Stream to CMS Class
          (7) Create a New Facility
          (8) Populate a Stream/Class in Selected Facilities
          (9) Compare Two CMS Classes
         (10) Archive/Obsolete a Facility

$!
$ read/End_of_File=Main_Exit/Error=time_expired/timeout=255 -
  /prompt="What action do you want to perform? (1-10,EXIT,HELP): " -
  SYS$COMMAND action
$!
$ action = f$edit(action,"COMPRESS,UPCASE")
$ if action .eqs. "" then GOTO Display_Menu
$
$ if F$locate(action,"EXIT") .eq. 0 then GOTO Main_Exit
$ if F$locate(action,"QUIT") .eq. 0 then GOTO Main_Exit
$
$ if action .eqs. "?" then GOTO Display_Menu
$ if F$locate(action,"HELP") .eq. 0 then GOTO Display_Menu
$!
$ if action .eqs. "1" then GOTO Create_a_Stream
$ if action .eqs. "2" then GOTO Populate_a_Stream
$ if action .eqs. "3" then GOTO Rename_a_Stream
$ if action .eqs. "4" then GOTO Setshow_logicals
$ if action .eqs. "5" then GOTO Create_a_Baselevel
$ if action .eqs. "6" then GOTO Compare_Vde_to_CMS
$ if action .eqs. "7" then GOTO Create_New_Facility
$ if action .eqs. "8" then GOTO Populate_Facilities
$ if action .eqs. "9" then GOTO Compare_CMS_Classes
$ if action .eqs. "10" then GOTO Archive_a_Facility
$ if f$integer(action) .ge. 32767 then $ EXIT
$!
$ GOTO Display_Menu
$!
$Time_expired:
$!------------
$ write sys$output "Timeout on input, exiting..."
$!
$Control_y:
$!---------
$!
$Main_Exit:
$!---------
$
$ on control_y then GOTO Main_Exit
$!
$! restore the old value of VDE$LIBRARY
$!
$ if old_vdelib .eqs. ""
$ then
$   if f$trnlnm("VDE$LIBRARY") .nes. "" then deassign VDE$LIBRARY
$ else
$   if old_vdelib .nes. f$trnlnm("VDE$LIBRARY")
$   then
$	define/nolog VDE$LIBRARY 'old_vdelib'
$   endif
$ endif
$!
$ EXIT 1+0*'f$verify(sav_verify)
$!
$!
$!+-----------------------------------------------------------------------------
$! Archive a Facility
$!+-----------------------------------------------------------------------------
$Archive_a_Facility:
$!------------------
$ @VDE$SYSTEM:PLMENU$$Obsolete_a_Facility
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$! Compare CMS representations of two streams
$!+-----------------------------------------------------------------------------
$Compare_CMS_Classes:
$!-------------------
$ @VDE$SYSTEM:PLMENU$$difference_stream
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$! Compare VDE and CMS representations of a stream
$!+-----------------------------------------------------------------------------
$Compare_Vde_to_CMS:
$!------------------
$ @VDE$SYSTEM:PLMENU$$compare_stream
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$! Create a baselevel Stream from a build stream
$!+-----------------------------------------------------------------------------
$Create_a_Baselevel:
$!-----------------
$ @VDE$SYSTEM:PLMENU$$create_baselevel_stream
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$! Create a new build Stream
$!+-----------------------------------------------------------------------------
$Create_a_Stream:
$!---------------
$ @VDE$SYSTEM:PLMENU$$create_build_stream
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$! Create a new facility
$!+-----------------------------------------------------------------------------
$Create_New_Facility:
$!-------------------
$ @VDE$SYSTEM:PLMENU$$create_facility
$ GOTO Display_Menu
$!
$!+-----------------------------------------------------------------------------
$! Populate a Stream
$!+-----------------------------------------------------------------------------
$Populate_a_Stream:
$!-----------------
$ @VDE$SYSTEM:PLMENU$$populate_stream
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$! Populate a Stream in some facilities
$!+-----------------------------------------------------------------------------
$Populate_Facilities:
$!-------------------
$ @VDE$SYSTEM:PLMENU$$populate_facilities
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$! Rename a Stream
$!+-----------------------------------------------------------------------------
$Rename_a_Stream:
$!---------------
$ @VDE$SYSTEM:PLMENU$$rename_stream
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$! Show or set project leader menu logicals
$!+-----------------------------------------------------------------------------
$Setshow_logicals:
$!----------------
$ @VDE$SYSTEM:PLMENU$$plmenu_logicals
$ GOTO Display_Menu
$!
$!
$!+-----------------------------------------------------------------------------
$!  PLMENU - VAX/VMS Project leader menu
$!
$!	Provides functions commonly used by the VMS release project leaders.
$!
$! Modification History:
$!
$!	DJS	13-JAN-1992	X-1    Original Version 
$!	DJS     24-MAR-1992	X-2    Add create baselevel and compare
$!				       VDE to CMS functions
$!      DJS     15-APR-1992     X-3    Add create new facility option
$!      DJS     23-APR-1992     X-4    Add populate facilities option(8)
$!      DJS     29-APR-1992     X-5    Add compare CMS classes option(9)
$!      DJS     05-MAY-1992     X-6    Add Archive a Facility(10)
$!
$!+-----------------------------------------------------------------------------
