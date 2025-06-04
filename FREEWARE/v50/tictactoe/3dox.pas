{ Copyright 1982,2000   Bruce Baker
                   11 Streetly Close
                   Tullamarine
                   Victoria
                   Australia 3043
                   bjbaker@technologist.com

 Originally written for a Western Digital Pascal MicroEngine.
 The first (1700 line) version was written over a day and a half.  
 So I can blame the code quality on it being quick and dirty :-)

 Adapted for DEC Pascal and VT control strings in October 2000.
 
 Do what you like with this program so long as you don't plan to make money
 from it.  If money is involved, I want my share ;-)

 A possible development of this game is extension to 4 dimensions.  I
 considered that in 1982, but I was limited to a 80x24 Hazeltine terminal.  
 That limitation can be overcome on a DECterm. The calculation of the necessary
 magic numbers (see the SETSTYLE routine) for the 5x5x5x5 version would be
 somewhat headache inducing.  Determining the logic behind the magic numbers
 in this version is left as a puzzle for the reader (no prizes, as I don't 
 remember the details).  I did produce a modified version of this game that
 played itself with two different sets of numbers, trying to find the best set.
 It turned out to be a circular argument, so the numbers here are as good as
 they need to be.  

 The algorithm for the computer's moves is simplistic, with no strategy, just 
 a brute force analysis of the situation for every move.  Given how well it 
 plays, I think anything more would be overkill.

 }

program noughts_and_crosses(input,output);
const
     interrow  =
 '+---+---+---+---+ +---+---+---+---+ +---+---+---+---+ +---+---+---+---+';
     row       =
 '|   |   |   |   | |   |   |   |   | |   |   |   |   | |   |   |   |   |';
     prompt    =
 '         Grid  :             Row  :            Column  :';


type
     dimension = 1..4;

var
     cmd       : char;
     enough    : boolean;
     box       : packed array[dimension,dimension,dimension] of char;
     eog       : boolean;
     attack3   : integer;
     attack2   : integer;
     attack1   : integer;
     block3    : integer;
     block2    : integer;
     block1    : integer;
     nicespot  : integer;

{ The next three routines were the ones to be modified for VT control }

procedure gotoxy (x,y : integer);
begin	{gotoxy}
	write(chr(27),'[');
	if (y < 10) then
		write(y:1,';')
	else
		write(y:2,';');
	if (x < 10) then
		write(x:1,'H')
	else
		write(x:2,'H');
end;	{gotoxy}

procedure cleareol;
begin	{cleareol}
	write(chr(27),'[K');
end;	{cleareol}

procedure cleareos;
begin	{cleareos}
	write(chr(27),'[J');
end;	{cleareos}

procedure rollyourown;

procedure skipstr ( str : string ;
                var loc : integer );
var
        skip : boolean;
        len  : integer;
begin   {skipstr}
        skip := true; len := length(str);
        while (loc <= len) and skip do begin
                skip := str[loc] = ' ';
                if skip then loc := loc + 1;
        end;
end;    {skipstr}

function s2int ( str : string ;
             var val : integer ) : boolean;
var
        i, sign, ndigits, len : integer;
        more : boolean;
begin   {s2int}
        sign := +1;  ndigits := 0;  val := 0;
        len := length(str);
        i := 1;
        skipstr(str,i);
        if (i < len) then
                case (str[i]) of
                '+': begin sign := +1; i := i + 1 end;
                '-': begin sign := -1; i := i + 1 end;
                end;
        skipstr(str,i);
        more := true;
        while (i <= len) and more do begin
                more := str[i] in ['0'..'9'];
                if (more) then begin
                        val := val * 10 + (ord(str[i]) - ord('0'));
                        ndigits := ndigits + 1;
                        i := i + 1;
                end;
        end;
        val := val * sign;
        s2int := ndigits > 0;
end;    {s2int}

function vgetint ( x,y    : integer;
                   prompt : string ) : integer;
var
        num : string (80);
        val : integer;
        xpos : integer;
begin   {vgetint}
        xpos := length(prompt) + x;
        gotoxy(x,y);
        write(prompt);
        repeat
                gotoxy(xpos,y);
                cleareol;
                readln(num);
        until s2int(num,val);
        vgetint := val;
end;    {vgetint}

begin   {rollyourown}
        nicespot := vgetint(10,13,'Loading for advantageous cubes : ');
        block1   := vgetint(10,14,'Loading for blocking   1 cube  : ');
        attack1  := vgetint(10,15,'Loading for improving  1 cube  : ');
        block2   := vgetint(10,16,'Loading for blocking   2 cubes : ');
        attack2  := vgetint(10,17,'Loading for improving  2 cubes : ');
        block3   := vgetint(10,18,'Loading for blocking   3 cubes : ');
        attack3  := vgetint(10,19,'Loading for improving  3 cubes : ');
end;    {rollyourown}

procedure setstyle;
begin   {setstyle}
        gotoxy(0,0);
	cleareos;
        writeln;
        writeln('             3D Noughts and Crosses');
        writeln;
        write('  What style would you ');
        writeln('like the computer to play?');
        writeln('                  1/ Myopic');
        writeln('                  2/ Defensive');
        writeln('                  3/ Attacking');
        writeln('                  4/ Pretty smart');
        writeln('                  5/ Truly Nasty');
        writeln('                  6/ Set the style constants');
	gotoxy(0,11);
        write('                Enter the number ');
        repeat
                gotoxy(35,11);
                read(cmd);
        until cmd in ['1'..'6'];
        case cmd of
        '1'     : begin
                        attack3 := 16;
                        attack2 := 0;
                        attack1 := 0;
                        block3  := 2;
                        block2  := 0;
                        block1  := 0;
                        nicespot:= 1;
                  end;
        '2'     : begin
                        attack3 := 467;
                        attack2 := 0;
                        attack1 := 0;
                        block3  := 65;
                        block2  := 9;
                        block1  := 1;
                        nicespot:= 1;
                  end;
        '3'     : begin
                        attack3 := 467;
                        attack2 := 9;
                        attack1 := 1;
                        block3  := 65;
                        block2  := 0;
                        block1  := 0;
                        nicespot:= 1;
                  end;
        '4'     : begin
                        attack3 := 467;
                        attack2 := 9;
                        attack1 := 0;
                        block3  := 65;
                        block2  := 2;
                        block1  := 0;
                        nicespot:= 1;
                  end;
         '5'    : begin
                        nicespot:= 1;
                        block1  := 1;
                        attack1 := 2;
                        block2  := 8;
                        attack2 := 40;
                        block3  := 282;
                        attack3 := 1976;
                  end;
        '6'     : rollyourown;
        end;
end;    {setstyle}

procedure initialise;
var
     i,j,k     : dimension;
begin     {initialise}
     enough := false;
     gotoxy(0,0);
     cleareos;
     writeln;
     writeln('                     3D Noughts and Crosses');
     writeln;
     writeln;
     writeln('  ',interrow);
     writeln('  ',row);
     writeln('  ',interrow);
     writeln('  ',row);
     writeln('  ',interrow);
     writeln('  ',row);
     writeln('  ',interrow);
     writeln('  ',row);
     writeln('  ',interrow);
     writeln;
     writeln;
     writeln(' ',prompt);
     for i := 1 to 4 do
          for j := 1 to 4 do
               for k := 1 to 4 do
                    box[i,j,k] := ' ';
end;      {initialise}

procedure plotmove(i,j,k:dimension;mark:char);
var  x,y  : integer;
begin     {plotmove}
     case i of
     1    : x := 3;
     2    : x := 21;
     3    : x := 39;
     4    : x := 57;
     end;
     case j of
     1    : y := 5;
     2    : y := 7;
     3    : y := 9;
     4    : y := 11;
     end;
     case k of
     1    : x := x + 2;
     2    : x := x + 6;
     3    : x := x + 10;
     4    : x := x + 14;
     end;
     gotoxy(x,y);
     write(mark);
end;      {plotmove}

procedure checkwin(who:char);
var
     l         : integer;

procedure checkxplanes(who:char);
var
     x,y,z     : dimension;
     count     : integer;
begin     {checkxplanes}
     for x := 1 to 4 do begin
          for y := 1 to 4 do begin
               count := 0;
               for z := 1 to 4 do
                    if box[x,y,z] = who then
                         count := count + 1;
               if count = 4 then
                    eog := true;
          end;
          if not eog then
               if ((box[x,1,1] = who) and (box[x,2,2] = who) and
                   (box[x,3,3] = who) and (box[x,4,4] = who)) or
                  ((box[x,4,1] = who) and (box[x,3,2] = who) and
                   (box[x,2,3] = who) and (box[x,1,4] = who)) then
                    eog := true;
     end;
