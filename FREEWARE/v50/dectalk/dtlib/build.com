$!
$! This command file is run on a Vax/VMS system (which has the
$! Decus C toolkit installed) to build the DECtalk support library.
$! It creates command files for vax native, vax compatibility,
$! and RSTS/E.  Then, it compiles both vax libraries and
$! the documentation.
$!
$	was_verify := 'f$verify(1)'
$	set default [.source]
$	purge *.*
$	delete *.obj;*,vdtlib.olb;*,xdtlib.olb;*
$!
$! The build utility creates command files to compile the
$! DECtalk library.  If you do not have build installed,
$! and haven't added or removed files from the library,
$! you may comment out the next three lines without problems.
$!
$	build -x vaxnative	-l vdtlib.olb *.c >vdtlib.com
$	build -x vaxrsx		-l xdtlib.olb *.c >xdtlib.com
$	build -x rstsrt		-l  dtlib.obj *.c >rdtlib.cmd
$	@vdtlib				! Vax-11 C library compilation
$	rename vdtlib.olb [-]
$	@xdtlib				! Decus C library compilation
$	rename xdtlib.olb [-]
$	purge *.*			! Cleanup unneeded files
$	delete *.obj;*			! Don't need objects now
$	set default [-]			! Back to [.dtlib]
$	@dtdoc				! Build documentation
$	purge *.*			! Cleanup unneeded files
$	was_verify := 'f$verify(was_verify)'	! Done
