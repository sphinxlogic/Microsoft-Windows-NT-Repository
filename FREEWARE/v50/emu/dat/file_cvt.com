$! File : FILE_CVT.COM          Author : K. O'BRIEN     29th May 1991
$       goto start_code
$!
$! Description
$!
$! This file provides the general capability to convert/fdl files,having
$! previously produced and FDL file by analysing the data file specified.
$!
$! Several parameters are required for this purpose. These parameters are
$! expected to be found on the command line, and if not, they will be prompted
$! for. These command lines are expected to be issued from another command 
$! file, on specified days, and therefore not necessarily typed in, although 
$! this is possible for adhoc requests. Once the relevant parameters have been 
$! parsed, the command file resubmits itself to run on a Batch queue, unless 
$! already running in batch mode, in order to free up the terminal.
$! A log file is kept of the conversions done.
$!
$! Parameter specifications 
$!
$! P1>  The full file specification of the file to be converted. Any parts of
$!      the filespecification not given will be taken from the current default
$!      directory, although the filename and filetype must be specified.
$!              e.g., DATA_FILE.IDX
$!
$! P2>  The Directory specification of the directory where the 
$!      converted file is to be created. This is specified if there is not
$!      enough room on the disk where the file being converted normally resides.
$!       If this is specified then the original file being converted is deleted,
$!      before the converted file replaces it, in order to create enough room
$!      for the new converted file to be copied. 
$!       If this is specified then the analyse and optimised fdl files created
$!      are created in this directory.
$!       This only happens if the conversion process is successful and if 
$!      there is enough room on the output device following any deletion of 
$!      the original file being converted.
$!       If not required type NULL, then the output file from the conversion,
$!      simple supercedes the original file in the same location and the FDL 
$!      files are created in the directory of the file being converted. The 
$!      input file is only purged depending on the purge parameter.
$!              e.g., NULL
$!              e.g., DISK$SPARE:[CONVERTS]
$!
$!
$! P3>  The sort directory specification. This directory on the specified disk
$!      is used for temporary sort work files. If specified two sort files
$!      sortwork0 & sortwork1 are defined to this directory location.
$!      If no other directory is available other than the default one then 
$!      use NULL.
$!              e.g., NULL                      !directory where the file is
$!              e.g., DISK$SPARE:[CONVERTS]     !another device:[directory]
$!
$!
$! P4>  The copy directory specification. If a copy of the file is required
$!      for backup/safety purposes before the conversion takes place then
$!      this parameter can be specified as the directory where the original
$!      file can be copied to. The file is not deleted following the 
$!      conversion process. If not required type "NULL".
$!
$! P5>  The purge parameter. This parameter specifies whether the files created
$!      by this procedure are purged. Those files are as follows :-
$!              i)      The analyse fdl file of the file being converted
$!              ii)     The optimised fdl file of the file being converted
$!              iii)    The file being converted
$!      If specified as NULL, then the fdl files are deleted after the 
$!      conversion process is completed, if successfull.
$!      If specified as 1, then all three files are purged /keep=1 after the 
$!      conversion process is completed, if successfull; etc
$!
$! P6>  Mail User(s) to be informed upon conversion with the log file from the
$!      conversion. This is an optional list of Users who receive the batch
$!      log file following the conversion process. This contains the details
$!      of each operation during the conversion.
$!      If not required type "NULL".
$!
$! P7>  An FDL file to use in the conversion process, specifically tuned for 
$!      the file being converted. If not required type "NULL". If specified, 
$!      then no analyse/rms/fdl and edit/fdl/optim steps are undertaken during
$!      the convert process, just the file convert.
$!
$!
$! Initialise parameters
$start_code:
$       ver = f$ver(0)
$       ident = "1.1"
$       if ver then file_cvt_debug = 1
$       old_def = f$environment("default")
$       privs_required = "sysprv,bypass,world"
$       priv = f$setprv (privs_required)
$       old_sylogin = f$trnlnm ("SYS$LOGIN")
$       old_scratch = f$trnlnm ("SYS$SCRATCH")
$       this_proc = f$environment ("procedure")
$!
$       write sys$output -
        f$fao("!/!_File conversion procedure (V!AS). !%D!/",ident,0)
