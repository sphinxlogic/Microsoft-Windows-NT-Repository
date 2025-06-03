$ If F$Mode () .eqs. "INTERACTIVE"
$   Then
$     if F$Type (make_tiff_verify) .EQS. "" then make_tiff_verify = 0
$     VERIFY = F$Verify ('make_tiff_verify')
$   Else
$       VERIFY = F$Verify (1)
$ EndIf
$ On Control_Y Then GoTo EXIT
$ On Error     Then GoTo EXIT
$!========================================================================
$!
$!  Name      : MAKE_TIFF.COM
$!
$!  Purpose   : Compile and build TIFF library under VMS
$!  Suggested usage: called by @ [-]MAKE_XV.COM
$!
$!  Created:  9-JAN-1992   by David Jones (jonesd@kcgl1.eng.ohio-state.edu)
$!  Updated: 19-JAN-1992   by Rick Dyson  (dyson@iowasp.physics.uiowa.edu)
$!            9-MAR-1992   by Rick Dyson  for xv v2.11
$!           16-APR-1992   by Rick Dyson  for xv v2.20a and TIFF v3.0
$!            6-MAY-1992   by Rick Dyson  for xv v2.21
$!            9-FEB-1993   by Rick Dyson  for xv v2.21b and ALPHA Support
$!            2-MAR-1993   by Rick Dyson  for xv v3.00
$!           23-AUG-1994   by Rick Dyson  for xv v3.10
$!            6-SEP-1995   by Brian J. McCarthy 
$!                            (mccarthy@hndymn.enet.dec.com)
$!                            Update for use with future DECC releases
$!                            Move use of lnk$library to VAXC area only
$!
$!========================================================================
$ THIS_PATH = F$Element (0, "]", F$Environment ("PROCEDURE")) + "]"
$ If F$Trnlnm ("SYS") .eqs. "" Then Define Sys Sys$Share
$ If F$Trnlnm ("X11") .eqs. "" Then Define X11 DECW$Include
$!
$! Make assumption that version is in form Vn.n (ignoring any -n)
$! and then make an integer out of the n.n value.
$!
$ ver = F$GetSyi ("VERSION")
$ system_ver = f$extract (1,1,ver) + F$EXTRACT (3,1,ver)
$!
$! Test for ALPHA or VAX
$ use_decc = 1
$ If F$GetSyi ("HW_MODEL") .gt. 1023
$ Then    ! it's an ALPHA
$    Define /NoLog Sys DECC$Library_Include
$    QUALIFIERS = " /Prefix=ALL_ENTRIES /Warnings = NoInformationals /NoDebug /Optimize /NoList"
$ Else        ! its a VAX
$    ! check for DEC C  
$    If F$Search ("SYS$SYSTEM:DECC$COMPILER.EXE") .nes. ""
$    Then
$      QUALIFIERS = "/Prefix=All_ENTRIES /NoDebug /Optimize /NoList"
$    Else
$      use_decc = 0
$      QUALIFIERS = "/NoDebug /Optimize /NoList"
$      Define Lnk$Library Sys$Library:VAXCRTL.OLB
$    Endif
$    Define /NoLog Sys Sys$Share
$ EndIf
$!
$! Special case define of BSDTYPES for 6.2 and beyond systems.
$! (It should really be based of the compiler version)
$!
$ DEFS = "/Define = (""BSDTYPES=1"", ""USE_VARARGS=1"", ""USE_PROTOTYPES=0"", ""USE_CONST"", __STDC__)"
$!
$ IF system_ver .ge. 62 
$ THEN
$   ! On 6.2 systems and beyond the CRTL has many new routines that 
$   ! preclude the use of the BSDTYPES define qualifier.
$   DEFS = "/Define = (""USE_VARARGS=1"", ""USE_PROTOTYPES=0"", ""USE_CONST"")"
$ ENDIF
$!
$ CC := CC 'QUALIFIERS' 'DEFS' 
$!
$ sources = "tif_fax3,tif_fax4,tif_aux,tif_ccittrle,tif_close,tif_compress," + -
            "tif_dir,tif_dirinfo,tif_dirread,tif_dirwrite,tif_dumpmode,"     + -
            "tif_error,tif_getimage,tif_jpeg,tif_flush,tif_lzw," + -
            "tif_next,tif_open,tif_packbits,tif_print,tif_read,tif_swab,"    + -
            "tif_strip,tif_thunder,tif_tile,tif_version,tif_vms,tif_warning,"+ -
            "tif_write"
$!
$ new_objects = ""
$!
$!  search for missing object files.
$!
$ If "''F$Search ("g3states.h")'" .eqs. ""
$   Then
$       CC mkg3states.c
$       Link /Trace /NoMap mkg3states
$       Define /User_Mode Sys$Output g3states.h
$       Run Sys$Disk:[]mkg3states.exe
$ EndIf
$!
$ sndx = 0
$NEXT_SOURCE:
$   sfile = F$Element (sndx, ",", sources)
$   sndx = sndx + 1
$   If sfile .eqs. "," Then GoTo SOURCES_DONE
$   ofile = F$Parse (".OBJ", sfile)
$   If F$Search (ofile) .nes. "" Then GoTo NEXT_SOURCE
$   Write Sys$Output "Compiling ", sfile, ".c ..."
$   CC 'sfile'.c
$   If F$Search (ofile) .nes. "" Then new_objects = new_objects + "," + sfile
$   GoTo NEXT_SOURCE
$!
$SOURCES_DONE:
$   If new_objects .eqs. "" .and. p1 .eqs. "" Then GoTo FINISHED
$   new_objects = new_objects - ","
$!
$   Write Sys$Output "Building LIBTIFF.OLB..."
$   If "''F$Search ("LIBTIFF.OLB")'" .eqs. "" Then Library /Create LIBTIFF.OLB
$   Library /Replace LIBTIFF.OLB tif_fax3.obj,tif_fax4.obj,tif_aux.obj,    -
             tif_ccittrle.obj,tif_close.obj,tif_compress.obj,tif_dir.obj,  -
             tif_dirinfo.obj,tif_dirread.obj,tif_dirwrite.obj,             -
             tif_dumpmode.obj,tif_error.obj,tif_getimage.obj,tif_jpeg.obj, -
             tif_flush.obj,tif_lzw.obj,tif_next.obj,       -
             tif_open.obj,tif_packbits.obj,tif_print.obj,tif_read.obj,     -
             tif_swab.obj,tif_strip.obj,tif_thunder.obj,tif_tile.obj,      -
             tif_version.obj,tif_vms.obj,tif_warning.obj,tif_write.obj 
$FINISHED:
$   Write Sys$Output "Finished building LibTIFF"
$EXIT:
$   VERIFY = F$Verify (VERIFY)
$   Exit
