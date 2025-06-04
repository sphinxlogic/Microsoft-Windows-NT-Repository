NETRTG040, SYSTEM_MGMT, DECnet Phase IV Routing Key for V4.x

Unsupported DECnet Phase IV Routing Key

  The BACKUP saveset kit in this directory enables both DECnet end-node 
  and DECnet routing capabilities for DECnet Phase IV on MicroVMS V4.x 
  and VAX/VMS V4.x.  This key does *not* enable DECnet on VAX/VMS V5.x 
  nor any later VMS or OpenVMS releases.

  Limitations:

     o There is absolutely no support for this.

     o There is absolutely no support for DECnet on these releases.

     o This key operates only on VAX/VMS V4.x and MicroVMS V4.x.

     o If this does not work, you are entirely on your own.

  That said, this key has occasionally been found to work.

  FTP Warning:

    When a BACKUP saveset kit is transfered via FTP, you may (will?) 
    need to reset the OpenVMS file attributes via a tool such as:

      RESET_BACKUP_SAVESET_ATTRIBUTES.COM

    This tool is available on the OpenVMS Freeware distribution.

