          .Title        DELIVER error messages
 
! Written by Ned Freed, 24-Sep-1985, modified 30-Apr-1986
 
          .Facility     DELIVER,667/prefix=DELIVER__
          .Ident        'DELIVER Version 1.4'
 
     .Severity  fatal
 
        INTSTKOVR    <Internal error, stack overflow>
        STKEMPTY     <Internal error, stack empty>
        BADSTKELE    <Internal error, bad element found on stack>
 
     .Severity  error
 
        CANACCUAF    <No privileges to access SYSUAF.DAT>
	GOTNOSYSPRV  <Don't have SYSPRV>
	NOTPRIV      <Don't have needed priviledges>
        NOSUCHUSER   <No such user exists in SYSUAF.DAT>
        USERNOEXIST  <Specified user "!AD" does not exist>/FAO=2
        NAMETOOLONG  <User name "!AD" is too long>/FAO=2
        NODEFAULTDIR <No default directory exists for user "!AD">/FAO=2
        NOMDFILE     <No MAIL.DELIVERY file exists for user "!AD">/FAO=2
        MESREAERR    <Error reading intermediate message file, status = !UL>/FAO=1
        MESWRTERR    <Message file write error, Pascal status = !UL>/FAO=1
        GETFILERR    <Can't get file name from CLI, status = !UL>/FAO=1
        MESOPNERR    <Error opening message file, Pascal status = !UL>/FAO=1
        MSGWRTERR    <Error writing intermediate message file, status = !UL>/FAO=1
        MSGREAERR    <Error reading message file, Pascal status = !UL>/FAO=1
        UNKFUNC      <Foreign MAIL protocol invoked with unknown function !UL.>/   FAO=1
 
     .Severity warning
 
        TOOMANYPARAMS <Too many parameters on line !UL of MAIL.DELIVERY file>/FAO=1
        TOOFEWPARAMS  <Too few parameters on line !UL of MAIL.DELIVERY file>/FAO=1
        MDIGNORED     <MAIL.DELIVERY file ignored>
        NORULES       <No rules found in MAIL.DELIVERY file>
 
     .End
