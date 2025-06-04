$!FDHSTIMG STARTUP
$ask:==inquire
$oldprv=f$setprv("cmkrnl,sysprv,log_io,phy_io,tmpmbx,volpro")
$tprv=f$privilege("cmkrnl,sysprv,log_io,phy_io,tmpmbx,volpro")
$if (tprv) then goto gotprv
$write sys$output "Not enough privileges."
$exit
$gotprv:
$!if (f$search("sys$system:DTdriver.exe") .nes. "") then
$! write sys$output "Cannot find DTdriver...aborting"
$! exit
$!endif
$!! See if the driver is loaded by checking unit 0
$!if(f$getdvi("DTA0:","EXISTS")) THEN GOTO GOTFQ
$! WRITE SYS$OUTPUT "Cannot find DTA0: device. Loading."
$!sysgen:==$sysgen
$!sysgen connect DTA0:/noadapter/driver=fqdriver
$!if(f$getdvi("FQA0:","EXISTS")) THEN GOTO GOTFQ
$!write sys$output "Loading unsuccessful. Exiting."
$!exit
$gotfq:
$ask dvc "Give the device name for the virtual disk [DTA0:]:"
$if (dvc.eqs."")then dvc = "_DTA0:"
$if(f$getdvi(dvc,"EXISTS")) THEN GOTO DVCEXIST
$ WRITE SYS$OUTPUT "Cannot find device. Loading DTA0: if possible."
$sysgen:==$sysgen
$sysgen connect DTA0:/noadapter/driver=fqdriver
$dvc="DTA0:"
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
$! we have a free DT type device now.
$ask tp "Please enter the tape device [MUA0:]:"
$if (tp .eqs. "") then tp = "mua0:"
$if(f$getdvi(tp,"EXISTS")) THEN GOTO MDVCEXIST
$cfd = "Cannot find device "
$cfdv:='cfd''tp'
$WRITE SYS$OUTPUT cfdv
$exit
$mdvcexist:
$ask ctyp "Should cache be in Tape blocks instead of Pages [Y/N]?"
$ctf=0
$if f$extract(1,1,f$edit(ctyp,"UPCASE,COLLAPSE")) .EQS. "Y" THEN CTF=1
$IF CTF.eq.1 then ask csiz "Please enter cache size in tape blks [300]:"
$IF CTF.eq.0 then ask csiz "Please enter cache size in pages [3000]:"
$if ctf .eq. 0 then goto pgdft
$if csiz .gt. 10000 then csize=10000
$if csiz .eqs. "" then csiz=300
$goto gskp
$pgdft:
$if csiz .gt. 100000 then csize=100000
$if csiz .eqs. "" then csiz=3000
$gskp:
$ask skip "How many files should we skip [0]"
$if skip .le. 0 then skip = 0
$! Emit a command file and start a subprocess to handle the tape.
$open/write cmf sys$login:fdh.com
$write cmf "$set verify"
$write cmf "$set command sys$system:fdhstimg2"
$write cmf "set proc/name=dt_control"
$mfr = "$mount/for/multi "
$mfr2:='mfr''tp'
$write cmf mfr2
$fh1 = "$fdhstimg/tcache:"
$if ctf.eq.0 then fh1 = "$fdhstimg/cache:"
$fh2 = "/skip:"
$fhi := 'fh1''csiz''fh2''skip' 'dvc' 'tp'
$write cmf fhi
$write cmf "$eoj"
$close cmf
$run/det/auth/out=nl:/input=sys$login:fdh.com sys$system:loginout
$write sys$output "Waiting for disk to come online now"
$lpctr=0
$wtonl:
$wait 0:0:15
$tstt=f$getdvi(dvc,"sts")
$! test online bit. If not online just warn.
$ionline=16
$oln = ionline .and. tstt
$if oln .ne. 0 then goto lpdun
$lpctr = lpctr+1
$if lpctr.lt.12 goto wtonl
$write sys$output "Waiting is taking a long time"
$write sys$output "Here is the show device display."
$write sys$output "If tape is OK please mount when the device becomes ready"
$write sys$output "Otherwise you can abort the 'DT_CONTROL' process now."
$lpdun:
$ show dev 'dvc'
$! wait 1 minute
$! test device to see if it's online, and when it is mount it and
$! do dir list of dvc:[000000]000000.dir to get the MFD read in.
$set nover
