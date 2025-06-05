$ $Verify	= F$Verify (F$TrnLnm ("UAI$Verify"))
$ ! ++
$ ! Facility:	UAI$EXPIRED_ACCOUNTS.COM
$ !
$ ! Abstract:	This procedure will check SYSUAF records for active accounts
$ !		with imminent expiration dates.  It will then create a
$ !		command procedure to reset the expiration date for these
$ !		accounts, to be run by the System Manager after checking the
$ !		procedure.  It will also create a separate command procedure
$ !		to disable accounts which have been inactive for a defined
$ !		period. Accounts with either no expiration date or with the
$ !		DisUser flag set will not be modified.
$ !
$ !		Also, as an experiment, it sends an email to any accounts
$ !		nearing the deadline of being disabled due to inactivity, in
$ !		the hope that they have email forwarded to an account that they
$ !		actually use.
$ !
$ ! Logical Names:
$ !		Verification can be enabled by defining the following logical 
$ !		names:
$ !
$ !		UAI$Debug	- detail of each phase of checking
$ !		UAI$Verbose	- general information
$ !		UAI$Verify	- DCL verification
$ !
$ !
$ !		Warnings of pending account expiry is controlled using the 
$ !		logical name:
$ !
$ !		UAI$Mail_Expiry_Warning	
$ !
$ !		If TRUE warnings will be sent to the end user (default FALSE)
$ !
$ !
$ !		Use the following logical names to override the default values:
$ !
$ !		UAI$Expiry_Days		Default	  64 (+)
$ !		UAI$Usage_Days		Default	  64 (-)
$ !		UAI$Disable_Days	Default	 365 (-)
$ !		UAI$Renewal_Days	Default	 365 (+)
$ !
$ !		Theses logical names are used to calculate the following:
$ !
$ ! 		Expiry_Window		check accounts with an expiration date
$ !					before UAI$Expiry_Days
$ ! 		Usage_Window		interactive or non-interactive usage 
$ !					since UAI$Usage_Days
$ ! 		Disable_Window 		no interactive or non-interactive 
$ !					usage since Today-UAI$Disable_Days
$ ! 		Renewal_Window		eligable accounts will have their 
$ !					expiration reset to 
$ !					Today+UAI$Renewal_Days
$ !
$ ! Environment: No particular environment is assumed.
$ !
$ ! Inputs:-
$ !
$ !		[Sys$System:]SysUAF.Dat
$ !		Sys$Scratch:UAI$Records.Tmp - deleted on exit
$ !
$ ! Outputs:-
$ !
$ !		Sys$Scratch:UAI$Records.Tmp - deleted on exit
$ !		Sys$Scratch:<Username>_Expire.Tmp - deleted on exit
$ !		Sys$Login:UAI$Expired_Accounts_LckDwn.Com
$ !		Sys$Login:UAI$Expired_Accounts_Reset.Com
$ !
$ !
$ ! Author:	Ian Ring
$ ! Creation:	21 August 2001
$ !
$ !
$ ! Modified by:
$ !
$ ! 14-JUN-2002	IR	Added information to Rules: regarding DisUser flag
$ !
$ !
$ !--
$ 
$ ! +
$ ! Variables
$ ! -
$ !
$ ! Expiry_Window - check accounts with an expiration date before UAI$Expiry_Days
$ !
$ UAI$Expiry_Days	= F$Integer (F$TrnLnm ("UAI$Expiry_Days"))
$ If (UAI$Expiry_Days .Eq. 0) Then UAI$Expiry_Days = 64
$ !
$ Expiry_Window_A	= F$CVtime ("Today+''UAI$Expiry_Days'-", "Absolute", "Date")
$ Expiry_Window_C	= F$CVtime (Expiry_Window_A, "Comparison", "Date")
$ !
$ ! Usage_Window - interactive or non-interactive usage since UAI$Usage_Days
$ !
$ UAI$Usage_Days	= F$Integer (F$TrnLnm ("UAI$Usage_Days"))
$ If (UAI$Usage_Days .Eq. 0) Then UAI$Usage_Days = 64
$ !
$ Usage_Window_A	= F$CVtime ("Today-''UAI$Usage_Days'-", "Absolute", "Date")
$ Usage_Window_C	= F$CVtime (Usage_Window_A, "Comparison", "Date")
$ !
$ ! Disable_Window - no interactive or non-interactive usage since Today-UAI$Disable_Days
$ !
$ UAI$Disable_Days	= F$Integer (F$TrnLnm ("UAI$Disable_Days"))
$ If (UAI$Disable_Days .Eq. 0) Then UAI$Disable_Days = 365
$ !
$ Disable_Window_A	= F$CVtime ("Today-''UAI$Disable_Days'-", "Absolute", "Date")
$ Disable_Window_C	= F$CVtime (Disable_Window_A, "Comparison", "Date")
$ !
$ ! Renewal_Window - eligable accounts will have their expiration reset to Today+UAI$Renewal_Days
$ !
$ UAI$Renewal_Days	= F$Integer (F$TrnLnm ("UAI$Renewal_Days"))
$ If (UAI$Renewal_Days .Eq. 0) Then UAI$Renewal_Days = 365
$ !
$ Renewal_Window_A	= F$CVtime ("Today+''UAI$Renewal_Days'-", "Absolute", "Date")
$ !
$ Today_C		= F$CVtime ("Today", "Comparison", "Date")
$ !
$ ! +
$ ! General Symbols
$ ! -
$ !
$ SayF			= "Write Sys$Output F$Fao"
$ !
$ UAI$Debug		= F$TrnLnm ("UAI$Debug")
$ UAI$Verbose		= F$TrnLnm ("UAI$Verbose")
$ !
$ UAI$Mail_Expiry_Warning	= F$Integer (F$TrnLnm ("UAI$Mail_Expiry_Warning"))
$ !
$ Procedure	= F$Environment ("Procedure")
$ File		= F$Parse (Procedure, , , "Name")
$ Type		= F$Parse (Procedure, , , "Type")
$ !
$ ! +
$ ! Error handling and announcements
$ ! -
$ !
$ On Control_Y Then Goto Abort
$ On Error Then Goto Abort
$ !
$ Sayf ("!/!AS!AS, check SYSUAF for unused or expiring accounts and disable or renew expiration!/", File, Type)
$ !
$ Sayf ("Rules:")
$ Sayf ("o Do nothing if account is DisUsered")
$ Sayf ("o Expiration date within the next !SL day!%S", UAI$Expiry_Days)
$ Sayf ("o Mark for new expiration date if interactive or non-interactive usage is within the last !SL day!%S", UAI$Usage_Days)
$ Sayf ("o Mark account to be disabled if no interactive or non-interactive usage for !SL day!%S", UAI$Disable_Days)
$ Sayf ("o Mark eligable accounts for a new expiration date in !SL day!%S!/", UAI$Renewal_Days)
$ !
$ Sayf ("Procedure will!0UL!0%C not!%F mail warning of pending account expiry to end users!/", UAI$Mail_Expiry_Warning)
$ !
$ !
$ ! +
$ ! Main procedure
$ ! -
$ !
$ If (F$Search ("Sys$Login:UAI$Expired_Accounts_LckDwn.Com") .Nes. "") Then -
   Rename /NoLog /NoConfirm Sys$Login:UAI$Expired_Accounts_LckDwn.Com;* *.Com_Old;