end;      {checkxplanes}

procedure checkyplanes(who:char);
var
     x,y,z     : dimension;
     count     : integer;
begin     {checkyplanes}
     for y := 1 to 4 do begin
          for z := 1 to 4 do begin
               count := 0;
               for x := 1 to 4 do
                    if box[x,y,z] = who then
                         count := count + 1;
               if count = 4 then
                    eog := true;
          end;
          if not eog then
               if ((box[1,y,1] = who) and (box[2,y,2] = who) and
                   (box[3,y,3] = who) and (box[4,y,4] = who)) or
                  ((box[1,y,4] = who) and (box[2,y,3] = who) and
                   (box[3,y,2] = who) and (box[4,y,1] = who)) then
                    eog := true;
     end;
end;      {checkyplanes}

procedure checkzplanes(who:char);
var
     x,y,z     : dimension;
     count     : integer;
begin     {checkzplanes}
     for z := 1 to 4 do begin
          for x := 1 to 4 do begin
               count := 0;
               for y := 1 to 4 do
                    if box[x,y,z] = who then
                         count := count + 1;
               if count = 4 then
                    eog := true;
          end;
          if not eog then
               if ((box[1,1,z] = who) and (box[2,2,z] = who) and
                   (box[3,3,z] = who) and (box[4,4,z] = who)) or
                  ((box[1,4,z] = who) and (box[2,3,z] = who) and
                   (box[3,2,z] = who) and (box[4,1,z] = who)) then
                    eog := true;
     end;
end;      {checkzplanes}

procedure checkdiagonals(who:char);
begin     {checkdiagonals}
     if (box[1,1,1] = who) and (box[2,2,2] = who) and
        (box[3,3,3] = who) and (box[4,4,4] = who) then
          eog := true
     else if (box[1,1,4] = who) and (box[2,2,3] = who) and
             (box[3,3,2] = who) and (box[4,4,1] = who) then
               eog := true
     else if (box[1,4,1] = who) and (box[2,3,2] = who) and
             (box[3,2,3] = who) and (box[4,1,4] = who) then
               eog := true
     else if (box[1,4,4] = who) and (box[2,3,3] = who) and
             (box[3,2,2] = who) and (box[4,1,1] = who) then
               eog := true;
end;      {checkdiagonals}

begin     {checkwin}
     checkxplanes(who);
     if not eog then
          checkyplanes(who);
     if not eog then
          checkzplanes(who);
     if not eog then
          checkdiagonals(who);
     l := 0;
     if eog then begin
          gotoxy(3,17);
          repeat
               write(who,' WINS   ');
               l := l + 9;
          until l + 9 > 64;
     end;
end;      {checkwin}

procedure yourmove;
var
     grid,
     row,
     column    : dimension;

function val(x:char):dimension;
begin     {val}
     case x of
     '1'  : val := 1;
     '2'  : val := 2;
     '3'  : val := 3;
     '4'  : val := 4;
     end;
end;      {val}

function freespot(i,j,k:dimension):boolean;
begin     {freespot}
     if box[i,j,k] = ' ' then begin
          freespot := true;
          box[i,j,k] := 'X';
     end
     else begin
          freespot := false;
          gotoxy(17,17);
          write('That position is not available');
     end;
end;      {freespot}

begin     {yourmove}
     repeat
          repeat
               gotoxy(19,15);
               read(cmd);
               gotoxy(0,17);
               cleareol;
          until cmd in ['1'..'4'];
          grid := val(cmd);
          repeat
               gotoxy(38,15);
               read(cmd);
          until cmd in ['1'..'4'];
          row := val(cmd);
          repeat
               gotoxy(59,15);
               read(cmd);
          until cmd in ['1'..'4'];
          column := val(cmd);
     until freespot(grid,row,column);
     plotmove(grid,row,column,'X');
     checkwin('X');
end;      {yourmove}

procedure mymove;
type
     range      = 1..64;
var
     index      : range;
     choice     : array[range] of record
                        grid,row,col : dimension;
                  end;
     i,j,k      : dimension;
     list       : array[dimension,dimension,dimension] of integer;

procedure assignloading;
var
     counto,
     countx    : 0..4;
     i,j,k     : 0..8;
     sequence  : integer;
     incr      : integer;

procedure makestr(character:char);
begin   {makestr}
        case character of
        'X'     : sequence:=sequence*3;
        'O'     : sequence:=sequence*3+1;
        ' '     : sequence:=sequence*3+2;
        end;
end;    {makestr}

procedure scanxplanes;

procedure scan1xdiag;

procedure case1x1;
begin   {case1x1}
     case sequence of
     2         : list[i,4,4] := list[i,4,4] + block3;
     6         : list[i,3,3] := list[i,3,3] + block3;
     18        : list[i,2,2] := list[i,2,2] + block3;
     8         : begin
                    list[i,4,4] := list[i,4,4] + block2;
                    list[i,3,3] := list[i,3,3] + block2;
                 end;
     24        : begin
                    list[i,3,3] := list[i,3,3] + block2;
                    list[i,2,2] := list[i,2,2] + block2;
                 end;
     20        : begin
                    list[i,2,2] := list[i,2,2] + block2;
                    list[i,4,4] := list[i,4,4] + block2;
                 end;
     26        : begin
                    list[i,2,2] := list[i,2,2] + block1;
                    list[i,3,3] := list[i,3,3] + block1;
                    list[i,4,4] := list[i,4,4] + block1;
                 end;
     end;
end;    {case1x2}

procedure case1x2;
begin   {case1x2}
     case sequence of
     41        : list[i,4,4] := list[i,4,4] + attack3;
     43        : list[i,3,3] := list[i,3,3] + attack3;
     49        : list[i,2,2] := list[i,2,2] + attack3;
     44        : begin
                    list[i,4,4] := list[i,4,4] + attack2;
                    list[i,3,3] := list[i,3,3] + attack2;
                 end;
     52        : begin
                    list[i,3,3] := list[i,3,3] + attack2;
                    list[i,2,2] := list[i,2,2] + attack2;
                 end;
     50        : begin
                    list[i,2,2] := list[i,2,2] + attack2;
                    list[i,4,4] := list[i,4,4] + attack2;
                 end;
     53        : begin
                    list[i,2,2] := list[i,2,2] + attack1;
                    list[i,3,3] := list[i,3,3] + attack1;
                    list[i,4,4] := list[i,4,4] + attack1;
                 end;
     end;
end;    {case1x2}

procedure case1x3a;
begin   {case1x3a}
     case sequence of
     54        : list[i,1,1] := list[i,1,1] + block3;
     67        : list[i,1,1] := list[i,1,1] + attack3;
     60        : begin
                    list[i,1,1] := list[i,1,1] + block2;
                    list[i,3,3] := list[i,3,3] + block2;
                 end;
     56        : begin
                    list[i,1,1] := list[i,1,1] + block2;
                    list[i,4,4] := list[i,4,4] + block2;
                 end;
     70        : begin
                    list[i,1,1] := list[i,1,1] + attack2;
                    list[i,3,3] := list[i,3,3] + attack2;
                 end;
     68        : begin
                    list[i,1,1] := list[i,1,1] + attack2;
                    list[i,4,4] := list[i,4,4] + attack2;
                 end;
     62        : begin
                    list[i,1,1] := list[i,1,1] + block1;
                    list[i,3,3] := list[i,3,3] + block1;
                    list[i,4,4] := list[i,4,4] + block1;
                 end;
     71        : begin
                    list[i,1,1] := list[i,1,1] + attack1;
                    list[i,3,3] := list[i,3,3] + attack1;
                    list[i,4,4] := list[i,4,4] + attack1;
                 end;
     end;
end;    {case1x3a}

procedure case1x3b;
begin   {case1x3b}
     case sequence of
     72        : begin
                    list[i,2,2] := list[i,2,2] + block2;
                    list[i,1,1] := list[i,1,1] + block2;
                 end;
     76        : begin
                    list[i,2,2] := list[i,2,2] + attack2;
                    list[i,1,1] := list[i,1,1] + attack2;
                 end;
     74        : begin
                    list[i,1,1] := list[i,1,1] + block1;
                    list[i,2,2] := list[i,2,2] + block1;
                    list[i,4,4] := list[i,4,4] + block1;
                 end;
     78        : begin
                    list[i,1,1] := list[i,1,1] + block1;
                    list[i,2,2] := list[i,2,2] + block1;
                    list[i,3,3] := list[i,3,3] + block1;
                 end;
     77        : begin
                    list[i,1,1] := list[i,1,1] + attack1;
                    list[i,2,2] := list[i,2,2] + attack1;
                    list[i,4,4] := list[i,4,4] + attack1;
                 end;
     79        : begin
                    list[i,1,1] := list[i,1,1] + attack1;
                    list[i,2,2] := list[i,2,2] + attack1;
                    list[i,3,3] := list[i,3,3] + attack1;
                 end;
     end;
