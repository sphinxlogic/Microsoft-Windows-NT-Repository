.\"	@(#)tmac.e	2.31 (Berkeley) 5/21/88
.\"	Modified by James Clark for use with groff.
.\"
.\" Copyright (c) 1988 Regents of the University of California.
.\" All rights reserved.
.\"
.\" Redistribution and use in source and binary forms are permitted
.\" provided that this notice is preserved and that due credit is given
.\" to the University of California at Berkeley. The name of the University
.\" may not be used to endorse or promote products derived from this
.\" software without specific prior written permission. This software
.\" is provided ``as is'' without express or implied warranty.
.\"	%beginstrip%
.\"
.\"**********************************************************************
.\"*									*
.\"*	******  - M E   N R O F F / T R O F F   M A C R O S  ******	*
.\"*									*
.\"*	Produced for your edification and enjoyment by:			*
.\"*		Eric Allman						*
.\"*		Electronics Research Laboratory				*
.\"*		U.C. Berkeley.						*
.\"*	current address:						*
.\"*		Britton-Lee, Inc.					*
.\"*		1919 Addison Street Suite 105				*
.\"*		Berkeley, California  94704				*
.\"*									*
.\"*	VERSION 2.31	First Release: 11 Sept 1978			*
.\"*									*
.\"*	Documentation is available.					*
.\"*									*
.\"**********************************************************************
.\"
.\"	Code on .de commands:
.\"		***	a user interface macro.
.\"		&&&	a user interface macro which is redefined
.\"			when used to be the real thing.
.\"		$$$	a macro which may be redefined by the user
.\"			to provide variant functions.
.\"		---	an internal macro.
.\"
.if !\n(.g .ig
.de @R			\" --- initialize number register to 0, if undefined
.if !r\\$1 .nr \\$1 0
..
.@R pf
.if \n(pf .nx
.if !\n(.g .ig
.de @S			\" --- initialize string/macro to empty, if undefined
.if !d\\$1 .ds \\$1 \" empty
..
.@R @\"			\" debugging level
.\"		*** INTERNAL GP MACROS ***
.de @C			\" --- change ev's, taking info with us
.nr _S \\n(.s
.nr _V \\n(.v
.nr _F \\n(.f
.do ds _A \\n[.fam]
.nr _I \\n(.i
.ev \\$1
.ps \\n(_S
.vs \\n(_Vu
.ft \\n(_F
.do @fam \\*(_A
'in \\n(_Iu
.xl \\n($lu
.lt \\n($lu
.rr _S
.rr _V
.rr _F
.rr _I
.ls 1
'ce 0
..
.de @D			\" --- determine display type (Indent, Left, Center)
.ds |p "\\$3
.nr _d 0
.if "\\$2"C" \
.	nr _d 1
.if "\\$2"L" \
.	nr _d 2
.if "\\$2"I" \
.	nr _d 3
.if "\\$2"M" \
.	nr _d 4
.if !\\n(_d \{\
.	nr _d \\$1
.	ds |p "\\$2
.\}
..
.de @z			\" --- end macro
.if \n@>1 .tm >> @z, .z=\\n(.z ?a=\\n(?a
.if !"\\n(.z"" \
\{\
.	tm Line \\n(c. -- Unclosed block, footnote, or other diversion (\\n(.z)
.	di
.	ex
.\}
.if \\n(?a \
.	bp			\" force out final table
.ds bp
.ds @b\"			\" don't start another page
.br
.if \n@>1 .tm << @z
..
.\"		*** STANDARD HEADERS AND FOOTERS ***
.ie \n(.g .ds $* \\\\$*
.el .ds $* \\\\$1 \\\\$2 \\\\$3 \\\\$4 \\\\$5 \\\\$6 \\\\$7 \\\\$8 \\\\$9
.de he			\" *** define header
.ie !\\n(.$ \
\{\
.	rm |4
.	rm |5
.\}
.el \
\{\
.	ds |4 "\*($*
.	ds |5 "\*($*
.\}
..
.de eh			\" *** define even header
.ie !\\n(.$ \
.	rm |4
.el \
.	ds |4 "\*($*
..
.de oh			\" *** define odd header
.ie !\\n(.$ \
.	rm |5
.el \
.	ds |5 "\*($*
..
.de fo			\" *** define footer
.ie !\\n(.$ \
\{\
.	rm |6
.	rm |7
.\}
.el \
\{\
.	ds |6 "\*($*
.	ds |7 "\*($*
.\}
..
.de ef			\" *** define even foot
.ie !\\n(.$ \
.	rm |6
.el \
.	ds |6 "\*($*
..
.de of			\" *** define odd footer
.ie !\\n(.$ \
.	rm |7
.el \
.	ds |7 "\*($*
..
.de ep			\" *** end page (must always be followed by a .bp)
.if \\n(nl>0 \
\{\
.	wh 0
.	rs
.	@b
.\}
..
.\"		*** INTERNAL HEADER AND FOOTER MACROS ***
.de @h			\" --- header
.if \n@>1 .tm >> @h %=\\n% ?a=\\n(?a ?b=\\n(?b ?w=\\n(?w
.if (\\n(.i+\\n(.o)>=\\n(.l \
.	tm Line \\n(c. -- Offset + indent exceeds line length
.\" initialize a pile of junk
.nr ?h \\n(?H			\" transfer "next page" to "this page"
.nr ?H 0
.nr ?c \\n(?C
.nr ?C 0
.rn |4 |0
.rn |5 |1
.rn |6 |2
.rn |7 |3
.nr _w 0			\" reset max footnote width
.nr ?W 0			\" no wide floats this page (yet)
.nr ?I 1
.\" begin actual header stuff
.ev 2
.rs
.if \\n(hm>0 \
.	sp |\\n(hmu		\" move to header position
.@t $h				\" output header title
.if \\n(tm<=0 \
.	nr tm \n(.Vu
.sp |\\n(tmu			\" move to top of text
.ev
.mk _k				\" for columned output
.if \\n(?n .nm 1		\" restore line numbering if n1 mode
.nr $c 1			\" set first column
.if \n@>4 .tm -- @h >> .ns nl=\\n(nl %=\\n% _k=\\n(_k tm=\\n(tm
.ie \\n(?s \
\{\
.	nr ?s 0
.	rs
'	@b
.\}
.el \
.	@n			\" begin the column
.if \n@>2 .tm << @h
..
.de @n			\" --- new column or page
.if \n@>3 .tm >> @n nl=\\n(nl %=\\n% ?f=\\n(?f ?o=\\n(?o
.if \\n(bm<=0 \
.	nr bm \\n(.Vu
.if (\\n(_w<=\\n($l)&(\\n(?W=0) \
\{\
.	nr _b (\\n(ppp*\\n($vu)/200u	\" compute fudge factor (must be < 1P)
.	if \\n(_bu>((\\n(bmu-\\n(fmu-((\\n(tpp*\\n($vu)/100u))/2u) \
.		nr _b (\\n(ppp*\\n($vu)/100u-\n(.Vu
.	nr _b +\\n(bmu
.\}
.nr _B \\n(_bu
.ch @f
.wh -\\n(_bu @f
.nr _b +(\\n(ppp*\\n($vu)/100u \" add 1 paragraph v in case of sweep past
.if \n@>2 .tm @n .p=\\n(.p bm=\\n(bm _b=\\n(_b _B=\\n(_B
.nr ?f 0			\" reset footnote flag
.if \\n(?o \
\{\
.	(f _			\" reprocess footnotes which run off page
.	nf
.	|o
.	fi
.	)f
.	rm |o
.\}
.nr ?o 0
.if \\n(?T \
\{\
.	nr _i \\n(.i
.	in \\n($iu
.	|h			\" output the table header
.	in \\n(_iu
.	rr _i
.	mk #T			\" for tbl commands
.	ns
.\}
.if (\\n(?a)&((\\n($c<2):(\\n(?w=0)) \
\{\
.	nr ?a 0			\" output floating keep
.	@k |t
.	if \\n(?w \
.		mk _k		\" don't overstrike wide keeps
.	nr ?w 0
.\}
.os
.$H				\" special column header macro
.ns
..
.de @f			\" --- footer
.if \n@>1 .tm >> @f %=\\n% nl=\\n(nl ?a=\\n(?a ?b=\\n(?b ?f=\\n(?f
.if \n@>2 .nr VL \\n(.pu-\\n(nlu
.if \n@>2 .tm @f bm=\\n(bm _B=\\n(_B _b=\\n(_b .p-nl=\\n(VL
.ec
.if \\n(?T \
\{\
.	nr T. 1			\" for tbl commands (to output bottom line)
.	T# 1			\" output the sides and bottom lines
.	br
.\}
.ev 2
.ce 0
.if \\n(?b \
\{\
.	nr ?b 0
.	@k |b\"			\" output bottom of page tables
.\}
.if \\n(?f \
.	@o			\" output footnote if present
.ie \\n($c<\\n($m \
.	@c			\" handle new column
.el \
.	@e			\" new page
.ev
.if \n@>2 .tm << @f
..
.de @o			\" --- output footnote
.nf
.ls 1
.in 0
.if \n@>2 .tm @o last printed text = \\n(nl placing @r trap at -\\n(_B
.wh -\\n(_Bu @r
.|f
.fi
.if \n@>2 .tm @o triggered @r (?o) = \\n(?o
.if \\n(?o \
\{\
.	di			\" just in case triggered @r
.	if \\n(dn=0 \
\{\
.		rm |o
.		nr ?o 0
.	\}
.	nr dn \\n(_D
.	rr _D
.\}
.rm |f
.ch @r
..
.de @c			\" --- new column
.if \n@>2 .tm	>> @c %=\\n%
.rs
.sp |\\n(_ku
.@O +\\n($lu+\\n($su
.nr $c +1
.@n
..
.de @e			\" --- end page
.if \n@>2 .tm	>> @e
.@O \\n(_ou
.rs
.sp |\\n(.pu-\\n(fmu-((\\n(tpp*\\n($vu)/100u)	\" move to footer position
.@t $f				\" output footer title
.nr ?h 0
.bp
..
.de @t			\" --- output header or footer title
.if !\\n(?h \
\{\
.	sz \\n(tp		\" set header/footer type fonts, etc.
.	@F \\n(tf
.	lt \\n(_Lu		\" make title span entire page
.	nf
.	\\$1
.	br
.\}
..
.de $h			\" $$$ print header
.ds |z
.if !\\n(?c \
\{\
.	if e .ds |z "\\*(|0
.	if o .ds |z "\\*(|1
.\}
.if !\(ts\\*(|z\(ts\(ts \
'	tl \\*(|z
.rm |z
..
.de $f			\" $$$ print footer
.ds |z
.if \\n(?c \
\{\
.	if e .ds |z "\\*(|0
.	if o .ds |z "\\*(|1
.\}
.if \(ts\\*(|z\(ts\(ts \
\{\
.	if e .ds |z "\\*(|2
.	if o .ds |z "\\*(|3
.\}
.if !\(ts\\*(|z\(ts\(ts \
'	tl \\*(|z
.rm |z
..
.de @r			\" --- reprocess overflow footnotes
.if \n@>3 .tm		>> @r .z=\\n(.z ?f=\\n(?f ?a=\\n(?a ?b=\\n(?b _b=\\n(_b
.di |o				\" save overflow footnote
.nr ?o 1
.nr _D \\n(dn
.ns
..
.\"		*** COMMANDS WITH VARIANT DEFINITIONS ***
.rn bp @b		\" --- begin page
.de bp			\" *** begin new page (overrides columns)
.nr $c \\n($m			\" force new page, not new column
.ie \\n(nl>0 \
.	@b \\$1
.el \
\{\
.	if \\n(.$>0 \
.		pn \\$1
.	if \\n(?I \
.		@h		\" 'spring' the header trap
.\}
.br
.wh 0 @h			\" reset header
..
.rn ll xl		\" *** special line length (local)
.de ll			\" *** line length (global to environments)
.xl \\$1
.lt \\$1
.nr $l \\n(.l
.if (\\n($m<=1):(\\n($l>\\n(_L) \
.	nr _L \\n(.l
..
.rn po @O		\" --- local page offset
.de po			\" *** page offset
.@O \\$1
.nr _o \\n(.o
..
.\" Redefine the fam request to set the family in
.\" environment 2 as well as the current environment.
.if !\n(.g .ig
.do rn fam @fam		\" --- set family in current environment
.do de fam		\" *** set font family in ev 2 and current ev
.do @fam \\$1
.ev 2
.do @fam \\$1
.ev
..
.\"		*** MISCELLANEOUS ROFF COMMANDS ***
.de hx			\" *** suppress headers and footers next page
.nr ?H 1
..
.de ix			\" *** indent, no break
'in \\$1
..
.de bl			\" *** contiguous blank lines
.br
.ne \\$1
.rs
.sp \\$1
..
.de n1			\" *** line numbering 1
.nm 1
.xl -\w'0000'u
.nr ?n 1
..
.de n2			\" *** line numbering 2
.nm \\$1
.ie \\n(.$ \
.	xl -\w'0000'u
.el \
.	xl \\n($lu
..
.de pa			\" *** new page
.bp \\$1
..
.de ro			\" *** roman page numbers
.af % i
..
.de ar			\" *** arabic page numbers
.af % 1
..
.de m1			\" *** position one space
.nr _0 \\n(hmu
.nr hm \\$1v
.nr tm +\\n(hmu-\\n(_0u
.rr _0
..
.de m2			\" *** position two space
.nr tm \\n(hmu+\\n(tpp+\\$1v
..
.de m3			\" *** position three space
.nr bm \\n(fmu+\\n(tpp+\\$1v
..
.de m4			\" *** position four space
.nr _0 \\n(fmu
.nr fm \\$1v
.nr bm +\\n(fmu-\\n(_0u
..
.de sk			\" *** leave a blank page (next page)
.if \\n(.$>0 \
.	tm Line \\n(c. -- I cannot skip multiple pages
.nr ?s 1
..
.\"		*** MISCELLANEOUS USER SUPPORT COMMANDS ***
.if !\n(.g .ig
.de re			\" *** reset tabs (TROFF defines 15 stops default)
.ta T 0.5i
..
.if \n(.g .ig
.de re
.ta 0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i +0.5i
..
.de ba			\" *** set base indent
.ie \\n(.$ \
.	nr $i \\$1n
.el \
.	nr $i \\n(siu*\\n($0u
..
.de hl			\" *** draw horizontal line
.br
.ie \n(.g .do nr _I \\n[.in]
.el .nr _I \\n(.i
\l'\\n(.lu-\\n(_Iu'
.sp
..
.\"		*** PARAGRAPHING ***
.de pp			\" *** paragraph
.lp \\n(piu
..
.de lp			\" *** left aligned paragraph
.@p
.if \\n(.$ \
.	ti +\\$1
.nr $p 0 1
..
.de ip			\" *** indented paragraph w/ optional tag
.if (\\n(ii>0)&(\\n(ii<1n) \
.	nr ii \\n(iin
.nr _0 \\n(ii
.if \\n(.$>1 \
.	nr _0 \\$2n
.@p \\n(_0u
.if \\w"\\$1" \
\{\
.	ti -\\n(_0u
.	ie \\w"\\$1">=\\n(_0 \
\{\
\&\\$1
.		br
.	\}
.	el \&\\$1\h'|\\n(_0u'\c
.\}
.rr _0
..
.de np			\" *** numbered paragraph
.\" use string comparison in case user has changed format of $p
.if '\\n($p'-1' \
.	nr $p 0			\" reset number after .bu
.nr $p +1			\" increment paragraph number
.@p \w'\0(000)\0'u
.ti -\w'\0(000)\0'u
\0(\\n($p)\h'|\w'\0(000)\0'u'\c
..
.de bu			\" *** bulleted paragraph
.br
.\" use string comparison in case user has changed format of $p
.if '\\n($p'-1' \
.	ns			\" don't space between .bu paragraphs
.nr $p 0-1			\" mark "bulleted paragraph" mode
.@p \w'\0\(bu\0'u
.ti -\w'\0\(bu\0'u
\0\(bu\0\c
..
.de @p			\" --- initialize for paragraph
.if "\\n(.z"|e" .tm Line \\n(c. -- Unmatched continued equation
.in \\n($iu+\\n(pou
.if \\n(.$ \
.	in +\\$1n
.ce 0
.fi
.@F \\n(pf
.sz \\n(pp
.sp \\n(psu
.ne \\n(.Lv+\\n(.Vu
.ns
..
.\"		*** SECTION HEADINGS ***
.de sh			\" *** section heading
.fi
.if (\\n(si>0)&(\\n(si<1n) \
.	nr si \\n(sin
.ce 0
.@d "\\$1" +1 \\$3 \\$4 \\$5 \\$6 \\$7 \\$8
.if !"\\$2"_" \
\{\
.	ds |n \&\\$2
.	$p "\\*(|n" "\\*($n" \\n($0
.	$0 "\\*(|n" "\\*($n" \\n($0
.	rm |n
.\}
.nr $p 0 1			\" reset .np count
..
.de @d			\" --- change section depth
.if !""\\$1" \
.	nr $0 \\$1
.if \\n($0&(\\n(.$>1) \
.	nr $\\n($0 \\$2
.ds $n \&\"
.ie \\n($0>=1 \
\{\
.	if '\\n($1'0' \
.		nr $1 1
.	if (\\n(.$>=3) .if !"\\$3"-" \
.		nr $1 \\$3
.	as $n \\n($1
.\}
.el \
.	nr $1 0
.ie \\n($0>=2 \
\{\
.	if '\\n($2'0' \
.		nr $2 1
.	if (\\n(.$>=4) .if !"\\$4"-" \
.		nr $2 \\$4
.	as $n .\\n($2
.\}
.el \
.	nr $2 0
.ie \\n($0>=3 \
\{\
.	if '\\n($3'0' \
.		nr $3 1
.	if (\\n(.$>=5) .if !"\\$5"-" \
.		nr $3 \\$5
.	as $n .\\n($3
.\}
.el \
.	nr $3 0
.ie \\n($0>=4 \
\{\
.	if '\\n($4'0' \
.		nr $4 1
.	if (\\n(.$>=6) .if !"\\$6"-" \
.		nr $4 \\$6
.	as $n .\\n($4
.\}
.el \
.	nr $4 0
.ie \\n($0>=5 \
\{\
.	if '\\n($5'0' \
.		nr $5 1
.	if (\\n(.$>=7) .if !"\\$7"-" \
.		nr $5 \\$7
.	as $n .\\n($5
.\}
.el \
.	nr $5 0
.ie \\n($0>=6 \
\{\
.	if '\\n($6'0' \
.		nr $6 1
.	if (\\n(.$>=8) .if !"\\$8"-" \
.		nr $6 \\$8
.	as $n .\\n($6
.\}
.el \
.	nr $6 0
..
.de sx			\" *** heading up, no increment (2.1.1 -> 2.1)
.ce 0
.ul 0
.nr _0 \\n($0-1
.if \\n(.$ .nr _0 +1
.if \\n(.$ .nr _0 \\$1
.@d \\n(_0
.rr _0
.$p "" "" \\n($0
.nr $p 0 1			\" reset .np count
..
.de uh			\" *** unnumbered section heading
.$p "\\$1"
.$0 "\\$1"
..
.de $p			\" $$$ print section heading
.if (\\n(si>0)&(\\n(.$>2) \
.	nr $i \\$3*\\n(si
.in \\n($iu
.ie !"\\$1\\$2"" \
\{\
.	sp \\n(ssu 		\" one of them is non-null
.	ne \\n(.Lv+\\n(.Vu+\\n(psu+((\\n(spp*\\n($vu*\\n(.Lu)/100u)
.	\" exdent if \\$3 > 0
.	ie 0\\$3 \
.		ti -(\\n(siu-\\n(sou)
.	el \
.		ti +\\n(sou
.	@F \\n(sf
.	sz \\n(sp
.	if 0\\$3 \
.		$\\$3
.	if \w"\\$2">0 \\$2.
.	if \w"\\$1">0 \\$1\f1\ \ \&
.\}
.el \
.	sp \\n(psu
.@F \\n(pf
.sz \\n(pp
..
.\"		*** COLUMNNED OUTPUT ***
.de 2c			\" *** double columned output
.br
.if \\n($m>1 \
.	1c			\" revert to 1c if already 2c
.nr $c 1
.nr $m 2
.if \\n(.$>1 \
.	nr $m \\$2
.if \\n(.$>0 \
.	nr $s \\$1n		\" param 1: column seperation
.nr $l (\\n(.l-((\\n($m-1)*\\n($s))/\\n($m
.xl \\n($lu
.mk _k
.ns
..
.de 1c			\" *** single columned output
.br
.nr $c 1
.nr $m 1
.ll \\n(_Lu			\" return to normal output
.sp |\\n(.hu
.@O \\n(_ou
..
.de bc			\" *** begin column
.sp 24i
..
.\"		*** FLOATING TABLES AND NONFLOATING BLOCKS ***
.de (z			\" *** begin floating keep
.if \n@>4 .tm >> (z, .z=\n(.z
.@D 4 \\$1 \\$2
.@(
..
.de )z			\" *** end floating keep
.if \n@>4 .tm >> )z, .z=\n(.z
.sp \\n(zsu
.@)
.if \n@>4 .tm -- )z << @), .z=\n(.z
.rr _0
.if !\\n(?b \
.	nr dn +(\\n(ppp*\\n($vu)/200u+\\n(zsu
.nr dl -\n(.H			\" fudge factor necessary to make it work
.ie ((\\n(dn+\n(.V)>=\\n(.t):(\\n(?a):((\\n(dl>\\n($l)&(\\n($c>1)) \
\{\
.	nr ?a 1
.	if (\\n(dl>\\n($l)&(\\n($m>1) \
.		nr ?w 1		\" mark wider than one column (top)
.	ds |x |t
.\}
.el \
\{\
.	nr ?b 1
.	if (\\n(dl>\\n($l)&(\\n($m>1) \
.		nr ?W 1		\" mark wider than one column (bottom)
.	nr _b +\\n(dnu
.	\" avoid moving @f back past the current position
.	if \\n(.p-\\n(nl-\n(.V<\\n(_b \
.		nr _b \\n(.p-\\n(nl-\n(.V
.	ch @f -\\n(_bu
.	ds |x |b
.\}
.da \\*(|x			\" copy to save macro
.nf
.ls 1
.nr ?k 1
.if \n@>4 .tm -- )z >> \\*(|x
\!.if \\\\n(nl>(\\\\n(tm+2v) .ne \\n(dnu-\\n(zsu
.|k\"				\" and the body
.ec
.if \n@>4 .tm -- )z << \\*(|x, .z=\\n(.z
.nr ?k 0
.rm |k\"			\" remove the temp macro
.da
.in 0
.ls 1
.xl \\n($lu
.ev
.if \n@>4 .tm << )z, .z=\\n(.z
..
.de @k			\" --- output floating keep
.if \n@>4 .tm >> @k, $1=\\$1, .z=\\n(.z
.ev 1
.nf
.ls 1
.in 0
.sp \\n(zsu
.\\$1
.ec
.br
.rm \\$1
.ev
..
.de (t			\" XXX temp ref to (z
.(z \\$1 \\$2
..
.de )t			\" XXX temp ref to )t
.)z \\$1 \\$2
..
.de (b			\" *** begin block
.br
.@D 3 \\$1 \\$2
.sp \\n(bsu
.@(
..
.de )b			\" *** end block
.br
.@)
.if (\\n(bt=0):(\\n(.t<\\n(bt) \
.	ne \\n(dnu		\" make it all on one page
.ls 1
.nf
.|k
.ec
.fi
.in 0
.xl \\n($lu
.ev
.rm |k
.sp \\n(bsu+\\n(.Lv-1v
..
.de @(			\" --- begin keep
.if !"\\n(.z"" .tm Line \\n(c. -- Illegal nested keep \\n(.z
.@M
.di |k
\!'rs
..
.de @M			\" --- set modes for display
.nr ?k 1
.@C 1
.@F \\n(df
.if \\n($R .@V
.vs \\n(.sp*\\n($Vu/100u
.nf
.if "\\*(|p"F" \
.	fi			\" set fill mode if "F" parameter
.if \\n(_d=4 \
.	in 0
.if \\n(_d=3 \
\{\
.	in +\\n(biu
.	xl -\\n(biu
.\}
.if \\n(_d=1 \
.	ce 10000
..
.de @)			\" --- end keep
.br
.if !"\\n(.z"|k" .tm Line \\n(c. -- Close of a keep which has never been opened
.nr ?k 0
.di
.in 0
.ce 0
..
.de (c			\" *** begin block centered text
.if "\\n(.z"|c" .tm Line \\n(c. -- Nested .(c requests
.di |c
..
.de )c			\" *** end block centered text
.if !"\\n(.z"|c" .tm Line \\n(c. -- Unmatched .)c
.br				\" force out final line
.di
.if \n@>4 .tm >> .)c .l=\\n(.l .i=\\n(.i $i=\\n($i dl=\\n(dl
.ev 1
.ls 1
.in (\\n(.lu-\\n(.iu-\\n(dlu)/2u
.if \n@>4 .tm -- .)c << .in .l=\\n(.l .i=\\n(.i dl=\\n(dl
.nf
.|c
.ec
.in
.ls
.ev
.rm |c
..
.\"		*** BLOCK QUOTES (OR WHATEVER) AND LISTS ***
.de (q			\" *** begin block quote
.br
.@C 1
.fi
.sp \\n(qsu
.in +\\n(qiu
.xl -\\n(qiu
.sz \\n(qp
..
.de )q			\" *** end block quote
.br
.ev
.sp \\n(qsu+\\n(.Lv-1v
.nr ?k 0
..
.de (l			\" *** begin list
.br
.sp \\n(bsu
.@D 3 \\$1 \\$2
.@M
..
.de )l			\" *** end list
.br
.ev
.sp \\n(bsu+\\n(.Lv-1v
.nr ?k 0
..
.\"		*** PREPROCESSOR SUPPORT ***
.\"
.\"	EQN
.\"
.de EQ			\" *** equation start
.ec
.if !\\n(?e \
\{\
.	if "\\n(.z"|e" .tm Line \\n(c. -- Nested .EQ request
.	@D 1 "\\$1" "\\$2"
.	@C 2
.	di |e
.\}
.ls 1
.in 0
.nf
..
.de EN			\" *** equation end
.br
.ie "\\$1"C" \
\{\
.	nr ?e 1
.	sp \\n(esu
.\}
.el \
\{\
.	nr ?e 0
.	di
.	if \\n(dn \
.		@q		\" actual equation output
.	rm |e
.	ev
.\}
..
.de @q			\" --- equation output
.nr _Q \\n(dnu
.ev
.sp \\n(esu			\" output rest of preceeding text
.if !"\\n(.z"" \!.ne \\n(_Qu
.ne \\n(_Qu+\n(.Vu		\" keep it on one page
.@C 2				\" .ev 2 may be jumbled from header
.if \\n(_d=1 \
.	in (\\n(.lu+\\n($iu-\\n(dlu)/2u
.if \\n(_d=2 \
.	in \\n($iu
.if \\n(_d=3 \
.	in \\n(biu+\\n($iu
.if \\n(_d=4 \
.	in 0
.mk _q
.if \n@>1 .tm --@e: _Q=\\n(_Q _q=\\n(_q nl=\\n(nl |p=\\*(|p
.if !"\\*(|p"" \
\{\
.	rs
.	sp (\\n(_Qu-\\n(.vu)/2u
.	tl """\\*(|p"
.	rt \\n(_qu
.\}
.|e
.sp |\\n(_qu+\\n(_Qu
.sp \\n(esu+\\n(.Lv-1v
.rr _q
.rr _Q
..
.\"
.\"	TBL
.\"
.de TS			\" *** table start
.sp \\n(bsu
.@C 1
.fi				\" drop into fill mode for text boxes
.if "\\$1"H" \
\{\
.	di |h			\" save header part
.	nr ?T 1
.\}
.ls 1
.ch @f -(\\n(_bu+1v)		\" set pseudo-trap for bottom line
.if \\n(.p-\\n(_b-1v<=\\n(nl \
.	ch @f \\n(nlu+\n(.Vu
..
.de TH			\" *** end header part of table
.nr T. 0
.T# 0
.di
.nr _T \\n(?T
.nr ?T 0
.ne \\n(dnu+1v
.nr ?T \\n(_T
.nr _i \\n(.i
.in 0
.|h				\" put in the initial header
.in \\n(_iu
.rr _i
.mk #T
..
.de TE			\" *** table end
.nr ?T 0
.ch @f -\\n(_bu			\" reset pseudo-trap
.if \\n(.p-\\n(_b<=\\n(nl \
.	ch @f \\n(nlu+\n(.Vu
.ev
.sp \\n(bsu+\\n(.Lv-1v
.re
..
.\"
.\"	REFER
.\"
.de ][				\" *** refer output
.if \\$1>5 .tm Bad arg to []
.[\\$1
..
.de [0				\" --- other
.(f
.ip "\\*([F.\0"
.if !"\\*([A"" \\*([A,
.if !"\\*([T"" \\f2\\*([T\\f1\c
.if !"\\*([T"" .if !"\\*([O"" ,\ 
.ie !"\\*([O"" \\*([O
.el .if !"\\*([T"" \&.
.if !"\\*([D"" \\*([D.
.@p
.)f
..
.de [1				\" --- journal article
.(f
.ip "\\*([F.\0"
\\*([A,
.if !"\\*([T"" \\*(lq\\*([T,\\*(rq
.if "\\*([V"" \\f2\\*([J\\f1,
.if !"\\*([V"" \\f2\\*([J\\f1
.if !"\\*([V"" \\f3\\*([V\\f1\c
.if !"\\*([N"" (\\*([N)\c
.if !"\\*([P"" \
\{\
.	ie \\n([P>0 \ pp.\&
.	el \ p.\&
\\*([P
.\}
.if !"\\*([I"" .if "\\*([R"" \\*([I,
(\\*([D).
.if !"\\*([O"" \\*([O
.)f
..
.de [2				\" --- book
.(f
.ip "\\*([F.\0"
\\*([A, \\f2\\*([T,\\f1
\\*([I\c
.if !"\\*([C"" , \\*([C\c
 (\\*([D).
.if !"\\*([G"" Gov't. ordering no. \\*([G
.if !"\\*([O"" \\*([O
.)f
..
.de [3				\" --- article in book
.(f
.ip "\\*([F.\0"
\\*([A, \\*(lq\\*([T,\\*(rq
.if !"\\*([P"" pp. \\*([P
in \\f2\\*([B\\f1, \c
.if !"\\*([E"" ed. \\*([E, \c
.if !"\\*([I"" \\*([I\c
.if !"\\*([C"" , \\*([C\c
 (\\*([D).
.if !"\\*([O"" \\*([O
.)f
..
.de [4				\" --- report
.(f
.ip "\\*([F.\0"
\\*([A, \\*(lq\\*([T,\\*(rq
\\*([R\c
.if !"\\*([G"" \& (\\*([G)\c
.if !"\\*([I"" , \\*([I\c
.if !"\\*([C"" , \\*([C\c
 (\\*([D).
.if !"\\*([O"" \\*([O
.)f
..
.de [5				\" --- tm style
.(f
.ip "\\*([F.\0"
\\*([A, \\f2\\*([T\\f1,
.ie \\n(TN \\*([M.
.el Bell Laboratories internal memorandum (\\*([D).
.)f
..
.de ]<
.$p References
.lp
.rm (f )f
..
.de ]>
.sp
..
.de ]-
.rm [V [P [A [T [N [C [B [O [R [I [E [D
..
.ie \n(.V<1v \
\{\
.	ds [. \s-2\v'-.4m'\f1
.	ds .] \v'.4m'\s+2\fP
.\}
.el \
\{\
.	ds [. " [
.	ds .] ]
.\}
.\"
.\"	IDEAL
.\"
.de IS			\" *** start ideal picture
.nr g7 \\n(.u
.ls 1
..
.de IF
.if \\n(g7 .fi
.ls
..
.de IE			\" *** end ideal picture
.if \\n(g7 .fi
.ls
..
.\"
.\"	PIC
.\"
.de PS		\" *** start picture: $1=height, $2=width in units or inches
.sp 0.3
.nr g7 \\$2
.in (\\n(.lu-\\n(g7u)/2u
.ne \\$1u
.nr g7 \\n(.u
.ls 1
..
.de PE			\" *** end picture
.ls
.in
.if \\n(g7 .fi
.sp .6
..
.\"
.\"	GREMLIN
.\"
.de GS			\" *** start gremlin picture
.nr g7 (\\n(.lu-\\n(g1u)/2u
.if "\\$1"L" .nr g7 \\n(.iu
.if "\\$1"R" .nr g7 \\n(.lu-\\n(g1u
.in \\n(g7u
.nr g7 \\n(.u
.ls 1
.nf
.ne \\n(g2u
..
.de GE			\" *** end gremlin picture
.GF
.sp .6
..
.de GF			\" *** finish gremlin picture; stay at top
.ls
.in
.if \\n(g7 .fi
..
.\"		*** FONT AIDS ***
.de sz			\" *** set point size and vertical spacing
.ps \\$1
.if \\n($r .@v
.vs \\n(.sp*\\n($vu/100u	\" default vs at pointsize + 20%
..
.de @v			\" --- possibly set $v from $r
.if (1i>=240u)&(1p<=\\n($r)&(\\n($r<=4p) .nr $v \\n($r00/1p
..
.de @V			\" --- possibly set $V from $R
.if (1i>=240u)&(1p<=\\n($R)&(\\n($R<=4p) .nr $V \\n($R00/1p
..
.de @E			\" --- store in _F argument to \f for restoring font
.ie \\n(.f<10 \
.	ds _F \\n(.f
.el \
\{\
.	ie \\n(.f<100&\n(.g \
.		ds _F (\\n(.f
.	el \
.		ds _F P
.\}
..
.de r			\" *** enter roman font
.@E
.ft 1
.if \\n(.$ \&\\$1\f\\*(_F\\$2
..
.de i			\" *** enter italic
.@E
.ft 2
.if \\n(.$ \&\\$1\f\\*(_F\\$2
..
.de b			\" *** enter boldface
.@E
.ft 3
.if \\n(.$ \&\\$1\f\\*(_F\\$2
..
.de rb			\" *** enter real boldface
.@E
.ft 3
.if \\n(.$ \&\\$1\f\\*(_F\\$2
..
.de bi			\" *** enter bold italic
.@E
.ft 4
.if \\n(.$ \&\\$1\f\\*(_F\\$2
..
.de u			\" *** enter underlined word
\&\\$1\l'|0\(ul'\\$2
..
.\" a better version of u
.if !\n(.g .ig
.de u
\Z'\\$1'\v'.25m'\D'l \w'\\$1'u 0'\v'-.25m'\\$2
..
.de q			\" *** enter quoted word
\&\\*(lq\\$1\\*(rq\\$2
..
.de bx			\" *** enter boxed word
\k~\(br\|\\$1\|\(br\l'|\\n~u\(rn'\l'|\\n~u\(ul'\^\\$2
..
.de sm			\" *** print in smaller font
\s-1\\$1\\s0\\$2
..
.de @F			\" --- change font (0 -> no change)
.nr ~ \\$1
.if \\n~>0 \
.	ft \\n~
.rr ~
..
.\"		*** FOOTNOTING ***
.de (f			\" *** begin footnote
.ec
.if "\\n(.z"|f" .tm Line \\n(c. -- Illegal footnote nesting
.ie "\\n(.z"" \
\{\
.	nr _D \\n(dn
.	nr _0 1v+\\n(nlu
.	ev 2
.	da |f
.	in 0
.	xl \\n($lu-\\n(fuu
.	@F \\n(ff
.	sz \\n(fp
.	vs \\n(.sp*\\n($Vu/100u
.	if !\\n(?f \
\{\
.		nr _b +1v	\" allow space for $s
.		$s
.	\}
.	br
.	if \\n(.p-\\n(_b-\\n(_0-\\n(.h-1v-\\n(fs<0 \
\{\
.		da\"		\" won't fit on page at all
.		bc
.		if !\\n(?f \
.			rm |f
.		da |f
.\" next five lines could be dropped if headers had their own environment
.		in 0		\" reset everything from .bc
.		xl \\n($lu-\\n(fuu
.		@F \\n(ff
.		sz \\n(fp
.		vs \\n(.sp*\\n($Vu/100u
.		if !\\n(?f \
.			$s
.		br
.	\}
.	rr _0
.	sp \\n(fsu
.	nr ?f 1
.	fi
.	if !"\\$1"_" \
.		ti \\n(fiu
.	if \n@>2 .tm	<< (f $f=\\n($f
.\}
.el \
\{\
.	ev 2
.	in 0
.	xl \\n($lu-\\n(fuu
.	@F \\n(ff
.	sz \\n(fp
.	vs \\n(.sp*\\n($Vu/100u
.	fi
\!.(f \\$1
\!.@N
.\}
..
.de @N			\" --- set no fill mode in the top-level diversion
.ie "\\n(.z"" .nf
.el \!.@N
..
.de )f			\" *** end footnote
.ie "\\n(.z"|f" \
\{\
.	if \\n* \
.		nr $f +1
.	ds * \\*{\\n($f\\*}\k*
.	nr * 0
.	in 0
.	da
.	ev
.	if \\n(_w<\\n(dl \
.		nr _w \\n(dl	\" compute maximum fn width
.	nr _b +\\n(dn
.	ch @f -\\n(_bu
.	if \\n(.p-\\n(_b<=\\n(nl \
.		ch @f \\n(nlu+\n(.Vu
.	nr dn \\n(_D
.	rr _D
.\}
.el \
\{\
.	br
\!.)f
.	ev
.\}
..
.@R ff
.if \n(ff<=0 \
.	nr ff 1			\" footnote font: Times Roman
.@R fp
.if \n(fp<=0 \
.	nr fp 8			\" footnote pointsize
.de $s			\" $$$ footnote separator
\l'2i'
..
.\"		*** DELAYED TEXT ***
.de (d			\" *** begin delayed text
.am |d )d
.sp \\n(bsu
..
.de )d			\" *** end delayed text
.if \\n# \
.	nr $d +1
.ds # [\\n($d]\k#
.rr #
..
.de pd			\" *** print delayed text
.|d
.rm |d
.nr $d 1 1
.ds # [1]\k#
..
.\"		*** INDEXES (TABLE OF CONTENTS) ***
.nr _x 0 1
.af _x a
.de (x			\" *** begin index entry
.if \n@>4 .tm >> (x, .z=\\n(.z
.ds |X x
.if \\n(.$>0 \
.	ds |X \\$1
.ie "\\n(.z"" \
.	nr _z 0
.el \
.	nr _z 1
.@\\n(_z
..
.de @0			\" --- finish (x if no current diversion
.am %\\*(|X )x
.sp \\n(xsu
.ti -\\n(piu
..
.de @1			\" --- finish (x if current diversion
.if "\\n(_x"z" .nr _x 0
.de =\\n+(_x )x
..
.de )x			\" *** end index entry
.if \n@>4 .tm >> )x, .z=\\n(.z
.ie "\\n(.z"" \
\{\
.	ds |x \\n%
.	if \\n(.$>0 \
.		ds |x \\$1
.	if "\\*(|x"_" \
.		ig ..
.	am %\\*(|X ..
.	if \w"\\$2">(\\n(.l-\\n(.i-\\n(.k) \
.		ti +\\n(xuu
\\\\a\\\\t\\$2\\*(|x
...
.	rm |x
.	rm |X
.\}
.el \
\{\
\!.(x \\*(|X
\!\\\\*(=\\n(_x\\\\
\!.)x \\$1 \\$2
\!.rm =\\n(_x
.\}
..
.de xp			\" *** print the index
.br
.@C 2
.ls 1
.vs \\n(.sp*\\n($Vu/100u
.fi
.in +\\n(piu
.ds |X x
.if \\n(.$>0 \
.	ds |X \\$1
.xl -(\\n(xuu+\w'...'u)
.di |x
.%\\*(|X
.br
.di
.rm %\\*(|X
.xl \\n($lu
.rm |X
.ev
.nf
.in 0
.ta \\n(.lu-\\n(xuuR \\n(.luR
.|x
.fi
.in
.rm |x
..
.de +c			\" *** begin chapter
.ep				\" force out footnotes
.if \\n(?o:\\n(?a \
\{\
.	bp			\" force out a table or more footnote
.	rs
.	ep
.\}
.nr ?C 1
.nr $f 1 1
.ds * \\*{1\\*}\k*
.if \\n(?R \
.	pn 1
.bp
.in \\n($iu			\" reset the indent
.rs
.ie \\n(.$ \
.	$c "\\$1"
.el \
.	sp 3
..
.de ++			\" *** declare chapter type
.nr _0 0
.if "\\$1"C" \
.	nr _0 1			\" chapter
.if "\\$1"RC" \
.	nr _0 11		\" renumbered chapter
.if "\\$1"A" \
.	nr _0 2			\" appendix
.if "\\$1"RA" \
.	nr _0 12		\" renumbered appendix
.if "\\$1"P" \
.	nr _0 3			\" preliminary material
.if "\\$1"B" \
.	nr _0 4			\" bibliographic material
.if "\\$1"AB" \
.	nr _0 5			\" abstract
.if \\n(_0=0 \
.	tm Line \\n(c. -- Bad mode to .++
.nr ?R 0
.if \\n(_0>10 \
.\{
.	nr ?R 1
.	nr _0 -10
.\}
.nr ch 0 1
.if (\\n(_0=3):(\\n(_0=5) \
.	pn 1			\" must do before .ep
.if !\\n(_0=\\n(_M .if \\n(_M=3 \
.	pn 1			\" must do before .ep
.ep				\" end page for correct page number types
.if \\n(_0=1 \
\{\
.	af ch 1
.	af % 1
.\}
.if \\n(_0=2 \
\{\
.	af ch A
.	af % 1
.\}
.if \\n(_0=3 \
.	af % i
.if \\n(_0=4 \
.	af % 1
.if \\n(_0=5 \
.	af % 1
.if \\n(.$>1 \
.	he \\$2
.nr _M \\n(_0
.rr _0
..
.de $c			\" $$$ print chapter title
.sz 12
.ft 3
.ce 1000
.if \\n(_M<3 \
.	nr ch +1
.ie \\n(_M=1 CHAPTER\ \ \\n(ch
.el .if \\n(_M=2 APPENDIX\ \ \\n(ch
.if \w"\\$1" .sp 3-\\n(.L
.if \w"\\$1" \\$1
.if (\\n(_M<3):(\w"\\$1") \
.	sp 4-\\n(.L
.ce 0
.ft
.sz
.ie \\n(_M=1 \
.	$C Chapter \\n(ch "\\$1"
.el .if \\n(_M=2 \
.	$C Appendix \\n(ch "\\$1"
..
.de tp			\" *** title page
.hx
.bp
.br
.rs
.pn \\n%
..
.\"		*** DATES ***
.if \n(mo=1 .ds mo January
.if \n(mo=2 .ds mo February
.if \n(mo=3 .ds mo March
.if \n(mo=4 .ds mo April
.if \n(mo=5 .ds mo May
.if \n(mo=6 .ds mo June
.if \n(mo=7 .ds mo July
.if \n(mo=8 .ds mo August
.if \n(mo=9 .ds mo September
.if \n(mo=10 .ds mo October
.if \n(mo=11 .ds mo November
.if \n(mo=12 .ds mo December
.if \n(dw=1 .ds dw Sunday
.if \n(dw=2 .ds dw Monday
.if \n(dw=3 .ds dw Tuesday
.if \n(dw=4 .ds dw Wednesday
.if \n(dw=5 .ds dw Thursday
.if \n(dw=6 .ds dw Friday
.if \n(dw=7 .ds dw Saturday
.ds td \*(mo \n(dy, 19\n(yr
.\"		*** PARAMETRIC INITIALIZATIONS ***
.rr x
.nr $v \n(.v00+\n(.sp-1/\n(.sp	\" vs as percentage of ps for .sz request
.nr $V \n($v			\" same for displays & footnotes
.nr hm 4v			\" header margin
.nr tm 7v			\" top margin
.nr bm 6v			\" bottom margin
.nr fm 3v			\" footer margin
.nr tf 3			\" title font: (real) Times Bold
.nr tp 10			\" title point size
.hy 14
.nr bi 4m			\" indent for blocks
.nr pi 5n			\" indent for paragraphs
.nr pf 1			\" normal text font
.nr pp 10			\" normal text point size
.nr qi 4n			\" indent for quotes
.nr qp -1			\" down one point
.nr ii 5n			\" indent for .ip's and .np's
.nr $m 1			\" max number of columns
.nr $s 4n			\" column separation
.nr sf 3			\" section font -- Times Bold
.nr sp 10			\" section title pointsize
.nr ss 12p			\" section prespacing
.nr si 0			\" section indent
.\"		*** OTHER INITIALIZATION ***
.\" GNU pic sets this register to 1, to indicate that \x should not be used.
.@R 0x
.ds { \v'-0.4m'\x'\\n(0x=0*-0.2m'\s-3
.ds } \s0\v'0.4m'
.\" for compatibility with traditional -me
.\" (the first will work only in compatibility mode)
.ds [ \*{
.ds ] \*}
.ds < \v'0.4m'\x'\\n(0x=0*0.2m'\s-3
.ds > \s0\v'-0.4m'
.ds - \(em
.\" Avoid warnings from groff -ww.
.@S |0
.@S |1
.@S |2
.@S |3
.@S $H
.@S $0
.@S $1
.@S $2
.@S $3
.@S $4
.@S $5
.@S $6
.@S $7
.@S $8
.@S $9
.@S ..
.@R po\"			\" simulated page offset
.@R $0\"			\" section depth
.@R $i\"			\" paragraph base indent
.@R $p\"			\" numbered paragraph number
.@R $r\"			\" ratio of vs to ps (may override $v)
.@R $R\"			\" same for displays (may override $V)
.@R df\"			\" display font: same as surrounding text
.@R so\"			\" additional section title offset
.@R fu\"			\" footnote undent
.@R bt\"			\" block keep threshold
.@R *\"				\" has \** been referenced?
.@R ?a\"			\" pending floating keep at page top?
.@R ?b\"			\" pending floating keep at page bottom?
.@R ?C\"			\" at chapter header?
.@R ?e\"			\" in equation?
.@R ?H\"			\" suppress headers and footers next page?
.@R ?I\"			\" has the header trap been sprung?
.@R ?n\"			\" n1 mode?
.@R ?o\"			\" footnote overflow?
.@R ?R\"			\" renumbered chapter?
.@R ?s\"			\" skip next page?
.@R ?T\"			\" inside .TS H?
.@R ?W\"			\" wide floating keep at page bottom?
.@R ?w\"			\" wide floating keep at page top?
.nr fi 0.3i
.nr _o \n(.o
.nr $b 3			\" bold
.nr ps 0.35v
.if \n(ps<\n(.V .nr ps \n(.V
.nr bs \n(ps			\" block pre/post spacing
.nr qs \n(ps			\" quote pre/post spacing
.nr zs 1v			\" float-block pre/postspacing
.nr xs 0.2v			\" index prespacing
.nr xu 0.5i			\" index undent
.nr fs 0.2v			\" footnote prespacing
.nr es 0.5v			\" equation pre/postspacing
.if \n(es<\n(.V .nr es \n(.V
.wh 0 @h			\" set header
.nr $l \n(.lu			\" line length
.nr _L \n(.lu			\" line length of page
.nr $c 1			\" current column number
.nr $f 1 1			\" footnote number
.ds * \*{1\*}\k*\"		\" footnote "name"
.nr $d 1 1			\" delayed text number
.ds # [1]\k#\"			\" delayed text "name"
.nr _M 1			\" chapter mode is chapter
.ds lq \(lq\"			\" left quote
.ds rq \(rq\"			\" right quote
.em @z
.\"		*** FOREIGN LETTERS AND SPECIAL CHARACTERS ***
.ds #h ((1u-(\\\\n(.fu%2u))*0.13m)
.ds #v 0.6m
.\"			\" accents
.ds ' \k_\h'-(\\n(.wu*8/10-\*(#h)'\(aa\h'|\\n_u'
.ds ` \k_\h'-(\\n(.wu*7/10-\*(#h)'\(ga\h'|\\n_u'
.\"			\" umlaut
.ds : \k_\h'-(\\n(.wu*8/10-\*(#h+0.1m)'\v'-\*(#v'\z.\h'0.2m'.\h'|\\n_u'\v'\*(#v'
.\"			\" circumflex and tilde
.ds ^ \k_\h'-(\\n(.wu-\*(#h-0.05m)'^\h'|\\n_u'
.ds ~ \k_\h'-(\\n(.wu-\*(#h-0.05m)'~\h'|\\n_u'
.\"			\" cedilla and czech
.ds , \k_\h'-(\\n(.wu)',\h'|\\n_u'
.ds v \k_\h'-(\\n(.wu*9/10-\*(#h)'\v'-\*(#v'\s-4v\s0\v'\*(#v'\h'|\\n_u'
.\"			\" Norwegian A or angstrom
.ds o \k_\h'-(\\n(.wu+\w'\(de'u-\*(#h)/2u'\v'-0.4n'\z\(de\v'0.4n'\h'|\\n_u'
.\"			\" there exists, for all
.ds qe \s-2\v'0.45m'\z\(em\v'-0.625m'\z\(em\v'-0.625m'\(em\v'0.8m'\s0\h'-0.1m'\v'-0.05m'\(br\v'0.05m'\h'0.1m'
.ds qa \z\e\h'0.35m'\z\(sl\h'-0.33m'\v'-0.3m'\s-4\(em\s0\v'0.3m'\h'0.15m'
.rm #h #v
.ll 6.0i
.lt 6.0i
.de @U
.tm The \\$1 macro has been removed from this version of the -me macros.
..
.de lo
.@U lo
..
.de th
.@U th
..
.de ac
.@U ac
..
