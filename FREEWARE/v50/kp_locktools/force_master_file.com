$	old_verify = f$verify('f$trnlnm("FORCE_MASTER_FILE$VERIFY")'+0)
$! FORCE_MASTER_FILE.COM
$!
$! Try to induce lock mastership for a specified file to move to this node.
$!
$! P1:	Filespec
$!						K.Parris 03/97
$! Generates lots of lock requests on the RMS lock tree for an RMS indexed
$! file, in anticipation that VMS will re-master that lock tree to this node.
$!
$! Be sure PE1 is 0 on the current lock-master node to enable lock trees to
$! be remastered off that node.
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	file = f$search(p1)
$	if file .eqs. ""
$	then
$		write sys$output "File ",p1," not found."
$		exit 1+0*f$verify(old_verify)
$	endif
$	write sys$output "File: ",p1," (",file,")"
$! Find out the File ID of the file of interest, so we can determine the
$! resource name of the RMS file lock based on that
$	fid = f$file_attributes(p1,"FID") - "(" - ")"	!File ID
$! show symbol fid	!!!
$	filnum = f$integer(f$element(0,",",fid))	!File number
$	seqnum = f$integer(f$element(1,",",fid))	!Sequence number
$	rvolnum = f$integer(f$element(2,",",fid))	!Relative volume number
$! show symbol filnum	!!!
$! show symbol seqnum	!!!
$! show symbol rvolnum	!!!
$	filx = filnum/65536	!File number extension byte
$	filn = filnum - (filx*65536)
$! show sym filx	!!!
$! show sym filn	!!!
$	fidhex = f$fao("!XW,!XW,!XB,!XB",filn,seqnum,rvolnum,filx)
$! show sym fidhex	!!!
$	volname = f$getdvi(f$parse(p1,"DEVICE"),"VOLNAM")
$	volnam = f$extract(0,12,volname+"            ")	!Pad with blanks
$! show sym volname	!!!
$!
$!	write sys$output "Resource name is ""RMS$"",",fidhex,",02,""''volnam'"""
$	resnam = "RMS$"	!resnam[<0 through 3>*8,8]
$	filn2 = filn/256
$	filn1 = filn - 256*filn2
$	resnam[4*8,8] = filn1
$	resnam[5*8,8] = filn2
$	seqnum2 = seqnum/256
$	seqnum1 = seqnum - 256*seqnum2
$	resnam[6*8,8] = seqnum1
$	resnam[7*8,8] = seqnum2
$!	reshex = f$fao("!6(XB)",filn1,filn2,seqnum1,seqnum2,rvolnum,filx)
$	resnam[8*8,8] = rvolnum
$	resnam[9*8,8] = filx
$	resnam[10*8,8] = %x02
$	resnam = resnam + volnam
$	resnam[23*8,8] = 0
$	resnam[24*8,8] = 0
$	resnam[25*8,8] = 0
$! show symbol resnam	!!!
$	i = 0
$	reshex = ""
$ loop:
$	byte = f$cvui(i*8,8,resnam)
$	reshex = reshex + " " + f$fao("!XB",byte)
$	i = i + 1
$	if i .lt. f$length(resnam) then goto loop
$! show sym reshex	!!!
$!
$	LOTSALOX_RESOURCE = resnam
$ write sys$output "Running on node ",f$getsyi("nodename")
$ show sym resnam
$	LOTSALOX_ACMODE = "EXEC"	!RMS locks are in EXEC mode
$	proc = f$environment("procedure")
$	proc_dir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$	run 'proc_dir'LOTSALOX
$ write sys$output "LOTSALOX completed."
$	exit 1+0*f$verify(old_verify)
