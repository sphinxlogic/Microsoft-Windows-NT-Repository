$ set noon
$ filename = "TCPWATCH"
$ copy nl: 'filename'.RNT
$! copy nl: 'filename'.RNX
$ runoff/intermediate/nooutput 'filename'
$ runoff/contents 'filename'
$! runoff/index 'filename'
$ runoff/form_size=66/right=5/down=2 'filename'
$ delete 'filename'.BRN;*
$ delete 'filename'.RNT;*
$! delete 'filename'.RNX;*
$ purge/nolog
$ exitt 1
