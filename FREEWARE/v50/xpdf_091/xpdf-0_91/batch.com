$!  Batch.com
$
$ Path = F$Element (0, "]", F$Environment ("PROCEDURE"))
$ Set Default 'Path']
$
$ Path = F$Element (0, "]", F$Environment ("PROCEDURE")) - ".XPDF-0_91"
$
$ Define LibT1  'Path'.T1LIB]
$ Define T1Inc  'Path'.T1LIB.Lib.T1Lib]
$ Define LibTTF 'Path'.FreeType-1_3_1]
$ Define TTFInc 'Path'.FreeType-1_3_1.Lib],'Path'.FreeType-1_3_1.Lib.Extend]
$ Define LibXPM 'Path'.XPM]
$
$ @ VMS_Make "xpm" "" (inc=T1Inc:,olb=LibT1:T1.olb) LibTTF: TTFInc: LibTTF:LibTTF.olb
$
$ Exit
