$ verflg = 'f$verify(0)'
$
$ title = p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8
$
$ OSC[0,8] = 157
$ ST[0,8]  = 156
$ say OSC,"2L;''title'",ST, OSC,"21;''title'",ST
$
$ EXIT 1 + 0 * f$verify(verflg)
