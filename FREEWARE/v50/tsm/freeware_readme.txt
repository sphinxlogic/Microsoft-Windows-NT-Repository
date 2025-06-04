TSM, Utilities, Terminal Server Manager

  Terminal Server Manager (TSM) V2.1 

  This file is based on the Web Distribution Kit README.TXT.

  The files in this directory are as follows:

    TSM021.DCX_VAXEXE
    TSM021.DCX_AXPEXE
    TMS_PAK.COM
    README.TXT

  The TMS_PAK.COM file will register and load a License Management
  Facility (LMF) Product Authorzation Key (PAK) for TSM. You may or
  may not need to execute this VMS command file, depending on the 
  availability of a TSM license in LMF.

    $ @tsm_pak.com

  The TSM021.DCX_VAXEXE file is an OpenVMS / VAX executable file.
  To extract the contents on a VAX system, run it .  

    $ run tsm021.dcx_vaxexe 

  The TSM021.DCX_AXPEXE file is an OpenVMS / AXP executable file.
  To extract the contents on a Alpha system, run it .  

    $ run tsm021.dcx_axpexe

  You will get a resultant file (name dependent on what you told 
  the decompression utility to use for an output file name), which
  is an Open VMS SAVESET.

    e.g. TSM021.SAV

  Create a working directory, for example:

    $ create /directory [.tms]

  Don't do this in an area (default directory) that is a VMS 
  LOGICAL that uses those funky VMS search lists.  Examples of 
  this are:

    SYS$MANAGER: and SYS$UPDATE:
        
  Extract the saveset into the working directory:

    $ backup tsm021.sav /sav [.tsm...]

  Then execute the OpenVMS installation procedure in the normal 
  fashion:

    $ @sys$update:vmsinstal

  The location of the kit directory should be the fully specified
  path to the [.KIT] subdirectory in the working directory you
  created (e.g. DKA100:[JOE.TSM.KIT]).  Don't specify the path
  using system logicals for the disk; use the disk device name.

  The softcopy manuals are to be found in the [.DOCUMENTATION]
  subdirectory of the working directory that you created
  (e.g. DKA100:[JOE.TSM.DOCUMENTATION]).

  Enjoy!

