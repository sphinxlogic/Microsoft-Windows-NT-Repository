(* DELIVER.PAS - General MAIL delivery manager for VMS MAIL.

Modification History
 
   Written by Ned Freed, 23-Sep-1985, modified 30-Apr-1986.
   Mail dispatch interface originally written by Kevin Carosso.
   Some modifications by Sheldon Smith, December 1986.
   Rewrite by Ned Freed to use new $GETUAI system service, 15-Dec-1986.
   This change requires the use of VMS 4.4 and Pascal 3.4.
            
   Modified by Andy Leslie to run under VMS X5.0-3N2 21-Dec-1987
   - Changed destination to 0::_username - _username has the "_"
     stripped by VMS otherwise. Also, V5 changes to MAIL meant
     I had to move the "! sent to ..." string to the subject field.
    
   Modified by Andy Leslie to run under VMS V5.0-1 20-Jul-1988
   - Re-ordered $SNOJBC item list so that Username is first. This
     is to allow for a feature of the VMS Job Controller in 5.0-1
     Built and tested on VMS V5.0-1 using a FT version of PASCAL and 
     PASSTR.

   Modified by Rich Whalen 11-Feb-1993
   - The lengths of the device string and the device with directory
     string were insufficent, extended to 31 and 79 respectively.

Caveat
   It is believed that it will work on versions of VMS prior to VMS V5, 
   but this is untested by me, AJL 23-7-88

What is DELIVER?
     
   DELIVER provides a general-purpose MAIL delivery manager similar
   to the MMDF-II MAILDELIVERY system. DELIVER makes it possible
   for users to set up a file containing screening information that
   automatically deals with each incoming message. Actions may be
   selectively taken by DELIVER based on information in the From:,
   To: and Subject: fields of the message header.
 
   DELIVER operates as a foreign mail interface to VMS MAIL and is
   invoked with a call to LIB$FIND_IMAGE_SYMBOL in MAIL. The shareable
   image containing this code should be placed in SYS$LIBRARY as the file
   DELIVER_MAILSHR.EXE).
 
   Users may activate DELIVER by setting their forwarding address
   to DELIVER%username, where "username" is the user's own user name.
   In order for DELIVER to perform any useful function a file called
   MAIL.DELIVERY must be present in the user's default login directory.
 
   DELIVER's operation is only meaningful in outgoing mode; however, rather
   than waste a possible incoming MAIL interface, DELIVER implements a
   rudimentary mail posting mechanism on the incoming side that can be used
   to send messages contained in data files. 

   If privileges are not granted by you using the DELIVER$STARTUP.COM
   provided, SYSNAM and CMKRNL will be required to run DELIVER in this mode. 
   You may wish to do this  since it is possible to forge From: 
   addresses using this mechanism.
 
  Note:
    The foreign protocol interface within MAIL is undocumented at
    this time.  It may change without notice in a future release
    of VMS. Indeed it already has, once.
 
    The information necessary to write this code comes from the MAIL
    source on the VMS microfiche.  The most useful information is the
    routine NETJOB in module MAIL$MAIL (230-E2), which handles incoming
    foreign mail, and the various routines in module NETSUBS (230-N11),
    most of which deal with outgoing foreign mail.

Gotchas

    File Protections:
    
    It seems that SYS$SHARE:PASRTL.EXE must be (W:RE)
                  SYS$SYSTEM:VMSMAIL_PROFILE.DATA must be (W:RW) 
                  SYS$SYSTEM:*MAIL*.EXE must be (W:RE) 
                  SYS$MESSAGE:*.MSG must be (W:RE)
    		  SYS$SYSTEM:SEARCH.EXE must be (W:RE)
    
    DON'T forget to Edit MAIL.DELIVERY and the DELIVER_REPLY.COM files
    to suit you or it'll all fall apart.
  
    Don't forget to create the DELIVER$DIR directory and put DELIVER_REPLY.COM
    plus DELIVER_REPLY.TXT there. DELIVER.HISTORY will be created there too.
    
    Finally, if all else fails, SET AUDIT/ALARM/ENABLE=ALL and watch
    the messages fly past! Sooner or later you'll see what the problems
    are.

*)
 
[inherit ('SYS$LIBRARY:STARLET')] module deliver (output);
 
const
  (* Debugging control flags *)
  DEBUG_IN  = FALSE;            (* Debug messages produced by receive code. *)
  DEBUG_OUT = FALSE;		(* Debug messages produced by send code.    *)
 
  (* Switch for use of SJC$_USER_IDENTIFICATION *)
  use_sjc_user_identification = false;

  LNK_C_OUT_CONNECT  = 0;       (* MAIL protocol link actions.           *)
  LNK_C_OUT_SENDER   = 1;       (* These are defined in MAILSHR.MAR      *)
  LNK_C_OUT_CKUSER   = 2;       (* but because we cannot have external   *)
  LNK_C_OUT_TO       = 3;       (* constants in Pascal, they are         *)
  LNK_C_OUT_SUBJ     = 4;       (* redefined here.                       *)
  LNK_C_OUT_FILE     = 5;
  LNK_C_OUT_CKSEND   = 6;
  LNK_C_OUT_DEACCESS = 7;
 
  LNK_C_IN_CONNECT = 8;
  LNK_C_IN_SENDER  = 9;
  LNK_C_IN_CKUSER  = 10;
  LNK_C_IN_TO      = 11;
  LNK_C_IN_SUBJ    = 12;
  LNK_C_IN_FILE    = 13;
 
  LNK_C_IO_READ  = 14;
  LNK_C_IO_WRITE = 15;
 
  LNK_C_IN_CC = 16;
  LNK_C_OUT_CC = 17;

  LNK_C_IN_ATTRIBS = 18;
  LNK_C_OUT_ATTRIBS = 19;

  parameter_size     = 512;     (* Size of a single parameter in the
                                   MAIL.DELIVERY file. This is also the
                                   maximum size of lines read from any file. *)
  max_parameters     = 7;       (* Maximum number of parameters that
                                   can appear on a line in a
                                   MAIL.DELIVERY file *)
  min_parameters     = 5;       (* Minimum number of parameters that
                                   can appear on a line in a
                                   MAIL.DELIVERY file *)
  from_parameter     = 1;       (* Position of the From: parameter *)
  to_parameter       = 2;       (* Position of the To: parameter *)
  subject_parameter  = 3;       (* Position of the Subject: parameter *)
  decision_parameter = 4;       (* Position of the decision parameter *)
  action_parameter   = 5;       (* Position of the action parameter *)
  argument1_parameter = 6;       (* Position of the first argument parameter *)
  argument2_parameter = 7;       (* Position of the second argument parameter *)
 
  stack_size = 10;              (* State mach. stack for messages from MAIL *)

  device_string_len = 31;
  directory_string_len = 79;
 
  DCL_line_size = 256;          (* Maximum possible line allowed by DCL. *)

  big_size = 1024;              (* Longest text line allowed in mail. *)

  UAF$S_DEFDEV = 32;            (* Size of fields in SYSUAF.DAT. *)
  UAF$S_DEFDIR = 64;
  UAF$S_ACCOUNT = 32;

type
  $ubyte = [byte] 0..255;
  $uword = [word] 0..65535;
  string = varying [parameter_size] of char;
  big_string = varying [big_size] of char;
 
  (* A string descriptor type used to handle the descriptors MAIL hands to
     DELIVER. *)
  longest_possible_string = packed array [1..65536] of char;
  string_descriptor = record
                        length : [word] 0..65535;
                        dclass, dtype : [byte] 0..255;
                        address : ^longest_possible_string;
                      end;
 
  (* Storage for a single line of MAIL.DELIVERY file information *)
  parameter_block_ptr = ^parameter_block;
  parameter_block = record
                      parameters  : array [1..max_parameters] of string;
                      next        : parameter_block_ptr;
                      any_from    : boolean;
                      any_to      : boolean;
                      any_subject : boolean;
                    end; (* parameter_block record *)
 
  account_name = packed array [1..UAF$S_ACCOUNT] of char;
  user_name = packed array [1..12] of char;
  priorities = [byte] 0..255;
 
  (* Storage for information about a single recipient. The uic, username,
     account and priority fields are ordered to match the format of a
     SJC$_USER_IDENTIFICATION buffer and cannot be reordered. *)
  user_block_ptr = ^user_block;
  user_block = record
                 uic         : unsigned;
                 username    : user_name;
                 account     : account_name;
                 priority    : [byte] 0..255;
                 user_length : [word] 0..65535;
                 directory   : string;
                 rules_list  : parameter_block_ptr;
                 next        : user_block_ptr;
                 copyname    : varying [29] of char;
               end; (* user_block record *)
 
  (* Possible reasons why MAIL_IO_WRITE will be called. *)
  write_states = (bad_msg, user_check, delivery_check, error_msg);
 
  (* A stack structure to store information about pending calls to
     MAIL_IO_WRITE. *)
  write_state_stack = record
                        top : integer;
                        store : array [1..stack_size] of write_states;
                      end; (* write_state_check record *)
 
  (* Record for VMS item lists. *)
  item = record
           len    : [word] 0..65535;
           code   : [word] 0..65535;
           addr   : [long] integer;
           rlen   : [long] integer;
         end; (* item record *)

  file_attribute_block = array [0..1] of integer;
 