$ If (F$Search ("Sys$Login:UAI$Expired_Accounts_Reset.Com") .Nes. "") Then -
   Rename /NoLog /NoConfirm Sys$Login:UAI$Expired_Accounts_Reset.Com;* *.Com_Old;
$ !
$ If (F$Trnlnm ("SYSUAF") .Eqs. "") Then Define /NoLog SYSUAF Sys$System:SysUAF
$ !
$ Message = F$Environment ("Message")
$ If .not. (UAI$Debug) Then -
   Set Message /NoFacility /NoSeverity /NoIdentification /NoText
$ !
$ MC Authorize List /Full *
$ !
$ Search /Output=Sys$Scratch:UAI$Records.Tmp SysUAF.Lis Username:, Flags:, Expiration:, "Last Login:"
$ !
$ Open /Read UAFrec Sys$Scratch:UAI$Records.Tmp
$ !
$Get_Details:
$ Read /End=Get_Details_End UAFrec Rec
$ Username 		= F$Element (1, " ", Rec)
$ !
$ Read /End=Get_Details_End UAFrec Rec
$ Rec			= F$Edit (Rec, "Compress")
$ Flags 		= F$Element (1, ":", Rec)
$ Flags 		= F$Edit (Flags, "Trim")
$ !
$ Read /End=Get_Details_End UAFrec Rec
$ Rec			= F$Edit (Rec, "Compress")
$ Expiration_A    = F$Element (1, " ", Rec)
$ !
$ Read /End=Get_Details_End UAFrec Rec
$ Rec			= F$Edit (Rec, "Compress")
$ Last_I_Login_A	= F$Element (2, " ", Rec)
$ If (Last_I_Login_A .Eqs. "(none)")
$ Then
$   Last_NonI_Login_A	= F$Element (4, " ", Rec)
$ Else
$   Last_NonI_Login_A	= F$Element (5, " ", Rec)
$ Endif
$ !
$ If (UAI$Verbose)
$ Then
$   SayF ("!/!12AS !AS", "Username:", Username)
$   SayF ("!12AS !AS", "Flags:", Flags)
$   SayF ("!12AS !AS", "Expiration:", Expiration_A)
$   SayF ("!12AS !AS (interactive), !AS (non-interactive)", "Last Login:", Last_I_Login_A, Last_NonI_Login_A)
$ Endif
$ !
$ If (F$Locate ("DisUser", Flags) .Nes. F$Length (Flags))
$ Then
$   If (UAI$Debug)
$   Then
$       Sayf ("!12AS Account is DisUsered, no further action will be taken", Username)
$   Endif
$   Goto Get_Details
$ Endif
$ !
$ If (Expiration_A .Eqs. "(none)")
$ Then
$   Sayf ("!12AS has no expiration date", Username)
$   If (UAI$Debug)
$   Then
$       !
$   Endif
$   Goto Get_Details
$ Endif
$ !
$ Expiration_C		= F$CVtime (Expiration_A, "Comparison", "Date")
$ !
$ If (Last_I_Login_A .Eqs. "(none)")
$ Then
$   Last_I_Login_C		= F$CVtime ("17-Nov-1858", "Comparison", "Date")
$ Else
$   Last_I_Login_C		= F$CVtime (Last_I_Login_A, "Comparison", "Date")
$ Endif
$ !
$ If (Last_NonI_Login_A .Eqs. "(none)")
$ Then
$   Last_NonI_Login_C		= F$CVtime ("17-Nov-1858", "Comparison", "Date")
$ Else
$   Last_NonI_Login_C		= F$CVtime (Last_NonI_Login_A, "Comparison", "Date")
$ Endif
$ !
$ !
$ If (Expiration_C .Lts. Expiry_Window_C)
$ Then
$   Renew_Expiry = 0
$   Disable_Account = 0
$   If (UAI$Debug) 
$   Then
$       If .Not. (UAI$Verbose) Then Sayf ("!12AS -", Username)
$       Sayf ("!13* Expiration (!AS) is within the expiry window (!AS)", Expiration_A, Expiry_Window_A)
$   Endif
$   If (Last_I_Login_C .Gts. Usage_Window_C)
$   Then
$       If (UAI$Debug) Then -
         Sayf ("!13* Last interactive login (!AS) is within usage window (since !AS)", Last_I_Login_A, Usage_Window_A)
