
Date: Monday, 27 November 1989  09:11-MST
From: Leonard Campbell <UCIC309%UNLVM.BITNET@ricevm1.rice.edu>
To:   Info-IBMPC@WSMRSIMTEL20.ARMY.MIL
Re:   ^Z problem in turbo pascal

By Default Turbo pascal uses a file handle that filters out ^Z's and
posibly massages other characters (such as tabs).  I have a small program
stub that changes the file handle to process characters in 'RAW' mode.
Program follows:

PROGRAM printout; { This stub allow you to write the EOF char to }
                  { device LPT1}
USES dos;

VAR
  ExitSave: pointer;
  dosregs:dos.registers;
  Lst: FILE;

PROCEDURE init;

  VAR
    i, j: Integer;

  BEGIN
    IF ParamCount = 0 THEN BEGIN
      Writeln('NO argument given');
      Writeln('SYNTAX: cmd <outputloc>');
      Writeln('  outputloc is the destination (default PRN)');
      Writeln('  ... more desc');
      Halt; END;
    IF ParamCount > 0 THEN outn := ParamSTR(1)
    ELSE outn := 'LPT1';
    Assign(Lst, outn); {$i-}
    Rewrite(Lst, 1); {$i+}
    IF IOresult <> 0 THEN BEGIN
      Writeln('Error opening output file:', outn);
      Halt; END;
    Writeln(' Output routed to ', outn);
    with dosregs do begin   { here is where we change mode}
       ah := $44;  { get device info}
       al := $00;
       bx := filerec(lst).handle;
       msdos(dosregs);
       devinfo := dx;
       isptr := 0 <> (dx and $0080);   {if device is block char device then}
       if isptr then begin
          ah := $44;
          al := $01;
          bx := filerec(lst).handle;
          dx := devinfo or $0020;      {set device to RAW mode}
          dh := 0;
          msdos(dosregs); end; end;
     isopen := true;
  END {init};

{$F+}
procedure ExitHandler;
  { Return the ptr to chr device }
  begin
   if isopen then begin
       if isptr then with dosregs do begin
          ah := $44; { set device info}
          al := $01;
          bx := filerec(lst).handle;
          dx := devinfo;
          dh := 0;
          msdos(dosregs); end;
       close(lst);  end;
  ExitProc := ExitSave
  end;
{$F-}

BEGIN {listfont}
  isopen := false;
   ExitSave := ExitProc;
   ExitProc := @ExitHandler;
  init;
  {do your stuf here}
  {I.E.  for i := 0 to 255 do write(lst,chr(i)); }

END.{printout}.
