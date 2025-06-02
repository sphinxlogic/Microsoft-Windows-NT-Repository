Unit grafdump;

Interface

   Uses
      dos, graph;

   Procedure hardcopy(inverse: boolean;
                      gmode, gdevice: integer);

   { MGA, CGA, Herc, EGA, VGA Graphics screen dump routine
     for EPSON  compatible printers. Turbo Pascal, v4 or better

     Any non black screen pixel (in first page) is printed. }


Implementation

   Procedure hardcopy(inverse: boolean;
                      gmode, gdevice: integer);

    { Inverse true produces an inverse image.
      Gmode and gdevice are the arguments returned from Turbo's
           InitGraph procedure.

      Dot spacing (mode) and multiples of each byte across the page
          are manipulated to give the 'best' aspect ratio on the print.

      Mode: 1 = Double-Density 120 dpi,        2 = High-Speed D-D 120 dpi
            3 = Quad-Density 240 dpi,          4 = 80 dpi
            5 = 72 dpi,                        6 = 90 dpi
            0 = 60 dpi.

      Pre-FX series of EPSON printers should only use Mode 1.

      This routine uses mode 2 for VGAHi (ymaxglbl=479) and CGACn
           (xmaxglb=319), -  mode 6 for all other gmodes.
           CGACn print is 1/2 page and crude, rest are full page

      Nb. Uses ReadMode 1 for EGA & VGA to cmp black with all bit planes.

      Reference : "Programmers guide to PC & PS/2 Video System"
                   R Wilton, MicroSoft Press, 1987
           Graft:  Gordon Findlay, Christchurch School of Medicine
                    (gordon%chmeds.ac.nz@cunyvm.cuny.edu)
                   Mac McLennan, Forest Research Institute
                                 Christchurch, New Zealand
    }

      Const
         esc = 27;
         lptportnum = 1; { Defaults to LPT1. 2 = LPT2 }

      Var
         regs: registers;
         boff: integer;
         mode, pbyte, sb, i, j, n1, n2: byte;
         pwide, grafbase, ymaxglb, xmaxglb: word;


      Function nextpbyte(x, y: word): byte;


         Begin
         Case gdevice Of
            cga:
               If (gmode = cgahi) Then
                  pbyte := mem[grafbase: (y shr 1) * 80 + (y And
                           1) * $2000 + (x shr 3)]
               Else
                  Begin
                  pbyte := 0;
                  i := 0;
                  boff := (y shr 1) * 80 + (y And 1) * $2000 + (x shr 2);
                  Repeat
                     sb := mem[grafbase: boff + i];
                     j := 0;
                     Repeat
                        pbyte := pbyte shl 1;
                        If ((sb And $c0) > 0) Then
                           inc(pbyte);
                        sb := sb shl 2;
                        inc(j);
                     Until (j > 3);
                     inc(i);
                  Until (i > 1);
                  End;

            vga, ega, ega64, egamono:
               pbyte := Not mem[grafbase: (y * 80 + (x shr 3))];

            hercmono:
               pbyte := mem[grafbase: (y And
                        3)shl 13 + 90 * (y shr 2) + (x shr 3)];

            Else
               pbyte := 0 { should never happen }
            End;

         If inverse Then
            nextpbyte := Not pbyte
         Else
            nextpbyte := pbyte;
         End;


      Procedure sendbyte(pb: byte);
       { Send one byte to the printer }

         Begin
         regs.ah := 0;
         regs.al := pb;
         regs.dx := pred(lptportnum);
         intr($17, regs);
         End; { SendByte }


      Procedure dumpgraf;

         Var
            xi, yi: integer; { Pixel coordinates }


         Procedure printaline;

            Begin
            { Select 8-Pin graphics print mode for each line }
            sendbyte(esc);
            sendbyte(ord('*'));
            sendbyte(mode);
            sendbyte(n1);
            sendbyte(n2); { dots on line }

           { Get the next print line off the screen, bottom of screen
             is the lh side of the paper.  Each byte is 8 pixels in the
             X coordinate and 1 in the Y, and is printed twice to improve
             aspect ratio (except where ymaxglb = 199 - printed 3x).      }

            yi := ymaxglb;
            Repeat
               pbyte := nextpbyte(xi, yi); { Each byte is 1 pixel (Y axis)}
               sendbyte(pbyte);
               sendbyte(pbyte);
               If (ymaxglb < 200) Then
                  sendbyte(pbyte);
               dec(yi);
            Until (yi < 0);
            sendbyte(10); { Send LF }
            End;


         Begin
         xi := 0;
         Repeat
            printaline; { Each printed line is 8 pixels deep (X axis) }
            inc(xi, 8);
         Until (xi > xmaxglb);
         sendbyte(13);
         sendbyte(12); { Send CR & FF }
         End; { DumpGraf }


      Function checkprt: boolean;
       { Check Printer connected, online and has paper, If not then BEEP }

         Begin
         regs.ax := $200;
         regs.dx := pred(lptportnum);
         intr($17, regs);
         regs.ah := regs.ah xor $80;
         regs.ah := regs.ah And $A9;
         If (regs.ah <> 0) Then
            Begin
            writeln(#7,#7);
            checkprt := false;
            End
         Else
            checkprt := true;
         End;


      Begin { HardCopy }
      If Not checkprt Then
         exit; { and beep }
      Case gdevice Of
         cga:
            Begin
            grafbase := $B800;
            ymaxglb := 199;
            If (gmode = cgahi) Then
               xmaxglb := 639
            Else
               xmaxglb := 319;
            End;
         ega, ega64, egamono:
            Begin
            grafbase := $A000;
            xmaxglb := 639;
            ymaxglb := 349;
            portw[$3CE] := $805; { Set ReadMode = 1 - all bit planes cmpd.}
            { Defaults assumed:- Color Don't Care register = $0F }
            {                  - Color Compare    register = $00 }
            End;
         vga:
            Begin
            grafbase := $A000;
            xmaxglb := 639;
            portw[$3CE] := $805;
            Case gmode Of
               vgalo:
                  ymaxglb := 199;
               vgamed:
                  ymaxglb := 349;
               vgahi:
                  ymaxglb := 479;
               End;
            End;
         hercmono:
            Begin
            grafbase := $B000;
            xmaxglb := 719;
            ymaxglb := 347
            End;
         Else
            exit
         End;

      sendbyte(esc); { Select 24/216-inch line spacing }
      sendbyte(ord('3'));
      sendbyte(24);

      If ((xmaxglb < 320) Or (ymaxglb = 479)) Then
         mode := 2
      Else
         mode := 6;
      pwide := ymaxglb + 1;
      If (ymaxglb < 200) Then
         pwide := pwide * 3
      Else
         pwide := pwide shl 1;
      n1 := lo(pwide); { Determine 2 byte control code for }
      n2 := hi(pwide); { the number of dots per print line }

      dumpgraf;

      If (gdevice In [vga, ega, ega64, egamono]) Then
         portw[$3CE] := 5;

      sendbyte(esc);
      sendbyte(2); { Reset to 1/6-inch line spacing }
      End; { HardCopy }


End.