$       Renew_Expiry = 1
$   Else
$       If (UAI$Debug) Then -
         Sayf ("!13* Last interactive login (!AS) is outside usage window (since !AS)", Last_I_Login_A, Usage_Window_A)
$       If (Last_NonI_Login_C .Gts. Usage_Window_C)
$       Then
$           If (UAI$Debug) Then -
             Sayf ("!13* Last non-interactive login (!AS) is within usage window (since !AS)", Last_NonI_Login_A, Usage_Window_A)
$           Renew_Expiry = 1
$       Else
$           If (UAI$Debug) Then -
             Sayf ("!13* Last non-interactive login (!AS) is outside usage window (since !AS)", Last_NonI_Login_A, Usage_Window_A)
$           If ((Last_I_Login_C .Lts. Disable_Window_C) .And. (Last_NonI_Login_C .Lts. Disable_Window_C))
$           Then
$               If (UAI$Debug) Then -
                 Sayf ("!13* Last interactive and non-interactive use is prior to disable window (!AS)", Disable_Window_A)
$               Disable_Account = 1
$           Endif
$       Endif
$   Endif
$   If (Disable_Account)
$   Then
$       If (F$TrnLnm ("Accounts_LckDwn") .Eqs. "") 
$       Then 
$           Open /Write Accounts_LckDwn Sys$Login:UAI$Expired_Accounts_LckDwn.Com
$           Write Accounts_LckDwn "$ ! ++"
$           Write Accounts_LckDwn "$ ! Facility:		UAI$EXPIRED_ACCOUNTS_LCKDWN.COM"
$           Write Accounts_LckDwn "$ ! Created by:		''File'''Type'"
$           Write Accounts_LckDwn "$ ! Date:		''F$Fao ("!11%D", 0)'"
$           Write Accounts_LckDwn "$ ! --"
$           Write Accounts_LckDwn "$ "
$           Write Accounts_LckDwn "$ Write Sys$Output ""This command procedure will disable specified accounts"""
$           Write Accounts_LckDwn "$ Read /End_of_File=End Sys$Command Okay /Prompt=""Okay to continue? [No]: """
$           Write Accounts_LckDwn "$ If .Not. (Okay) Then Goto End"
$           Write Accounts_LckDwn "$ !"
$           Write Accounts_LckDwn "$ If (F$Trnlnm (""SYSUAF"") .Eqs. """") Then Define /NoLog SYSUAF Sys$System:SysUAF"
$           Write Accounts_LckDwn "$ !"
$       Endif
$       Write Accounts_LckDwn "$ ! +"
$       Write Accounts_LckDwn "$ ! Username ''Username' - Expiration ''Expiration_A'"
$       Write Accounts_LckDwn "$ ! Last Login: ''Last_I_Login_A' (interactive), ''Last_NonI_Login_A' (non-interactive)"
$       Write Accounts_LckDwn "$ ! Flags: ''Flags'"
$       Write Accounts_LckDwn "$ ! "
$       Write Accounts_LckDwn "$ Read /End_of_File=End Sys$Command Okay /Prompt=""Disable ''Username'? [No]: """
$       Write Accounts_LckDwn "$ If .Not. (Okay) Then Goto ''Username'_End"
$       Write Accounts_LckDwn "$ MC Authorize Modify ''Username' /Flags=DisUser /NoAccess"
$       Write Accounts_LckDwn "$ Write Sys$Output ""''Username' has been disabled"""
$       Write Accounts_LckDwn "$ ! "
$       Write Accounts_LckDwn "$''Username'_End:"
$       Write Accounts_LckDwn "$ ! -"
$       Write Accounts_LckDwn "$ ! "
$       If (UAI$Debug) Then -
         Sayf ("!12AS will be marked to be disabled", Username)
