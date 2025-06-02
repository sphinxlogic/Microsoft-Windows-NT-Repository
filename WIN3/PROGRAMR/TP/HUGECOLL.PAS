L{File name :HUGECOLL.PAS; Revision Date 23/5/1992 Size :556 Lines }
unit hugecoll; {implement huge collection in TurboPascal for Windows}

interface

{----------Huge Collection and Huge SortedCollection Object----------}
{      May 1992                                                      }
{      Ver 0.1       (c) Nicholas Waltham, Oxford, United Kingdom    }
{                        <SPEEDY%UK.AC.OX.VAX@UKACRL>                }
{                        <100013.3330@COM.COMPUSERVE>                }
{                                                                    }
{                    Thanks to Jeroen Pluimers and other members of  }
{                    the Usenet community for memory handling advice }
{--------------------------------------------------------------------}

{ NB                                                                    }
{                                                                       }
{ o Programs compiled with the only386 option defined will not run in   }
{   real mode - but who runs a 386 in real mode anyway!                 }

{ o If anyone makes any significant alterations or has any bright ideas }
{   then please forward them to me so I can keep one up to date copy    }

{ o This is supplied as is - there is no warrenty expressed or implied  }

{ o This code is released to the public domain and may be freely copied }
{   No money must be charged for this code                              }


uses
 Wintypes,WinProcs,WObjects,Strings;

{$I p:\shared\objid.inc}
 { This is a Pascal '.INC' file containing contants for all my object ids I have ever written
   and prevents me from assigning the same id twice. You will need to define oidHugeCollection
   and oidHugeSortedCollection constants for this unit}

