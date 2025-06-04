$ copy monde_paris.world sys$common:[syslib]FLT$paris.WORLD /lo/prot=(w:re) 
$! Optionnal : install new FLT$KEYMAP.DAT file
$! copy flt$keymap.dat sys$common:[syslib]* /lo/prot=(w:re) 
