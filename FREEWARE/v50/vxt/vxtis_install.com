$ procedure_version = "VXT T2.1K-10"
$ procedure_date = "24-Sep-1997"
$ procedure = f$env("PROCEDURE")
$ say := write sys$output
$ say procedure
$ say ""
$ say "VXT Installation procedure for InfoServer network kits"
$ say "	Version = ",procedure_version
$ say "	   Date = ",procedure_date
$ say ""
$
$ if p1 .eqs. "HELP" then goto help
$ goto START
$ ! VXTIS_INSTALL.COM
$ !
$ !  Copyright © Digital Equipment Corp. 1994-1997. All Rights Reserved.
$ !
$ !  This software is furnished under a license and may be used and copied
$ !  only in accordance with the terms of such license and with the inclusion
$ !  of the above copyright notice.  This software or any other copies
$ !  thereof may not be provided or otherwise made available to any other
$ !  person.  No title to and ownership of the software is hereby transferred.
$ !
$ !  The information in this software is subject to change without notice and
$ !  should not be construed as a commitment by Digital Equipment Corporation.
$ !
$ !  Digital assumes no responsibility for the use or reliability of its
$ !  software on equipment that is not supplied by Digital.
$ !***************************************************************************
$ HELP:
$ create sys$output:
This procedure will initalize an InfoServer partition and restore a VMS
backup saveset to the partition. The partition can then be used to
install VXT Software on the InfoServer using the InfoServer command
UPDATE VXT DKm:PartitionName DKn:

USAGE: @VXTIS_INSTALL p1 p2 p3
    p1 = LASTport service name for output InfoServer partition
    p2 = VMS file specification for saveset containing InfoServer kit
    p3 = Volume label to be used for virtual disk (otherwise use file name)

    EXAMPLES:
	$ @VXTIS_INSTALL HELP
	$ @VXTIS_INSTALL IS1000_VXT VXTISCD021KT10.IMG VXT021KT10

EXAMPLE INSTALLATION LOG:
 1. On InfoServer IS1000
    InfoServer> CREATE PARTITION DK1:VXT021KT10 BLOCK 25000
    InfoServer> CREATE SERVICE IS1000_VXT021KT10 FOR DK1:VXT021KT10
    InfoServer> SAVE

 2. On OpenVMS
    $ @vxtis_install
    Installing VXT InfoServer Network kit
    LASTport service name? is1000_vxt021kt10<Return>

	Using service name IS1000_VXT021KT10
    %LADCP-I-BIND, service bound to logical unit DAD$IS1000_VXT021KT10 (_DAD29:)
    %DCL-I-ALLOC, _DAD29: allocated
    Source saveset [DKA200:[VXT]VXTISCD021KT10.IMG;1]? <Return>

	Using saveset DKA200:[VXT]VXTISCD021KT10.IMG;1

	Using volume label VXTISCD021KT10
	Starting Installation at 24-SEP-1997 17:54:16.30
	Initializing partition...
	Restoring kit to partition...
    %MOUNT-I-MOUNTED, VXTISCD021KT10     mounted on _DAD29:
    %BACKUP-S-CREATED, created DAD$IS1000_VXT021KT10:[000000]BACKUP.SYS;1
     . . . Log from backup . . .
    %BACKUP-S-CREATED, created DAD$IS1000_VXT021KT10:[]000000.DIR;1
	Dismounting DAD$IS1000_VXT021KT10...
	Deallocating DAD$IS1000_VXT021KT10...
	Unbinding service DAD$IS1000_VXT021KT10...
	Notifying VXT development team...
	Installation completed at  24-SEP-1997 17:56:59.02

 3. On InfoServer IS1000
    InfoServer> UPDATE VXT DK1:VXT021KT10 DK1:
    InfoServer> SAVE

