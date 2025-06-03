GETCMD V3.6, TOOLS, Display others' command recall buffers

   [16-AUG-1995]

   GETCMD displays the DCL command recall buffer for any interactive
   process on the system. Unlike DCL's RECALL command, GETCMD will
   display all of the commands stored in the buffer, not just the most
   recent 20 or 255. For example:

   $ getcmd 202000af
     1 getcmd 20200af
     2 sho quota
     3 sh use/ful
     4 sd #
     5 ty [local.finger]*.mms
     6 Mail
   $

   GETCMD supports both the 1K buffer used by OpenVMS VAX and OpenVMS AXP
   V1.x, as well as the new 4K buffer used by OpenVMS AXP V6.1.

   Two qualifiers, /PAGE and /OUTPUT, are supported to control the GETCMD
   output.

   The BLISS sources are distributed with the software.

     _________________________________________________________________

  CONTACTING THE AUTHOR

   MadGoat GETCMD was written by Hunter Goatley. To report bugs, please
   send mail to MadGoat-Bugs@WKUVX1.WKU.EDU. Additional MadGoat mailing
   lists are available as well.
