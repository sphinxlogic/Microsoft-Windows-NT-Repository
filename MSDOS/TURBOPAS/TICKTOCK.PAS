PROGRAM ticktock(input,output);
{
       Name: ticktock.pas
     Author: Michael Tighe
     System: IBM PC/XT/AT/PS2, MS-DOS 3.30
   Language: Turbo Pascal Version 5.0
Description: Demo of IBM PC's high resolution clock
 
    INFO FOR TICKTOCK:
 
    The TICKTOCK programs demonstrate how to obtain accurate timing
information from the IBM PC/XT/AT family of computers. The next few
paragraphs should give you a basic idea of how the time is stored in these
computer systems.
 
    In the PC family, an internal clock runs at 1.193180 Mhz. This clock
is divided by 65536 to give 18.206482 clock pulses per second (.0549255
seconds per clock pulse). Therefore, the clock 'ticks' every .0549255
seconds.
 
    Two addresses in low memory are used to keep track of the tick count.
They are both 1 word (two bytes) in length. The first is at address
0000:046C. It is incremented 18.2 times a second. When it overflows, it is
reset to 0 and another word at address 0000:046E is incremented.
 
    It should be noted that the word at address 0000:046E is also the
current hour, in 24 hour format. The address at 0000:046C when divided by
18.2, is the current time past the hour, in seconds.

}
{ compiler directives }
{$a+}               {word alignment         (+=on,-=off                  (+)}
{$b+}               {boolean evaluation     (+=complete,-=short circuit) (-)}
{$d-}               {debug information      (+=on,-=off)                 (+)}
{$e-}               {emulation              (+=on,-=off)                 (+)}
{$f-}               {force far calls        (+=all far,-=as needed)      (-)}
{$i-}               {i/o error checking     (+=on,-=off)                 (+)}
{$l-}               {local symbols          (+=on,-=off)                 (+)}
{$m 16384,0,655360} {memory allocation      (stk,minhp,maxhp)   (16k,0,640k)}
{$n+}               {numeric data processor (+=true,-=false              (-)}
{$o-}               {overlays               (+=on,-=off                  (+)}
{$r-}               {range checking         (+=on,-=off                  (+)}
{$s-}               {stack checking         (+=on,-=off                  (+)}
{$v-}               {var string checking    (+=on,-=off)                 (+)}
 
USES
crt;
 
CONST
version_number = '[TICKTOCK Version 87.365]';
 
VAR
t1,t2     : byte;
tick,tock : real;
 
BEGIN
   writeln(version_number); writeln;
 
   t1 := 0; t2 := 0;
   t1 := mem[$0000:$046c]; t2 := mem[$0000:$046d];
   tick := int(t1) + int(t2)*256;
   t1 := mem[$0000:$046e]; t2 := mem[$0000:$046f];
   tock := int(t1) + int(t2)*256;
   writeln('Tick value is ',tick:6:0,', Tock value is ',tock:6:0);
 
   writeln('Sleeping for 5 seconds (~91 ticks)...');
   delay(5000);
 
   t1 := 0; t2 := 0;
   t1 := mem[$0000:$046c]; t2 := mem[$0000:$046d];
   tick := int(t1) + int(t2)*256;
   t1 := mem[$0000:$046e]; t2 := mem[$0000:$046f];
   tock := int(t1) + int(t2)*256;
   writeln('Tick value is ',tick:6:0,', Tock value is ',tock:6:0);
END.
