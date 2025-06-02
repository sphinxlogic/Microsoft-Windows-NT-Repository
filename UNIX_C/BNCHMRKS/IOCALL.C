 3-Dec-85 04:27:29-MST,3288;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Tue 3 Dec 85 04:27:25-MST
Received: from usenet by TGR.BRL.ARPA id a014973; 3 Dec 85 5:42 EST
From: Jan Stubbs <stubbs@ncr-sd.uucp>
Newsgroups: net.sources
Subject: IOCALL, a Unix sytem performance benchmark
Message-ID: <346@ncr-sd.UUCP>
Date: 2 Dec 85 19:59:24 GMT
Keywords: benchmark
To:       unix-sources@BRL-TGR.ARPA

"IOCALL, A UNIX SYSTEM PERFORMANCE BENCHMARK"

We have found the dhrystone benchmark to be very useful in predicting
the performance of systems software written in 'C'.  Mr. Richardson
points out several deficiencies in it however, which IOCALL, the following easy
to run benchmark corrects:

> 1. Dhrystone does not do IO. Typical programs do but we'd open up a whole can > of worms. 

We aren't going to open this can completely, IOCALL does invoke UNIX system     calls to open, read write and close a file, but only the cpu cycles to do so aremeasured. Thus we avoid measuring the physical characteristics of the actual 
disk device used on the system. Also, since we are really careful not to cross 
the buffer boundary, UNIX won't do physical IO, it merely moves bytes to and 
from the buffer cache, a function that is done much more often than physical IO,and therefore more important.

Also, many of the so-called IO throughput tests I have seen for UNIX
are very sensitive to the size of the buffer cache on the system being
measured. In many cases a sequence of writes followed by reads will do
much physical IO on one system, but very little on another depending on
the file size compared to buffer cache size which is an installation
option for most unixes. IOCALL only uses one buffer (over and over) so
it does not have this problem.

> 2. Dhrystone does not measure OS performance as it avoids calling the OS.

IOCALL measures OS performance, in fact that is nearly all it measures. It also concentrates on system call interface efficiency, and especially read()
system call performance, the most used system call on most systems. IOCALL
is intended for UNIX systems only. 

To create it, save in iocall.c, cut off the top, cc -o iocall.
The way to time it is simply type:
time iocall

The time to report is system time (normally output with accuracy to
10ths of a second), not real or user time. This introduces a new
deficiency of course, namely trusting the OS to correctly report the
system time.

VAX 11/780 times on BSD 4.2 are around 6 system seconds.

Please send results, comments, complaints, etc. directly to me and I will post  them to the net.
                         
"The opinions expressed herein are those of the author".
 
Jan Stubbs
NCR Corp.
San Diego, CA
619 485-3052
..sdcsvax!ncr-sd!stubbs

------cut here------cut here-------------------------------
/*This benchmark tests speed of Unix system call interface
  and speed of cpu doing common Unix io system calls. */

char buf[512];
int fd,count,i,j;

main()
{
 fd = creat("/tmp/testfile",0777);
 close(fd);
  fd = open("/tmp/testfile",2);
  unlink("/tmp/testfile");
for (i=0;i<=1000;i++) {
  count = write(fd,buf,500);
  lseek(fd,0,0);

  for (j=0;j<=3;j++) 
  	count = read(fd,buf,100);
  }
}
