!
!  LSTMAIL.MMS - LSTMAIL utility build procedure
!
.IFDEF debug
basflags = $(basflags)/DEBUG
linkflags = $(linkflags)/DEBUG
.ELSE
linkflags = $(linkflags)/NOTRACEBACK
.ENDIF

lstmail.exe :  lstmail.obj
	$(link) $(linkflags) lstmail/options

lstmail.obj : lstmail.bas, lstcommon.bas, $mailmsgdef.bas, mail$routines.bas

.LAST
	@- purge lstmail.exe, lstmail.obj, lstmail.bas, lstcommon.bas
	@- if f$search ("lstmail.dia").NES."" then delete lstmail.dia;*
	@- if f$search ("lstmail.lis").NES."" then delete lstmail.lis;*
