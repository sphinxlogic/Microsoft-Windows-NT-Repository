VNCVIEWER, NETWORKING, A VNC viewer client for OpenVMS

A new version of the VNC Viewer for OpenVMS VAX and Alpha is now
available. The VNCviewer lets you connect to a VNC server running on
another system.

VNC stands for Virtual Network Computing.  It is, in essence, a remote
display system which allows you to view a computing "desktop"
environment not only on the machine where it is running, but from
anywhere on the Internet and from a wide variety of machine
architectures.

In particular, I use VNC on my AlphaStation to control my
PCs---essentially, I get a PC in an X window, with my VMS keyboard and
mouse movements used to control the PC.  What you see in the PC window
is an exact replica of your PC screen.

You can find out more about VNC, and download viewers and servers for
other platforms (in particular, to control a PC from VMS, you'll need
to download the VNC Server for Windows), check out the original AT&T
VNC site:

http://www.uk.research.att.com/vnc/index.html

The VNC333R1VMS011 package contains V1.1 of the VMS VNC Viewer, which
is based on VNC V3.3.3r1.  The port to VMS was performed by Hunter
Goatley and David North.  It comes as a VMSINSTAL kit, and provides
VNCVIEWER and VNCPASSWD images for both VAX and Alpha.  Note that a
VNC server for VMS is not included, though I hope to look into the
possibility of adding it sometime.

You can get the VMS VNC Viewer using one of the following URLs:

ftp://ftp.wku.edu/vms/fileserv/vnc333r1vms011.zip
ftp://ftp.process.com/wku/vms/fileserv/vnc333r1vms011.zip
ftp://ftp.tmk.com/ftp-wku-edu/vms/fileserv/vnc333r1vms011.zip

http://www2.wku.edu/vms/fileserv
http://www.tmk.com/ftp/

Hunter Goatley, Process Software, http://www.process.com/
<goathunter@goatley.com>   http://www.goatley.com/hunter/
