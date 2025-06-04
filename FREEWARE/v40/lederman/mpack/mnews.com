$! MNEWS.COM
$! V1.0  18-OCT-1995, David Mathog, Biology Division, Caltech
$!
$! This procedure posts one or more MPACK produced MIME files to a newserver
$! who's internet address is NEWS.
$!
$! It does this by talking directly to port 119 on the server.  It ignores
$! all status information that comes back.
$!
$! P1   File(s) to be mailed - they WILL be deleted!!!!
$! P2   internet name of current machine, otherwise looks for logical MULTINET_HOST_NAME
$!
$! Subject should be inside the MIME messages produced by MPACK.
$! Group(s) will also be inside the MIME messages produced by MPACK
$!
$! Based on MMAIL.COM and RFC850.
$!
$ now = f$time()
$ username = f$getjpi("","USERNAME")
$ username = f$edit(username,"COLLAPSE")
$ tempname = "kill_" + username + "_" -
              + f$cvtime(now,,"MONTH") -
              + f$cvtime(now,,"DAY") -
              + f$cvtime(now,,"HOUR") -
              + f$cvtime(now,,"MINUTE") -
              + f$cvtime(now,,"HUNDREDTH")
$!
$!
$ if("''P1'" .eqs. "")
$ then
$ type sys$input

Here are examples of valid comma delimited file lists, telling MMAIL which
files to send out (which also deletes them):

outfile.txt

  or

outfile*.txt

  or

outfile1.txt, outfile2.txt, outfile3.txt, others.*


****************************************************************************
* IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT:             *
*                                                                          *
*  MMAIL deletes any file that it mails!!!!                                *
*  If this isn't what you want, hit ^Y now and make copies to mail.        *
*                                                                          *
* IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT:             *
****************************************************************************

$   inquire filelist "Please enter a comma delimited list of files to mail"
$ else
$   filelist = P1
$ endif
$!
$ if("''P2'" .eqs. "")
$ then
$   thisnode   =   f$trnlnm("MULTINET_HOST_NAME")
$ else
$   thisnode = P2
$ endif
$!
$!  Now start actually doing things
$!
$ open/write tfil: sys$scratch:'tempname'_header.txt
$!
$!
$!
$ write tfil: "$ telnet/port=119/create_nty NEWS"
$ write tfil: "$ copy sys$input 'f$trnlnm(""telnet_nty"")"
$ write tfil: "POST"
$ write tfil: "From: ''username'@''thisnode'"
$!
$ close tfil:
$!
$! Now, send all of the files
$!
$ ALLCOUNT = 0
$ COUNT = -1
$ lastfile="+++"
$top_files:
$  count = count + 1
$  file=f$element(count,",",filelist)
$  if(file .nes. ",")
$  then
$    file = f$edit(file,"COLLAPSE")
$    sent = 0
$!
$!
$!
$top_out:
$    ofile=f$search(file)
$    if(ofile .eqs. lastfile)then goto top_files
$    lastfile = ofile
$    if(ofile .eqs. "")
$    then
$      if(sent .eq. 0)
$      then
$        write sys$error "Fatal error: ''file' does not exist"
$        delete sys$scratch:'tempname'_header.txt.*
$        exit
$      else
$        goto top_files
$      endif
$    else
$      allcount = allcount + 1
$      fileo := "sys$scratch:''tempname'_''allcount'.dat"
$      fileh := "sys$scratch:''tempname'_header.txt"
$      filet := "sys$scratch:''tempname'_temp.dat"
$      convert/fdl=sys$input 'fileh' 'filet'
RECORD
        BLOCK_SPAN              yes
        CARRIAGE_CONTROL        carriage_return
        FORMAT                  stream_lf
        SIZE                    32767
$      append/new 'filet','ofile' 'fileo'
$      delete 'filet';
$      delete 'ofile'
$      open/append gfil: 'fileo'
$      write gfil: "."
$      write gfil: "QUIT"
$      write gfil: "$! end of message"
$      write gfil: "$ dealloc 'f$trnlnm(""telnet_nty"")"
$      write gfil: "$ wait 00:00:01"
$      close gfil:
$      @'fileo'
$      delete 'fileo';
$      sent = sent + 1
$      goto top_out
$    endif
$!
$!
$!
$    goto top_files
$  endif
$!
$ write sys$output -
"''Allcount' files have been posted "
$ delete sys$scratch:'tempname'_header.txt.*
$ exit
$!
