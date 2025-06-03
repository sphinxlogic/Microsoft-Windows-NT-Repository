	.file	"InitClnStub.s"

/#####################################################################
/# InitClnStub.s						     #
/# ----------------------------------------------------------------- #
/# This stub goes to .init section of the client binary and calls    #
/# the code which binds the client to the library		     #
/# Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany       #
/# (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)		     #
/#####################################################################

/# $XFree86: mit/lib/Xt/InitClnStub.s,v 2.0 1993/09/30 17:35:13 dawes Exp $

.section .init,"x" 

	.align 4

	.globl __bind_to_sv3shlib_

/ bind client to library (init pointers in subclasses of the client)
/ _bind_to_sv3shlib_() must be supplied by the client, otherwise
/ an empty dummy function from the host shared library is linked to it

__bind_to_sv3shlib_:
	pushl %ebp
	movl %esp,%ebp
/ call function in client
	call _bind_to_sv3shlib_	
	leave