$
$       file_to_cvt             = ""
$       file_to_cvt_devdir      = ""
$       dir_to_cvt_in           = ""
$       dir_to_cvt_in_dev       = ""
$       dir_to_cvt_in_dev_free  = 0
$       sortwork_dir            = ""
$       sortwork_dev            = ""
$       sortwork_dev_free       = 0
$       backup_dir              = ""
$       purge_files             = 0
$       mail_users              = ""
$       fdl_to_use              = ""
$       do_cvt_in_place         = 0
$
$
$! Check Parameters supplied
$!
$       goto p1_check
$bad_p1:
$       if f$mode() .nes. "INTERACTIVE" then goto tidyup
$       p1 = ""
$p1_check:
$       p1 = f$edit(p1, "upcase,collapse,uncomment")
$       if "''p1'" .eqs. ""
$       then
$          if f$mode() .nes. "INTERACTIVE"
$          then
$             write sys$output f$fao("P1 Error: Parameter not supplied")
$             goto tidyup
$          endif
$          type sys$input

P1>     Please supply the full file specification of the file to be converted. 
        Any parts of the file specification not given will be taken from the 
        current default directory, although the filename and filetype must be 
        specified.
                e.g., DATA_FILE.IDX

$!
$          read sys$command p1 /prompt="File to convert >"/end=tidyup
$          goto p1_check
$       endif 
$       if f$parse(p1,,,"name") .eqs. ""
$       then
$          write sys$output f$fao("P1 Error: No filename supplied in !AS", p1)
$          goto bad_p1
$       endif
$       if f$parse(p1,,,"type") .eqs. "."
$       then
$          write sys$output f$fao("P1 Error: No filetype supplied in !AS", p1)
$          goto bad_p1
$       endif
$       file_to_cvt =   f$parse (p1,,,"device","no_conceal") + -
                        f$parse (p1,,,"directory","no_conceal") + -
                        f$parse (p1,,,"name") + -
                        f$parse (p1,,,"type")
$       file_to_cvt = file_to_cvt - "]["
$       file_to_cvt = file_to_cvt - ".000000"
$       file_to_cvt_devdir =    f$parse (file_to_cvt,,,"device") + -
                                f$parse (file_to_cvt,,,"directory")
$       open/read test_file/error=open_file_test_failure 'file_to_cvt'
$       close test_file
$       goto open_file_test_ok
$open_file_test_failure:
$       write sys$output f$fao -
                ("P1 Error: Filespec error using !AS (Status:!AS)", -
                file_to_cvt, $status)
$       goto bad_p1
$
$open_file_test_ok:
$       if "''file_cvt_debug'" .nes. "" then write sys$output -
        f$fao("File to convert >!AS", file_to_cvt)
$       if "''file_cvt_debug'" .nes. "" then write sys$output -
        f$fao("File to convert devdir >!AS", file_to_cvt_devdir)
$!
$       p1 = file_to_cvt
$       goto p2_check
$!
$bad_p2:
$       if f$mode() .nes. "INTERACTIVE" then goto tidyup
$       p2 = ""
$p2_check:
$       p2 = f$edit(p2, "upcase,collapse,uncomment")
$       if p2 .eqs. "NULL"
$       then
$          p2 = file_to_cvt
$          goto p2_check_end
$       endif
$       if "''p2'" .eqs. ""
$       then
$          if f$mode() .nes. "INTERACTIVE"
$          then
$             write sys$output f$fao("P2 Error: Parameter not supplied")
$             goto tidyup
$          endif
$          type sys$input