var
  default_batch_queue : [static, readonly] string := 'MAIL$BATCH';
  system_batch_queue : [readonly] string := 'SYS$BATCH';
  batch_log : [static] string; batch_keep : [static] boolean;
 
  (* Storage for message header information on the outgoing side *)
  tostring, fromstring, subjectstring, ccstring : [static] string;

  folder_invocation : [static] string;

  (* List of active recipients and associated information *)
  user_list, user_list_last : [static] user_block_ptr;
  user_count : [static] integer := 0;
 
  from_owner : [static] boolean;
 
  (* Storage for accumulated To: line for incoming messages. *)
 
  toline : [static] string;
 
  (* The state machine for MAIL status information. *)
 
  write_recv_states  : write_state_stack;
  last_error, sticky_error : integer;
 
  (* Error message codes defined in DELIVER_ERR.MSG *)
  DELIVER__GOTNOSYSPRV, DELIVER__NOTPRIV,
  DELIVER__CANACCUAF, DELIVER__NOSUCHUSER, DELIVER__NAMETOOLONG,
  DELIVER__NODEFAULTDIR, DELIVER__TOOMANYPARAMS, DELIVER__TOOFEWPARAMS,
  DELIVER__NOMDFILE, DELIVER__MDIGNORED, DELIVER__NORULES, DELIVER__MESREAERR,
  DELIVER__GETFILERR, DELIVER__MESWRTERR, DELIVER__INTSTKOVR, DELIVER__STKEMPTY,
  DELIVER__BADSTKELE, DELIVER__MESOPNERR, DELIVER__MSGWRTERR,
  DELIVER__MSGREAERR, DELIVER__USERNOEXIST : [external, value] integer;
 
  MAIL$C_PROT_MAJOR : [external, value] integer;

  arm_write_access : [static, readonly] unsigned := ARM$M_WRITE;

(* Routine to get job/process information *)

function LIB$GETJPI (
  item_code : integer; var process_id : unsigned := %immed 0;
  process_name : [readonly] varying [u1] of char := %immed 0;
  var out_value : unsigned := %immed 0;
  var out_string : varying [u2] of char := %immed 0;
  var out_len : integer := %immed 0) : integer; external;

(* Routine to signal errors *)
 
procedure LIB$SIGNAL (%IMMED stat : [list, unsafe] integer); extern;
 
(* Routine to read command line arguments *)
 
function CLI$GET_VALUE (name : varying [max1] of char;
  var val : varying [max2] of char) : integer; extern;
 
(* Routine to get symbol values *)
 
function LIB$GET_SYMBOL (name : varying [max1] of char;
  var result : varying [max2] of char) : integer; extern;
 
(* Routine to set symbol values *)
 
function LIB$SET_SYMBOL (name : varying [max1] of char;
  svalue : varying [max2] of char) : integer; extern;
 
function STR$UPCASE (var dststr : varying [l1] of char;
  var srcstr : varying [l2] of char) : integer; extern;

(* create_with_SYSPRV is a Pascal user-action routine for OPEN statements.
   It enables SYSPRV while doing certain OPEN's so we can write files into
   user directories. *)
 
function create_with_SYSPRV (var fab : FAB$TYPE;
                             var rab : RAB$TYPE;
                             var fil : text) : integer;
type
  protection = packed array [0..15] of boolean;

var
  stat, i : integer;
  ppriv, priv : [quad] array [0..1] of unsigned;
  xabptr : ^XAB$TYPE;
 
begin (* create_with_SYSPRV *)
  if DEBUG_OUT then writeln ('create_with_SYSPRV called.');

  priv[0] := PRV$M_SYSPRV; priv[1] := 0;
  stat := $SETPRV (ENBFLG := 1, PRVADR := priv, PRMFLG := 0, PRVPRV := ppriv);
  if odd (stat) then begin
    FAB.FAB$V_LNM_MODE := PSL$C_EXEC;
    new (xabptr);
    xabptr^.XAB$L_ACLBUF := 0;
    xabptr^.XAB$W_ACLSIZ := 0;
    xabptr^.XAB$B_BLN := XAB$K_PROLEN;
    xabptr^.XAB$B_COD := XAB$C_PRO;
    xabptr^.XAB$L_NXT := fab.FAB$L_XAB;
    (* s:rwed, o:rwed, g, w *)
    FOR i := 0 TO 7 DO xabptr^.XAB$W_PRO::protection[i] := false;
    FOR i := 8 TO 15 DO xabptr^.XAB$W_PRO::protection[i] := true;
    xabptr^.XAB$L_UIC := 0;
    stat := $CREATE (FAB);
    if odd (stat) then stat := $CONNECT (RAB);
    fab.FAB$L_XAB := xabptr^.XAB$L_NXT;
    dispose (xabptr);
  end
  else
    if DEBUG_OUT then writeln ('Error setting privs for create_with_SYSPRV');  
  priv[0] := uand (priv[0], unot (ppriv[0]));
  priv[1] := uand (priv[1], unot (ppriv[1]));
  $SETPRV (ENBFLG := 0, PRVADR := priv, PRMFLG := 0);
  create_with_SYSPRV := stat;
end; (* create_with_SYSPRV *)
 
(* open_with_SYSPRV is a Pascal user-action routine for OPEN statements.
   It enables SYSPRV while doing certain OPEN's so we can read system
   files. *)
 
function open_with_SYSPRV (var fab : FAB$TYPE;
                           var rab : RAB$TYPE;
                           var fil : text) : integer;
var
  stat : integer; ppriv, priv : [quad] array [0..1] of unsigned;
 
begin (* open_with_SYSPRV *)
  if DEBUG_OUT then writeln ('open_with_SYSPRV called.');
  priv[0] := PRV$M_SYSPRV; priv[1] := 0;
  stat := $SETPRV (ENBFLG := 1, PRVADR := priv, PRMFLG := 0, PRVPRV := ppriv);
  if odd (stat) then begin
    FAB.FAB$V_LNM_MODE := PSL$C_EXEC; stat := $OPEN (FAB);
    if odd (stat) then stat := $CONNECT (RAB);
  end;
  priv[0] := uand (priv[0], unot (ppriv[0]));
  priv[1] := uand (priv[1], unot (ppriv[1]));
  $SETPRV (ENBFLG := 0, PRVADR := priv, PRMFLG := 0);
  open_with_SYSPRV := stat;
end; (* open_with_SYSPRV *)
 
(* find_user_directory locates a user in the system authorization file
   and returns his or her default login directory (which is where a
   MAIL.DELIVERY file must reside). find_user_directory also returns
   the user's UIC and account since this information will be needed
   for creating the delivery batch job. *)
 
function find_user_directory (username : varying [l1] of char;
  var user_directory : string; var user_uic : unsigned;
  var user_account : account_name; var priority : priorities) : boolean;
 
var
  device_with_prefix : packed array [1..device_string_len] of char;
  directory_with_prefix : packed array [1..directory_string_len] of char;
  items : array [1..6] of item; stat : integer;
  ppriv, priv : [quad] array [0..1] of unsigned;
 
begin (* find_user_directory *)
  if DEBUG_OUT then writeln ('find_user_directory called.');
  find_user_directory := false;
  if length (username) > 12 then
    LIB$SIGNAL (DELIVER__NAMETOOLONG, 2,
                username.length, iaddress (username.body))
  else begin
    with items[1] do begin
      len := size (user_uic); code := UAI$_UIC;
      addr := iaddress (user_uic); rlen := 0;
    end; (* with *)
    with items[2] do begin
      len := UAF$S_ACCOUNT; code := UAI$_ACCOUNT;
      addr := iaddress (user_account); rlen := 0;
    end; (* with *)
    with items[3] do begin
      len := device_string_len; code := UAI$_DEFDEV;
      addr := iaddress (device_with_prefix); rlen := 0;
    end; (* with *)
    with items[4] do begin
      len := UAF$S_DEFDIR; code := UAI$_DEFDIR;
      addr := iaddress (directory_with_prefix); rlen := 0;
    end; (* with *)
    with items[5] do begin
      len := size (priority); code := UAI$_PRI;
      addr := iaddress (priority); rlen := 0;
    end; (* with *)
    with items[6] do begin
      len := 0; code := 0; addr := 0; rlen := 0;
    end; (* with *)
    (* Enable SYSPRV to check for valid user recipient-name. *)
    priv[0] := PRV$M_SYSPRV; priv[1] := 0;
    $SETPRV (ENBFLG := 1, PRVADR := priv, PRMFLG := 0, PRVPRV := ppriv);
    stat := $GETUAI (usrnam := username, itmlst := items);
    if stat = SS$_NOPRIV then LIB$SIGNAL (DELIVER__CANACCUAF)
    else if stat = SS$_NOSYSPRV then
      LIB$SIGNAL (DELIVER__GOTNOSYSPRV, 2, username.length,
                  iaddress (username.body))
    else if not odd (stat) then
      LIB$SIGNAL (DELIVER__NOSUCHUSER, 2,
                  username.length, iaddress (username.body))
    else begin
      user_directory := substr (device_with_prefix, 2,
                                ord (device_with_prefix[1])) +
                        substr (directory_with_prefix, 2,
                                ord (directory_with_prefix[1]));
      if DEBUG_OUT then writeln ('  Default directory: "',
                                 user_directory, '".');
      if DEBUG_OUT then writeln ('  Account: "', user_account, '".');
      if DEBUG_OUT then writeln ('  UIC: ', hex (user_uic, 8, 8), '.');
      if length (user_directory) <= 0 then
        LIB$SIGNAL (DELIVER__NODEFAULTDIR, 2,
                    username.length, iaddress (username.body))
      else find_user_directory := true;
      (* Disable and reestablish former privs. *)
      priv[0] := uand (priv[0], unot (ppriv[0]));
      priv[1] := uand (priv[1], unot (ppriv[1]));
      $SETPRV (ENBFLG := 0, PRVADR := priv, PRMFLG := 0);
    end;
  end;
end; (* find_user_directory *)
 
(* copy_descr_to_string copies a MAIL string (passed by descriptor) into
   a Pascal VARYING string. *)
 
procedure copy_descr_to_string (
  var mail_string : string_descriptor;
  var out_string : string; DEBUG_ON : boolean);
 
var
  index : integer;
 
begin (* copy_descr_to_string *)
  if DEBUG_ON then writeln ('copy_descr_to_string called.');
  out_string := '';
  if mail_string.length > 256 then index := 256
  else index := mail_string.length;
  for index := 1 to mail_string.length do
    out_string := out_string + mail_string.address^[index];
  if DEBUG_ON then writeln ('  String copied: "', out_string, '".');
end; (* copy_descr_to_string *)
 
