$ if f$getsyi ("HW_MODEL").gt.1024	! alpha
$ then mc sysman io connect kill0/driver=killdriver/noadapter/log
$ else mc sysgen connect kill0/driver=killdriver/noadapter
$ endif
