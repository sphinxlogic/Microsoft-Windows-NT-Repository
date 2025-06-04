ARCHIE, UTILITIES, Utility to search files available at FTP sites

[Last changed 04/14/92 v1.3.2]

 Enclosed you'll find a Prospero client for the archie service.  It'll
let you query the archie databases without actually using an
interactive process on the remote server's machine (e.g., archie.ans.net),
resulting in a MUCH better response time.  It also helps lessen the
load on the archie server itself.

 What's Archie?  It's a system that will let you check a database containing
thousands of entries for the files that're available at FTP sites around
the world.

 This is a third child of Clifford Neuman's Prospero project.  It's really
the Archie client that's included in the prospero stuff, but I've taken out
everything that's unnecessary for this client to work.  (Aka, you don't
have to build all of Prospero to get the Archie client.)  Khun Yee Fung
wrote an archie client in Perl, George Ferguson has written a client
for use with XWindows, based in part upon this code.  Also, Scott
Stark wrote a NeXT-Step client for the NeXT.

 Using the Archie Prospero interface in its true form will probably be of
interest---check out the file `Prospero' for an example of its interface.
If you find it intriguing, you should probably get the full prospero kit
from the University of Washington on cs.washington.edu in pub/prospero.tar.Z.

 Suffice to say, there are now a number of ways to query Archie without
bogging a server down with your logins.

 Check out the man page (or archie.doc, if you're using VMS or DOS)
for instructions on how to use this archie client.  VMS users please
note that you have to put quotes around args that are capital letters;
for example,  $ ARCHIE "-L"  to list the available servers.

 Please check to make sure you don't have "archie" aliased or modified
in some way to do a telnet or rlogin (which you may've done before
this command-line ability came into being).

 If Archie consistently hangs (at different times of day with
different queries), it's possible that your site has UDP traffic on
ports > 1000 blocked, for security reasons.  Type `make udptest' to
check---if it prints out the date, the Archie server is probably down;
if it doesn't print out the date, either Widener's computer is down
(God forbid ;-) ) or you do indeed have UDP blocked.  See your system
administrator in this case.  If the problem persists and haven't the
vaguest, then write me describing the situation and what machine/OS
you're using.

 Write to archie-group@cs.mcgill.ca with questions about Archie itself.
 Write to info-prospero@isi.edu about the Prospero protocol.
 Write to brendan@cs.widener.edu with questions about this specific package.
