$! This procedure will permit relinking of RESET_PASSWD even if user does not
$! have a Dec C compiler installed. Primary usage would be if user's VMS
$! version is not same as supplied RESET_PASSWD.EXE (23-Jan-2001)
$
$   echo = "WRITE SYS$OUTPUT"
$   on warning then goto OOOPS
$   echo "Relinking RESET_PASSWD..."
$   If f$getsyi("HW_MODEL") .ge. 1024
$    Then
$      link/notrace/exec=[.axp]reset_passwd.exe/sysexe -
        [.axp]reset_passwd.olb/incl=reset_passwd/library, -
        sys$loadable_images:dcldef.stb
$    Else	link/notrace/exec=[.vax]reset_passwd.exe -
        [.vax]reset_passwd.olb/incl=reset_passwd/library, -
	    sys$input/opt
sys$system:sys.stb/selective_search
$   Endif
$   exit
$OOOPS:
$   echo "Link Unsuccessful, exiting"
$   exit
