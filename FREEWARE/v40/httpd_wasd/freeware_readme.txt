WASD_Web_Package, UTILITIES, VMS HTTP Server and CGI applications

The "WASD VMS Hypertext Services" package is a collection of World Wide Web
oriented software for supporting hypertext within a VMS environment.

Provides a multi-threaded HTTP server, under VMS V6.n, 7.1 (and probably 7.2)
Alpha and VAX platforms, and DEC TCP/IP Services and MadGoat NETLIB IP
networking support. It features integrated pre-processed HTML (Server Side
Includes) and image-mapping, and a versatile VMS directory listing capability. 
Includes CGI applications providing extensive access to VMS oriented
information, including HELP and TEXT libraries, and Bookreader documentation.

Access the HTML documentation using a browser on the local system.  First
ensure the freeware CD is mounted /SYSTEM, then enter

        file:///cd-device/httpd_wasd/index.html

into the "Location:" field of the browser.

Alternatively, print a copy of the Technical Overview.

        $ PRINT /QUEUE=postscript-queue cd-device:[HTTPD_WASD.DOC]HTD.PS

This is a large document, so if you have a printer providing two pages per
sheet and/or double-sided it is recommended to submit the job with the
appropriate parameters!

Mark.Daniel@dsto.defence.gov.au
5th September 1998
