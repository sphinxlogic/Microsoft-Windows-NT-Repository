This directory tree contains the UCX SMTP images with an additional feature
added for Majordomo support. The new feature is an SMTP address that allows you
to specify the name of a mailing list file as the local part of an address. EG:

   "UCX$MAJORDOMO_LIST:SAMPLE."@acme.com

The above address will invoke the new feature and tell UCX SMTP to open the file
UCX$MAJORDOMO_LIST:SAMPLE. as a list of SMTP addresses. The mail message will be
sent to each of the addresses in the file. The format of this file is exactly
the same as that of "old style" UCX SMTP mailing lists.

This new feature is roughly analogous to the Unix alias file include capability
with which one can point an alias to a list file and sending to the alias sends
to all addresses in the file. It was added to allow a close Unix to VMS port of
the Majordomo alias file entry "listname-OUTGOING". The VMS SET FORWARD/USER
command is used by VMS Majordomo in the same way that the Unix alias file is
used by Unix Majordomo. For more information see the VMS Majordomo readme file.


Do I need to install the images?

These images are only needed if the SMTP that you have installed doesn't support
the new feature.

If you have UCX V4.2:

   All versions UCX V4.2 will have support for this so if you have UCX V4.2 you
   don't need to bother with any of this.

If you have UCX V4.0 or lower:

   It is most likely that the above feature will not make it into an ECO.
   Consequently you will need to upgrade to either V4.1 or V4.2. As stated above
   upgrading to V4.2 means you don't have to install these images. Going to V4.1
   means you might. Read on...

If you have UCX V4.1:

|  If you have UCX V4.1 ECO 3 or later then you do not need to install the
|  SMTP images here. If you do not have ECO 3 then you shoudl either upgrade to
|  ECO 3 or install these images.

How to install the images.

   New versions of all four UCX SMTP images are provided. They are built against
   UCX V4.1 as stated earlier. Images for VAX are in [.VAX] and images for AXP
   are in [.AXP]. To install them turn on your priv's and @ the INSTALL.COM in
   this directory.

   It is recommended that you do this when no SMTP activity is taking place on
   your system.

   If you are running on a cluster you need to do this on all the cluster nodes
   on which you want Majordomo to run.
