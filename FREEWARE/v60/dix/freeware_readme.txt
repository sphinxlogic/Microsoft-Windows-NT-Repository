DIX, Utilities, A program to read/modify records in any RMS (seq/relative/idx) file

This program lets you view/modify records in any RMS file.

Dix can work in three modes

a. Full screen mode. DIX uses SMG to display the data on the screen and 
   allows you to modify it (if you specify /MODIFY (not the default))
   This mode is the default on a terminal.
   You enter this mode by specifying the /SCREEN qualifier.

b. Interactive mode. DIX will prompt you for commands. This mode can also
   be used in batch with command files. If you specified /MODIFY
   the file can also be modified (not the default). 
   You also have some scripting possibilities with the IF and GOTO commands.
   You enter this mode by specifying the /INTER qualifier.
   See the help with DIX/HELP INTER for possible commands.

c. File mode. DIX will display a or more records and returns to DCL. No
   interaction is possible.
   You enter this mode by specifying the /FILE qualifier.

In all three modes the data can be displayed in two ways.

a. Interpreted. You need a record descriptor to do this.
   The descriptor file syntax looks like fortran record definitions(structures)
   and the descriptors can be in a file or in the DIX_DES.TLB text library.
   The layout of the descriptor files is described in the DIX
   help library under the topic 
   DISPLAY_MODES INTERPRETED /DESCRIPTION

   See DIX/HELP DISPL INTERP /DESCR

   DIX delivers (in DIX_DES.TLB) descriptors for the following files
    SYSUAF.DAT
    RIGHTSLIST.DAT
    INDEXF.SYS 
    VMS$PASSWORD_HISTORY
    VMSMAIL_PROFILE
    DIRECTORY files
    Some ALL-IN-1 files.

   And you can add any file you like, if you know the record layout.
  
b. Raw dump. The program displays the data like VMS DUMP and
   you can modify any byte.


  See the help via the HELP command.

usage:
Define DIX as a foreign command

DIX:=$'directory'dix_alpha or DIX_VAX

DIX filename /qualifiers

For the qualifiers see the help via DIX/HELP, but one very 
one important one is the /MODIFY. If you do not specify /MODIFY the
file is opened readonly and cannot be modified.

Note:
The files DIX.HLB and DIX_DES.TLB must be in the same directory as DIX, or
you must define the logical DIX_HELP and/or DIX_DES to another file.

Building:
The executables for Alpha and Vax are skipped with the kit, as well
as the sources. If you want to rebuild DIX, goto the dix directory and
use the command procedure @create_dix. This will compile (fortran) and
link DIX.

Examples:

$DIX SYSUAF[/SCREEN]/EQ=smith [/MODIFY]
  Will display the SYSUAF record of user "SMITH" using SMG, and lets
  you scroll though this data. If you specified /MODIFY, you can also
  modify entries (When you type any character on that field, you enter 
  modify mode for that field (this is signalled by an underline under
  the text)). You leave modify mode for that field when you type ENTER.
  The (modified) data is not written to the file until you type DO or PF4.
  F10 or ^Z returns you to DCL.

$DIX SYSUAF/INTER/EQ=smith [/MODIFY]
  Will enter interactive mode (non-smg), and allows you to inspect/modify
  fields of the current record. It also contains a (limited) scripting
  language. Type help in this mode to see the possible commands.

$DIX SYSUAF/FILE/EQ=smith
  Will display the data on the terminal, but will not allow you to modify
  the record (DUMP command).

$DIX datafile/FILE/REC=10
  Will display the data on the terminal, but will not allow you to modify
  the record (DUMP command). Only record 10 will be displayed

$DIX datafile/FILE/RECORD=10/COUNT=5
  Will display the data on the terminal, but will not allow you to modify
  the record (DUMP command). The records 10 upto 14 (5) wil be displayed.


