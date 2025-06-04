MULTIA, SYSTEM_MGMT, Tools for booting OpenVMS on Multia systems

  Unsupported files for use of OpenVMS on Multia Alpha Systems

  Complete instructions are included IN the kits.  

  PLEASE READ THE INSTRUCTIONS BEFORE PROCEEDING.

  This directory includes various versions of files that will likely
  allow OpenVMS hobbyists to bootstrap OpenVMS on the Multia (Alpha)
  system.

  Limitations:

     o Many; please read the instructions in the kit for details.

     o There is absolutely no support for OpenVMS on Multia.

     o If the kit does not work, you are entirely on your own.
       Reading the instructions might help, but there is NO support.

     o Kits are highly version-specific, and are available only for 
       specific OpenVMS releases.  As of this writing, all available 
       kits are included here and no other kits are planned, nor is 
       there any expectation of any new kits for any other OpenVMS 
       releases.

     o The PCMCIA support was completely removed, because the Intel
       chip on the Multia was not compatable with the Cirrus chip on
       the Alphabook.

       This means, of course, that you will not see and cannot use
       any PCMCIA cards on a Multia.

     o The Multia uses shared interrupts, and as a result, a special
       ZLXp-E series graphics device driver -- one that does not use
       interrupts -- is needed.  This driver is provided in the kit.

     o The serial lines don't work.

     o If you have a Multia with a PCI slot, you can't use any PCI
       card that requires interrupts.

     o The SRM console on this system is very old and very fragile.
       (This SRM console was designed only and strictly for diagnostic
       use, and was not particularly tested or used with OpenVMS.)

       YOU MUST USE THE PROVIDED SRM CONSOLE.

     o If things don't work for you, don't expect to see any OpenVMS
       updates, nor SRM console updates, nor any support.

  That said, these files have been occasionally found to work, and
  even to permit the operation of DECwindows on Multia.

  Pointers to Multia information are included in the OpenVMS FAQ.