P2>     Please supply the Directory specification of the directory where the 
        converted file is to be created. This is specified if there is not
        enough room on the disk where the file being converted normally resides.
         If this is specified then the original file being converted is deleted,
        before the converted file replaces it, in order to create enough room
        for the new converted file to be copied. 
         If this is specified then the analyse and optimised fdl files created
        are created in this directory.
         This only happens if the conversion process is successful and if 
        there is enough room on the output device following any deletion of 
        the original file being converted.
         If not required type NULL, then the output file from the conversion,
        simple supercedes the original file in the same location and the FDL 
        files are created in the directory of the file being converted. The 
        input file is only purged depending on the purge parameter.
                e.g., NULL
                e.g., DISK$SPARE:[CONVERTS]

$!
$          read sys$command p2 /end=tidyup -
                /prompt="Device:[Directory] to convert file in >" 
$          goto p2_check
$       endif 
$       if f$parse(p2,,,"node") .nes. ""
$       then                        
$          write sys$output f$fao("P2 Error: No Nodename is allowed (!AS)",p2)
$          goto bad_p2
$       endif
$       if f$parse(p2,,,"name") .nes. ""
$       then
$          write sys$output f$fao("P2 Error: No Filename is allowed (!AS)",p2)
$          goto bad_p2
$       endif
$       if f$parse(p2,,,"type") .nes. "."
$       then
$          write sys$output f$fao("P2 Error: No Filetype is allowed (!AS)",p2)
$          goto bad_p2
$       endif
$       if f$parse(p2,,,"version") .nes. ";"
$       then
$          write sys$output f$fao("P2 Error: No version is allowed (!AS)", p2)
$          goto bad_p2
$       endif
$
$p2_check_end:
$       dir_to_cvt_in = f$parse (p2,,,"device","no_conceal") + -
                        f$parse (p2,,,"directory","no_conceal")
$       dir_to_cvt_in = dir_to_cvt_in - "]["
$       dir_to_cvt_in = dir_to_cvt_in - ".000000"
$       dir_to_cvt_in_dev = f$parse (dir_to_cvt_in,,,"device")
$       dir_to_cvt_in_dev_free = -
                f$int(f$getdvi (dir_to_cvt_in_dev, "freeblocks"))
$       do_cvt_in_place         = f$int(0)
$       if "''file_to_cvt_devdir'" .eqs. "''dir_to_cvt_in'" then -
                do_cvt_in_place = f$int(1)
$
$       if "''file_cvt_debug'" .nes. ""
$       then
$          write sys$output -
             f$fao("Directory to convert in >!AS", dir_to_cvt_in)
$          write sys$output f$fao("Device to convert in >!AS", -
                dir_to_cvt_in_dev)
$          write sys$output f$fao("Device to convert in Free >!UL", -
                dir_to_cvt_in_dev_free)
$          write sys$output f$fao("Do convert in place :>!UL", -
                f$int(do_cvt_in_place))
$       endif
$!
$       p2 = dir_to_cvt_in
$       goto p3_check
$!
$!
$bad_p3:
$       if f$mode() .nes. "INTERACTIVE" then goto tidyup
$       p3 = ""
$p3_check:
$       p3 = f$edit(p3, "upcase,collapse,uncomment")
$       if p3 .eqs. "NULL"
$       then
$          p3 = file_to_cvt
$          goto p3_check_end
$       endif
$       if "''p3'" .eqs. ""
$       then
$          if f$mode() .nes. "INTERACTIVE"
$          then
$             write sys$output f$fao("p3 Error: Parameter not supplied")
$             goto tidyup
$          endif
$          type sys$input

P3>     Please supply the sort directory specification. This directory on the 
        specified disk is used for temporary sort work files. If specified, 
        then two sort files sortwork0 & sortwork1 are defined to this 
        directory location. If no other directory is available other than the
        default one then use NULL.
                e.g., NULL                      !directory where the file is
                e.g., DISK$SPARE:[CONVERTS]     !another device:[directory]