Example of a (complicated) descriptor record of the INDEXF.SYS file
   the (n) are explained below

	ubyte		id_offset	! Offset to Ident area
	ubyte		map_offset	! Offset mapping area
	ubyte		acl_offset	! Offset to ACL area
	ubyte		res_offset	! Offset to reserved area
	integer*2	seg_num		! Extension seqment number
	byte struct_lev_min		! On disk structure level
	byte struct_lev_maj		! On disk structure level
	fileid          file_id		! File id
	fileid          ext_fid		! File id extension header
        structure       rec_attr	! RMS record attributes
         byte rectyp
         byte recattr
         integer*2 recsiz
         rinteger*4 hblk
         rinteger*4 eofblk
         integer*2 eofbyte
         byte bucketsize
         byte vfcsize
         integer*2 maxrec
         integer*2 defext
         integer*2 globbuf
         integer*2 %fil3(4)
         integer*2 verslim
	end structure
	bits*4		file_char -		(1)
		[Wascontig,Nobackup,Writeback,Readcheck,Writecheck,-
                 Contigb,Locked,Contig,,,,Badacl,-
		 Spool,Directory,Badblock,Markdel,Nocharge,Erase,alm_aip,-
                 shelved,scratch,nomove,noshelvable,shelv_res]
	character*2	%res_1		! reserved 1
	ubyte		map_in_use	! # map words in use
	byte		acc_mode	! File accessor priv mode needed
	uic		owner		! Owning UIC
	protection	protection	! File protection
	fileid		backl_fid	! Backlink file id
	bits*2		journal		! Journalling flags
	integer*2	ru_active	! Recover facility unit number
	integer		highwater	! Highest blocknr written + 1
	union
	 map struct_lev_maj=5                     (2)
	  byte FI5$B_CONTROL    [0=ODS-2,1=ODS-5] (3)
	  byte FI5$B_NAMELEN   
	  integer*2 FI5$W_REVISION   
	  date FI5$Q_CREDATE
	  date FI5$Q_REVDATE 
	  date FI5$Q_EXPDATE   
	  date FI5$Q_BAKDATE    
	  date FI5$Q_ACCDATE    
	  date FI5$Q_ATTDATE  
	  integer*8 FI5$Q_EX_RECATTR
	  integer*8 FI5$Q_LENGTH_HINT_LOW
	  integer*8 FI5$Q_LENGTH_HINT_HIGH
	  character*(fi5$b_namelen) FI5$S_FILENAME  (7)
	 end map
	 map struct_lev_maj=2
	  character*20	fnam		! Variable mapped entries
	  integer*2         revnr
	  date*8            cdat
	  date*8            rdat
	  date*8            edat
	  date*8            bdat
	  character*66	rest_fnam
	 end map
	 map *			(4)
	  integer*4 data(20)
	 end map
	end union
	range (map_offset*2:map_offset*2+map_in_use-1) (5)
           diskmap maps(256)	 
	end range
	range (acl_offset*2:res_offset*2-1)
	   acl acls(50)	
	end range
	position (min(510,max(0,recordsize)))    (6)
	integer*2/hex checksum

(1) is an example for a bits type. The part between the []
    gives an more friendly view of the bits.
    For example bit3 will be displayed as "readonly"
    Any bit not described will be displayed as BITnn (for example bit8)
(2) is an example of a union/map structure. A union contains one
    or more maps. This part is selected if the field STRUCT_LEV_MAJ 
    contains 5. See also (4)
(3) This is an example for an integer (byte). The part between
    the [] gives a list of possible values of the integer.
    For example : if the value of FI5$B_CONTROL is 1, the
    text displayed is ODS-5. Anly other value than 0 or 1 will
    be displayed as the normal numeric text.
