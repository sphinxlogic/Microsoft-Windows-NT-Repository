$! FREEWARE_COPYRIGHT.COM - Freeware CD-ROM Copyright and Version Definitions
$!
$! Copyright 2003 Hewlett-Packard Company
$! Copyright 2001 Compaq Computer Corporation
$!
$! p1  = procedure name being executed
$! p2  = required privileges of the procedure
$! p3  = DCL Boolean string; TRUE/YES if quota checking needed
$!
$! Result:  freeware_version  = this version
$!          Freeware_status   = ERROR  if an error
$
$
$ freeware_status  == "ERROR"
$ freeware_version == "V6.0"
$
$ write sys$output " "
$ write sys$output "Welcome to the OpenVMS Freeware ''freeware_version' CD-ROM."
$
$ type sys$input

Copyright 2003 by Hewlett-Packard Company
Copyright 2001 by Compaq Computer Corporation
  ...menu system developed by Touch Technologies, Inc


The OpenVMS Freeware CD-ROM contains free software tools and utilities to aid
software developers in creating applications and managing and using OpenVMS
systems.  Contained on this CD-ROM are dozens of packages for VAX and Alpha.

The OpenVMS Freeware CD-ROM (for OpenVMS I64, OpenVMS Alpha and OpenVMS VAX
Systems) is provided "AS IS", free of charge, and without any warranty express
or implied.  HP makes no claims of the quality, content, or usefulness of this
software.  This software is provided to OpenVMS customers and hobbyists as a
free service. 

HP provides no support and no services for packages here, and the owners of 
specific packages here may or may not choose to provide support, services or
to accept problem reports or requests related to any software found here.

HP imposes no additional and no specific restrictions on this OpenVMS Freeware 
distribution, nor on the redistribution of any packages on it.  

Included within the [FREEWARE] directory on this CD-ROM is a copy of the
GNU software license.  (For additional information on the GNU license, please
contact the Free Software Foundation, 59 Temple Place, Suite 330, Boston, MA
02111-1307, USA.)  Various of the packages on the CD-ROM may carry this or 
another GNU license, and/or other restrictions and/or copyrights, and/or
specific restrictions on the package imposed by the original authors and/or
current owners.  Accordingly, you should carefully read the documentation 
for each product of interest.

$
$ version = f$getsyi("version")
$! typvers = f$extract(0,1,version)
$ version = f$extract(1,16,version)
$ majvers = f$integer(f$element(0,".",version))
$ version = f$element(1,".",version)
$ minvers = f$integer(f$element(0,"-",version))
$ version = f$element(1,"-",version)
$ updvers = f$integer(f$element(0,1,version))
$ updvers2 = f$integer(f$element(0,2,version))
$ updvers3 = f$integer(f$element(0,3,version))
$ if updvers .lt. updvers2 then updvers = updvers2
$ if updvers .lt. updvers3 then updvers = updvers3
$ synvers = f$fao("!2ZB!2ZB!2ZB",majvers,minvers,updvers)
$
$! Allow a lower version (V5.5-2) on OpenVMS VAX, but require
$! force OpenVMS Alpha and (implicitly) OpenVMS I64 to a higher 
$! minimum version (V6.2).  While there is no version of OpenVMS
$! I64 below V8.0 (since I64 models are also larger than 1024),
$! that version is above the required version minimum and thus
$! the test works correctly.
$!
$ if f$getsyi("HW_MODEL") .gt. 1024
$ then 
$   ! OpenVMS Alpha V6.2 (and OpenVMS I64 V8.0) are allowed
$   min_vversion = "V6.2"
$   min_version = "060200"
$ else
$   ! OpenVMS VAX V5.5-2 (and even A5.5-2) are allowed
$   min_vversion = "V5.5-2"
$   min_version  = "050502"
$ endif
$
$ bell[0,8] = 7
$
$ if version .lts. min_version then goto bad_version
$
$ if p2 .eqs. "" then goto okay
$ if f$privilege("''p2'") .eqs. "FALSE" then goto nopriv
$
$ if .not. p3 then goto okay  ! no quota checking requested
$
$ bytlm_min = 24000
$ bytlm = f$getjpi("000000","bytlm")
$ if bytlm .le. bytlm_min then goto bad_quota
$
$okay:
$ freeware_status == "OKAY"
$ exit
$
$nopriv:
$   write sys$output " "
$   write sys$output bell,"Freeware CD-ROM requires additional privilege(s)."
$   write sys$output "Required privileges are:"
$   write sys$output "  ''p2'"
$   write sys$output " "
$   exit
$
$bad_quota:
$   write sys$output " "
$   write sys$output bell,"Freeware CD-ROM requires BYTLM of ''bytlm_min', or higher."
$   write sys$output "Current BYTLM value: ''bytlm'"
$   write sys$output " "
$   exit
$
$bad_version:
$   write sys$output " "
$   write sys$output bell,-
       "Freeware CD-ROM requires OpenVMS 'min_version', or higher."
$   write sys$output " "
$   exit
