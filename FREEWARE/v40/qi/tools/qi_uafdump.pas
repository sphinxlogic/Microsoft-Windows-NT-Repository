[INHERIT ('SYS$LIBRARY:STARLET', 'SYS$LIBRARY:PASCAL$LIB_ROUTINES')]
Program UAF(Input, Output);

Type
  $UBYTE = [BYTE] 0..255;
  $UWORD = [WORD] 0..65535;
  $UQUAD = [QUAD, UNSAFE] RECORD L0, L1:Integer; END;

  Itemlist3 = Packed Record
		Case Integer of
		1: (Length: $UWORD;
		    Code: $UWORD;
		    Address: Integer;
		    Ret: Integer);
		2: (Term: Integer)
		End;

  UAF_Rec = Record
		Length: Integer;
		User_Name: [Key(0)] Packed Array [1..32] of Char;
		UIC: Integer;
		Filler: Packed Array [1..1000] of Char;
	    End;

Var
  UAF: File of UAF_Rec;
  Users: Text;
  Item_List: Array [1..10] of Itemlist3;
  Call_Status, Context: Unsigned;
  Expire, Login_I, Login_N, Pwd_Date: $UQUAD;
  Account, Owner, User_Name, Temp: Varying [35] of Char;
  D_Expire, D_Login_I, D_Login_N, D_Pwd_Date: Packed Array [1..8] of Char;
  UIC_Grp, UIC_Mem, Counter: Integer;
  D_UIC_Grp, D_UIC_Mem: Packed Array [1..6] of Char;
                                            
Function Check_File(File_Status: Integer): Boolean;
Begin
  If File_Status = 74 then
    LIB$WAIT(0.5)
  else If File_Status > 0 then Begin
    Writeln('UAFDUMP found a file status of ', File_Status, Chr(10), Chr(13));
    Halt
    End;
  Check_File := (File_Status <= 0)
End;

Begin
  Open(Output, Carriage_Control := None);	{ Regular output channel, turn off buffering }
  Writeln('UAFDUMP: Dump the User Authorization File', Chr(10), Chr(13));
  open(UAF, 'SYSUAF', history := readonly, organization := indexed,
       sharing := readwrite);
  reset(UAF);
  open(Users, 'UAFDATA.TXT');
  rewrite(Users);
  Context := 0;
  counter := 0;
  Call_Status := lib$init_date_time_context(Context, LIB$K_OUTPUT_FORMAT, '|!Y4!MN0!D0|');
  while not EOF(UAF) do Begin
    counter := counter + 1;
    if (counter mod 100) = 0 then writeln(counter:5, Chr(10), Chr(13));
    Item_List[1].Code := UAI$_ACCOUNT;
    Item_List[1].Length := 8;
    Item_List[1].Address := IAddress(Account.Body);
    Item_List[1].Ret := IAddress(Account.Length);
    Item_List[2].Code := UAI$_EXPIRATION;
    Item_List[2].Length := 8;
    Item_List[2].Address := IAddress(Expire);
    Item_List[2].Ret := 0;
    Item_List[3].Code := UAI$_LASTLOGIN_I;
    Item_List[3].Length := 8;
    Item_List[3].Address := IAddress(Login_I);
    Item_List[3].Ret := 0;
    Item_List[4].Code := UAI$_LASTLOGIN_N;
    Item_List[4].Length := 8;
    Item_List[4].Address := IAddress(Login_N);
    Item_List[4].Ret := 0;
    Item_List[5].Code := UAI$_OWNER;
    Item_List[5].Length := 32;
    Item_List[5].Address := IAddress(Owner.Body);
    Item_List[5].Ret := IAddress(Owner.Length);
    Item_List[6].Code := UAI$_PWD_DATE;
    Item_List[6].Length := 8;
    Item_List[6].Address := IAddress(Pwd_Date);
    Item_List[6].Ret := 0;
    Item_List[7].Code := UAI$_USERNAME;
    Item_List[7].Length := 12;
    Item_List[7].Address := IAddress(User_Name.Body);
    Item_List[7].Ret := IAddress(User_Name.Length);
    Item_List[8].Code := UAI$_MEM;
    Item_List[8].Length := 4;
    Item_List[8].Address := IAddress(UIC_Mem);
    Item_List[8].Ret := 0;
    Item_List[9].Code := UAI$_GRP;
    Item_List[9].Length := 4;
    Item_List[9].Address := IAddress(UIC_Grp);
    Item_List[9].Ret := 0;
    Item_List[10].Term := 0;
    Call_Status := $getuai(usrnam := UAF^.User_Name, itmlst := Item_List);
    Temp := SUBSTR(Owner, 2, Owner[1]::$UBYTE);
    Owner := PAD(Temp, ' ', 31);
    If Expire.L0 = 0 then D_Expire := '00000000'
    else if Expire.L0 = -1 then D_Expire := '99999999'
    else Call_Status := lib$format_date_time(D_Expire, Expire, Context);
    If Login_I.L0 = 0 then D_Login_I := '00000000'
    else if Login_I.L0 = -1 then D_Login_I := '99999999'
    else Call_Status := lib$format_date_time(D_Login_I, Login_I, Context);
    If Login_N.L0 = 0 then D_Login_N := '00000000'
    else if Login_N.L0 = -1 then D_Login_N := '99999999'
    else Call_Status := lib$format_date_time(D_Login_N, Login_N, Context);
    If Pwd_Date.L0 = 0 then D_Pwd_Date := '00000000'
    else if Pwd_Date.L0 = -1 then D_Pwd_Date := '99999999'
    else Call_Status := lib$format_date_time(D_Pwd_Date, Pwd_Date, Context);
    D_UIC_Grp := OCT(UIC_Grp, 6, 6);
    D_UIC_Mem := OCT(UIC_Mem, 6, 6);
    writeln(Users, User_Name, ' ', Owner, ' ',Account, ' ', D_Expire, ' ',
            D_Login_I, ' ', D_Login_N, ' ', D_Pwd_Date, ' ', D_UIC_Grp, ' ',
            D_UIC_Mem);
    Repeat
      Get(UAF, Error := Continue);
    Until Check_File(Status(UAF))
    End;
  close(UAF);
  close(users);
  Call_Status := lib$free_date_time_context(Context);
  Writeln('Done', Chr(10), Chr(13));
End.
