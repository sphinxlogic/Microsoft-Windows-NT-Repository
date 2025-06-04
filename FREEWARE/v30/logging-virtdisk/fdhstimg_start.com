$!FDHSTIMG STARTUP
$ask:==inquire
$oldprv=f$setprv("cmkrnl,sysprv,log_io,phy_io,tmpmbx,volpro")
$tprv=f$privilege("cmkrnl,sysprv,log_io,phy_io,tmpmbx,volpro")
$if (tprv) then goto gotprv
$write sys$output "Not enough privileges."
$exit
$gotprv:
$!if (f$search("sys$system:fqdriver.exe") .nes. "") then
$! write sys$output "Cannot find fqdriver...aborting"
$! exit
$!endif
$!! See if the driver is loaded by checking unit 0
$!if(f$getdvi("FQA0:","EXISTS")) THEN GOTO GOTFQ
$! WRITE SYS$OUTPUT "Cannot find FQA0: device. Loading."
$!sysgen:==$sysgen
$!sysgen connect fqa0:/noadapter/driver=fqdriver
$!if(f$getdvi("FQA0:","EXISTS")) THEN GOTO GOTFQ
$!write sys$output "Loading unsuccessful. Exiting."
$!exit
$gotfq:
$ask dvc "Give the device name for the virtual disk [FQA0:]:"
$if (dvc.eqs."")then dvc = "_FQA0:"
$if(f$getdvi(dvc,"EXISTS")) THEN GOTO DVCEXIST
$cfd = "Cannot find device "
$cfdv:='cfd''dvc'
$WRITE SYS$OUTPUT cfdv
$exit
$dvcexist:
$if(f$getdvi(dvc,"REFCNT") .EQ. 0) THEN GOTO DVCFRE
$cfd="Device "
$cfd2=" is in use...exiting"
$cfd3:='cfd''dvc''cfd2'
$WRITE SYS$OUTPUT cfd3
$exit
$dvcfre:
$! we have a free fq type device now.
$ask tp "Please enter the tape device [MUA0:]:"
$if (tp .eqs. "") then tp = "mua0:"
$if(f$getdvi(tp,"EXISTS")) THEN GOTO MDVCEXIST
$cfd = "Cannot find device "
$cfdv:='cfd''tp'
$WRITE SYS$OUTPUT cfdv
$exit
$mdvcexist:
$ask csiz "Please enter cache size in tape blks [200]:"
$if csiz .gt. 10000 then csize=10000
$if csiz .eqs. "" then csiz=200
$ask skip "How many files should we skip [0]"
$if skip .le. 0 then skip = 0
$! Emit a command file and start a subprocess to handle the tape.
$open/write cmf fdh.com
$write cmf "$set verify"
$write cmf "$set command sys$system:fdhstimg"
$mfr = "$mount/for/multi "
$mfr2:='mfr''tp'
$write cmf mfr2
$fh1 = "$fdhstimg/cache:"
$fh2 = "/skip:"
$fhi := 'fh1''csiz''fh2''skip' 'dvc' 'tp'
$write cmf fhi
$write cmf "$eoj"
$close cmf
$!spawn/nowait/input=fdh.com/output=fdh.jnl
$set nover
