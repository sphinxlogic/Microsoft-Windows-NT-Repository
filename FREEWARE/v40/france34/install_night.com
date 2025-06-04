$ copy monde_france_night.world sys$common:[syslib]FLT$france_night.WORLD /lo/prot=(w:re) 
$! Optionnal : install new FLT$KEYMAP.DAT file
$! copy flt$keymap.dat sys$common:[syslib]* /lo/prot=(w:re) 
