$ __save_verif = 'f$verify(0)
$! COMPARE_DIRS.COM -- Compares directory specifications
$! P1 = first directory
$! P2 = second directory
$! P3 = global variable to put result in
$
$ proc = f$environment("PROCEDURE")
$ proc_dir = f$parse(proc,,,"NODE")+f$parse(proc,,,"DEVICE")+f$parse(proc,,,"DIRECTORY")
$ __debug = f$trnlnm("DEBUG_COMPARE_DIRS")
$ if __debug
$  then
$   write sys$output "%COMPARE_DIRS-I-DEBUG, P1 = """,p1,""""
$   write sys$output "%COMPARE_DIRS-I-DEBUG, P2 = """,p2,""""
$  endif
$ @'proc_dir'decomplnm 'p1' __c_d_dir1 DIRS
$ @'proc_dir'decomplnm 'p2' __c_d_dir2 DIRS
$ if __debug
$  then
$   write sys$output "%COMPARE_DIRS-I-DEBUG, __c_d_dir1 = """,__c_d_dir1,""""
$   write sys$output "%COMPARE_DIRS-I-DEBUG, __c_d_dir2 = """,__c_d_dir2,""""
$  endif
$ 'p3' == f$edit(__c_d_dir1,"UPCASE") .eqs. f$edit(__c_d_dir2,"UPCASE")
$
$exit:
$ a = f$verify(__save_verif)
