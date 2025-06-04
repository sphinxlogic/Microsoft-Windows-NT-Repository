MINISM, UTILITIES, Remote Xwindows Session Manager Utility

Launch remote X applications and display the output on your VMS
workstation. Executables are provided for Alpha and Vax. These
exe's require VMS V7.1 on the remote system(s).

To use:

Add following line (or similar) to your remote login.com(s):

  $ vmsm :== submit/noprint msmtcp.com /que=sys$batch/param=viking

Note that VIKING is the tcpip/decnet/nodename of local VMS workstation.
Make sure MSMTCP.COM in in the remote sys$login directory. Enter the
appropriate security info for the remote nodes and be sure the tcpip
transport is loaded on your VMS workstation. Also copy MINISM.DAT and
MSMAPPS.INP to wherever DECW$USER_DEFAULTS resides. When you are able
to display MiniSm locally from the remote node, use online help to see
how to use the tool. 

Two files are included to show how to implement your own application
launching command files. DECTERM.COM is used to create a remote decterm.
XV.COM shows a method for creating a launch file for custom apps.

MiniSm was built on VMS 7.1 using DecC 5.6 on VAX & ALPHA. In the past,
previous versions were built with VMS 5.5-2, Motif 1.1, and VaxC. This
may still work today, BUT IS NOT SUPPORTED. Remember, your mileage may
vary. 

If you do not have reasonable experience with displaying remote apps on
local displays, you will probably have trouble with this app. Otherwise,
it should be simply plug and play.

Lyle W. West