$!
$          read sys$command p3 /end=tidyup -
                /prompt="Device:[Directory] for sortwork files >" 
$          goto p3_check
$       endif 
$       if f$parse(p3,,,"node") .nes. ""
$       then                        
$          write sys$output f$fao("p3 Error: No Nodename is allowed (!AS)",p3)
$          goto bad_p3
$       endif
$       if f$parse(p3,,,"name") .nes. ""
$       then
$          write sys$output f$fao("p3 Error: No Filename is allowed (!AS)",p3)
$          goto bad_p3
$       endif
$       if f$parse(p3,,,"type") .nes. "."
$       then
$          write sys$output f$fao("p3 Error: No Filetype is allowed (!AS)",p3)                                                                        
$          goto bad_p3
$       endif
$       if f$parse(p3,,,"version") .nes. ";"
$       then
$          write sys$output f$fao("p3 Error: No version is allowed (!AS)", p3)
$          goto bad_p3
$       endif
$
$p3_check_end:
$       sortwork_dir =  f$parse (p3,,,"device","no_conceal") + -
                        f$parse (p3,,,"directory","no_conceal")
$       sortwork_dir = sortwork_dir - "]["
$       sortwork_dir = sortwork_dir - ".000000"
$       sortwork_dev = f$parse (sortwork_dir,,,"device")
$       sortwork_dev_free = f$int(f$getdvi (sortwork_dev, "freeblocks"))
$
$       if "''file_cvt_debug'" .nes. ""
$       then
$          write sys$output f$fao("Sort work Dir >!AS", sortwork_dir)
$          write sys$output f$fao("Sort work Dev >!AS", sortwork_dev)
$          write sys$output f$fao("Sort work Dev Free >!UL",
sortwork_dev_free)
$       endif
$!
$       p3 = sortwork_dir
$       goto p4_check
$!
$!
$bad_p4:
$       if f$mode() .nes. "INTERACTIVE" then goto tidyup
$       p4 = ""
$p4_check:
$       p4 = f$edit(p4, "upcase,collapse,uncomment")
$       if p4 .eqs. "NULL"
$       then
$          backup_dir = "NULL"
$          goto p4_check_end
$       endif
$       if "''p4'" .eqs. ""
$       then
$          if f$mode() .nes. "INTERACTIVE"
$          then
$             write sys$output f$fao("p4 Error: Parameter not supplied")
$             goto tidyup
$          endif
$          type sys$input

P4>     Please supply the copy directory specification. If a copy of the file 
        is required for backup/safety purposes before the conversion takes 
        place then this parameter can be specified as the directory where the 
        original file can be copied to. The file is not deleted following the 
        conversion process. If not required use "NULL".
                e.g., NULL                      !Not required
                e.g., []                        !directory where the file is
                e.g., DISK$SPARE:[CONVERTS]     !another device:[directory]

$!
$          read sys$command p4 /end=tidyup -
                /prompt="Device:[Directory] for copy of original file >" 
$          goto p4_check
$       endif 
$       if f$parse(p4,,,"node") .nes. ""
$       then                        
$          write sys$output f$fao("p4 Error: No Nodename is allowed (!AS)",p4)
$          goto bad_p4
$       endif
$       if f$parse(p4,,,"name") .nes. ""
$       then
$          write sys$output f$fao("p4 Error: No Filename is allowed (!AS)",p4)
$          goto bad_p4
$       endif
$       if f$parse(p4,,,"type") .nes. "."
$       then
$          write sys$output f$fao("p4 Error: No Filetype is allowed (!AS)",p4)
$          goto bad_p4
$       endif
$       if f$parse(p4,,,"version") .nes. ";"
$       then
$          write sys$output f$fao("p4 Error: No version is allowed (!AS)", p4)
$          goto bad_p4
$       endif
$
$       backup_dir =    f$parse (p4,,,"device","no_conceal") + -
                        f$parse (p4,,,"directory","no_conceal")
