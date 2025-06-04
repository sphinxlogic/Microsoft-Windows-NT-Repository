$! From: NCCODE::SSMITH "Sheldon E. Smith, Jr. DTN 442-2254  18-May-1989 
$! 1615"  18-MAY-1989 17:30:32.80
$! To:	METOO::TOOLSHED
$! Subj:	DELIVER_MAKE.COM (v1.4 rev. -- Extract /NoHeader)
$ type = "type"
$ type sys$input:
*****
***** After this procedure completes, build DELIVER by
***** $ @BUILD_DELIVER.COM
*****
....................... Cut between dotted lines and save ......................
$!..............................................................................
$! VAX/VMS archive file created by VMS_SHAR V-5.01 01-Oct-1987
$! which was written by Michael Bednarek (U3369429@ucsvc.dn.mu.oz.au)
$! To unpack, simply save and execute (@) this file.
$!
$! This archive was created by SSMITH
$! on Thursday 18-MAY-1989 16:04:05.79
$!
$! It contains the following 19 files:
$! BATCH_LOG.PAS BUILD_DELIVER.COM DELIVER$STARTUP.COM DELIVER.PAS DELIVER.RNO
$! DELIVER_ERR.MSG DELIVER_MYMAIL.COM DELIVER_REPLY.COM DELIVER_REPLY.TXT
$! FORWARD.COM FORWARD.DELIVERY MAILSHR.MAR MAILSHR.OPT MAIL_BATCH.COM
$! MAIL_BATCH_CLEANUP.COM MAKE_MAIL_DELIVERY.COM NO_BATCH_LOG.PAS TRIP.COM
$! TRIP.DELIVERY
$!==============================================================================
$ Set Symbol/Scope=(NoLocal,NoGlobal)
$ Version=F$GetSYI("VERSION") ! See what VMS version we have here:
$ If Version.ges."V4.4" then goto Version_OK
$ Write SYS$Output "Sorry, you are running VMS ",Version, -
                ", but this procedure requires V4.4 or higher."
$ Exit 44
$Version_OK: CR[0,8]=13
$ Pass_or_Failed="failed!,passed."
$ Goto Start
$Convert_File:
$ Read/Time_Out=0/Error=No_Error1/Prompt="creating ''File_is'" SYS$Command ddd
$No_Error1: Define/User_Mode SYS$Output NL:
$ Edit/TPU/NoSection/NoDisplay/Command=SYS$Input/Output='File_is' -
        VMS_SHAR_DUMMY.DUMMY