end;    {case1x3b}

begin     {scan1xdiag};
     sequence := 0;
     makestr(box[i,1,1]);
     makestr(box[i,2,2]);
     makestr(box[i,3,3]);
     makestr(box[i,4,4]);
     if sequence < 40 then
        case1x1
     else if sequence < 54 then
        case1x2
     else if sequence < 72 then
        case1x3a
     else
        case1x3b;
end;    {scan1xdiag}

procedure scan2xdiag;

procedure case2x1;
begin   {case2x1}
     case sequence of
     2         : list[i,4,1] := list[i,4,1] + block3;
     6         : list[i,3,2] := list[i,3,2] + block3;
     18        : list[i,2,3] := list[i,2,3] + block3;
     8         : begin
                    list[i,4,1] := list[i,4,1] + block2;
                    list[i,3,2] := list[i,3,2] + block2;
                 end;
     24        : begin
                    list[i,3,2] := list[i,3,2] + block2;
                    list[i,2,3] := list[i,2,3] + block2;
                 end;
     20        : begin
                    list[i,2,3] := list[i,2,3] + block2;
                    list[i,4,1] := list[i,4,1] + block2;
                 end;
     26        : begin
                    list[i,2,3] := list[i,2,3] + block1;
                    list[i,3,2] := list[i,3,2] + block1;
                    list[i,4,1] := list[i,4,1] + block1;
                 end;
     end;
end;    {case2x1}

procedure case2x2;
begin   {case2x2}
     case sequence of
     41        : list[i,4,1] := list[i,4,1] + attack3;
     43        : list[i,3,2] := list[i,3,2] + attack3;
     49        : list[i,2,3] := list[i,2,3] + attack3;
     44        : begin
                    list[i,4,1] := list[i,4,1] + attack2;
                    list[i,3,2] := list[i,3,2] + attack2;
                 end;
     52        : begin
                    list[i,3,2] := list[i,3,2] + attack2;
                    list[i,2,3] := list[i,2,3] + attack2;
                 end;
     50        : begin
                    list[i,2,3] := list[i,2,3] + attack2;
                    list[i,4,1] := list[i,4,1] + attack2;
                 end;
     53        : begin
                    list[i,2,3] := list[i,2,3] + attack1;
                    list[i,3,2] := list[i,3,2] + attack1;
                    list[i,4,1] := list[i,4,1] + attack1;
                 end;
     end;
end;    {case2x2}

procedure case2x3a;
begin   {case2x3a}
     case sequence of
     54        : list[i,1,4] := list[i,1,4] + block3;
     67        : list[i,1,4] := list[i,1,4] + attack3;
     60        : begin
                    list[i,1,4] := list[i,1,4] + block2;
                    list[i,3,2] := list[i,3,2] + block2;
                 end;
     56        : begin
                    list[i,1,4] := list[i,1,4] + block2;
                    list[i,4,1] := list[i,4,1] + block2;
                 end;
     70        : begin
                    list[i,1,4] := list[i,1,4] + attack2;
                    list[i,3,2] := list[i,3,2] + attack2;
                 end;
     68        : begin
                    list[i,1,4] := list[i,1,4] + attack2;
                    list[i,4,1] := list[i,4,1] + attack2;
                 end;
     62        : begin
                    list[i,1,4] := list[i,1,4] + block1;
                    list[i,3,2] := list[i,3,2] + block1;
                    list[i,4,1] := list[i,4,1] + block1;
                 end;
     71        : begin
                    list[i,1,4] := list[i,1,4] + attack1;
                    list[i,3,2] := list[i,3,2] + attack1;
                    list[i,4,1] := list[i,4,1] + attack1;
                 end;
     end;
end;    {case2x3a}

procedure case2x3b;
begin   {case2x3b}
     case sequence of
     72        : begin
                    list[i,2,3] := list[i,2,3] + block2;
                    list[i,1,4] := list[i,1,4] + block2;
                 end;
     76        : begin
                    list[i,2,3] := list[i,2,3] + attack2;
                    list[i,1,4] := list[i,1,4] + attack2;
                 end;
     74        : begin
                    list[i,1,4] := list[i,1,4] + block1;
                    list[i,2,3] := list[i,2,3] + block1;
                    list[i,4,1] := list[i,4,1] + block1;
                 end;
     78        : begin
                    list[i,1,4] := list[i,1,4] + block1;
                    list[i,2,3] := list[i,2,3] + block1;
                    list[i,3,2] := list[i,3,2] + block1;
                 end;
     77        : begin
                    list[i,1,4] := list[i,1,4] + attack1;
                    list[i,2,3] := list[i,2,3] + attack1;
                    list[i,4,1] := list[i,4,1] + attack1;
                 end;
     79        : begin
                    list[i,1,4] := list[i,1,4] + attack1;
                    list[i,2,3] := list[i,2,3] + attack1;
                    list[i,3,2] := list[i,3,2] + attack1;
                 end;
     end;
end;    {case2x3b}

begin   {scan2xdiag}
     sequence := 0;
     makestr(box[i,1,4]);
     makestr(box[i,2,3]);
     makestr(box[i,3,2]);
     makestr(box[i,4,1]);
     if sequence < 40 then
        case2x1
     else if sequence < 54 then
        case2x2
     else if sequence < 72 then
        case2x3a
     else
        case2x3b;
end;      {scan2xdiag}

begin     {scanxplanes}
     for i := 1 to 4 do begin
          for j := 1 to 4 do begin
               countx := 0;
               counto := 0;
               incr   := 0;
               for k := 1 to 4 do begin
                    if box[i,j,k] = 'X' then
                         countx := countx + 1;
                    if box[i,j,k] = 'O' then
                         counto := counto + 1;
               end;
               if countx = 3 then
                    incr := block3;
               if counto = 3 then
                    incr := attack3;
               if (countx = 2) and (counto = 0) then
                    incr := block2;
               if (countx = 0) and (counto = 2) then
                    incr := attack2;
               if (countx = 1) and (counto = 0) then
                    incr := block1;
               if (countx = 0) and (counto = 1) then
                    incr := attack1;
               if incr > 0 then
                    for k := 1 to 4 do
                         if box[i,j,k] = ' ' then
                              list[i,j,k] := list[i,j,k] + incr;
          end;
          scan1xdiag;
          scan2xdiag;
     end;
end;      {scanxplanes}

procedure scanyplanes;

procedure scan1ydiag;

procedure case1y1;
begin   {case1y1}
        case sequence of
        2         : list[4,j,4] := list[4,j,4] + block3;
        6         : list[3,j,3] := list[3,j,3] + block3;
        18        : list[2,j,2] := list[2,j,2] + block3;
        8         : begin
                       list[4,j,4] := list[4,j,4] + block2;
                       list[3,j,3] := list[3,j,3] + block2;
                    end;
        24        : begin
                       list[3,j,3] := list[3,j,3] + block2;
                       list[2,j,2] := list[2,j,2] + block2;
                    end;
        20        : begin
                       list[2,j,2] := list[2,j,2] + block2;
                       list[4,j,4] := list[4,j,4] + block2;
                    end;
        26        : begin
                       list[2,j,2] := list[2,j,2] + block1;
                       list[3,j,3] := list[3,j,3] + block1;
                       list[4,j,4] := list[4,j,4] + block1;
                    end;
        end;
end;    {case1y1}


procedure case1y2;
begin   {case1y2}
        case sequence of
        41        : list[4,j,4] := list[4,j,4] + attack3;
        43        : list[3,j,3] := list[3,j,3] + attack3;
        49        : list[2,j,2] := list[2,j,2] + attack3;
        44        : begin
                       list[4,j,4] := list[4,j,4] + attack2;
                       list[3,j,3] := list[3,j,3] + attack2;
                    end;
        52        : begin
                       list[3,j,3] := list[3,j,3] + attack2;
                       list[2,j,2] := list[2,j,2] + attack2;
                    end;
        50        : begin
                       list[2,j,2] := list[2,j,2] + attack2;
                       list[4,j,4] := list[4,j,4] + attack2;
                    end;
        53        : begin
                       list[2,j,2] := list[2,j,2] + attack1;
                       list[3,j,3] := list[3,j,3] + attack1;
                       list[4,j,4] := list[4,j,4] + attack1;
                    end;
        end;
