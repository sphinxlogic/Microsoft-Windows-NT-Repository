GNUDIFFUTILS, UTILITIES, Unix-style file comparison utilities

1-Feb-2001	Craig A. Berry	     <craigberry@mac.com>
		Charles Lane         <lane@DUPHY4.Physics.Drexel.Edu>
		Martin Vorlaender    <mv@PDV-SYSTEME.de>

This is a port of GNU diffutils 2.7.2 for OpenVMS.  Some functionality is
not available and there are no guarantees of any kind about the performance
of the software.  Only cursory testing has been performed, but it seems to
work fine in a number of fairly complicated cases.  This work is based on
earlier work by Richard Levitte, but far fewer heroics are required now
than five years ago to work around deficiencies in the C compiler and
run-time library.  

The port is based on the 2.7.2 GNU sources, which are still in beta as of this
writing.  The beta sources were used because they obviate some VMS-specific
changes (such as a wrapper for read()) that would have been necessary with the
final-release 2.7 sources, and also because the GNU maintainer (Paul Eggert
<eggert@twinsun.com>) requested that patches required for VMS be submitted
against 2.7.2.  We submitted such a patch on 22-Feb-2000 and hope the next
GNU-released version will work out of the box on VMS.

The VMS additions may be distributed under the same terms as the GNU
General Public License.  We'd be happy to hear about things that need
fixing but can't promise to do anything about it.

INSTALLATION

Object libraries for both OpenVMS VAX and OpenVMS Alpha are included with
this distribution and were created under OpenVMS 6.2, so they should link
properly under that and later versions of OpenVMS.  To link, run the command
procedure LINK.COM in the main directory as follows:

    $ @LINK

Copy the executable files to a suitable location, set the protection on
them so whoever needs to can run them, and create symbols that can be
used as foreign commands.  The commands to do this look something
like:

    $ copy gdiff.alpha_exe dra0:[gnu]gdiff.exe
    $ set protection=(s:rwed,o:rwed,g:re,w:re) dra0:[gnu]gdiff.exe
    $ gdiff :== $dra0:[gnu]gdiff.exe  ! put this in LOGIN.COM or SYLOGIN.COM

You can do something similar for the subsidiary programs GCMP.EXE,
GDIFF3.EXE, and GSDIFF.EXE.  Be aware that GDIFF3 and GSDIFF spawn a
process to run GDIFF and must be in the same directory it is.  Also
note: you probably shouldn't use "diff" as a foreign command symbol
because that will conflict with the native DIFFERENCES command.

BUILDING FROM SOURCE

If you have a C compiler, the included descrip.mms in [.SRC.VMS] can be made
with MMS or the freeware MMK. It's doubtful the package will compile with DEC
C before 5.2 or VAX C, nor is it likely to run on OpenVMS versions prior to
6.2.  

RUNNING

For a quick start, you can run the programs using the --help option to get a
brief usage summary.  For more detailed documentation, see the file
[.SRC]DIFF.INFO, or the same information converted to OpenVMS help format in
[.SRC.VMS]GNUDIFFUTILS.HLP.  This latter file is suitable for insertion into
an OpenVMS help library, although the logical structure of the original
document is not ideally suited to that environment.  The help file was
created with a Perl script written by Martin Vorlaender and available at
<http://www.pdv-systeme.de/users/martinv/info2hlp>.

VMS-SPECIFIC USAGE NOTES

 o I/O redirection is built into the program. The redirection
   symbols recognized are

     <    redirect SYS$INPUT
     >    redirect SYS$OUTPUT
     >>   redirect SYS$OUTPUT, append mode
     2>   redirect SYS$ERROR
     2>>  redirect SYS$ERROR, append mode

   When redirecting SYS$ERROR, the special filename '&1' is recognized; 
   it redirects SYS$ERROR to SYS$OUTPUT.

 o The programs, where possible, exit with the same success statuses that
   the OpenVMS DIFFERENCES utility uses. 
