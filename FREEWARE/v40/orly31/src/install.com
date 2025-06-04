$ copy monde_orly3.world sys$common:[syslib]FLT$orly31.WORLD /lo/prot=(w:re) 
$! Optionnal : install new FLT$KEYMAP.DAT file
$! copy flt$keymap.dat sys$common:[syslib]* /lo/prot=(w:re) 