$       backup_dir = backup_dir - "]["
$       backup_dir = backup_dir - ".000000"
$
$p4_check_end:
$       if "''file_cvt_debug'" .nes. "" then write sys$output -
        f$fao("Copy/Backup Directory >!AS", backup_dir)
$!      
$!
$       p4 = backup_dir
$       goto p5_check
$!
$!
$bad_p5:
$       if f$mode() .nes. "INTERACTIVE" then goto tidyup
$       p5 = ""
$p5_check:
$       p5 = f$edit(p5, "upcase,collapse,uncomment")
$       purge_files = f$int(p5)
$       if p5 .eqs. "NULL" then goto p5_check_end
$       if purge_files .eq. 0
$       then
$          if f$mode() .nes. "INTERACTIVE"
$          then
$             write sys$output f$fao("p5 Error: Parameter not supplied")
$             goto tidyup
$          endif
$          type sys$input

P5>     Please supply the purge parameter. This parameter specifies whether 
        the files created by this procedure are purged. Those files are as 
        follows :-
                i)      The analyse fdl file of the file being converted
                ii)     The optimised fdl file of the file being converted
                iii)    The file being converted
        If specified as NULL, then the fdl files are deleted after the 
        conversion process is completed, if successfull.
        If specified as 1, then all three files are purged /keep=1 after the 
        conversion process is completed, if successfull; etc

$!
$          read sys$command p5 /end=tidyup -
                /prompt="Number of files to purge >" 
$          goto p5_check
$       endif 
$
$       if purge_files .lt. 1
$       then                        
$          write sys$output f$fao("p5 Error: Invalid purge number (!AS)", p5)
$          goto bad_p5
$       endif
$
$p5_check_end:
$       if "''file_cvt_debug'" .nes. "" then write sys$output -
        f$fao("Number of files to purge >!AS", p5)
$!
$       goto p6_check
$!
$bad_p6:
$       if f$mode() .nes. "INTERACTIVE" then goto tidyup
$       p6 = ""
$p6_check:
$       p6 = f$edit(p6, "upcase,collapse,uncomment")
$       if p6 .eqs. "NULL"
$       then
$          mail_users = "NULL"
$          goto p6_check_end
$       endif
$       if "''p6'" .eqs. ""
$       then
$          if f$mode() .nes. "INTERACTIVE"
$          then
$             write sys$output f$fao("p6 Error: Parameter not supplied")
$             goto tidyup
$          endif
$          type sys$input

P6>     Please supply the User(s) to be Mailed/informed upon conversion
        completion. These Users with receive the log file from the conversion. 
        If not required type "NULL".
                e.g., NULL              !Not required
                e.g., SYSTEM,JONES_S    !Users to receive VMS MAIL

$!
$          read sys$command p6 /end=tidyup -
                /prompt="Mail Users List (or NULL) >" 
$          goto p6_check
$       endif 
$check_p6_elements:
$       ele=-1
$next_p6_ele:
$       ele=f$int(ele+1)
$       next_mail_user = f$element('ele',",","''p6'")
$       if next_mail_user .eqs. "," then goto fin_p6_ele_check
$       goto next_p6_ele
$       
$fin_p6_ele_check:
$       if ele .eq. 0
$       then
$          write sys$output f$fao("p6 Error: No mail Users in list, (!AS)",p6)
$          goto bad_p6
$       endif
$
$       mail_users = p6
$
$p6_check_end:
$       if "''file_cvt_debug'" .nes. "" then write sys$output -
        f$fao("Mail Users are >!AS", mail_users)
$!      
$       p6 = mail_users
$       goto p7_check
$!
$bad_p7:
$       if f$mode() .nes. "INTERACTIVE" then goto tidyup
$       p7 = ""
$p7_check:
$       p7 = f$edit(p7, "upcase,collapse,uncomment")
$       if p7 .eqs. "NULL"
$       then
$          fdl_to_use = "NULL"
$          goto p7_check_end
$       endif
$       if "''p7'" .eqs. ""
$       then
$          if f$mode() .nes. "INTERACTIVE"
$          then
$             write sys$output f$fao("p7 Error: Parameter not supplied")
$             goto tidyup
$          endif
$          type sys$input

