$! A DCL command procedure created by MMS from Makefile.MMS
$! and hand edited for VAX vs. Alpha
$!
$! This assumes the DEC C compiler for a default.
$
$ Options = ""
$ If F$GetSYI("Arch_Name") .nes. "Alpha" Then Options = ",VAXCRTL.OPT /Option"
$
$ CC /Standard = VAXC /Object=PCAL.OBJ     /Define = ("TIMEZONE=""6 [Iowa City]""", EPS, "NOTESFONT=""Helvetica/8""") /NoList /Optimize /NoDebug PCAL.C
$ CC /Standard = VAXC /Object=EXPRPARS.OBJ /Define = ("TIMEZONE=""6 [Iowa City]""", EPS, "NOTESFONT=""Helvetica/8""") /NoList /Optimize /NoDebug EXPRPARS.C
$ CC /Standard = VAXC /Object=MOONPHAS.OBJ /Define = ("TIMEZONE=""6 [Iowa City]""", EPS, "NOTESFONT=""Helvetica/8""") /NoList /Optimize /NoDebug MOONPHAS.C
$ CC /Standard = VAXC /Object=PCALUTIL.OBJ /Define = ("TIMEZONE=""6 [Iowa City]""", EPS, "NOTESFONT=""Helvetica/8""") /NoList /Optimize /NoDebug PCALUTIL.C
$ CC /Standard = VAXC /Object=READFILE.OBJ /Define = ("TIMEZONE=""6 [Iowa City]""", EPS, "NOTESFONT=""Helvetica/8""") /NoList /Optimize /NoDebug READFILE.C
$ CC /Standard = VAXC /Object=PCALINIT.OBJ /Define = ("TIMEZONE=""6 [Iowa City]""", EPS, "NOTESFONT=""Helvetica/8""") /NoList /Optimize /NoDebug PCALINIT.C
$
$ Link  pcalinit'Options'
$ MCR Sys$Disk:[]pcalinit pcalinit.ps pcalinit.h header
$
$ CC /Standard = VAXC /Object=WRITEFIL.OBJ /Define = ("TIMEZONE=""6 [Iowa City]""", EPS, "NOTESFONT=""Helvetica/8""") /NoList /Optimize /NoDebug WRITEFIL.C
$ Link pcal,exprpars,moonphas,pcalutil,readfile,writefil'Options'
$
$ If "''F$Search("PCAL.HLB")'" .EQS. "" Then Library /Create /Help PCAL.HLB
$ Library /Replace PCAL.HLB PCAL.HLP
$
$ Exit
