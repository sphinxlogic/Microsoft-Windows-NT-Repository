!
!  Vfymail.mms - VFYMAIL utility build procedure
!
.IFDEF debug
debugflags = /DEBUG
.ELSE
debugflags = /NODEBUG
.ENDIF
basflags = $(basflags) $(debugflags)
linkflags = $(linkflags) $(debugflags)

vfymail.exe depends_on  vfymail_main.obj, -
			vfymail_tree.obj, -
			vfymail_subs.obj, -
			vfymail_misc.obj, -
			vfymail.opt
	$(link) $(linkflags) vfymail/option

vfymail_main.obj depends_on vfymail_main.bas, vfycommon.bas
	$(basic) $(basflags) vfymail_main

vfymail_tree.obj depends_on vfymail_tree.bas, vfycommon.bas
	$(basic) $(basflags) vfymail_tree

vfymail_subs.obj depends_on vfymail_subs.bas, vfycommon.bas
	$(basic) $(basflags) vfymail_subs

vfymail_misc.obj depends_on vfymail_misc.bas, vfycommon.bas
	$(basic) $(basflags) vfymail_misc

.LAST
	@- purge vfy*.*
	@- if f$search ("*.dia").NES."" then delete *.dia;*
	@- if f$search ("vfycommon.obj").NES."" then delete vfycommon.obj;*