P7>     Please supply an FDL file to use in the conversion process,
        specifically tuned for  the file being converted. If specified, then no
        analyse/rms/fdl and edit/fdl/optim steps are undertaken during the
        convert process, just the file convert.
        If not required type "NULL".
                e.g., DATA_FILE.FDL

$!
$          read sys$command p7 /prompt="FDL to use in convert >"/end=tidyup
$          goto p7_check
$       endif 
$       if f$parse(p7,,,"name") .eqs. ""
$       then
$          write sys$output f$fao("p7 Error: No filename supplied in !AS", p7)
$          goto bad_p7
$       endif
$       if f$parse(p7,".FDL",,"type") .eqs. "."
$       then
$          write sys$output f$fao("p7 Error: No filetype supplied in !AS", p7)
$          goto bad_p7
$       endif
$       fdl_to_use =    f$parse (p7,,,"device","no_conceal") + -
                        f$parse (p7,,,"directory","no_conceal") + -
                        f$parse (p7,,,"name") + -
                        f$parse (p7,".FDL",,"type")
$       fdl_to_use = fdl_to_use - "]["
$       fdl_to_use = fdl_to_use - ".000000"
$
$       open/read test_file/error=open_fdl_test_failure 'fdl_to_use'
$       close test_file
$       goto open_fdl_test_ok
$open_fdl_test_failure:
$       write sys$output f$fao("p7 Error: FDL file error using !AS", -
                fdl_to_use)
$       goto bad_p7
$
$open_fdl_test_ok:
$
$p7_check_end:
$       if "''file_cvt_debug'" .nes. ""
$       then
$          write sys$output f$fao("FDL to use in convert >!AS", fdl_to_use)
$       endif
$       p7 = fdl_to_use
$!
$!**********************************************************************
$!
$       write sys$output f$fao -
        ("File to convert        : !AS", p1)
$       write sys$output f$fao -
        ("Dir to convert in      : !AS    (!UL free block!%S)", -
        p2, f$int(dir_to_cvt_in_dev_free))
$       write sys$output f$fao -
        ("Convert in place       : !UL", f$int(do_cvt_in_place))
$       write sys$output f$fao -
        ("Sort work Dir          : !AS    (!UL free block!%S)", -
        p3, f$int(sortwork_dev_free))
$       write sys$output f$fao -
        ("Copy/Backup Directory  : !AS", p4)
$       write sys$output f$fao -
        ("Files purge limit      : !AS", p5)
$       write sys$output f$fao -
        ("Mail Users are         : !AS", p6)
$       write sys$output f$fao -
        ("FDL to use in convert  : !AS", p7)
$!
$       if f$mode() .eqs. "INTERACTIVE"
$       then
$          read sys$command cont /prompt="Confirm (Y/N) [No] >" /end=tidyup
$          if (.not. cont) then goto tidyup
$       endif
$!
$       goto post_param_checks
$!
$!
$!
$!      !kob
$!
$post_param_checks:
$       if f$mode() .nes. "BATCH"
$       then
$          proc_params = "(''p1',''p2',''p3',''p4',''p5',''p6',''p7')"
$          submit%/log/keep/notify/noprint 'this_proc' /params='proc_params'
$          write sys$output "Submitted to batch"
$          goto tidyup
$       endif
$
$!Set & test scratch device OK
$       set noon
$       set proc/prio=4
$       set on
$       scratch_devdir = file_to_cvt_devdir
$       if dir_to_cvt_in .nes. scratch_devdir then -
                scratch_devdir = dir_to_cvt_in
$       
$       if "''file_cvt_debug'" .nes. "" then write sys$output -
        f$fao("Scratch Area is !AS", scratch_devdir)
