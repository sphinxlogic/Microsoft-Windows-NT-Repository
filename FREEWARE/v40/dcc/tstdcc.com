$ set noon
$ set def [.src]
$ dcc -zcc +zepl dcblk dcdecl dcdir dcexp dcext dcfmt dcinst dcmain dcprag -
dcrec dcrecdir dctxttok
$ set def [-]
