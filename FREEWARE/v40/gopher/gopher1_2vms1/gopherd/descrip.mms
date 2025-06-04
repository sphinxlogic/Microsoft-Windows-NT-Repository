! MMS file for building GOPHERD.EXE
!
!    19940908	FMacrides	macrides@sci.wfeb.edu
!		fixed prefixing for DECC
!    19931221	JLWilkinson	jlw@psulias.psu.edu
!		adapted Foteos Macrides' UCX special handling for DECC
!    19931125	JLWilkinson	jlw@psulias.psu.edu
!		original version

GOPHERD_VERSION = 1.2VMS-1

.IFDEF DEC_C
TCPIP_AGENT = [-.optfiles]$(TCPIP)_DECC.opt
.ELSE
TCPIP_AGENT = [-.optfiles]$(TCPIP)_VAXC.opt
.ENDIF

PRODUCTION = ok
GOPHERD_DEBUG = /nodebug
GOPHERD_EXE  = [-.exe]gopherd.exe

!==============================================================================

! The GopherD object file list
GOPHERD_OBJECT =-
 [-.obj]compatible.obj,-
 [-.obj]daarray.obj,-
 [-.obj]debug.obj,-
 [-.obj]ext.obj,-
 [-.obj]ftp.obj,-
 [-.obj]gdgopherdir.obj,-
 [-.obj]globals.obj,-
 [-.obj]gopherd.obj,-
 [-.obj]gopherdconf.obj,-
 [-.obj]gsgopherobj.obj,-
 [-.obj]index.obj,-
 [-.obj]kernutils.obj,-
 [-.obj]site.obj,-
 [-.obj]sockets.obj,-
 [-.obj]special.obj,-
 [-.obj]strstring.obj,-
 [-.obj]util.obj,-
 [-.obj]waisgopher.obj

! Link the executables
$(GOPHERD_EXE) : $(GOPHERD_OBJECT) [-.optfiles]ident.opt, $(TCPIP_AGENT)
	link $(GOPHERD_DEBUG) /exe=$(GOPHERD_EXE) -
	$(GOPHERD_OBJECT), [-.optfiles]ident/opt, $(TCPIP_AGENT)/opt
.IFDEF PRODUCTION
	@ write sys$output "PRODUCTION Gopher Built!"
.ELSE
	@ write sys$output "DEBUG Gopher Built!"
.ENDIF

!==============================================================================

! Nested Header Files abound in this application
GOPHERD_H =-
 gopherd.h,-
 [-]conf.h,-
 gopherdconf.h,-
 globals.h,-
 GDgopherdir.h,-
 DAarray.h,-
 GSgopherobj.h,-
 STRstring.h

GDGOPHERDIR_H =-
 GDgopherdir.h,-
 DAarray.h,-
 GSgopherobj.h,-
 STRstring.h

GSGOPHEROBJ_H =-
 GSgopherobj.h,-
 STRstring.h

MALLOC_H =-
 Malloc.h,-
 Stdlib.h

EXT_H =-
 ext.h,-
 DAarray.h,-
 STRstring.h,-
 boolean.h

SITE_H =-
 site.h,-
 DAarray.h,-
 STRstring.h,-
 boolean.h

! A default compilation procedure
.IFDEF DEC_C
.c.obj
	cc$(PREFIX)/warning=(disable=implicitfunc)/include=([-])-
/object=[-.obj]/define=($(TCPIP),SERVER) $<
.ELSE
.c.obj
	cc/include=([-])/object=[-.obj]/define=($(TCPIP),SERVER) $<
.ENDIF

! Define dependencies for all object files

[-.obj]compatible.obj : compatible.c $(MALLOC_H) compatible.h
.IFDEF DEC_C
	cc$(PREFIX)/warning=(disable=implicitfunc)/include=([-])-
		/object=[-.obj]/define=($(TCPIP),SERVER)/stand=vaxc $<
.ELSE
.c.obj
	cc/include=([-])/object=[-.obj]/define=($(TCPIP),SERVER) $<
.ENDIF

[-.obj]daarray.obj : daarray.c $(MALLOC_H) daarray.h

[-.obj]debug.obj : debug.c

[-.obj]ext.obj : ext.c $(MALLOC_H) $(EXT_H)

[-.obj]ftp.obj : ftp.c $(GOPHERD_H) debug.h

[-.obj]gdgopherdir.obj : gdgopherdir.c $(MALLOC_H)  $(GDGOPHERDIR_H)

[-.obj]globals.obj : globals.c $(GOPHERD_H)

[-.obj]gopherd.obj : gopherd.c $(GOPHERD_H) compatible.h

[-.obj]gopherdconf.obj : gopherdconf.c $(GOPHERD_H) $(MALLOC_H) string.h

[-.obj]gsgopherobj.obj : gsgopherobj.c $(MALLOC_H) $(GSGOPHEROBJ_H) \
		compatible.h string.h 

[-.obj]index.obj : index.c $(GOPHERD_H)

[-.obj]kernutils.obj : kernutils.c gopherd.h gopherdconf.h

[-.obj]site.obj : site.c $(MALLOC_H) $(SITE_H)

[-.obj]sockets.obj : sockets.c boolean.h compatible.h sockets.h

[-.obj]special.obj : special.c $(GOPHERD_H)

[-.obj]strstring.obj : strstring.c $(MALLOC_H) string.h strstring.h

[-.obj]util.obj : util.c $(MALLOC_H) string.h boolean.h util.h

[-.obj]waisgopher.obj : waisgopher.c
