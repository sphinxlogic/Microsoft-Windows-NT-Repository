$! MMAIL.COM
$! V1.0  16-OCT-1995, David Mathog, Biology Division, Caltech
$!
$! This procedure mails one or more MPACK produced MIME files to one or 
$! more internet users.
$!
$! The method MMAIL uses is to make a direct entry onto the MULTINET_SMTP queue.
$! Obviously, this is MULTINET specific, but something similar should work
$! for other mailers.  A second method is provided and has been tested
$! on a Multinet system - it will likely need to be tweaked for
$! other TCP/IP vendor's systems.  This uses telnet to port 25 on localhost
$! and standard SMTP protocols (which is a bit of a pain).  
$! Before MMAIL will work in the Multinet mode somebody with privs
$! must modify the Multinet outgoing mail queue to accept submissions from W.
$!
$! P1   File(s) to be mailed - they WILL be deleted!!!!
$! P2   Address(es) to mail to.
$! P3   If defined, use second mode (via port 25) instead of MULTINET mode.
$! Subject should be inside the MIME messages produced by MPACK.
$! Message-id should be inside the MIME messages produced by MPACK.
$!
$! Thanks to Aaron Leonard for suggesting this method of bypassing
$! MAIL's unbypassable blank line between header and message body.
$!
$! Here are most of the Multinet specific pieces, there is also one
$! Multinet specific line below.
$!
$ if(f$trnlnm("MULTINET_ROOT") .nes. "" .and. "''P3'" .eqs. "")
$ then
$   whichqueue =   f$trnlnm("MULTINET_SMTP_QUEUE")
$   sendit     :== submit/que='whichqueue'/delete &fileo
$   thisnode   =   f$trnlnm("MULTINET_HOST_NAME")
$   method     =   "MULTINET"
$ else
$   write sys$Output "WARNING, this is a demonstration mode only!"
$   sendit     :== @"'fileo'"
$   inquire thisnode "enter your internet node name"
$   method     =   "DEMO"
$ endif
$!
$! pieces common to both methods
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
$   type sys$Input

This procedure sends mail to a comma delimited list of internet addresses.  
For instance: 

"fred@what.where.com"

     or

"fred@what.where.com, ginger@which.what.edu, astaire@dance.org"

****************************************************************************
* IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT:             *
*                                                                          *
*  Enclose everything in double quotes to maintain the case                *
*  that you type, otherwise, it will be converted to upper case.  This     *
*  will usually break mail delivery to Unix systems, where the user's name *
*  is fred, not FRED!                                                      *
*                                                                          *
* IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT:             *
****************************************************************************
$   inquire addresslist "Please enter the addresslist now"
$ else
$   addresslist = P2
$ endif
$!
$!  Now start actually doing things
$!
$ open/write tfil: sys$scratch:'tempname'_header.txt
$!
$!
$!
$ if(method .eqs. "DEMO")
$ then
$   write tfil: "$ telnet/port=25/create_nty localhost"
$   write tfil: "$ copy sys$input 'f$trnlnm(""telnet_nty"")"
$   write tfil: "HELO"
$ endif
$!
$! This next section prepares a header file that will go on the front of
$! each outgoing message.
$!
$ write tfil: "MAIL FROM:<''username'>"
$!
$! one or more valid addresses
$!
$count=0
$top_address:
$  address=f$element(count,",",addresslist)
$  if(address .nes. ",")
$  then
$    write tfil: "RCPT TO:<''address'>"
$    count = count + 1
$    goto top_address
$  endif
$  address_count = count
$!
$ if(method .eqs. "MULTINET")
$ then
$   write tfil: "ARRIVAL_TIME: ''now'"
$ endif
$ if(method .eqs. "DEMO")
$ then
$   write tfil: "DATA"
$ endif
$!
$! The next line is probably not quite correct, but hopefully won't
$! jam any mail systems.
$!
$ write tfil: "Date: ''now' ,sender's local time"
$!
$! These next two are self evident
$!
$ write tfil: "From: ''username'@''thisnode'"
$ write tfil: "To: ''addresslist'"
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
$        write sys$output "Fatal error: ''file' does not exist"
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
$      if (method .eqs. "MULTINET")then sendit
$      if (method .eqs. "DEMO")
$      then
$          open/append gfil: 'fileo'
$          write gfil: "."
$          write gfil: "QUIT"
$          write gfil: "$! end of message"
$          write gfil: "$ dealloc 'f$trnlnm(""telnet_nty"")"
$          write gfil: "$ wait 00:00:01"
$          close gfil:
$          @'fileo'
$          delete 'fileo';
$      endif
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
"''Allcount' files have been mailed to ''address_count' addresses"
$ delete sys$scratch:'tempname'_header.txt.*
$ exit
$!
