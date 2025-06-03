$ __save_verif = 'f$verify(0)
$! CANONICALDIR.COM -- Turns a directory spec. into its canonical form.
$! Copyright (C) 1994  the Free Software Foundation, Inc.
$!
$! Author: Richard Levitte <levitte@e.kth.se>
$!
$! Please see the end of file for further comments
$!
$! Description:
$!
$! P1 = Original directory spec.
$! P2 = symbol to put the result in.
$
$ __debug = f$trnlnm("DEBUG_CANONICALDIR")
$
$! There are five things that you should convert:
$! < -> [, and > -> ]
$! [000000.x] -> [x] (and [x.][000000.y] -> [x.][y])
$! [x.][000000] -> [x]
$! [x.][y] -> [x.y]
$! [x.-.y] -> [y] (and [x.y.--.z] -> [z], etc...)
$
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Converting ""<"" and "">"" to ""["" and ""]"""
$ i = -1
$angle_loop:	! < -> [, and > -> ]
$ i = i + 1
$ if i .lt. 2
$  then
$   oangle = f$extract(i,1,"<>")
$   nangle = f$extract(i,1,"[]")
$  inner_angle_loop:
$   pl = f$element(0,oangle,p1)
$   pr = f$element(1,oangle,p1)
$   if pr .nes. oangle
$    then
$     p1 = pl+nangle+pr
$     goto inner_angle_loop
$    endif
$   goto angle_loop
$  endif
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Result: """,p1,""""
$
$ 'p2' == p1
$ if f$locate("[",p1) .eq. f$length(p1) then goto exit ! No directory part...
$
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Removing ""000000"", take 1"
$z_loop1:	! [000000.x] -> [x] (and [x.][000000.y] -> [x.][y])
$ p = f$locate("[000000.",p1)
$ if p .lt. f$length(p1)
$  then
$   p1 = f$extract(0,p,p1) + "[" + f$extract(p+8,f$length(p1)-8,p1)
$   goto z_loop1
$  endif
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Result: """,p1,""""
$
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Removing ""000000"", take 2"
$z_loop2:	! [x.][000000] -> [x]
$ if f$extract(f$length(p1)-10,10,p1) .eqs. ".][000000]"
$  then
$   p1 = f$extract(0,f$length(p1)-10,p1) + "]"
$   goto z_loop2
$  endif
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Result: """,p1,""""
$
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Removing ""]["""
$dbl_loop:	! [x.][y] -> [x.y]
$ p = p1 - "]["
$ if p .nes. p1
$  then
$   p1 = p
$   goto dbl_loop
$  endif
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Result: """,p1,""""
$
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Removing "".-."" (this one is hard)"
$back_loop:	! [x.-.y] -> [y] (and [x.y.--.z] -> [z], etc...)
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, back_loop"
$ p = f$locate(".-",p1)
$ if __debug then show sym p
$ if p .lt. f$length(p1)
$  then
$   dot0 = f$locate("[",p1) + 1
$   if __debug then show sym dot0
$   offset = dot0
$   searched = f$extract(dot0,f$length(p1),p1)
$   dot_counter = 0
$  back_loop_dots:
$   if __debug then write sys$output "%CANONICALDIR-I-DEBUG, back_loop_dots"
$   if __debug then wait 0:0:1
$   q = f$locate(".",searched)
$   if __debug then show sym searched
$   if __debug then show sym q
$   if searched .nes. "" .and. q + offset .le. p
$    then
$     r = dot_counter + 1
$     dot'r' = q + offset
$     dot_counter = r
$     if __debug then show sym dot'r'
$     offset = offset + q + 1
$     searched = f$extract(q + 1,f$length(searched),searched)
$     goto back_loop_dots
$    endif
$  back_loop_process:
$   if __debug then write sys$output "%CANONICALDIR-I-DEBUG, back_loop_process"
$   p = p + 1
$   if f$extract(p,1,p1) .eqs. "-" .and. dot_counter .gt. 0
$    then
$     dot_counter = dot_counter - 1
$     goto back_loop_process
$    endif
$   if f$extract(p,1,p1) .eqs. "." .and. dot_counter .eq. 0 then p = p + 1
$   if __debug then show sym dot'dot_counter
$   if __debug then show sym p
$   p1 = f$extract(0,dot'dot_counter',p1) + f$extract(p,f$length(p1),p1)
$   goto back_loop
$  endif
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Result: """,p1,""""
$
$ if __debug then write sys$output "%CANONICALDIR-I-DEBUG, Converting possible ""[]"" to ""[000000]"""
$! [] -> [000000]
$ if p1 - "[]" .nes. p1 then p1 = p1 - "[]" + "[000000]"
$
$ 'p2' == p1
$exit:
$ exit 1 + 0*f$verify(__save_verif)
$
$! This file is a part of GNU VMSLIB, the GNU library for porting GNU
$! software to VMS.
$!
$! GNU VMSLIB is free software; you can redistribute it and/or
$! modify it under the terms of the GNU General Public License
$! as published by the Free Software Foundation; either version 2
$! of the License, or (at your option) any later version.
$! 
$! GNU VMSLIB is distributed in the hope that it will be useful,
$! but WITHOUT ANY WARRANTY; without even the implied warranty of
$! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
$! GNU General Public License for more details.
$! 
$! You should have received a copy of the GNU General Public License
$! along with GNU VMSLIB; if not, write to the Free Software
$! Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
