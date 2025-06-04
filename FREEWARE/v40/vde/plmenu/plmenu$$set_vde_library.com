$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ Target = f$edit(p1,"collapse,upcase")
$ Define/NoLog PLMENU$MNEMONIC "???"
$ Define/NoLog PLMENU$LABEL    "???"
$ Define/NoLog PLMENU$RDBROOT  "???"
$ Define/NoLog PLMENU$CMSROOT  "???"
$ loopindex = F$TrnLnm("VDE$LIBRARY_DEFAULTS_MNEMONICS",,,,,"MAX_INDEX")
$ Loop:
$ mnemonic = F$TrnLnm("VDE$LIBRARY_DEFAULTS_MNEMONICS",,loopindex)
$ if (mnemonic .eqs. "" ) then Goto InvalidVdeLnm
$ if f$length(target) .eq. 0 then target = mnemonic
$ if target .eqs. mnemonic
$ then
$   Define/NoLog PLMENU$MNEMONIC "''mnemonic'"
$   vdelabel   = "''F$TrnLnm(""VDE$LIBRARY_DEFAULTS_LABELS"",,loopindex)'"
$   vdelabel   = f$extract(0,40,vdelabel)
$   vderdbroot = F$TrnLnm("VDE$LIBRARY_DEFAULTS",,loopindex)
$   vdecmsroot = F$TrnLnm("VDE$LIBRARY_DEFAULTS_CMS",,loopindex) - ":"
$   if (vdecmsroot .eqs. "" .or. vderdbroot .eqs. "" .or. vdelabel .eqs. "" ) then Goto InvalidVdeLnm
$   Define/NoLog PLMENU$LABEL   "''vdelabel'"
$   Define/NoLog PLMENU$RDBROOT "''vderdbroot'"
$   Define/NoLog PLMENU$CMSROOT	"''vdecmsroot'"
$   Define/Nolog VDE$LIBRARY	"''vderdbroot'"
$ Else
$   loopindex = loopindex - 1
$   if loopindex .ge. 0 then goto Loop
$   Goto InvalidRoot
$ endif
$Done:
$
$!
$!
$ If f$mode() .nes. "INTERACTIVE"
$ then
$   scsnode = f$getsyi("SCSNODE")
$   timenow = f$time()
$   write sys$output "%PLMENU-I-VDEMNE, selecting ''mnemonic'"
$   write sys$output "%PLMENU-I-VDELIB, library ''vdelabel'"
$   write sys$output "-PLMENU-I-WHERE, executing on ''scsnode'"
$   write sys$output "-PLMENU-I-WHEN, at ''timenow'"
$ EndIf
$
$! Success!
$!
$ EXIT 1+0*f$verify(sav_verify)
$  
$InvalidRoot:
$!------------
$ if f$trnlnm("VDE$LIBRARY","LNM$PROCESS") .nes. "" then deassign/process VDE$LIBRARY
$ if f$trnlnm("VDE$LIBRARY","LNM$JOB") .nes. "" then deassign/job VDE$LIBRARY
$ write sys$output "%PLMENU-E-IVPROD, ''p1' is not a valid product" 
$
$! Failure!
$!
$ EXIT  0+0*f$verify(sav_verify)  ! %VDE-E-LIBNOTSET
$
$InvalidVdeLnm:
$
$ write sys$output "%PLMENU-E-VDELNMERR, translation error on VDE logical name(s)"
$
$! Failure!
$!
$ EXIT  0+0*f$verify(sav_verify)  ! %VDE-E-LIBNOTSET
