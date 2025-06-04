$! FREEWARE_COPYRIGHT.COM - Freeware CD-ROM Copyright and Version Definitions
$!
$! Copyright 2001 Compaq Computer Corporation
$!
$! p1  = procedure name being executed
$! p2  = required privileges of the procedure
$! p3  = NO  if no quota checking is needed
$!
$! Result:  freeware_version  = this version
$!          Freeware_status   = ERROR  if an error
$ bell[0,8] = 7
$
$ version=f$extract(1,3,f$getsyi("version"))-"."  ! makes V5.2 --> 52
$
$ freeware_status  == "ERROR"
$ freeware_version == "V5.0"
$
$ write sys$output ""
$ write sys$output "OpenVMS Freeware CD-ROM (Version ''freeware_version')"
$
$ type sys$input

Copyright 2001 by Compaq Computer Corporation
  ...menu system developed by Touch Technologies, Inc

Welcome to Compaq Computer Corporation's OpenVMS Freeware CD-ROM!

The OpenVMS Freeware CD-ROM contains free software tools and utilities to aid
software developers in creating applications and managing and using OpenVMS
systems.  Contained on this CD-ROM are dozens of packages for VAX and Alpha.

The OpenVMS Freeware for VAX and Alpha Systems CD-ROM is provided "AS IS", free
of charge, and without warranty.  Compaq makes no claims about the quality of
this software.  It is provided to the customer as a free service.  

Compaq imposes no restrictions on its distribution, nor on the redistribution
of anything on it.  Included in the [FREEWARE] directory on this CD are the GNU
software licenses. Unless otherwise stated in sources or product documentation,
the GNU licenses cover the software supplied on this CD.  Some of the packages
on the CD-ROM may carry restrictions on their use imposed by the original
authors.  Therefore, you should carefully read the documentation accompanying 
the products.

$
$ arch = "VAX"
$ min_version = "50"
$ if f$getsyi("HW_MODEL") .gt. 1024 then arch = "AXP"
$ if arch .eqs. "AXP" then min_version = "15"
$
$ if version .lts. min_version then goto bad_version
$
$ if p2 .eqs. "" then goto okay
$ if f$privilege("''p2'") .eqs. "FALSE" then goto nopriv
$
$ if p3 .eqs. "NO" then goto okay  ! no quota checking
$
$ a=f$getjpi("","bytlm")
$ if a .le. 12000 then goto bad_bytlm
$
$ okay:
$ freeware_status == "OKAY"
$ exit
$
$ nopriv:
$   write sys$output ""
$   write sys$output bell,"Insufficient privilege...Required privileges are:"
$   write sys$output "        ''p2'"
$   write sys$output ""
$   exit
$
$ bad_bytlm:
$   temp = f$getjpi("", "bytlm")
$   write sys$output bell,"Freeware CD requires BYTLM >= 12000"
$   write sys$output "Current BYTLM --> ''temp'"
$   write sys$output ""
$   exit
$
$ bad_version:
$   write sys$output ""
$   write sys$output bell,-
       "Freeware-CDROM requires OpenVMS V'min_version'' or higher"
$   write sys$output ""
$   exit