$   Endif
$   !
$   If (Renew_Expiry)
$   Then
$       If (F$TrnLnm ("Accounts_Reset") .Eqs. "") 
$       Then 
$           Open /Write Accounts_Reset Sys$Login:UAI$Expired_Accounts_Reset.Com
$           Write Accounts_Reset "$ ! ++"
$           Write Accounts_Reset "$ ! Facility:		UAI$EXPIRED_ACCOUNTS_RESET.COM"
$           Write Accounts_Reset "$ ! Created by:		''File'''Type'"
$           Write Accounts_Reset "$ ! Date:		''F$Fao ("!11%D", 0)'"
$           Write Accounts_Reset "$ ! --"
$           Write Accounts_Reset "$ "
$           Write Accounts_Reset "$ Write Sys$Output ""This command procedure will reset the expiry date of specified accounts"""
$           Write Accounts_Reset "$ Read /End_of_File=End Sys$Command Okay /Prompt=""Okay to continue? [No]: """
$           Write Accounts_Reset "$ If .Not. (Okay) Then Goto End"
$           Write Accounts_Reset "$ !"
$           Write Accounts_Reset "$ If (F$Trnlnm (""SYSUAF"") .Eqs. """") Then Define /NoLog SYSUAF Sys$System:SysUAF"
$           Write Accounts_Reset "$ !"
$       Endif
$       Write Accounts_Reset "$ ! +"
$       Write Accounts_Reset "$ ! Username ''Username' - Expiration ''Expiration_A'"
$       Write Accounts_Reset "$ ! Last Login: ''Last_I_Login_A' (interactive), ''Last_NonI_Login_A' (non-interactive)"
$       Write Accounts_Reset "$ ! "
$       Write Accounts_Reset "$ Read /End_of_File=End Sys$Command Okay -"
$       Write Accounts_Reset "   /Prompt=""Set expiration of ''Renewal_Window_A' for ''Username'? [No]: """
$       Write Accounts_Reset "$ If .Not. (Okay) Then Goto ''Username'_End"
$       Write Accounts_Reset "$ MC Authorize Modify ''Username' /Expiration=''Renewal_Window_A'"
$       Write Accounts_Reset "$ Write Sys$Output -"
$       Write Accounts_Reset """ Account ''Username' expiration ''Expiration_A' has been reset ''Renewal_Window_A'"""
$       Write Accounts_Reset "$ ! "
$       Write Accounts_Reset "$''Username'_End:"
$       Write Accounts_Reset "$ ! -"
$       Write Accounts_Reset "$ ! "
$       Sayf ("!12AS expiration (!AS) would be reset", Username, Expiration_A)
$   Endif
$   !
$   If (.not. (Renew_Expiry) .And. .not. (Disable_Account))
$   Then
$       !
$       ! Account has an expiry date before Expiry_Window
$       ! Has not been active during Usage_Window
$       ! If already expired, no point alerting for renewed usage
$       ! If constant factors, it will remain dormant until Disable_Window
$       ! 
$       If (UAI$Debug) Then -
         Sayf ("!13* !AS will not be modified", Username)