$ exit 1
$!
$ START:
$ copy := copy
$ write := write
$ create := create
$ wait := wait
$ initialize := initialize
$ directory := directory
$ mount := mount
$ dismount := dismount
$!
$ service_name = p1
$ source_file = p2
$ volume_label = p3
$ mounted = 0
$ bound = 0
$ allocated = 0
$ status = 1
$
$ on w then goto abort
$ on c then goto abort
$!
$ ladcp := $ess$ladcp
$ error := call errmsg
$ tmp=f$ele(0,";",procedure)
$
$ say "	Use @",tmp," HELP for instructions."
$ say "	Press Ctrl/Z to quit."
$ say ""
$ say "Installing VXT InfoServer Network kit"
$
$ gosub CHECK_SERVICE_NAME
$ if .not status then goto abort
$!
$ gosub CHECK_SOURCE_FILE
$ if .not status then goto abort
$
$ gosub CHECK_VOLUME_LABEL
$ if .not status then goto abort
$
$ ! Start installation
$ say "    Starting Installation at ",f$time()
$ say "    Initializing partition..."
$ gosub INIT_VOLUME
$ if .not status then goto abort
$
$ say "    Restoring kit to partition..."
$ gosub RESTORE_KIT
$ if .not status then goto abort
$
$ gosub DISMOUNT_VOLUME
$
$ say "    Notifying VXT development team..."
$ gosub MAIL_DEVELOPERS
$!
$ say "    Installation completed at ",f$time()
$ exit 1
$!
$! Get LASTport service name for target partition
$!
$ CHECK_SERVICE_NAME:
$ service_name = f$edit(service_name,"COLLAPSE,UPCASE")
$ if service_name .nes. "" then goto GOT_SERVICE_NAME
$ read/end=abort/err=abort sys$command service_name /prompt="LASTport service name? "
$ goto CHECK_SERVICE_NAME
$ GOT_SERVICE_NAME:
$ say ""
$ say "    Using service name ",service_name
$ ladcp bind/write 'service_name'
$ bound = 1
$ bound_dev = f$trn("DAD$''service_name'")
$ allocate 'bound_dev'
$ allocated = 1
$ return
$!
$! Get filespec for saveset
$!
$ CHECK_SOURCE_FILE:
$ source_file = f$edit(source_file,"COMPRESS,UPCASE")
$ if source_file .nes. "" then goto GOT_SOURCE_FILE
$ default_source_file = f$search("*.IMG")
$ if default_source_file .eqs. "" then default_source_file = "*.img"
$ read/end=abort/err=abort sys$command source_file /prompt="Source saveset [''default_source_file']? "
$ if source_file .eqs. "" then source_file = default_source_file
$ goto CHECK_SOURCE_FILE
$ GOT_SOURCE_FILE:
$ source_filespec = f$search(source_file)
$ if source_filespec .nes. "" then goto SOURCE_FILE_OK
$ error "FILNOTFND, cannot find " source_file
$ default_source_file = source_file
$ source_file = ""
$ goto CHECK_SOURCE_FILE
$ SOURCE_FILE_OK:
$ x = f$loc("::",source_filespec)
$ if x .ne. f$len(source_filespec) then gosub CHECK_NODE_NAME
$ say ""
$ say "    Using saveset ",source_filespec
$ return
$!
$ CHECK_NODE_NAME:
$ y = f$loc("::",source_file)
$ if y .ne. f$len(source_file)
$ then
$    source_filespec = f$extract(0,y,source_file) + f$extract(x,255,source_filespec)
$    return
$ endif
$ 
$ return
$!
$! Get label for partition (up to 12 chars)
$!
$ CHECK_VOLUME_LABEL:
$ volume_label = f$edit(volume_label,"COLLAPSE,UPCASE")
$ if volume_label .nes. "" then goto GOT_VOLUME_LABEL
$ volume_label = f$parse(source_filespec,,,"NAME")
$ GOT_VOLUME_LABEL:
$ if f$len(volume_label) .gt. 12 then volume_label = volume_label - "_" - "_" - "_" - "_"
$ len = f$len(volume_label)
$ if len .le. 12 .and len .gt. 0 then goto VOLUME_LABEL_OK
$ error "BADLABEL, invalid label" volume_label
$ error "BADLABEL, volume label must be between 1 and 12 chars"
$ default_volume_label = f$extract(0,12,volume_label)
$ read/end=abort/err=abort sys$command volume_label /prompt="Volume label [''default_volume_label']? "
$ if volume_label .eqs. "" then volume_label = default_volume_label
$ goto CHECK_VOLUME_LABEL
$ VOLUME_LABEL_OK:
$ say ""
$ say "    Using volume label ",volume_label
$ return
$!
$ INIT_VOLUME:
$ init 'bound_dev' /header=100/prot=(s:RWED,g:rwed,w:rwed,o:rwed) 'volume_label'
$ return
$
$ RESTORE_KIT:
$ mount/foreign/write/noassist/nounload 'bound_dev'
$ mounted = 1
$ backup/image/noinit/log 'source_filespec'/save 'bound_dev'
$ return
$!
$! Notify development team of installation
$!
$ MAIL_DEVELOPERS:
$ VXT_MAIL_DEST = "7806::VXT_KIT"
$! VXT_MAIL_DEST = "7806::FAULKNER"
$ VMI$PRETTY := "VXT Software"
$ VXT_KITTYP = " InfoServer "+f$parse(source_filespec,,,"NAME")
$!
$ vmi_mail_prefix = ""
$ if f$search("SYS$SYSROOT:nm_mailshr.exe") .nes. "" then -
	vmi_mail_prefix = "NM%"
