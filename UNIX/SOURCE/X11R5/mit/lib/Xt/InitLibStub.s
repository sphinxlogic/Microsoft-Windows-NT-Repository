	.file	"InitLibStub.s"

/#####################################################################
/# InitLibStub.s						     #
/# ----------------------------------------------------------------- #
/# This stub goes to .init section of the client binary. It checks   #
/# the revision of the library and calls the init code for the       #
/# library							     #
/# Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany       #
/# (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)		     #
/#####################################################################

/# $XFree86: mit/lib/Xt/InitLibStub.s,v 2.0 1993/08/22 12:36:56 dawes Exp $

.data
/ create unresolved reference
	.long __bind_to_sv3shlib_

.section .init,"x" 

	.align 4
	.globl __libXt_init_stub_

__libXt_init_stub_:

/ check revision of target shared library

        pushl %ebp
        movl %esp,%ebp
        call __check_libXt_rev_
        leave

/ inititialize library

        pushl %ebp
        movl %esp,%ebp
        call __default_libXt_init_
        leave

        pushl %ebp
        movl %esp,%ebp
        call __libXt_init_
        leave


