$! INT_DEMO_COPYRIGHT.COM - INT_DEMO Copyright and Version Definitions
$!
$! p1  = procedure name being executed
$! p2  = required privileges of the procedure
$! p3  = NO  if no quota checking is needed
$!
$! Result:  int_demo_version  = this version
$!          int_demo_status   = ERROR  if an error
$
$ int_demo_status  == "ERROR"
$ int_demo_version == "4.1"
$
$ write sys$output -
    "INTOUCH Demo - VAX INTOUCH Demonstration (V''INT_DEMO_version') - ''p1' Procedure"
$ write sys$output "Copyright (c) 1994 Touch Technologies, Inc."
$ write sys$output ""
$
$ if p2 .eqs. "" then goto no_priv_required
$ if f$privilege("''p2'") .eqs. "FALSE" then goto nopriv
$
$ no_priv_required:
$! Define various missing logicals, if necessary
$ if f$trnlnm("SYS$LOGIN") .eqs. "" then -
    define/process sys$login tti_INT_DEMO
$ if f$trnlnm("SYS$SCRATCH") .eqs. "" then -
    define/process sys$scratch tti_INT_DEMO
$
$ delete :==""
$ create :==""
$ delete/symbol/global delete
$ delete/symbol/global create
$! Check SYS$SCRATCH for write access
$
$ scratch_file = "sys$scratch:temp_''INT_DEMO_local_nodename'.tmp"
$ if f$search("''scratch_file'") .nes. ""  then -
     delete 'scratch_file';*
$ on warning then goto noscratch
$ define/user sys$error  nla0:
$ define/user sys$output nla0:
$ create 'scratch_file'
$ create 'scratch_file'
$
$ on warning then goto version_limit
$ delete 'scratch_file';  ! get rid of version 2
$ delete 'scratch_file';  ! try to get rid of version 1 also
$ set noon
$
$! C H E C K   Q U O T A S
$!
$ if p3 .eqs. "NO" then goto okay  ! no quota checking
$
$ a=f$getjpi("","bytlm")
$ if a .le. 23000 then goto bad_bytlm
$
$ a=f$getjpi("","pgflquota")
$ if a .le. 20000 then goto bad_pgflquota
$
$ a=f$getsyi("virtualpagecnt")
$ if a .le. 20000 then goto bad_virtualpagecnt
$
$ okay:
$ INT_DEMO_status == "OKAY"
$ set process/resource_wait  ! enable waiting for resources...just in case
$ exit
$
$ nopriv:
$   write sys$output ""
$   write sys$output "Insufficient privilege...Required privileges are:"
$   write sys$output "        ''p2'"
$   write sys$output ""
$   exit
$
$ noscratch:
$   scratch_dir = f$trnlnm("sys$scratch")
$   write sys$output "INTOUCH Demo requires write access to SYS$SCRATCH:"
$   write sys$output "SYS$SCRATCH --> ''scratch_dir'"
$   write sys$output ""
$   exit
$
$ version_limit:
$   scratch_dir = f$trnlnm("sys$scratch")
$   write sys$output -
     "Directory version limit of SYS$SCRATCH: must be at least 2"
$   write sys$output "SYS$SCRATCH --> ''scratch_dir'"
$   write sys$output ""
$   exit
$
$ bad_bytlm:
$   temp = f$getjpi("", "bytlm")
$   write sys$output "INTOUCH Demo requires BYTLM >= 24000"
$   write sys$output "Current BYTLM --> ''temp'"
$   write sys$output ""
$   exit
$
$ bad_pgflquota:
$   temp = f$getjpi("", "pgflquota")
$   write sys$output "INTOUCH Demo requires PGFLQUO >= 20480"
$   write sys$output "Current PGFLQUO --> ''temp'"
$   write sys$output ""
$   exit
$
$ bad_virtualpagecnt:
$   temp = f$getsyi("virtualpagecnt")
$   write sys$output "INTOUCH Demo requires VIRTUALPAGECNT >= 20480"
$   write sys$output "Current VIRTUALPAGECNT --> ''temp'"
$   write sys$output ""
$   exit
