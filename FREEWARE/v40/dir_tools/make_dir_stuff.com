$   zip "-9V" dir_stuff *.com
$   zip "-9V" dir_stuff *.txt
$   vms_share dir_stuff.zip directory_stuff
