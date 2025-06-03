$!
$! Make ImageMagick X image utilities for VMS.
$!
$
$define/nolog X11 decw$include:
$define/nolog sys sys$library:
$link_options="/nodebug/notraceback"
$define/nolog lnk$library sys$library:vaxcrtl
$
$if ((p1.nes."").and.(p1.nes."mogrify")) then goto SkipMogrify
$write sys$output "Making Mogrify..."
$call Make mogrify
$
$link'link_options' mogrify,[-]X,[-]image,[-]rotate,[-]quantize,[-]colors, -
  [-]signature,[-]compress,[-]alien,[-]PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$mogrify:==$'f$environment("default")'mogrify
$write sys$output "..symbol MOGRIFY defined."
$
$SkipMogrify:
$if ((p1.nes."").and.(p1.nes."convert")) then goto SkipConvert
$write sys$output "Making Convert..."
$call Make convert
$
$link'link_options' convert,[-]X,[-]image,[-]rotate,[-]quantize,[-]colors, -
  [-]signature,[-]compress,[-]alien,[-]PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$convert:==$'f$environment("default")'convert
$write sys$output "..symbol CONVERT defined."
$SkipConvert:
$if ((p1.nes."").and.(p1.nes."combine")) then goto SkipCombine
$write sys$output "Making Combine..."
$call Make combine
$
$link'link_options' combine,[-]X,[-]image,[-]rotate,[-]quantize,[-]colors, -
  [-]signature,[-]compress,[-]alien,[-]PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$combine:==$'f$environment("default")'combine
$write sys$output "..symbol CONVERT defined."
$SkipConvert:
$
$if ((p1.nes."").and.(p1.nes."MIFFtoSTEREO")) then goto SkipMIFFtoSTEREO
$write sys$output "Making MIFFtoSTEREO..."
$call Make MIFFtoSTEREO
$
$link'link_options' MIFFtoSTEREO,[-]X,[-]image,[-]rotate,[-]quantize, -
  [-]colors,[-]signature,[-]compress,[-]alien,[-]PreRvIcccm,sys$input:/opt
sys$share:decw$xlibshr.exe/share
$
$MIFFtoSTEREO:== $'f$environment("default")'MIFFtoSTEREO
$write sys$output "..symbol MIFFTOSTEREO defined."
$
$SkipMIFFtoSTEREO:
$type sys$input

Use this command to specify which X server to contact:

  $set display/create/node=node_name::

This can be done automatically from your LOGIN.COM with the following 
command:

  $if (f$trnlmn("sys$rem_node").nes."") then -
  $  set display/create/node='f$trnlmn("sys$rem_node")'
$exit
$
$Make: subroutine
$!
$! A very primitive "make" (or MMS) hack for DCL.
$!
$if (p1.eqs."") then exit
$source_file=f$search(f$parse(p1,".c"))
$if (source_file.nes."")
$  then
$    object_file=f$parse(source_file,,,"name")+".obj"
$    object_file=f$search( object_file )
$    if (object_file.nes."") 
$      then
$        object_time=f$file_attribute(object_file,"cdt")
$        source_time=f$file_attribute(source_file,"cdt")
$        if (f$cvtime(object_time).lts.f$cvtime(source_time)) then -
$          object_file=""
$      endif
$    if (object_file.eqs."") 
$      then
$        write sys$output "Compiling ",p1
$        cc/nodebug/optimize/include_directory=[-] 'source_file'
$      endif
$  endif
$exit
$endsubroutine