end;    {case1y2}


procedure case1y3a;
begin   {case1y3a}
        case sequence of
        54        : list[1,j,1] := list[1,j,1] + block3;
        67        : list[1,j,1] := list[1,j,1] + attack3;
        60        : begin
                       list[1,j,1] := list[1,j,1] + block2;
                       list[3,j,3] := list[3,j,3] + block2;
                    end;
        56        : begin
                       list[1,j,1] := list[1,j,1] + block2;
                       list[4,j,4] := list[4,j,4] + block2;
                    end;
        70        : begin
                       list[1,j,1] := list[1,j,1] + attack2;
                       list[3,j,3] := list[3,j,3] + attack2;
                    end;
        68        : begin
                       list[1,j,1] := list[1,j,1] + attack2;
                       list[4,j,4] := list[4,j,4] + attack2;
                    end;
        62        : begin
                       list[1,j,1] := list[1,j,1] + block1;
                       list[3,j,3] := list[3,j,3] + block1;
                       list[4,j,4] := list[4,j,4] + block1;
                    end;
        71        : begin
                       list[1,j,1] := list[1,j,1] + attack1;
                       list[3,j,3] := list[3,j,3] + attack1;
                       list[4,j,4] := list[4,j,4] + attack1;
                    end;
        end;
end;    {case1y3a}

procedure case1y3b;
begin   {case1y3b}
        case sequence of
        72        : begin
                       list[2,j,2] := list[2,j,2] + block2;
                       list[1,j,1] := list[1,j,1] + block2;
                    end;
        76        : begin
                       list[2,j,2] := list[2,j,2] + attack2;
                       list[1,j,1] := list[1,j,1] + attack2;
                    end;
        74        : begin
                       list[1,j,1] := list[1,j,1] + block1;
                       list[2,j,2] := list[2,j,2] + block1;
                       list[4,j,4] := list[4,j,4] + block1;
                    end;
        78        : begin
                       list[1,j,1] := list[1,j,1] + block1;
                       list[2,j,2] := list[2,j,2] + block1;
                       list[3,j,3] := list[3,j,3] + block1;
                    end;
        77        : begin
                       list[1,j,1] := list[1,j,1] + attack1;
                       list[2,j,2] := list[2,j,2] + attack1;
                       list[4,j,4] := list[4,j,4] + attack1;
                    end;
        79        : begin
                       list[1,j,1] := list[1,j,1] + attack1;
                       list[2,j,2] := list[2,j,2] + attack1;
                       list[3,j,3] := list[3,j,3] + attack1;
                    end;
        end;
end;    {case1y3b}

begin     {scan1ydiag};
     sequence := 0;
     makestr(box[1,j,1]);
     makestr(box[2,j,2]);
     makestr(box[3,j,3]);
     makestr(box[4,j,4]);
     if sequence < 40 then
        case1y1
     else if sequence < 54 then
        case1y2
     else if sequence < 72 then
        case1y3a
     else
        case1y3b;
end;    {scan1ydiag}

procedure scan2ydiag;

procedure case2y1;
begin   {case2y1}
     case sequence of
     2         : list[4,j,1] := list[4,j,1] + block3;
     6         : list[3,j,2] := list[3,j,2] + block3;
     18        : list[2,j,3] := list[2,j,3] + block3;
     8         : begin
                    list[4,j,1] := list[4,j,1] + block2;
                    list[3,j,2] := list[3,j,2] + block2;
                 end;
     24        : begin
                    list[3,j,2] := list[3,j,2] + block2;
                    list[2,j,3] := list[2,j,3] + block2;
                 end;
     20        : begin
                    list[2,j,3] := list[2,j,3] + block2;
                    list[4,j,1] := list[4,j,1] + block2;
                 end;
     26        : begin
                    list[2,j,3] := list[2,j,3] + block1;
                    list[3,j,2] := list[3,j,2] + block1;
                    list[4,j,1] := list[4,j,1] + block1;
                 end;
     end;
end;    {case2y1}

procedure case2y2;
begin   {case2y2}
     case sequence of
     41        : list[4,j,1] := list[4,j,1] + attack3;
     43        : list[3,j,2] := list[3,j,2] + attack3;
     49        : list[2,j,3] := list[2,j,3] + attack3;
     44        : begin
                    list[4,j,1] := list[4,j,1] + attack2;
                    list[3,j,2] := list[3,j,2] + attack2;
                 end;
     52        : begin
                    list[3,j,2] := list[3,j,2] + attack2;
                    list[2,j,3] := list[2,j,3] + attack2;
                 end;
     50        : begin
                    list[2,j,3] := list[2,j,3] + attack2;
                    list[4,j,1] := list[4,j,1] + attack2;
                 end;
     53        : begin
                    list[2,j,3] := list[2,j,3] + attack1;
                    list[3,j,2] := list[3,j,2] + attack1;
                    list[4,j,1] := list[4,j,1] + attack1;
                 end;
     end;
end;    {case2y2}

procedure case2y3a;
begin   {case2y3a}
     case sequence of
     54        : list[1,j,4] := list[1,j,4] + block3;
     67        : list[1,j,4] := list[1,j,4] + attack3;
     60        : begin
                    list[1,j,4] := list[1,j,4] + block2;
                    list[3,j,2] := list[3,j,2] + block2;
                 end;
     56        : begin
                    list[1,j,4] := list[1,j,4] + block2;
                    list[4,j,1] := list[4,j,1] + block2;
                 end;
     70        : begin
                    list[1,j,4] := list[1,j,4] + attack2;
                    list[3,j,2] := list[3,j,2] + attack2;
                 end;
     68        : begin
                    list[1,j,4] := list[1,j,4] + attack2;
                    list[4,j,1] := list[4,j,1] + attack2;
                 end;
     62        : begin
                    list[1,j,4] := list[1,j,4] + block1;
                    list[3,j,2] := list[3,j,2] + block1;
                    list[4,j,1] := list[4,j,1] + block1;
                 end;
     71        : begin
                    list[1,j,4] := list[1,j,4] + attack1;
                    list[3,j,2] := list[3,j,2] + attack1;
                    list[4,j,1] := list[4,j,1] + attack1;
                 end;
     end;
end;    {case2y3a}

procedure case2y3b;
begin   {case2y3b}
     case sequence of
     72        : begin
                    list[2,j,3] := list[2,j,3] + block2;
                    list[1,j,4] := list[1,j,4] + block2;
                 end;
     76        : begin
                    list[2,j,3] := list[2,j,3] + attack2;
                    list[1,j,4] := list[1,j,4] + attack2;
                 end;
     74        : begin
                    list[1,j,4] := list[1,j,4] + block1;
                    list[2,j,3] := list[2,j,3] + block1;
                    list[4,j,1] := list[4,j,1] + block1;
                 end;
     78        : begin
                    list[1,j,4] := list[1,j,4] + block1;
                    list[2,j,3] := list[2,j,3] + block1;
                    list[3,j,2] := list[3,j,2] + block1;
                 end;
     77        : begin
                    list[1,j,4] := list[1,j,4] + attack1;
                    list[2,j,3] := list[2,j,3] + attack1;
                    list[4,j,1] := list[4,j,1] + attack1;
                 end;
     79        : begin
                    list[1,j,4] := list[1,j,4] + attack1;
                    list[2,j,3] := list[2,j,3] + attack1;
                    list[3,j,2] := list[3,j,2] + attack1;
                 end;
     end;
end;    {case2y3b}

begin   {scan2ydiag}
     sequence := 0;
     makestr(box[1,j,4]);
     makestr(box[2,j,3]);
     makestr(box[3,j,2]);
     makestr(box[4,j,1]);
     if sequence < 40 then
        case2y1
     else if sequence < 54 then
        case2y2
     else if sequence < 72 then
        case2y3a
     else
        case2y3b;
end;      {scan2ydiag}

begin     {scanyplanes}
     for j := 1 to 4 do begin
          for k := 1 to 4 do begin
               countx := 0;
               counto := 0;
               incr   := 0;
               for i := 1 to 4 do begin
                    if box[i,j,k] = 'X' then
                         countx := countx + 1;
                    if box[i,j,k] = 'O' then
                         counto := counto + 1;
               end;
               if countx = 3 then
                    incr := block3;
               if counto = 3 then
                    incr := attack3;
               if (countx = 2) and (counto = 0) then
                    incr := block2;
               if (countx = 0) and (counto = 2) then
                    incr := attack2;
               if (countx = 1) and (counto = 0) then
                    incr := block1;
               if (countx = 0) and (counto = 1) then
                    incr := attack1;
               if incr > 0 then
                    for i := 1 to 4 do
                         if box[i,j,k] = ' ' then
                              list[i,j,k] := list[i,j,k] + incr;
          end;
          scan1ydiag;
          scan2ydiag;
     end;
