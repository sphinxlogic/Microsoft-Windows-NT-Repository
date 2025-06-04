HyperSPI Read-me (OSU release)
----------------

This application generates VMS System Performance Information HTML documents. 
Coupled with HYPERSPI$AGENT.C, the data-collection application, it can
profile, complete with graphics, fundamental system performance indicators as 
CPU usage, memory usage, IO values.  It works reasonably well.

HyperSPI comprises two distinct applications.


Data Collection Agent
---------------------

HYPERSPI$AGENT.C

This is the System Performance Information collection agent for the Hypertext 
SPI facility.  It executes on selected nodes, writing selected system 
performance information into a data file, once every minute.  Data collection
has negligable impact on system performance.  The data contains per-minute
average and total values (depending on the datum), and peak values based on
the granularity of whatever is the collection period (currently 2 seconds). 
These data files are kept on a per-day basis.  This data may then be processed
and displayed by the Hypertext SPI facility.  It utilizes the undocumented
EXE$GETSPI interface.  Interface details plagiarized (and extensively
reworked) from a VMS X Window System utility named SPI. 

The collection agent be must started up on all nodes for which information is
required.  The logical name HYPERSPI$DATA locates where the data is recorded
and read from.  It is important this data is cleaned-up occasionally.  It uses
approximately 140 blocks (70Kbytes) per day per system. There is an example
HYPERSPI$AGENT.COM for the data collection agent.  To start the data collection
agent up during system startup add

   $ DEFINE /NOLOG /SYSTEM /EXEC HYPERSPI$DATA device:[directory]
   $ @WWW_ROOT:[SCRIPT_CODE.WASD]HYPERSPI$AGENT STARTUP

at an appropriate place in the system or HTTP server startup procedures.

HyperSPI seems to be fairly sensitive to the EXE$GETSPI() system call (probably
not unreasonably) and may need at-the-least relinking depending on the exact
VMS version. It was been developed and tested on 6.n systems.  It also works
under 7.1.


Data Presentation Application
-----------------------------

HYPERSPI.C

This application is executed as a CGI script.  It operates in two distinct
modes, text and graphic.  In text mode it returns an HTML stream to the
browser comprising a hypertext page, with selection menu, node performance
presentation page, data listing or dump page.  In graphic mode it returns a
GIF image to the browser, first processing the specified data into an
in-memory bitmap graph, then sending this image to the client via an internal
GIF processor. 

A mapping rule should be included in the mapping rule configuration file:

   script /hyperspi* /htbin/hyperspi*

Quick links to selected reports can be useful:

  Graphical <A HREF="/HyperSpi">performance information</A> for selected VMS
  compute servers. 
  <BR>Since 7am today:
  <A HREF="/HyperSpi?list_node=SYSTEM&period=until_now&cpu=yes&memory=yes&hard_faults=yes&do=page">SYSTEM</A>
  <BR>Since yesterday:
  <A HREF="/HyperSpi?list_node=SYSTEM&period=since_yesterday&cpu=yes&memory=yes&hard_faults=yes&do=page">SYSTEM</A>
  <BR>The last week:
  <A HREF="/HyperSpi?list_node=SYSTEM&period=7&cpu=yes&memory=yes&hard_faults=yes&do=page">SYSTEM</A>


MARK DANIEL
-----------
20th August 1998
mailto:Mark.Daniel@dsto.defence.gov.au

ftp://www.vsm.com.au/wasd/index.html
http://www.vsm.com.au:8000/wasd/index.html
http://www.vsm.com.au:8000/
