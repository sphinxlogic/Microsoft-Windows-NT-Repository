$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ if f$trnlnm("PLMENU$DEBUG") .nes. "" then set verify
$ set control = Y
$ on control_y then GOTO Control_y
$ set noon
$!
$ user = F$edit("''f$getjpi("","USERNAME")'","Trim,Upcase")
$ cur_dir = f$environment("default")
$ default_value = ""
$ facility = ""
$ mnemonic = ""
$ root = ""
$ remark = ""
$ source = ""
$ propagate = "YES"
$ type sys$input

Create Facility Version V1.3

Enter ? or "HELP" at any prompt to obtain help.
Prompt default values (if any) appear in brackets [].
Enter <Ctrl/Z>, "Quit" or "Exit" to exit immediately.

$!
$! Check if this account has write access to the Masterpack CMS libraries
$!
$ @vde$system:plmenu$$validate_cms_access
$ if .not. $status
$ then
$   write sys$output -
 "%CREFAC-F-NOCMSWRITE, This account does not have write access to MASD CMS libraries"
$   GOTO Main_Exit
$ endif
$!
$!
$!+
$!   USER PROMPTS
$!+----------------------------------------------------------------------------
$! obtain the VDE library mnemonic (eg EVMS, VMSTEST, etc.), and store it in
$! the symbol MNEMONIC.  From the mnemonic, derive the CMS root and store it
$! in ROOT. 
$
$! By default we use the specified mnemonic value, the translation of the
$! PLMENU$DEFAULT_PRODUCT logical name, or the first translation of the
$! system-wide library list.
$!+----------------------------------------------------------------------------
$Get_mnemonic:
$!----------
$ root = "???"
$ if mnemonic .nes. ""
$ then
$   default_value = mnemonic
$ else
$   default_value = f$trnlnm("PLMENU$DEFAULT_PRODUCT")
$   if default_value .eqs. "" then default_value = "''f$trnlnm(""VDE$LIBRARY_DEFAULTS_MNEMONICS"",,0)'"
$ endif
$!
$ user_prompt = "VDE library"
$ prompt_symbol = "mnemonic"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   type sys$input

   This is the first prompt.