end;      {scanyplanes}

procedure scanzplanes;

procedure scan1zdiag;

procedure case1z1;
begin   {case1z1}
     case sequence of
     2         : list[4,4,k] := list[4,4,k] + block3;
     6         : list[3,3,k] := list[3,3,k] + block3;
     18        : list[2,2,k] := list[2,2,k] + block3;
     8         : begin
                    list[4,4,k] := list[4,4,k] + block2;
                    list[3,3,k] := list[3,3,k] + block2;
                 end;
     24        : begin
                    list[3,3,k] := list[3,3,k] + block2;
                    list[2,2,k] := list[2,2,k] + block2;
                 end;
     20        : begin
                    list[2,2,k] := list[2,2,k] + block2;
                    list[4,4,k] := list[4,4,k] + block2;
                 end;
     26        : begin
                    list[2,2,k] := list[2,2,k] + block1;
                    list[3,3,k] := list[3,3,k] + block1;
                    list[4,4,k] := list[4,4,k] + block1;
                 end;
     end;
end;    {case1z1}

procedure case1z2;
begin   {case1z2}
     case sequence of
     41        : list[4,4,k] := list[4,4,k] + attack3;
     43        : list[3,3,k] := list[3,3,k] + attack3;
     49        : list[2,2,k] := list[2,2,k] + attack3;
     44        : begin
                    list[4,4,k] := list[4,4,k] + attack2;
                    list[3,3,k] := list[3,3,k] + attack2;
                 end;
     52        : begin
                    list[3,3,k] := list[3,3,k] + attack2;
                    list[2,2,k] := list[2,2,k] + attack2;
                 end;
     50        : begin
                    list[2,2,k] := list[2,2,k] + attack2;
                    list[4,4,k] := list[4,4,k] + attack2;
                 end;
     53        : begin
                    list[2,2,k] := list[2,2,k] + attack1;
                    list[3,3,k] := list[3,3,k] + attack1;
                    list[4,4,k] := list[4,4,k] + attack1;
                 end;
     end;
end;    {case1z2}

procedure case1z3a;
begin   {case1z3a}
     case sequence of
     54        : list[1,1,k] := list[1,1,k] + block3;
     67        : list[1,1,k] := list[1,1,k] + attack3;
     60        : begin
                    list[1,1,k] := list[1,1,k] + block2;
                    list[3,3,k] := list[3,3,k] + block2;
                 end;
     56        : begin
                    list[1,1,k] := list[1,1,k] + block2;
                    list[4,4,k] := list[4,4,k] + block2;
                 end;
     70        : begin
                    list[1,1,k] := list[1,1,k] + attack2;
                    list[3,3,k] := list[3,3,k] + attack2;
                 end;
     68        : begin
                    list[1,1,k] := list[1,1,k] + attack2;
                    list[4,4,k] := list[4,4,k] + attack2;
                 end;
     62        : begin
                    list[1,1,k] := list[1,1,k] + block1;
                    list[3,3,k] := list[3,3,k] + block1;
                    list[4,4,k] := list[4,4,k] + block1;
                 end;
     71        : begin
                    list[1,1,k] := list[1,1,k] + attack1;
                    list[3,3,k] := list[3,3,k] + attack1;
                    list[4,4,k] := list[4,4,k] + attack1;
                 end;
     end;
end;    {case1z3a}

procedure case1z3b;
begin   {case1z3b}
     case sequence of
     72        : begin
                    list[2,2,k] := list[2,2,k] + block2;
                    list[1,1,k] := list[1,1,k] + block2;
                 end;
     76        : begin
                    list[2,2,k] := list[2,2,k] + attack2;
                    list[1,1,k] := list[1,1,k] + attack2;
                 end;
     74        : begin
                    list[1,1,k] := list[1,1,k] + block1;
                    list[2,2,k] := list[2,2,k] + block1;
                    list[4,4,k] := list[4,4,k] + block1;
                 end;
     78        : begin
                    list[1,1,k] := list[1,1,k] + block1;
                    list[2,2,k] := list[2,2,k] + block1;
                    list[3,3,k] := list[3,3,k] + block1;
                 end;
     77        : begin
                    list[1,1,k] := list[1,1,k] + attack1;
                    list[2,2,k] := list[2,2,k] + attack1;
                    list[4,4,k] := list[4,4,k] + attack1;
                 end;
     79        : begin
                    list[1,1,k] := list[1,1,k] + attack1;
                    list[2,2,k] := list[2,2,k] + attack1;
                    list[3,3,k] := list[3,3,k] + attack1;
                 end;
     end;
end;    {case1z3b}

begin     {scan1zdiag}
     sequence := 0;
     makestr(box[1,1,k]);
     makestr(box[2,2,k]);
     makestr(box[3,3,k]);
     makestr(box[4,4,k]);
     if sequence < 40 then
        case1z1
     else if sequence < 54 then
        case1z2
     else if sequence < 72 then
        case1z3a
     else
        case1z3b;
end;    {scan1zdiag}

procedure scan2zdiag;

procedure case2z1;
begin   {case2z1}
     case sequence of
     2         : list[1,4,k] := list[1,4,k] + block3;
     6         : list[2,3,k] := list[2,3,k] + block3;
     18        : list[3,2,k] := list[3,2,k] + block3;
     8         : begin
                    list[1,4,k] := list[1,4,k] + block2;
                    list[2,3,k] := list[2,3,k] + block2;
                 end;
     24        : begin
                    list[2,3,k] := list[2,3,k] + block2;
                    list[3,2,k] := list[3,2,k] + block2;
                 end;
     20        : begin
                    list[3,2,k] := list[3,2,k] + block2;
                    list[1,4,k] := list[1,4,k] + block2;
                 end;
     26        : begin
                    list[2,3,k] := list[2,3,k] + block1;
                    list[3,2,k] := list[3,2,k] + block1;
                    list[4,1,k] := list[4,1,k] + block1;
                 end;
     end;
end;    {case2z1}

procedure case2z2;
begin   {case2z2}
     case sequence of
     41        : list[1,4,k] := list[1,4,k] + attack3;
     43        : list[2,3,k] := list[2,3,k] + attack3;
     49        : list[3,2,k] := list[3,2,k] + attack3;
     44        : begin
                    list[1,4,k] := list[1,4,k] + attack2;
                    list[2,3,k] := list[2,3,k] + attack2;
                 end;
     52        : begin
                    list[2,3,k] := list[2,3,k] + attack2;
                    list[3,2,k] := list[3,2,k] + attack2;
                 end;
     50        : begin
                    list[3,2,k] := list[3,2,k] + attack2;
                    list[1,4,k] := list[1,4,k] + attack2;
                 end;
     53        : begin
                    list[2,3,k] := list[2,3,k] + attack1;
                    list[3,2,k] := list[3,2,k] + attack1;
                    list[4,1,k] := list[4,1,k] + attack1;
                 end;
     end;
end;    {case2z2}

procedure case2z3a;
begin   {case2z3a}
     case sequence of
     54        : list[4,1,k] := list[4,1,k] + block3;
     67        : list[4,1,k] := list[4,1,k] + attack3;
     60        : begin
                    list[4,1,k] := list[4,1,k] + block2;
                    list[2,3,k] := list[2,3,k] + block2;
                 end;
     56        : begin
                    list[4,1,k] := list[4,1,k] + block2;
                    list[1,4,k] := list[1,4,k] + block2;
                 end;
     70        : begin
                    list[4,1,k] := list[4,1,k] + attack2;
                    list[2,3,k] := list[2,3,k] + attack2;
                 end;
     68        : begin
                    list[4,1,k] := list[4,1,k] + attack2;
                    list[1,4,k] := list[1,4,k] + attack2;
                 end;
     62        : begin
                    list[4,1,k] := list[4,1,k] + block1;
                    list[2,3,k] := list[2,3,k] + block1;
                    list[1,4,k] := list[1,4,k] + block1;
                 end;
     71        : begin
                    list[4,1,k] := list[4,1,k] + attack1;
                    list[2,3,k] := list[2,3,k] + attack1;
                    list[1,4,k] := list[1,4,k] + attack1;
                 end;
     end;
end;    {case2z3a}

