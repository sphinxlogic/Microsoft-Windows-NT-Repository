SIMH302-VMS, UTILITIES, simh with ethernet support for OpenVMS

This is a simh with ethernet support for OpenVMS Alpha. It contains
executables (for 7.3-1 or later) of the VAX and pdp-11 simulator with
(and without) ethernet support. To use the ethernet support CMKRNL is
needed and the PCAPVCM.EXE execlet needs to be copied to SYS$LOADABLE_DEVICES.


To build simh from sourcesw, unpack the ZIP file and start by building the
pcap support in [.PCAP-VMS] by invoking the BUILD_ALL.COM procedure.

To build simh with ethernet support, use:

$ MMS/MACRO=("__ALPHA__=1", "__PACP__=1") [vax | pdp11]

NOTE: Only the VAX and pdp-11 supports ethernet.


