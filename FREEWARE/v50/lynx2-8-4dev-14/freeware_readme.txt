LYNX284, UTILITIES, Text Web Browser
This is Lynx version 2.8.4dev.14, snapshot November 3, 2000.

WHAT IS LYNX?

   Lynx is a fully-featured World Wide Web (WWW) client for users running
   cursor-addressable, character-cell display devices such as vt100
   terminals, vt100 emulators running on PCs or Macs, or any other
   character-cell display.  It will display Hypertext Markup Language
   (HTML) documents containing links to files on the local system, as well
   as files on remote systems running http, gopher, ftp, wais, nntp,
   finger, or cso/ph/qi servers, and services accessible via logins to
   telnet, tn3270 or rlogin accounts.  Current versions of Lynx run on
   Unix, VMS, Windows95/NT, 386DOS and OS/2 EMX.

   Lynx can be used to access information on the WWW, or to establish
   information systems intended primarily for local access.  Lynx has been
   used to build several Campus Wide Information Systems (CWIS).  Lynx can
   also be used to build systems isolated within a single LAN.

   Links to the current sources and support materials for Lynx are
   maintained at:
			  "Lynx links"
	     <URL:http://www.crl.com/~subir/lynx.html>
   and at the Lynx homepage:
			"Lynx Information"
		  <URL:http://lynx.browser.org/>.
   View these pages for information about Lynx, including new updates.

   Lynx is distributed under the GNU General Public License (GPL) without
   restrictions on usage or redistribution.  The Lynx copyright statement,
   "COPYHEADER", and GNU GPL, "COPYING", are included in the top-level
   directory of the distribution.  Lynx is supported by the Lynx user
   community, an entirely volunteer (and unofficial) organization.

   Certain portions of the Lynx source distribution were originally created
   by CERN and have been modified during the development of Lynx.  See
   WWW/Copyright.txt for copyright info regarding CERN products used in
   Lynx.

YEAR 2000 COMPLIANCE

   We believe Lynx works properly for the Year 2000 issues, since it does
   not store dates in 2-digit form.  Since it must communicate with a wide
   range of web servers, it interprets dates in a variety of formats.  In
   particular, if Lynx receives a date with a 2-digit year, it assumes that
   values less than 70 are in the range 2000-2069.

DOCUMENTATION

   A users guide is included in this distribution along with a man page for
   Unix systems and a help file for VMS systems.  All documentation is
   contained in the top directory and the docs, samples and lynx_help
   subdirectories.

   While running Lynx, type 'h', 'H', or '?' to invoke the help menu
   system.  From the help menu you may access several useful documents
   pertaining to Lynx and the World Wide Web.  The most important of these
   is the Lynx Users Guide.  By default, Lynx will use the Lynx Enhanced
   Pages, which includes http links for help and FAQs concerning Lynx.  It
   is recommended that you install your own help menu system at your site
   in order to lessen the load on http servers.  This also will allow you
   to customize the help menu system for your site and greatly speed up
   access for those using Lynx over a slow connection.

   For Unix and related systems which support the autoconf configure
   script, the help menu system is installed by the "make install-help"
   command.

   For other systems (such as VMS), copy COPYHEADER and COPYING into the
   lynx_help/ subdirectory.  Then copy the lynx_help subdirectory to a
   public place on your system, or into your $HOME directory if you are a
   single user.  Finally, edit the lynx.cfg file so that the HELPFILE line
   is defined as follows:

      HELPFILE:file://localhost/[public_path]/lynx_help/lynx_help_main.html

   where [public_path] is the absolute path to the lynx_help directory.
   Customizing the help menu system is just a matter of editing a set of
   HTML files.  Additional information about installing and customizing
   the help file set is available at <URL:http://www.irm.nara.kindai.ac.jp/
   lynxdev/README.help>.

INSTALLING LYNX

   To install Lynx, follow the steps in the INSTALLATION file, which is
   located in the top directory of the source distribution.

PROBLEMS

   If you experience problems configuring, compiling or installing Lynx,
   please read Section VI.  "General installation instructions" in the
   INSTALLATION file.  Instructions are given there for reporting your
   problem to the "lynx-dev" mailing list, which is frequented by
   experienced Lynx users.

LYNX-DEV MAILING LIST

   To subscribe to lynx-dev, send email to majordomo@sig.net with only the
   following message in the body:

      SUBSCRIBE LYNX-DEV address

   where inclusion of your email address is optional if it can be obtained,
   correctly, from the mail headers of your subscription request.

   If you wish to unsubscribe from lynx-dev, send email to
   majordomo@sig.net with only the following message in the body:

      UNSUBSCRIBE LYNX-DEV address

   where inclusion of your email address is optional if it can be obtained,
   correctly, from the mail headers of your request.

   Also, to get a list of useful majordomo commands, send email to
   majordomo@sig.net with the following command:
   ^^^^^^^^^
      HELP

   PLEASE!!! do not send commands to the lynx-dev mailing list itself.

   Any messages you wish to post should be sent to lynx-dev@sig.net. 
   PLEASE use the lynx-dev list, NOT private email to the developers,
   for questions or discussion about Lynx, or contributions of patches. 
   Patches should use the context diff format (diff -c).

   You need not be subscribed to the lynx-dev list in order to post.  If
   you post without subscribing, though, you should read replies to your
   questions or comments in the archive since more often than not nobody
   will send a carbon copy to you.  View the archives at:

		    "lynx-dev Mailing list archives"
		<URL:http://www.flora.org/lynx-dev/html/>