procedure case2z3b;
begin   {case2z3b}
     case sequence of
     72        : begin
                    list[3,2,k] := list[3,2,k] + block2;
                    list[4,1,k] := list[4,1,k] + block2;
                 end;
     76        : begin
                    list[3,2,k] := list[3,2,k] + attack2;
                    list[4,1,k] := list[4,1,k] + attack2;
                 end;
     74        : begin
                    list[4,1,k] := list[4,1,k] + block1;
                    list[3,2,k] := list[3,2,k] + block1;
                    list[1,4,k] := list[1,4,k] + block1;
                 end;
     78        : begin
                    list[4,1,k] := list[4,1,k] + block1;
                    list[3,2,k] := list[3,2,k] + block1;
                    list[2,3,k] := list[2,3,k] + block1;
                 end;
     77        : begin
                    list[4,1,k] := list[4,1,k] + attack1;
                    list[3,2,k] := list[3,2,k] + attack1;
                    list[1,4,k] := list[1,4,k] + attack1;
                 end;
     79        : begin
                    list[4,1,k] := list[4,1,k] + attack1;
                    list[3,2,k] := list[3,2,k] + attack1;
                    list[2,3,k] := list[2,3,k] + attack1;
                 end;
     end;
end;    {case2z3b}

begin   {scan2zdiag}
     sequence := 0;
     makestr(box[4,1,k]);
     makestr(box[3,2,k]);
     makestr(box[2,3,k]);
     makestr(box[1,4,k]);
     if sequence < 40 then
        case2z1
     else if sequence < 54 then
        case2z2
     else if sequence < 72 then
        case2z3a
     else
        case2z3b;
end;      {scan2zdiag}

begin     {scanzplanes}
     for k := 1 to 4 do begin
          for i := 1 to 4 do begin
               countx := 0;
               counto := 0;
               incr   := 0;
               for j := 1 to 4 do begin
                    if box[i,j,k] = 'X' then
                         countx := countx + 1;
                    if box[i,j,k] = 'O' then
                         counto := counto + 1;
               end;
               if countx = 3 then
                    incr := block3;
               if counto = 3 then
                    incr := attack3;
               if (countx = 2) and (counto = 0) then
                    incr := block2;
               if (countx = 0) and (counto = 2) then
                    incr := attack2;
               if (countx = 1) and (counto = 0) then
                    incr := block1;
               if (countx = 0) and (counto = 1) then
                    incr := attack1;
               if incr > 0 then
                    for j := 1 to 4 do
                         if box[i,j,k] = ' ' then
                              list[i,j,k] := list[i,j,k] + incr;
          end;
          scan1zdiag;
          scan2zdiag;
     end;
end;      {scanzplanes}

procedure scan1diag;

procedure case1d1;
begin   {case1d1}
     case sequence of
     2         : list[4,4,4] := list[4,4,4] + block3;
     6         : list[3,3,3] := list[3,3,3] + block3;
     18        : list[2,2,2] := list[2,2,2] + block3;
     8         : begin
                    list[4,4,4] := list[4,4,4] + block2;
                    list[3,3,3] := list[3,3,3] + block2;
                 end;
     24        : begin
                    list[3,3,3] := list[3,3,3] + block2;
                    list[2,2,2] := list[2,2,2] + block2;
                 end;
     20        : begin
                    list[2,2,2] := list[2,2,2] + block2;
                    list[4,4,4] := list[4,4,4] + block2;
                 end;
     26        : begin
                    list[2,2,2] := list[2,2,2] + block1;
                    list[3,3,3] := list[3,3,3] + block1;
                    list[4,4,4] := list[4,4,4] + block1;
                 end;
     end;
end;    {case1d1}

procedure case1d2;
begin   {case1d2}
     case sequence of
     41        : list[4,4,4] := list[4,4,4] + attack3;
     43        : list[3,3,3] := list[3,3,3] + attack3;
     49        : list[2,2,2] := list[2,2,2] + attack3;
     44        : begin
                    list[4,4,4] := list[4,4,4] + attack2;
                    list[3,3,3] := list[3,3,3] + attack2;
                 end;
     52        : begin
                    list[3,3,3] := list[3,3,3] + attack2;
                    list[2,2,2] := list[2,2,2] + attack2;
                 end;
     50        : begin
                    list[2,2,2] := list[2,2,2] + attack2;
                    list[4,4,4] := list[4,4,4] + attack2;
                 end;
     53        : begin
                    list[2,2,2] := list[2,2,2] + attack1;
                    list[3,3,3] := list[3,3,3] + attack1;
                    list[4,4,4] := list[4,4,4] + attack1;
                 end;
     end;
end;    {case1d2}

procedure case1d3a;
begin   {case1d3a}
     case sequence of
     54        : list[1,1,1] := list[1,1,1] + block3;
     67        : list[1,1,1] := list[1,1,1] + attack3;
     60        : begin
                    list[1,1,1] := list[1,1,1] + block2;
                    list[3,3,3] := list[3,3,3] + block2;
                 end;
     56        : begin
                    list[1,1,1] := list[1,1,1] + block2;
                    list[4,4,4] := list[4,4,4] + block2;
                 end;
     70        : begin
                    list[1,1,1] := list[1,1,1] + attack2;
                    list[3,3,3] := list[3,3,3] + attack2;
                 end;
     68        : begin
                    list[1,1,1] := list[1,1,1] + attack2;
                    list[4,4,4] := list[4,4,4] + attack2;
                 end;
     62        : begin
                    list[1,1,1] := list[1,1,1] + block1;
                    list[3,3,3] := list[3,3,3] + block1;
                    list[4,4,4] := list[4,4,4] + block1;
                 end;
     71        : begin
                    list[1,1,1] := list[1,1,1] + attack1;
                    list[3,3,3] := list[3,3,3] + attack1;
                    list[4,4,4] := list[4,4,4] + attack1;
                 end;
     end;
end;    {case1d3a}

procedure case1d3b;
begin   {case1d3b}
     case sequence of
     72        : begin
                    list[2,2,2] := list[2,2,2] + block2;
                    list[1,1,1] := list[1,1,1] + block2;
                 end;
     76        : begin
                    list[2,2,2] := list[2,2,2] + attack2;
                    list[1,1,1] := list[1,1,1] + attack2;
                 end;
     74        : begin
                    list[1,1,1] := list[1,1,1] + block1;
                    list[2,2,2] := list[2,2,2] + block1;
                    list[4,4,4] := list[4,4,4] + block1;
                 end;
     78        : begin
                    list[1,1,1] := list[1,1,1] + block1;
                    list[2,2,2] := list[2,2,2] + block1;
                    list[3,3,3] := list[3,3,3] + block1;
                 end;
     77        : begin
                    list[1,1,1] := list[1,1,1] + attack1;
                    list[2,2,2] := list[2,2,2] + attack1;
                    list[4,4,4] := list[4,4,4] + attack1;
                 end;
     79        : begin
                    list[1,1,1] := list[1,1,1] + attack1;
                    list[2,2,2] := list[2,2,2] + attack1;
                    list[3,3,3] := list[3,3,3] + attack1;
                 end;
     end;
end;    {case1d3b}

begin     {scan1diag}
     sequence := 0;
     makestr(box[1,1,1]);
     makestr(box[2,2,2]);
     makestr(box[3,3,3]);
     makestr(box[4,4,4]);
     if sequence < 40 then
        case1d1
     else if sequence < 54 then
        case1d2
     else if sequence < 72 then
        case1d3a
     else
        case1d3b;
end;      {scan1diag}

procedure scan2diag;

procedure case2d1;
begin   {case2d1}
     case sequence of
     2         : list[4,4,1] := list[4,4,1] + block3;
     6         : list[3,3,2] := list[3,3,2] + block3;
     18        : list[2,2,3] := list[2,2,3] + block3;
     8         : begin
                    list[4,4,1] := list[4,4,1] + block2;
                    list[3,3,2] := list[3,3,2] + block2;
                 end;
     24        : begin
                    list[3,3,2] := list[3,3,2] + block2;
                    list[2,2,3] := list[2,2,3] + block2;
                 end;
     20        : begin
                    list[2,2,3] := list[2,2,3] + block2;
                    list[4,4,1] := list[4,4,1] + block2;
                 end;
     26        : begin
                    list[2,2,3] := list[2,2,3] + block1;
                    list[3,3,2] := list[3,3,2] + block1;
                    list[4,4,1] := list[4,4,1] + block1;
                 end;
     end;
end;    {case2d1}

