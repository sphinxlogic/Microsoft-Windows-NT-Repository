$ If F$Mode () .eqs. "INTERACTIVE"
$   Then
$       VERIFY = F$Verify (0)
$   Else
$       VERIFY = F$Verify (1)
$ EndIf
$!========================================================================
$!
$!  Name      : MAKE_PCAL.COM
$!
$!  Purpose   : Compile and Link pcal under VMS
$!
$!  Arguments : P1/P2 = DATE_DEBUG: compile with DEBUG info
$!              P1/P2 = LINK : link only
$!
$!  Created: 13-SEP-1990 Rick Dyson   dyson@iowasp.physics.uiowa.edu
$!  Updated: 20-SEP-1990 pcal v2.3
$!  Updated:  2-OCT-1990 pcal v2.4
$!  Updated:  8-OCT-1990 pcal v2.5
$!  Updated:  2-JAN-1991 pcal v3.0
$!  Updated: 15-FEB-1991 pcal v3.1
$!  Updated:  1-MAR-1991 pcal v4.0
$!  Updated: 15-AUG-1991 pcal v4.1
$!  Updated:  4-OCT-1991 pcal v4.2
$!
$!========================================================================
$   THIS_PATH = F$Element (0, "]", F$Environment ("PROCEDURE")) + "]"
$   Set Default 'THIS_PATH'
$   On Error     Then GoTo FINISH
$   On Control_Y Then GoTo FINISH
$   If P1 .eqs. "DATE_DEBUG" .or. P2 .eqs. "DATE_DEBUG"
$       Then
$           CDEFS = "/Define = (""VMS"", ""DATE_DEBUG"")"
$       Else
$           CDEFS = "/Define = (""VMS"")"
$   EndIf
$   If P1 .eqs. "LINK" .or. P2 .eqs. "LINK" Then GoTo LINK
$   Write Sys$Output "Making pcalinit..."
$   Cc /NoList 'CDEFS' PCALINIT
$   Link /NoMap PCALINIT,VAXCRTL.OPT /Option
$   PCALINIT := $ 'THIS_PATH'PCALINIT.EXE
$   Write Sys$Output "Executing pcalinit..."
$   PCALINIT PCALINIT.PS PCALINIT.H
$   Write Sys$Output "Compiling pcal programs..."
$   Cc /NoList 'CDEFS' PCAL.C
$   Cc /NoList 'CDEFS' EXPRPARS.C
$   Cc /NoList 'CDEFS' MOONPHAS.C
$   Cc /NoList 'CDEFS' PCALUTIL.C
$   Cc /NoList 'CDEFS' READFILE.C
$   Cc /NoList 'CDEFS' WRITEFIL.C
$LINK:
$   Write Sys$Output "Linking pcal..."
$   Link /NoMap PCAL,EXPRPARS,MOONPHAS,PCALUTIL,READFILE,WRITEFIL,VAXCRTL/Option
$EXIT:
$   If .not. $Status Then GoTo FINISH
$   Set NoOn
$   If F$Search ("*.EXE;-1") Then Set Protection = Owner:RWED *.OBJ;*,*.EXE;-1
$   Set Protection = Owner:RWED *.OBJ;*
$   Purge /NoConfirm *.OBJ,*.EXE
$   If F$Search ("PCAL.HLB") .eqs. ""
$       Then
$           Write Sys$Output "Creating a help library for Pcal..."
$           Library /Help /Create PCAL PCAL
$       Else
$           Write Sys$Output "Updating help library for Pcal..."
$           Library /Help PCAL PCAL
$   EndIf
$   If F$Mode () .nes. "INTERACTIVE" Then GoTo FINISH
$   Set File /Truncate PCAL.HLB
$   LIB = "Hlp$Library"
$   X = F$Trnlnm (LIB, "Lnm$Process")
$   If "''X'" .eqs. "" Then GoTo INSERT
$   If "''X'" .eqs. "''THIS_PATH'PCAL.HLB" Then GoTo FINISH
$   BASE = LIB + "_"
$   N = 1
$NEXTLIB:
$   LIB := 'BASE''N'
$   X = F$Trnlnm (LIB, "Lnm$Process")
$   If "''X'" .eqs. "" Then GoTo INSERT
$   If "''X'" .eqs. "''THIS_PATH'PCAL.HLB" Then GoTo FINISH
$   N = N + 1
$   GoTo NEXTLIB
$INSERT:
$   Define 'LIB' 'THIS_PATH'PCAL.HLB
$FINISH:
$   Set Protection = (System:RWE, Owner:RWED, Group:RE, World:RE) *.*;*
$   Purge /NoConfirm *.*
$   Set Protection = Owner:RWE *.*
$   Set Protection = Owner:RWED *.OBJ
$   VERIFY = F$Verify (VERIFY)
$   Exit
