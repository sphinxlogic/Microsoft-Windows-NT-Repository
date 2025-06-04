W X W E B
*********

Features...

- SSL enabled.

- Free, portable (WIN32 and Unix XView/Motif).

- Includes FRAMES, TABLES, FORMS and everything I can find time to add.

- Supports GIF, PNG & JPEG (MSW only), also WAV and AVI files under MSW.

- Proxies, (persistent) caching, persistent connections (from HTTP/1.1)

- FTP via proxy servers.

- Basic authentication scheme for userid & passwords.

- Site-wide link verification.

- Grab mode for downloading whole or partial sites.

- Site generator tool for adding generic content, style, headers, footers,
  navigation bars, multi-style views etc.

- SMPTP and POP3 client code.

- Available as a class library for inclusion in your own applications
  or as a stand-alone web browser.


The wxHtml class
----------------

This class provides, in it's most basic form, a rich-text viewer and
hypertext engine with HTML as it's markup language. However the
required functionality has been added to perform URL lookup & caching.
This class is derived from wxPanel (except under XView where it derives
from wxCanvas) and implements scrolling (which wxPanel does not).

This has now been split into wxHtml (which does basic parsing & hypertext)
and wxWWW which adds URL fetching and caching (i.e. all the HTTP stuff).


The wxWeb client program
------------------------

This is a simple framework for interactively controlling a wxHtml
object from a menu. As such it constitutes a simple Web browser. A
much more interesting alternative is to embed a wxHtml object in your
own application and thus make it Web aware.


The wxHttpd server program
--------------------------

As an aid to development I needed a simple Web server so that I could
see what was going on. It handles multiple simultanenous connections,
supports HTTP/1.0 and extensions such as keep-alives (which most freeware
and many commercial servers don't, and greatly increases efficiency).
It DOES NOT support standard CGI and will never do so... get a real
web server!


To build
--------

1. On a command-line (DOS or Unix) enter...

	unzip wxwebXXX.zip		# DOS
or
	unzip -La wxwebXXX.zip		# Unix


Note: don't use PKZIP, use the better (and free) ZIP package.

2. Change to the 'client' sub-directory. Use the supplied makefile and
build...

	nmake FINAL=1 -f makefile.nt	# WIN95 or NT with MSVC++
or
	nmake FINAL=1 -f makefile.b32	# WIN95 or NT with BC++
or
	make -f makefile.unx motif	# Unix with Motif
or
	make -f makefile.unx xview	# Unix with Xview

then run the wxweb executable. For example, if using the
wxhttpd server then simply enter...

      wxweb [URL]

...and point and click. You may need to go into Options|Configure
in order to setup proxies if behind a firewall or your ISP uses
them.

3. Change to the 'server' sub-directory. Use the supplied makefile
and build as in (2) above. Then run the wxhttpd executable (will need
to be SU under Unix if using the default port number of 80).
THIS IS OPTIONAL.

Note
----

You will of course need a copy of wxWindows (1.63 or later, or 1.65 at
least for Windows 95 or NT) in order to compile the source. See...

      http://www.aiai.ed.ac.uk/~jacs/wxwin.html

for details.


Writing WX-CGI programs
-----------------------

WX-CGI was an attempt at a FAST-CGI style protocol (before I ever heard
of FAST-CGI of course), but I don't think it is currently in working
order.

In the server/cgi directory is a sample program called 'SURVEY' that
responds to a simple query form (see server/document/survey.html).
This program is meant to illustrate and document the technique of
writing a WX-CGI program and serves as a template for writing others.
It doesn't do anything with the data it is sent, but it would be nice
to oneday  use the wxODBC classes perhaps to build a simple database
and return some statistics.

When a WX-CGI program is executed the port number will passed as the
first command-line argument, and the keepalive-timeout value as the
second. Request data will be passed in via a socket and the response
data will be returned via the same socket in standard HTTP/1.0 format.


8) Disclaimer
-------------

Not everything works. Notably transparent GIFs for X-Windows, JPEGs
(never sorted out the compilation issues), and of course real CGI.





Andrew Davison
http://www.ozemail.com.au/~adavison/wxweb.html
mailto:adavison@s054.aone.net.au



