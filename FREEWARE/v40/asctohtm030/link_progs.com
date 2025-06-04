$!
$! Command file to link AscToHTM and AscToTab.  Detects Vax/Alpha and names
$! resulting .exe accordingly
$!
$! P1 - NOSHARE 	Produce an .exe with all shareable references resolved
$!			to make an .exe runnable on machines withou the RTL's
$!
$ if p1.eqs."NOSHARE"
$ then
$    noshare   = "_NOSHARE"
$    sysshropt = "/NOSYSSHR"
$    nstext    = "(NOSHARE)"
$ else
$    noshare   = ""
$    sysshropt = ""	! /SYSSHR is actually the default
$    nstext    = "(against RTLs)"
$ endif
$!
$ if F$GETSYI("NODE_HWTYPE").EQS."ALPH"
$ then
$    platform = "ALPHA"
$    olbext   = ".OLB_ALPHA"
$ else
$    platform = "VAX"
$    olbext   = ".OLB"
$ endif
$!
$!-- so just do it, okay!
$!
$ Write sys$output "Linking AscToHTM ''nstext' for ''platform'.  File will be ASCTOHTM.EXE_''platform'''noshare'"
$ LINK/EXE=ASCTOHTM.EXE_'platform''noshare' 'sysshropt'/NOMAP -
      HTML_TOOLS'olbext'/LIB/INC=(ASCTOHTM)
$!
$ Write sys$output "Linking AscToTab ''nstext' for ''platform'.  File will be ASCTOTAB.EXE_''platform'''noshare'"
$ LINK/EXE=ASCTOTAB.EXE_'platform''noshare' 'syschropt'/NOMAP -
      HTML_TOOLS'olbext'/LIB/INC=(ASCTOTAB)