(4) The rest of the union/map. If none of the map statements has
    matched, this one will. If you do not specify a map with an *,
    the first map will be taken (in this case the map struct_lev_maj=5 
(5) This is an example for the RANGE statement. A RANGE defines a
    part of the record. 
    In this case the field diskmap map(256) in contained in a part
    of the record between bytes map_offset*2:map_offset*2 and map_in_use-1
    The length 256 is choosen to be big enough.
(6) A example of a Position statement. This set the offset for the 
    next field to an absolute value (in this case 510).
    The next field (checksum) will be at offset 510.
(7) The length of the character string depends on a previous field
    (fi5$b_namelen). 

All fieldsizes are in bytes, except within a bitfield/endbitfield range.
In this bitfield_mode only (u)integer, bits and logical fields are allowed.


Another example of files that are linked 

  The example is about 3 RMS indexed files that form a simple sourcemodule
  cross_reference system

  The first file (CROSS_REF.CRF_CROSS) has the following description
  (.CRF_CROSS in the system or user textlibrary)
          integer*2    caller_nr /file=.crf_mod_names   !link to modulename
          integer*2    called_nr /file=.crf_mod_names   !link to modulename

  The second file (CROSS_REF.CRF_MOD_NAMES) has the following description
  (.CRF_MOD_NAMES in the system or user textlibrary)
          integer*2    mod_nr                           !primary key
          character*32 mod_name
          integer*2    file_nr/file=.crf_file_names     !link to the filename

  The third file (CROSS_REF.CRF_FILE_NAMES) has the following description
  (.CRF_FILE_NAMES in the system or user textlibrary)
          integer*2    file_nr                          !primary key
          character*60 file_name
          integer*2    %filler

  When you now (dix-)edit the CROSS_REF.CRF_CROSS file
    $DIX CROSS_REF.CRF_CROSS
    %DIX-I-USINGFIL, Using file DSA40:[DIR]CROSS_REC.CRF_CROSS
    %DIX-I-USINGDES, Using description SYSLIB(.CRF_CROSS)
    DIX> EXA *
     0|CALLER_NR>|738             !the > tells us there is a link present
     2|CALLED_NR>|-262

 You can follow the link to the next file

    DIX> Follow CALLER_NR         !try to follow this link
    File .CRF_MOD_NAMES not (yes) opened, open it (y/[n]):Y     !do you want to o
    %DIX-I-USINGFIL, Using file DSA40:[DIR]CROSS_REF.CRF_MOD_NAMES
    %DIX-I-USINGDES, Using description SYSLIB(.CRF_MOD_NAMES)
    DIX> Exa *
    0|MOD_NR  |738
    2|MOD_NAME|CHECK_ALLOWED_USER
   34|FILE_NR>|66                 !and this field also has a link defined

 Now follow the link to the third file (open automatic)

    DIX> Follow/automatic file_nr
    %DIX-I-USINGFIL, Using file DSA40:[DIR]CROSS_REF.CRF_FILE_NAMES
    %DIX-I-USINGDES, Using description SYSLIB(.CRF_FILE_NAMES)
    DIX> Exa *
    0|FILE_NR  |66
    2|FILE_NAME|REM_SERVER_CHECK_ACCESS

 Now backtrace to the CROSS_REF.CRF_MOD_NAMES file

    DIX> BACK
    %DIX-I-USINGFIL, Using file DSA40:[DIR]CROSS_REF.CRF_MOD_NAMES
    %DIX-I-USINGDES, Using description SYSLIB(.CRF_MOD_NAMES)
    DIX> Exa *
    0|MOD_NR  |738
    2|MOD_NAME|CHECK_ALLOWED_USER
   34|FILE_NR>|66
  

For a complete list of directives see the help in the DIX.HLB under the
topic "DISPLAY_MODES INTERPRETED_DUMP/DESCRIPTION RECORD_FORMAT". Use
DIX/HELP to display this help.

Note:

Be careful when modifying datafiles. DIX is very powerful and has no UNDO
function after you update the record. If you modify the record, there is 
no way back except the backup (if you have one). /BLOCK mode is even more
powerful, since it bypasses RMS. 

This package contains the following files

In the main directory

 DIX_VAX.EXE			The Vax executable
 DIX_ALPHA.EXE			The Alpha Executable 
 DIX.HLB			The help library
 DIX_DES.TLB			The file containing the descriptor records
 CREATE_DIX.COM			The command procedure to compile/link
 FREEWARE_README.FIRST		This file

In the [.SRC] directory

 The FORTRAN sources

  DIX_MAIN.FOR			The main program
  DIX_CON_LIBRARY.FOR           The conversion routines ascii<>binary
  DIX_DES_LIBRARY.FOR		The descriptor file routines
  DIX_DUMP_LIBRARY.FOR 		The dump routines
  DIX_DUMP_FILE_LIBRARY.FOR	The dump to file (noscreen) routines
  DIX_DUMP_INTERACTIVE_LIBRARY.FOR	The dump to file (noscreen) routines
  DIX_DUMP_SCREEN_LIBRARY.FOR	The dump to screen (SMG) routines
  DIX_EDIT_LIBRARY.FOR		The editor functions
  DIX_EVAL_LIBRARY.FOR		The expression evaluation routines
  DIX_HELP_LIBRARY.FOR		The interface to the help (FSHELP) program 
  DIX_KEYDEFS.FOR               The key-definition library
  DIX_LBR_LIBRARY.FOR		The LBR interface routines
  DIX_MEMTAB.FOR		The memory-file routines
  DIX_RMS_LIBRARY.FOR		The interface to RMS routines
  DIX_STARTUP_LIBRARY.FOR       The Startup routines
  DIX_SYMBOL_LIBRARY.FOR        The routines for symbol manipulation.
  DIX_SMG_LIBRARY.FOR		The interface to SMG routines
  DIX_UTIL_LIBRARY.FOR		The utility library

 The include files

  DIX_DEF.INC			The general include definitions
  DIX_MEMTAB.INC		The MEMTAB definitions
  DIX_KEYDEFS.INC		THe keydefs definitions
  
 The Help file

  DIX.HLP

 The CLD files

  DIX_CLD.CLD			The command line definitions
  DIX_INT_CLD.CLD	   	The commands for interactive mode
  DIX_STARTUP_CLD.CLD		The commands for the startup file

In the [.VAX] directory
 The vax objects
In the [.ALPHA]
 The Alpha objects

Instructions:

 Unpack the save set
 If you want to rebuild the program

 If you have a fortran compiler 
   @create_dix 
 otherwise, just link
   @create_dix link
    I included all objects so you can relink.

Author: Fekko Stubbe

If you have questions or suggestions, please mail to the mail address below:
Email : dixdev (at) 4ovms.dyndns.org 
  or    fekko.stubbe (at) xs4all.nl
