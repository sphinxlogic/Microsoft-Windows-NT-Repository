
$! install reset_passwd image with privs 11-OCT-2000 lww
$
$ install add reset_passwd /open/head/share /priv=(SYSPRV,BYPASS)
$ exit