$   GOTO Get_mnemonic
$ endif
$!
$Validate_mnemonic:
$!
$ @vde$system:plmenu$$set_vde_library 'mnemonic'
$ if .not. $status
$ then
$   root = "???"
$   mnemonic = ""
$   GOTO Get_mnemonic
$ endif
$ root = f$trnlnm("PLMENU$CMSROOT")
$ mnemonic = f$trnlnm("PLMENU$MNEMONIC")
$!
$!
$!+-----------------------------------------------------------------------------
$! obtain the development stream for which to create the new facility
$! store the value in symbol SOURCE
$!+-----------------------------------------------------------------------------
$Get_source:
$!----------
$ if source .nes. ""
$ then
$   default_value = source
$ else
$   default_value = f$trnlnm("PLMENU$DEFAULT_SOURCE_STREAM")
$ endif
$!
$ user_prompt = "Facility begins with what development stream?"
$ prompt_symbol = "source"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Mnemonic
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! obtain the new facility name
$! store the value in symbol FACILITY
$!+-----------------------------------------------------------------------------
$Get_Facility:
$!------------
$ if facility .nes. ""
$ then
$   default_value = facility
$ else
$   default_value = ""
$ endif
$!
$ user_prompt = "Facility Name?"
$ prompt_symbol = "facility"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Source
$ endif
$!
$! Insure that there is no CMS library for this facility already
$!
$Validate_Facility:
$!-----------------
$ if f$search("''root':[''facility'.CMS]00CMS.CMS") .nes. ""
$ then
$   write sys$output "%CREFAC-E-ALREXISTS, facility ''facility' CMS library already exists!" 
$   facility = ""
$   GOTO Get_Facility
$ endif   
$!
$!
$!+-----------------------------------------------------------------------------
$!  get a remark to be associated with the new facility
$!+-----------------------------------------------------------------------------
$Get_Remark:
$!----------
$ if remark .eqs. "" 
$ then
$   default_value = "''facility' FACILITY"
$ else
$   default_value = "''remark'"
$ endif
$!
$ user_prompt = "Remark string for ''facility'"
$ prompt_symbol = "remark"
$ GOSUB Get_Text_Value
$!
$ if f$locate("""",remark) .lt. f$length(remark)
$ then
$   remark = ""
$   GOTO Get_Remark
$ endif
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Facility
$ else
$   default_value = ""
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$!  See if the new facility accepts change propagation
$!+-----------------------------------------------------------------------------
$Get_Propagate:
$!------------
$ if propagate .eqs. "" 
$ then
$   default_value = "YES"
$ else
$   default_value = propagate
$ endif
$!
$ user_prompt = "Will facility ''facility' accept module change propagation?"
$ prompt_symbol = "propagate"
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Remark
$ else
$   default_value = ""
$ endif
$!
$ if propagate
$ then
$   propagate_qual = "/PROPAGATE"
$ else
$   propagate_qual = "/NOPROPAGATE"
$ endif
$!
$!
$!+-----------------------------------------------------------------------------
$! confirm the input
$!+-----------------------------------------------------------------------------
$ write sys$output ""
$ write sys$output " If you proceed, you will create the facility ''facility', under"
$ write sys$output " the CMS root ''root', in the VDE library ''mnemonic'.  The"
$ write sys$output " stream ''source' will be populated with three system build template"
$ write sys$output " files.  The facility is created directly by this procedure, and"
$ write sys$output " no batch job is submitted."
$ write sys$output ""
$ user_prompt = "Do you wish to proceed?"
$ default_value = "Yes"
$ prompt_symbol = "submit_ans"
$!
$ GOSUB Get_Text_Value
$!
$ if return_to_previous_prompt
$ then
$   GOTO Get_Propagate
$ endif
$!
$ if .not. submit_ans then GOTO Get_Mnemonic
$!
$!
$ valid_input = 1
$!
$ GOSUB Validate_Source
$ if .not. valid_input
$ then
$   source = ""
$   GOTO Get_Source
$ endif
$!
$ GOSUB Add_facility
$!
$Control_y:
$!
$Main_Exit:
$!---------
$ EXIT 1+0*'f$verify(sav_verify)
$!
$Error_Exit:
$!----------
$ EXIT 0+0*'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$!  Perform the VDE and CMS commands necessary to set up the new facility
$!+-----------------------------------------------------------------------------
$Add_Facility:
$!------------
$!
$ write sys$output "%CREFAC-I-CREATING, Creating facility ''facility'..."
$!
$ ON Error then GOTO Error_Exit
$!
$! Do the VDE command to create the VDE facility and the CMS library
$!
$ define/user/nolog sys$output _nla0:
$ define/user/nolog sys$error _nla0:
$ VDE SET PRIV CREFAC; SET STREAM 'SOURCE'; -
      CREATE FACILITY/REMARK="''remark'"'propagate_qual' 'facility'
$!
$! create the No propagate flag file if appropriate
$!
$ if .NOT. propagate
$ then
$   Create 'root':['facility']VMSCMS$NO_PROPAGATE.FLAG
$ endif
$!
$! Create the system build template files
$!rena 
$ facbld = f$edit(f$search("VMSCMS$:FACBLD.COM"),"collapse")
$ if f$leng(facbld) .eq. 0 then facbld = f$edit(f$search("VDE$EXAMPLES:FACBLD.COM"),"collapse")
$ write sys$output "%CREFAC-I-FACBLD, FACBLD loaded from ''facbld'" 
$ VDE SET FACILITY 'FACILITY';CREATE MODULE/INPUT='facbld' -
  /STREAM='SOURCE'/REMARK="Facility build command file" 'facility'BLD.COM 
$ facasm = f$edit(f$search("VMSCMS$:FACASM.DAT"),"collapse")
$ if f$leng(facasm) .eq. 0 then facasm = f$edit(f$search("VDE$EXAMPLES:FACASM.DAT"),"collapse")
$ write sys$output "%CREFAC-I-FACASM, FACASM loaded from ''facasm'" 
$ VDE SET FACILITY 'FACILITY';CREATE MODULE/INPUT='facasm' -
  /STREAM='SOURCE'/REMARK="Facility Assembly & Compile instructions" 'facility'ASM.DAT
$ faclnk = f$edit(f$search("VMSCMS$:FACLNK.COM"),"collapse")
$ if f$leng(facLNK) .eq. 0 then faclnk = f$edit(f$search("VDE$EXAMPLES:FACLNK.COM"),"collapse")
$ write sys$output "%CREFAC-I-FACLNK, FACLNK loaded from ''faclnk'"
$ VDE SET FACILITY 'FACILITY';CREATE MODULE/INPUT='faclnk' -
  /STREAM='SOURCE'/REMARK="Facility link file" 'facility'LNK.COM
$!
$! Create the CMS build group and insert the template files
$! 
$ define/nolog CMS$LIB 'root':['facility'.CMS]
$ CMS Create Group BUILD "Build Files"
$ CMS insert element 'facility'BLD.COM, 'facility'ASM.DAT, 'facility'LNK.COM BUILD "initial entry"
$!
$ set noon
$ write sys$output "%CREFAC-S-CREATED, facility ''facility' created" 
$ RETURN
$!
$!+-----------------------------------------------------------------------------
$! validate the source stream exists before submitting the job
$!+-----------------------------------------------------------------------------
$Validate_source:
$ write sys$output "Validating stream ''source' in VDE..."
$ define/user/nolog sys$output _nla0:
$ define/user/nolog sys$error _nla0:
$ VDE show stream 'source'
$ if $status .ne. %x10000001
$ then
$   write sys$output "%CREFAC-E-VALERR, validation error for stream ''source' in VDE"
$   source = ""
$   valid_input = 0
$ endif
$!
$ RETURN
$!
$!
$!+-----------------------------------------------------------------------------
$!
$! obtain a text value for the symbol name stored in symbol prompt_symbol
$!
$!  prompt_symbol => symbol to contain the value to the symbol name to prompt
$!  user_prompt   => prompt string for 'prompt_symbol'
$!  default_value => default value for 'prompt_symbol'(if any)
$!
$!+-----------------------------------------------------------------------------
$Get_Text_Value:
$!-------------
$ return_to_previous_prompt = 0
$!
$ if default_value .eqs. ""
$ then
$   inquire_prompt = user_prompt + ": "
$ else
$   inquire_prompt = user_prompt + " [''default_value']: "
$ endif
$!
$ read/End_of_File=Main_Exit/Error=time_expired/timeout=255 -
      /prompt=" ''inquire_prompt'" SYS$COMMAND 'prompt_symbol' 
$!
$ 'prompt_symbol' = f$edit('prompt_symbol',"TRIM")
$!
$ if 'prompt_symbol'  .eqs. ""
$ then
$   if default_value .eqs. ""
$   then
$	goto Get_Text_Value 
$   else
$       'prompt_symbol'  = default_value
$   endif
$ endif
$!
$ temp_value = f$extract(0,4,f$edit('prompt_symbol',"UPCASE"))
$!
$ if temp_value .eqs. "?" .or. (F$locate(temp_value,"HELP") .eq. 0) 
$ then
$   GOSUB Help_'prompt_symbol'
$   GOTO Get_Text_Value
$ endif
$
$ if F$locate(temp_value,"QUIT") .eq. 0 .OR. -
     F$locate(temp_value,"EXIT") .eq. 0 
$ then
$   GOTO Main_Exit
$ endif
$!
$ if F$locate(temp_value,"PREV") .eq. 0 .OR. -
     F$locate(temp_value,"BACK") .eq. 0 
$ then
$   'prompt_symbol' = default_value
$   return_to_previous_prompt = 1
$ endif
$!
$ RETURN
$!
$Time_Expired:
$!------------
$ write sys$output "Timeout on input, exiting..."
$ GOTO Main_exit
$!
$Help_Mnemonic:
$!
$ type sys$input 

    Enter the VDE library mnemonic on which to operate.  Examples
    of mnemonics are "EVMS" and "VMSTEST".  Define the logical name
    PLMENU$DEFAULT_PRODUCT to override the default value (the first
    translation from VDE$LIBRARY_DEFAULTS_MNEMONICS) for this prompt.

$!
$ RETURN
$!
$Help_Remark:
$!
$ write sys$output ""
$ write sys$output -
"    Enter a remark to be associated with facility ''facility'." 
$!
$ type sys$input

    Enter BACK or PREV to return to build stream name prompt.

$!
$ RETURN
$!
$Help_source:
$!-----------
$!
$ type sys$input

    It is required that the new facility be associated with a development
    stream.  The stream should be the initial version in which you expect the   
    the components of this facility to ship.  Define the logical
    PLMENU$DEFAULT_SOURCE_STREAM to automatically supply a default value
    to this prompt.  Enter BACK or PREV to return to mnemonic name prompt.

$!
$ RETURN
$!
$!
$Help_Facility:
$!-------------
$!
$ type sys$input

    Enter the new facility name.  Enter BACK or PREV to return to stream
    name prompt.

$!
$ RETURN
$!
$Help_Remark:
$!-----------
$!
$ write sys$output ""
$ write sys$output -
"    Enter a remark to be associated with facility ''facility'." 
$!
$ type sys$input
    Enter BACK or PREV to return to facility name prompt.

$!
$ RETURN
$!
$Help_Propagate:
$!--------------
$ type sys$input

    Enter affirmatively if this facility will be active for more than one
    development stream.  If this facility is only to be used by one stream, for
    example to hold a release's update files, answer negatively to this
    questions.  Enter BACK or PREV to return to facility remark prompt.

$!
$ RETURN
$!
$Help_Submit_Ans:
$!---------------
$!
$   type sys$input

    Answer affirmatively if you wish to proceed and create the facility.
    Enter BACK or PREV to return to the previous prompt.

$!
$ RETURN
$!+-----------------------------------------------------------------------------
$!
$! CREATE_FACILITY.COM - Creates a new facility
$!
$!  Author: David Sweeney	15-APR-1992
$!+-----------------------------------------------------------------------------