(* copy_string_to_descr copies a Pascal VARYING string into a MAIL string
   (passed by descriptor). *)
 
procedure copy_string_to_descr (
  var in_string : string;
  var mail_string : string_descriptor; DEBUG_ON : boolean);
 
  [asynchronous, unbound, external (LIB$SCOPY_DXDX)]
  function copy_string (var src : varying [max1] of char;
    var dst : string_descriptor) : integer; extern;
 
begin (* copy_string_to_descr *)
  if DEBUG_ON then writeln ('copy_string_to_descr called.');
  copy_string (in_string, mail_string);
  if DEBUG_ON then writeln ('  String copied: "', in_string, '".');
end; (* copy_string_to_descr *)
 
(* charupper is a simple function to convert characters to upper case.
   The full DEC Multinational Character Set is accomodated. *)
 
function charupper (ch : char) : char;
 
begin (* charupper *)
  if (ch >= 'a') and (ch <= 'z') then
    charupper := chr (ord (ch) + (ord ('A') - ord ('a')))
  else if (ord (ch) >= 224) and (ord (ch) <= 253) then
    charupper := chr (ord (ch) + (192 - 224))
  else charupper := ch;
end; (* charupper *)
 
(* dispose_rules_list disposes of heap storage associated with
   a list of parameter blocks. *)
 
procedure dispose_rules_list (var rules_list : parameter_block_ptr);
 
var
  temp_list : parameter_block_ptr;
 
begin (* dispose_rules_list *)
  if DEBUG_OUT then writeln ('dispose_rules_list called.');
  while rules_list <> nil do begin
    temp_list := rules_list; rules_list := rules_list^.next;
    dispose (temp_list);
  end; (* while rules_list <> nil *)
end; (* dispose_rules_list *)
 
(* dispose_user_list disposes of heap storage associated with
   a list of user name blocks. *)
 
procedure dispose_user_list (var user_list : user_block_ptr);
 
var
  temp_list : user_block_ptr;
 
begin (* dispose_user_list *)
  if DEBUG_OUT then writeln ('dispose_user_list called.');
  while user_list <> nil do begin
    temp_list := user_list; user_list := user_list^.next;
    dispose_rules_list (temp_list^.rules_list); dispose (temp_list);
  end; (* while user_list <> nil *)
end; (* dispose_user_list *)
 
(* read_maildelivery_file reads the contents of a MAIL.DELIVERY file
   and creates a rules_list structure. Any errors are signalled
   by returning FALSE. *)
 
function read_maildelivery_file (var dfile : text;
  var rules_list : parameter_block_ptr) : boolean;
 
label
  99;
 
var
  current, last : parameter_block_ptr; quoted : boolean;
  pindex, lindex, rindex, lcount : integer; line : string;
 
  procedure addch (ch : char);
 
  label
    88;
 
  var
    cindex : integer;
 
  begin (* addch *)
    if pindex > max_parameters then begin
      if FROM_OWNER then LIB$SIGNAL (DELIVER__TOOMANYPARAMS, 1, lcount);
      goto 99;
    end;
    if current = nil then if (ch = '!') or (ch = ';') then begin
      if DEBUG_OUT then writeln ('  Skipping comment line.');
      lindex := length (line); goto 88;
    end else begin
      new (current);
      with current^ do begin
        for cindex := 1 to max_parameters do parameters[cindex] := '';
        next := nil;
      end; (* with current^ *)
      rindex := rindex + 1;
      if DEBUG_OUT then writeln (' Rule #', rindex:0, '.');
      if last = nil then begin
        last := current; rules_list := current;
      end else begin
        last^.next := current; last := current;
      end;
    end;
    current^.parameters[pindex] := current^.parameters[pindex] + ch;
  88:
  end; (* addch *)
 
begin (* read_maildelivery_file *)
  if DEBUG_OUT then writeln ('read_maildelivery_file called.');
  read_maildelivery_file := false; last := nil; lcount := 0;
  rindex := 0;
  while not eof (dfile) do begin
    readln (dfile, line); lcount := lcount + 1;
    if DEBUG_OUT then writeln ('  Line from MAIL.DELIVERY: "', line, '".');
    pindex := 1; current := nil; lindex := 1; quoted := false;
    while lindex <= length (line) do begin
      if (not quoted) and (line[lindex] in [' ', chr (9)]) then begin
        if current <> nil then if pindex <= max_parameters then
          if length (current^.parameters[pindex]) > 0 then begin
            if DEBUG_OUT then writeln ('  Parameter #', pindex:0, ' is: "',
                                       current^.parameters[pindex], '".');
            pindex := pindex + 1;
          end;
      end else if line[lindex] = '"' then begin
        if length (line) > lindex then
          if line[lindex+1] = '"' then begin
            addch ('"'); lindex := succ (lindex);
          end else quoted := not quoted
        else quoted := not quoted;
      end else if quoted and (pindex > 5) then addch (line[lindex])
      else addch (charupper (line[lindex]));
      lindex := lindex + 1;
    end; (* while not eoln *)
    if current <> nil then with current^ do begin
      if pindex <= max_parameters then
        if length (parameters[pindex]) > 0 then begin
          if DEBUG_OUT then writeln ('  Parameter #', pindex:0, ' is: "',
                                     parameters[pindex], '".');
          pindex := pindex + 1;
        end;
      pindex := pindex - 1;
      if pindex < min_parameters then begin
        if FROM_OWNER then LIB$SIGNAL (DELIVER__TOOFEWPARAMS, 1, lcount);
        goto 99;
      end;
      any_from      := parameters[from_parameter]      = '*';
      any_to        := parameters[to_parameter]        = '*';
      any_subject   := parameters[subject_parameter]   = '*';
      if parameters[subject_parameter] = '"' then
        parameters[subject_parameter] := '';
    end;
  end; (* while not eof *)
  if FROM_OWNER and (rules_list = nil) then
    LIB$SIGNAL (DELIVER__NORULES)
  else read_maildelivery_file := true;
  99:
  close (dfile);
end; (* read_maildelivery_file *)
 
(* MAIL_OUT_CONNECT is called by VMS MAIL to initiate a send operation. *)
 
[global] function MAIL_OUT_CONNECT (var context : unsigned;
  var link_flag : integer;
  var protocol, node : string_descriptor;
  var log_link_error : integer;
  var file_RAT, file_RFM : integer;
  var MAIL$GL_FLAGS : integer;
  var attached_file : string_descriptor) : integer;
 
var
  stat : integer;

begin (* MAIL_OUT_CONNECT *)
  if DEBUG_OUT then writeln ('MAIL_OUT_CONNECT called.');
  fromstring := ''; tostring := ''; ccstring := '';
  subjectstring := ''; user_list_last := nil;
  stat := $TRNLOG (lognam := 'PMDF_ROOT',
                   rslbuf := folder_invocation.body,
                   rsllen := folder_invocation.length);
  if (not odd (stat)) or (stat = SS$_NOTRAN) then
    folder_invocation := '$ FOLDER'
  else folder_invocation := '$ PMDF FOLDER';
  MAIL_OUT_CONNECT := SS$_NORMAL;
end; (* MAIL_OUT_CONNECT *)
 
(* MAIL_OUT_LINE is called by VMS MAIL whenever a single line of stuff
   must be delivered to the DELIVER mail relay. *)
 
[global] function MAIL_OUT_LINE (var context : unsigned;
  var link_flag : integer;
  var node, line : string_descriptor) : integer;
 
begin (* MAIL_OUT_LINE *)
  if DEBUG_OUT then writeln ('MAIL_OUT_LINE called.');
  case iaddress (link_flag) of
    (* MAIL is delivering a To: address *)
    LNK_C_OUT_TO     : begin
                         if DEBUG_OUT then writeln ('  OUT_TO option used.');
                         copy_descr_to_string (line, tostring, DEBUG_OUT);
                       end; (* LNK_C_OUT_TO *)
    (* MAIL is delivering a From: address *)
    LNK_C_OUT_SENDER : begin
                         if DEBUG_OUT then
                           writeln ('  OUT_SENDER option used.');
                         copy_descr_to_string (line, fromstring, DEBUG_OUT);
                       end; (* LNK_C_OUT_SENDER *)
    (* MAIL is delivering a Subject: line *)
    LNK_C_OUT_SUBJ   : begin
                         if DEBUG_OUT then writeln ('  OUT_SUBJ option used.');
                         copy_descr_to_string (line, subjectstring,
                                               DEBUG_OUT);
                       end; (* LNK_C_OUT_SUBJ *)

    otherwise begin
                         if DEBUG_OUT then writeln ('  Otherwise option used.', link_flag);
	      end;
  end; (* case *)
  MAIL_OUT_LINE := SS$_NORMAL;
end; (* MAIL_OUT_LINE *)
 
(* MAIL_OUT_CHECK is called once with each addressee for the current
   message and once again after the message body has been sent. *)
 
[global] function MAIL_OUT_CHECK (
  var context : unsigned;
  var link_flag : integer;
  var protocol, addressee : string_descriptor;
  procedure MAIL$READ_ERROR_TEXT) : integer;
 
var
  usernamebuffer, userdirectory : string; userpriority : priorities;
  maildelivery : text; useruic : unsigned; useraccount : account_name;
  currenttime : [quad] record
                         l0, l1 : unsigned;
                       end;
 
