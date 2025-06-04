$ set noon
$ filename = "AUTO_TUNE"
$ copy nl: 'filename'.RNT
$! copy nl: 'filename'.RNX
$ runoff/intermediate/nooutput 'filename'
$ runoff/contents 'filename'
$! runoff/index 'filename'
$ runoff/form_size=69/right=5/down=5 'filename'
$ delete 'filename'.BRN;*
$ delete 'filename'.RNT;*
$! delete 'filename'.RNX;*
$ purge/nolog
$ exitt 1
