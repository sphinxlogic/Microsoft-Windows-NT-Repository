$ set def [kronos.source]
$ !
$ ! This file assumes that MERLIB is assigned to LNK$LIBRARY, otherwise it
$ !  must be explicitely linked in.
$ !
$ fpp/def=(interval=15)/def=(queue=sys$batch)/def=hi_priority kronos.for kronos.fortran
$ fpp/def=(interval=15)/def=(queue=sys$batch) kron.for kron.fortran
$ fpp/def=(interval=15) kparse.for kparse.fortran
$ fpp/def=(interval=15) do_jobs.for do_jobs.fortran
$ goto nodeb
$ fortran/debug/noopt           kronos.fortran
$ fortran/debug/noopt           kron.fortran
$ fortran/debug/noopt           kparse.fortran
$ fortran/debug/noopt           do_jobs.fortran
$ fortran/debug/noopt/contin=50 functions
$ fortran/debug/noopt           string
$ link/debug kronos,kparse,do_jobs,functions,string
$ link/debug kron,kparse,string,do_jobs,functions
$ exit
$nodeb:
$ fortran/debug=notrace                 kronos.fortran
$ fortran/debug=notrace                 kron.fortran
$ fortran/debug=notrace                 kparse.fortran
$ fortran/debug=notrace                 do_jobs.fortran
$ fortran/debug=notrace/contin=50       functions
$ fortran/debug=notrace                 string
$ link/notrace kronos,kparse,do_jobs,functions,string
$ link/notrace kron,kparse,string,do_jobs,functions
