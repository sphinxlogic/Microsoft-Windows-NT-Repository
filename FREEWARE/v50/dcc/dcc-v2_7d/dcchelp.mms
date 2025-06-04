!! Program Name            : DCCHELP.MMS
!!   Original Author       : JLAURET
!!   Date                  : 22-Jul-1999
!!   Last modification     : 16-Feb-2000
!!   Program Description   : Update all DCC documentation. 
!!                         : 

ALL : dcc.man dcc.html dcc.hlp dcc.txt dcc.ps

dcc.man  : dcreadme.pod
	pod2man dcreadme.pod >dcc.man

dcc.html : dcreadme.pod
	pod2html dcreadme.pod >dcc.html

dcc.txt  : dcreadme.pod
	perl pod2text.pl dcreadme.pod >dcc.txt

dcc.hlp  : dcreadme.pod
	copy  dcreadme.pod dcc.pod
	pod2hlp dcc.pod

dcc.ps   : dcc.html
	html2ps dcc.html >dcc.ps
