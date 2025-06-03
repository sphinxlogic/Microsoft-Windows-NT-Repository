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

/# $XFree86: mit/lib/Xmu/InitLibStub.s,v 1.2 1993/03/27 09:17:05 dawes Exp $

.section .init,"x" 

	.align 4
	.globl __libXmu_init_stub_

__libXmu_init_stub_:

/ check revision of target shared library

        pushl %ebp
        movl %esp,%ebp
        call __check_libXmu_rev_
        leave

