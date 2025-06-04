# Wakeup interval.  Defaults to 1 hour.
# INT = 
INT = /DEFINE=(INTERVAL=15)
# Submit jobs or test only?
TEST = 
#TEST = /DEFINE=TEST_ONLY

kronos.exe: kronos.obj kparse.obj do_jobs.obj functions.obj string.obj
    link/nomap/notrace kronos,kparse,do_jobs,functions,string
#
# Note: the default build assumes MERLIB is assigned to LNK$LIBRARY
#    link/nomap/notrace kronos,kparse,do_jobs,functions,string,merlib/lib

kron: kron.exe

kron.exe: kron.obj kparse.obj string.obj do_jobs.obj functions.obj
    link/nomap/notrace kron,kparse,string,do_jobs,functions

kron.obj: kron.fortran strings.cmn
    fortran/nolist/debug=notrace kron.fortran

kronos.obj:  kronos.fortran kronos.cmn
    fortran/nolist/debug=notrace kronos.fortran

kparse.obj:  kparse.fortran kparse.cmn strings.cmn
    fortran/nolist/debug=notrace kparse.fortran

do_jobs.obj:  do_jobs.fortran kronos.cmn strings.cmn
    fortran/nolist/debug=notrace do_jobs.fortran

functions.obj:  functions.for kronos.cmn strings.cmn
    fortran/nolist/debug=notrace/contin=50 functions

string.obj:  string.for strings.cmn
    fortran/nolist/debug=notrace string

kron.fortran: kron.for
    fpp $(INT) /def=(QUEUE=SYS$BATCH) kron.for kron.fortran

kronos.fortran: kronos.for
    fpp $(INT) $(TEST) /def=(QUEUE=SYS$BATCH)/def=hi_priority kronos.for kronos.fortran

kparse.fortran: kparse.for
    fpp $(INT) kparse.for kparse.fortran

do_jobs.fortran: do_jobs.for
    fpp $(INT) do_jobs.for do_jobs.fortran