$
$       err = "Parse error"
$       on error then goto scratch_space_check_error
$       disk=f$parse(scratch_devdir,,,"device")
$       err = "''disk' doesn't exist"
$       if (.not. f$getdvi(disk,"exists")) then goto sort_check_err
$       err = "''disk' isn't mounted"
$       if (.not. f$getdvi(disk,"mnt")) then goto sort_check_err
$       err = "''disk' isn't mounted"
$       disk = f$getdvi(disk,"fulldevnam")
$       scratch_devdir_free = f$getdvi(disk,"freeblocks")
$       if "''file_cvt_debug'" .nes. "" then write sys$output f$fao -
        ("Scratch device !AS contains !UL freeblock!%S", disk, -
        scratch_devdir_free)
$       goto post_scratch_space_check
$       
$scratch_space_check_error:
$       write sys$output f$fao("Error :!AS, STATUS:!AS", -
                err, $status)
$       goto tidyup
$post_scratch_space_check:
$
$       on control_y then goto finished
$       on error then goto finished
$l_check_privs:
$       priv = f$setprv (privs_required)
$       have_priv=f$priv (privs_required)
$       if have_priv then goto l_privs_ok
$       write sys$output f$fao -
        ("Error: Insufficient priviledges to Convert files. !%D",0)
$       goto tidyup
$l_privs_ok:
$       bell[0,8]=7                     !Set bell output value
$       define sys$login 'scratch_devdir'
$       define sys$scratch 'scratch_devdir'
$       on control_y then goto l_non_severe_error
$       on error then goto l_non_severe_error
$       fdl_dir = scratch_devdir 
$
$       define/nolog sortwork0 'sortwork_dir'
$       define/nolog  sortwork1 'sortwork_dir'
$
$       name = f$parse (file_to_cvt,,,"name")
$       cvt_op_file = f$parse(dir_to_cvt_in,file_to_cvt)
$
$       if "''file_cvt_debug'" .nes. ""
$       then
$          write sys$output f$fao ("FDL directory set to !AS", fdl_dir)
$          show logical sys$login
$          show logical sys$scratch
$          show logical sortwork0
$          show logical sortwork1
$          write sys$output f$fao("Filename of cvt file = !AS", name)
$          write sys$output f$fao("CVT o/p File = !AS", cvt_op_file)
$       endif
$!
$       write sys$output f$fao("!%D Processing file !AS.",0,file_to_cvt)
$       file_exists = f$search(file_to_cvt)
$       if "''file_exists'" .nes. "" then goto l_file_exists
$       write sys$output f$fao -
        ("!%D!AS!AS!AS Error Processing file !AS.",0,bell,bell,bell,file_to_cvt)
$       write sys$output "Error :File not found"
$       goto tidyup
$l_file_exists:
$       if backup_dir .nes. "NULL"
$       then
$          write sys$output f$fao -
                ("Copied !AS to !AS",file_to_cvt, backup_dir)
$          backup/log/noconfirm 'file_to_cvt'; 'backup_dir'/new/OWN=PAR
$       endif
$
$       write sys$output f$fao - 
                ("!%D Sort file Device !AS has !UL free block!%S", -
                        0, sortwork_dev, sortwork_dev_free)
$       write sys$output f$fao -
                ("!%D Output Device !AS has !UL free block!%S", -
                        0, dir_to_cvt_in_dev, dir_to_cvt_in_dev_free)
$       alq_file = f$file (file_to_cvt, "alq")
$       alq_file = f$int(alq_file)
$       write sys$output f$fao ("!%D !AS contains !UL block!%S", -
                0, file_to_cvt, alq_file)
$l_check_sort:    
$       if alq_file .lt. dir_to_cvt_in_dev_free then goto l_continue_cvt
$       write sys$output f$fao -
        ("!%D !AS!UL block!%S needed on !AS, !UL block!%S available", -
        0, bell,  alq_file, dir_to_cvt_in_dev, dir_to_cvt_in_dev_free)