$ vmi_mail_file = "SYS$SCRATCH:VXTIS_INSTALL.TMP"
$ VMI$NO_ERROR := define/user sys$error nl:
$ VMI$NO_OUTPUT:= define/user sys$output nl:
$ copy/nolog nl: 'vmi_mail_file'
$ vmi_node = f$edit(f$getsyi("NODENAME"),"TRIM")
$ if vmi_node .eqs. "" then vmi_node = f$trn("SYS$NODE") - "_" - "::"
$ vmi_node = f$edit(vmi_node,"TRIM")
$ vmi_clustered = f$getsyi("CLUSTER_MEMBER")
$ vmi_cluster_alias = ""
$ if vmi_clustered then -
    vmi_cluster_alias = f$trn("SYS$CLUSTER_NODE") - "_" - "::"
$ if vmi_cluster_alias .eqs. "" then vmi_cluster_alias = "<NONE>"
$ set noon
$ close/nolog mf
$ open/append mf 'vmi_mail_file'
$ write mf "Date:            ",f$fao("!17%D",0)
$ write mf "Product:         ",VMI$PRETTY,",",vxt_kittyp
$ write mf "Installation procedure: ",procedure
$ write mf "Procedure version: ",procedure_version,", ",procedure_date
$ write mf "Installed by:    ",f$getjpi(0,"USERNAME")
$ write mf "Node:            ",vmi_node
$ write mf "CPU Type:        ",f$getsyi("NODE_HWTYPE")
$ write mf "VMS Version:     ",f$getsyi("VERSION")
$ write mf "Clustered:       ",vmi_clustered
$ if vmi_clustered then -
     write mf "Cluster Alias:   ",vmi_cluster_alias
$ write mf "Network Information:"
$ assign/user mf: sys$output
$ PHASE_V = 0
$ set noon
$ PHASE_V = (f$integer(f$extract(0,4,f$getsyi("DECNET_VERSION"))) .eq. 5)
$ set on
$ if .not. PHASE_V
$ then
$    mcr ncp show exec
$ else
$    mcr ncl show node 0
$ endif
$ if .not. vmi_clustered then goto CLOSE_MF
$ create sys$scratch:vmi_show_cluster_commands.ini
ADD HW_TYPE
$ assign/user sys$scratch:vmi_show_cluster_commands.ini show_cluster$init
$ show cluster/output=sys$scratch:cluster.tmp
$ write mf ""
$
$ VMI_CLU_LOOP:
$ open/read/error=CLOSE_MF cf sys$scratch:cluster.tmp
$ read/end=CLOSE_CF/err=CLOSE_CF cf line
$ line = f$edit(line,"trim")
$ if line .nes. "" then write mf line
$ goto VMI_CLU_LOOP
$
$ CLOSE_CF:
$ close/nolog cf
$
$ CLOSE_MF:
$ write mf ""
$ close/nolog mf
$ MAIL_IT:
$ set noon
$ VMI$NO_OUTPUT
$ VMI$NO_ERROR
$ mail/noself/subj="''VMI$PRETTY',''vxt_kittyp' INSTALLATION" 'vmi_mail_file' -
    'vmi_mail_prefix''vxt_mail_dest'
$ set on
$
$ return
$!
$ ABORT:
$ gosub DISMOUNT_VOLUME
$ error "ABORT, Installation aborted"
$ exit
$
$ DISMOUNT_VOLUME:
$ if mounted
$ then
$    say "    Dismounting DAD$",service_name,"..."
$    if f$getdvi(bound_dev,"exists") then -
	dismount/nounload 'bound_dev'
$    mounted = 0
$ endif
$ if allocated
$ then
$    say "    Deallocating DAD$",service_name,"..."
$    if f$getdvi(bound_dev,"exists") then -
	deallocate 'bound_dev'
$    allocated = 0
$ endif
$ if bound
$ then
$    say "    Unbinding service DAD$",service_name,"..."
$    if f$getdvi(bound_dev,"exists") then -
	ladcp unbind 'bound_dev'
$    bound = 0
$ endif
$ return
$!
$ ERRMSG: subroutine
$ a = ""
$ if p2 .eqs. "" then p2 := a
$ write sys$error "%VXTIS_INSTALL-E-",p1,'p2
$ endsubroutine
