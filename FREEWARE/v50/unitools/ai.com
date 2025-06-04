$ verflg = 'f$verify(0)'
$!------------------------------------------------------------------------------
$!  AI.COM   Do an Analyze /Image and return the image identification section
$!	     of the specified image(s).
$!
$!  Usage:   @AI [[p1] [p2]]
$!		where  p1 = image name (can have wildcards).
$!		       p2 = anything - if present, do NOT display executable
$!			    filename.
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne.
$!           Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	15-Aug-1991	JER	Original version.
$!------------------------------------------------------------------------------
$
$ set control=(Y,T)
$ on control_y  then  goto  EXIT_DOOR
$ on error      then  goto  EXIT_DOOR
$
$ say := write SYS$OUTPUT
$ bell[0,8] = 7
$ outfile = ""
$
$ image = p1
$ show_exe = 1
$ if p2 .nes. ""  then  show_exe = 0
$
$GET_IMAGE:
$ if image .eqs. ""  then  inquire /nopunct image "Image name? "
$ image = f$edit (image,"TRIM,COMPRESS,UNCOMMENT")
$ if image .eqs. ""  then  goto  GET_IMAGE
$
$ image_string = "SYS$SYSTEM:''image'" - ".EXE" + ".EXE"
$ full_image = f$search (image_string)
$ if full_image .eqs. ""
$ then
$   image_string = "SYS$LIBRARY:''image'" - ".EXE" + ".EXE"
$   full_image = f$search (image_string)
$ endif
$ if full_image .eqs. ""
$ then
$   image_string = image - ".EXE" + ".EXE"
$   full_image = f$search (image_string)
$ endif
$
$ if full_image .eqs. ""
$ then
$   say "%Error: could not find image file." + bell
$ else
$   outfile := SYS$SCRATCH:ai_search_'f$getjpi("","PID")'.tmp
$   first_image = full_image
$   blank = show_exe
$LOOP:
$   if show_exe
$   then
$     say full_image
$     say ""
$   endif
$   set noon
$   define /user SYS$ERROR  NL:
$   define /user SYS$OUTPUT NL:
$   analyze /image 'full_image' /out='outfile'
$   zip = blank + 2 - (show_exe * 2)
$   search 'outfile' "image name", "image file identification", -
		     "link date/time", "linker identification"  /nohighlight
$   delete /nolog 'outfile';*
$   set on
$   blank = 1
$   full_image = f$search (image_string)
$   go_go = (full_image .nes. "") .and. (full_image .nes. first_image)
$   if go_go .and. show_exe  then  say ""
$   if go_go  then  goto  LOOP
$ endif
$
$EXIT_DOOR:
$ if f$search (outfile) .nes. ""  then  delete /nolog 'outfile';*
$ EXIT 1 + 0 * f$verify (verflg)