begin (* MAIL_OUT_CHECK *)
  if DEBUG_OUT then writeln ('MAIL_OUT_CHECK called.');
  case iaddress (link_flag) of
    (* Check out an addressee *)
    LNK_C_OUT_CKUSER : if (addressee.length = 1) and
                          (addressee.address^[1] = chr (0)) then begin
                         (* The null byte indicates that all the addressees
                            have been accomodated. *)
                         if DEBUG_OUT then writeln ('  Terminate user list.');
                         MAIL_OUT_CHECK := SS$_NORMAL;
                       end else begin
                         if DEBUG_OUT then writeln ('  CKUSER option used.');
                         copy_descr_to_string (addressee, usernamebuffer,
                                               DEBUG_OUT);
                         STR$UPCASE (usernamebuffer, usernamebuffer);
                         if DEBUG_OUT then
                           writeln ('  Checking out user "',
                                    usernamebuffer, '".');
                         if not find_user_directory (usernamebuffer,
                                                     userdirectory,
                                                     useruic, useraccount,
						     userpriority) then
                           MAIL_OUT_CHECK := DELIVER__USERNOEXIST
                         else begin
                           if DEBUG_OUT then writeln ('  Trying to open "',
                             userdirectory + 'MAIL.DELIVERY', '".');
                           open (file_variable := maildelivery,
                                 file_name := userdirectory + 'MAIL.DELIVERY',
                                 organization := SEQUENTIAL,
                                 sharing := READONLY,
                                 user_action := open_with_SYSPRV,
                                 error := CONTINUE, history := READONLY);
                           if status (maildelivery) <= 0 then
                             reset (maildelivery, error := CONTINUE);
                           if status (maildelivery) > 0 then begin
                             LIB$SIGNAL (DELIVER__NOMDFILE, 2,
                               usernamebuffer.length,
                               iaddress (usernamebuffer.body));
                             MAIL_OUT_CHECK := DELIVER__NOMDFILE;
                           end else begin
                             if DEBUG_OUT then
                               writeln ('  Adding this user to active list.');
                             user_count := user_count + 1;
                             if user_list_last = nil then begin
                               new (user_list_last);
                               user_list := user_list_last;
                             end else begin
                               new (user_list_last^.next);
                               user_list_last := user_list_last^.next;
                             end;
                             with user_list_last^ do begin
                               FROM_OWNER := fromstring = usernamebuffer;
                               if not read_maildelivery_file (maildelivery,
                                  rules_list) then begin
                                 if FROM_OWNER then LIB$SIGNAL (
                                   DELIVER__MDIGNORED);
                                 dispose_rules_list (rules_list);
                               end;
                               next      := nil;
                               username  := pad (usernamebuffer, ' ', 12);
                               directory := userdirectory;
                               uic       := useruic;
                               account   := useraccount;
                               priority    := userpriority;
                               user_length := usernamebuffer.length;
                               $GETTIM (currenttime);
                               copyname := 'MAIL_' +
                                           hex (currenttime.l0, 8, 8) +
                                           hex (currenttime.l1, 8, 8) +
                                           hex (user_count, 8, 8);
                               if DEBUG_OUT then
                                 writeln ('  Added user "', username,
                                          '"; file code is "',
                                          copyname, '".');
                             end; (* with user_list_last^ *)
                             MAIL_OUT_CHECK := SS$_NORMAL;
                           end;
                         end;
                       end; (* LNK_C_OUT_CKUSER *)
    (* Check out the message send operation *)
    LNK_C_OUT_CKSEND : begin
                         if DEBUG_OUT then writeln ('  CKSEND option used.');
                         MAIL_OUT_CHECK := SS$_NORMAL;
                       end; (* LNK_C_OUT_CKSEND *)
    otherwise begin
                         if DEBUG_OUT then writeln ('  Otherwise option used.', link_flag);
	      end;
  end; (* case *)
end; (* MAIL_OUT_CHECK *)
 
(* MAIL_OUT_FILE is called when the body of the message is ready to be
   sent. The message is available as a file and must be read from this
   temporary file using RMS. MAIL_OUT_FILE is where most of the actual
   work DELIVER does takes place. The following steps are taken:
 
   (1) The mode of the message file is set to record I/O (MAIL sometimes
       leaves the file in block mode).
 
   (2) The list of users to whom messages are being sent is scanned.
       For each user on the list:
 
       (a) A copy of the message is placed in the user's default
           directory. The file is created with SYSPRV, so it will
           be owned by that user.
 
       (b) The user's rules are scanned and checked for matches.
 
       (c) If any of the rules are satisfied, a command file is also
           created. This files contains some initial symbol definitions
           and then commands to implement each of the user's rules that
           matched. The command file ends with commands that delete the
           copy of the message as well as the commmand file itself.
 
       (d) A batch job is created to run the command file. Note that this
           means MAIL must be installed with CMKRNL privilege.
*)
 
[global] function MAIL_OUT_FILE (var context : unsigned;
  var link_flag : integer;
  var protocol : string_descriptor;
  var message_RAB : RAB$TYPE;
  [asynchronous, unbound] procedure UTIL$REPORT_ERROR) : integer;
 
label
  44, 99;

