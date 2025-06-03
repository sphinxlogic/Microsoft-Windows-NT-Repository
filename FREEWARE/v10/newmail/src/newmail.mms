!
!  newmail.mms - newmail utility build procedure
!
.IFDEF debug
basflags = $(basflags)/DEBUG
linkflags = $(linkflags)/DEBUG
.ELSE
basflags = $(basflags)/NODEBUG
linkflags = $(linkflags)/NOTRACEBACK/NODEBUG
.ENDIF
!
newmail.exe :  newmail.obj
	$(link) $(linkflags) newmail/option

newmail.obj : newmail.bas, $mailmsgdef.bas, mail$routines.bas
	$(basic) $(basflags) newmail

.LAST
	@- purge newmail.exe, newmail.obj, newmail.bas
	@- if f$search ("newmail.dia").NES."" then delete newmail.dia;*