$       If (Expiration_C .Ges. Today_C)
$       Then
$           !
$           ! Account is yet to expire
$           !
$           If (UAI$Debug) Then -
             Sayf ("!13* !AS will expire on !AS if no further activity is recorded before that date!/", Username, Expiration_A)
$           If ((UAI$Mail_Expiry_Warning) .And. (F$Locate ("DisMail", Flags) .Eqs. F$Length (Flags)))
$           Then
$               If (UAI$Debug) Then -
                 Sayf ("!13* Mail warning to !AS.", Username)
$               !
$               Open /Write /Error=Mail_Msg_Err Mail_Msg Sys$Scratch:'Username'_Expire.Tmp
$               Write Mail_Msg ""
$               Write Mail_Msg "Your account ''Username' will expire on ''Expiration_A' if no further "
$               Write Mail_Msg "activity is recorded before that date"
$               Write Mail_Msg ""
$               Write Mail_Msg "Last Login information: ''Last_I_Login_A' (interactive), ''Last_NonI_Login_A' (non-interactive)"
$               Write Mail_Msg ""
$               Write Mail_Msg ""
$               Write Mail_Msg "Please contact the System Manager for further information"
$               Close Mail_Msg
$		Set NoOn
$               Mail /Self Sys$Scratch:'Username'_Expire.Tmp  'Username' -
                 /Subject="Warning: Account ''Username' will expire on ''Expiration_A'"
