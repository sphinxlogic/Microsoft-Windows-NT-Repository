You should see in this directory the following subdirectories:

Directory	Contents
[.PIPE]		Holds files for VMS mail mailpipe add-on. Needed by Majordomo.
[.SMTP]		Holds new UCX SMTP images with support for new feature required
		by Majordomo. You might not need to install this. See
		[.SMTP]README.TXT.
[.SFF]		Holds image providing back door into UCX SMTP's Send From File
		mechanism. Only needed because direct 'C' calls from the
		Majordomo Perl scripts have not yet been implemented.
[.MAJORDOMO]	Holds Majordomo scripts, doc's, etc.

In each of these directories are files that you need to install on your system.
The ones not in the [.MAJORDOMO] directory tree are needed by Majordomo but not
part of Majordomo. You need to read all the readme files in the directories
above and follow the instructions in each one.


Perl information

Majordomo requires that Perl be installed. Perl is a scripting language
originally created for Unix but which has also been ported to VMS by the
University of Pennsylvania. This kit does not include Perl. You can get Perl for
VMS as freeware in one of two ways.

   1) Digital's OpenVMS Internet Product Suite (IPS) contains a copy of UPenn's
   VMS Perl port. It contains pre-built Perl images so you don't have to build
   Perl yourself. The disadvantage is that the  Perl included in IPS V1.0 does
   not include support for building your own Perl 'C' extensions so if you have
   plans to do something with Perl other than run Majordomo and those plans are
   likely to include Perl scripts calling 'C' code then better to go for option
   2.

   2) You can also pick up Perl directly from UPenn's FTP site. It's at
   ftp://genetics.upenn.edu/perl5/perl5_vvv_yymmddx.zip, where vvv is the
   version and yymmddx is the date the kit was put up for ftp.  It's also
   available as perl5_vmstest.zip if you'd like a constant name for periodic
   fetches.

Up to this point VMS Majordomo has been running on our development cluster in
the UCX group in Digital on the following platforms:

   AXP, VMS V6.1, Perl V 5.002_01    (from IPS V1.0)
   AXP, VMS V6.2, Perl V 5.002_03 (from UPenn's FTP site)
   VAX, VMS V5.5-2, Perl V 5.002_03 (from UPenn's FTP site)

Known differences between VMS Majordomo and the Unix original are documented in
in [.MAJORDOMO]VMS_README.TXT.

Overview of changes for FT6
|-  [.PIPE]
|   The MAILPIPE code now requires a PIPE Security Registry File (named
|   SYS$SYSTEM:PIPE_MAILSHR_REGISTRY.DAT). This file tells MAILPIPE which
|   usernames can PIPE to which .COM files running under which usernames.
|-  [.SMTP]
|   No changes.
|-  [.SFF]
|   No changes
|-  [.MAJORDOMO]
|   You must register the UCX Majordomo .COM files in the new PIPE Security
|   Registry file. This will be done for you automatically when you
|   @[.MAJORDOMO]INSTALL.COM. There is also a separate file to do it called
|   INSTALL_PIPE_REGISTRY.COM