f:=Get_Info(Command_Line,"File_Name");b:=Create_Buffer("",f);
o:=Get_Info(Command_Line,"Output_File");Set(Output_File,b,o);
Position(Beginning_of(b));Loop x:=Erase_Character(1);Loop ExitIf x<>"V";
Move_Vertical(1);x:=Erase_Character(1);Append_Line;
Move_Horizontal(-Current_Offset);EndLoop;Move_Vertical(1);
ExitIf Mark(None)=End_of(b) EndLoop;Position(Beginning_of(b));Loop
x:=Search("`",Forward,Exact);ExitIf x=0;Position(x);Erase_Character(1);
If Current_Character='`' then Move_Horizontal(1);else
Copy_Text(ASCII(INT(Erase_Character(3))));EndIf;EndLoop;Exit;
$ Delete VMS_SHAR_DUMMY.DUMMY;*
$ Checksum 'File_is
$ Success=F$Element(Check_Sum_is.eq.CHECKSUM$CHECKSUM,",",Pass_or_Failed)+CR
$ Read/Time_Out=0/Error=No_Error2/Prompt=" CHECKSUM ''Success'" SYS$Command ddd
$No_Error2: Return
$Start:
$ File_is="BATCH_LOG.PAS"
$ Check_Sum_is=671627300
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
Xvar
X  batch_log   : [readonly] string := '';
$ GoSub Convert_File
$ File_is="BUILD_DELIVER.COM"
$ Check_Sum_is=1074419731
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$`009SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
X`009  + F$VERIFY(0'BUILD_DELIVER_VERIFY' .GT. 0)`009`009`009`009
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$`009ON WARNING THEN GOTO ABNORMAL_EXIT
X$!
X$`009VERSION = "V1.0-0"`009`009`009`009`009`009
X$`009GOTO END_OF_HEADER_COMMENTS
X$ ! BUILD.COM - Builds and installs DELIVER on your System
X$ ! Andy Leslie, May 88
X$ ! 
X$END_OF_HEADER_COMMENTS:
X$!
X$!
X$! Reset commands that the user may have redefined.
X$!
X$`009APPEND`009`009= "APPEND"
X$`009CLOSE`009`009= "CLOSE"
X$`009CONVERT`009`009= "CONVERT"
X$`009COPY`009`009= "COPY"
X$`009CREATE`009`009= "CREATE"
X$`009DEASSIGN`009= "DEASSIGN"
X$`009DEFINE`009`009= "DEFINE"
X$`009DELETE`009`009= "DELETE"
X$`009EDIT`009`009= "EDIT"
X$`009EXIT`009`009= "EXIT"
X$`009GOTO`009`009= "GOTO"
X$`009IF`009`009= "IF"
X$`009LINK`009`009= "LINK"
X$`009MAIL`009`009= "MAIL"
X$`009ON`009`009= "ON"
X$`009OPEN`009`009= "OPEN"
X$`009PASCAL`009`009= "PASCAL"
X$`009PURGE`009`009= "PURGE"
X$`009READ`009`009= "READ"
X$`009RENAME`009`009= "RENAME"
X$`009SEARCH`009`009= "SEARCH"
X$`009SET`009`009= "SET"
X$`009SORT`009`009= "SORT"
X$`009SPAWN`009`009= "SPAWN"
X$`009SUBMIT`009`009= "SUBMIT"
X$`009TYPE`009`009= "TYPE"
X$`009WAIT`009`009= "WAIT"
X$`009WRITE`009`009= "WRITE"
X$ write sys$output "Building DELIVER_ERR.EXE and moving it to SYS$MESSAGE"
X$ message deliver_err
X$ set NoOn
V$ write sys$output "Now linking DELIVER_ERR messages. Ignore a LINK message her
Xe"
X$ link deliver_err /executable=sys$common:[SYSMSG]DELIVER_ERR
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$`009ON WARNING THEN GOTO ABNORMAL_EXIT
X$ set file sys$common:[SYSMSG]DELIVER_ERR.EXE/prot=(S:WRED,W:RE)
X$ purge deliver_err.*
X$ write sys$output "Building MAILSHR"
X$ macro mailshr
X$ write sys$output "Compiling DELIVER.PAS"
X$ define BATCH_LOG_DEF NO_BATCH_LOG.PAS /user
X$ pascal deliver
X$ ! image has to be linked /NOTRACEBACK if image is to be installed with privs
X$ write sys$output "Linking/creating DELIVER_MAILSHR.EXE"
X$ link/notraceback/share=sys$common:[syslib]deliver_mailshr.exe -
X  deliver,deliver_err,mailshr,mailshr/opt
X$ write sys$output "Compiling DELIVER.PAS"
X$ define BATCH_LOG_DEF BATCH_LOG.PAS /user
X$ pascal deliver
X$ pu deliver.*
X$ ! image has to be linked /NOTRACEBACK if image is to be installed with privs
X$ write sys$output "Linking/creating DELIVER-LOG_MAILSHR.EXE"
X$ link/notraceback/share=sys$common:[syslib]deliver-log_mailshr.exe -
X  deliver,deliver_err,mailshr,mailshr/opt
X$ set file sys$share:deliver*_mailshr.exe /prot=(S:WRED,W:RE)
X$ write sys$output "Creating DELIVER.MEM manual"
X$ RUNOFF/OUT=DELIVER.MEM/VAR=MANUAL DELIVER.RNO
X$ Write sys$output "Move the DELIVER$STARTUP.COM procedure to SYS$MANAGER"
X$ copy DELIVER$STARTUP.COM sys$common:<sysmgr>
X$ Write sys$output "Now execute the DELIVER$STARTUP.COM procedure"
X$ write sys$output " To complete installation"
X$ @Sys$Manager:DELIVER$STARTUP.com
X$ type sys$input:
X
X    The installation should now be complete; DELIVER is ready to run.
X
X    1a) The system manager must add the line
X`009$ @SYS$MANAGER:DELIVER$STARTUP.COM
X    to the site-specific startup file.
X
X    1b) If DELIVER is being installed on a VAXcluster, it is necessary to
X    invoke the DELIVER$STARTUP command procedure on each node in the cluster.
X
X    2) Each user must set their forwarding address to their own user-name:
X`009MAIL> SET FORWARD DELIVER%user-name
X
X$!
X$! Normal exit.
X$!
X$NORMAL_EXIT:
X$`009STATUS = $STATUS`009`009`009!save success status
X$`009GOTO EXIT
X$!            
X$! Abnormal exit.
X$!
X$ABNORMAL_EXIT:
X$`009STATUS = $STATUS`009`009`009!save failure status
X$`009IF STATUS THEN STATUS = "%X0800002A"`009!force error if neccessary
X$`009GOTO EXIT
X$!
X$! Exit.
X$!
X$EXIT:
X$`009EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
$ GoSub Convert_File
$ File_is="DELIVER$STARTUP.COM"
$ Check_Sum_is=224203798
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$ ! DELIVER$STARTUP.COM - place this in SYS$MANAGER:
X$ !  Put the command @SYS$MANAGER:DELIVER$STARTUP in your SYSTARTUP.COM
X$ ! Andy Leslie, 13-4-88
X$ Set NoOn
X$`009SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
X`009  + F$VERIFY(0'BUILD_DELIVER_VERIFY' .GT. 0)`009`009`009`009
X$ ! If DELIVER_MAILSHR has not been INSTALLed previously
X$ !    an error message will be generated by the /DELETE line. Don't worry.
X$ INSTALL 
Vreplace SYS$LIBRARY:DELIVER_MAILSHR.EXE/OPEN/HEADER/PROT/SHARE/PRIV=(bypass,SYS
XPRV,OPER,WORLD,NETMBX,CMKRNL)
Vreplace SYS$LIBRARY:DELIVER-log_MAILSHR.EXE/OPEN/HEADER/PROT/SHARE/PRIV=(bypass
X,SYSPRV,OPER,WORLD,NETMBX,CMKRNL)
Vreplace SYS$SYSTEM:MAIL/OPEN/HEADER/SHARE/PRIV=(bypass,SYSPRV,OPER,WORLD,NETMBX
X,CMKRNL)
X$ !`009Check VMS version
X$ VMS_VERSION = F$GETSYI("VERSION")
X$ ! `009Check the major version number only
X$ SYS_VERSION = F$EXTRACT(1,1,VMS_VERSION)
X$ IF  (SYS_VERSION .NE. 5) THEN goto exeunt
X$ ! This stuff only needed for VMS V5.0-0 and above
X$ INSTALL 
Vreplace SYS$SYSTEM:MAIL_SERVER/HEADER/OPEN/SHARE/PRIV=(bypass,SYSPRV,OPER,WORLD
X,NETMBX,CMKRNL)
Vreplace sys$system:decw$mail/open/header/share/priv=(bypass,sysprv,oper,world,n
Xetmbx,cmkrnl)
Xreplace sys$share:decw$mailshr
Xreplace sys$message:decw$mail_messages
X$ Exeunt:
X$`009EXIT 1 + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
$ GoSub Convert_File
$ File_is="DELIVER.PAS"
$ Check_Sum_is=1866425799
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X(* DELIVER.PAS - General MAIL delivery manager for VMS MAIL.
X
XModification History
X 
X   Written by Ned Freed, 23-Sep-1985, modified 30-Apr-1986.
X   Mail dispatch interface originally written by Kevin Carosso.
X   Some modifications by Sheldon Smith, December 1986.
X   Rewrite by Ned Freed to use new $GETUAI system service, 15-Dec-1986.
X   This change requires the use of VMS 4.4 and Pascal 3.4.
X            
X   Modified by Andy Leslie to run under VMS X5.0-3N2 21-Dec-1987
X   - Changed destination to 0::_username - _username has the "_"
X     stripped by VMS otherwise. Also, V5 changes to MAIL meant
X     I had to move the "! sent to ..." string to the subject field.
X    
X   Modified by Andy Leslie to run under VMS V5.0-1 20-Jul-1988
X   - Re-ordered $SNOJBC item list so that Username is first. This
X     is to allow for a feature of the VMS Job Controller in 5.0-1
X     Built and tested on VMS V5.0-1 using a FT version of PASCAL and 
X     PASSTR.  
X
X   Modified by Shel Smith to run under V5.1-T2 23-DEC-1988
X   - Re-changed destination from 0::_username to %_username.
X
XCaveat
X   It is believed that it will work on versions of VMS prior to VMS V5, 
X   but this is untested by me, AJL 23-7-88
X
XWhat is DELIVER?
X     
X   DELIVER provides a general-purpose MAIL delivery manager similar
X   to the MMDF-II MAILDELIVERY system. DELIVER makes it possible
X   for users to set up a file containing screening information that
X   automatically deals with each incoming message. Actions may be
X   selectively taken by DELIVER based on information in the From:,
X   To: and Subject: fields of the message header.
X 
X   DELIVER operates as a foreign mail interface to VMS MAIL and is
X   invoked with a call to LIB$FIND_IMAGE_SYMBOL in MAIL. The shareable
X   image containing this code should be placed in SYS$LIBRARY as the file
X   DELIVER_MAILSHR.EXE).
X 
X   Users may activate DELIVER by setting their forwarding address
X   to DELIVER%username, where "username" is the user's own user name.
X   In order for DELIVER to perform any useful function a file called
X   MAIL.DELIVERY must be present in the user's default login directory.
X 
X   DELIVER's operation is only meaningful in outgoing mode; however, rather
X   than waste a possible incoming MAIL interface, DELIVER implements a
X   rudimentary mail posting mechanism on the incoming side that can be used
X   to send messages contained in data files. 
X
X   If privileges are not granted by you using the DELIVER$STARTUP.COM
X   provided, SYSNAM and CMKRNL will be required to run DELIVER in this mode. 
X   You may wish to do this  since it is possible to forge From: 
X   addresses using this mechanism.
X 
X  Note:
X    The foreign protocol interface within MAIL is undocumented at
X    this time.  It may change without notice in a future release
X    of VMS. Indeed it already has, once.
X 
X    The information necessary to write this code comes from the MAIL
X    source on the VMS microfiche.  The most useful information is the
X    routine NETJOB in module MAIL$MAIL (230-E2), which handles incoming
X    foreign mail, and the various routines in module NETSUBS (230-N11),
X    most of which deal with outgoing foreign mail.
X
XGotchas
X
X    File Protections:
X    
X    It seems that SYS$SHARE:PASRTL.EXE must be (W:RE)
X                  SYS$SYSTEM:VMSMAIL_PROFILE.DATA must be (W:RW) 
X                  SYS$SYSTEM:*MAIL*.EXE must be (W:RE) 
X                  SYS$MESSAGE:*.MSG must be (W:RE)
X    `009`009  SYS$SYSTEM:SEARCH.EXE must be (W:RE)
X    
X    DON'T forget to Edit MAIL.DELIVERY and the DELIVER_REPLY.COM files
X    to suit you or it'll all fall apart.
X  
X    Don't forget to create the DELIVER$DIR directory and put DELIVER_REPLY.COM
X    plus DELIVER_REPLY.TXT there. DELIVER.HISTORY will be created there too.
X    
X    Finally, if all else fails, SET AUDIT/ALARM/ENABLE=ALL and watch
X    the messages fly past! Sooner or later you'll see what the problems
X    are.
X
X*)
X 
X[inherit ('SYS$LIBRARY:STARLET')] module deliver (output);
X 
Xconst
X  (* Debugging control flags *)
X  DEBUG_IN  = false;            (* Debug messages produced by receive code. *)
X  DEBUG_OUT = false;            (* Debug messages produced by send code.    *)
X 
X  LNK_C_OUT_CONNECT  = 0;       (* MAIL protocol link actions.           *)
X  LNK_C_OUT_SENDER   = 1;       (* These are defined in MAILSHR.MAR      *)
X  LNK_C_OUT_CKUSER   = 2;       (* but because we cannot have external   *)
X  LNK_C_OUT_TO       = 3;       (* constants in Pascal, they are         *)
X  LNK_C_OUT_SUBJ     = 4;       (* redefined here.                       *)
X  LNK_C_OUT_FILE     = 5;
X  LNK_C_OUT_CKSEND   = 6;
X  LNK_C_OUT_DEACCESS = 7;
X 
X  LNK_C_IN_CONNECT = 8;
X  LNK_C_IN_SENDER  = 9;
X  LNK_C_IN_CKUSER  = 10;
X  LNK_C_IN_TO      = 11;
X  LNK_C_IN_SUBJ    = 12;
X  LNK_C_IN_FILE    = 13;
X 
X  LNK_C_IO_READ  = 14;
X  LNK_C_IO_WRITE = 15;
X 
X  parameter_size     = 512;     (* Size of a single parameter in the
X                                   MAIL.DELIVERY file. This is also the
X                                   maximum size of lines read from any file. *)
X  max_parameters     = 7;       (* Maximum number of parameters that
X                                   can appear on a line in a
X                                   MAIL.DELIVERY file *)
X  min_parameters     = 5;       (* Minimum number of parameters that
X                                   can appear on a line in a
X                                   MAIL.DELIVERY file *)
X  from_parameter     = 1;       (* Position of the From: parameter *)
X  to_parameter       = 2;       (* Position of the To: parameter *)
X  subject_parameter  = 3;       (* Position of the Subject: parameter *)
X  decision_parameter = 4;       (* Position of the decision parameter *)
X  action_parameter   = 5;       (* Position of the action parameter *)
X  argument_parameter = 6;       (* Position of the argument parameter *)
X 
X  stack_size = 10;              (* State mach. stack for messages from MAIL *)
X 
Xtype
X  string = varying [parameter_size] of char;
X 
X  (* A string descriptor type used to handle the descriptors MAIL hands to
X     DELIVER. *)
X  longest_possible_string = packed array [1..65536] of char;
X  string_descriptor = record
X                        length : [word] 0..65535;
X                        dclass, dtype : [byte] 0..255;
X                        address : ^longest_possible_string;
X                      end;
X 
X  (* Storage for a single line of MAIL.DELIVERY file information *)
X  parameter_block_ptr = ^parameter_block;
X  parameter_block = record
X                      parameters  : array [1..max_parameters] of string;
X                      next        : parameter_block_ptr;
X                      any_from    : boolean;
X                      any_to      : boolean;
X                      any_subject : boolean;
X                    end; (* parameter_block record *)
X 
X  account_name = packed array [1..8] of char;
X 
X  (* Storage for information about a single recipient *)
X  user_block_ptr = ^user_block;
X  user_block = record
X                 username    : varying [12] of char;
X                 account     : account_name;
X                 directory   : string;
X                 uic         : unsigned;
X                 rules_list  : parameter_block_ptr;
X                 next        : user_block_ptr;
X                 copyname    : varying [29] of char;
X               end; (* user_block record *)
X 
X  (* Possible reasons why MAIL_IO_WRITE will be called. *)
X  write_states = (bad_msg, user_check, delivery_check, error_msg);
X 
X  (* A stack structure to store information about pending calls to
X     MAIL_IO_WRITE. *)
X  write_state_stack = record
X                        top : integer;
X                        store : array [1..stack_size] of write_states;
X                      end; (* write_state_check record *)
X 
X  (* Record for VMS item lists. *)
X  item = record
X           len    : [word] 0..65535;
X           code   : [word] 0..65535;
X           addr   : [long] integer;
X           rlen   : [long] integer;
X         end; (* item record *)
X 
Xvar
X  batch_queue : [readonly] string := 'MAIL$BATCH';
X  system_batch_queue : [readonly] string := 'SYS$BATCH';
X%include 'BATCH_LOG_DEF /list'
X 
X  (* Storage for message header information on the outgoing side *)
X  tostring, fromstring, subjectstring : [static] string;
X 
X  (* List of active recipients and associated information *)
X  user_list, user_list_last : [static] user_block_ptr;
X  user_count : [static] integer := 0;
X 
X  from_owner : [static] boolean;
X 
X  (* Storage for accumulated To: line for incoming messages. *)
X 
X  toline : [static] string;
X 
X  (* The state machine for MAIL status information. *)
X 
X  write_recv_states  : write_state_stack;
X  last_error : integer;
X 
X  (* Error message codes defined in DELIVER_ERR.MSG *)
X  DELIVER__CANACCUAF, DELIVER__NOSUCHUSER, DELIVER__NAMETOOLONG,
X  DELIVER__NODEFAULTDIR, DELIVER__TOOMANYPARAMS, DELIVER__TOOFEWPARAMS,
X  DELIVER__NOMDFILE, DELIVER__MDIGNORED, DELIVER__NORULES, DELIVER__MESREAERR,
V  DELIVER__GETFILERR, DELIVER__MESWRTERR, DELIVER__INTSTKOVR, DELIVER__STKEMPTY
X,
X  DELIVER__BADSTKELE, DELIVER__MESOPNERR, DELIVER__MSGWRTERR,
X  DELIVER__MSGREAERR, DELIVER__USERNOEXIST : [external, value] integer;
X 
X(* Routine to signal errors *)
X 
Xprocedure LIB$SIGNAL (%IMMED stat : [list, unsafe] integer); extern;
X 
X(* Routine to read command line arguments *)
X 
Xfunction CLI$GET_VALUE (name : varying [max1] of char;
X  var val : varying [max2] of char) : integer; extern;
X 
X(* Routine to get symbol values *)
X 
Xfunction LIB$GET_SYMBOL (name : varying [max1] of char;
X  var result : varying [max2] of char) : integer; extern;
X 
X(* Routine to set symbol values *)
X 
Xfunction LIB$SET_SYMBOL (name : varying [max1] of char;
X  svalue : varying [max2] of char) : integer; extern;
X 
X(* create_with_SYSPRV is a Pascal user-action routine for OPEN statements.
X   It enables SYSPRV while doing certain OPEN's so we can write files into
X   user directories. *)
X 
Xfunction create_with_SYSPRV (var fab : FAB$TYPE;
X                             var rab : RAB$TYPE;
X                             var fil : text) : integer;
Xvar
X  stat : integer; ppriv, priv : [quad] array [0..1] of unsigned;
X 
Xbegin (* create_with_SYSPRV *)
X  priv[0] := PRV$M_SYSPRV; priv[1] := 0;
X  stat := $SETPRV (ENBFLG := 1, PRVADR := priv, PRMFLG := 0, PRVPRV := ppriv);
X  if odd (stat) then begin
X    FAB.FAB$V_LNM_MODE := PSL$C_EXEC; stat := $CREATE (FAB);
X    if odd (stat) then stat := $CONNECT (RAB);
X  end;
X  priv[0] := uand (priv[0], unot (ppriv[0]));
X  priv[1] := uand (priv[1], unot (ppriv[1]));
X  $SETPRV (ENBFLG := 0, PRVADR := priv, PRMFLG := 0);
X  create_with_SYSPRV := stat;
Xend; (* create_with_SYSPRV *)
X 
X(* open_with_SYSPRV is a Pascal user-action routine for OPEN statements.
X   It enables SYSPRV while doing certain OPEN's so we can read system
X   files. *)
X 
Xfunction open_with_SYSPRV (var fab : FAB$TYPE;
X                           var rab : RAB$TYPE;
X                           var fil : text) : integer;
Xvar
X  stat : integer; ppriv, priv : [quad] array [0..1] of unsigned;
X 
Xbegin (* open_with_SYSPRV *)
X  priv[0] := PRV$M_SYSPRV; priv[1] := 0;
X  stat := $SETPRV (ENBFLG := 1, PRVADR := priv, PRMFLG := 0, PRVPRV := ppriv);
X  if odd (stat) then begin
X    FAB.FAB$V_LNM_MODE := PSL$C_EXEC; stat := $OPEN (FAB);
X    if odd (stat) then stat := $CONNECT (RAB);
X  end;
X  priv[0] := uand (priv[0], unot (ppriv[0]));
X  priv[1] := uand (priv[1], unot (ppriv[1]));
X  $SETPRV (ENBFLG := 0, PRVADR := priv, PRMFLG := 0);
X  open_with_SYSPRV := stat;
Xend; (* open_with_SYSPRV *)
X 
X(* find_user_directory locates a user in the system authorization file
X   and returns his or her default login directory (which is where a
X   MAIL.DELIVERY file must reside). find_user_directory also returns
X   the user's UIC and account since this information will be needed
X   for creating the delivery batch job. *)
X 
Xfunction find_user_directory (username : varying [l1] of char;
X  var user_directory : string; var user_uic : unsigned;
X  var user_account : account_name) : boolean;
X 
Xvar
X  device_with_prefix : packed array [1..16] of char;
X  directory_with_prefix : packed array [1..64] of char;
X  items : array [1..5] of item; stat : integer;
X  ppriv, priv : [quad] array [0..1] of unsigned;
X 
Xbegin (* find_user_directory *)
X  if DEBUG_OUT then writeln ('find_user_directory called.');
X  find_user_directory := false;
X  if length (username) > 12 then
X    LIB$SIGNAL (DELIVER__NAMETOOLONG, 2,
X                username.length, iaddress (username.body))
X  else begin
X    with items[1] do begin
X      len := 4; code := UAI$_UIC; addr := iaddress (user_uic); rlen := 0;
X    end; (* with *)
X    with items[2] do begin
X      len := 8; code := UAI$_ACCOUNT;
X      addr := iaddress (user_account); rlen := 0;
X    end; (* with *)
X    with items[3] do begin
X      len := 16; code := UAI$_DEFDEV;
X      addr := iaddress (device_with_prefix); rlen := 0;
X    end; (* with *)
X    with items[4] do begin
X      len := 64; code := UAI$_DEFDIR;
X      addr := iaddress (directory_with_prefix); rlen := 0;
X    end; (* with *)
X    with items[5] do begin
X      len := 0; code := 0; addr := 0; rlen := 0;
X    end; (* with *)
X    (* Enable SYSPRV to check for valid user recipient-name. *)
X    priv[0] := PRV$M_SYSPRV; priv[1] := 0;
X    $SETPRV (ENBFLG := 1, PRVADR := priv, PRMFLG := 0, PRVPRV := ppriv);
X    stat := $GETUAI (usrnam := username, itmlst := items);
X    if stat = SS$_NOPRIV then LIB$SIGNAL (DELIVER__CANACCUAF)
X    else if not odd (stat) then
X      LIB$SIGNAL (DELIVER__NOSUCHUSER, 2,
X                  username.length, iaddress (username.body))
X    else begin
X      user_directory := substr (device_with_prefix, 2,
X                                ord (device_with_prefix[1])) +
X                        substr (directory_with_prefix, 2,
X                                ord (directory_with_prefix[1]));
X      if DEBUG_OUT then writeln ('  Default directory: "',
X                                 user_directory, '".');
X      if DEBUG_OUT then writeln ('  Account: "', user_account, '".');
X      if DEBUG_OUT then writeln ('  UIC: ', hex (user_uic, 8, 8), '.');
X      if length (user_directory) <= 0 then
X        LIB$SIGNAL (DELIVER__NODEFAULTDIR, 2,
X                    username.length, iaddress (username.body))
X      else find_user_directory := true;
X      (* Disable and reestablish former privs. *)
X      priv[0] := uand (priv[0], unot (ppriv[0]));
X      priv[1] := uand (priv[1], unot (ppriv[1]));
X      $SETPRV (ENBFLG := 0, PRVADR := priv, PRMFLG := 0);
X    end;
X  end;
Xend; (* find_user_directory *)
X 
X(* copy_descr_to_string copies a MAIL string (passed by descriptor) into
X   a Pascal VARYING string. *)
X 
Xprocedure copy_descr_to_string (
X  var mail_string : string_descriptor;
X  var out_string : string; DEBUG_ON : boolean);
X 
Xvar
X  index : integer;
X 
Xbegin (* copy_descr_to_string *)
X  if DEBUG_ON then writeln ('copy_descr_to_string called.');
X  out_string := '';
X  for index := 1 to mail_string.length do
X    out_string := out_string + mail_string.address^[index];
X  if DEBUG_ON then writeln ('  String copied: "', out_string, '".');
Xend; (* copy_descr_to_string *)
X 
X(* copy_string_to_descr copies a Pascal VARYING string into a MAIL string
X   (passed by descriptor). *)
X 
Xprocedure copy_string_to_descr (
X  var in_string : string;
X  var mail_string : string_descriptor; DEBUG_ON : boolean);
X 
X  [asynchronous, unbound, external (LIB$SCOPY_DXDX)]
X  function copy_string (var src : varying [max1] of char;
X    var dst : string_descriptor) : integer; extern;
X 
Xbegin (* copy_string_to_descr *)
X  if DEBUG_ON then writeln ('copy_string_to_descr called.');
X  copy_string (in_string, mail_string);
X  if DEBUG_ON then writeln ('  String copied: "', in_string, '".');
Xend; (* copy_string_to_descr *)
X 
X(* charupper is a simple function to convert characters to upper case.
X   The full DEC Multinational Character Set is accomodated. *)
X 
Xfunction charupper (ch : char) : char;
X 
Xbegin (* charupper *)
X  if (ch >= 'a') and (ch <= 'z') then
X    charupper := chr (ord (ch) + (ord ('A') - ord ('a')))
X  else if (ord (ch) >= 224) and (ord (ch) <= 253) then
X    charupper := chr (ord (ch) + (192 - 224))
X  else charupper := ch;
Xend; (* charupper *)
X 
X(* dispose_rules_list disposes of heap storage associated with
X   a list of parameter blocks. *)
X 
Xprocedure dispose_rules_list (var rules_list : parameter_block_ptr);
X 
Xvar
X  temp_list : parameter_block_ptr;
X 
Xbegin (* dispose_rules_list *)
X  while rules_list <> nil do begin
X    temp_list := rules_list; rules_list := rules_list^.next;
X    dispose (temp_list);
X  end; (* while rules_list <> nil *)
Xend; (* dispose_rules_list *)
X 
X(* dispose_user_list disposes of heap storage associated with
X   a list of user name blocks. *)
X 
Xprocedure dispose_user_list (var user_list : user_block_ptr);
X 
Xvar
X  temp_list : user_block_ptr;
X 
Xbegin (* dispose_user_list *)
X  while user_list <> nil do begin
X    temp_list := user_list; user_list := user_list^.next;
X    dispose_rules_list (temp_list^.rules_list); dispose (temp_list);
X  end; (* while user_list <> nil *)
Xend; (* dispose_user_list *)
X 
X(* read_maildelivery_file reads the contents of a MAIL.DELIVERY file
X   and creates a rules_list structure. Any errors are signalled
X   by returning FALSE. *)
X 
Xfunction read_maildelivery_file (var dfile : text;
X  var rules_list : parameter_block_ptr) : boolean;
X 
Xlabel
X  99;
X 
Xvar
X  current, last : parameter_block_ptr; quoted : boolean;
X  pindex, lindex, rindex, lcount : integer; line : string;
X 
X  procedure addch (ch : char);
X 
X  label
X    88;
X 
X  var
X    cindex : integer;
X 
X  begin (* addch *)
X    if pindex > max_parameters then begin
X      if FROM_OWNER then LIB$SIGNAL (DELIVER__TOOMANYPARAMS, 1, lcount);
X      goto 99;
X    end;
X    if current = nil then if (ch = '!') or (ch = ';') then begin
X      if DEBUG_OUT then writeln ('  Skipping comment line.');
X      lindex := length (line); goto 88;
X    end else begin
X      new (current);
X      with current^ do begin
X        for cindex := 1 to max_parameters do parameters[cindex] := '';
X        next := nil;
X      end; (* with current^ *)
X      rindex := rindex + 1;
X      if DEBUG_OUT then writeln (' Rule #', rindex:0, '.');
X      if last = nil then begin
X        last := current; rules_list := current;
X      end else begin
X        last^.next := current; last := current;
X      end;
X    end;
X    current^.parameters[pindex] := current^.parameters[pindex] + ch;
X  88:
X  end; (* addch *)
X 
Xbegin (* read_maildelivery_file *)
X  if DEBUG_OUT then writeln ('read_maildelivery_file called.');
X  read_maildelivery_file := false; last := nil; lcount := 0;
X  rindex := 0;
X  while not eof (dfile) do begin
X    readln (dfile, line); lcount := lcount + 1;
X    if DEBUG_OUT then writeln ('  Line from MAIL.DELIVERY: "', line, '".');
X    pindex := 1; current := nil; lindex := 1; quoted := false;
X    while lindex <= length (line) do begin
X      if (not quoted) and (line[lindex] in [' ', chr (9)]) then begin
X        if current <> nil then if pindex <= max_parameters then
X          if length (current^.parameters[pindex]) > 0 then begin
X            if DEBUG_OUT then writeln ('  Parameter #', pindex:0, ' is: "',
X                                       current^.parameters[pindex], '".');
X            pindex := pindex + 1;
X          end;
X      end else if line[lindex] = '"' then begin
X        if length (line) > lindex then
X          if line[lindex+1] = '"' then begin
X            addch ('"'); lindex := succ (lindex);
X          end else quoted := not quoted
X        else quoted := not quoted;
X      end else if quoted and (pindex > 5) then addch (line[lindex])
X      else addch (charupper (line[lindex]));
X      lindex := lindex + 1;
X    end; (* while not eoln *)
X    if current <> nil then with current^ do begin
X      if pindex <= max_parameters then
X        if length (parameters[pindex]) > 0 then begin
X          if DEBUG_OUT then writeln ('  Parameter #', pindex:0, ' is: "',
X                                     parameters[pindex], '".');
X          pindex := pindex + 1;
X        end;
X      pindex := pindex - 1;
X      if pindex < min_parameters then begin
X        if FROM_OWNER then LIB$SIGNAL (DELIVER__TOOFEWPARAMS, 1, lcount);
X        goto 99;
X      end;
X      any_from      := parameters[from_parameter]      = '*';
X      any_to        := parameters[to_parameter]        = '*';
X      any_subject   := parameters[subject_parameter]   = '*';
X      if parameters[subject_parameter] = '"' then
X        parameters[subject_parameter] := '';
X    end;
X  end; (* while not eof *)
X  if FROM_OWNER and (rules_list = nil) then
X    LIB$SIGNAL (DELIVER__NORULES)
X  else read_maildelivery_file := true;
X  99:
X  close (dfile);
Xend; (* read_maildelivery_file *)
X 
X(* MAIL_OUT_CONNECT is called by VMS MAIL to initiate a send operation. *)
X 
X[global] function MAIL_OUT_CONNECT (var context : unsigned;
X  var link_flag : integer;
X  var protocol, node : string_descriptor;
X  var log_link_error : integer;
X  var file_RAT, file_RFM : integer;
X  var MAIL$GL_FLAGS : integer;
X  var attached_file : string_descriptor) : integer;
X 
Xbegin (* MAIL_OUT_CONNECT *)
X  fromstring := ''; tostring := '';
X  subjectstring := ''; user_list_last := nil;
X  if DEBUG_OUT then writeln ('MAIL_OUT_CONNECT called.');
X  MAIL_OUT_CONNECT := SS$_NORMAL;
Xend; (* MAIL_OUT_CONNECT *)
X 
X(* MAIL_OUT_LINE is called by VMS MAIL whenever a single line of stuff
X   must be delivered to the DELIVER mail relay. *)
X 
X[global] function MAIL_OUT_LINE (var context : unsigned;
X  var link_flag : integer;
X  var node, line : string_descriptor) : integer;
X 
Xbegin (* MAIL_OUT_LINE *)
X  if DEBUG_OUT then writeln ('MAIL_OUT_LINE called.');
X  case iaddress (link_flag) of
X    (* MAIL is delivering a To: address *)
X    LNK_C_OUT_TO     : begin
X                         if DEBUG_OUT then writeln ('  OUT_TO option used.');
X                         copy_descr_to_string (line, tostring, DEBUG_OUT);
X                       end; (* LNK_C_OUT_TO *)
X    (* MAIL is delivering a From: address *)
X    LNK_C_OUT_SENDER : begin
X                         if DEBUG_OUT then
X                           writeln ('  OUT_SENDER option used.');
X                         copy_descr_to_string (line, fromstring, DEBUG_OUT);
X                       end; (* LNK_C_OUT_SENDER *)
X    (* MAIL is delivering a Subject: line *)
X    LNK_C_OUT_SUBJ   : begin
X                         if DEBUG_OUT then writeln ('  OUT_SUBJ option used.');
X                         copy_descr_to_string (line, subjectstring,
X                                               DEBUG_OUT);
X                       end; (* LNK_C_OUT_SUBJ *)
X  end; (* case *)
X  MAIL_OUT_LINE := SS$_NORMAL;
Xend; (* MAIL_OUT_LINE *)
X 
X(* MAIL_OUT_CHECK is called once with each addressee for the current
X   message and once again after the message body has been sent. *)
X 
X[global] function MAIL_OUT_CHECK (
X  var context : unsigned;
X  var link_flag : integer;
X  var protocol, addressee : string_descriptor;
X  procedure MAIL$READ_ERROR_TEXT) : integer;
X 
Xvar
X  usernamebuffer, userdirectory : string;
X  maildelivery : text; useruic : unsigned; useraccount : account_name;
X  currenttime : [quad] record
X                         l0, l1 : unsigned;
X                       end;
X 
Xbegin (* MAIL_OUT_CHECK *)
X  if DEBUG_OUT then writeln ('MAIL_OUT_CHECK called.');
X  case iaddress (link_flag) of
X    (* Check out an addressee *)
X    LNK_C_OUT_CKUSER : if (addressee.length = 1) and
X                          (addressee.address^[1] = chr (0)) then begin
X                         (* The null byte indicates that all the addressees
X                            have been accomodated. *)
X                         if DEBUG_OUT then writeln ('  Terminate user list.');
X                         MAIL_OUT_CHECK := SS$_NORMAL;
X                       end else begin
X                         if DEBUG_OUT then writeln ('  CKUSER option used.');
X                         copy_descr_to_string (addressee, usernamebuffer,
X                                               DEBUG_OUT);
X                         if DEBUG_OUT then
X                           writeln ('  Checking out user "',
X                                    usernamebuffer, '".');
X                         if not find_user_directory (usernamebuffer,
X                                                     userdirectory,
X                                                     useruic, useraccount) then
X                           MAIL_OUT_CHECK := DELIVER__USERNOEXIST
X                         else begin
X                           if DEBUG_OUT then writeln ('  Trying to open "',
X                             userdirectory + 'MAIL.DELIVERY', '".');
X                           open (file_variable := maildelivery,
X                                 file_name := userdirectory + 'MAIL.DELIVERY',
X                                 organization := SEQUENTIAL,
X                                 sharing := READONLY,
X                                 user_action := open_with_SYSPRV,
X                                 error := CONTINUE, history := READONLY);
X                           if status (maildelivery) <= 0 then
X                             reset (maildelivery, error := CONTINUE);
X                           if status (maildelivery) > 0 then begin
X                             LIB$SIGNAL (DELIVER__NOMDFILE, 2,
X                               usernamebuffer.length,
X                               iaddress (usernamebuffer.body));
X                             MAIL_OUT_CHECK := DELIVER__NOMDFILE;
X                           end else begin
X                             if DEBUG_OUT then
X                               writeln ('  Adding this user to active list.');
X                             user_count := user_count + 1;
X                             if user_list_last = nil then begin
X                               new (user_list_last);
X                               user_list := user_list_last;
X                             end else begin
X                               new (user_list_last^.next);
X                               user_list_last := user_list_last^.next;
X                             end;
X                             with user_list_last^ do begin
X                               FROM_OWNER := fromstring = usernamebuffer;
X                               if not read_maildelivery_file (maildelivery,
X                                  rules_list) then begin
X                                 if FROM_OWNER then LIB$SIGNAL (
X                                   DELIVER__MDIGNORED);
X                                 dispose_rules_list (rules_list);
X                               end;
X                               next      := nil;
X                               username  := usernamebuffer;
X                               directory := userdirectory;
X                               uic       := useruic;
X                               account   := useraccount;
X                               $GETTIM (currenttime);
X                               copyname := 'MAIL_' +
X                                           hex (currenttime.l0, 8, 8) +
X                                           hex (currenttime.l1, 8, 8) +
X                                           hex (user_count, 8, 8);
X                               if DEBUG_OUT then
X                                 writeln ('  Added user "', username,
X                                          '"; file code is "',
X                                          copyname, '".');
X                             end; (* with user_list_last^ *)
X                             MAIL_OUT_CHECK := SS$_NORMAL;
X                           end;
X                         end;
X                       end; (* LNK_C_OUT_CKUSER *)
X    (* Check out the message send operation *)
X    LNK_C_OUT_CKSEND : begin
X                         if DEBUG_OUT then writeln ('  CKSEND option used.');
X                         MAIL_OUT_CHECK := SS$_NORMAL;
X                       end; (* LNK_C_OUT_CKSEND *)
X  end; (* case *)
Xend; (* MAIL_OUT_CHECK *)
X 
X(* MAIL_OUT_FILE is called when the body of the message is ready to be
X   sent. The message is available as a file and must be read from this
X   temporary file using RMS. MAIL_OUT_FILE is where most of the actual
X   work DELIVER does takes place. The following steps are taken:
X 
X   (1) The mode of the message file is set to record I/O (MAIL sometimes
X       leaves the file in block mode).
X 
X   (2) The list of users to whom messages are being sent is scanned.
X       For each user on the list:
X 
X       (a) A copy of the message is placed in the user's default
X           directory. The file is created with SYSPRV, so it will
X           be owned by that user.
X 
X       (b) The user's rules are scanned and checked for matches.
X 
X       (c) If any of the rules are satisfied, a command file is also
X           created. This files contains some initial symbol definitions
X           and then commands to implement each of the user's rules that
X           matched. The command file ends with commands that delete the
X           copy of the message as well as the commmand file itself.
X 
X       (d) A batch job is created to run the command file. Note that this
X           means MAIL must be installed with CMKRNL privilege.
X*)
X 
X[global] function MAIL_OUT_FILE (var context : unsigned;
X  var link_flag : integer;
X  var protocol : string_descriptor;
X  var message_RAB : RAB$TYPE;
X  [asynchronous, unbound] procedure UTIL$REPORT_ERROR) : integer;
X 
Xvar
X  user_list_scan : user_block_ptr; onehasmatched, match : boolean;
X  rules_list_scan : parameter_block_ptr; message_file : text;
X  fromupstring, toupstring, subjectupstring, line : string;
X  index, stat : integer; ppriv, priv, iosb : [quad] array [0..1] of unsigned;
X  items : array [1..8] of item;
X 
X  function STR$MATCH_WILD (candidate : varying [l1] of char;
X    pattern : varying [l2] of char) : integer; extern;
X 
X  function STR$UPCASE (var dststr : varying [l1] of char;
X    var srcstr : varying [l2] of char) : integer; extern;
X 
X  (* function to read a line from the message file *)
X 
X  function get_line (var line : string) : boolean;
X 
X  var
X    stat : integer;
X 
X  begin (* get_line *)
X    get_line := false;
X    message_RAB.RAB$L_UBF := iaddress (line.body);
X    message_RAB.RAB$W_USZ := parameter_size;
X    stat := $GET (RAB := message_RAB);
X    if odd (stat) then begin
X      line.length := message_RAB.RAB$W_RSZ;
X      get_line := true;
X    end else if stat <> RMS$_EOF then
X      LIB$SIGNAL (DELIVER__MESREAERR, 1, stat);
X  end; (* get_line *)
X 
Xbegin (* MAIL_OUT_FILE *)
X  if DEBUG_OUT then writeln ('MAIL_OUT_FILE called.');
X 
X  (* Do some fancy footwork with RMS to insure that the file is open
X     for sequential access and not block access. MAIL sometimes has
X     this file open in block mode. The only way to change modes is
X     to disconnect the RAB, diddle the mode bit and then reconnect it. *)
X  if DEBUG_OUT then writeln ('  The BIO field of the RAB is set ',
X    uand (message_RAB.RAB$L_ROP, RAB$M_BIO) <> 0);
X  $DISCONNECT (RAB := message_RAB);
X  message_RAB.RAB$L_ROP := uand (message_RAB.RAB$L_ROP, unot (RAB$M_BIO));
X  $CONNECT (RAB := message_RAB);
X 
X  if DEBUG_OUT then writeln (' Creating upper case copies of header strings.');
X  STR$UPCASE (fromupstring,    fromstring);
X  STR$UPCASE (toupstring,      tostring);
X  STR$UPCASE (subjectupstring, subjectstring);
X  if DEBUG_OUT then writeln ('  From: "', fromupstring, '", To: "', toupstring,
X                             '", Subject: "', subjectupstring, '".');
X 
X  if DEBUG_OUT then writeln (' Pruning the rules list.');
X  user_list_scan := user_list;
X  while user_list_scan <> nil do begin
X    if DEBUG_OUT then writeln ('  User: "', user_list_scan^.username, '".');
X 
X    if DEBUG_OUT then writeln ('  Create copy of message in file "',
X                               user_list_scan^.directory,
X                               user_list_scan^.copyname, '.TEXT".');
X    open (file_variable := message_file, history := NEW,
X          record_length := parameter_size, record_type := VARIABLE,
X          file_name := user_list_scan^.directory +
X                       user_list_scan^.copyname + '.TEXT',
X          user_action := create_with_SYSPRV, error := CONTINUE,
X          organization := SEQUENTIAL);
X    if status (message_file) <= 0 then rewrite (message_file);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    $REWIND (RAB := message_RAB);
X    while get_line (line) do begin
X      writeln (message_file, line, error := CONTINUE);
X      if status (message_file) > 0 then
X        LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    end; (* while get_line *)
X    close (message_file, error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X 
X    if DEBUG_OUT then writeln ('  Creating command file named "',
X                               user_list_scan^.directory,
X                               user_list_scan^.copyname, '.COM".');
X    open (file_variable := message_file, history := NEW,
X          record_length := parameter_size, record_type := VARIABLE,
X          file_name := user_list_scan^.directory +
X                       user_list_scan^.copyname + '.COM',
X          user_action := create_with_SYSPRV, error := CONTINUE,
X          organization := SEQUENTIAL);
X    if status (message_file) <= 0 then rewrite (message_file);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    writeln (message_file, '$ SET NOON', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    writeln (message_file, '$ DELETE = "DELETE"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    writeln (message_file, '$ MESSAGE_DELETE == "YES"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    write (message_file, '$ FROM == "');
X    for index := 1 to length (fromstring) do begin
X      if fromstring[index] = '"' then write (message_file, '"');
X      write (message_file, fromstring[index]);
X    end; (* for *)
X    writeln (message_file, '"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    write (message_file, '$ QFROM == "');
X    for index := 1 to length (fromstring) do begin
X      if fromstring[index] = '"' then write (message_file, '"""');
X      write (message_file, fromstring[index]);
X    end; (* for *)
X    writeln (message_file, '"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    write (message_file, '$ TO == "');
X    for index := 1 to length (tostring) do begin
X      if tostring[index] = '"' then write (message_file, '"');
X      write (message_file, tostring[index]);
X    end; (* for *)
X    writeln (message_file, '"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    write (message_file, '$ QTO == "');
X    for index := 1 to length (tostring) do begin
X      if tostring[index] = '"' then write (message_file, '"""');
X      write (message_file, tostring[index]);
X    end; (* for *)
X    writeln (message_file, '"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    write (message_file, '$ SUBJECT == "');
X    for index := 1 to length (subjectstring) do begin
X      if subjectstring[index] = '"' then write (message_file, '"');
X      write (message_file, subjectstring[index]);
X    end; (* for *)
X    writeln (message_file, '"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    write (message_file, '$ QSUBJECT == "');
X    for index := 1 to length (subjectstring) do begin
X      if subjectstring[index] = '"' then write (message_file, '"""');
X      write (message_file, subjectstring[index]);
X    end; (* for *)
X    writeln (message_file, '"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    writeln (message_file, '$ MESSAGE_FILE == "',
X             user_list_scan^.directory,
X             user_list_scan^.copyname, '.TEXT"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    writeln (message_file, '$ COMMAND_FILE == "',
X             user_list_scan^.directory,
X             user_list_scan^.copyname, '.COM"', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X 
X    if DEBUG_OUT then writeln ('  Check this user''s delivery list.');
X    onehasmatched := false; rules_list_scan := user_list_scan^.rules_list;
X    while rules_list_scan <> nil do with rules_list_scan^ do begin
X      match := (any_to      or (STR$MATCH_WILD (toupstring,
X                                parameters[to_parameter]) = STR$_MATCH)) and
X               (any_from    or (STR$MATCH_WILD (fromupstring,
X                                parameters[from_parameter]) = STR$_MATCH)) and
X               (any_subject or (STR$MATCH_WILD (subjectupstring,
X                                parameters[subject_parameter]) = STR$_MATCH));
X      case parameters[decision_parameter][1] of
X        'A'      : match := true;           
X        'X'      : match := false;
X        'T', 'Y' : match := match;
X        'F', 'N' : match := not match;
X        '?', 'O' : match := match and (not onehasmatched);
X        'B', 'Q' : match := (not match) and (not onehasmatched);
X        'E'      : match := match or (not onehasmatched);
X        otherwise match := false;
X      end; (* decision case *)
X      if match then begin
X        if DEBUG_OUT then writeln (' Rule matched. From: pattern: "',
X          parameters[from_parameter], '", To: pattern: "',
X          parameters[to_parameter], '", Subject: pattern: "',
X          parameters[subject_parameter], '", Decision character: ',
X          parameters[decision_parameter][1], '.');
X        onehasmatched := true;              
X        case parameters[action_parameter][1] of
X          (* deliver *)                                      
X          'D'  : begin
X                   write (message_file, '$ MAIL/NOSELF/SUBJECT="(From: ');
X                   for index := 1 to length (fromstring) do begin
X                     if fromstring[index] = '"' then write (message_file, '"');
X                     write (message_file, fromstring[index]);
X                   end; (* for *)
V                   write (message_file, ') ''''QSUBJECT''! sent to ''''QTO''"')
X;
X                   write (message_file, ' ''MESSAGE_FILE'' " %_');
X                   writeln (message_file, user_list_scan^.username);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR,
X                                 1, status (message_file));
X                 end; (* deliver *)
X          (* privileged-deliver *)
X          'V'  : begin
X                   writeln (message_file,'$ PRIV = F$SETPRV("DETACH,SYSPRV")',
X                            error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR,
X                                 1, status (message_file));
X                   write (message_file, '$ MAIL/NOSELF',
V                   '/PROTOCOL=DELIVER_MAILSHR/SUBJECT="''''QSUBJECT'' ! sent to
X ''''QTO''"'); 
X                   write (message_file, ' ''MESSAGE_FILE'' " %_');
X                   writeln (message_file, user_list_scan^.username);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X                   writeln (message_file,'$ PRIV = F$SETPRV(PRIV)',
X                            error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR,
X                                 1, status (message_file));
X                 end; (* privileged-deliver *)
X          (* forward *)
X          'F'  : begin
X                   write (message_file, '$ MAIL/NOSELF/SUBJECT="(From: ');
X                   for index := 1 to length (fromstring) do begin
X                     if fromstring[index] = '"' then write (message_file, '"');
X                     write (message_file, fromstring[index]);
X                   end; (* for *)
X                   write (message_file, ') ''''QSUBJECT''"');
X                   writeln (message_file, ' ''MESSAGE_FILE'' ',
X                            parameters[argument_parameter], error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X                 end; (* forward *)
X          (* privileged-forward *)
X          'W'  : begin                       
X                   writeln (message_file,'$ PRIV = F$SETPRV("DETACH,SYSPRV")',
X                            error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR,
X                                 1, status (message_file));
X                   write (message_file, '$ MAIL/NOSELF',
X                     '/PROTOCOL=DELIVER_MAILSHR/SUBJECT="''''QSUBJECT''"');
X                   writeln (message_file, ' ''MESSAGE_FILE'' ',
X                            parameters[argument_parameter], error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X                   writeln (message_file,'$ PRIV = F$SETPRV(PRIV)',
X                            error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR,
X                                 1, status (message_file));
X                 end; (* privileged-forward *)
X          (* create, copy *)
X          'C'  : begin
X                   writeln (message_file, '$ COPY ''MESSAGE_FILE'' ',
X                            parameters[argument_parameter],
X                            error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X                 end; (* create, copy *)
X          (* append *)
X          'A'  : begin
X                   writeln (message_file, '$ APPEND ''MESSAGE_FILE'' ',
X                            parameters[argument_parameter],
X                            error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X                 end; (* append *)
X          (* execute *)
X          'E'  : begin
X                   if parameters[argument_parameter][1] <> '$' then
X                   write (message_file, '$ ');
X                   writeln (message_file, parameters[argument_parameter],
X                            error := CONTINUE);
X                   if status (message_file) > 0 then
X                     LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X                 end; (* execute *)
X          (* quit *)
X          'Q'  : rules_list_scan := nil;
X          otherwise begin end;
X        end; (* case *)
X      end; (* add commands to implement this matching rule *)
X      if rules_list_scan <> nil then rules_list_scan := rules_list_scan^.next;
X    end; (* while *)
X 
X    if not onehasmatched then begin
X      if DEBUG_OUT then writeln ('  No rules matched, just deliver it.');
X      write (message_file, '$ MAIL/NOSELF/SUBJECT="(From: ');
X      for index := 1 to length (fromstring) do begin
X        if fromstring[index] = '"' then write (message_file, '"');
X        write (message_file, fromstring[index]);
X      end; (* for *)
X      writeln (message_file, ') ''''QSUBJECT''" ''MESSAGE_FILE'' _',
X               user_list_scan^.username, error := CONTINUE);
X      if status (message_file) > 0 then
X        LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    end;
X 
X    if DEBUG_OUT then writeln ('  Finishing up delivery command file.');
X    writeln (message_file, '$ IF MESSAGE_DELETE .nes. "NO" then ',
X             'DELETE ''MESSAGE_FILE'';', error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    writeln (message_file, '$ DELETE ''COMMAND_FILE'';',
X             error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X    close (message_file, error := CONTINUE);
X    if status (message_file) > 0 then
X      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
X 
X    if DEBUG_OUT then writeln ('  Submitting batch job.');
X    priv[0] := PRV$M_CMKRNL + PRV$M_SYSPRV; priv[1] := 0;
X    $SETPRV (ENBFLG := 1, PRVADR := priv, PRMFLG := 0, PRVPRV := ppriv);
X    with items[1] do begin
X      len := length (user_list_scan^.username); code := SJC$_USERNAME;
X      addr := iaddress (user_list_scan^.username) + 2; rlen := 0;
X    end;
X    with items[2] do begin
X      len := length (batch_queue); code := SJC$_QUEUE;
X      addr := iaddress (batch_queue) + 2; rlen := 0;
X    end;
X    with items[3] do begin
X      len := length (batch_log); code := SJC$_LOG_SPECIFICATION;
X      addr := iaddress (batch_log) + 2; rlen := 0;
X    end;
X    with items[4] do begin
X      len := 8; code := SJC$_ACCOUNT_NAME;
X      addr := iaddress (user_list_scan^.account); rlen := 0;
X    end;
X    with items[5] do begin
X      len := 0; code := SJC$_NO_LOG_SPOOL;
X      addr := 0; rlen := 0;
X    end;
X    with items[6] do begin
X      len := 4; code := SJC$_UIC;
X      addr := iaddress (user_list_scan^.uic); rlen := 0;
X    end;
X    line := user_list_scan^.directory + user_list_scan^.copyname + '.COM';
X    with items[7] do begin
X      len := length (line); code := SJC$_FILE_SPECIFICATION;
X      addr := iaddress (line) + 2; rlen := 0;
X    end;
X    with items[8] do begin
X      len := 0; code := 0; addr := 0; rlen := 0;
X    end;
X    if DEBUG_OUT then writeln ('    Opening job.');
X    stat := $SNDJBCW (func := SJC$_ENTER_FILE, itmlst := items, iosb := iosb);
X    if (not odd (stat)) or (not odd (iosb[0])) then begin
X      with items[2] do begin
X        len := length (system_batch_queue);
X        addr := iaddress (system_batch_queue) + 2;
X      end;
V      stat := $SNDJBCW (func := SJC$_ENTER_FILE, itmlst := items, iosb := iosb)
X;
X    end;
X    if not odd (stat) then LIB$SIGNAL (stat);
X    if not odd (iosb[0]) then LIB$SIGNAL (iosb[0]);
X    priv[0] := uand (priv[0], unot (ppriv[0]));
X    priv[1] := uand (priv[1], unot (ppriv[1]));
X    $SETPRV (ENBFLG := 0, PRVADR := priv, PRMFLG := 0);
X    user_list_scan := user_list_scan^.next;
X  end; (* while *)
X  MAIL_OUT_FILE := SS$_NORMAL;
Xend; (* MAIL_OUT_FILE *)
X 
X(* MAIL_OUT_DEACCESS is called to shut down the current send operation. *)
X 
X[global] function MAIL_OUT_DEACCESS (var context : unsigned;
X  var link_flag : integer) : integer;
X 
Xbegin (* MAIL_OUT_DEACCESS *)
X  if DEBUG_OUT then writeln ('MAIL_OUT_DEACCESS called.');
X  if user_list <> nil then begin
X    if DEBUG_OUT then writeln ('  Deleting user list and associated rules.');
X    dispose_user_list (user_list);
X  end;
X  MAIL_OUT_DEACCESS := SS$_NORMAL;
Xend; (* MAIL_OUT_DEACCESS *)
X 
X(* These routines manipulate a stack in which we maintain state information for
X   information being "written" to us when MAIL calls MAIL_IO_WRITE. *)
X 
Xprocedure init_stack (var stack : write_state_stack);
X 
Xbegin (* init_stack *)
X  if DEBUG_IN then writeln ('INIT_STACK called.');
X  stack.top := 0;
Xend; (* init_stack *)
X 
Xprocedure push (var stack : write_state_stack; state : write_states);
X 
Xvar
X  i : integer;
X 
Xbegin (* push *)
X  if DEBUG_IN then writeln ('PUSH called.');
X  with stack do begin
X    top := succ (top);
X    if top > stack_size then LIB$SIGNAL (DELIVER__INTSTKOVR);
X    store[top] := state;
X  end; (* with *)
X  if DEBUG_IN then begin
X    writeln ('  after PUSH:');
X    for i := stack.top downto 1 do writeln ('    ', stack.store[i]);
X  end;
Xend; (* push *)
X 
Xprocedure pop (var stack : write_state_stack);
X 
Xvar
X  i : integer;
X 
Xbegin (* pop *)
X  if DEBUG_IN then writeln ('POP called.');
X  with stack do begin
X    top := pred (top);
X    if top < 1 then LIB$SIGNAL (DELIVER__STKEMPTY);
X  end; (* with *)
X  if DEBUG_IN then begin
X    writeln ('  after POP:');
X    for i := stack.top downto 1 do writeln ('    ', stack.store[i]);
X  end;
Xend; (* pop *)
X 
Xfunction top_of_stack (var stack : write_state_stack) : write_states;
X 
Xbegin (* top_of_stack *)
X  if DEBUG_IN then writeln ('TOP_OF_STACK called.');
X  top_of_stack := stack.store[stack.top];
X  if DEBUG_IN then writeln (' returning ', stack.store[stack.top]);
Xend; (* top_of_stack *)
X 
X(* The incoming mail handling routines are activated by a command line of
X   the form:
X 
X   $ MAIL/PROTOCOL=DELIVER_MAILSHR/SUBJECT="subject" message.txt address-list
X 
X   Everything is specified on the command line except the From: address,
X   which is obtained by translating the logical name DELIVER$FROM.
X 
X   System privileges are required to use this interface since this routine
X   makes it possible to "forge" return addresses. *)
X 
X[global] function MAIL_IN_CONNECT (var context : unsigned;
X  var link_flag : integer;
X  var input_tran : string_descriptor;
X  var file_RAT, file_RFM : integer;
X  var MAIL$GL_SYSFLAGS : integer;
X  var MAIL$Q_PROTOCOL : string_descriptor;
X  var pflags : integer) : integer;
X 
Xbegin (* MAIL_IN_CONNECT *)
X  if DEBUG_IN then writeln ('MAIL_IN_CONNECT called.');
X  toline := '';
X  if DEBUG_IN then writeln ('Initializing state stack.');
X  init_stack (write_recv_states);
X  push (write_recv_states, bad_msg);
X  LIB$SET_SYMBOL ('DELIVER$STATUS', '%X00000001');
X  MAIL_IN_CONNECT := SS$_NORMAL;
Xend; (* MAIL_IN_CONNECT *)
X 
X(* MAIL calls MAIL_IN_LINE to get single line information from DELIVER. *)
X 
X[global] function MAIL_IN_LINE (var context : unsigned;
X  var link_flag : integer;
X  var line : string_descriptor) : integer;
X 
Xvar
X  linebuffer : string; stat : integer;
X 
Xbegin (* MAIL_IN_LINE *)
X  if DEBUG_IN then writeln ('MAIL_IN_LINE called.');
X  case iaddress (link_flag) of
X    (* Return From: information to MAIL *)
X    LNK_C_IN_SENDER : begin
X                        if DEBUG_IN then writeln ('IN_SENDER option used.');
X                        stat := LIB$GET_SYMBOL ('FROM', linebuffer);
X                        if not odd (stat) then
X                          linebuffer := '<not specified>';
X                        copy_string_to_descr (linebuffer, line, DEBUG_IN);
X                       end; (* LNK_C_IN_SENDER *)
X    (* Return To: information to MAIL *)
X    LNK_C_IN_CKUSER : begin
X                        if DEBUG_IN then writeln ('IN_CKUSER option used.');
X                        stat := CLI$GET_VALUE ('TOLIST', linebuffer);
X                        if not odd (stat) then linebuffer := chr (0) else begin
X                          if length (toline) > 0 then toline := toline + ',';
X                          toline := toline + linebuffer;
X                          push (write_recv_states, user_check);
X                        end;
X                        copy_string_to_descr (linebuffer, line, DEBUG_IN);
X                      end; (* LNK_C_IN_CKUSER *)
X    (* Return entire To: line to MAIL *)
X    LNK_C_IN_TO     : begin
X                        if DEBUG_IN then writeln ('IN_TO option used.');
X                        copy_string_to_descr (toline, line, DEBUG_IN);
X                       end; (* LNK_C_IN_TO *)
X    (* Return entire Subject: line to MAIL *)
X    LNK_C_IN_SUBJ   : begin
X                        if DEBUG_IN then writeln ('IN_SUBJ option used.');
X                        stat := CLI$GET_VALUE ('SUBJECT', linebuffer);
X                        if not odd (stat) then linebuffer := '';
X                        copy_string_to_descr (linebuffer, line, DEBUG_IN);
X                       end; (* LNK_C_IN_SUBJ *)
X  end; (* case *)
X  MAIL_IN_LINE := SS$_NORMAL;
Xend; (* MAIL_IN_LINE *)
X 
X(* MAIL_IN_FILE is called by MAIL to read the body of the message to be
X   delivered. This routine gets the file name from the command line, opens
X   the file and copies it into MAIL's intermediate file. *)
X 
X[global] function MAIL_IN_FILE (var context : unsigned;
X  var link_flag : integer;
X  var scratch : integer;
X  var RAB : RAB$TYPE;
X  procedure UTIL$REPORT_IO_ERROR) : integer;
X 
X
Xvar
X  filename, linebuffer : string; message_file : text; stat : integer;
X 
Xbegin (* MAIL_IN_FILE *)
X  if DEBUG_IN then writeln ('MAIL_IN_FILE called.');
X  (* Get the name of the file containing the message to be delivered. *)
X  stat := CLI$GET_VALUE ('FILE', filename);
X  if not odd (stat) then begin
X    LIB$SIGNAL (DELIVER__GETFILERR, 1, stat);
X    MAIL_IN_FILE := DELIVER__GETFILERR;
X  end else begin
X    open (file_variable := message_file, file_name := filename,
X          organization := SEQUENTIAL, sharing := READONLY,
X          error := CONTINUE, history := READONLY);
X    if status (message_file) <= 0 then
X      reset (message_file, error := CONTINUE);
X    if status (message_file) > 0 then begin
X      LIB$SIGNAL (DELIVER__MESOPNERR);
X      MAIL_IN_FILE := DELIVER__MESOPNERR;
X    end else begin
X      RAB.RAB$L_RBF := iaddress (linebuffer) + 2;
X      stat := SS$_NORMAL;
X      while (not eof (message_file)) and (odd (stat)) do begin
X        readln (message_file, linebuffer, error := CONTINUE);
X        if status (message_file) > 0 then begin
X          LIB$SIGNAL (DELIVER__MSGREAERR, 1, status (message_file));
X          stat := DELIVER__MSGREAERR;
X        end else begin
X          RAB.RAB$W_RSZ := length (linebuffer);
X          stat := $PUT (RAB := RAB);
X          if not odd (stat) then
X            LIB$SIGNAL (DELIVER__MSGWRTERR, 1, stat);
X        end;
X      end; (* while *)
X      close (message_file, error := CONTINUE);
X      MAIL_IN_FILE := stat;
X    end;
X  end;
X  push (write_recv_states, delivery_check);
Xend; (* MAIL_IN_FILE *)
X 
X(* MAIL_IO_WRITE is called by MAIL to tell DELIVER what it thinks of the
X   results returned by the various MAIL_IN_ routines. *)
X 
X[global] function MAIL_IO_WRITE (var context : unsigned;
X  var link_flag : integer;
X  line : string_descriptor) : integer;
X 
Xvar
X  error_text : string;
X 
X  function string_to_integer (var str : string_descriptor) : integer;
X 
X  var
X    number : packed array [1..4] of char; i : integer;
X 
X  begin (* string_to_integer *)
X    if str.length <> 4 then string_to_integer := 0 else begin
X      for i := 1 to 4 do number[i] := str.address^[i];
X      string_to_integer := number :: integer;
X    end;
X  end; (* string_to_integer *)
X 
Xbegin (* MAIL_IO_WRITE *)
X  if DEBUG_IN then writeln ('MAIL_IO_WRITE called.');
X  case top_of_stack (write_recv_states) of
X    delivery_check : begin
X                       if DEBUG_IN then writeln ('Delivery check.');
X                       last_error := string_to_integer (line);
X                       if DEBUG_IN then writeln (' got a stat : ', last_error);
X                       pop (write_recv_states);
X                       if not odd (last_error) then
X                         LIB$SET_SYMBOL ('DELIVER$STATUS',
X                                         '%X' + hex (last_error, 8, 8));
X                       if last_error <> SS$_NORMAL then
X                         push (write_recv_states, error_msg);
X                     end; (* delivery_check *)
X    user_check :     begin
X                       if DEBUG_IN then writeln ('User check.');
X                       last_error := string_to_integer (line);
X                       if DEBUG_IN then writeln (' got a stat : ', last_error);
X                       pop (write_recv_states);
X                       if not odd (last_error) then
X                         LIB$SET_SYMBOL ('DELIVER$STATUS',
X                                         '%X' + hex (last_error, 8, 8));
X                       if last_error <> SS$_NORMAL then
X                         push (write_recv_states, error_msg);
X                     end; (* user_check *)
X    error_msg :      begin
X                       if DEBUG_IN then writeln ('Error text.');
X                       if (line.length = 1) and (line.address^[1] = chr (0))
X                       then begin
X                         if DEBUG_IN then
V                           writeln (' got a NULL -- popping write_recv_states')
X;
X                       end else begin
X                         copy_descr_to_string (line, error_text, DEBUG_IN);
X                         if DEBUG_IN then
X                           writeln ('Error message: "', error_text, '"');
X                         if not odd (last_error) then
X                           LIB$SET_SYMBOL ('DELIVER$MESSAGE', error_text);
X                       end;
X                       pop (write_recv_states);
X                     end; (* error_text *)
X    bad_msg :        begin
X                       if DEBUG_IN then writeln ('Unexpected message.');
X                       last_error := string_to_integer (line);
X                       if DEBUG_IN then
X                         writeln (' UNEXPECTED stat : ', last_error);
X                       push (write_recv_states, error_msg);
X                     end; (* bad_msg *)
X    otherwise LIB$SIGNAL (DELIVER__BADSTKELE);
X  end; (* case *)
X  MAIL_IO_WRITE := SS$_NORMAL;
Xend; (* MAIL_IO_WRITE *)
X 
X[global] function MAIL_IO_READ (var context : unsigned;
X  var link_flag : integer;
X  var returned_line : string_descriptor) : integer;
X 
Xbegin (* MAIL_IO_READ *)
X  if DEBUG_IN then writeln ('MAIL_IO_READ called.');
X  MAIL_IO_READ := SS$_NORMAL;
Xend; (* MAIL_IO_READ *)
X 
X(* End of DELIVER.PAS *)
Xend.
$ GoSub Convert_File
$ File_is="DELIVER.RNO"
$ Check_Sum_is=1348117871
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X.! DELIVER.RNO - Programmable delivery interface for VMS MAIL
X.! Written by Ned Freed, 10-Oct-1985, modified 30-Apr-1986
X.! and again 26-Sep-1986 and again 17-Dec-1986
X.!
X.! RUNOFF operation instructions:
X.!   This file can either produce a printed document or an entry
X.!   for VMS online HELP. Use the command
X.!
X.!     $ RUNOFF/NOBOLD/NOUNDER/OUT=DELIVER.HLP DELIVER.RNO
X.!
X.!   to create the online help entry. DELIVER.HLP can be inserted
X.!   into any VMS help library. Use the command
X.!
X.!     $ RUNOFF/OUT=DELIVER.MEM/VAR=MANUAL DELIVER.RNO
X.!
X.!   To create a document for printing.
X.!
X.if manual
X.!
X.! We set the layout so that running page numbers are on the bottom
X.!
X.lo 1,2
X.st
X.!
X.! Initial right margin - sections should never set it higher than this.
X.! Set page size too.
X.ps 57,67 .rm 67
X.!
X.! Header level setup
X.sthl 6,0,0,8,9,1,1,9,2
X.dhl D,D,lu,d,ll,ll
X.!
X.!
X.flags bold
X.!
X.sk 2
X.c;DELIVER - A Programmable Message Delivery System for VMS MAIL
X.title DELIVER - A Programmable Message Delivery System for VMS MAIL
X.sk 2
X.c;Ned Freed
X.c;Computing Services
X.c;Harvey Mudd College
X.sk 1
X.c;17-Dec-1986
X.sk 3
X.else manual
X.NO NUMBER .NO PAGING .STHL 5,1,1
X.endif manual
X.if manual
X.send toc .ifnot global
X.hl 1 ^*Overview\*
X.send toc .endif global
X.else manual
X.br
X1 DELIVER
X.br
X.endif manual
XDELIVER is an adjunct to VMS MAIL which makes it possible for
Xincoming mail messages to be handled and processed automatically based
Xon information provided in a user-supplied file. Different actions
Xcan be taken based on a message's address, subject or contents.
XThese actions include delivering the message, forwarding
Xthe message or even invoking a DCL command script to perform some
Xcomplex operation.
XAny actions taken occur immediately upon receipt of the message; the
Xuser does not need to log in for DELIVER to operate.
X.s 1
XDELIVER is modelled after the MAILDELIVERY facility of the MMDF mail
Xsystem. DELIVER is, however, completely distinct from MMDF and the
Xformats of .MAILDELIVERY files for MMDF and MAIL.DELIVERY files for
XDELIVER are dissimiliar.
X.s 1
XThis document assumes that the reader is familiar with the VMS MAIL facility.
X.if manual
X.send toc .ifnot global
X.hl 1 ^*Setting up DELIVER\*
X.send toc .endif global
X.else manual
X.br
X2 Setup
X.br
X.endif manual
XUsers can set up DELIVER to handle their incoming mail by
Xdoing just two things:
X.test page 5
X.s 1 .ls0 .dle "(",ll,")"
X.le;Create a MAIL.DELIVERY file in their accounts' initial default
Xdirectory (SYS$LOGIN). For security reasons
Xthis file MUST be located in the initial
Xdefault directory -- it cannot be stored anyplace else. The
Xformat of a MAIL.DELIVERY file is described below.
X.s 1
X.le;Set their mail forwarding address for the account to "DELIVER_%user",
Xwhere "user" is the username associated with the user's account. The MAIL
Xcommand to change or set a forwarding address is SET FORWARD.
XSpecifying another user's name in conjunction with "DELIVER_%"
Xwill perform no useful function -- mail will simply be forwarded and
Xhandled by that user's MAIL.DELIVERY file, assuming one exists.
X.els 1
XOnce these two steps have been taken, DELIVER will
Xbe invoked automatically to handle all mail as it is delivered.
XFor example, suppose user BOB wants to tell DELIVER to handle incoming
Xmessages. BOB should create a MAIL.DELIVERY file and then type:
X.s 1 .i 4 .test page 4
X$ MAIL
X.i 4
XMAIL_> SET FORWARD DELIVER_%BOB
X.i 4
XMAIL_>
X.if manual
X.send toc .ifnot global
X.hl 1 ^*MAIL.DELIVERY file format\*
X.send toc .endif global
X.else manual
X.br
X2 Format
X.br
X.endif manual
XThe MAIL.DELIVERY file controls DELIVER and tells it how to handle
Xeach message. A MAIL.DELIVERY file consists of a series of directives with
Xone directive on each line of the file. Each directive specifies how
Xa certain kind of message is to be handled. A particular directive
Xmay or may not apply to a given message. An attempt is made to apply
Xevery directive in the MAIL.DELIVERY file to each message, thus
Xmore than one directive may apply to (and more than
Xone action may be the result of) a single message.
X.s 1
XAny line in the file which begins with a semicolon or an exclamation point
Xis considered to be a comment and is ignored.
X.s 1
XA directive line consists of the following items in order from left to right:
X.s 1 .i 3
X_<from-pat_> _<to-pat_> _<subject-pat_> _<accept_> _<action_> _<parameter_>
X.s 1
XItems must be delimited by one or more spaces or tabs. Quoted strings
X(use double quotes, not single quotes)
Xare allowed as single items; the quotes are removed
Xfrom the items as they are read. A double quote can be obtained by using
Xtwo double quotes with no space between them. This form of quote handling
Xis consistent with that of VMS DCL.
X.if manual
X.send toc .ifnot global
X.hl 2 ^*Directive applicability\*
X.send toc .endif global
X.else manual
X.br
X3 Applicability
X.br
X.endif manual
XThe _<from-pat_>, _<to-pat_>, _<subject-pat_> and _<accept_> items
Xdetermine whether or not the directive applies to a particular message.
XA string comparison is performed between the patterns _<from-pat_>,
X_<to-pat_> and _<subject-pat_> and the "From:", "To:" and
X"Subject:" fields of the message header respectively.
XThe comparison is not case sensitive.
XThe usual VMS wildcard characters ("_*" and "_%") can be used in
Xthe patterns. The pattern "_*" will match anything.
X.s 1
XOnce the comparisons have been performed, the _<accept_> item
Xdetermines if the directive should be applied to the message. Only the first
Xcharacter of _<accept_> is significant. It should be one
Xof the following:
X.s 1 .nf .lm +3 .test page 5
X A - always apply this directive; ignore the results of
X     the comparisons.
X.test page 2
X X - never apply this directive; ignore the results of
X     the comparisons.
X.test page 2
X T - apply this directive if the patterns all matched.
X.test page 2
X F - apply this directive if the patterns did not all match.
X.test page 2
X O - apply this directive if the patterns all matched and
X     no previous directive has been applied to the message.
X.test page 2
X B - apply this directive if a pattern did not match and
X     no previous directive has been applied to the message.
X.test page 2
X E - this directive applies if all the patterns matched
X     OR no other directive has been applied so far.
X.s 1 .f .lm -3
XAny character whatsoever is legal: Y is the same as T,
XN is the same as F,
Xquestion mark is the same as O, Q is the same as B and all other
Xcharacters are the same as X.
X.s 1
XDirectives are tested in the order they appear in the MAIL.DELIVERY file.
X.s 1
XFor example, suppose JIM sends a message to BOB. The subject line of
Xthe message is "Re: Mooses". BOB's MAIL.DELIVERY
Xfile contains the following lines (the function of the last two columns
Xof each line, the _<action_> and _<parameter_> items, is described later):
X.s 1 .lm +4 .nf
X.test page 5
X"FRED _*" _* _*         T Q
X"JIM _*"  _* _*         T A JIM.LOG
X_*        _* _*mooses_*  T A MOOSE.LOG
X_*        _* _*         O A OTHER.LOG
X_*        _* _*         A D
X.s 1 .lm -4 .f
XThe first directive in the file does not apply since the message is not
Xfrom FRED. The second and third directives both apply since JIM is the
Xsender and the subject line contains the string "mooses". The fourth
Xdirective's patterns all apply, but a preceeding directive has applied,
Xso it does not apply. The final directive applies since it would apply
Xto any message. The result is that three directives apply to this
Xmessage, and thus three separate actions are taken in processing the
Xmessage.
X.s 1
XNote: The patterns "FRED _*" and "JIM _*"
Xare useful since VMS MAIL lets users set up personal name fields
Xwhich become part of the "From:" field of the message -- the personal
Xname is enclosed in quotes and appended to the user name.
XDepending on
Xpersonal name fields for message handling is not a good idea since some
Xusers have a tendency to change personal names frequently and without warning.
XThe use of the space followed by an asterisk will match any personal name
Xfield a user sets up; the result is a MAIL.DELIVERY file which is
Xinsensitive to personal names.
X.s 1
XIf none of the directives in the file are found to apply to the message,
Xthe message is just delivered normally. In effect, each MAIL.DELIVERY
Xfile implicitly ends with the directive:
X.s 1 .i 4
X_* _* _* O D
X.if manual
X.send toc .ifnot global
X.hl 2 ^*Actions\*
X.send toc .endif global
X.else manual
X.br
X3 Actions
X.br
X.endif manual
XThe _<action_> and _<parameter_> items specify what action is taken
Xwhen a directive is applied to a message. The first character of
X_<action_> specifies what type of action to take. The legal characters for
X_<action_> and what they do are:
X.s 1 .nf .lm +3
X.test page 6
X A - append the body (or contents) of the message to a file.
X     The _<parameter_> item specifies the file name. The file
X     must already exist and the recipient must have write access
X     to the file; DELIVER grants the user no special file access
X     privileges.
X.test page 3
X C - copy the body of the message to a file whose name is
X     _<parameter_>. Write access to the directory where the
X     file is to be created is required.
X.test page 2
X D - deliver the message normally. The message is placed in
X     the user's NEWMAIL folder. _<parameter_> is ignored.
X.test page 8
X V - deliver the message normally using system privileges.
X     This action is identical to action "D" above except that
X     the "From:" field of the message header is set to match
X     the name of the original sender instead of the name of
X     the user. This action makes use of the DELIVER foreign
X     mail interface in incoming mode which in turn requires
X     that the user be fully privileged. General users should
X     use action "D" instead.
X.test page 6
X E - execute the specified command. The DCL command specified
X     by _<parameter_> is executed. The command is executed in
X     the environment of the recipient's own account. Any
X     noninteractive DCL command is valid, including an
X     indirect command file specification. Several DCL symbols
X     can be used in the command to facilitate message
X     processing:
X.s 1
X.test page 3
X     _FROM           - the message's "From:" address.
X     _TO             - the message's "To:" address.
X     _SUBJECT        - the message's "Subject:".
X.test page 3
X     _QFROM          - "From:" with quotes doubled.
X     _QTO            - "To:" with quotes doubled.
X     _QSUBJECT       - "Subject:" with quotes doubled.
X.test page 7
X     MESSAGE__FILE   - the name of the file containing the
X     _                 body of the message. MESSAGE__FILE
X     _                 is always fully qualified.
X     MESSAGE__DELETE - initially set to "YES", if this
X     _                 symbol is set to "NO" no attempt will
X     _                 be made to delete MESSAGE__FILE after
X     _                 all actions are complete.
X.s 1
X.test page 2
X     The "Q" forms are useful if the symbol must be expanded
X     inside a quoted string. The MESSAGE__DELETE flag is useful
X     if MESSAGE__FILE has to be queued for further processing
X     at a later time.
X.test page 2
X F - forward the message. The message is forwarded to the
X     address specified by _<parameter_>.
X.test page 8
X W - forward the message using system privileges. This action
X     is identical to action "F" above except that the "From:"
X     field of the message header is set to match the name of
X     the original sender instead of the name of the user. This
X     action makes use of the DELIVER foreign mail interface in
X     incoming mode which in turn requires that the user be
X     fully privileged.  General users should use action "F"
X     instead.
X.test page 5
X Q - quit; take no action and abort. If this action is taken
X     DELIVER stops scanning the MAIL.DELIVERY file at this
X     point. No subsequent directives will apply after this
X     one. Use this directive with care; it is very easy to
X     lose messages when this action is employed.
X.s 1 .lm -3 .f
XFor example, suppose that BOB sends JIM a message. JIM has the
Xfollowing (rather complex) MAIL.DELIVERY file:
X.s 1 .nf .lm +4 .test page 6
X! Messages with subject "Loopback" are returned to sender
X"JIM _*"   _* "Loopback" T D
X_*         _* "Loopback" O F """''F$ELEMENT(0,"" "",QFROM)'"""
X_*         _* "Loopback" T Q
X! All other messages are logged
X_*         _* _*          A E @LOGALL.COM
X! Just log messages from TERRY
X.test page 7
X"TERRY _*" _* _*          T Q
X! Just log archive messages from myself
X"JIM _*"   _* "Archives" T Q
X! Save messages from BOB in a special file
X"BOB _*"   _* _*          T A BOB.LOG
X! Then deliver anything that gets this far
X_*         _* _*          A D
X.s 1 .f .lm -4
XJIM's LOGALL.COM contains the following commands:
X.s 1 .nf .lm +4
X.test page 6
X$ from    == "From:    " + from
X$ to      == "To:      " + to
X$ subject == "Subject: " + subject
X$ open/append/error=make__one x message.log
X$ next:
X$ write x ""
X$ write x from
X$ write x to
X$ write x subject
X$ write x ""
X$ close x
X.test page 7
X$ append 'message__file' message.log
X$ exit
X$ !
X$ make__one:
X$ create message.log
X$ open/append x message.log
X$ goto next
X.s 1 .f .lm -4
XIf the subject line of BOB's message is not the string "Loopback",
Xthe message will be logged with a header
Xin the file MESSAGE.LOG (located in
XJIM's SYS$LOGIN directory), appended to the file BOB.LOG without
Xany header and delivered to JIM's NEWMAIL folder.
XIf subject line is the string "Loopback", JIM's MAIL.DELIVERY file
Xwill bounce the message right back to BOB.
X.s 1
XAs another example, if TERRY sends a message to BOB, the message
Xis logged only in BOB's MESSAGE.LOG file; BOB never receives any
Xnotification that the message arrived. Apparently TERRY never
Xsays anything of importance to BOB.
X.s 1
XIt is clear that the ability to execute an arbitrary set of DCL
Xcommands in response to a message is a very powerful tool. It must,
Xhowever, be used with care, since processing is initiated whenever
Xa message is received and operates in a completely unattended
Xenvironment.
X.if manual
X.send toc .ifnot global
X.hl 1 ^*Implementation\*
X.send toc .endif global
X.else manual
X.br
X2 Implementation
X.br
X.endif manual
XWarning: The details in this section are technical in nature and are probably
Xof interest only to system managers or programers.
X.s 1
XDELIVER is implemented as foreign interface to VMS MAIL. DELIVER
Xis activated when a message addressed to "DELIVER_%user" is
Xsent. MAIL invokes DELIVER by loading the DELIVER__MAILSHR shareable image
Xand calling a set of routines DELIVER provides to interface to MAIL.
XDELIVER does the following:
X.s 1 .ls 0 .dle "(",d,")"
X.le;The $GETUAI system service is used to validate the address.
XDELIVER will signal an error if an attempt is made to deliver mail
Xto someone who does not exist. The recipient's default directory
Xis retrieved to use when opening the recipient's MAIL.DELIVERY file.
X.le;DELIVER checks to see that the recipient has a MAIL.DELIVERY file in his or
Xher home directory. If this file does not exist DELIVER signals an error.
XIf the file exists it is read and parsed.
X.le;DELIVER writes the body of the message to a temporary file in the
Xrecipient's home directory.
X.le;A command file is constructed to complete the delivery process.
XThis file is also created in the recipient's home directory. The
Xdirectives previously read from the MAIL.DELIVERY file are compared
Xwith the message.  Any directories that match cause commands to be written to
Xthe command file that implement the requested action.
X.le;After the list of directives is exhausted DELIVER checks to see
Xthat at least one directive caused an action to be taken. If none
Xdid, DELIVER writes a default action command to deliver the message
Xnormally into the command file. Commands to delete the message file
X(unless the MESSAGE__DELETE flag is set to "NO" by one of the actions)
Xand the command file itself are written to the command file and the
Xcommand file is closed.
X.le;The command file is queued to the batch queue MAIL$BATCH
Xfor processing. (SYS$BATCH is used if MAIL$BATCH does not exist.)
XThe file is queued so that it will execute just
Xas if the recipient had submitted it for processing from his or
Xher own account. (Note: This requires
XCMKRNL privilege.) Once the command
Xfile is submitted DELIVER tidies up, deallocating any storage allocated
Xfor directive lists, and returns control to MAIL.
X.els 0
X.if manual
X.send toc .ifnot global
X.hl 1 ^*The other half - using DELIVER to send messages\*
X.send toc .endif global
X.else manual
X.br
X2 Sending__messages
X.br
X.endif manual
XEvery foreign protocol interface to VMS MAIL has two parts -- one which
Xdeals with received messages (the outgoing part) and another which
Xtransfers messages to VMS MAIL (the incoming part). All the functions
Xof DELIVER described up to this point are part of the outgoing part.
XRather than include a null incoming handler in DELIVER, it was decided
Xto add a general-purpose message queueing system that might be useful both
Xas a simple interface to VMS MAIL and as an example of how this part
Xof a foreign protocol interface is constructed.
X.s 1
XThe message enqueueing part of DELIVER can be used only by fully privileged
Xusers since it allows arbitrary settings of "From:" addresses and so
Xcould violate MAIL security in the hands of general users. Thus, this
Xmechanism is of little interest to most users.
X.s 1
XA message is sent via DELIVER to VMS MAIL with a command of the form:
X.s 1 .i 4
X$ MAIL/PROTOCOL=DELIVER__MAILSHR -
X.i 4
X######/SUBJECT="subject"#message-file#to-list
X.s 1
Xwhere "subject" is the subject line for the message, "message-file"
Xis the name of a file containing the text of the message and to-list
Xis a list of user names (delimited by commas) the message is to be sent to.
XThe "From:" address for the message is not specified as part of the command
Xline; it is obtained instead by translating the DCL symbol FROM.
XNo checking is done on the validity of the translation.
XThis mode of MAIL operation roughly corresponds to the "trusted submit"
Xmode available in MMDF-II.
X.s 1
XDELIVER sets two DCL symbols to indicate the success or failure of
Xits attempt to deliver the message. The symbol DELIVER$STATUS will contain
Xthe status code of the last error that occurs while the message is being
Xsent. If DELIVER$STATUS indicates that some sort of failure occurred, the
Xsymbol
XDELIVER$MESSAGE will contain the text form of the error message.
X.s 1
XIn the event of multiple errors while sending (e.g. two addresses
Xin the "to-list" are illegal) only the more recent error status information
Xwill be returned. This interface is somewhat crude but should prove
Xadequate for most applications.
X.s 1
XThe incoming side of DELIVER is used by the outgoing side to process
Xthe "V" and "W" actions, which correspond to "privileged deliver" and
X"privileged forward" operations respectively.
X.if manual
X.send toc .ifnot global
X.hl 1 ^*Availability\*
X.send toc .endif global
X.else manual
X.br
X2 Availability
X.br
X.endif manual
XDELIVER is in the public domain. Copies can be obtained from:
X.s 1 .nf .lm +10 .test page 5
XNed Freed (ned@ymir.bitnet)
XComputing Services
XHarvey Mudd College
XClaremont, CA 91711
X(714) 621-8006
X.lm -10 .s 1
XPlease write, call or send e-mail for additional information.
X.if manual
X.send toc .ifnot global
X.hl 1 ^*Installation\*
X.send toc .endif global
X.else manual
X.br
X2 Installation
X.br
X.endif manual
XThis section describes the installation procedure for DELIVER.
XDELIVER must be installed by a system manager or other suitably
Xprivileged person. If you are not authorized to install privileged
Xsoftware on your system, you will have to have your system manager
Xdo this for you.
X.if manual
X.send toc .ifnot global
X.hl 2 ^*Reading the distribution\*
X.send toc .endif global
X.else manual
X.br
X3 Distribution
X.br
X.endif manual
XDELIVER is available as a standalone system but it is also
Xincluded in the distribution of PMDF -- the Pascal
XMail Distribution Facility. If you received your copy of DELIVER
Xin conjunction with PMDF the installation of PMDF covers the installation
Xof DELIVER and you should follow the instructions PMDF provides and not
Xthe instructions below.
X.s 1
XThe first thing to do is to decide where the source,
Xexamples and documentation for DELIVER are to be stored. This
Xinstallation procedure assumes the directory DISK:[DELIVER] is
Xused; you should substitute your own directory wherever DISK:[DELIVER]
Xappears.
X.s 1
XDELIVER is distributed either as a VMS BACKUP save set on tape or as
Xan archive file. In the case of a tape, a single save set
Xmust be restored. Issue the following commands:
X.s 1 .i 4
X$ MOUNT/FOREIGN tape:
X.i 4
X$ BACKUP/TRUNCATE/LOG tape:DELIVER.BCK DISK:[DELIVER]
X.s 1
Xwhere "tape" is the name of the tape drive where the DELIVER distribution
Xtape is mounted. This process will create the directory for
XDELIVER if it does not already exist.
X.s 1
XIn the case of an archive file the directory for DELIVER must be created
Xmanually. Once this is done
Xplace the archive file in the directory and execute it as
Xa command file.
X.if manual
X.send toc .ifnot global
X.hl 2 ^*Files\*
X.send toc .endif global
X.else manual
X.br
X3 Files
X.br
X.endif manual
XThe distribution of DELIVER contains the following files:
X.s 1 .lm +1 .nf .test page 4
XBUILD.COM           Command file to build the DELIVER
X                    shareable image.
XDELIVER.PAS         Main source file for DELIVER.
XDELIVER.RNO         This file.
XDELIVER__ERR.MSG     Error messages for DELIVER.
XDELIVER__MAILSHR.EXE The DELIVER shareable image.
XFORWARD.COM         Companion to FORWARD.DELIVERY.
XFORWARD.DELIVERY    A MAIL.DELIVERY file for people who have
X                    moved to a new address.
XMAILSHR.MAR         Routine interface to VMS MAIL.
XMAILSHR.OPT         LINK options needed to build DELIVER.
X.test page 3
XTRIP.COM            Companion to TRIP.DELIVERY.
XTRIP.DELIVERY       A MAIL.DELIVERY file for people who are
X                    out of town.
X.lm -1 .f .s 1
XObject and executable files corresponding to the various source
Xfiles listed above are also included if the distribution media is a tape.
X.if manual
X.send toc .ifnot global
X.hl 2 ^*Compilation and linking\*
X.send toc .endif global
X.else manual
X.br
X3 Compilation__and__linking
X.br
X.endif manual
XThe distribution tape comes with an executable version of
XDELIVER that has already
Xbeen compiled and linked. The archive does not contain such
Xan executable image, so DELIVER must be re-compiled from source.
X.s 1
XDELIVER should be recompiled and relinked by executing
Xthe command file BUILD.COM. This command
Xprocedure produces the shareable image DELIVER__MAILSHR.EXE.
X.s 1
XCAUTION: DELIVER must be compiled with VAX Pascal version 3.1 or later.
XDELIVER will not run on versions of VMS prior to 4.0.
X.if manual
X.send toc .ifnot global
X.hl 2 ^*File locations\*
X.send toc .endif global
X.else manual
X.br
X3 File__locations
X.br
X.endif manual
XThe shareable image for DELIVER must be located on SYS$LIBRARY so
XMAIL can access it (there is a logical name mechanism that allows
Xthis image to be located in another directory, but this mechanism
Xis broken in some versions of VMS, so it is best just to put the
Ximage in SYS$LIBRARY). Copy the file DELIVER__MAILSHR.EXE to
XSYS$LIBRARY. Make sure the file is owned by [SYSTEM] and has
Xworld read access.
X.s 1
XDELIVER requires CMKRNL privilege so it can queue batch jobs
Xon the behalf of other users. This means that MAIL must be
Xinstalled with CMKRNL privilege and DELIVER
Xmust be installed. However, MAIL is already
Xinstalled, so to install MAIL with CMKRNL privilege the image
Xmust be deleted and reinstalled. The following INSTALL
Xcommands will accomplish this:
X.s 1 .nf .lm +4
X$ MCR INSTALL
XINSTALL_> SYS$LIBRARY:DELIVER__MAILSHR.EXE
XINSTALL_> SYS$SYSTEM:MAIL/DELETE
XINSTALL_> SYS$SYSTEM:MAIL/OPEN/HEADER/SHARE -
X_         /PRIV=(SYSPRV,OPER,WORLD,NETMBX,CMKRNL)
X.s 1 .f .lm -4
XThese commands should be placed in the local startup command file
Xfor the system so they will execute each time the system is booted.
X(If SYS$MANAGER:VMSIMAGES.COM is used, a "!/NOREMOVE" qualifier should
Xbe added to each of the above commands.)
X.if manual
X.send toc .ifnot global
X.hl 2 ^*Batch queue for DELIVER\*
X.send toc .endif global
X.else manual
X.br
X3 Batch__queues
X.br
X.endif manual
XThe last step in installing DELIVER is to define the batch queue
XDELIVER should use for executing delivery command files.
XDELIVER expects to use a queue named MAIL$BATCH. This can be
Xan actual batch queue or a logical name equated to some
Xexisting queue. Either the queue or the logical name should be
Xstarted or set up by the local startup command file.
XA queue with a small job limit (1 or#2)
Xand a relatively high priority (3 or#4) is recommended so users
Xdon't experience long delays in receiving messages.
X.s 1
XIf no MAIL$BATCH queue exists DELIVER will try to use the standard
Xsystem batch queue SYS$BATCH instead.
X.s 1
XDELIVER enables SYSPRV as well as CMKRNL while queueing the job,
Xso the queue can be protected against normal user submissions if
Xdesired.
X.s 1
XDELIVER should now be ready to use. Create a
Xsample MAIL.DELIVERY file and try it out (or use one of the sample
Xfiles included in the distribution).
X.if manual
X.send toc .ifnot global
X.hl 2 ^*Documentation\*
X.send toc .endif global
X.else manual
X.br
X3 Documentation
X.br
X.endif manual
XThe file DELIVER.RNO contains the only documentation for DELIVER.
X(You are reading it right now.) DELIVER.RNO can be
Xused either to produce a printed document or a VMS help file entry.
XUse the command
X.s 1 .i 4
X$ RUNOFF/NOBOLD/NOUNDER/OUT=DELIVER.HLP DELIVER.RNO
X.s 1
Xto create the online help entry. DELIVER.HLP can be inserted
Xinto any VMS help library. Use the command
X.s 1 .i 4
X$ RUNOFF/OUT=DELIVER.MEM/VAR=MANUAL DELIVER.RNO
X.s 1
Xto create a document for printing.
X.if manual
X.send toc .ifnot global
X.hl 1 ^*Bugs\*
X.send toc .endif global
X.else manual
X.br
X2 Bugs
X.br
X.endif manual
XThere are no known bugs in DELIVER at this time. However, there are
Xa few minor nuisances which users should be aware of:
X.s 1 .ls 0 .dle "(",d,")"
X.le;DELIVER changes the "From:" address of any message it delivers or
Xforwards to the address of the owner of the MAIL.DELIVERY file.
XThe original "From:" address is not lost entirely -- it is merged into
Xthe subject line of the message. This problem arises due to VMS MAIL's
Xlack of distinction between a transport address and a header address --
XDELIVER has to set the "From:" address to itself so that authorization
Xcode in other mailers will see it. Privileged users can circumvent
Xthis restriction by using the "V" and "W" actions, but no such
Xmechanism is available to general users.
X.le;It is difficult to debug MAIL.DELIVERY files since there is no
Xway to watch deliver process the file except by enabling debug
Xcode in DELIVER (which is not an option normal users can exercise).
XNote that output from command files invoked by
XDELIVER can be captured in a file by using the#/OUTPUT qualfier:
X.s 1 .i 4
X! Execute a command file with logging
X.i 4
X_* _* _* A E @DO__SOMETHING.COM/OUTPUT=DO__SOMETHING.LOG
X.s 1
XDELIVER does watch for users sending messages to themselves and then
Xtries to be somewhat more informative about any errors it finds
Xin MAIL.DELIVERY files.
X.le;Enabling the COPY__SELF feature in MAIL while DELIVER is also set to
Xsend messages to the user's mailbox may send MAIL into an infinite loop. The
XCOPY__SELF facility should not follow forwarding addresses; unfortunately
Xit does do this in the present implementation. Thus a message is sent by
XDELIVER to the user's mailbox, which in turn re-activates DELIVER, which
Xsends the message to the user's mailbox, and so on.
X.le;Lines in all files processed by DELIVER are limited to, at
Xmost,#256 characters.
XIndividual directive items as well as message "From:", "To:" and "Subject:"
Xlines are also limited to#256 characters. (This limit can be changed
Xby altering the constant PARAMETER__SIZE in DELIVER.PAS.)
X.els
$ GoSub Convert_File
$ File_is="DELIVER_ERR.MSG"
$ Check_Sum_is=1917791014
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X          .Title        DELIVER error messages
X 
X! Written by Ned Freed, 24-Sep-1985, modified 30-Apr-1986
X 
X          .Facility     DELIVER,667/prefix=DELIVER__
X          .Ident        'DELIVER Version 1.4'
X 
X     .Severity  fatal
X 
X        INTSTKOVR    <Internal error, stack overflow>
X        STKEMPTY     <Internal error, stack empty>
X        BADSTKELE    <Internal error, bad element found on stack>
X 
X     .Severity  error
X 
X        CANACCUAF    <No privileges to access SYSUAF.DAT>
X        NOSUCHUSER   <No such user exists in SYSUAF.DAT>
X        USERNOEXIST  <Specified user "!AD" does not exist>/FAO=2
X        NAMETOOLONG  <User name "!AD" is too long>/FAO=2
X        NODEFAULTDIR <No default directory exists for user "!AD">/FAO=2
X        NOMDFILE     <No MAIL.DELIVERY file exists for user "!AD">/FAO=2
V        MESREAERR    <Error reading intermediate message file, status = !UL>/FA
XO=1
X        MESWRTERR    <Message file write error, Pascal status = !UL>/FAO=1
X        GETFILERR    <Can't get file name from CLI, status = !UL>/FAO=1
X        MESOPNERR    <Error opening message file, Pascal status = !UL>/FAO=1
V        MSGWRTERR    <Error writing intermediate message file, status = !UL>/FA
XO=1
X        MSGREAERR    <Error reading message file, Pascal status = !UL>/FAO=1
V        UNKFUNC      <Foreign MAIL protocol invoked with unknown function !UL.>
X/   FAO=1
X 
X     .Severity warning
X 
V        TOOMANYPARAMS <Too many parameters on line !UL of MAIL.DELIVERY file>/F
XAO=1
V        TOOFEWPARAMS  <Too few parameters on line !UL of MAIL.DELIVERY file>/FA
XO=1
X        MDIGNORED     <MAIL.DELIVERY file ignored>
X        NORULES       <No rules found in MAIL.DELIVERY file>
X 
X     .End
$ GoSub Convert_File
$ File_is="DELIVER_MYMAIL.COM"
$ Check_Sum_is=1008742497
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$`009SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
X`009  + F$VERIFY(0'DELIVER_MYMAIL_VERIFY' .GT. 0)
X$`009SET ON
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$`009ON WARNING THEN GOTO ABNORMAL_EXIT
X$ `009GoTo After_Headers
X$ !
X$ ! The purpose of this DELIVER_MYMAIL routine is to reply to a message whose
X$ ! subject is specified in my mail.delivery, with all new mail since I last 
X$ ! read mail on my home node. There are two methods of getting newmail, see
X$ ! below.
X$ !
X$ ! The entry in MAIL.DELIVERY looks like this, excluding "$ !" on each line
X$ ! It should go after your auto-forward line and before your auto-reply line.
X$ !    WARNING: DO NOT BE OBVIOUS IN YOUR CHOICE OF THE MAGIC SUBJECT.
X$ !============================================================================
X$ !! Send a reply containing all new mail
X$ !!
X$ !*                       *   "Chosensubj"    T E "$@dir:DELIVER_MYMAIL.COM"
X$ !*                       *   "Chosensubj"    T Q
X$ !============================================================================
X$ !          
X$ !`009V1.0`009- Andy Leslie, Last updated 1-4-1988.
X$ !
X$After_headers:
X$ !
X$ ! Put all your DELIVER command files EXCEPT MAIL.DELIVERY in DELIVER$DIR
X$ ! MAIL.DELIVERY must go in SYS$LOGIN
X$ !
X$ ! Reset commands that the user may have redefined.
X$ !
X$`009APPEND`009= "APPEND"
X$`009CLOSE`009= "CLOSE"
X$`009CONVERT`009= "CONVERT"
X$`009CREATE`009= "CREATE"
X$`009DEFINE`009= "DEFINE"       
X$`009DELETE `009= "DELETE"
X$`009EXIT`009= "EXIT"
X$`009GOTO`009= "GOTO"
X$`009IF`009= "IF"
X$`009MAIL`009= "MAIL"
X$`009ON`009= "ON"
X$`009OPEN`009= "OPEN"
X$`009PURGE`009= "PURGE"                                       
X$`009READ`009= "READ"
X$`009RENAME`009= "RENAME"
X$`009SEARCH`009= "SEARCH"
X$`009SET`009= "SET"
X$`009SPAWN`009= "SPAWN"
X$`009SUBMIT`009= "SUBMIT"
X$`009TYPE`009= "TYPE"         
X$`009WAIT   `009= "WAIT"
X$`009WRITE`009= "WRITE"
X$       
X$
X$ dquote="""
X$ !
X$ from=f$edit(from,"COMPRESS,TRIM")
X$ address = from
X$ address_len = 'f$length(address)'
X$ space_pos = 'f$locate(" ",address)'
X$ mrgate_pos = 'f$locate("MRGATE::",address)'
X$ extstr= f$extract(mrgate_pos,99,address)
X$       
X$ first_dquote_pos = 'f$locate(dquote,address)'  
X$ extstr= f$extract(first_dquote_pos+1,99,address)
X$ second_dquote_pos = 'f$locate(dquote,extstr)' + first_dquote_pos +1
X$
X$ extstr= f$extract(second_dquote_pos+1,99,address)
X$ third_dquote_pos = 'f$locate(dquote,extstr)' + second_dquote_pos +1
X$
X$ extstr=f$extract(third_dquote_pos+1,99,address) 
X$ fourth_dquote_pos = 'f$locate(dquote,extstr)'`009+ third_dquote_pos    +1
X$ !
X$Remove_Personal_name:
X$ 
X$ If first_dquote_pos +1 .eq. address_len then goto remove_poor_mans_routing
X$ ! No Quotes at all!
X$
X$ ! First quote is pers
X$ If f$extract(first_dquote_pos-1 ,1,address) .nes. " " then goto endif1
X$ `009address = f$element(0," ", ADDRESS)
X$ `009goto remove_poor_mans_routing
X$ endif1:
X$ ! First quote is routing
X$ If f$extract(first_dquote_pos-1 ,1,address) .nes. ":" then goto endif2
X$ `009address = f$extract(0,second_dquote_pos+1, ADDRESS)
X$ `009goto remove_poor_mans_routing
X$ endif2:
X$
X$ ! address = f$extract(0,pers_dquote_pos, ADDRESS) ! Remove Personal Name
X$ 
X$REMOVE_POOR_MANS_ROUTING: 
X$       
X$`009POS = 'F$LOCATE("::",ADDRESS)' 
X$ `009MRGATE_POS=F$LOCATE("MRGATE",ADDRESS) 
X$ `009QUOTE_POS=F$LOCATE(QUOTE,ADDRESS)
X$`009
X$`009STRING = F$EXTRACT(POS+2,999,ADDRESS)
X$`009MRSTRING = F$EXTRACT(POS+2,6,address)
X$                                  
X$      `009IF MRSTRING .EQS. "MRGATE" THEN goto remote_address_found
X$ !`009`009deals with NODE::MRGATE::"A User @REO"
X$`009IF STRING - "::" .EQS. STRING THEN goto remote_address_found
X$  `009IF POS .gt. quote_pos THEN goto remote_address_found
X$ !`009`009deals with "A1::A User"
X$`009address=string
X$`009GOTO REMOVE_POOR_MANS_ROUTING
X$
X$ remote_address_found:
X$ qfrom = address
X$ from  = address
X$ write sys$output "Final address to send all newmail to was: ",address
X$ !      
X$ ! Now to extract and send the NEWMAIL folder to the sender of the magic
X$ !  subject line.
X$ ! There are two ways of handling this. 
X$ ! 1) copies all the mail into a deliver$temp folder
X$ ! 2) doesn't
X$ ! The difference is that the EXTRACT/ALL
X$ ! directly from Newmail places all the newmail into the mail
X$ ! folder - thus you only get the mail since last you executed this 
X$ ! procedure. take your pick and change the goto if need be.
X$ !
X$ goto method_1   ! All newmail
X$ ! goto method_2 ! Only newmail since last I did this
X$                                                        
X$ Method_1:
X$ !      
X$ mail
Xsel newmail
Xcopy/all/noconfirm deliver$temp
Xselect deliver$temp
Xextract/all/mail sys$scratch:deliver$temp_mail.mai;
Xselect deliver$temp
Xdelete/all
Xexit
X$ !
X$ goto MAIL_IT
X$ !
X$ Method_2:
X$ !                                                      
X$ mail
Xsel newmail
Xcopy/all/noconfirm deliver$temp
Xextract/all/mail sys$scratch:deliver$temp_mail.mai;
Xexit
X$
X$MAIL_IT:
X$ VMS_VERSION = F$GETSYI("VERSION")                
X$ SYS_VERSION = F$EXTRACT(1,1,VMS_VERSION)
X$ IF  (SYS_VERSION .EQ. 5) THEN goto V5
X$ V4:
X$`009mail/noself sys$scratch:deliver$temp_mail.mai;  -
X`009/subject="Newmail from the DELIVER-daemon" -
X   `009DELIVER$DIR:DELIVER_REPLY.TXT 'qfrom
X$`009Goto Normal_Exit
X$ V5:
X$ ! Note that "/personal_name=" is a VMS V5 feature
X$`009mail/noself sys$scratch:deliver$temp_mail.mai;  -
X`009/pers="DELIVER Mail-Daemon" -
X`009/subject="Newmail from the DELIVER-daemon" -
X   `009DELIVER$DIR:DELIVER_REPLY.TXT 'qfrom
X$
X$ delete sys$scratch:deliver$temp_mail.mai;*
X$ !                                                      
X$ ! Normal exit.
X$ !
X$NORMAL_EXIT:
X$`009STATUS = $STATUS`009`009`009!save success status
X$`009GOTO EXIT                                               
X$ !
X$ ! Abnormal exit.
X$ !
X$ABNORMAL_EXIT:
X$`009STATUS = $STATUS`009`009`009!save failure status
X$`009IF STATUS THEN STATUS = "%X0800002A"`009!force error if neccessary
X$`009GOTO EXIT
X$ !
X$ ! Exit.
X$ !                     
X$EXIT:
X$`009EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
$ GoSub Convert_File
$ File_is="DELIVER_REPLY.COM"
$ Check_Sum_is=1024874470
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$`009SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
X`009  + F$VERIFY(0'DELIVER_REPLY_VERIFY' .GT. 0)
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$`009ON WARNING THEN GOTO ABNORMAL_EXIT
X$ `009GoTo AFter_Headers
X$ !
X$ !     Provided for use with DELIVER by Andy Leslie
X$ !                                      Sandhurst, Surrey, England
X$ !`009DELIVER$DIR:DELIVER_REPLY.TXT
X$ !
X$ ! The purpose of this auto-reply routine is to be able to cope with
X$ !  the various quotated address notations - ALLIN1 and ARPAnet for two
X$ ! e.g.
X$ !`009ARPAGATEWAY::"xxxxxx@xxx.xxx.xxx" "Personal Name"
X$ !`009NODE::MRGATE::"A1::A User" "Personal Name"
X$ !
X$ !  as well as ordinary DECnet addresses, including PMR
X$ ! e.g.
X$ !`009NODE::User `009`009"Personal Name"
X$ !
X$ !`009`009and MRGATE. Other gateways must be added manually.
X$ !
X$ !     DELIVER_REPLY.COM
X$ !`009V1.4`009- Andy Leslie, Last updated 28-7-1987.
X$ !
X$After_Headers:
X$ !
X$ ! Put all your DELIVER command files EXCEPT MAIL.DELIVERY in DELIVER$DIR
X$ ! MAIL.DELIVERY must go in SYS$LOGIN
X$ !
X$ ! Reset commands that the user may have redefined.
X$ !
X$`009APPEND`009= "APPEND"
X$`009CLOSE`009= "CLOSE"
X$`009CONVERT`009= "CONVERT"
X$`009CREATE`009= "CREATE"
X$`009DEFINE`009= "DEFINE"
X$`009DELETE`009= "DELETE"
X$`009EXIT`009= "EXIT"
X$`009GOTO`009= "GOTO"
X$`009IF`009= "IF"
X$`009MAIL`009= "MAIL"
X$`009ON`009= "ON"
X$`009OPEN`009= "OPEN"
X$`009PURGE`009= "PURGE"
X$`009READ`009= "READ"
X$`009RENAME`009= "RENAME"
X$`009SEARCH`009= "SEARCH"
X$`009SET`009= "SET"
X$`009SPAWN`009= "SPAWN"
X$`009SUBMIT`009= "SUBMIT"
X$`009TYPE`009= "TYPE"
X$`009WAIT`009= "WAIT"
X$`009WRITE`009= "WRITE"
X$ ! Redefine OPUT as follows for debugging
X$ ! Oput = "Write SYS$OUTPUT"
X$ ! Leave it as a comment for now
X$ Oput ="!"
X$ !
X$ dquote="""
X$ !
X$ ! read sys$input from
X$ ! NODE::MRGATE::"A1::USER"
X$ ! NODE::NODE::User "Me"
X$ !
X$ from=f$edit(from,"COMPRESS,TRIM")
X$ !
X$ Oput from
X$ !
X$ address = from
X$ address_len = 'f$length(address)'
X$ oput "address length:",address_len
X$ space_pos = 'f$locate(" ",address)'
X$ mrgate_pos = 'f$locate("MRGATE::",address)'
X$ oput "mrgate_pos:",mrgate_pos
X$ extstr= f$extract(mrgate_pos,99,address)
X$ Oput "from mrgate:: on:",extstr
X$
X$ first_dquote_pos = 'f$locate(dquote,address)'
X$ oput first_dquote_pos
X$ extstr= f$extract(first_dquote_pos+1,99,address)
X$ Oput "from first quote on:",extstr
X$ second_dquote_pos = 'f$locate(dquote,extstr)' + first_dquote_pos +1
X$ Oput second_dquote_pos
X$
X$ extstr= f$extract(second_dquote_pos+1,99,address)
X$ Oput "from second quote on:",extstr
X$ third_dquote_pos = 'f$locate(dquote,extstr)' + second_dquote_pos +1
X$ Oput third_dquote_pos
X$
X$ extstr=f$extract(third_dquote_pos+1,99,address)
X$ Oput "from third quote on:",extstr
X$ fourth_dquote_pos = 'f$locate(dquote,extstr)'`009+ third_dquote_pos    +1
X$ Oput fourth_dquote_pos
X$ !
X$ !
X$Remove_Personal_name:
X$
X$ If first_dquote_pos +1 .eq. address_len then goto remove_poor_mans_routing
X$ ! No Quotes at all!
X$
X$ oput f$extract(first_dquote_pos-1 ,1,address)
X$ oput f$extract(0,second_dquote_pos+1, ADDRESS)
X$ ! First quote is pers
X$ If f$extract(first_dquote_pos-1 ,1,address) .nes. " " then goto endif1
X$ `009address = f$element(0," ", ADDRESS)
X$ `009goto remove_poor_mans_routing
X$ endif1:
X$ ! First quote is routing
X$ If f$extract(first_dquote_pos-1 ,1,address) .nes. ":" then goto endif2
X$ `009address = f$extract(0,second_dquote_pos+1, ADDRESS)
X$ `009goto remove_poor_mans_routing
X$ endif2:
X$
X$ ! address = f$extract(0,pers_dquote_pos, ADDRESS) ! Remove Personal Name
X$
X$REMOVE_POOR_MANS_ROUTING:
X$
X$`009POS = 'F$LOCATE("::",ADDRESS)'
X$ `009MRGATE_POS=F$LOCATE("MRGATE",ADDRESS)
X$ `009QUOTE_POS=F$LOCATE(DQUOTE,ADDRESS)
X$
X$`009STRING = F$EXTRACT(POS+2,999,ADDRESS)
X$`009MRSTRING = F$EXTRACT(POS+2,6,address)
X$
X$ oput address
X$ oput string
X$ oput mrstring
X$ oput f$extract(quote_pos,999,address)
X$
X$      `009IF MRSTRING .EQS. "MRGATE" THEN goto remote_address_found
X$ !`009`009deals with NODE::MRGATE::"A User @REO"
X$`009IF STRING - "::" .EQS. STRING THEN goto remote_address_found
X$  `009IF POS .gt. quote_pos THEN goto remote_address_found
X$ !`009`009deals with "A1::A User"
X$`009address=string
X$`009GOTO REMOVE_POOR_MANS_ROUTING
X$
X$ remote_address_found:
X$ qfrom = address
X$ from  = address
X$ search_string = address
V$ if mrstring .eqs. "MRGATE" then search_string = f$extract(quote_pos,999,addre
Xss)
X$ write sys$output "Final address was: ",address
X$ ! Check for replies sent list's presence
X$ if F$SEARCH("DELIVER$DIR:DELIVER.HISTORY") .nes. "" then goto search_list
X$ open/write HISTORY DELIVER$DIR:DELIVER.HISTORY
X$ goto add_list
X$ !
X$ ! Check the list for this sender
X$ search_list:
X$ search/nooutput DELIVER$DIR:DELIVER.HISTORY 'search_string
X$ if $status .eq. 1 then exit
X$ open/append HISTORY DELIVER$DIR:DELIVER.HISTORY
X$ !
X$ add_list:
X$ write HISTORY FROM
X$ close HISTORY
X$ VMS_VERSION = F$GETSYI("VERSION")
X$ SYS_VERSION = F$EXTRACT(1,1,VMS_VERSION)
X$ IF  (SYS_VERSION .EQ. 5) THEN goto V5
X$ V4:
X$`009mail/noself -
X`009/subject="Auto-Reply from my DELIVER Mail-daemon" -
X   `009DELIVER$DIR:DELIVER_REPLY.TXT 'qfrom
X$`009Goto Normal_Exit
X$ V5:
X$ ! Note that "/personal_name=" is a VMS V5 feature
X$`009mail/noself -
X`009/pers="DELIVER Mail-Daemon" -
X`009/subject="Auto-Reply from my DELIVER Mail-daemon" -
X   `009DELIVER$DIR:DELIVER_REPLY.TXT 'qfrom
X$
X$ !
X$ ! Normal exit.
X$ !
X$NORMAL_EXIT:
X$`009STATUS = $STATUS     `009`009`009!save success status
X$`009GOTO EXIT
X$ !
X$ ! Abnormal exit.
X$ !
X$ABNORMAL_EXIT:
X$`009STATUS = $STATUS`009`009`009!save failure status
X$`009IF STATUS THEN STATUS = "%X0800002A"`009!force error if neccessary
X$`009GOTO EXIT
X$ !
X$ ! Exit.
X$ !
X$EXIT:
V$      `009EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*
X0
$ GoSub Convert_File
$ File_is="DELIVER_REPLY.TXT"
$ Check_Sum_is=2038388767
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X                
X
X    Subject: Auto Reply from my Mail DELIVERy Daemon
X
X    Hi,
X
X    `009thanks for your mail. However I'm out of the office today.
X
X    This is the only copy of this message you will receive to the mail
X    address at which you receive it.
X
X                                best regards,            
X
X`009`009`009`009`009/your name here/
$ GoSub Convert_File
$ File_is="FORWARD.COM"
$ Check_Sum_is=1242202045
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$ GoTo SkipComments
X$!
X$!    FORWARD.COM - Command procedure to forward mail and notify
X$!    sender of new address, etc.
X$!
X$!    SYS$LOGIN:NEWADDRESS.TXT should contain the text body of
X$!    a message that will be mailed to the sender.
X$!
X$!    SYS$LOGIN:NOTIFIED.LIST will contain a list of senders
X$!    that have already been notified.
X$!
X$!    The symbol NEW_ADDRESS defined below should be edited to reflect
X$!    the new electronic mail address to forward mail to.
X$!
X$! Provided for use with DELIVER by:
X$!
X$!    Dan Ehrlich
X$!    4-Sep-1986
X$!    Schlumberger-Doll Research
X$!
X$SkipComments:
X$!
X$!    Set up new address
X$!    Note: All double quotes internal to the address must be QUADRUPLED!
X$!
X$ NEW_ADDRESS = "IN%""""ned@ymir.bitnet"""""
X$!
X$!    If the list of people we have already notified does
X$!    not exist then create it.
X$!
X$ If F$Search("SYS$LOGIN:NOTIFIED.LIST") .eqs. "" -
X    Then -
X        $ Create Sys$Login:Notified.List
X$!
X$!    See if we have already notified this sender
X$!
X$ Search/NoOutput    Sys$Login:Notified.List    "''QFROM'"
X$ If $Status .eq. 1 -
X    Then -
X        $ GoTo ForwardMail
X$!
X$!    Add this sender to the list
X$!
X$ Open/Append    TR    Sys$Login:Notified.List
X$ Write    TR    FROM
X$ Close    TR
X$!
X$!    Send notification of new address
X$!
X$ Mail/NoSelf/Subject="New Address" Sys$Login:NewAddress.Txt -
X  "''F$Element(0," ",QFROM)'"
X$!
X$!    Forward message to new address
X$!
X$ForwardMail:
X$ Mail/NoSelf/Subject="''SUBJECT'" 'MESSAGE_FILE' "''NEW_ADDRESS'"
X$ Purge/NoLog    Mail*.*
X$ MESSAGE_DELETE == "YES"
$ GoSub Convert_File
$ File_is="FORWARD.DELIVERY"
$ Check_Sum_is=293029684
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X! FORWARD.DELIVERY - Send a reply saying this user has moved to a new
X!                    address. Keep a list of who has received a notice
X!                    of the new address so that only one address change
X!                    notice is sent to each person. Forward the message
X!                    to the new address in any case.
X! Written by Ned Freed, 15-Sep-1986
X!
X* * * A E @FORWARD.COM
$ GoSub Convert_File
$ File_is="MAILSHR.MAR"
$ Check_Sum_is=1765433097
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X          .Title        MAILSHR - Foreign mail protocol interface
X 
X;
X; Written by Kevin Carosso @ Hughes Aircraft Co., SCG/CTC, January 11, 1985
X; Modified by Ned Freed, 16-Nov-86, to use proper global symbols.
X;
X;---------------------------------------------------------------------------
X; This is invoked by MAIL when it encounters the foreign mail protocol.
X; This module really has nothing protocol-specific to it and can be used
X; to dispatch to any handler.  The handler should supply the following
X; action routines:
X;
X;       status := MAIL_OUT_CONNECT (context : unsigned;
X;                                   LNK_C_OUT_CONNECT : immediate;
X;                                   protocol, node : string_descriptor;
X;                                   MAIL$_LOGLINK : immediate;
X;                                   file_RAT, file_RFM : immediate;
X;                                   MAIL$GL_FLAGS : immediate;
X;                                   attached_file : descriptor := immediate 0)
X;
X;       status := MAIL_OUT_LINE    (context : unsigned;
X;                                   [LNK_C_OUT_SENDER | LNK_C_OUT_TO |
X;                                    LNK_C_OUT_SUBJ] : immediate;
X;                                   node, sender_name : string_descriptor)
X;
X;       status := MAIL_OUT_CHECK   (context : unsigned;
X;                                   [LNK_C_OUT_CKUSER |
X;                                    LNK_C_OUT_CKSEND] : immediate;
X;                                   node, addressee : string_descriptor;
X;                                   procedure MAIL$READ_ERROR_TEXT);
X;
X;       status := MAIL_OUT_FILE    (context : unsigned;
X;                                   LNK_C_OUT_FILE : immediate;
X;                                   node : string_descriptor;
X;                                   rab : $RAB_TYPE;
X;                                   procedure UTIL$REPORT_IO_ERROR);
X;
X;       status := MAIL_OUT_DEACCESS (context : unsigned;
X;                                    LNK_C_OUT_DEACCESS : immediate);
X;
X;       status := MAIL_IN_CONNECT (context : unsigned;
X;                                  LNK_C_IN_CONNECT : immediate;
X;                                  input_tran : string_descriptor;
X;                                  file_RAT, file_RFM : immediate;
X;                                  MAIL$GL_FLAGS : immediate;
X;                                  MAIL$Q_PROTOCOL : string_descriptor;
X;                                  pflags : immediate);
X;
X;       status := MAIL_IN_LINE   (context : unsigned;
X;                                 [LNK_C_IN_SENDER | LNK_C_IN_CKUSER |
X;                                  LNK_C_IN_TO | LNK_C_IN_SUBJ] : immediate;
X;                                 returned_line : string_descriptor);
X;
X;       status := MAIL_IN_FILE     (context : unsigned;
X;                                   LNK_C_OUT_FILE : immediate;
X;                                   0 : immediate;
X;                                   rab : $RAB_TYPE;
X;                                   procedure UTIL$REPORT_IO_ERROR);
X;
X;       status := MAIL_IO_READ  (context : unsigned;
X;                                LNK_C_IO_READ : immediate;
X;                                returned_text_line : string_descriptor);
X;
X;       status := MAIL_IO_WRITE (context : unsigned;
X;                                LNK_C_IO_WRITE : immediate;
X;                                text_line : string_descriptor);
X;
X;---------------------------------------------------------------------------
X;
X; Define major and minor protocol identifiers.  MAIL requires that these
X; be 1.  The shareable image MUST be linked with the options file MAILSHR.OPT
X; that promotes these symbols to UNIVERSAL symbols so they will end up
X; in the shareable image's symbol table.
X;
X                MAIL$C_PROT_MAJOR == 1
X                MAIL$C_PROT_MINOR == 1
X;
X; Constants for dispatcher, taken from MAIL.SDL listing
X;
X        LNK_C_FIRST = 0
X        LNK_C_OUT_CONNECT  == 0
X        LNK_C_OUT_SENDER   == 1
X        LNK_C_OUT_CKUSER   == 2
X        LNK_C_OUT_TO       == 3
X        LNK_C_OUT_SUBJ     == 4
X        LNK_C_OUT_FILE     == 5
X        LNK_C_OUT_CKSEND   == 6
X        LNK_C_OUT_DEACCESS == 7
X 
X        LNK_C_IN_CONNECT   == 8
X        LNK_C_IN_SENDER    == 9
X        LNK_C_IN_CKUSER    == 10
X        LNK_C_IN_TO        == 11
X        LNK_C_IN_SUBJ      == 12
X        LNK_C_IN_FILE      == 13
X 
X        LNK_C_IO_READ      == 14
X        LNK_C_IO_WRITE     == 15
X        LNK_C_LAST = 15
X;
X; Here's the main routine that is called by MAIL.  Note that we don't really
X; do any work here, just dispatch the call to the appropriate handler.  The
X; reason I do it this way is that I am not interested in writing the handlers
X; in MACRO, and I cannot easily deal with different numbers of arguments in
X; the same procedure in other languages.
X;
X 
X;
X; General argument offset to the function code:
X;
X        LNK_FUNCTION = 8
X;
X; Shareable image transfer vectors
X;
X        .Transfer       MAIL$PROTOCOL
X        .Mask           MAIL$PROTOCOL
X        jmp     L^MAIL$PROTOCOL + 2
X 
X        .Entry  MAIL$PROTOCOL, ^M<r2,r3>
X 
X        caseb   LNK_FUNCTION(ap), #LNK_C_FIRST, -       ; Dispatch to handler
X                #<LNK_C_LAST - LNK_C_FIRST>
X 
X10$:      .word Dispatch_out_connect - 10$              ; LNK_C_OUT_CONNECT
X          .word Dispatch_out_line - 10$                 ; LNK_C_OUT_SENDER
X          .word Dispatch_out_check - 10$                ; LNK_C_OUT_CKUSER
X          .word Dispatch_out_line - 10$                 ; LNK_C_OUT_TO
X          .word Dispatch_out_line - 10$                 ; LNK_C_OUT_SUBJ
X          .word Dispatch_out_file - 10$                 ; LNK_C_OUT_FILE
X          .word Dispatch_out_check - 10$                ; LNK_C_OUT_CKSEND
X          .word Dispatch_out_deaccess - 10$             ; LNK_C_OUT_DEACCESS
X 
X          .word Dispatch_in_connect - 10$               ; LNK_C_IN_CONNECT
X          .word Dispatch_in_line - 10$                  ; LNK_C_IN_SENDER
X          .word Dispatch_in_line - 10$                  ; LNK_C_IN_CKUSER
X          .word Dispatch_in_line - 10$                  ; LNK_C_IN_TO
X          .word Dispatch_in_line - 10$                  ; LNK_C_IN_SUBJ
X          .word Dispatch_in_file - 10$                  ; LNK_C_IN_FILE
X 
X          .word Dispatch_IO_read - 10$                  ; LNK_C_IO_READ
X          .word Dispatch_IO_write - 10$                 ; LNK_C_IO_WRITE
X 
Xunknown:
X        pushl   LNK_FUNCTION(ap)        ; FAO parameter in the function code
X        pushl   #1
X        pushl   #DELIVER__UNKFUNC       ; Signal unknown function code
X        calls   #3, G^LIB$SIGNAL        ; if we fall through dispatcher.
X        movl    #DELIVER__UNKFUNC, r0
X        ret
X;
X; The dispatchers
X;
XDispatch_out_connect:
X        callg   (ap), MAIL_OUT_CONNECT
X        ret
X 
XDispatch_out_line:
X        callg   (ap), MAIL_OUT_LINE
X        ret
X 
XDispatch_out_check:
X        callg   (ap), MAIL_OUT_CHECK
X        ret
X 
XDispatch_out_file:
X        callg   (ap), MAIL_OUT_FILE
X        ret
X 
XDispatch_out_deaccess:
X        callg   (ap), MAIL_OUT_DEACCESS
X        ret
X 
XDispatch_in_connect:
X        callg   (ap), MAIL_IN_CONNECT
X        ret
X 
XDispatch_in_line:
X        callg   (ap), MAIL_IN_LINE
X        ret
X 
XDispatch_in_file:
X        callg   (ap), MAIL_IN_FILE
X        ret
X 
XDispatch_IO_read:
X        callg   (ap), MAIL_IO_READ
X        ret
X 
XDispatch_IO_write:
X        callg   (ap), MAIL_IO_WRITE
X        ret
X 
X        .end
$ GoSub Convert_File
$ File_is="MAILSHR.OPT"
$ Check_Sum_is=993680312
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
Xuniversal=MAIL$C_PROT_MAJOR, MAIL$C_PROT_MINOR
$ GoSub Convert_File
$ File_is="MAIL_BATCH.COM"
$ Check_Sum_is=943469693
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$`009SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
X`009  + F$VERIFY(0'MAIL_BATCH_VERIFY' .GT. 0)
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$!                          
X$`009GOTO END_OF_HEADER_COMMENTS
X$!
X$! A DELIVER utility...
X$!
X$! This command file enables you to execute a batch job on your home
X$! system whilst you are away. To use, put the following lines in your
X$! MAIL.DELIVERY file:
X$!
X$! * * "SUBJECT" T E @deliver$dir:mail_batch
X$! * * "SUBJECT" T Q
X$!
X$! "SUBJECT" should be changed to something obscure. Ensure that you haver both
X$! this file and MAIL_BATCH_CLEANUP.COM in the directory pointed to by the 
X$! logical DELIVER$DIR. You will also need to add the following line to your 
X$! LOGIN.COM file:
X$!
X$!  $ define deliver$forward_address NODE::NAME
X$!
X$! where NODE and NAME should be changed to the node and username from
X$! which you will use this facility.
X$!
X$! Now you can simply mail a batch command file to your home system, using
X$! the subject chosen above, and your home system will execute the command
X$! file and mail the logfile back to you. Useful for checking directories,
X$! changing file protection, looking at text files (just TYPE them), etc.
X$!
X$! Donated to DELIVER_V1_3 kit by
X$! Keith Edmunds, June 11, 1988
X$!
X$! Edit History:
X$!
X$! 11 June 1988    `009Original version (T.01)
X$! 28 July 1988`009`009Cosmetic changes, tidied up (V1.0)
X$!
X$END_OF_HEADER_COMMENTS:
X$!
X$`009version = "V1.0"
X$!
X$! Reset commands that the user may have redefined.
X$!
X$`009APPEND`009`009= "APPEND"
X$`009CLOSE`009`009= "CLOSE"
X$`009CONVERT`009`009= "CONVERT"
X$`009CREATE`009`009= "CREATE"
X$`009DEASSIGN`009= "DEASSIGN"
X$`009DEFINE`009`009= "DEFINE"
X$`009DELETE`009`009= "DELETE"
X$`009EDIT`009`009= "EDIT"
X$`009EXIT`009`009= "EXIT"
X$`009GOTO`009`009= "GOTO"
X$`009IF`009`009= "IF"
X$`009INQUIRE`009`009= "INQUIRE"
X$`009MAIL`009`009= "MAIL"
X$`009ON`009`009= "ON"
X$`009OPEN`009`009= "OPEN"
X$`009PURGE`009`009= "PURGE"
X$`009READ`009`009= "READ"
X$`009RENAME`009`009= "RENAME"
X$`009SEARCH`009`009= "SEARCH"
X$`009SET`009`009= "SET"
X$`009SORT`009`009= "SORT"
X$`009SPAWN`009`009= "SPAWN"
X$`009SUBMIT`009`009= "SUBMIT"
X$`009TYPE`009`009= "TYPE"
X$`009WAIT`009`009= "WAIT"
X$`009WRITE`009`009= "WRITE"
X$ `009on error then goto it_broke
X$
X$ ! Use PID to construct temporary filenames
X$
X$ `009pid = f$getjpi("","PID")
X$
X$ ! Now check the batch request came from us
X$
X$ `009from = f$element(0," ",from)
X$ `009fwd_user = f$trnlnm("DELIVER$FORWARD_ADDRESS")
X$ `009if fwd_user .eqs. from then goto okay
X$ breakin:
X$
X$ ! Here means either someone picked our subject field for batch requests
X$ ! inadvertently, or they are pushing their luck...
X$ ! Send the offending mail to me:
X$
X$ `009mail/noself/subject="Invalid batch request received from ''from'" -
X`009`009'message_file' 'fwd_user'
X$
X$ ! ...and quit
X$ `009exit
X$
X$ okay:
X$
X$ ! Now construct a .COM file to run our batch job
X$ ! First some crude error handling:
X$
X$ `009open/write batch_file deliver$dir:B_MAIL_'PID'.COM
X$`009write batch_file "$ set noverify"
X$ `009write batch_file "$ define sys$output deliver$dir:B_Mail_''pid'.Log"
X$ `009write batch_file "$ ex*it = ""goto b_mail_done"""
X$ `009write batch_file "$ error_occured = 0"
X$ `009write batch_file "$ on error then goto catch_error"
X$ `009write batch_file "$ set verify"
X$
X$ ! Now copy the text of the mail accross:
X$
X$ `009open/read input_file 'message_file'
X$ copy_file:
X$ `009read/end_of_file=copy_done input_file the_line
X$ `009write batch_file the_line
X$ `009goto copy_file
X$
X$ ! Now some cleanup code:
X$
X$ copy_done:
X$`009write batch_file "$ set noverify"
X$ `009write batch_file "$ b_mail_done:"
X$ `009write batch_file "$ ex*it:=exit"
X$ `009write batch_file "$ goto cleanup"
X$ `009write batch_file "$ catch_error:"
X$ `009write batch_file "$ error_occured = 1"
X$ `009write batch_file "$ show symbol/all"
X$ `009write batch_file "$ cleanup:"
X$ `009write batch_file "$ deassign sys$output"
X$ `009write batch_file "$ close B_Mail_''pid'.Log"
X$ !
X$ ! Ensure that our batch job cleans up after itself, and tells us of any
X$ ! errors:
X$ !
X$ `009write batch_file "$ submit/noprint/after=""+0:1:00"" -"
X$ `009write batch_file "  `009/nolog -"
X$ `009write batch_file "`009deliver$dir:mail_batch_cleanup.com -"
X$ `009write batch_file "`009/parameter=(''pid')"
X$ `009write batch_file "$ exit"               
X$
X$ ! Close off our files:
X$
X$ `009close input_file
X$ `009close batch_file
X$
X$ ! ...and submit the job:
X
X$
X$ `009submit/noprint/log=deliver$dir:b_mail_'pid'.log -
X`009`009deliver$dir:b_mail_'pid'.com
X$
X$ ! ...and get out.
X$  
X$ `009goto exit
X$
X$ it_broke:
X$
X$ ! There is little we can do if it failed, other than tell ourselves it
X$ ! died:
X$
X$ `009mail/noself/subject="Error occured in ''f$environment(""PROCEDURE"")'" -
X`009`009nl: 'fwd_user'
X$ exit:
X$`009EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
$ GoSub Convert_File
$ File_is="MAIL_BATCH_CLEANUP.COM"
$ Check_Sum_is=1631606231
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$`009SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
X`009  + F$VERIFY(0'MAIL_BATCH_CLEANUP_VERIFY' .GT. 0)
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$!                          
X$`009GOTO END_OF_HEADER_COMMENTS
X$!
X$! A DELIVER utility...
X$
X$! Provides support to MAIL_BATCH.COM
X$
X$! Keith Edmunds, June 11, 1988
X$
X$ 
X$! Edit History:
X$!
X$! 11 June 1988    `009Original version (T.01)
X$! 28 July 1988`009`009Cosmetic changes, tidied up (V1.0)
X$!
X$END_OF_HEADER_COMMENTS:
X$!
X$`009version = "V1.0"
X$!
X$! Reset commands that the user may have redefined.
X$!
X$`009APPEND`009`009= "APPEND"
X$`009CLOSE`009`009= "CLOSE"
X$`009CONVERT`009`009= "CONVERT"
X$`009CREATE`009`009= "CREATE"
X$`009DEASSIGN`009= "DEASSIGN"
X$`009DEFINE`009`009= "DEFINE"
X$`009DELETE`009`009= "DELETE"
X$`009EDIT`009`009= "EDIT"
X$`009EXIT`009`009= "EXIT"
X$`009GOTO`009`009= "GOTO"
X$`009IF`009`009= "IF"
X$`009INQUIRE`009`009= "INQUIRE"
X$`009MAIL`009`009= "MAIL"
X$`009ON`009`009= "ON"
X$`009OPEN`009`009= "OPEN"
X$`009PURGE`009`009= "PURGE"
X$`009READ`009`009= "READ"
X$`009RENAME`009`009= "RENAME"
X$`009SEARCH`009`009= "SEARCH"
X$`009SET`009`009= "SET"
X$`009SORT`009`009= "SORT"
X$`009SPAWN`009`009= "SPAWN"
X$`009SUBMIT`009`009= "SUBMIT"
X$`009TYPE`009`009= "TYPE"
X$`009WAIT`009`009= "WAIT"
X$`009WRITE`009`009= "WRITE"
X$!
X$
X$ `009on error then goto it_broke
X$
X$! Firstly, we mail the logfile from our received batch job back to us:
X$
X$ `009fwd_user = f$trnlnm("DELIVER$FORWARD_ADDRESS")
X$ `009mail/noself/subject="Batch results" deliver$dir:b_mail_'P1'.log -
X`009`009'fwd_user'
X$
X$! Now delete all temporary files we used:
X$                       
X$ `009delete deliver$dir:b_mail_'P1'.*;*
X$ `009goto exit
X$
X$! If something went wrong, tell us where the files are:
X$
X$ it_broke:
X$ `009mail/noself/subject="Do: $ dir DELIVER$DIR:*''pid'" sys$input 'fwd_user'
X$ deck
X`009Your mailed batch job failed somewhere. On your home node, type in the
XDCL command given in the subject of this mail. Examination of any files listed
Xthere will possibly help identify what went wrong.
X$ eod
X$ exit:
X$`009EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
$ GoSub Convert_File
$ File_is="MAKE_MAIL_DELIVERY.COM"
$ Check_Sum_is=1864305038
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$`009SAVE_VERIFY = "''F$ENVIRONMENT("VERIFY_IMAGE")'"*2 + 'F$VERIFY(0)' -
X`009  + F$VERIFY(0'MAKE_MAIL_DELIVERY_VERIFY' .GT. 0)
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$`009ON WARNING THEN GOTO ABNORMAL_EXIT
X$!                          
X$`009VERSION = "V1.0"`009
X$`009GOTO END_OF_HEADER_COMMENTS
X$!        
X$! This procedure builds a MAIL.DELIVERY of your choice 
X$! and provides several helpful hints. Written by Andy Leslie, 27-July-1988
X$!
X$! NOTE: DELIVER *MUST* be installed with privs if Mail.Delivery's created
X$! here are to run unmodified. You should change all V's to D's and W's to F's
X$! otherwise.
X$!
X$! The DELIVER$STARTUP.COM supplied will Install DELIVER with all privs needed
X$!
X$END_OF_HEADER_COMMENTS:
X$!
X$! Reset commands that the user may have redefined.
X$!
X$`009APPEND`009`009= "APPEND"
X$`009CALL`009`009= "CALL"
X$ `009CLOSE`009`009= "CLOSE"
X$`009CONVERT`009`009= "CONVERT"
X$`009CREATE`009`009= "CREATE"
X$`009DEASSIGN`009= "DEASSIGN"
X$`009DEFINE`009`009= "DEFINE"
X$`009DELETE`009`009= "DELETE"
X$`009EDIT`009`009= "EDIT"
X$`009EXIT`009`009= "EXIT"
X$`009GOTO`009`009= "GOTO"
X$`009IF`009`009= "IF"
X$`009INQUIRE`009`009= "INQUIRE"
X$`009MAIL`009`009= "MAIL"
X$`009ON`009`009= "ON"
X$`009OPEN`009`009= "OPEN"
X$`009PURGE`009`009= "PURGE"
X$`009READ`009`009= "READ"
X$`009RENAME`009`009= "RENAME"
X$`009SEARCH`009`009= "SEARCH"
X$`009SET`009`009= "SET"
X$`009SORT`009`009= "SORT"
X$`009SPAWN`009`009= "SPAWN"
X$`009SUBMIT`009`009= "SUBMIT"
X$`009TYPE`009`009= "TYPE"
X$`009WAIT`009`009= "WAIT"
X$`009WRITE`009`009= "WRITE"
X$
X$ `009ESC[0,8]`009= 27
X$
X$ type/PAGE sys$input
X                              MAKE_MAIL_DELIVERY
X
X    This Procedure helps you create a MAIL.DELIVERY file.
X    A MAIL.DELIVERY file is necessary for you to successfully use DELIVER.
X    You may have to re-edit the output before use: see "constraints".
X
X    Hit Control-Y at any time to abort it if you wish.
X
X    Once it has run, sanity check it and then rename the output file from
X    NEW_MAIL.DELIVERY to SYS$LOGIN:MAIL.DELIVERY
X
X    Constraints: 20 Chars for "from"
X                 18 chars for subject, "to"
X                 Addresses containing quotes may not be coped with too well!
X    
X    Wildcards ("*") can be used anywhere.
X
X    Good Luck.
X
X    Andy Leslie, 
X
X$ user = F$GETJPI("","USERNAME")
X$ time = f$time()
X$ node=f$trnlnm("SYS$CLUSTER_NODE")
X$ if node .eqs. "" then node=f$trnlnm("SYS$NODE")
X$ user="''node'''user'"
V$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Cont
Xinue''esc'[m"
X$ write sys$output "''esc'[2J''esc'[H"
X$ write sys$output " "
X$ CLOSE/NOLOG NMD
X$ open/WRITE nmd new_mail.delivery
V$ write nmd "!=================================================================
X============="
X$ write nmd "! MAIL.DELIVERY   "
X$ write nmd "!`009   created by " + "''user' at ''time'"
X$ write nmd "!"
X$ write nmd "! - Parameter file for DELIVER, this MUST be placed in SYS$LOGIN:"
X$ write nmd "!"
V$ write nmd "!=================================================================
X============="
X$ write nmd "!"
X$ write nmd "! Keys:"
X$ write nmd "!"
X$ write nmd "!<from> <to> <sub> <accept> <action> <parameter>"
X$ write nmd "!                     A always"
X$ write nmd "!                     X never"
X$ write nmd "!                     T if all match"
X$ write nmd "!                     F if patterns did not all match"
X$ write nmd "!                     O if all match and no previous match"
X$ write nmd "!                     B if no match and no previous match"
X$ write nmd "!                     E if all match OR no previous match"
X$ write nmd "!<from> <to> <sub>  <accept> <action> <parameter>"
V$ write nmd "!                               A append contents to file <paramet
Xer>"
V$ write nmd "!                               C copy contents to file <parameter
X>"
X$ write nmd "!                               D deliver normally"
X$ write nmd "!                               V deliver using system privilege"
V$ write nmd "!                               E execute command file <parameter>
X"
X$ write nmd "!                               F forward to <parameter>"        
V$ write nmd "!                               W forward using system priv to <pa
Xrameter>"
V$ write nmd "!                               Q quit - no action (Always the las
Xt action for this item)"
X$ write nmd "!"
V$ write nmd "! <from>               <to>  <subject>    <accept> <action>   <par
Xameter>"
X$ write nmd "!"   
X$ type sys$input
X  
X`009If you wish to forward any messages to yourself at a remote
X`009location, please give the address here
X
X$ read sys$command/prompt="Forwarding Address? " forward_addr
X$ type sys$input
X  
X`009If you wish to auto reply using DELIVER_REPLY or DELIVER_MYMAIL
X`009then give a full directory specification here (no logical names)
X`009for the location of these files. You will also be asked where you 
X`009wish logfiles to go, default to both is SYS$LOGIN:.
X
X`009This will create a DELIVER$DIR.COM file to re-create the DELIVER$DIR
X`009and DELIVER$LOG logicals each time DELIVER runs.
X
X        WARNING: DO NOT EDIT or DELETE SYS$LOGIN:DELIVER$DIR.COM EVER!
X
X`009If you have already DEFINED these logicals then the translation of 
X`009this logical will be used.
X
X`009This will always be executed by your DELIVER batch jobs and must 
X`009remain in SYS$LOGIN:.
X
V$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Cont
Xinue''esc'[m"
X$ if f$trnlnm("DELIVER$DIR") .nes. "" then goto dir_known
V$ read sys$command/prompt="Full DELIVER$DIR directory spec: [SYS$LOGIN:] " deli
Xver_dir 
X$
X$Dir_known:
X$
X$ if "''deliver_dir'" .eqs. "" then deliver_dir = f$trnlnm("DELIVER$DIR")
X$
X$ if f$trnlnm("DELIVER$LOG") .nes. "" then goto log_known
V$ read sys$command/prompt="Full DELIVER$LOG directory spec: [SYS$LOGIN:] " deli
Xver_log
X$
X$log_known:
X$                
V$ if f$trnlnm("DELIVER$LOG") .nes. "" then deliver_log = f$trnlnm("DELIVER$LOG"
X) 
X$ close/nolog deliver$dirfile
X$ open/write deliver$dirfile sys$login:deliver$dir.com
V$ write deliver$dirfile "$! DELIVER$DIR.COM - DO NOT EDIT AS THIS WILL BE PURGE
XD!"
X$ write deliver$dirfile "$!`009   created by " + "''user' at ''time'"
X$ if "''deliver_dir'" .eqs. "" then deliver_dir = "SYS$LOGIN:"
X$ if "''deliver_log'" .eqs. "" then deliver_log = "SYS$LOGIN:"
X$ write deliver$dirfile "$ DEFINE DELIVER$DIR ''deliver_dir'"
X$ write deliver$dirfile "$ DEFINE DELIVER$LOG ''deliver_log'"
X$ close/nolog deliver$dirfile
X$ purge sys$login:deliver$dir.com/nolog        
X$ write nmd "!"                 
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!             -*- Define DELIVER$DIR every time -*-"
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!"                 
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ""$@DELIVER$DIR.COM""", -
X"*", "*", "*", "A E")
X$ Call FROM
X$ Call FWD_BY_ADDR
X$ CALL SEND_NEWMAIL
X$ CALL AUTO_REPLY
X$ CALL AUTO_FORWARD
X$ CALL DELIVER
X$ CALL FINISH
X$ goto NORMAL_EXIT
X$
X$
X$ FROM: Subroutine       
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$ type/page sys$input
X                           
X
X`009We will now deal with messages tags on "From's", "To's" and 
X`009Subjects that you do not wish to reply to. Optionally you may
X`009elect to forward them to the forwarding address (if given earlier).
X
X    `009This facility would normally be used to not reply to distribution 
X`009lists and standard messages, such as testing for "*lights* so that
X`009you don't reply to messages about car lights left on in the car park.
X                  
V$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Cont
Xinue''esc'[m"
X$ write sys$output "''esc'[2J''esc'[H"
X$ write sys$output " "
X$
X$ From_i_loop: 
X$ write sys$output " "
X$ inquire yesno  -
X"Do you wish to enter a ""From"" to not auto-reply to? <Y/N>"
X$ if .not. yesno then goto to_i_loop
X$ From_Banner:
X$ write nmd "!"                 
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!                      -*- <from> <to> <sub>  rules -*-"
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!"                 
X$ from_loop:
X$ read/End_Of_File=To_i_loop -
Vsys$command/prompt="""From:"" node? (may include *) [^Z to goto next prompt] " 
Xfrom_node 
X$ From_node = "''From_node'" - "::"
X$ read sys$command/prompt="""From:"" username? (may include *) " From_username 
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)", -
X"""''From_node'::''From_username' *""","*","*","T V")
X$ if forward_addr .eqs. "" then goto from_loop
X$ inquire yesno  -
X"Do you wish this mail to be forwarded to you? <Y/N>"
X$ if yesno then -      
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''forward_addr'", -
X"""''From_node'::''From_username' *""","*","*","T W")
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)", -
X"""''From_node'::''From_username' *""","*","*","T Q")
X$ goto From_loop
X$ !
X$ To_i_loop:                    
X$ write sys$output " "
X$ inquire yesno  -
X"Do you wish to enter a ""To:"" to not auto-reply to? <Y/N>"
X$ if .not. yesno then goto Subject_i_loop
X$
X$ To_loop:
X$ read/End_Of_File=subject_i_loop -
V sys$command/prompt="""To:"" (may be user, * or a distribution list) [^Z to got
Xo next prompt] " to 
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)","*","""''to'""","*","T V")
X$ if forward_addr .eqs. "" then goto to_loop
X$ inquire yesno  -
X"Do you wish This mail to be forwarded to you? <Y/N>"
X$ if yesno then -
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''forward_addr'" -
X,"*","""''to'""","*","T W")
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) " -
X,"*","""''to'""","*","T Q")
X$ goto To_loop
X$
X$ Subject_i_loop:                                  
X$ write sys$output " "
X$ inquire yesno  -
X"Do you wish to enter a subject to not auto-reply to? <Y/N>"
X$ if .not. yesno then goto from_end
X$
X$ Subject_loop:
X$ read/end_of_file=from_end -
Vsys$command/prompt="Subject? (may include *) [^Z to goto next prompt] " Subject
X 
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)", - 
X"*","*","""''subject'""","T V")
X$ if forward_addr .eqs. "" then goto Subject_loop
X$ inquire yesno  -
X"Do you wish This mail to be forwarded to you? <Y/N>"
X$ if yesno then -
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''forward_addr'" -
X,"*","*","""''subject'""","T W")
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)", -
X"*","*","""''subject'""","T Q")
X$ goto Subject_loop
X$ FROM_End:
X$ EndSubroutine              
X$ !               
X$ FWD_BY_ADDR: SUBROUTINE
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$ Fwd_to_3rd:
X$ write sys$output " "
X$ inquire yesno  -
X"Do you wish to enter a subject to be Forwarded to a 3rd party ? <Y/N>"
X$ if .not. yesno then goto fwd_by_addr_end
X
X$
X$Fwd_to_3rd_banner:
X$ write nmd "!" 
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!     -*- Forwarding by Address + Subject to a 3rd Party -*- "
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!"                                             
X$ fwd_to_3rd_loop:             
X$ read/end_of_file= FWD_BY_ADDR_END -
Vsys$command/prompt="Subject? (may include *) [^Z to goto next prompt] " Subject
X              
X$ read/end_of_file=FWD_BY_ADDR_END -
Xsys$command/prompt="3rd Party to forward this mail to? " address
V$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''address'","*","""''subject'
X""","*","T W")
X$ inquire yesno  -                
X"Do you wish this mail to be discarded after delivery to the 3rd Party? <Y/N>"
X$ if .not. yesno then -      
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ", -
X"*","""''subject'""","*","T V")
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ", -
X"*","""''subject'""","*","T Q")
X$ goto fwd_to_3rd_loop
X$ FWD_BY_ADDR_END:
X$ ENDSUBROUTINE
X$                                 
X$ AUTO_FORWARD: SUBROUTINE
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$ Forward_i_Loop:            
X$ if forward_addr .eqs. "" then goto auto_forward_end
X$ write sys$output " "
X$ inquire yesno  -                
X"Do you wish to auto-forward remaining messages? <Y/N>"
X$ if .not. yesno then goto AUTO_FORWARD_END
X$
X$ Forward_Banner:
X$ write nmd "!" 
V$ write nmd "!-----------------------------------------------------------------
X------------"
V$ write nmd "!                          -*- Forwarding Address -*-             
X            "
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!"
X$ Forward_loop:
V$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ''forward_addr'","*","*","*",
X"A W")
X$ !                        
X$ AUTO_FORWARD_END:
X$ ENDSUBROUTINE
X$
X$ AUTO_REPLY: SUBROUTINE
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$ write sys$output " "
X$
X$ inquire yesno "Do you wish to auto-reply to any mail? <Y/N>"
X$ if .not. yesno then goto AUTO_REPLY_END
X$
X$ write nmd "! "
V$ write nmd "!-----------------------------------------------------------------
X------------"
V$ write nmd "!                                Reply to mail                    
X            "
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!"
V$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS) ""$@deliver$dir:DELIVER_REPLY
X.COM/OUT=deliver$dir:DELIVER_REPLY.LOG""" , -
X"*","*","*","T E")
X$ AUTO_REPLY_END:
X$ ENDSUBROUTINE
X$                                              
X$ SEND_NEWMAIL: SUBROUTINE
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$ Allmail_i_loop:                                         
X$ type sys$input                        
X
X`009You can specify a subject that, if it is used, will cause the 
X`009DELIVER_MYMAIL procedure to be invoked, which will send ALL NEW 
X`009MAIL to the sender.
X
X`009You might use this if you have a borrowed account and don't wish to
X `009auto-forward your mail to it.
X
X$ inquire yesno "Do you wish to be able to use this facility? <Y/N>"
X$ if .not. yesno then goto send_newmail_end
X$ Allmail_Loop:
X$ write nmd "! "
V$ write nmd "!-----------------------------------------------------------------
X------------"
V$ write nmd "!                                Send on all newmail              
X            "
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!"
X$  read sys$command/prompt="Chosen Subject (be obtuse!)? " subject 
X$ write nmd -
Vf$fao("!(20AS) !(18AS) !(18AS) !(4AS) ""$@deliver$dir:DELIVER_MYMAIL.COM/OUT=de
Xliver$dir:DELIVER_MYMAIL.LOG""" ,-
X "*","*","""''subject'""","T E")
X$ write nmd -
X"! Don't bother to deliver this mail" 
X$ write nmd -
Xf$fao("!(20AS) !(18AS) !(18AS) !(4AS)" , "*","*","""''subject'""","T Q")
X$ SEND_NEWMAIL_END:
X$ ENDSUBROUTINE
X$ 
X$ FINISH: SUBROUTINE 
X$`009ON CONTROL_Y THEN GOTO ABNORMAL_EXIT
X$ Quit:                                                   
X$ write nmd "!"
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!               Finally, explicitely quit              "
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!"
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)","*", "*", "*", "T Q")
X$ write nmd "! End"
X$ FINISH_END:
X$ ENDSUBROUTINE
X$
X$DELIVER: SUBROUTINE
X$ write nmd "!"
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!               Deliver all mail not quit from by this point"
V$ write nmd "!-----------------------------------------------------------------
X------------"
X$ write nmd "!"
X$ write nmd f$fao("!(20AS) !(18AS) !(18AS) !(4AS)","*", "*", "*", "T V")
X$DELIVER_END:
X$ ENDSUBROUTINE
X$!
X$! Normal exit.
X$!
X$NORMAL_EXIT:
X$`009STATUS = $STATUS`009`009`009!save success status
X$ type/page sys$input  
X
X`009Now review the NEW_MAIL.DELIVER file. If it is correct, rename it
X`009to SYS$LOGIN:MAIL_DELIVER.COM. Ensure that DELIVER_REPLY.COM,
X        DELIVER_REPLY.TXT and DELIVER_MYMAIL.COM are in place and MAIL 
X`009SET FORW DELIVER%USERNAME.
X
X`009DELIVER$DIR:DELIVER.HISTORY holds a history of all those replied to.
X`009
X`009You may wish to delete this at the end of each absence.
X
X`009- Don't forget to test this before setting it going!
X
V$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Cont
Xinue''esc'[m"
X$ write sys$output "''esc'[2J''esc'[H"
X$ write sys$output " "
X$`009GOTO EXIT
X$!
X$! Abnormal exit.
X$!
X$ABNORMAL_EXIT:
X$`009STATUS = $STATUS`009`009`009!save failure status
X$`009IF STATUS THEN STATUS = "%X0800002A"`009!force error if neccessary
X$ type/page sys$input
X
X`009Procedure has aborted
X
X`009Your NEW_MAIL.DELIVER file will be in an unfinished state
X
V$ inquire/nopunc dummy "                          ''esc'[7mPress RETURN to Cont
Xinue''esc'[m"
X$ write sys$output "''esc'[2J''esc'[H"
X$ write sys$output " "
X$`009GOTO EXIT
X$!
X$! Exit.
X$!
X$EXIT:
X$
X$ `009CLOSE/NOLOG NMD
X$ `009Write SYS$OUTPUT " "
X$`009Write SYS$OUTPUT "All Done"
X$ `009Write SYS$OUTPUT " "
X$`009EXIT ('STATUS'.OR.%X10000000) + F$VERIFY(SAVE_VERIFY,SAVE_VERIFY/2)*0
$ GoSub Convert_File
$ File_is="NO_BATCH_LOG.PAS"
$ Check_Sum_is=1732278730
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
Xvar
X  batch_log   : [readonly] string := '_NLA0:';
$ GoSub Convert_File
$ File_is="TRIP.COM"
$ Check_Sum_is=87527804
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X$ ! TRIP.COM - Send a reply saying this user is on vacation.
X$ ! Written by Ned Freed, 19-Oct-1985
X$ !
X$ ! TRIP.COM sends the file SYS$LOGIN:TRIP.TXT to any user
X$ ! who sends mail. Only one copy of TRIP.TXT is sent to a given
X$ ! user no matter how many messages he or she sends.
X$ !
X$ ! Check for replies sent list's presence
X$ if F$SEARCH("sys$login:trip.reply") .nes "" then goto search_list
X$ open/write tr sys$login:trip.reply
X$ goto add_list
X$ !
X$ ! Check the list for this sender
X$ search_list:
X$ search/nooutput sys$login:trip.reply "''QFROM'"
X$ if $status .eq. 1 then exit
X$ open/append tr sys$login:trip.reply
X$ !
X$ add_list:
X$ write tr FROM
X$ close tr
X$ !
X$ mail/noself/subject="Away on a trip" trip.txt "''F$ELEMENT(0," ",QFROM)'"
X$ exit
$ GoSub Convert_File
$ File_is="TRIP.DELIVERY"
$ Check_Sum_is=1577876203
$ Copy SYS$Input VMS_SHAR_DUMMY.DUMMY
X! TRIP.DELIVERY - Send a reply saying this user is on vacation. Only
X!                 send one copy to each sender, regardless of how many
X!                 messages they send.
X! Written by Ned Freed, 19-Oct-1985
X!
X* * * A E @TRIP.COM
X* * * A D
$ GoSub Convert_File
$ Exit