$               On Error Then Goto Abort
$               Delete /NoLog /NoConfirm Sys$Scratch:'Username'_Expire.Tmp;*
$           Else
$               If (UAI$Debug) Then -
                Sayf ("!13* Will not mail warning to !AS.", Username)
$           Endif
$       Else
$           !
$           ! Account has expired
$           !
$           If (UAI$Debug) Then -
             Sayf ("!13* Expired on !AS and will be disabled if dormant for !SL day!%S", Expiration_A, UAI$Disable_Days)
$       Endif
$   Endif
$   If ((UAI$Debug) .And. .Not. (UAI$Verbose)) Then -
     Sayf ("")
$ Endif
$ !
$ Goto Get_Details
$ !
$Get_Details_End:
$ !
$ If (F$TrnLnm ("Accounts_LckDwn") .Nes. "") 
$ Then
$   Sayf ("!/!/''F$Parse ("Sys$Login:UAI$Expired_Accounts_LckDwn.Com")' created")
$   Write Accounts_LckDwn "$End: "
$   Write Accounts_LckDwn "$ Exit "
$ Endif
$ !
$ If (F$TrnLnm ("Accounts_Reset") .Nes. "") 
$ Then
$   If (F$TrnLnm ("Accounts_LckDwn") .Eqs. "") Then Sayf ("!/")
$   Sayf ("''F$Parse ("Sys$Login:UAI$Expired_Accounts_Reset.Com")' created")
$   Write Accounts_Reset "$End: "
$   Write Accounts_Reset "$ Exit "
$ Endif
$ !
$ ! +
$ ! Common exit routine
$ ! - 
$ SayF ("!/!AS!AS, completed!/", File, Type)
$ !
$End:
$ If (F$TrnLnm ("UAFrec") .Nes. "") Then Close UAFrec
$ If (F$TrnLnm ("Accounts_LckDwn") .Nes. "") Then Close Accounts_LckDwn 
$ If (F$TrnLnm ("Accounts_Reset") .Nes. "") Then Close Accounts_Reset 
$ If (F$Search ("Sys$Login:UAI$Expired_Accounts_LckDwn.Com;-4") .Nes. "") Then -
   Purge /NoLog /Keep=4 Sys$Login:UAI$Expired_Accounts_LckDwn.Com
$ If (F$Search ("Sys$Login:UAI$Expired_Accounts_Reset.Com;-4") .Nes. "") Then -
   Purge /NoLog /Keep=4 Sys$Login:UAI$Expired_Accounts_Reset.Com
$ If (F$Search ("Sys$Scratch:UAI$Records.Tmp") .Nes. "") Then -
   Delete /NoConfirm /NoLog Sys$Scratch:UAI$Records.Tmp;*
$ If (F$Search ("Sys$Scratch:*_Expire.Tmp") .Nes. "") Then -
   Delete /NoConfirm /NoLog Sys$Scratch:*_Expire.Tmp;*
$ If (F$Search ("SysUAF.Lis") .Nes. "") Then -
   Delete /NoConfirm /NoLog SysUAF.Lis;*
$ !
$ If (F$Type (Message) .Nes. "") Then Set Message 'Message'
$ !
$ Exit							! 'F$Verify ($Verify)'
$ !
$ !
$ !
$Mail_Msg_Err:
$ Status = $Status
$ Write Sys$Output "Error creating Sys$Scratch:''Username'_Expire.Tmp"
$ Write Sys$Output F$Message (Status)
$ Goto End
$ !
$Abort:
$ Status = $Status
$ SayF ("!/!AS!AS, Aborted!/", File, Type)
$ Write Sys$Output F$Message (Status)
$ Goto End
$ !
