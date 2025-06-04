$! install persona image with privs 22-MAR-1999 lww
$
$ install add persona /open/head/share -
/priv=(AUDIT,SYSPRV,IMPERSONATE,CMKRNL,BYPASS)
$ exit

