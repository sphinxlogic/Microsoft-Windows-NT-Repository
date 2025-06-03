$! expected:
$! p1 = output filename to create
$! P2 = definition file to copy
$! p3 = record size
$intouch/noframe tti_run:create_dbase3.int/source 
''p2'
''p3'
''p1'
