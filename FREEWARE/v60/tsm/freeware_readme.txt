TSM, Utilities, Terminal Server Manager

  Terminal Server Manager (TSM) V2.1 (with ECOs)

  This file is loosely based on the TSM Web Distribution Kit 
  README.TXT, and on materials and changes for Freeware V6.0.

  The files in this directory are as follows:

    TSM.ZIP, containing:
      TSMECO07021.A
      TSMECO08021.A
      TSMECO09021.A
    TSM_PAK.COM
    FREEWARE_README.TXT

  The TSM_PAK.COM file will register and load a License Management
  Facility (LMF) Product Authorzation Key (PAK) for TSM.  If you
  already have a PAK for TSM, you do not need this PAK.

    $ @tsm_pak.com

  Once you have unzipped the kit, execute the OpenVMS VMSINSTAL
  installation procedure (for the target ECO installation kit) in
  the normal fashion:

    $ @sys$update:vmsinstal

  The location of the kit directory should be the fully specified
  path to the subdirectory containing the unzipped kits.

  Additional materials and TSM-related documentation is available
  on the OpenVMS Freeware V5.0 TSM kit.