{
{$DEFINE Only386}

 {Define this flag is the subsequent code is only going to run on a 386base computer
  or above - includes pointer calculation optimisation}


type
  LongType = record
    case Word of
      0: (Ptr: Pointer);
      1: (Long: Longint);
      2: (Lo: Word;
	  Hi: Word);
  end;

 ppointer = ^pointer;


 pHugeCollection = ^tHugeCollection;
 tHugeCollection = Object (tObject)


                   Items  : tHandle; {Handle to Global Memory}
                   Count  : longint; {Current Number of Items}
                   Limit  : longint; {Current Allocated size}
                   Delta  : longint; {Number of items by which the collection grows when full}


                   base   : longtype;  {global pointer to memory when locked}


                   constructor init(aLimit, aDelta : Longint);

                   constructor Load(Var S : tStream);


                   destructor  done; virtual;

                   function    At            (Index : Longint) : Pointer;
                   procedure   AtDelete      (Index : Longint);
                   procedure   AtInsert      (Index : Longint; Item : Pointer);
                   procedure   AtPut         (Index : Longint; Item : Pointer);
                   procedure   Delete        (Item : Pointer);
                   procedure   DeleteAll;
                   procedure   Error         (Code,Info : Integer); virtual;
                   function    FirstThat     (Test : Pointer) : Pointer;
                   procedure   ForEach       (Action : Pointer);
                   procedure   Free          (Item : Pointer);
                   procedure   FreeAll;
                   procedure   FreeItem      (Item : Pointer); virtual;
                   function    GetItem       (Var S : tStream) : Pointer; virtual;
                   function    IndexOf       (Item : Pointer) : longint; virtual;
                   procedure   Insert        (Item : Pointer); virtual;
                   function    LastThat      (Test : Pointer) : Pointer;
                   procedure   Pack;
                   procedure   PutItem       (Var S : tStream; Item : Pointer); virtual;
                   procedure   SetLimit      (aLimit : Longint);virtual;

                   procedure   Store         (Var S : tStream);

                   procedure   AtZero        (Index : Longint);

                   procedure   Lock;
                   procedure   UnLock;

                   end;



 pHugeSortedCollection = ^tHugeSortedCollection;

 tHugeSortedCollection = Object(tHugeCollection)


                         function       Compare (Key1,Key2 : Pointer): Integer; virtual;
                         function       IndexOf (Item : Pointer): Longint; virtual;
                         procedure      Insert  (Item : Pointer); virtual;
                         function       KeyOf   (Item : Pointer): Pointer; virtual;
                         function       Search  (key : Pointer; Var Index : Longint) : Boolean; virtual;


                         end;



pCharHugeCollection        = ^tCharHugeCollection;
tCharHugeCollection        = Object(tHugeCollection)

                             procedure   FreeItem      (Item : Pointer); virtual;

                             end;

pStrHugeCollection         = ^tStrHugeCollection;
tStrHugeCollection         = Object(tHugeSortedCollection)


                              function       Compare       (Key1,Key2 : Pointer): Integer; virtual;
                              procedure      FreeItem      (Item : Pointer); virtual;

                              end;



const
 RHugeCollection : tStreamRec =
  (ObjType : oidHugeCollection;
   VmtLink : Ofs(Typeof(tHugeCollection)^);
   Load    : @tHugeCollection.load;
   Store   : @tHugeCollection.Store);

 RHugeSortedCollection : tStreamRec =

  (ObjType : oidHugeSortedCollection;
   VmtLink : Ofs(Typeof(tHugeSortedCollection)^);
   Load    : @tHugeSortedCollection.load;
   Store   : @tHugeSortedCollection.Store);



implementation


Procedure _AHShift;  External 'KERNEL' Index 113;
procedure _AHIncr;far; external 'Kernel' index 114; {The MAGINC! function}

const
 cAHShift = {Ofs(_AHShift)}3 ;{This won't work in real mode!}
  AHShift : word = cAHShift;
 cAHIncr  = {Ofs(_AHShift)}8 ;{This won't work in real mode!}
  AHIncr  : word = cAHIncr;


{$IFDEF Only386}

 function Compute(base : Pointer;aIndex : Longint) : Pointer;
 inline(
 $66/$5B                      {Pop EBX                  ; Load EBX with Index}
 /$58                         {Pop AX                   ; Load AX  with Offset(base)
                                                         (Sensible since pointers are returned as DX:AX}
 /$5A                         {Pop DX                   ; Load DX  with Segment(base) }
 /$66/$C1/$E3/$02             {SHL EBX,2                ; Multiply EBX by 4           }
 /$03/$C3                     {ADD AX,BX                ; Add Lower half of pointer to AX}
 /$33/$DB                     {XOR BX,BX                ; Zero bottom 16bits of EBX      }
 /$66/$C1/$EB/<($10-cAHShift) {SHR EBX,16 - AHShift     ; Shift Top of EBX into BX compensating for AHShift}
                                                         {This won't work in real mode}
 /$03/$D3                     {ADD DX,BX                ; Add to BX}
 );

{$ELSE}
 function Compute(base : Pointer;aIndex : Longint) : Pointer;
 INLINE(
$5B                     { POP BX                             }
/$5A                    { POP DX                             }
/$58                    { POP AX                             }
/$D1/$E3                { SHL BX,1                           }
/$D1/$D2                { RCL DX,1                           }
/$D1/$E3                { SHL BX,1                           }
/$D1/$D2                { RCL DX,1                           }
/$03/$C3                { ADD AX,BX                          }
/$8B/$DA                { MOV BX,DX                          }
/$5A                    { POP DX                             }
/$8B/$0E/>AHShift       { MOV CX,word([AHSHIFT])             }
/$D3/$E3                { SHL BX,CL                          }
/$03/$D3                { ADD DX,BX                          }
);
{$ENDIF}

const
 sp = Sizeof(pointer);

constructor tHugeCollection.Init;

begin
 tObject.Init;
 Limit:=aLimit;
 Delta:=aDelta;
 Count:=0;
 Items:=GlobalAlloc(gmem_moveable or gmem_nodiscard or gmem_zeroinit,Limit*sp);
 If Items=0 then
  begin
  Error(-1,0);
  exit;
  end;
end;

constructor tHugeCollection.Load;

var
 i      : integer;
 aCount : Longint;

begin
 tObject.Init;
 S.Read(Limit,Sizeof(Limit));
 S.Read(Delta,Sizeof(Delta));
 S.Read(aCount,Sizeof(aCount));
 Count:=0;
 Items:=GlobalAlloc(gmem_moveable or gmem_nodiscard or gmem_zeroinit,Limit*sp);
 If Items=0 then
  begin
  Error(-1,0);
  exit;
  end;
 For i:=0 to aCount-1 do
  begin
  AtInsert(I,GetItem(S));
  end;
end;


destructor tHugeCollection.Done;

begin
tObject.Done;
FreeAll;
Limit:=0;
Items:=GlobalFree(Items);
If Items<>0 then
 begin
 Error(-2,0);
 exit;
 end;
end;

function tHugeCollection.At;


begin
If Index>Count-1 then
 begin
 Error(coIndexError,0);
 At:=nil;
 exit;
 end;
Lock;
 At:=ppointer(Compute(base.ptr,Index))^;
UnLock;
end;

procedure tHugeCollection.Lock;

begin
 Base.ptr:=GlobalLock(Items);
 If Base.ptr=nil then
  begin
  Error(-3,0);
  exit;
  end;
end;

procedure tHugeCollection.UnLock;

begin
 GlobalUnLock(Items);
end;


{
function tHugeCollection.Compute(base.ptr,aIndex : Longint) : pointer;

var
 Result : LongType;
 Posn   : LongType;


begin
 Posn.Long:=aIndex*sp;
 Result.Lo:=Base.Lo+Posn.Lo;
 Result.Hi:=Base.Hi+(Posn.Hi*Ofs(AHIncr));
 Compute:=Result.ptr;
end;
}


procedure tHugeCollection.AtDelete;

var
 i : Longint;

begin
If (Index<0) or (Index>=Count) then
 begin
 Error(coIndexError,0);
 exit;
 end;
Lock;
 If Index<Count-1 then
 begin
 for i:=Index to Count-2 do
  begin
   Move(Compute(base.ptr,i+1)^,Compute(base.ptr,i)^,sp);
  end;
 end;
Dec(Count);
UnLock;
end;

procedure tHugeCollection.AtInsert;

var
 i : Longint;

begin
If (Index<0) or (Index>Count) then
 begin
 Error(coIndexError,0);
 exit;
 end;
If Limit=Count then
 begin
 If Delta=0 then
  begin
  Error(coOverFlow,0);
  exit;
  end;
 Inc(Limit,Delta);
 Items:=GlobalReAlloc(Items,Limit*sp,gmem_moveable or gmem_nodiscard or gmem_zeroinit);
 If Items=0 then
  begin
  Error(coOverFlow,0);
  exit;
  end;
 end;
Lock;
If Index<>Count then
 begin {Do a shuffle first}
 i:=Count-1;
 While i>=Index do
  begin
  Move(Compute(base.ptr,i)^,Compute(base.ptr,i+1)^,sp);
  Dec(i);
  end;
 end;
Move(Item,Compute(base.ptr,index)^,sp);
UnLock;
Inc(Count);
end;

procedure tHugeCollection.AtPut;

begin
 If (Index<0) or (Index>=Count) then
  begin
   Error(coIndexError,0);
   exit;
  end;
 Lock;
 Move(Item,Compute(base.ptr,index)^,sp);
 UnLock;
end;

procedure tHugeCollection.AtZero;


begin
Lock;
 If (Index<Count) and (Index>=0) then LongType(Compute(base.ptr,index)^).long:=0;
UnLock;
end;

procedure tHugeCollection.Delete;

begin
 AtDelete(Indexof(Item));
end;

procedure tHugeCollection.DeleteAll;

begin
 Count:=0;
end;

procedure tHugeCollection.Error;

begin
MessageBox(0,'There has been a HugeCollection error','tHuge Collection',mb_ok);
Halt(1);
end;

function tHugeCollection.FirstThat;

type
 tTestFunc = function(i : pointer;bp : word) : Boolean;

var
 i       : integer;
 tbp     : word;

begin
 i:=0;
 asm
  mov ax,[bp]
  and al,$FE
  mov tbp,ax
 end;
 While (i<Count) and Not (tTestFunc(Test)(At(i),tbp)) do Inc(i);
 If i<Count then FirstThat:=At(i) else FirstThat:=nil;
end;

procedure tHugeCollection.ForEach;

type
 tActionProc = procedure(i : pointer;bp : word);

var
 i     : longint;
 tbp   : word;

begin
 asm
  mov ax,[bp]
  and al,$FE
  mov tbp,ax
 end;
 For i:=0 to Count-1 do
  begin
   tActionProc(Action)(At(i),tbp);
  end;
end;

procedure tHugeCollection.Free;

begin
 Delete(Item);
 FreeItem(Item);
end;

procedure tHugeCollection.FreeAll;

var
 i : integer;

begin
for i:=0 to Count-1 do
 begin
 FreeItem(At(i));
 end;
Count:=0;
end;

procedure tHugeCollection.FreeItem;

begin
 If Item<>nil then Dispose(pObject(Item),Done);
end;

function tHugeCollection.GetItem;

begin
GetItem:=S.Get;
end;

function tHugeCollection.IndexOf;

var
 i : integer;

begin
Lock;
 i:=0;
 while (i<count) and (ppointer(Compute(base.ptr,i))^<>item) do
  begin
  inc(i);
  end;
If i=count then IndexOf:=-1 else Indexof:=i;
UnLock;
end;


procedure tHugeCollection.Insert;

begin
 AtInsert(Count,Item);
end;

function tHugeCollection.LastThat;

type
 tTestFunc = function(i : pointer;bp : word) : Boolean;

var
 i       : integer;
 tbp     : word;

begin
 i:=Count-1;
 asm
  mov ax,[bp]
  and al,$FE
  mov tbp,ax
 end;
 While (i>=0) and Not (tTestFunc(Test)(At(i),tbp)) do Dec(i);
 If i>=0 then LastThat:=At(i) else LastThat:=nil;
end;


{$IFDEF only386}

procedure lodsd; inline ($66/$AD);
procedure stosd; inline ($66/$AB);
procedure or_eax_eax; inline ($66/$0B/$C0);

{$ELSE}

procedure lodsd; inline($AD/            {LODSW}
                        $8B/$C8/        {MOV CX,AX}
                        $AD             {LODSW}
                        );

procedure stosd; inline($50/            {PUSH AX}
                        $8B/$C1/        {MOV  AX,CX}
                        $AB/            {STOSW}
                        $58/            {POP  AX}
                        $AB             {STOSW}
                       );

procedure or_eax_eax; inline($0B/$C9/   {OR CX,CX}
                             $75/$02/   {JNZ past the next compare}
                             $0B/$C0    {OR AX,AX}
                      );

{$ENDIF}

procedure tHugeCollection.Pack;

Label lp1,lp2,lp3;

var
 sCount : Longint;
 sBase  : Pointer;
 sShift : Word;
 sIncr  : Word;

begin
 Lock;
 sCount:=Count*Sizeof(pointer);
 sBase:=Base.ptr;
 sShift:=Ofs(_AHShift);
 sIncr:=Ofs(_AHIncr);     { Move variables onto stack so still }
 asm                      { available when DS has changed.     }
  push ds;                { Store DS}
  CLD                     { Clear Direction flag so that copy goes in right direction}
  LDS  SI,sBase;          { Load DS:SI and ES:DI with array base}
  LES  DI,sBase;
  MOV  DX,DS              { Load DX:BX with array base }
  MOV  BX,SI
  ADD  BX,Word(sCount);
  MOV  AX,Word(sCount)+2;
  MOV  CX,[sShift];
  SHL  AX,CL;
  ADD  DX,AX;             { Set DX:BX to point to last element in array+1 }
lp1:
 end;
  lodsd;                  { Load EAX with dword pointed to by DS:SI ; INC SI,4 }
  or_eax_eax;             { Compare EAX with Zero If zero then don't copy it }
 asm
  JZ   lp2
 end;
  stosd;                  { Store EAX at ES:DI; INC DI,4 }
 asm
  OR   di,di
  JNZ  lp2
  MOV  AX,ES
  ADD  AX,[sIncr]
  MOV  ES,AX              { Increment ES selector by right amount when neccessary}
lp2:
  MOV  AX,DS
  OR   SI,SI
  JNZ  lp3
  ADD  AX,[sIncr]
  MOV  DS,AX              { Increment DS selector by right amount when neccessary}
lp3:
  CMP  AX,DX
  JNE  lp1
  CMP  SI,BX
  JNE  lp1                  { Continue loop until DS=DX and SI=BX }
  MOV  AX,DI
  SUB  AX,word(sBase)
  MOV  word(sCount),AX
  MOV  AX,ES
  SUB  AX,word(sBase)+2
  MOV  CX,[sShift]
  SHR  AX,CL
  MOV  word(sCount)+2,AX
  pop  ds;
  end;
 Count:=sCount DIV Sizeof(pointer);
 UnLock;
end;

Procedure tHugeCollection.PutItem;

begin
 S.Put(Item);
end;

procedure tHugeCollection.SetLimit;

begin
Limit:=aLimit;
Items:=GlobalReAlloc(Items,Limit*sp,gmem_moveable or gmem_nodiscard or gmem_zeroinit);
If Items=0 then
 begin
 Error(-3,0);
 exit;
 end;
end;

procedure tHugeCollection.Store;

var
 i : integer;

begin
 S.Write(Limit,Sizeof(Limit));
 S.Write(Delta,Sizeof(Delta));
 S.Write(Count,Sizeof(Count));
For i:=0 to Count-1 do
  begin
  PutItem(S,At(i));
  end;
end;

function tHugeSortedCollection.Compare;

begin
 Abstract;
end;

function tHugeSortedCollection.IndexOf;

var
 I : longint;

begin
 if Search(KeyOf(Item),I) then IndexOf:=I else Indexof:=-1;
end;

procedure tHugeSortedCollection.Insert;

var
 I : longint;

begin
 If Count=0 then AtInsert(0,Item) else
 If not Search(Keyof(Item),I) then AtInsert(I,Item);
end;

function tHugeSortedCollection.KeyOf;

begin
 KeyOf:=Item;
end;

function tHugeSortedCollection.Search;

var
 First,Last,Middle : Longint;
 result            : integer;

begin
 First:=0;
 Last:=Count-1;
  repeat
   middle:=(first+last) div 2;
   result:=Compare(At(middle),Key);
   if result>0 then
    last:=middle-1
   else
    first:=middle+1
  until (result=0) or (first>last);
 if result=0 then
  begin
   Search:=True;
   Index:=Middle
  end else
  begin
   Search:=False;
   Index:=first;
  end;
end;

{----------------------tCharHugeCollection--------------------------}

procedure tCharHugeCollection.FreeItem;

begin
 If Item<>nil then StrDispose(pChar(Item));
end;

{----------------------tStrHugeCollection--------------------------}

procedure tStrHugeCollection.FreeItem;

begin
 If Item<>nil then StrDispose(pChar(Item));
end;

function tStrHugeCollection.Compare;

begin
 Compare:=StrComp(pChar(key1),pChar(key2));
end;


begin
 AhIncr := Ofs(_AhIncr);
 AHShift:= Ofs(_AhShift);
end.
