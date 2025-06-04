
wxshlib1.66-11.tar.gz is a GNU configure script which builds 
static and shared WXwin library in most UNIX platform.

Now it supports Linux-ELF, SGI IRIX5, Solaris 2.x,
SunOs 4.x, DEC OSF1, NetBSD, FreeBSD, HPUX, DGUX,
AIX 3, SYSV4, DEC Ultrix(only static library).


  From version 1.66-5, wxshlib use new files structure. 
  All variables are defined in file "make.tmpl.in" and
  all rules are defined in file "make.rule". All object
  files go into GUI depend subdirectory such as obj_motif
  and obj_ol.
  
 
  Running "configure" will create a file "make.tmpl" which includes 
  all system depend variables. User can modify this variables if it
  is necessary.



<<FTP SITE>>

ftp://ftp.aiai.ed.ac.uk/pub/packages/wxwin/contrib/wxshlib/sh166_11.tgz

http://www.aero.gla.ac.uk/People/xiaokun/wxshlib/wxshlib1.66-11.tar.gz 


<<WHAT'S NEW>>

Update Makefile.in for wxweb.




<<PLATFORM>>

The following platform have been tested(ver 1.66-10):

static library,
---------------
           Platform                                  Compile     Run   
--------------------------------------------------------------------
Linux 1.3.94, gcc 2.7.2, XView   , 486DX-2	       OK        OK
Linux ?     , gcc ?    , Motif2.0, ?     	       OK	 OK
IRIX 5.3    , gcc 2.7.2, Motif1.2, SGI Indy            OK        OK
IRIX 5.3    , CC  4.0  , Motif1.2, SGI Indy            OK        OK
SunOS 4.1.3 , gcc 2.7.2, XView   , Sparc-ELC           OK        OK
SunOS 5.5   , gcc 2.7.2, XView   , Sparc10             OK        OK
SunOS 5.5   , CC 3.0.1 , XView   , Sparc10             OK        OK
SunOS 5.5   , gcc 2.7.2, Motif1.2, Sparc10             OK        OK
SunOS 5.5   , CC 3.0.1 , Motif1.2, Sparc10             OK        OK
OSF  3.2    , gcc 2.7.2, Motif1.2, Dec Alpha	       OK        OK
HPUX 9.05   , CC A.03.65 Motif1.2, HP 9000             OK        OK
Ultrix 4.3  , gcc 2.7.2, Motif?  , Dec mips	       OK*	 Fail


shared library,
---------------
---------------------------------------------------------------------
Linux 1.3.94, gcc 2.7.2, XView   , 486DX-2	       OK         OK*
Linux ?     , gcc ?    , Motif2.0, ?     	       OK 	  OK*
IRIX 5.3    , gcc 2.7.2, Motif1.2, SGI Indy            OK         OK
IRIX 5.3    , CC  4.0  , Motif1.2, SGI Indy            OK         OK
SunOS 5.5   , gcc 2.7.2, XView   , Sparc10             OK         OK
SunOS 5.5   , CC 3.0.1 , XView   , Sparc10             OK         OK
SunOS 5.5   , gcc 2.7.2, Motif1.2, Sparc10             OK         OK
SunOS 5.5   , CC 3.0.1 , Motif1.2, Sparc10             OK         OK
OSF  3.2    , gcc 2.7.2, Motif1.2, Dec Alpha	       OK         OK
HPUX 9.05   , CC A.03.65 Motif1.2, HP 9000             OK         OK


In Linux, all program got core dump if use g++, but everythig are ok
if use c++. The reason maybe is the global var RXwhite be defined 
twice in wxregex.cc and libg++.a
Hermann Himmelbauer <h9025909@asterix.wu-wien.ac.at> tested wxshlib1.66 

In Linux with motif2.0. Thanks ;-) 
he have no troubles (Except the strange Seg-Fault on the Exit when he
compile my App with the g++ instead of the c++)

Harri Pasanen (pa@tekla.fi) tested wxshlib1.65 at Dec alpha. Thanks!
He said that "-I/usr/include" should be omitted because of 
the system headers conflict with the gcc internal headers.
From 1.66-7, configure can take care of this automatically.

In DEC Ultrix mips workstation with gcc2.7.2, I can compile
and create the static WX library. But no any application
program can be link successfully. The linker reports many
symbol undefine ;-(
Milton Hankins <einstein@lite.sabbagh.com> reported he compiled
successfully Ultrix 4.3 with gcc2.7.2. You need change
contrib/Makefile to delete xmgauge and combobox in variables
"libsubdirs" and "demodirs"
 
Jean-Charles Jorel (jjorel@silr.ireste.fr) tested wxshlib1.66-10 at
HP 9000 and send me some patch relative to HPUX. Thanks!


<<INSTALL>>

                            wx166
                              |
           +---------+--------+--------+-------+
          src       lib      doc      bin     obj


0) *Need GNU Make*

1) Install wxshlib1.66-10.tgz
----------------------

Usage:
	cd wx166
	gzip -dc wxshlib1.66-10.tgz | tar xvf -


2) Create your configuration
----------------------------

Usage: 
	./configure options

If you want to use system's C and C++ compiler,
set environment varaibles CC and CCC as

% setenv CC cc
% setenv CCC CC
% ./configure options


 options:
  --prefix=your_install_directory
  --with-shared   	   build shared WXwin library
  --with-opt      	   set optimazation (-O)
  --with-debug    	   set debug (-g3)
  --with-motif=your_dir    use Motif library
  --with-xview=your_dir    use XView library


 default:
  no debug, no optimazition, --prefix=/local, 
  automatic search C and C++ compiler,
  automatic search Motif and XView library,
  and No shared WXwin library. 


3) Build WX library
-------------------

Usage:
	mkdir bin; mkdir doc
	make all
or
	mkdir bin; mkdir doc
	make depend; make all


4) Build extra WX library and utility programs, 
   such as wxbuild, dialoged, wxhelp, tex2rtf
-----------------------------------------------

Usage:
	make prog


5) Build demo programs
----------------------
Usage:
	make demo


6) Build documents
------------------

Usage:
	make ps                     <== for postscript files
	make html                   <== for html files


7) Test demo program
--------------------

For example, if you need test wxbuild and no shared WXwin library,

	bin/wxbuild

If you have built shared WXwin library,

	% setenv LD_LIBRARY_PATH `pwd`/lib    <== for csh
	$ export LD_LIBRARY_PATH `pwd`/lib    <== for sh, bash, ksh
	bin/wxbuild


8) Install WXwin library and utilities programs
--------------------------------------------

Usage:
	make install

All documents locate in subdirectory doc, you have to install them manaully.

