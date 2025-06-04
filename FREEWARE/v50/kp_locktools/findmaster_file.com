$	old_verify = f$verify('f$trnlnm("findmaster_file$verify")'+0)
$!
$! Find the lock master for a given file's lock tree
$!
$! P1:	Filespec
$!						K.Parris 03/97
$! Modification history:
$!  KP 05/26/98	Fix so volume name as used in lock trees is obtained from the
$!		lock device name field, not the present volume name; this
$!		prevents problems after the volume name is changed on-the-fly.
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$	file = f$search(p1)
$	if file .eqs. ""
$	then
$		write sys$output "File ",p1," not found."
$		FINDMASTER_NODE == ""
$		exit 1+0*f$verify(old_verify)
$	endif
$	write sys$output "File: ",p1," (",file,")"
$! Find out the File ID of the file of interest, so we can determine the
$! resource name of the RMS file lock based on that
$!!	fid = f$file_attributes(p1,"FID") - "(" - ")"	!File ID
$!! F$FILE_ATTRIBUTES fails for files opened for exclusive access
$!! Use DIR/FILE_ID instead
$	close/nolog fileid
$	pid = f$getjpi("","PID")
$	temp_file := findmaster_file_temp_'pid'.temp_file
$	directory/file_id/noheader/notrailer/column=1/width=file=100/output='temp_file' 'p1'
$	open/read fileid 'temp_file'
$	read fileid rec
$	close fileid
$	delete 'temp_file';*
$	fid = f$element(1," ",f$edit(rec,"COMPRESS")) - "(" - ")"
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
$!!	volname = f$getdvi(f$parse(p1,"DEVICE"),"VOLNAM")
$!!	volnam = f$extract(0,12,volname+"            ")	!Pad with blanks
$!! show sym volname	!!!
$! Get device lock name from F$GETDVI(x,"DEVLOCKNAM") instead of "VOLNAM"
$	devlocknam = f$getdvi(f$parse(p1,"DEVICE"),"DEVLOCKNAM")
$!Note: This is returned in reverse order (little-endian representation), thus:
$!SCOTTY $ write sys$output f$getdvi("dpa121:","devlocknam")
$!00000020202020202020202052435302
$! show sym devlocknam	!!!
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
$	resnam[8*8,8] = rvolnum
$	resnam[9*8,8] = filx
$!! We now get device lock name from F$GETDVI(x,"DEVLOCKNAM") instead of "VOLNAM"
$!!	resnam[10*8,8] = %x02
$!!	resnam = resnam + volnam
$!!	resnam[23*8,8] = 0
$!!	resnam[24*8,8] = 0
$!!	resnam[25*8,8] = 0
$	i = 1
$ dln_loop:
$	dln_byte = f$extract(f$length(devlocknam)-(i*2),2,devlocknam)
$	resnam[(9+i)*8,8] = %x'dln_byte'
$! write sys$output " i = ",i,", dln_byte = ",dln_byte	!!!
$	i = i + 1
$	if i .le. 16 then goto dln_loop
$! show symbol resnam	!!!
$! Convert the resource name from hex to a string for input to FINDMASTER.EXE
$	i = 0
$	reshex = ""
$ loop:
$	byte = f$cvui(i*8,8,resnam)
$	reshex = reshex + " " + f$fao("!XB",byte)
$	i = i + 1
$	if i .lt. f$length(resnam) then goto loop
$! show sym reshex	!!!
$!
$	FINDMASTER_RESOURCE = resnam
$	FINDMASTER_ACMODE = "EXEC"	!RMS locks are in EXEC mode
$! show sym FINDMASTER_RESOURCE	!!!
$! show sym FINDMASTER_ACMODE	!!!
$	proc = f$environment("procedure")
$	proc_dir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$	run 'proc_dir'findmaster
$	exit 1+0*f$verify(old_verify)
