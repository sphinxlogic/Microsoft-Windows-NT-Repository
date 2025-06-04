$! This procedure will permit relinking of PERSONA even if user does not
$! have a Dec C compiler installed. Primary usage would be if user's VMS
$! version is not same as supplied PERSONA.EXE (23-Jan-2001)
$
$   echo = "WRITE SYS$OUTPUT"
$   on warning then goto OOOPS
$   echo "Relinking PERSONA..."
$   If f$getsyi("HW_MODEL") .ge. 1024
$    Then
$      link/notrace/exec=[.axp]persona.exe/sysexe -
        [.axp]persona.olb/incl=persona/library, -
        sys$loadable_images:dcldef.stb
$    Else	link/notrace/exec=[.vax]persona.exe -
        [.vax]persona.olb/incl=persona/library, -
	    sys$input/opt
sys$system:sys.stb/selective_search
$   Endif
$   exit
$OOOPS:
$   echo "Link Unsuccessful, exiting"
$   exit
