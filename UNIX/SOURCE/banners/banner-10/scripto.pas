program cursive;
type
  line=array[0..255]of char;
  str255=string[255];
  str80=string[80];
  fontch=array[-3..2,1..11]of char;
  fontrec=record
    width:byte;
    def:fontch;
  end;

var
  A:string[255];
  fonts:array[0..255]of fontrec;
  Inp:str80;
  Buf:array[1..6]of str255;
  Oup:array[-3..2]of line absolute Buf;

procedure loadfont;
var
  f:text;
  Ltr:byte;
  S:string[12];
  i,j,
  m:integer;
begin
  assign(f,'scripto.set');
  reset(f);
  A:='';
  fillchar(fonts,sizeof(fonts),#0);
  while not(eof(f)) do
    begin
    readln(f,S);
    A:=A+S;
    Ltr:=ord(S[1]);
    for i:=-3 to 2 do
      begin
      readln(f,S);
      if(i=0)then
        begin
        if(copy(S,length(S),1)='@')then
          delete(S,length(S),1);
        fonts[Ltr].width:=length(S);
        end;
      for j:=1 to length(S) do
        begin
        fonts[Ltr].def[i,j]:=S[j];
        end;
      end;
    end;
  {make blank 6 chars wide}
  fonts[ord(' ')].width:=6;
end; {loadfont}

procedure scripto(Inp:str80);
var
  ch:char;
  Ltr:byte;
  i,j,n,c:integer;
begin
  fillchar(Oup,sizeof(Oup),' ');
  c:=0;
  for n:=1 to length(Inp) do
    begin
    Ltr:=ord(Inp[n]);
    for i:=-3 to 2 do
      begin
      for j:=1 to 11 do
        begin
        ch:=fonts[Ltr].def[i,j];
        if(ch>' ')then
          Oup[i,c+j]:=ch;
        end;
      end;
      c:=c+fonts[Ltr].width+1;
    end;
  for i:=1 to 6 do
    begin
    Buf[i][0]:=chr(c);
    writeln(Buf[i]);
    end;
end; {scripto}

begin
  loadfont;
  readln(Inp);
  while not(Inp='') do
    begin
    scripto(Inp);
    readln(Inp);
    end;
end.
