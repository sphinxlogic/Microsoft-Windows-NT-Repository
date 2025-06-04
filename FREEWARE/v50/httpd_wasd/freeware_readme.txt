WASD_WEB, WEB_TOOLS, VMS HTTP Server and CGI applications

The "WASD VMS Hypertext Services" package is a collection of World Wide Web
software for supporting hypertext within a VMS environment.

Provides a multi-threaded HTTP server, for VMS V5.5-2, 6.n, 7.n Alpha and VAX
platforms, and the TCP/IP BG: driver (i.e. supported for UCX v3/4.n, Digital
TCP/IP Services v5.n, MultiNet, TCPware, and anything else supporting the BG:
QIO API). It features integrated pre-processed HTML (Server Side Includes) and
image-mapping, and a versatile VMS directory listing capability.  Includes CGI
applications providing extensive access to VMS oriented information, including
HELP and TEXT libraries, and Bookreader documentation.  Supports high
efficiency and throughput, *persistant* scripts, including scripting
environments such as Perl and Java.

Access the HTML documentation using a browser on the local system.  First
ensure the freeware CD is mounted /SYSTEM, then enter

        file:///cd-device/httpd_wasd/index.html

into the "Location:" field of the browser.

Alternatively, print a copy of the Technical Overview.

        $ PRINT /QUEUE=postscript-queue cd-device:[HTTPD_WASD.DOC.HTD]HTD.PS

This is a large document, so if you have a printer providing two pages per
sheet and/or double-sided it is recommended to submit the job with the
appropriate parameters!

Mark.Daniel@wasd.vsm.com.au
(Mark.Daniel@dsto.defence.gov.au)
January 2001
