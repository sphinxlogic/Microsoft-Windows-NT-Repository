$! MAKE.COM      Compile and link SOUNDTOOLS..
$!
$ ON ERROR then $GOTO COMP_ERROR
$ set verify
$!--------------------------------------
$define vaxc$include   sys$library
$define lnk$library    sys$library:vaxccurse.olb
$define lnk$library_1  sys$library:vaxcrtl.olb
$DEFINES = "/define=("""__STDC__""","""VMS""")"
$CC 'defines 8SVX.C           
$CC 'defines AIFF.C           
$CC 'defines AU.C             
$CC 'defines AVG.C            
$CC 'defines BAND.C           
$CC 'defines COPY.C           
$CC 'defines ECHO.C           
$CC 'defines GETOPT.C         
$CC 'defines HANDLERS.C       
$CC 'defines HCOM.C           
$CC 'defines LIBST.C          
$CC 'defines LOWP.C           
$CC 'defines MISC.C           
$CC 'defines PRED.C           
$CC 'defines RATE.C           
$CC 'defines RAW.C            
$CC 'defines SF.C             
$CC 'defines SNDRTOOL.C       
$CC 'defines SOX.C            
$CC 'defines STAT.C           
$CC 'defines VIBRO.C          
$CC 'defines VOC.C            
$!
$ write sys$output "Compilation completed succesessfully..."
$!
$! -------------------------------------------------------------------
$ ON ERROR THEN $GOTO LINK_ERR
$ write sys$output "Now linking.   Ignore warnings about ERRNO..."
$!
$ set noverify
$ Link/Map/Full SOX,SOX.Opt/Option
$ EXIT
$! -------------------------------------------------------------------
$LINK_ERR:
$ write sys$output "LINK failed..."
$ set noverify
$ EXIT
$!--------------------------------------
$COMP_ERROR:
$ write sys$output "Compilation failed..."
$ set noverify
$ exit
$!--------------------------------------