$       goto tidyup
$l_continue_cvt:
$! determine if any fdl is to be used, or one created
$       if fdl_to_use .nes. "NULL"
$       then
$          write sys$output f$fao ("!%D Converting file !AS using !AS", -
                0,file_to_cvt, fdl_to_use)
$          fdl_filespec = fdl_to_use
$          goto convert_file
$       endif
$
$       write sys$output f$fao("!%D Analysing file !AS",0,file_to_cvt)
$       anal/rms/fdl 'file_to_cvt' /out='fdl_dir''name'.anal_fdl
$       write sys$output f$fao("!%D Optimising FDL file !AS",0,-
                fdl_dir+name+".anal_fdl")
$       edit%/fdl/noint/anal='fdl_dir''name'.anal_fdl -
                /out='fdl_dir''name'.fdl 'fdl_dir''name'.anal_fdl
$       write sys$output f$fao("!%D Converting file !AS",0,file_to_cvt)
$       on error then goto l_severe_error
$       on control_y then goto l_severe_error
$       fdl_filespec = "''fdl_dir'''name'.fdl"
$convert_file:
$       convert/stat/nosort/fast/fdl='fdl_filespec' -
                'file_to_cvt'; 'cvt_op_file'
$       write sys$output f$fao("!%D Convert Complete. Tidying up",0)
$!
$       if (.not. do_cvt_in_place) then -
                delete/log/noconfirm 'file_to_cvt';
$       wait 0:0:10     !0:2:0
$       if (.not. do_cvt_in_place) then -
                backup/log/noconfirm 'cvt_op_file' 'file_to_cvt'/new/own=par
$
$       if purge_files .eq. 0
$       then
$          if fdl_to_use .eqs. "NULL"
$          then
$             type 'fdl_dir''name'.anal_fdl;
$             delete%/log 'fdl_dir''name'.anal_fdl;
$             type 'fdl_filespec';
$             delete%/log 'fdl_filespec';
$          endif
$       else
$          purge_files = f$int(purge_files)
$          if fdl_to_use .eqs. "NULL"
$          then
$             purge%/log/keep='purge_files' 'fdl_dir''name'.anal_fdl
$             purge%/log/keep='purge_files' 'fdl_filespec'
$          endif
$          purge%/log/keep='purge_files' 'file_to_cvt'
$       endif
$       if (.not. do_cvt_in_place) then -
                delete%/log/noconfirm 'cvt_op_file' 
$l_exit:
$       goto tidyup
$l_abort_taken:
$       write sys$output f$fao -     
                ("!%D Abort taken before converting file !AS",0,file_to_cvt)
$       goto l_exit
$l_non_severe_error:
$       write sys$output f$fao -
        ("!/!%D!AS!AS!AS Non-Severe Error!/!_Aborted conversion of !AS!/", -
                        0,bell,bell,bell, file_to_cvt)
$       goto l_exit
$l_severe_error:
$       write sys$output f$fao -
        ("!/!%D!AS!AS!AS Severe Error!/!_Aborted conversion of !AS!/", -
                        0,bell,bell,bell, file_to_cvt)
$       dir/date/size 'file_to_cvt';*
$       type sys$input

        Contact SUPPORT to check that the original file has not been superseded
by a partially converted output file on the same directory as the
original.
Check the date on these files and remove the one just  created.
        
        YOU MUST ENSURE THAT ONLY THE ORIGINAL FILE IS LEFT.

$finished:
$tidyup:
$       write sys$output f$fao("Exiting... (!%D)", 0)
$       on error then continue
$       set def 'old_def
$       priv = f$setprv (priv)
$       define/nolog sys$login 'old_sylogin 
$       define/nolog sys$scratch 'old_scratch 
$       set mess/fac/id/sev/text
$       ver = f$ver(ver)
$       exit
$
