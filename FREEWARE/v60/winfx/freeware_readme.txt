WINFX, UTILITIES, Windows File Exchange Utility

The WINFX utility provides the capability to exchange disk-resident
files between OpenVMS and Windows operating systems. It supports the
FAT12, FAT16, and FAT32 file systems on floppy disks and partitioned
hard disks, including ZIP disks. It can also construct and access
these file systems within container files.

The WINFX utility works on both OpenVMS VAX and OpenVMS Alpha.
On VAX systems, version 7.1 or higher is recommended, and on
Alpha systems, version 7.1-2 or higher is recommended. It may work
on older versions, but it will depend on the level of SCSI support
in the base operating system and applied ECOs (some work, some don't).
WINFX was compiled using DEC C version 6.2 on Alpha and version 6.0 on
VAX. It was tested under OpenVMS versions 6.2, 7.1-2, 7.2-2, 7.3 and
7.3-1 on Alpha, and version 7.2 on VAX.

The WINFX utility is a command-driven program. Please see the help
within WINFX for detailed information. Following is a brief summary
of the commands it supports.

  USE           Associates a drive letter (A-Z) with a file system.

  DISUSE        Releases a drive letter file system association.

  FORMAT        Creates a new empty file system.

  PARTITION     Partitions a hard disk.

  IMPORT (WRITE)
                Copies files into a file system from OpenVMS. Binary
                and text modes are supported. Can create and import
                an associated FDL file.

  EXPORT (READ) Copies files from a file system to OpenVMS. Binary and
                text modes are supported. Can use an FDL file when
                creating exported files.

  DIRECTORY     Lists the contents of a directory. Several attributes
                of the files can be displayed, and a MSDOS-like display
                is supported.

  CREATE/FILE_SYSTEM
                Creates an empty container file in which one or more
                file systems can be constructed.

  CREATE/DIRECTORY (MKDIR)
                Creates a new empty directory.

  DELETE        Deletes a set of files.

  DELETE/DIRECTORY (RMDIR)
                Deletes a directory.

  HELP          Get help about a command or function.

  SET PATH (CD) Sets the default directory path.

  SET STYLE     Sets long or short file name style.

  SHOW PATH (PWD)
                Shows the current default directory path.

  SHOW DRIVE    Shows information about a drive and its associated
                file system.

  SHOW PARTITION
                Shows partition information for a hard disk.

  EXIT          Terminate the program.
