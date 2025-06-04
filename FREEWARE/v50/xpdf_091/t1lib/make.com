$ cflags = "/NoList/prefix=all/float=ieee_float"
$ cflags = cflags + "/define=(T1LIB_IDENT=""""""""""""1.0.1"""""""""""",GLOBAL_CONFIG_DIR=""""""""""""T1_FONT_DIR"""""""""""",T1_AA_TYPE16=""""short"""",T1_AA_TYPE32=""""int"""")"
$ write sys$output "Compiling in [.lib.t1lib]"
$ set default [.lib.t1lib]
$ @make.com
$ write sys$output "Compiling in [.lib.type1]"
$ set default [-.type1]
$ @make.com
$ set default [--]
$ write sys$output "Creating t1.olb"
$ library/create t1.olb [.lib.t1lib]*.obj,[.lib.type1]*.obj
$!
$ Exit
$! these need XAW
$ write sys$output "Creating xglyph.exe"
$ set default [.xglyph]
$ @make.com
$ set default [-]
$ write sys$output "Creating type1afm.exe"
$ set default [.type1afm]
$ @make.com
$ set default [-]
$ write sys$output "Done"
$ exit
