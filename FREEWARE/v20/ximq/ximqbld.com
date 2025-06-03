$!
$! For AXP DECC compiles:
$!
$!	compile :== cc/stand=vaxc/opt/assume=align/prefix=all/gran=long
$!
$! For VAX VAX-C compiles:
$!
$!	compile :== cc/opt
$
$ arch = f$getsyi("ARCH_TYPE")
$
$ if P1 .nes. "" then goto 'P1'
$
$ write sys$output "Compile XIMQ modules"
$
$ if arch .eq. 2
$ then
$ compile :== cc/stand=vaxc/opt/assume=align/prefix=all/gran=long
$ else
$ compile :== cc/opt
$ endif
$ compile ximq
$ compile ximq_decompress
$ compile ximq_fileio
$ compile ximq_image_finder
$ compile ximq_message_display
$ compile ximq_image_processing
$ compile find_position
$
$LINK:
$
$ write sys$output "Insert into library"
$
$ lib/cre ximq *.obj;*
$ del *.obj;*
$
$ write sys$output "Link XIMQ"
$
$ if arch .eq. 2
$ then
$ link /exe=ximq -
       XIMQ/LIB/INCLUDE=( ximq,-
                          ximq_decompress,-
                          ximq_fileio,-
                          ximq_image_finder,-
                          ximq_image_processing,-
                          ximq_message_display,-
                          find_position),-
       sys$input/opt
sys$library:decw$xlibshr/share
$ else
$ link /exe=ximq_vax -
       XIMQ/LIB/INCLUDE=( ximq,-
                          ximq_decompress,-
                          ximq_fileio,-
                          ximq_image_finder,-
                          ximq_image_processing,-
                          ximq_message_display,-
                          find_position),-
       sys$input/opt
sys$library:decw$xlibshr/share
sys$library:vaxcrtl/share	! For VAX compiles
$ endif
$ pur *.exe
$ exit