var
  user_list_scan : user_block_ptr;
  onehasmatched, somehavematched, match : boolean;
  rules_list_scan : parameter_block_ptr; message_file : text;
  fromupstring, toupstring, subjectupstring, line, try_batch_queue,
    specified_batch_queue : string;
  index, lleft, stat, queue_try, last_item : integer;
  ppriv, priv : [quad] array [0..1] of unsigned;
  iosb : [quad] array [0..1] of integer;
  items : array [1..17] of item;
  submit_parameters : array [1..8] of record
                                        specified : boolean;
                                        contents  : string;
                                      end;
  previous_bio : boolean;
  previous_rbf, previous_ubf : unsigned;
  previous_rsz, previous_usz : $uword;
  previous_rac : $ubyte;
  big_line : big_string;
 
  function STR$MATCH_WILD (candidate : varying [l1] of char;
    pattern : varying [l2] of char) : integer; extern;
 
  function STR$UPCASE (var dststr : varying [l1] of char;
    var srcstr : varying [l2] of char) : integer; extern;
 
  procedure check_status;

  begin (* check_status *)
    if status (message_file) > 0 then begin
      LIB$SIGNAL (DELIVER__MESWRTERR, 1, status (message_file));
      MAIL_OUT_FILE := DELIVER__MESWRTERR;
      goto 99;
    end; (* if *)
  end; (* check_status *)

  (* function to read a line from the message file *)
 
  function get_line (var line : string) : boolean;
 
  var
    stat : integer;
 
  begin (* get_line *)
    if DEBUG_OUT then writeln ('MAIL_OUT_FILE\get_line called.');
    get_line := false;
    message_RAB.RAB$L_UBF := iaddress (big_line.body);
    message_RAB.RAB$W_USZ := big_size;
    stat := $GET (RAB := message_RAB);
    if odd (stat) then begin
      line.length := 0;
      for index := 1 to message_RAB.RAB$W_RSZ do
	line := line + big_line.body[index];
      get_line := true;
    end else if stat <> RMS$_EOF then
      LIB$SIGNAL (DELIVER__MESREAERR, 1, stat);
  end; (* get_line *)
 
  procedure put_string (line : string);

  begin (* put_string *)
    if lleft >= line.length then begin
      write (message_file, line);
      lleft := lleft - line.length;
    end; (* if *)
  end; (* put_string *)

  procedure put_char (ch : char);

  begin (* put_char *)
    if lleft >= 1 then begin
      write (message_file, ch);
      lleft := pred (lleft);
    end; (* if *)
  end; (* put_char *)

  procedure put_symbol (symbol, svalue : string);

  var
    index : integer;
    squote_previous : boolean;

  begin (* put_symbol *)
    write (message_file, '$ ', symbol, ' == "');
    lleft := DCL_line_size - 8 - symbol.length;
    squote_previous := false;
    for index := 1 to svalue.length do begin
      if svalue[index] = '"' then put_string ('""')
      else begin
        if (svalue[index] = '''') and squote_previous then put_string ('"+"');
        put_char (svalue[index]);
      end; (* if *)
      squote_previous := svalue[index] = '''';
    end; (* for *)
    writeln (message_file, '"', error := CONTINUE);
    check_status;
    write (message_file, '$ Q', symbol, ' == "');
    lleft := DCL_line_size - 9 - symbol.length;
    squote_previous := false;
    for index := 1 to length (svalue) do begin
      if svalue[index] = '"' then put_string ('""""')
      else if (not squote_previous) or (svalue[index] <> '''') then
        put_char (svalue[index]);
      squote_previous := svalue[index] = '''';
    end; (* for *)
    writeln (message_file, '"', error := CONTINUE);
    check_status;
    write (message_file, '$ QQ', symbol, ' == "');
    lleft := DCL_line_size - 10 - symbol.length;
    squote_previous := false;
    for index := 1 to svalue.length do begin
      if svalue[index] = '"' then put_string ('""""""""')
      else if (not squote_previous) or (svalue[index] <> '''') then
        put_char (svalue[index]);
      squote_previous := svalue[index] = '''';
    end; (* for *)
    writeln (message_file, '"', error := CONTINUE);
    check_status;
  end; (* put_symbol *)

begin (* MAIL_OUT_FILE *)
  if DEBUG_OUT then writeln ('MAIL_OUT_FILE called.');
 
  (* Do some fancy footwork with RMS to insure that the file is open
     for sequential access and not block access. MAIL sometimes has
     this file open in block mode. The only way to change modes is
     to disconnect the RAB, diddle the mode bit and then reconnect it. *)
  $DISCONNECT (RAB := message_RAB);
  previous_bio := message_RAB.RAB$V_BIO;
  previous_rbf := message_RAB.RAB$L_RBF;
  previous_rsz := message_RAB.RAB$W_RSZ;
  previous_ubf := message_RAB.RAB$L_UBF;
  previous_usz := message_RAB.RAB$W_USZ;
  previous_rac := message_RAB.RAB$B_RAC;
  if DEBUG_OUT then writeln ('  The BIO field of the RAB is set ',
    uand (message_RAB.RAB$L_ROP, RAB$M_BIO) <> 0);
  message_RAB.RAB$L_ROP := uand (message_RAB.RAB$L_ROP, unot (RAB$M_BIO));
  $CONNECT (RAB := message_RAB);
 
  if DEBUG_OUT then writeln (' Creating upper case copies of header strings.');
  STR$UPCASE (fromupstring,    fromstring);
  STR$UPCASE (toupstring,      tostring);
  STR$UPCASE (subjectupstring, subjectstring);
  if DEBUG_OUT then writeln ('  From: "', fromupstring, '", To: "', toupstring,
                             '", Subject: "', subjectupstring, '".');
 
  if DEBUG_OUT then writeln (' Pruning the rules list.');
  user_list_scan := user_list;
  while user_list_scan <> nil do begin
    if DEBUG_OUT then writeln ('  User: "',
      substr (user_list_scan^.username, 1, user_list_scan^.user_length), '".');
    if DEBUG_OUT then writeln ('  Create copy of message headers in file "',
                               user_list_scan^.directory,
                               user_list_scan^.copyname, '.HEADER".');
    open (file_variable := message_file, history := NEW,
          record_length := big_size, record_type := VARIABLE,
          file_name := user_list_scan^.directory +
                       user_list_scan^.copyname + '.HEADER',
          user_action := create_with_SYSPRV, error := CONTINUE,
          organization := SEQUENTIAL);
    if status (message_file) <= 0 then rewrite (message_file);
    check_status;
    writeln (message_file, 'From:   ',fromstring, error := CONTINUE);
    check_status;
    writeln (message_file, 'To:     ', tostring, error := CONTINUE);
    check_status;
    writeln (message_file, 'Cc:     ', ccstring, error := CONTINUE);
    check_status;
    writeln (message_file, 'Subj:   ', subjectstring, error := CONTINUE);
    check_status;
    close (message_file, error := CONTINUE);
    check_status;
    if DEBUG_OUT then writeln ('  Create copy of message in file "',
                               user_list_scan^.directory,
                               user_list_scan^.copyname, '.TEXT".');
    open (file_variable := message_file, history := NEW,
          record_length := parameter_size, record_type := VARIABLE,
          file_name := user_list_scan^.directory +
                       user_list_scan^.copyname + '.TEXT',
          user_action := create_with_SYSPRV, error := CONTINUE,
          organization := SEQUENTIAL);
    if status (message_file) <= 0 then rewrite (message_file);
    check_status;
    $REWIND (RAB := message_RAB);
    while get_line (line) do begin
      if DEBUG_OUT then writeln(line);
      writeln (message_file, line, error := CONTINUE);
      check_status;
    end; (* while get_line *)
    close (message_file, error := CONTINUE);
    check_status;
 
    if DEBUG_OUT then writeln ('  Creating command file named "',
                               user_list_scan^.directory,
                               user_list_scan^.copyname, '.COM".');
    open (file_variable := message_file, history := NEW,
          record_length := parameter_size, record_type := VARIABLE,
          file_name := user_list_scan^.directory +
                       user_list_scan^.copyname + '.COM',
          user_action := create_with_SYSPRV, error := CONTINUE,
          organization := SEQUENTIAL);
    if status (message_file) <= 0 then rewrite (message_file);
    check_status;

    writeln (message_file, '$ SET NOON', error := CONTINUE);
    check_status;
    writeln (message_file, '$ DELETE = "DELETE"', error := CONTINUE);
    check_status;
    writeln (message_file, '$ MESSAGE_DELETE == "YES"', error := CONTINUE);
    check_status;
    put_symbol ('FROM', fromstring);
    put_symbol ('TO', tostring);
    put_symbol ('SUBJECT', subjectstring);
    put_symbol ('CC', ccstring);

    writeln (message_file, '$ MESSAGE_FILE == "',
             user_list_scan^.directory,
             user_list_scan^.copyname, '.TEXT"', error := CONTINUE);
    check_status;
    writeln (message_file, '$ MESSAGE_HEADER == "',
             user_list_scan^.directory,
             user_list_scan^.copyname, '.HEADER"', error := CONTINUE);
    check_status;
    writeln (message_file, '$ COMMAND_FILE == "',
             user_list_scan^.directory,
             user_list_scan^.copyname, '.COM"', error := CONTINUE);
    check_status;
 
    if DEBUG_OUT then writeln ('  Check this user''s delivery list.');
    onehasmatched := false; rules_list_scan := user_list_scan^.rules_list;
    somehavematched := false;
    batch_log := '_NLA0:';
    batch_keep := false;
    specified_batch_queue := '';
    for index := 1 to 8 do submit_parameters[index].specified := false;
    rules_list_scan := user_list_scan^.rules_list;
    while rules_list_scan <> nil do with rules_list_scan^ do begin
      match := (any_to      or (STR$MATCH_WILD (toupstring,
                                parameters[to_parameter]) = STR$_MATCH)) and
               (any_from    or (STR$MATCH_WILD (fromupstring,
                                parameters[from_parameter]) = STR$_MATCH)) and
               (any_subject or (STR$MATCH_WILD (subjectupstring,
                                parameters[subject_parameter]) = STR$_MATCH));
      case parameters[decision_parameter][1] of
        'A'      : match := true;           
        'X'      : match := false;
        'T', 'Y' : match := match;
        'F', 'N' : match := not match;
        '?', 'O' : match := match and (not onehasmatched);
        'B', 'Q' : match := (not match) and (not onehasmatched);
        'E'      : match := match or (not onehasmatched);
        otherwise match := false;
      end; (* decision case *)
      if match then begin
        if DEBUG_OUT then writeln (' Rule matched. From: pattern: "',
          parameters[from_parameter], '", To: pattern: "',
          parameters[to_parameter], '", Subject: pattern: "',
          parameters[subject_parameter], '", Decision character: ',
          parameters[decision_parameter][1], '.');
        somehavematched := true;
        if parameters[decision_parameter][1] <> 'A' then onehasmatched := true;
        case parameters[action_parameter][1] of
          (* append *)
          'A'  : begin
                   writeln (message_file,
                            '$ APPEND/NEW_VERSION ''MESSAGE_FILE'' ',
                            parameters[argument1_parameter],
                            error := CONTINUE);
                   check_status;
                 end; (* append *)
          (* deliver *)                                      
          'B', 'D',
          'O'  : begin
                   if MAIL$C_PROT_MAJOR > 1 then
                     writeln (message_file, folder_invocation,
                              '/FROM="''''QFROM''"/TO="''''QTO''"',
                              '/CC="''''QCC''"/SUBJECT="''''QSUBJECT''"',
                              ' ''MESSAGE_FILE'' ',
                              parameters[argument1_parameter], ' ',
                              parameters[argument2_parameter],
                              error := CONTINUE)
                   else begin
                     write (message_file,
                            '$ MAIL/NOSELF/SUBJECT="(From: ');
                     write (message_file, '''''QFROM'') ''''QSUBJECT''"');
                     write (message_file, ' ''MESSAGE_FILE'' "_',
                            substr (user_list_scan^.username, 1,
                                    user_list_scan^.user_length));
                     writeln (message_file, '"', error := CONTINUE);
                   end; (* if *)
                   check_status;
                 end; (* deliver *)
          (* create, copy *)
          'C'  : begin
                   writeln (message_file, '$ COPY ''MESSAGE_FILE'' ',
                            parameters[argument1_parameter],
                            error := CONTINUE);
                   check_status;
                 end; (* create, copy *)
          (* execute *)
          'E'  : begin
                   if parameters[argument1_parameter][1] <> '$' then
                   write (message_file, '$ ');
                   writeln (message_file, parameters[argument1_parameter],
                            error := CONTINUE);
                   check_status;
                 end; (* execute *)
          (* forward *)
          'F'  : begin
                   if MAIL$C_PROT_MAJOR > 1 then
                     write (message_file,
                            '$ MAIL/NOTRAN/NOSELF/SUBJECT="(From: ')
                   else write (message_file,
                               '$ MAIL/NOSELF/SUBJECT="(From: ');
                   write (message_file, '''''QFROM'') ''''QSUBJECT''"');
                   writeln (message_file, ' ''MESSAGE_FILE'' ',
                            parameters[argument1_parameter], error := CONTINUE);
                   check_status;
                 end; (* forward *)
          (* append with mail headers *)
          'H'  : begin
                   writeln (message_file, '$ APPEND/NEW_VERSION ',
                            '''MESSAGE_HEADER''+SYS$INPUT:+''MESSAGE_FILE'' ',
                            parameters[argument1_parameter],
                            error := CONTINUE);
                   check_status;
                   writeln (message_file, error := CONTINUE);
                   check_status;
                 end; (* append with mail headers *)
          (* keep-command *)
          'K'  : batch_keep := true;
          (* log-keep *)
          'L'  : begin
                   if parameters[argument1_parameter] = '' then
                     batch_log := user_list_scan^.directory + 'DELIVER.LOG'
                   else batch_log := user_list_scan^.directory +
                                     parameters[argument1_parameter];
                 end; (* log-keep *)
          (* message-keep *)
          'M'  : begin
                   writeln (message_file, '$ MESSAGE_DELETE == "NO"',
                            error := CONTINUE);
                   check_status;
                 end; (* message-keep *)
          (* quit *)
          'Q'  : rules_list_scan := nil;
          (* privileged-deliver *)
          'V'  : begin
                   if MAIL$C_PROT_MAJOR > 1 then
                     writeln (message_file, folder_invocation,
                              '/FROM="''''QFROM''"/TO="''''QTO''"',
                              '/CC="''''QCC''"/SUBJECT="''''QSUBJECT''"',
                              ' ''MESSAGE_FILE'' ',
                              parameters[argument1_parameter], ' ',
                              parameters[argument2_parameter],
                              error := CONTINUE)
                   else begin
                     writeln (message_file, '$ DELIVER_FROM = FROM',
                              error := CONTINUE);
                     check_status;
                     writeln (message_file, '$ DELIVER_TO = TO',
                              error := CONTINUE);
                     check_status;
                     writeln (message_file, '$ DELIVER_CC = CC',
                              error := CONTINUE);
                     check_status;
                     writeln (message_file,
                              '$ PRIV = F$SETPRV("DETACH,SYSPRV,BYPASS")',
                              error := CONTINUE);
                     check_status;
                     write (message_file, '$ MAIL/NOSELF');
                     write (message_file,
                       '/PROTOCOL=DELIVER_MAILSHR/SUBJECT="''''QSUBJECT''"');
                     write (message_file, ' ''MESSAGE_FILE'' "_',
                            substr (user_list_scan^.username, 1,
                                    user_list_scan^.user_length));
                     writeln (message_file, '"', error := CONTINUE);
                     check_status;
                     writeln (message_file, '$ PRIV = F$SETPRV(PRIV)',
                              error := CONTINUE);
                     check_status;
                     writeln (message_file,
                              '$ DELETE/SYMBOL/LOCAL DELIVER_FROM',
                              error := CONTINUE);
                     check_status;
                     writeln (message_file, '$ DELETE/SYMBOL/LOCAL DELIVER_TO',
                              error := CONTINUE);
                     check_status;
                     writeln (message_file, '$ DELETE/SYMBOL/LOCAL DELIVER_CC',
                              error := CONTINUE);
                   end; (* if *)
                   check_status;
                 end; (* privileged-deliver *)
          (* privileged-forward *)
          'W'  : begin
                   writeln (message_file, '$ DELIVER_FROM = FROM',
                            error := CONTINUE);
                   check_status;
                   writeln (message_file, '$ DELIVER_TO = TO',
                            error := CONTINUE);
                   check_status;
                   writeln (message_file, '$ DELIVER_CC = CC',
                            error := CONTINUE);
                   check_status;
                   writeln (message_file,
                            '$ PRIV = F$SETPRV("DETACH,SYSPRV,BYPASS")',
                            error := CONTINUE);
                   check_status;
                   if MAIL$C_PROT_MAJOR > 1 then
                     write (message_file, '$ MAIL/NOTRAN/NOSELF')
                   else write (message_file, '$ MAIL/NOSELF');
                   write (message_file,
                          '/PROTOCOL=DELIVER_MAILSHR/SUBJECT="''''QSUBJECT''"');
                   writeln (message_file, ' ''MESSAGE_FILE'' ',
                            parameters[argument1_parameter], error := CONTINUE);
                   check_status;
                   writeln (message_file,'$ PRIV = F$SETPRV(PRIV)',
                            error := CONTINUE);
                   check_status;
                   writeln (message_file, '$ DELETE/SYMBOL/LOCAL DELIVER_FROM',
                            error := CONTINUE);
                   check_status;
                   writeln (message_file, '$ DELETE/SYMBOL/LOCAL DELIVER_TO',
                            error := CONTINUE);
                   check_status;
                   writeln (message_file, '$ DELETE/SYMBOL/LOCAL DELIVER_CC',
                            error := CONTINUE);
                   check_status;
                 end; (* privileged-forward *)
          (* job-queue or job parameter *)
          'J'  : if parameters[argument2_parameter] = '' then
                   specified_batch_queue := parameters[argument1_parameter]
                 else begin
                   STR$UPCASE (parameters[argument1_parameter],
                               parameters[argument1_parameter]);
                   if parameters[argument1_parameter] = 'QUEUE' then
                     specified_batch_queue := parameters[argument2_parameter]
                   else if parameters[argument1_parameter] = 'P1' then begin
                     submit_parameters[1].specified := true;
                     submit_parameters[1].contents :=
                       parameters[argument2_parameter];
                   end else if parameters[argument1_parameter] = 'P2' then begin
                     submit_parameters[2].specified := true;
                     submit_parameters[2].contents :=
                       parameters[argument2_parameter];
                   end else if parameters[argument1_parameter] = 'P3' then begin
                     submit_parameters[3].specified := true;
                     submit_parameters[3].contents :=
                       parameters[argument2_parameter];
                   end else if parameters[argument1_parameter] = 'P4' then begin
                     submit_parameters[4].specified := true;
                     submit_parameters[4].contents :=
                       parameters[argument2_parameter];
                   end else if parameters[argument1_parameter] = 'P5' then begin
                     submit_parameters[5].specified := true;
                     submit_parameters[5].contents :=
                       parameters[argument2_parameter];
                   end else if parameters[argument1_parameter] = 'P6' then begin
                     submit_parameters[6].specified := true;
                     submit_parameters[6].contents :=
                       parameters[argument2_parameter];
                   end else if parameters[argument1_parameter] = 'P7' then begin
                     submit_parameters[7].specified := true;
                     submit_parameters[7].contents :=
                       parameters[argument2_parameter];
                   end else if parameters[argument1_parameter] = 'P8' then begin
                     submit_parameters[8].specified := true;
                     submit_parameters[8].contents :=
                       parameters[argument2_parameter];
                   end;
                 end; (* if *)
          otherwise begin end;
        end; (* case *)
      end; (* add commands to implement this matching rule *)
      if rules_list_scan <> nil then rules_list_scan := rules_list_scan^.next;
    end; (* while *)
 
    if not somehavematched then begin
      if DEBUG_OUT then writeln ('  No rules matched, just deliver it.');
      if MAIL$C_PROT_MAJOR > 1 then
        writeln (message_file, folder_invocation,
                 '/FROM="''''QFROM''"/TO="''''QTO''"',
                 '/CC="''''QCC''"/SUBJECT="''''QSUBJECT''"',
                 ' ''MESSAGE_FILE'' ', error := CONTINUE)
      else writeln (message_file, '$ MAIL/NOSELF/SUBJECT="(From: ',
                    '''''QFROM'') ''''QSUBJECT''" ''MESSAGE_FILE'' _',
                    substr (user_list_scan^.username, 1,
                            user_list_scan^.user_length), error := CONTINUE);
      check_status;
    end; (* if *)

    if DEBUG_OUT then writeln ('  Finishing up delivery command file.');
    writeln (message_file, '$ IF MESSAGE_DELETE .nes. "NO" then ',
             'DELETE ''MESSAGE_FILE'';', error := CONTINUE);
    check_status;
    writeln (message_file, '$ IF MESSAGE_DELETE .nes. "NO" then ',
             'DELETE ''MESSAGE_HEADER'';', error := CONTINUE);
    check_status;
    writeln (message_file, '$ DELETE ''COMMAND_FILE'';',
             error := CONTINUE);
    check_status;
    close (message_file, error := CONTINUE);
    check_status;
 
    if DEBUG_OUT then writeln ('  Submitting batch job.');
    priv[0] := PRV$M_CMKRNL + PRV$M_SYSPRV + PRV$M_BYPASS + PRV$M_OPER + PRV$M_TMPMBX; priv[1] := 0;
    stat := $SETPRV (ENBFLG := 1, PRVADR := priv, PRMFLG := 0, PRVPRV := ppriv);
    if (not odd (stat)) and DEBUG_OUT then
        writeln ('    Unable to get privs to submit job');
    if (stat = SS$_NOTALLPRIV) and DEBUG_OUT then
	writeln ('    Unable to get all privs');
    if specified_batch_queue.length > 0 then queue_try := 1
    else queue_try := 2;
  44:
    case queue_try of
      1 : try_batch_queue := specified_batch_queue;
      2 : try_batch_queue := default_batch_queue;
      3 : try_batch_queue := system_batch_queue;
    end; (* case *)
    if DEBUG_OUT then writeln ('  Checking validity of batch queue ',
                               try_batch_queue);
    with items[1] do begin
      len := size (arm_write_access); code := CHP$_ACCESS;
      addr := iaddress (arm_write_access); rlen := 0;
    end; (* with items[1] *)
    with items[2] do begin
      len := 0; code := 0; addr := 0; rlen := 0;
    end; (* with items[2] *)
    stat := $CHECK_ACCESS (objtyp := ACL$C_JOBCTL_QUEUE,
                           objnam := try_batch_queue,
                           usrnam := substr (user_list_scan^.username, 1,
                                             user_list_scan^.user_length),
                           itmlst := items);
    if odd (stat) then begin
      line := user_list_scan^.directory + user_list_scan^.copyname + '.COM';
      if DEBUG_OUT then begin
        writeln ('    Queue ', try_batch_queue,
                                 ' is writable by user.');
        writeln ('  Submitting batch job.');
	writeln ('       ', line);
	writeln ('       logfile  = ', batch_log);
      end; (* if *)

      for index := 1 to 17 DO WITH items[index] do BEGIN
	len := 0; code := 0; addr := 0; rlen := 0;
	END;
      with items[2] do begin
        len := try_batch_queue.length; code := SJC$_QUEUE;
        addr := iaddress (try_batch_queue.body); rlen := 0;
      end; (* with items[2] *)
      with items[3] do begin
        len := line.length; code := SJC$_FILE_SPECIFICATION;
        addr := iaddress (line.body); rlen := 0;
      end; (* with items[3] *)
      with items[4] do begin
        len := batch_log.length; code := SJC$_LOG_SPECIFICATION;
        addr := iaddress (batch_log.body); rlen := 0;
      end; (* with items[4] *)
      with items[5] do begin
        len := 0; code := SJC$_NO_LOG_SPOOL;
        addr := 0; rlen := 0;
      end; (* with items[5] *)
      with items[6] do begin
        len := 0;
        if batch_keep then code := SJC$_NO_DELETE_FILE
        else code := SJC$_DELETE_FILE;
        addr := 0; rlen := 0;
      end; (* with items[6] *)
      if use_sjc_user_identification then begin
        with items[1] do begin
          len := 25; code := SJC$_USER_IDENTIFICATION;
          addr := iaddress (user_list_scan^.uic); rlen := 0;
	  if DEBUG_OUT then writeln(' user_identification used');
        end; (* with items[1] *)
        last_item := 6;
      end else begin
        with items[1] do begin
          len := user_list_scan^.user_length; code := SJC$_USERNAME;
          addr := iaddress (user_list_scan^.username); rlen := 0;
	  if DEBUG_OUT then writeln('  using username of ', user_list_scan^.username, 'length = ', user_list_scan^.user_length);
        end; (* with items[1] *)
        with items[7] do begin
          len := 4; code := SJC$_UIC;
          addr := iaddress (user_list_scan^.uic); rlen := 0;
        end; (* with items[7] *)
        with items[8] do begin
          len := 8; code := SJC$_ACCOUNT_NAME;
          addr := iaddress (user_list_scan^.account); rlen := 0;
        end; (* with items[8] *)
        last_item := 8;
      end; (* if *)
      if DEBUG_OUT then writeln ('    building submit parameters.');
      for index := 1 to 8 do if submit_parameters[index].specified then begin
        last_item := succ (last_item);
	IF DEBUG_OUT then writeln('    last_item =', last_item);
        with items[last_item] do begin
          len := submit_parameters[index].contents.length;
          code := SJC$_PARAMETER_1 + index - 1;
          addr := iaddress (submit_parameters[index].contents.body); rlen := 0;
        end; (* with items[last_item] *)
      end; (* if, for *)
      last_item := succ (last_item);
      IF DEBUG_OUT then writeln('    last_item =', last_item);
      with items[last_item] do begin
        len := 0; code := 0; addr := 0; rlen := 0;
      end; (* with items[last_item] *)
      if DEBUG_OUT then writeln ('    Queueing job.');
      stat := $SNDJBCW (func := SJC$_ENTER_FILE, itmlst := items, iosb := iosb);
      if odd (stat) then stat := iosb[0];
      if (not odd (stat)) and DEBUG_OUT then
        writeln ('    Error submitting job, status = ', stat:0, ' iosb =', iosb[0]:0,' ', iosb[1]:0);
    end else if DEBUG_OUT then
      writeln ('    Queue ', try_batch_queue,
               ' is not writable, status = ', stat:0);
    if (not odd (stat)) and (queue_try < 3) then begin
      queue_try := succ (queue_try);
      goto 44;
    end; (* if *)
    priv[0] := uand (priv[0], unot (ppriv[0]));
    priv[1] := uand (priv[1], unot (ppriv[1]));
    $SETPRV (ENBFLG := 0, PRVADR := priv, PRMFLG := 0);
    if not odd (stat) then begin
      LIB$SIGNAL (stat);
      MAIL_OUT_FILE := stat;
      goto 99;
    end; (* if *)
    if not odd (iosb[0]) then begin
      LIB$SIGNAL (iosb[0]);
      MAIL_OUT_FILE := iosb[0];
      goto 99;
    end; (* if *)
    user_list_scan := user_list_scan^.next;
  end; (* while *)
  MAIL_OUT_FILE := SS$_NORMAL;
99:
  (* Turn the BIO bit back on if it was set before. *)
  $DISCONNECT (RAB := message_RAB);
  message_RAB.RAB$V_BIO := previous_bio;
  message_RAB.RAB$L_RBF := previous_rbf;
  message_RAB.RAB$W_RSZ := previous_rsz;
  message_RAB.RAB$L_UBF := previous_ubf;
  message_RAB.RAB$W_USZ := previous_usz;
  message_RAB.RAB$B_RAC := previous_rac;
  $CONNECT (RAB := message_RAB);
end; (* MAIL_OUT_FILE *)
 
(* MAIL_OUT_DEACCESS is called to shut down the current send operation. *)
 
[global] function MAIL_OUT_DEACCESS (var context : unsigned;
  var link_flag : integer) : integer;
 
begin (* MAIL_OUT_DEACCESS *)
  if DEBUG_OUT then writeln ('MAIL_OUT_DEACCESS called.');
  if user_list <> nil then begin
    if DEBUG_OUT then writeln ('  Deleting user list and associated rules.');
    dispose_user_list (user_list);
  end;
  MAIL_OUT_DEACCESS := SS$_NORMAL;
end; (* MAIL_OUT_DEACCESS *)
 
(* MAIL_OUT_ATTRIBS delivers the message file's attributes to DELIVER. This
   information is currently unused (see comment in MAIL_IN_ATTRIBS below. *)

[global] function MAIL_OUT_ATTRIBS (var context : unsigned;
  var link_flag : integer; var system_flags : integer;
  var idtld : file_attribute_block) : integer;

begin (* MAIL_OUT_ATTRIBS *)
  if DEBUG_OUT then writeln ('MAIL_OUT_ATTRIBS called.');
  MAIL_OUT_ATTRIBS := SS$_NORMAL;
end; (* MAIL_OUT_ATTRIBS *)

(* These routines manipulate a stack in which we maintain state information for
   information being "written" to us when MAIL calls MAIL_IO_WRITE. *)
 
procedure init_stack (var stack : write_state_stack);
 
begin (* init_stack *)
  if DEBUG_IN then writeln ('INIT_STACK called.');
  stack.top := 0;
end; (* init_stack *)
 
procedure push (var stack : write_state_stack; state : write_states);
 
var
  i : integer;
 
begin (* push *)
  if DEBUG_IN then writeln ('PUSH called.');
  with stack do begin
    if top < stack_size then top := succ (top)
    else LIB$SIGNAL (DELIVER__INTSTKOVR);
    store[top] := state;
    if DEBUG_IN then begin
      writeln ('  after PUSH:');
      for i := top downto 1 do writeln ('    ', store[i]);
    end; (* if *)
  end; (* with *)
end; (* push *)
 
procedure pop (var stack : write_state_stack);
 
var
  i : integer;
 
begin (* pop *)
  if DEBUG_IN then writeln ('POP called.');
  with stack do begin
    top := pred (top);
    if top < 1 then LIB$SIGNAL (DELIVER__STKEMPTY);
  end; (* with *)
  if DEBUG_IN then begin
    writeln ('  after POP:');
    for i := stack.top downto 1 do writeln ('    ', stack.store[i]);
  end;
end; (* pop *)
 
function top_of_stack (var stack : write_state_stack) : write_states;
 
begin (* top_of_stack *)
  if DEBUG_IN then writeln ('TOP_OF_STACK called.');
  with stack do begin
    if top > 0 then begin
      top_of_stack := store[top];
      if DEBUG_IN then writeln (' returning ', store[top]);
    end else begin
      LIB$SIGNAL (DELIVER__STKEMPTY);
      top_of_stack := bad_msg;
      if DEBUG_IN then writeln (' returning ', bad_msg);
    end; (* if *)  
  end; (* with *)
end; (* top_of_stack *)
 
(* The incoming mail handling routines are activated by a command line of
   the form:
 
   $ MAIL/PROTOCOL=DELIVER_MAILSHR/SUBJECT="subject" message.txt address-list
 
   Everything is specified on the command line except the From: address,
   which is obtained by translating the logical name DELIVER$FROM.
 
   System privileges are required to use this interface since this routine
   makes it possible to "forge" return addresses. *)
 
[global] function MAIL_IN_CONNECT (var context : unsigned;
  var link_flag : integer;
  var input_tran : string_descriptor;
  var file_RAT, file_RFM : integer;
  var MAIL$GL_SYSFLAGS : integer;
  var MAIL$Q_PROTOCOL : string_descriptor;
  var pflags : integer;
  var file_ORG : $ubyte) : integer;
 
var
  priv : [quad] array [0..1] of unsigned; stat : integer;

begin (* MAIL_IN_CONNECT *)
  if DEBUG_IN then writeln ('MAIL_IN_CONNECT called.');
  (* Set file structure stuff *)
  file_RAT := FAB$M_CR;
  file_RFM := FAB$C_VAR;
  if MAIL$C_PROT_MAJOR > 1 then file_ORG := FAB$C_SEQ;

  priv[0] := 0; priv[1] := 0;
  stat := LIB$GETJPI (JPI$_PROCPRIV, , , priv[0], , );
  if not odd (stat) then LIB$SIGNAL (stat);
  if uand (priv[0], PRV$M_SYSPRV) = 0 then begin
    LIB$SIGNAL (DELIVER__NOTPRIV);
    MAIL_IN_CONNECT := DELIVER__NOTPRIV;
  end else begin
    toline := '';
    if DEBUG_IN then writeln ('Initializing state stack.');
    init_stack (write_recv_states);
    push (write_recv_states, bad_msg);
    sticky_error := SS$_NORMAL;
    LIB$SET_SYMBOL ('DELIVER_STATUS', '%X' + hex (sticky_error, 8, 8));
    MAIL_IN_CONNECT := SS$_NORMAL;
  end; (* if *)
end; (* MAIL_IN_CONNECT *)
 
(* MAIL calls MAIL_IN_LINE to get single line information from DELIVER. *)
 
[global] function MAIL_IN_LINE (var context : unsigned;
  var link_flag : integer;
  var line : string_descriptor) : integer;
 
var
  linebuffer : string; stat : integer;
 
begin (* MAIL_IN_LINE *)
  if DEBUG_IN then writeln ('MAIL_IN_LINE called.');
  case iaddress (link_flag) of
    (* Return From: information to MAIL *)
    LNK_C_IN_SENDER : begin
                        if DEBUG_IN then writeln ('IN_SENDER option used.');
                        stat := LIB$GET_SYMBOL ('DELIVER_FROM', linebuffer);
                        if not odd (stat) then linebuffer := '<not specified>';
                        copy_string_to_descr (linebuffer, line, DEBUG_IN);
                       end; (* LNK_C_IN_SENDER *)
    (* Return To: information to MAIL *)
    LNK_C_IN_CKUSER : begin
                        if DEBUG_IN then writeln ('IN_CKUSER option used.');
                        stat := CLI$GET_VALUE ('TOLIST', linebuffer);
                        if not odd (stat) then linebuffer := chr (0) else begin
                          if length (toline) > 0 then toline := toline + ',';
                          toline := toline + linebuffer;
                          push (write_recv_states, user_check);
                        end;
                        copy_string_to_descr (linebuffer, line, DEBUG_IN);
                      end; (* LNK_C_IN_CKUSER *)
    (* Return entire To: line to MAIL *)
    LNK_C_IN_TO     : begin
                        if DEBUG_IN then writeln ('IN_TO option used.');
                        stat := LIB$GET_SYMBOL ('DELIVER_TO', linebuffer);
                        if odd (stat) then 
                          copy_string_to_descr (linebuffer, line, DEBUG_IN)
                        else copy_string_to_descr (toline, line, DEBUG_IN);
                       end; (* LNK_C_IN_TO *)
    (* Return entire Subject: line to MAIL *)
    LNK_C_IN_SUBJ   : begin
                        if DEBUG_IN then writeln ('IN_SUBJ option used.');
                        stat := CLI$GET_VALUE ('SUBJECT', linebuffer);
                        if not odd (stat) then linebuffer := '';
                        copy_string_to_descr (linebuffer, line, DEBUG_IN);
                       end; (* LNK_C_IN_SUBJ *)
    (* Return entire Cc: line to MAIL *)
    LNK_C_IN_CC     : begin
                        if DEBUG_IN then writeln ('IN_CC option used.');
                        stat := LIB$GET_SYMBOL ('DELIVER_CC', linebuffer);
                        if not odd (stat) then linebuffer := '';
                        copy_string_to_descr (linebuffer, line, DEBUG_IN);
                       end; (* LNK_C_IN_SUBJ *)
    otherwise begin
                         if DEBUG_OUT then writeln ('  Otherwise option used.', link_flag);
	      end;
  end; (* case *)
  MAIL_IN_LINE := SS$_NORMAL;
end; (* MAIL_IN_LINE *)
 
(* MAIL_IN_FILE is called by MAIL to read the body of the message to be
   delivered. This routine gets the file name from the command line, opens
   the file and copies it into MAIL's intermediate file. *)
 
[global] function MAIL_IN_FILE (var context : unsigned;
  var link_flag : integer;
  var scratch : integer;
  var message_RAB : RAB$TYPE;
  procedure UTIL$REPORT_IO_ERROR) : integer;
 

var
  filename, linebuffer : string;
  message_file : text;
  stat : integer;
  previous_bio : boolean;
  previous_rbf, previous_ubf : unsigned;
  previous_rsz, previous_usz : $uword;
  previous_rac : $ubyte;
 
begin (* MAIL_IN_FILE *)
  if DEBUG_IN then writeln ('MAIL_IN_FILE called.');
  (* Get the name of the file containing the message to be delivered. *)
  stat := CLI$GET_VALUE ('FILE', filename);
  if not odd (stat) then begin
    LIB$SIGNAL (DELIVER__GETFILERR, 1, stat);
    MAIL_IN_FILE := DELIVER__GETFILERR;
  end else begin
    open (file_variable := message_file, file_name := filename,
          organization := SEQUENTIAL, sharing := READONLY,
          default := '.TXT', error := CONTINUE, history := READONLY);
    if status (message_file) <= 0 then
      reset (message_file, error := CONTINUE);
    if status (message_file) > 0 then begin
      LIB$SIGNAL (DELIVER__MESOPNERR);
      MAIL_IN_FILE := DELIVER__MESOPNERR;
    end else begin
      (* MAIL can never get it right... The logic for setting and resetting the
         BIO bit is broken up at least as far as VMS 5.4... We'll play our
         usual game and make sure the damn thing is properly set for what we
         want to do. *)
      $DISCONNECT (RAB := message_RAB);
      previous_bio := message_RAB.RAB$V_BIO;
      previous_rbf := message_RAB.RAB$L_RBF;
      previous_rsz := message_RAB.RAB$W_RSZ;
      previous_ubf := message_RAB.RAB$L_UBF;
      previous_usz := message_RAB.RAB$W_USZ;
      previous_rac := message_RAB.RAB$B_RAC;
      message_RAB.RAB$V_BIO := false;
      $CONNECT (RAB := message_RAB);
      message_RAB.RAB$L_RBF := iaddress (linebuffer) + 2;
      stat := SS$_NORMAL;
      while (not eof (message_file)) and (odd (stat)) do begin
        readln (message_file, linebuffer, error := CONTINUE);
        if status (message_file) > 0 then begin
          LIB$SIGNAL (DELIVER__MSGREAERR, 1, status (message_file));
          stat := DELIVER__MSGREAERR;
        end else begin
          message_RAB.RAB$W_RSZ := length (linebuffer);
          stat := $PUT (RAB := message_RAB);
          if not odd (stat) then
            LIB$SIGNAL (DELIVER__MSGWRTERR, 1, stat);
        end;
      end; (* while *)
      close (message_file, error := CONTINUE);
      (* Turn the BIO bit back on if it was set before. *)
      $DISCONNECT (RAB := message_RAB);
      message_RAB.RAB$V_BIO := previous_bio;
      message_RAB.RAB$L_RBF := previous_rbf;
      message_RAB.RAB$W_RSZ := previous_rsz;
      message_RAB.RAB$L_UBF := previous_ubf;
      message_RAB.RAB$W_USZ := previous_usz;
      message_RAB.RAB$B_RAC := previous_rac;
      $CONNECT (RAB := message_RAB);
      MAIL_IN_FILE := stat;
    end;
  end;
  push (write_recv_states, delivery_check);
end; (* MAIL_IN_FILE *)
 
(* MAIL_IN_ATTRIBS is called to get file attributes for the message file. This
   routine is currently unused. It is not possible to add support for file
   attributes to DELIVER at this time (VMS 5.0-2) because this routine is
   *never* called for foreign protocols. See the code in the accept_link
   routine in MAIL$SERVER_SUBS -- the only way that the SERV_FORRECV bit in
   MAIL$L_SRVFLAGS can be set is by a MAIL-11 transaction. This bit then
   determines if LNK_C_IN_ATTRIBS is used and MAIL_IN_ATTRIBS is called by the
   mail_server routine in MAIL$SERVER_MAIN. Until this code is expanded upon
   (or if we are willing to patch the MAIL image) it will not be possible for
   DELIVER to handle file attributes and the things they apply to like DDIF
   files. *)

[global] function MAIL_IN_ATTRIBS (var context : unsigned;
  var link_flag : integer; var idtld : file_attribute_block) : integer;

begin (* MAIL_IN_ATTRIBS *)
  if DEBUG_IN then writeln ('MAIL_IN_ATTRIBS called.');
  MAIL_IN_ATTRIBS := SS$_NORMAL;
end; (* MAIL_IN_ATTRIBS *)

(* MAIL_IO_WRITE is called by MAIL to tell DELIVER what it thinks of the
   results returned by the various MAIL_IN_ routines. *)
 
[global] function MAIL_IO_WRITE (var context : unsigned;
  var link_flag : integer;
  line : string_descriptor) : integer;
 
label
  99;

var
  error_text : string;
 
  function string_to_integer (var str : string_descriptor) : integer;
 
  var
    number : packed array [1..4] of char; i : integer;
 
  begin (* string_to_integer *)
    if str.length <> 4 then string_to_integer := 0 else begin
      for i := 1 to 4 do number[i] := str.address^[i];
      string_to_integer := number :: integer;
    end;
  end; (* string_to_integer *)
 
begin (* MAIL_IO_WRITE *)
  if DEBUG_IN then writeln ('MAIL_IO_WRITE called.');
  case top_of_stack (write_recv_states) of
    delivery_check : begin
                       if DEBUG_IN then writeln ('Delivery check.');
                       last_error := string_to_integer (line);
                       if DEBUG_IN then writeln (' got a stat : ', last_error);
                       pop (write_recv_states);
                       if not odd (last_error) and odd (sticky_error) then begin
                         sticky_error := last_error;
                         LIB$SET_SYMBOL ('DELIVER_STATUS',
                                         '%X' + hex (sticky_error, 8, 8));
                       end; (* if *)
                       if last_error <> SS$_NORMAL then
                         push (write_recv_states, error_msg);
                     end; (* delivery_check *)
    user_check :     begin
                       if DEBUG_IN then writeln ('User check.');
                       last_error := string_to_integer (line);
                       if DEBUG_IN then writeln (' got a stat : ', last_error);
                       pop (write_recv_states);
                       if not odd (last_error) and odd (sticky_error) then begin
                         sticky_error := last_error;
                         LIB$SET_SYMBOL ('DELIVER_STATUS',
                                         '%X' + hex (sticky_error, 8, 8));
                       end; (* if *)
                       if last_error <> SS$_NORMAL then
                         push (write_recv_states, error_msg);
                     end; (* user_check *)
    error_msg :      begin
                       if DEBUG_IN then writeln ('Error text.');
                       if (line.length = 1) and (line.address^[1] = chr (0))
                       then begin
                         if DEBUG_IN then
                           writeln (' got a NULL -- popping write_recv_states');
                       end else begin
                         copy_descr_to_string (line, error_text, DEBUG_IN);
                         if DEBUG_IN then
                           writeln ('Error message: "', error_text, '"');
                         if not odd (last_error) then
                           LIB$SET_SYMBOL ('DELIVER_MESSAGE', error_text);
                       end;
                       pop (write_recv_states);
                     end; (* error_text *)
    bad_msg :        begin
                       if DEBUG_IN then writeln ('Unexpected message.');
                       last_error := string_to_integer (line);
                       if not odd (last_error) and odd (sticky_error) then begin
                         sticky_error := last_error;
                         LIB$SET_SYMBOL ('DELIVER_STATUS',
                                         '%X' + hex (sticky_error, 8, 8));
                       end; (* if *)
                       if DEBUG_IN then
                         writeln (' UNEXPECTED stat : ', last_error);
                       push (write_recv_states, error_msg);
                     end; (* bad_msg *)
    otherwise begin
      LIB$SIGNAL (DELIVER__BADSTKELE);
      MAIL_IO_WRITE := DELIVER__BADSTKELE;
      goto 99;
    end;
  end; (* case *)
  MAIL_IO_WRITE := SS$_NORMAL;
99:
end; (* MAIL_IO_WRITE *)
 
[global] function MAIL_IO_READ (var context : unsigned;
  var link_flag : integer;
  var returned_line : string_descriptor) : integer;
 
begin (* MAIL_IO_READ *)
  if DEBUG_IN then writeln ('MAIL_IO_READ called.');
  MAIL_IO_READ := SS$_NORMAL;
end; (* MAIL_IO_READ *)
 
(* End of DELIVER.PAS *)
end.