procedure case2d2;
begin   {case2d2}
     case sequence of
     41        : list[4,4,1] := list[4,4,1] + attack3;
     43        : list[3,3,2] := list[3,3,2] + attack3;
     49        : list[2,2,3] := list[2,2,3] + attack3;
     44        : begin
                    list[4,4,1] := list[4,4,1] + attack2;
                    list[3,3,2] := list[3,3,2] + attack2;
                 end;
     52        : begin
                    list[3,3,2] := list[3,3,2] + attack2;
                    list[2,2,3] := list[2,2,3] + attack2;
                 end;
     50        : begin
                    list[2,2,3] := list[2,2,3] + attack2;
                    list[4,4,1] := list[4,4,1] + attack2;
                 end;
     53        : begin
                    list[2,2,3] := list[2,2,3] + attack1;
                    list[3,3,2] := list[3,3,2] + attack1;
                    list[4,4,1] := list[4,4,1] + attack1;
                 end;
     end;
end;    {case2d2}

procedure case2d3a;
begin   {case2d3a}
     case sequence of
     54        : list[1,1,4] := list[1,1,4] + block3;
     67        : list[1,1,4] := list[1,1,4] + attack3;
     60        : begin
                    list[1,1,4] := list[1,1,4] + block2;
                    list[3,3,2] := list[3,3,2] + block2;
                 end;
     56        : begin
                    list[1,1,4] := list[1,1,4] + block2;
                    list[4,4,1] := list[4,4,1] + block2;
                 end;
     70        : begin
                    list[1,1,4] := list[1,1,4] + attack2;
                    list[3,3,2] := list[3,3,2] + attack2;
                 end;
     68        : begin
                    list[1,1,4] := list[1,1,4] + attack2;
                    list[4,4,1] := list[4,4,1] + attack2;
                 end;
     62        : begin
                    list[1,1,4] := list[1,1,4] + block1;
                    list[3,3,2] := list[3,3,2] + block1;
                    list[4,4,1] := list[4,4,1] + block1;
                 end;
     71        : begin
                    list[1,1,4] := list[1,1,4] + attack1;
                    list[3,3,2] := list[3,3,2] + attack1;
                    list[4,4,1] := list[4,4,1] + attack1;
                 end;
     end;
end;    {case2d3a}

procedure case2d3b;
begin   {case2d3b}
     case sequence of
     72        : begin
                    list[2,2,3] := list[2,2,3] + block2;
                    list[1,1,4] := list[1,1,4] + block2;
                 end;
     76        : begin
                    list[2,2,3] := list[2,2,3] + attack2;
                    list[1,1,4] := list[1,1,4] + attack2;
                 end;
     74        : begin
                    list[1,1,4] := list[1,1,4] + block1;
                    list[2,2,3] := list[2,2,3] + block1;
                    list[4,4,1] := list[4,4,1] + block1;
                 end;
     78        : begin
                    list[1,1,4] := list[1,1,4] + block1;
                    list[2,2,3] := list[2,2,3] + block1;
                    list[3,3,2] := list[3,3,2] + block1;
                 end;
     77        : begin
                    list[1,1,4] := list[1,1,4] + attack1;
                    list[2,2,3] := list[2,2,3] + attack1;
                    list[4,4,1] := list[4,4,1] + attack1;
                 end;
     79        : begin
                    list[1,1,4] := list[1,1,4] + attack1;
                    list[2,2,3] := list[2,2,3] + attack1;
                    list[3,3,2] := list[3,3,2] + attack1;
                 end;
     end;
end;    {case2d3b}

begin     {scan2diag}
     sequence := 0;
     makestr(box[1,1,4]);
     makestr(box[2,2,3]);
     makestr(box[3,3,2]);
     makestr(box[4,4,1]);
     if sequence < 40 then
        case2d1
     else if sequence < 54 then
        case2d2
     else if sequence < 72 then
        case2d3a
     else
        case2d3b;
end;      {scan2diag}

procedure scan3diag;

procedure case3d1;
begin   {case3d1}
     case sequence of
     2         : list[4,1,1] := list[4,1,1] + block3;
     6         : list[3,2,2] := list[3,2,2] + block3;
     18        : list[2,3,3] := list[2,3,3] + block3;
     8         : begin
                    list[4,1,1] := list[4,1,1] + block2;
                    list[3,2,2] := list[3,2,2] + block2;
                 end;
     24        : begin
                    list[3,2,2] := list[3,2,2] + block2;
                    list[2,3,3] := list[2,3,3] + block2;
                 end;
     20        : begin
                    list[2,3,3] := list[2,3,3] + block2;
                    list[4,1,1] := list[4,1,1] + block2;
                 end;
     26        : begin
                    list[2,3,3] := list[2,3,3] + block1;
                    list[3,2,2] := list[3,2,2] + block1;
                    list[4,1,1] := list[4,1,1] + block1;
                 end;
     end;
end;    {case3d1}

procedure case3d2;
begin   {case3d2}
     case sequence of
     41        : list[4,1,1] := list[4,1,1] + attack3;
     43        : list[3,2,2] := list[3,2,2] + attack3;
     49        : list[2,3,3] := list[2,3,3] + attack3;
     44        : begin
                    list[4,1,1] := list[4,1,1] + attack2;
                    list[3,2,2] := list[3,2,2] + attack2;
                 end;
     52        : begin
                    list[3,2,2] := list[3,2,2] + attack2;
                    list[2,3,3] := list[2,3,3] + attack2;
                 end;
     50        : begin
                    list[2,3,3] := list[2,3,3] + attack2;
                    list[4,1,1] := list[4,1,1] + attack2;
                 end;
     53        : begin
                    list[2,3,3] := list[2,3,3] + attack1;
                    list[3,2,2] := list[3,2,2] + attack1;
                    list[4,1,1] := list[4,1,1] + attack1;
                 end;
     end;
end;    {case3d2}

procedure case3d3a;
begin   {case3d3a}
     case sequence of
     54        : list[1,4,4] := list[1,4,4] + block3;
     67        : list[1,4,4] := list[1,4,4] + attack3;
     60        : begin
                    list[1,4,4] := list[1,4,4] + block2;
                    list[3,2,2] := list[3,2,2] + block2;
                 end;
     56        : begin
                    list[1,4,4] := list[1,4,4] + block2;
                    list[4,1,1] := list[4,1,1] + block2;
                 end;
     70        : begin
                    list[1,4,4] := list[1,4,4] + attack2;
                    list[3,2,2] := list[3,2,2] + attack2;
                 end;
     68        : begin
                    list[1,4,4] := list[1,4,4] + attack2;
                    list[4,1,1] := list[4,1,1] + attack2;
                 end;
     62        : begin
                    list[1,4,4] := list[1,4,4] + block1;
                    list[3,2,2] := list[3,2,2] + block1;
                    list[4,1,1] := list[4,1,1] + block1;
                 end;
     71        : begin
                    list[1,4,4] := list[1,4,4] + attack1;
                    list[3,2,2] := list[3,2,2] + attack1;
                    list[4,1,1] := list[4,1,1] + attack1;
                 end;
     end;
end;      {case3d3a}

procedure case3d3b;
begin     {case3d3b}
     case sequence of
     72        : begin
                    list[2,3,3] := list[2,3,3] + block2;
                    list[1,4,4] := list[1,4,4] + block2;
                 end;
     76        : begin
                    list[2,3,3] := list[2,3,3] + attack2;
                    list[1,4,4] := list[1,4,4] + attack2;
                 end;
     74        : begin
                    list[1,4,4] := list[1,4,4] + block1;
                    list[2,3,3] := list[2,3,3] + block1;
                    list[4,1,1] := list[4,1,1] + block1;
                 end;
     78        : begin
                    list[1,4,4] := list[1,4,4] + block1;
                    list[2,3,3] := list[2,3,3] + block1;
                    list[3,2,2] := list[3,2,2] + block1;
                 end;
     77        : begin
                    list[1,4,4] := list[1,4,4] + attack1;
                    list[2,3,3] := list[2,3,3] + attack1;
                    list[4,1,1] := list[4,1,1] + attack1;
                 end;
     79        : begin
                    list[1,4,4] := list[1,4,4] + attack1;
                    list[2,3,3] := list[2,3,3] + attack1;
                    list[3,2,2] := list[3,2,2] + attack1;
                 end;
     end;
end;      {case3d3b}

begin     {scan3diag}
     sequence := 0;
     makestr(box[1,4,4]);
     makestr(box[2,3,3]);
     makestr(box[3,2,2]);
     makestr(box[4,1,1]);
     if sequence < 40 then
        case3d1
     else if sequence < 54 then
        case3d2
     else if sequence < 72 then
        case3d3a
     else
        case3d3b;
end;      {scan3diag}

procedure scan4diag;

