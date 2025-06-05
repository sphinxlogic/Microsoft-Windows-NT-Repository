TCPDUMP-3_7_2, NETWORKING, TCPDUMP and PCAP for OpenVMS

This a port of the tcpdump and the pcap library to OpenVMS. There are two
versions of the pcap library for OpenVMS, one which uses the $QIO interface
to the OpenVMS LAN driver, and one which uses the VCI interface to the
LAN driver.

To build everything, just invoke the build_all.com procedure. 
Notice that the tcpdump port uses MMS (MMK should work as well)
for the build.

The subdirecotries contains:

 [.pcap-lan]
   A port of the pcap library using the $QIO interface to the LAN driver.
   This version of the library can only capture packets.

 [.pcap-vci]
   A port of pcap using the VCI interface to the LAN driver. This
   version of pcap uses the PCAPVCM execlet and is faster than the
   QIO implementation of pcap. This version also allows you to send
   packets as well as receiving packets. 

 [.pcapvcm]
   The VMS execlet which uses the VCI interface to the LAN driver.
   Once built it must be placed in SYS$LOADABLE_IMAGES.

 [.tcpdump-3_7_2]
   A quick and dirty port of tcpdump. When built, two versions of tcpdump
   are produced. One using the $QIO version of pcap and one using the VCI
   version of pcap. These are the only executables created by the build.

 [.vci_10_spec]
   A html version of the VCI version 1.0 specification.

 [.exe]
   Executables for tcpdump