procedure case4d1;
begin   {case4d1}
     case sequence of
     2         : list[4,1,4] := list[4,1,4] + block3;
     6         : list[3,2,3] := list[3,2,3] + block3;
     18        : list[2,3,2] := list[2,3,2] + block3;
     8         : begin
                    list[4,1,4] := list[4,1,4] + block2;
                    list[3,2,3] := list[3,2,3] + block2;
                 end;
     24        : begin
                    list[3,2,3] := list[3,2,3] + block2;
                    list[2,3,2] := list[2,3,2] + block2;
                 end;
     20        : begin
                    list[2,3,2] := list[2,3,2] + block2;
                    list[4,1,4] := list[4,1,4] + block2;
                 end;
     26        : begin
                    list[2,3,2] := list[2,3,2] + block1;
                    list[3,2,3] := list[3,2,3] + block1;
                    list[4,1,4] := list[4,1,4] + block1;
                 end;
     end;
end;    {case4d1}

procedure case4d2;
begin   {case4d2}
     case sequence of
     41        : list[4,1,4] := list[4,1,4] + attack3;
     43        : list[3,2,3] := list[3,2,3] + attack3;
     49        : list[2,3,2] := list[2,3,2] + attack3;
     44        : begin
                    list[4,1,4] := list[4,1,4] + attack2;
                    list[3,2,3] := list[3,2,3] + attack2;
                 end;
     52        : begin
                    list[3,2,3] := list[3,2,3] + attack2;
                    list[2,3,2] := list[2,3,2] + attack2;
                 end;
     50        : begin
                    list[2,3,2] := list[2,3,2] + attack2;
                    list[4,1,4] := list[4,1,4] + attack2;
                 end;
     53        : begin
                    list[2,3,2] := list[2,3,2] + attack1;
                    list[3,2,3] := list[3,2,3] + attack1;
                    list[4,1,4] := list[4,1,4] + attack1;
                 end;
     end;
end;    {case4d2}

procedure case4d3a;
begin   {case4d3a}
     case sequence of
     54        : list[1,4,1] := list[1,4,1] + block3;
     67        : list[1,4,1] := list[1,4,1] + attack3;
     60        : begin
                    list[1,4,1] := list[1,4,1] + block2;
                    list[3,2,3] := list[3,2,3] + block2;
                 end;
     56        : begin
                    list[1,4,1] := list[1,4,1] + block2;
                    list[4,1,4] := list[4,1,4] + block2;
                 end;
     70        : begin
                    list[1,4,1] := list[1,4,1] + attack2;
                    list[3,2,3] := list[3,2,3] + attack2;
                 end;
     68        : begin
                    list[1,4,1] := list[1,4,1] + attack2;
                    list[4,1,4] := list[4,1,4] + attack2;
                 end;
     62        : begin
                    list[1,4,1] := list[1,4,1] + block1;
                    list[3,2,3] := list[3,2,3] + block1;
                    list[4,1,4] := list[4,1,4] + block1;
                 end;
     71        : begin
                    list[1,4,1] := list[1,4,1] + attack1;
                    list[3,2,3] := list[3,2,3] + attack1;
                    list[4,1,4] := list[4,1,4] + attack1;
                 end;
     end;
end;      {case4d3a}

procedure case4d3b;
begin     {case4d3b}
     case sequence of
     72        : begin
                    list[2,3,2] := list[2,3,2] + block2;
                    list[1,4,1] := list[1,4,1] + block2;
                 end;
     76        : begin
                    list[2,3,2] := list[2,3,2] + attack2;
                    list[1,4,1] := list[1,4,1] + attack2;
                 end;
     74        : begin
                    list[1,4,1] := list[1,4,1] + block1;
                    list[2,3,2] := list[2,3,2] + block1;
                    list[4,1,4] := list[4,1,4] + block1;
                 end;
     78        : begin
                    list[1,4,1] := list[1,4,1] + block1;
                    list[2,3,2] := list[2,3,2] + block1;
                    list[3,2,3] := list[3,2,3] + block1;
                 end;
     77        : begin
                    list[1,4,1] := list[1,4,1] + attack1;
                    list[2,3,2] := list[2,3,2] + attack1;
                    list[4,1,4] := list[4,1,4] + attack1;
                 end;
     79        : begin
                    list[1,4,1] := list[1,4,1] + attack1;
                    list[2,3,2] := list[2,3,2] + attack1;
                    list[3,2,3] := list[3,2,3] + attack1;
                 end;
     end;
end;      {case4d3b}

begin     {scan4diag}
     sequence := 0;
     makestr(box[1,4,1]);
     makestr(box[2,3,2]);
     makestr(box[3,2,3]);
     makestr(box[4,1,4]);
     if sequence < 40 then
        case4d1
     else if sequence < 54 then
        case4d2
     else if sequence < 72 then
        case4d3a
     else
        case4d3b;
end;      {scan4diag}

begin     {assignloading}
     for i := 1 to 4 do
             for j := 1 to 4 do
                     for k := 1 to 4 do
                        if box[i,j,k] <> ' ' then
                             list[i,j,k] := 0
                        else
                             list[i,j,k] := 1;
     scanxplanes;
     scanyplanes;
     scanzplanes;
     scan1diag;
     scan2diag;
     scan3diag;
     scan4diag;
     for i := 2 to 3 do
          for j := 2 to 3 do
               for k := 2 to 3 do
                    if box[i,j,k] = ' ' then
                         list[i,j,k] := list[i,j,k] + nicespot;
     i := 1;
     repeat
          j := 1;
          repeat
               k := 1;
               repeat
                    if box[i,j,k] = ' ' then
                         list[i,j,k] := list[i,j,k] + nicespot;
                    k := k + 3;
               until k > 4;
               j := j + 3;
          until j > 4;
          i := i + 3;
     until i > 4;
end;      {assignloading}

{
  UCSD Pascal on the MicroEngine didn't offer a random number generator, so
  it had to be fudged with a pseudo random number generator.  That is now gone.
}

function rand(limit:range):range;
var	foo : integer;
begin   {rand}
     foo := trunc(limit*random);
     if (foo = 0) then
	rand := 1
     else
        rand := foo;
end;    {rand}

begin     {mymove}
     gotoxy(28,17);
     write('My Move');
     assignloading;
     choice[1].grid := 1;
     choice[1].row  := 1;
     choice[1].col  := 1;
     index          := 1;
     for i := 1 to 4 do
          for j := 1 to 4 do
               for k := 1 to 4 do
                    if list[i,j,k] = list[choice[1].grid,
                                          choice[1].row,
                                          choice[1].col] then begin
                         index := index + 1;
                         choice[index].grid := i;
                         choice[index].row  := j;
                         choice[index].col  := k;
                    end
                    else if list[i,j,k] > list[choice[1].grid,
                                               choice[1].row,
                                               choice[1].col] then begin
                         index := 1;
                         choice[index].grid := i;
                         choice[index].row  := j;
                         choice[index].col  := k;
                    end;
     index := rand(index); { choose one of a set of equivalent moves }
     box[choice[index].grid,choice[index].row,choice[index].col] := 'O';
     plotmove(choice[index].grid,choice[index].row,choice[index].col,'O');
     gotoxy(0,18);
     cleareol;
     gotoxy(25,18);
     write('My move was ',choice[index].grid:1,',',choice[index].row:1);
     write(',',choice[index].col:1);
     checkwin('O');
end;      {mymove}

procedure play;
var
        countfree       : integer;
        i,j,k           : dimension;

begin     {play}
     eog  := false;
     repeat
          yourmove;
          if not eog then
               mymove;
          countfree := 0;
          for i := 1 to 4 do
                for j := 1 to 4 do
                        for k := 1 to 4 do
                                if box[i,j,k] = ' ' then
                                        countfree := countfree + 1;
          if countfree = 0 then
                eog := true;
     until eog;
end;      {play}

procedure checkcontinue;
begin     {checkcontinue}
     gotoxy(17,20);
     write('Do you want to play again (y or n)? ');
     repeat
	gotoxy(53,20);
     	read(cmd);
     until cmd in ['y','n'];
     if cmd = 'y' then begin
          gotoxy(24,21);
          write('Same style (y or n)?');
	  repeat 
		gotoxy(45,21);          
		read(cmd);
	  until cmd in ['y','n'];
          if cmd = 'n' then
                setstyle;
          enough := false;
     end
     else
          enough := true;
end;      {checkcontinue}

begin     {noughts_and_crosses}
     setstyle;
     repeat
          initialise;
          play;
          checkcontinue;
     until enough;  
end.      {noughts_and_crosses}
