
(*$S-*)		(* Supress non-standard warnings *)

{!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!}
{ Please search for				}
{						}
{	version :=				}
{						}
{ and modify the version date when mods		}
{ are made.					}
{!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!}

PROGRAM pat(INPUT,OUTPUT,INFILE,INCLFILE,OUTFILE,OUTPAS,TERPAS);

    {***********************************************}
    {	    pal					    }
    {						    }
    {	pattern action	language		    }
    {						    }
    {	author: G. Poonen (1975)		    }
    {						    }
    {	modified to produce VAX LALRPAT by	    }
    {						    }
    {	    Dan McCue, Ike Nassi, Charlie Mitchell, }
    {	    Topher Cooper (1979), Bill Spitzak(1980)}
    {						    }
    {***********************************************}



    {	global declarations }
    {***********************}


    { global constants	}
    CONST
    continue	= 9996 ; { Code for continue in table }
    elsecode	= 9997 ;
    noscancode	= 9998 ; { Code for scan }
    errorcode	= 9999 ; { Code for error in table }
    unknown	= -9999 ; { unknown qty }
    maxsegment	= 200 ;
    maxtabsize	= 7000;
    maxaddr	= 2147483647 ;
    maxname	= 31	; { max length of names}
    maxerr	= 20	; { max errors per line}
    maxlinecnt	= 256	; { line count}
    maxsym	= 9	; { size of special symbols}
    statetabsize= 3500	;
    maxstate	= 3500	;
    maxrhs	= 32	;
    maxvisit	= 5000	; { max no of states visited for resolving la states }
    maxvocsize	= 1800	; { max vocabulary code }
    maxterminals= 511	;
    nametabsize = 1023	; { keep it prime }
    maxruleno	= 1500	;
    digmax	= 5	; { maximum number of digits for integer}
    threshold	= 3	; { used for sorting and matching lists}
    filenamelen = 133	; { max characters in filespec}
    listflbegin ='	';{tab}
 

    { global types }
TYPE
    word = record
	    len : 0..maxname;
	    ch : PACKED ARRAY [1 .. maxname] OF char
	    end;
    symbol =	(ident ,res,intconst,semisy,slashsy,orsy,eqsy,colsy,atsy,
	periodsy,eofsy,commasy,othersy	) ;
    setofsymbol = SET OF symbol ;
    rwtype = (terminalrw, beginrw, endrw);	{ PAT reserved words }
    namety = (	internalname,
		decterm,	{ declared terminal }
		undterm,	{ undeclared terminal }
		nonterm,	{ non-terminal }
		semact) ;	{ semantics action }
    codety = -1 .. nametabsize ;
    list    =	PACKED	RECORD
			    cdr	    : ^list ;
			    car	    : 1 .. 12287
			END ;
    listptrty = ^list;
    setofnamety = SET OF namety ;


    {	name table entries  }
    {***********************}

    identry	=   RECORD
			nic	: ^identry ;
			firstalt: ^prod ;
			defin	: ^ list ;
			usedin	: ^ list ;
			name	: word ;
			intname	: word ;
			kind	: namety ;
			spid	: symbol ;
			code	: codety
		    END ;
    identptrty = ^ identry ;

    {	The productions

		A	= B  C
			| D;

	are represented via PROD records as follows:

			+-------+	+-------+	+-------+
		next	|    ---+------>|    ---+------>|  nil	|
			+-------+	+-------+	+-------+
		def	| P to A|	| P to A|	| P to A|
			+-------+	+-------+	+-------+
		action	|  nil  |	| P to B|	| P to C|
			+-------+	+-------+	+-------+
		nxtalt  |    ---+---+	|  nil	|	|  nil	|
			+-------+   |	+-------+	+-------+
		prod.	| number|   |	|  n/a  |	|  n/a	|
			+-------+   |	+-------+	+-------+
				    |
				    |
			+-------+<--+	+-------+
		next	|    ---+------>|  nil	|
			+-------+	+-------+
		def	| P to A|	| P to A|
			+-------+	+-------+
		action	|  nil  |	| P to D|
			+-------+	+-------+
		nxtalt  |    ---+---+	|  nil	|
			+-------+	+-------+
		prod.	| number|	|  n/a  |
			+-------+	+-------+

    }

    prod = RECORD
		    next : ^prod ;
		    def : ^ identry ;
		    action : ^ identry ;
		    nxtalt : ^ prod ;
		    production : 1 .. maxruleno
	    END ;


    prodptrty	= ^prod ;
    {	state entries	}
    {*******************}
    statekind = (reads,inadequate,lookahead,reduction,subgoal,lastring) ;
    stateptrty = ^ statety ;
    tranlistptr = ^ tranlist ;
    statelistptr = ^ statelist ;
    tranptrty = ^ tranty ;
    statety = RECORD
		    firsttran : tranptrty   ;
		    back    : tranptrty ;
		    nic	    : stateptrty ;
		    state   : 0 .. maxstate ;
		    kind    : statekind
		END ;

    statelist = RECORD
			car : stateptrty ;
			cdr : statelistptr
		END ;
    tranlist =	RECORD
			car : tranptrty ;
			cdr : tranlistptr
		END ;
    tranty =	PACKED RECORD
			next : tranptrty ;
			nextstate : stateptrty ;
			rule : prodptrty ;
			pos : 0 .. maxrhs ;
			marke : boolean ;
			markc : boolean
		END ;
    errorcodety = (errterm, errtermname, errtermequal,
	errnontermequal,errbegin,
	errnontermname, errprodsemi, errsemact, erreof, errdupname,
	errtoomanyprod, erreofincomment, errintconst,
	errtermsemi, errfirstprod, errgarbageateof,
	errnestincl, errinclsemi, errintnamgone, errillintname,
	intnameredef, errdupidcode, optionalitem );

    namarry	= packed array[1..filenamelen] of char;
    extarry	= packed array[1..3] of char;
    desc	= packed record
			len: integer;
			str: ^namarry;
			end;

    languagety	= (ada, bliss, oldbliss, ltext, nothing);



    { global variables }
VAR
    resolve_display_state : 0 .. maxstate;	{ Display details of resolution
						  for this state if not 0 - 
						  set with DEBUG-32 }
    heap,heapbot,heaptop	: integer ; { marks heap }

    { table entries }


    table : ARRAY [0 .. maxtabsize] OF PACKED RECORD
						tran : integer ;
						action : integer
					END ;
    index : -1 .. maxtabsize ;
    segment : ARRAY [0 .. maxsegment] OF PACKED RECORD
						tran : integer ;
						action : integer
					END ;
    segindex : -1 .. maxsegment ;
    newstate : ARRAY [0 .. maxstate] OF integer ;


    symbolname	: PACKED ARRAY [0 .. maxvocsize] OF identptrty;
    nametab	: ARRAY [0 .. nametabsize ] OF ^ identry ;
    ruletab	: ARRAY [1 .. maxruleno ] OF ^ prod ;
    { contains no.OF elements on rhs e.g    }
    {	x <- a	b   c	will be 3 }
    poptab  : ARRAY [ 1 .. maxruleno ] OF integer ;

    nstate : ARRAY [0 .. maxstate ] OF stateptrty ;
    statetab	: ARRAY [0 .. statetabsize ] OF stateptrty ;
    inadeq,conflict : statelistptr ; { list OF inadequate states }
    currentstate : 0 .. maxstate ;
    needtoken : boolean;
    sy	    : symbol ; { last symbol }
    syleng  : integer ; { length OF symbol }
    sykey   : integer ; { symbol key }

    syval : integer ;
    sypos : integer ;
    maxk : integer ; { max val OF lookahead }
    id	:   word    ; { symbol name	}
    ch	:   char    ; { last character	}
    chcnt : integer ;
    letters
    ,smallletters
    ,digits :	SET OF char ;

    maxpop : integer ;
    maxcode, maxsymbol : integer ;
    maxprodno : 0 .. maxruleno ;
    prodno  : 0 .. maxruleno ;

    l : integer ; { control variable }
    {	option flags }

    listfl
    ,cref
    ,grammar
    ,debug
    ,trace
    ,parsetables
    ,terminals
    ,trresolve
    ,lr0
    ,research
    ,fsm    :	boolean ;
    declaredterms : boolean ; { on if terminals declared }
    language : languagety;
    commentmark : packed array [1..2] of char ;

    {	error	list }
    errorflag	: boolean ;
    errinx  :	integer	    ;	{ no OF errors IN this line}
    errlist	: ARRAY [1 .. maxerr]	OF
	RECORD	pos :	0 .. maxlinecnt ;
		nmr :	errorcodety
	END ;
    {	files	}
    includeflag : boolean;
    openfilenm: desc;
    filename : PACKED	ARRAY [1 .. filenamelen] OF char    ;
    terpas : TEXT;				{   terminals }
    outpas : TEXT ;
    {	lalr	}
    lrk	    :	boolean	    ;
    {	special symbols }
    splist  :	ARRAY [1 .. maxsym] OF char	;
    spsy    :	ARRAY [1 .. maxsym] OF symbol	;

    tdate,ttime : PACKED ARRAY[1..11] OF char;
    version : packed array[1..47] of char;

 { files }
	infile : TEXT;
	inclfile : TEXT;
	outfile : TEXT;

 { used for automatic generation of action names }
	savedlhsid : word;	{last lhs encountered}
	usedsavedlhs : 0..99;	{maximum number of auto generated action names}

 { variables for procedure lalr.  Put them here so that don't uplevel
   reference (maybe faster that way. }

	visit   : ARRAY [ 1.. maxvisit ] OF tranptrty ;
	currentvisit : integer ;
	current_lastring_state : stateptrty;

    Value
	splist	:= ('!','|','@','/',';','=',':','.',',');
	spsy	:= ( orsy ,orsy, atsy, slashsy ,semisy ,eqsy ,colsy ,periodsy, commasy);
	includeflag := false;

	{ default options }
	parsetables	:=  true    ; { print parse tables }
	terminals	:=  false   ; { print terminals file }
	fsm		:=  false   ; { print state tables }
	grammar		:=  true    ; { print grammar }
	trresolve	:=  false   ; { print trace of inadq. resol }
	listfl		:=  true    ; { create list file }
	lr0		:=  true    ; { try to generate fsm tables }
	research	:=  false   ; { fixed false }
	debug		:=  false   ; { debug option }
	maxk		:=  1	    ; { default lookahead }
	trace		:=  false   ; { debug tracing }
	cref		:=  true    ; { print cross-ref table }
	language	:=  bliss   ; { output language }

    { global variables	}
	errorflag	:= false;
	conflict	:= NIL;
	inadeq		:= NIL;


 
	PROCEDURE upcase(VAR idname:word);

	(* This returns the word with all alpha *)
	(* characters shifted to upper case	*)

	VAR
		i	: integer;

	BEGIN
		FOR i := 1 TO idname.len DO
		    IF idname.ch[i] IN smallletters
			THEN idname.ch[i] := chr(ord(idname.ch[i])-32)

	END;	(* upcase *)



	FUNCTION crelog(filnam: namarry; 
		lognam:PACKED ARRAY [LF .. LL : INTEGER] OF CHAR;
		usedir: boolean; reqext, defext: extarry;
		VAR fnd: desc): integer;

	{*******************************************************}
	{							}
	{   crelog						}
	{							}
	{   This procedure creates a logical name.		}
	{							}
	{   Inputs						}
	{							}
	{	filnam	Filename				}
	{							}
	{	lognam	Logical name				}
	{							}
	{	usedir	Use device and directory info in	}
	{		filnam if true.				}
	{							}
	{	reqext	Extension (file type) to be used	}
	{		(replace extension in filnam if any)	}
	{							}
	{	defext	Default extension.  Use if none in	}
	{		in filnam.				}
	{							}
	{   Output						}
	{							}
	{	fnd	Actual filename associated with lognam	}
	{							}
	{*******************************************************}


	VAR
	lnd	: desc;
	i	: integer;
	startpos, endpos, dirpos, dotpos : 0 .. filenamelen;
	ext : extarry;
	crelogret : integer;

	FUNCTION sys$crelog(	%IMMED tblflg: integer;
				VAR ln: desc;
				VAR en: desc;
				%IMMED acmode: integer): integer; EXTERN;
	BEGIN
	new(fnd.str);				(* make string to hold file name *)
	new(lnd.str);				(* make string to hold logical	*)
	FOR i := 1 TO filenamelen DO BEGIN	(* blank out new strings	*)
	    fnd.str^[i] :=' ';
	    lnd.str^[i] :=' '
	    END;

	{ Find end position of device or directory and position of dot before file type }

	dirpos := 0;
	dotpos := 0;

	FOR i:= 1 TO filenamelen DO
	    BEGIN
	    IF (filnam [i] =']') OR (filnam [i] ='>') OR (filnam [i] =':') THEN
		dirpos := i
	    ELSE IF filnam [i] ='.' THEN
		dotpos := i;
	    END;

	IF dotpos < dirpos THEN dotpos := 0;	{ since dot mus be in directory name }

	IF usedir THEN startpos := 1 ELSE startpos := dirpos + 1;

	endpos := filenamelen;
	IF (reqext <> '   ') AND (dotpos > 0) THEN
	    endpos := dotpos - 1;

	i := startpos;			(* copy chars from first input	*)
	fnd.len := 0;			(* parameter until blank or end *)
	WHILE (i<=endpos) DO BEGIN	(* of string. NOTE: embedded	*)
	    IF (filnam[i]<>' ') AND	(* blanks will be compressed	*)
		(fnd.len<filenamelen)	(* out of input string		*)
		THEN BEGIN
		fnd.len := fnd.len + 1;
		fnd.str^[fnd.len] := filnam[i];
		END;
	    i := i + 1
	    END;

	{ Now put in the extension if necessary }

	ext :='   ';

	IF reqext <>'   ' THEN
	    ext := reqext
	ELSE IF dotpos = 0 THEN
	    ext := defext;

	if ext <>'   ' THEN
	    BEGIN
	    fnd.len := fnd.len + 1;
	    fnd.str^[fnd.len] :='.';
	    FOR i:= 1 TO 3 DO
		BEGIN
		fnd.len := fnd.len + 1;
		fnd.str^[fnd.len] := ext [i];
		END;
	    END;

	i := 1;			(* now do the same for 2nd	*)
	lnd.len := 0;
	WHILE (i<=upper(lognam)) DO BEGIN
	    IF (lognam[i]<>' ') AND
		(lnd.len<filenamelen)
		THEN BEGIN
		lnd.len := lnd.len + 1;
		lnd.str^[lnd.len] := lognam[i];
		END;
	    i := i + 1
	    END;

	crelogret := sys$crelog(2,lnd,fnd,3); (* do it. (2 indicates process logical name tbl *)

	if crelogret = 1585 THEN { Treat SS$_SUPERCEDE same as SS$_NORMAL }
	    crelogret := 1;

	crelog := crelogret;

	IF crelogret = 1 THEN
	    BEGIN
	    WRITE (' 	Logical name ');
	    for i:= 1 TO lnd.len DO write (lnd.str^[i]);
	    WRITE (' defined as file:  ');
	    for i:= 1 TO fnd.len DO write(fnd.str^[i]);
	    writeln(' ');
	    END
	ELSE
	    BEGIN
	    WRITE (' Error in defining logical name ');
	    for i:= 1 TO lnd.len DO write (lnd.str^[i]);
	    WRITE (' as file:  ');
	    for i:= 1 TO fnd.len DO write(fnd.str^[i]);
	    writeln(' ');
	    END;

	END (* crelog *);



    PROCEDURE	initp ;
	{*******************************************************}
	{							}
	{   initp						}
	{							}
	{	This procedure reads in the options		}
	{   and also initializes any tables as			}
	{   required.						}
	{							}
	{*******************************************************}
    VAR
	i		: integer ;
	fileerror, havefilename : boolean;
	extension	: packed array [1..3] of char;

	PROCEDURE	initsets    ;
	VAR
	    l : integer ;
	    BEGIN
	    digits := ['0','1','2','3','4','5','6','7','8','9'] ;
	    letters:= ['A','B','C','D','E','F','G','H','I','J','K',
		'L','M','N','O','P','Q','R','S','T','U','V',
		'W','X','Y','Z','_','a','b','c','d','e','f','g','h','i',
		'j','k','l','m','n','o','p','q','r','s','t','u','v','w',
		'x','y','z' ] ;
	    smallletters := ['a'..'z'];

	    FOR l := 0 TO nametabsize DO
		nametab[l] :=   NIL ;
	    FOR l := 0 TO maxvocsize DO
		symbolname[l] := NIL;
	    FOR l:= 1 TO maxruleno DO
		BEGIN
		ruletab[l] :=	NIL;
		poptab [l] := 0
		END ;
	maxprodno := 0;
	prodno := 0 ;
	    maxpop := 0 ;
	    FOR l := 0 TO maxstate DO
		BEGIN
		newstate [l] := 0 ;
		nstate[ l ] := NIL ;
		END ;
	    FOR l := 0 TO maxtabsize DO
		BEGIN
		table	[l].tran := unknown ;
		table [l].action := unknown;
		END ;
	    FOR l := 0 TO statetabsize DO
		statetab[l] := NIL ;
	    maxcode := 0 ;
	    currentstate := 0
	    END ;
	{ initsets }

	PROCEDURE helpoptions;
	    BEGIN
	WRITELN (' ');
	WRITELN (' PAT options are specified in the form:');
	WRITELN (' 	Options:  B+LIST- O=BLISS');
	WRITELN (' The "+" turns on the B option and the "-" turns off the L option,');
	WRITELN (' the O= sets the output language to BLISS.');
	WRITELN (' The options (and their default values) are:');
	WRITELN (' ');
	WRITELN ('   L(ist)	List the PAT input (and error diagnostics). (on)');
	WRITELN ('   G(rammar)	List formatted grammar. (on)');
	WRITELN ('   C(ref)	List cross-reference of terminals, non-terminals. (on)');
	WRITELN ('   S(tates)	List characteristic states. (off)');
	WRITELN ('   P(arsetables)List formatted parse tables. (on)');
	WRITELN ('   T(erminals)	Write terminal definitions in file-name.TER (off)');
	WRITELN ('   O(utput) =	Specify output language.  Current options are B(LISS),');
	WRITELN ('		O(ldBLISS), P(atParser), T(ext), and N(othing).');
	WRITELN ('		Default is PATPARSER.');

	    END; {helpoptions}


	PROCEDURE helpmain;
	VAR
	    ch : char;
	    done : boolean;

	PROCEDURE helptheory;
	    BEGIN
		WRITELN (' LALRPAT is a LALR parser generator which inputs a BNF language specification');
		WRITELN (' and outputs parse tables that can be used to parse the language.  A LALR');
		WRITELN (' parser generator uses a LR parsing technique and processes a class of grammars');
		WRITELN (' known as LALR(1) grammars.  The LALR technique is currently the most powerful');
		WRITELN (' scheme (in terms of grammars that it will accept) that can recognize languages');
		WRITELN (' in linear time without backup.  The technique also has the nice property that');
		WRITELN (' syntactic errors can be detected as soon as it is possible to do so on a left-');
		WRITELN (' to-right scan of the input.  For more information, see Principles of Compiler');
		WRITELN (' Design by Aho and Ullman.');
		WRITELN (' ');

	    END; { helptheory }

	PROCEDURE helpinput;
	    BEGIN

		WRITELN (' { Sample LALRPAT input }');
		WRITELN (' ');
		WRITELN (' TERMINAL');
		WRITELN (' ''('',LPAREN	= 1;');
		WRITELN (' '')'',RPAREN	= 2;');
		WRITELN (' END;');
		WRITELN (' ');
		WRITELN (' BEGIN		{ Define productions }');
		WRITELN ('   { This is a grammar for a single identifier surrounded by an ');
		WRITELN ('     arbitrary number of paired parentheses.  When the whole thing');
		WRITELN ('     has been processed, the semantics action SEM_SENTENCE is taken.');
		WRITELN ('     The semantics action SEM_PAREN_EXPR is taken whenever');
		WRITELN ('     "(" expression ")" is reduced to expression. }');
		WRITELN (' ');
		WRITELN (' SENTENCE	= EXPRESSION goalsy	/ SEM_SENTENCE ;');
		WRITELN (' EXPRESSION	= ''('' EXPRESSION '')''	/ SEM_PAREN_EXPR ');
		WRITELN (' 		| identifier ;');
		WRITELN (' ');
		WRITELN (' END.	{ End of sample LALRPAT Input }');

	    END; { helpinput }


	PROCEDURE helplex;
	    BEGIN

		WRITELN ('     1.	The first production must have a single right hand rule of the form: ');
		WRITELN (' 	  non-term-name = non-term-name GOALSY ;     or');
		WRITELN (' 	  non-term-name = non-term-name GOALSY	/ semantics-action-name ;');
		WRITELN (' ');
		WRITELN ('     2.	The "or" symbol (either "|" or "!") separates multiple right hand rules');
		WRITELN (' 	for a production.  The last right hand rule is delimited by ";".  e.g.,');
		WRITELN (' 	  X = Y goalsy ;');
		WRITELN (' 	  Y = OPTIONAL_A OPTIONAL_B 	/ SEM_Y ;');
		WRITELN (' 		{ OPTIONAL_A can be an "a" or nothing }');
		WRITELN (' 	  OPTIONAL_A = a		/ SEM_A');
		WRITELN (' 	  	     | {epsilon} 	/ SEM_NO_A ;');
		WRITELN (' 	  OPTIONAL_B = b | {epsilon} ;');
		WRITELN (' ');
		WRITELN ('     3.	Special characters in the input grammar must be quoted (e.g., ''+'',');
		WRITELN (' 	''<='' ).  The word END is a PAT reserved word and must be quoted ');
		WRITELN (' 	( ''end'' ) to be used as a terminal or non-terminal symbol.');
		WRITELN (' 	Comments can begin with "{" or "%" and end with "}" or "\".');
		WRITELN (' 	LALRPAT does not distinguish between upper and lower case.');
	    END; { helplex }

	PROCEDURE helpdecl;
	    BEGIN

		WRITELN (' { Sample LALRPAT input to illustrate declaration of terminal symbols }');
		WRITELN (' ');
		WRITELN (' { If terminals are not declared, LALRPAT assigns values by increasing a');
		WRITELN ('   counter for each new terminal or non-terminal name that it sees.');
		WRITELN ('   Alternatively, the LALRPAT user can specify the integer value to be');
		WRITELN ('   associated with each terminal.  This has the advantage that a range');
		WRITELN ('   check can be used to determine if an item is a reserved word, a');
		WRITELN ('   special character, etc.  }');
		WRITELN (' TERMINAL');
		WRITELN ('   begin,t_begin= 1;	{ illegal identifiers in your output language}');
		WRITELN ('   '';'',semi	= 3;	{ must have identifier names specified.}');
		WRITELN ('   ''end'',t_end	= 2;	{ END is a PAT reserved word so it has to be quoted. }');
		WRITELN ('   identifier	= 4;');
		WRITELN ('   goalsy	= 5;');
		WRITELN (' END;');
		WRITELN (' BEGIN');
		WRITELN ('   PROGRAM	= SENTENCE goalsy ;');
		WRITELN ('   SENTENCE	= begin identifier ''end'' '';'' ;');
		WRITELN (' END.');

	    END; { helpdecl}


	PROCEDURE helpincl;

	    BEGIN
		WRITELN (' { Include facility example }');
		WRITELN (' ');
		WRITELN (' BEGIN');
		WRITELN (' ');
		WRITELN (' SENTENCE	= COMP_UNIT goalsy ;');
		WRITELN (' ');
		WRITELN (' 	{ Include the contents of file SRC:GRAM.PAT at this point.  The');
		WRITELN (' 	  remainder of the line following the "@" is taken as a file-spec. }');
		WRITELN (' @SRC:GRAM.PAT');
		WRITELN (' ');
		WRITELN (' COMP_UNIT	= STATEMENT_LIST ;');
		WRITELN (' ');
		WRITELN (' 	{ ... }');
		WRITELN (' ');
		WRITELN (' END.');
	    END; { helpincl }

	PROCEDURE helpcall (PROCEDURE helproutine);
	    BEGIN
	    WRITELN (' ');
	    WRITELN (' ');
	    WRITELN (' --------------------------------------------------------------------');
	    helproutine;
	    END; { helpcall }


	BEGIN {helpmain}

	done := false;

	REPEAT
	    WRITELN (' ');
	    WRITELN (' ');
	    WRITELN (' PAT HELP.  Enter I (PAT Input), O (Options), L (Lexical rules), @ (Include),');
	    WRITE ('  D (Declaration of terminals), T (Theory), or press return when done:  ');
	    IF eoln (input) THEN
		done := true
	    ELSE
	    BEGIN
	    read (ch);
	    IF (ch ='T') OR (ch ='t') THEN helpcall (helptheory)
	    ELSE IF (ch ='O') OR (ch ='o') THEN helpcall (helpoptions)
	    ELSE IF (ch ='I') OR (ch ='i') THEN helpcall (helpinput)
	    ELSE IF (ch ='L') OR (ch ='l') THEN helpcall (helplex)
	    ELSE IF (ch ='D') OR (ch ='d') THEN helpcall (helpdecl)
	    ELSE IF (ch ='@') THEN helpcall (helpincl);
	    END;

	    readln;

	UNTIL done;

	END; {helpmain}



	PROCEDURE   options	;
	    {***********************************************************}
	    {								}
	    {	options							}
	    {								}
	    {	This procedure reads in the options			}
	    {	and sets the option flags appropriately.		}
	    {								}
	    {	    global in:	ch	last character			}
	    {								}
	    {***********************************************************}
	var
	    retry,dummy : BOOLEAN;

	function switch:boolean;
	    {  Read next + or - input, also moves pointer to after	}
	    {  current keyword and any white space.			}
		BEGIN
		WHILE NOT eoln(input) AND (ch IN letters) DO read(ch);
		IF (ch IN letters) then ch:=' ';
		WHILE NOT eoln(input) AND (ord(ch) <= ord(' ')) DO read(ch);
		switch := (ch <> '-')
		END;

	    BEGIN
	    { options }
	    retry := true;
	    WHILE retry DO
		BEGIN
		write('Options:  ');
		retry := false;
		read(ch);
		REPEAT
		    IF ch IN smallletters THEN ch := chr(ord(ch)-32);
		    CASE ch OF
			'H' : BEGIN
				helpoptions;
				retry := TRUE;
			      END;
			'T' : terminals := switch;
			'L' : listfl := switch;
			'G' : grammar := switch;
			'C' : cref := switch;
			'S' : fsm := switch;
			'P' : parsetables := switch;
			'D' : BEGIN
				read(ch);
				IF ch IN smallletters THEN ch := chr(ord(ch)-32);
				CASE ch OF
				    'D' : debug := switch;
				    'T' : trace := switch;
				    'R' : trresolve := switch;
				    'P' : lr0 := not switch; { parser check }
				    OTHERWISE BEGIN
					writeln('Error - Illegal Debug option.');
					retry := TRUE;
					END;
				    END;
			      END;
			'O' : BEGIN
				dummy := switch; { skip to = sign }
				IF ch<>'=' THEN
				    BEGIN
				    writeln('Error - No = after Output option.');
				    retry := TRUE;
				    END
				else
				    BEGIN
				    WHILE NOT eoln(input) AND NOT (ch IN letters) DO read(ch);
				    IF ch IN smallletters THEN ch:=chr(ord(ch)-32);
				    CASE ch OF
					'A' : language := ada;
					'B' : language := bliss;
					'O' : language := oldbliss;
					'P' : language := bliss;
					'T' : language := ltext;
					'N' : language := nothing;
					OTHERWISE BEGIN
						writeln('Error - illegal Output language specified.');
						retry := TRUE;
						END;
					END;
				    dummy := switch;
				    END;
				END;
			OTHERWISE
				IF ch IN letters THEN
				    BEGIN
				    writeln('Error - illegal Option specified.');
				    retry := TRUE;
				    END
				ELSE IF NOT eoln(input) THEN read(ch);
			END; {case}
		    UNTIL (eoln(input) AND NOT (ch IN letters)) OR retry;
		WHILE NOT eoln(input) DO read(ch);
		END;
	    END ;	{ options }





	BEGIN
	{   initp   }
	{   get filename }


	initsets;
	date(tdate);
	time(ttime);
	version :=' VAX/VMS PAT -- Version of 07 DEC 85      --   ';
	writeln(version, tdate,' ',ttime);

	havefilename := false;

	REPEAT
	writeln;
	write('Input File (Type ? for help):  ');

	I := 0;
	REPEAT
		READ(CH);
		I:=I+1;
		IF (ch IN smallletters) THEN
		    filename[i] := chr(ord(ch)-32) (* convert to upper case *)
		ELSE
		    filename[i] := ch;
	UNTIL EOLN(INPUT);
	for i:= i+1 TO filenamelen DO
	    filename [i] :=' ';

	IF filename [1] ='?' THEN
	    BEGIN
	    readln;
	    helpmain;
	    END
	ELSE IF crelog(filename,'INFILE',true,'   ','PAT',openfilenm)=1 THEN
	    BEGIN
	    open(infile,'INFILE',READONLY,133,SEQUENTIAL);
	    reset(infile);
	    havefilename := true;
	    END;

	UNTIL havefilename;


	options;

	IF crelog(filename,'OUTFILE',false,'LIS','   ',openfilenm) = 1 THEN BEGIN
	    open(outfile,'OUTFILE',NEW,500,SEQUENTIAL);
	    rewrite(outfile);
	    write(outfile,' ');
	    END;

	IF terminals THEN
	    BEGIN
	    IF crelog(filename,'TERPAS',false,'TER','   ',openfilenm)=1
		THEN BEGIN
		open(terpas,'TERPAS',NEW,133,SEQUENTIAL);
		rewrite(terpas);
		write(terpas,' ');
		END;
	    END;
	IF NOT (language = nothing) THEN
	    BEGIN
	    CASE language OF
		ada : extension := 'ADA';
		bliss,oldbliss : extension := 'REQ';
		ltext : extension := 'TAB';
	    END;
	    IF crelog(filename,'OUTPAS',false,extension,'   ',openfilenm)=1 THEN
		BEGIN
		open(outpas,'OUTPAS',NEW,133,SEQUENTIAL);
		rewrite(outpas);
		END;
	    END;
	writeln(outfile,version, tdate,' ',ttime);
	writeln(outfile, listflbegin);
	writeln(outfile, listflbegin);
	write(outfile, listflbegin);
	CASE language OF
	    ada : commentmark := '--';
	    bliss,oldbliss : commentmark := ' !';
	    OTHERWISE commentmark := '  '
	    END;
	if language <> nothing then
		begin
		writeln(OUTPAS,' ',commentmark,version,tdate,' ',ttime);
		end;
	END;
    {initp }


    PROCEDURE	error	(errorcode : errorcodety; errpos : integer) ;
	{***************************************************}
	{						    }
	{	error					    }
	{						    }
	{	This procedure records the error	    }
	{   and position of occurrence			    }
	{						    }
	{   global					    }
	{						    }
	{	    out : errorflag			    }
	{		errlist	    ARRAY OF errors	    }
	{		errinx				    }
	{						    }
	{			    i			    }
	{						    }
	{***************************************************}
	BEGIN
	{   error }
	errorflag :=	true ;
	IF errinx < maxerr  THEN
	    BEGIN
	    errinx  :=	errinx +1;
	    errlist[errinx].nmr	    := errorcode;
	    errlist[errinx].pos	    :=	errpos;
	    END ;
	END ;
    {	error }



{***************************************************************}
{								}
{	Start of routines that are designed to be used only by	}
{	the parsing primitive routines.				}
{								}
{***************************************************************}


    FUNCTION eofin : boolean;

	{******************************************************}
	{							}
	{   eofin						}
	{							}
	{   This procedure checks for eof in the current input	}
	{   file (INFILE or include file).			}
	{							}
	{******************************************************}

	BEGIN
	IF includeflag THEN
	    eofin := eof (inclfile)
	ELSE
	    eofin := eof (infile);

	END;	{ eofin }



    PROCEDURE readeolnin;

	{*******************************************************}
	{							}
	{   eolnin						}
	{							}
	{   This procedure reads the eoln in the current input	}
	{   file (INFILE or include file).			}
	{							}
	{*******************************************************}

	BEGIN
	IF NOT eofin THEN
	    IF includeflag THEN
	    readln (inclfile)
	    ELSE
	    readln (infile);

	END;	{ eolnin }



    FUNCTION eolnin : boolean;

	{*******************************************************}
	{							}
	{   eolnin						}
	{							}
	{   This procedure checks for eoln in the current input }
	{   file (INFILE or include file).			}
	{							}
	{*******************************************************}

	BEGIN
	IF includeflag THEN
	    eolnin := eoln (inclfile)
	ELSE
	    eolnin := eoln (infile);

	END;	{ eolnin }



    PROCEDURE	initchcnt;
	{*******************************************************}
	{							}
	{   initchcnt						}
	{							}
	{   This procedure initializes the character position	}
	{   counter chcnt for a new line.			}
	{							}
	{   chcnt is used to determine the position of ^	}
	{   to mark input errors in the .LIS file.  chcnt	}
	{   reflects the logical position as if spaces were	}
	{   substituted for tabs.				}
	{							}
	{*******************************************************}
    BEGIN
	chcnt := 0;
    END;


    PROCEDURE	endofline   ;
	{*******************************************************}
	{							}
	{   end of line						}
	{							}
	{	This procedure writes out the			}
	{	current line and follows it with		}
	{	any error messages and pointers.		}
	{							}
	{							}
	{	global	in: errinx   no. of errors in this line	}
	{							}
	{		    errlist array containing		}
	{			    attributes of error		}
	{							}
	{		    list    listing required if		}
	{			    true.			}
	{							}
	{		out : errinx    set to 0		}
	{							}
	{							}
	{*******************************************************}
    VAR
	freepos
	,currpos
	,k		    { loop control  }
	:   integer ;
	currnmr : errorcodety;

	procedure writemessage (errcode: errorcodety);
	BEGIN
	case errcode of
	    errterm:
		write (outfile,'Missing "TERMINAL" assumed present.');
	    errtermname:
		write (outfile,'Expecting terminal name (processing terminal declarations).');
	    errtermequal:
		write (outfile,'Expecting "=" (processing terminal declarations).');
	    errnontermequal:
		write (outfile,'Expecting "=".');
	    errbegin:
		write (outfile,'Missing "BEGIN" assumed present.');
	    errnontermname:
		write (outfile,'Expecting non-terminal name.');
	    errprodsemi:
		write (outfile,'Expecting ";".  (Probably missing from the end of the previous production.)');
	    errsemact:
		write (outfile,'Expecting semantics action name.');
	    erreof:
		write (outfile,'Unexpected end of file.');
	    errgarbageateof:
		write (outfile,'Expecting end of file.');
	    errdupname:
		write (outfile,'This name was previously declared as a terminal.');
	    errtoomanyprod:
		write (outfile,'Exceeded maximum number of productions.');
	    erreofincomment:
		write (outfile,'Unterminated comment.  Found end of file.');
	    errintconst:
		write (outfile,'Expecting integer constant (processing terminal declarations).');
	    errtermsemi:
		write (outfile,'Missing ";" at end of terminal declaration.');
	    errfirstprod:
		begin
		writeln (outfile,'First production must have one right hand side rule');
		writeln (outfile, listflbegin,'and be of the form:');
		writeln (outfile,' ');
		writeln (outfile, listflbegin,'		X = Y GOALSY;');
		writeln (outfile,' ');
		write (outfile, listflbegin,'	where X and Y are user-defined non-terminals.');
		end;
	    errnestincl:
		write (outfile,'Includes within include files are not supported.');
	    errintnamgone:
		write (outfile,'Internal name missing.');
	    errillintname:
		write (outfile,'Internal name may not contain quotes.');
	    intnameredef:
		write (outfile,'Attempt to redefine internal name.');
	    errdupidcode:
		write (outfile,'This number already assigned to a terminal');
	END;

	END;	{ writemessage }

	BEGIN {endofline}
	readeolnin;
	if listfl then
		begin
		writeln(outfile);
		end;
	IF (errinx > 0) and listfl THEN	{output error messages}
	    BEGIN
	    FOR k := 1 TO errinx DO
	    BEGIN
	    freepos := 1;
	    WITH errlist[k] DO
		BEGIN
		currpos := pos; currnmr := nmr
		END;
	    IF (currpos > 0) AND (currpos < 130) THEN
		BEGIN
		write(outfile, listflbegin);
		WHILE freepos < currpos DO
		    BEGIN
		    write(outfile,'.');
		    freepos := freepos + 1;
		    END;
		writeln(outfile,'^');
		END;
	    write (outfile,'*** ERROR:  ');
	    writemessage (currnmr);
	    writeln(outfile);
	    END;
	    writeln(' Error in input file');
	    writeln (outfile, listflbegin);	{ skip a line }
	    END;
	initchcnt;
	errinx :=0;
	if listfl then write (outfile, listflbegin);
	END {endofline} ;




    PROCEDURE	nextch	;
	{*******************************************************}
	{							}
	{   nextch.						}
	{							}
	{	This procedure returns the next character	}
	{   in ch						}
	{							}
	{							}
	{							}
	{   global	in:	chcnt	    see initchcnt	}
	{			listfl	    boolean option	}
	{							}
	{		out :	chcnt				}
	{							}
	{			ch	    next character	}
	{							}
	{*******************************************************}
	BEGIN
	{ nextch }
	IF eolnin OR	eofin THEN
	    ch :=' '
	ELSE
	    BEGIN
	    IF includeflag THEN
		read(inclfile,ch)
	    ELSE
		read(infile,ch);

	    IF ch ='	'{ tab } THEN
		chcnt := 8 * ((chcnt div 8) + 1)
	    ELSE
		BEGIN
		chcnt := chcnt + 1;
		IF ord(ch) < ord(' ') (* control char *) THEN ch :=' ';
		END;
	    IF listfl THEN write(outfile,ch);
	    END;

	END ;		{ nextch }



    PROCEDURE	insymbol ;
	{*******************************************************}
	{							}
	{   insymbol						}
	{							}
	{	This procedure returns the next			}
	{	symbol from the source input			}
	{							}
	{   global  in:	ch	next character			}
	{		sp list					}
	{		sp sy					}
	{							}
	{	    out:syleng	    symbol length		}
	{		sypos	    column position in input	}
	{		sy	    symbol type			}
	{		id.ch[i]    symbol characters		}
	{		sykey	    key for identifiers		}
	{							}
	{							}
	{*******************************************************}

    LABEL
	1,2,3,4;

    VAR
	i,k : integer ;
	BEGIN
	{   insymbol }
	IF NOT EOFIN THEN
	WHILE true DO
	    BEGIN
	    WHILE true DO
		BEGIN
		IF  (ch <>' ') AND (ch <>'	')
		    THEN GOTO 1;	{ skip blanks and tabs}
		IF  eolnin  THEN endofline ;
		IF eofin THEN GOTO 1;
		nextch	    ;
		END	;
 1:	    IF	(ch <>'%') and (ch <>'{') THEN GOTO 2 ;
	    nextch  ;
	    WHILE true DO
		BEGIN
		IF  eolnin  THEN endofline ;
		IF  (ch ='}') OR (ch ='\') THEN GOTO 3 ;
		IF eofin THEN
		    BEGIN
		    error (erreofincomment, 0);
		    GOTO 3;
		    END;
		nextch ;
		END ;
 3:	    nextch
	    END ;
 2:	syleng	:= 0 ;
	syval	:= 0 ;
	sykey	:= 0 ;
	sypos	:= chcnt;
	IF eofin THEN
	    BEGIN
	    sypos := 0;
	    sy := eofsy;
	    END
	ELSE IF	ch IN letters THEN
	    BEGIN
	    k := 0 ;
		REPEAT
		IF k < maxname THEN
		    BEGIN
		    k := k+1 ;
		    id.ch[k] := ch  ;
		    IF ch IN smallletters
			THEN sykey := sykey + (ord(ch)-32)
			ELSE sykey := sykey + ord(ch) ;
		    END	;

		nextch
		UNTIL NOT ((ch IN letters) OR (ch IN digits)) ;
	    id.len := k;
	    IF k < maxname  THEN
	    FOR i := k+1 TO maxname DO id.ch[i] :=' ' ;
	    sy	:= ident;
	    upcase(id);
	    END
	ELSE	IF  ch	IN  digits  THEN
	    BEGIN
	    sy := intconst ;
	    i	:=  0	;   syval :=0;
		REPEAT
		i := i+1 ;
		IF  i < digmax THEN
		syval := syval*10   + ord (ch) - ord ('0') ;
		id.ch[i]    :=	ch  ;
		nextch
		UNTIL NOT   (ch IN digits)  ;
	    id.len := i;
	    IF i < maxname THEN
	    FOR i:= i+1 TO maxname  DO	id.ch[i] :=' '
	    END
	ELSE IF ch ='''' THEN
	    BEGIN
	    k := 0 ;
		REPEAT
		IF k < maxname THEN
		    BEGIN
		    k := k+1 ;
		    id.ch[k] := ch ;
		    IF ch IN smallletters
			THEN sykey := sykey + (ord(ch)-32)
			ELSE sykey := sykey + ord(ch) ;
		    END ;
		nextch ;
		WHILE ch <>'''' DO
		    BEGIN
		    IF k < maxname THEN
			BEGIN
			k := k+1 ;
			id.ch[k] := ch ;
			IF ch IN smallletters
			    THEN sykey := sykey + (ord(ch)-32)
			    ELSE sykey := sykey + ord(ch) ;
			END ;
		    nextch ;
		    END ;
		IF k< maxname THEN
		    BEGIN
		    k := k+1 ;
		    id.ch[k] :=ch ;
		    IF ch IN smallletters
			THEN sykey := sykey + (ord(ch)-32)
			ELSE sykey := sykey + ord(ch) ;
		    END ;
		nextch ;
		UNTIL ch <>'''';
	    id.len := k;
	    IF k< maxname THEN
	    FOR k := k+1 TO maxname DO id.ch[k] :=' ';
	    sy := res ;
	    END
	ELSE
	    BEGIN
	    sy := othersy;
	    FOR i := 1 TO maxsym DO
		BEGIN
		IF splist[i]= ch    THEN
		    BEGIN
		    sy := spsy [i] ;
		    GOTO 4 ;
		    END ;
		END ;
	    4 : id.ch[1] := ch;
	    nextch ;
	    FOR i:= 2 TO maxname    DO id.ch[i]:=' ';
	    id.len := 1
	    END ;
	END ;
    {	insymbol }

{***************************************************************}
{								}
{	End of routines that are designed to be used only by	}
{	the parsing primitives.					}
{								}
{***************************************************************}



{***************************************************************}
{								}
{	Start of parsing primitives.				}
{								}
{***************************************************************}



    PROCEDURE parseinit;

	{***********************************************************}
	{							    }
	{   parseinit						    }
	{							    }
	{   This procedure initializes the parsing routines.	    }
	{   It should be called when changing from one file to	    }
	{   another (when doing an include or resuming processing   }
	{   in the main input file.				    }
	{							    }
	{***********************************************************}

    BEGIN
	initchcnt;	{ initialize the character position counter }
	ch :=' ';	{ pretend the first character is a blank }
	needtoken := true;
    END;	{ parseinit }


    PROCEDURE parsegettoken;

	{*******************************************************}
	{							}
	{   parsegettoken					}
	{							}
	{   The procedure is called by the parsing routines to	}
	{   get the next token.	It also handles the mechanics	}
	{   of include files.					}
	{							}
	{*******************************************************}

	VAR
	i, j : integer;

    BEGIN
	IF needtoken THEN
	BEGIN
	    insymbol;

	    IF (sy = eofsy) AND (includeflag) THEN
		BEGIN
		includeflag := false;	{ finished with include file }
		close (inclfile);
		parseinit;		{ reinitialize for main file }
		insymbol;		{ get the next token }
		END
	    ELSE IF sy = atsy THEN
		BEGIN
		IF includeflag THEN
		    error (errnestincl, sypos)
		ELSE
		    BEGIN { we already have the next character in hand }
		    IF (ch IN smallletters) THEN
			filename[1] := chr(ord(ch)-32) (* convert to upper case *)
		    ELSE
			filename[1] := ch;
		    i := 2;

		    WHILE NOT eolnin DO
			BEGIN
			nextch;			{ get the next character }
			IF i <= filenamelen THEN	{ save the character }
			    IF (ch IN smallletters) THEN
			    filename[i] := chr(ord(ch)-32) (* convert to upper case *)
			    ELSE
			    filename[i] := ch;
			i := i + 1;
			END;

		    endofline;

		    FOR j:= i TO filenamelen DO filename [j] :=' ';

		    IF crelog (filename,'INCLFILE', true,'   ','PAT', openfilenm)=1 THEN
			BEGIN
			open (inclfile,'INCLFILE',old, 133, sequential);
			reset (inclfile);
			includeflag := true;
			END;

		    parseinit;		{ reinitialize for new line in main file or at start of include file }
		    parsegettoken;	{ now make one recursive call to get a token }
		    END;		{ if not already in an include file }
		END;		{ if sy = atsy }
	    needtoken := false;
	END;		{ if needtoken }

    END;	{ parsegettoken }


    PROCEDURE parseend;

	{***********************************************************}
	{							    }
	{   parseend						    }
	{							    }
	{   This procedure is called when parsing is complete	    }
	{							    }
	{***********************************************************}

    BEGIN
	endofline;
    END;	{ parseend }


    PROCEDURE parseerror (errcode : errorcodety;
		synchsy : setofsymbol);

	{***********************************************************}
	{							    }
	{   parseerror						    }
	{							    }
	{   This procedure is called when a parse error is	    }
	{   detected.	It calls procedure error to remember	    }
	{   the error and recovers to a synchonizing symbol	    }
	{   unless the set of synchronizing symbols is empty.	    }
	{							    }
	{   In the error recovery situation (i.e., synchsy is not   }
	{   empty), either eofsy (end of file) or one of the	    }
	{   synchronizing symbols become the current token. In	    }
	{   addition, the needtoken flag is set so that will get    }
	{   another token before try to parse anything.		    }
	{							    }
	{***********************************************************}

    BEGIN
	error (errcode, sypos);
	IF synchsy <> [] THEN
	BEGIN
	    needtoken := true;
	    { skip to synch. symbol }
	    WHILE NOT (sy = eofsy) AND NOT(sy IN synchsy) DO insymbol ;
	END;
    END;	{ parseerror }


    FUNCTION parsesy (items : setofsymbol; errcode : errorcodety;
		synchsy : setofsymbol) : BOOLEAN;

	{***********************************************************}
	{							    }
	{   parsesy						    }
	{							    }
	{   This procedure parses a symbol and handles getting	    }
	{   tokens from insymbol.   If a match is not found,	    }
	{   and the item is not optional, procedure		    }
	{   parseerror is called to record the error and recover    }
	{   to a synchronizing symbol.				    }
	{							    }
	{***********************************************************}

    BEGIN

	parsegettoken;

	IF sy IN items THEN
	    BEGIN
	    parsesy := true;
	    needtoken := true;
	    END
	ELSE
	    BEGIN
	    parsesy := false;
	    IF errcode <> optionalitem THEN parseerror (errcode, synchsy);
	END;
    END;	{ parsesy }


    FUNCTION parserw (rw : rwtype; errcode : errorcodety;
		synchsy : setofsymbol) : BOOLEAN;

	{***********************************************************}
	{							    }
	{   parserw						    }
	{							    }
	{   This procedure parses a PAT reserved word and handles   }
	{   getting tokens from insymbol. If a match is not found,  }
	{   and the item is not optional, procedure		    }
	{   parseerror is called to record the error and recover    }
	{   to a synchronizing symbol.				    }
	{							    }
	{***********************************************************}

	VAR
	foundrw : boolean;

    BEGIN

	parsegettoken;

	foundrw := false;
	IF sy = ident THEN
	CASE rw OF
	    terminalrw:
	    IF (id.ch ='TERMINAL                       ') THEN
		foundrw := true;
	    beginrw:
	    IF (id.ch ='BEGIN                          ') THEN
		foundrw := true;
	    endrw:
	    IF (id.ch ='END                            ') THEN
		foundrw := true;
	END;

	IF foundrw THEN
	    BEGIN
	    parserw := true;
	    needtoken := true;
	    END
	ELSE
	    BEGIN
	    parserw := false;
	    IF errcode <> optionalitem THEN parseerror (errcode, synchsy);
	    END;
    END;	{ parserw }

{***************************************************************}
{								}
{	End of parsing primitives.				}
{								}
{***************************************************************}

{***************************************************************}
{								}
{	Start of routines that read and parse the input file	}
{	and create the symbol table.				}
{								}
{***************************************************************}



    FUNCTION findid (idname :word; key : integer ; idkind
	: setofnamety ) : identptrty ;
	{*******************************************}
	{					    }
	{   findid				    }
	{					    }
	{	This procedure returns a	    }
	{   a pointer to the given name and type    }
	{   in then symbol table.		    }
	{					    }
	{*******************************************}

    VAR
	i : integer ;
	found : boolean	;
	q : ^identry ;
	BEGIN
	i := key MOD nametabsize ;
	q := nametab[i] ;
	found := false;
	upcase(idname);
	WHILE (q <> NIL) AND (NOT found ) DO
	    IF (q^.name.ch = idname.ch) AND (q^.kind IN idkind) THEN
		found := TRUE
	    ELSE
		q := q^.nic;
	findid := q
	END ;




    FUNCTION enterid (idname, idintname: word; key: integer; idkind: namety;
	idsy : symbol)	:   identptrty	;
	{***************************************************}
	{						    }
	{   enterid					    }
	{						    }
	{   This procedure enters the given name	    }
	{   into the symbol table.			    }
	{						    }
	{   This function assumes that it is called only    }
	{   if function findid returns NIL.		    }
	{						    }
	{***************************************************}
    LABEL
	2;
    VAR
	i,j : integer ;
	p,q : ^ identry ;
	BEGIN
	{ enterid }
	i := key MOD nametabsize ;
	q := nametab[i]	;
	upcase(idname);
	IF trace THEN
	    BEGIN
	    writeln(' I= ',i,' Key= ',key);
	    END ;
	WHILE q <> NIL DO
	    q := q^.nic;
	new (p) ;
	WITH p^ DO
	    BEGIN
	    nic := NIL	;
	    firstalt := NIL ;
	    name    := idname	;
	    intname := idintname ;
	    spid := idsy ;
	    code    := 0;
	    kind    := idkind ;
	    usedin  := NIL  ;
	    defin   := NIL;
	    END	;
	q := nametab[i]	;
	nametab[i] := p	;
	p^. nic	:= q	    ;
	enterid	:= p	;
	2 :
	END ;
    { enterid }

PROCEDURE setcode (p : identptrty; idcode:codety);
	BEGIN
	    IF idcode= -1 THEN idcode :=maxcode+1;
	    IF idcode <= maxvocsize THEN
		IF symbolname[idcode] = NIL THEN symbolname[idcode] := p
			ELSE error(errdupidcode, sypos);
	    p^.code := idcode ;
	    IF idcode > maxcode THEN maxcode := idcode ;
	END;

FUNCTION parseid (newtype:namety; replacelist:setofnamety):identptrty;
	{***************************************************************}
	{ parseid							}
	{								}
	{   This procedure reads an identifier name and a symbolic	}
	{   name (if given).  If it doesn't exist yet, it stores	}
	{   it in the symbol list with type newtype.  If it does,	}
	{   and the type is in the set replace, it returns that.	}
	{								}
	{   This assumes you have just called parsesy and gotten	}
	{   an identifier to be stored.					}
	{								}
	{***************************************************************}
    VAR
	name, intname:word;
	namepos, intnamepos : integer;
	namekey : integer;
	namesy : symbol;
	dummy,intnamedef : boolean;
	x : integer;
	temp : identptrty;

	BEGIN
	{ remember info about id name }
	name := id;
	namekey := sykey;
	namepos := sypos;
	namesy	:= sy;
		{ Parse internal name, if present }
	intnamedef := FALSE;
	IF parsesy([commasy], optionalitem, []) THEN
		IF parsesy([ident,res], errintnamgone, []) THEN
			BEGIN
			intname := id;
			intnamepos := sypos;
			intnamedef := TRUE;
			END
		ELSE BEGIN
			intname.len := 0; { prevent double error }
			intname.ch[1] := 'X';
			END
	ELSE { use actual name for internal name }
		BEGIN
		intname := name;
		intnamepos := namepos;
		END;
	{ see if the id already exists }
	temp := findid(name,namekey,replacelist+[newtype]);
	IF temp = NIL THEN
	    BEGIN
		{ strip quotes off internal name }
		if intname.ch[1] = '''' THEN WITH intname DO
		    BEGIN
			FOR x := 2 TO len DO
				ch[x-1] := ch[x];
			len := len-2;
		    END;
		{ see if internal name is legal }
		dummy := FALSE;  { error flag }
		WITH intname DO
			FOR x := 1 TO len DO
			    BEGIN
				IF NOT (ch[x] IN letters+digits) THEN dummy := TRUE;
				IF ch[x] IN smallletters THEN ch[x] := chr(ord(ch[x])-32);
			    END;
		IF NOT(intname.ch[1] IN letters) THEN dummy := TRUE;
		IF dummy THEN error(errillintname, intnamepos);
		temp := enterid(name,intname,namekey,newtype,namesy);
	    END
	ELSE
	    IF NOT (temp^.kind IN replacelist) THEN
		error(errdupname,namepos)
	    ELSE IF intnamedef THEN error(intnameredef, intnamepos);
	parseid := temp;
	END;


	FUNCTION terminal : boolean;
	{*******************************************************}
	{							}
	{ terminal						}
	{							}
	{ This procedure reads the terminal			}
	{ definitions AND enters them IN the			}
	{ symbol table.						}
	{							}
	{*******************************************************}
    VAR
	tp	: identptrty ;
	dummy	: boolean;
	foundend: boolean;
	mark	: boolean;

	BEGIN	{ terminal }
	writeln(' Reading the input file');

	{ parse "TERMINAL" - assume it's present if it's missing }
	declaredterms := parserw (terminalrw, optionalitem, []);
	foundend := false;

    IF declaredterms THEN
	BEGIN
	{				    }
	{ Process all terminal definitions  }
	{				    }

	WHILE (SY <> EOFSY) AND NOT foundend DO

	    { Check for "END" }
	    IF parserw (endrw, optionalitem, []) THEN
		foundend := true
	    ELSE
		{ parse terminal-name }
		IF parsesy ([ident,res], errtermname, [semisy]) THEN
		BEGIN
		tp := parseid (decterm, []);
		{ Parse "=" }
		mark := FALSE;
		IF parsesy ([eqsy], errtermequal, [semisy]) THEN
		    { Parse value }
		    IF parsesy ([intconst], errintconst, [semisy]) THEN
			BEGIN
			IF syval>=0 THEN
				BEGIN setcode(tp,syval); mark := TRUE; END
			    ELSE error(errintconst, sypos);
			{ Parse ";" }
			dummy := parsesy ([semisy], errtermsemi, [semisy]);
			END;
		IF NOT mark THEN setcode(tp,-1);
		END;

	IF foundend THEN
	    BEGIN
	    { Parse optional ";" }
	    terminal := true;
	    dummy := parsesy ([semisy], optionalitem, [semisy]);
	    END
	ELSE
	    BEGIN
	    terminal := false;
	    error (erreof, 0);
	    END;
	END
    ELSE terminal := (sy <> eofsy);
    END; { terminal }



    PROCEDURE readg;
	{*******************************************}
	{					    }
	{   readg				    }
	{					    }
	{ This procedure reads in the grammar	    }
	{ and sets up the symbol table.		    }
	{					    }
	{*******************************************}
    VAR
	p:  ^identry ;{ lhs symbol }
	dummy, foundend, firstprodok : boolean;
	l: integer;
	lastsemact : integer;

    PROCEDURE parserhs (p : identptrty);
	{*******************************************}
	{					    }
	{   parserhs				    }
	{					    }
	{ This procedure parses all right hand	    }
	{ sides for one non-terminal and sets	    }
	{ up the symbol table.	On return,	    }
	{ the current token is the one following a  }
	{ semicolon or we are at the end of the	    }
	{ input file.				    }
	{					    }
	{*******************************************}
    LABEL
	1;
    VAR
	t,u{ current rhs transition }
	,q: ^prod	;{ lhs production }
	v:  ^identry	;{ lhs symbol }
	r,s:	^list	;{ OF production no }
	popno:	integer ;
	dummy, atendofrhs : boolean;
	tsy : symbol;
	tid : word;
	tsykey,i : integer;

	BEGIN
	{ parserhs }

	q := p^.firstalt ;
	IF q<> NIL THEN
	    WHILE q^.nxtalt<> NIL DO q := q^.nxtalt ;
	atendofrhs := false;

	{							    }
	{ Parse multiple right hand side rules for one non-terminal }
	{							    }

	WHILE NOT atendofrhs DO
	    BEGIN

		{ Define rhs transition node. }
		popno := 0  ;
		new (u)	;
		IF q = NIL THEN
		    p^.firstalt := u
		ELSE
		    q^.nxtalt := u ;
		q := u	;
		WITH q^ DO
		    BEGIN
		    next := NIL ;
		    def := p;
		    action := NIL;
		    nxtalt := NIL
		    END ;
		IF  prodno < maxruleno THEN
		    prodno := prodno +1
		ELSE
		    error (errtoomanyprod, 0);
		new (r)	;
		s := p^.defin;
		IF s <> NIL THEN
		    BEGIN
		    WHILE s^.cdr <> NIL DO s:= s^.cdr;
		    s^.cdr := r ;
		    END
		ELSE
		    p^.defin := r ;

		r^.cdr := NIL ;
		r^.car := prodno   ;
		ruletab[prodno]	:= q;
		q^.production := prodno ;

		{							    }
		{ Parse right hand side of one rule - up to "/", ";", or "|"}
		{							    }

		WHILE parsesy ([ident,res], optionalitem, []) DO
		BEGIN
		    popno := popno+1 ;
		    v := parseid(undterm,[nonterm,decterm,undterm]);
		    if (v^.kind = undterm) and (v^.code = 0) then
			setcode(v, -1);
		    new (t) ;
		    u^.next :=t ;
		    u	:=t ;
		    t^.action	:= v ;
		    t^.def	:= p ;
		    t^.nxtalt := NIL	;
		    t^.next	:= NIL	;
		    s := v^.usedin  ;
		    new (r) ;	r^.cdr	:= NIL ;
		    IF	s <> NIL THEN
			BEGIN
			WHILE	s^.cdr <> NIL DO s := s^.cdr	;
			s^.cdr	:= r	;
			END
		    ELSE    v^.usedin	:=  r ;
		    r^.car := prodno ;
		END ;

		poptab [prodno] := popno ;
		IF popno > maxpop THEN maxpop := popno;

		{				    }
		{ Parse optional semantics action   }
		{				    }

		IF parsesy ([slashsy], optionalitem, []) THEN
		BEGIN
		IF NOT parsesy ([ident,res], optionalitem, []) THEN
			begin { fool it into thinking it just read default }
			tsykey := sykey;
			tsy := sy;
			tid := id;
			sykey := 0;
			sy := ident;
			usedsavedlhs := usedsavedlhs + 1;
			i := usedsavedlhs div 10;
			savedlhsid.ch[savedlhsid.len-1] := chr(i+ord('0'));
			i := usedsavedlhs - i * 10;
			savedlhsid.ch[savedlhsid.len] := chr(i+ord('0'));
			{ Now it has to be hashed anew }
			for i := 1 to savedlhsid.len do
				if savedlhsid.ch[i] in smallletters
				then sykey := sykey+(ord(savedlhsid.ch[i])-32)
				else sykey := sykey+ord(savedlhsid.ch[i]);
			id := savedlhsid;
			v := parseid(semact,[semact]);
			id := tid;
			sykey := tsykey;
			sy := tsy;
			end
		    else v := parseid(semact,[semact]);


		{ Save info about semantics action }
		    lastsemact := lastsemact +1; { give it a number }
		    v^.code := lastsemact;
		    s := v^.usedin ;
		    new(r) ; r^.cdr := NIL;
		    IF s <> NIL THEN
			BEGIN
			WHILE s^.cdr <> NIL DO s :=s^.cdr;
			s^.cdr := r
			END
		    ELSE
			v^.usedin := r;
		    r^.car := prodno ;
		    q^.action := v  ;
		END;
		{								}
		{ Save number of items on right hand side of the rule.	Then 	}
		{ continue in loop if find "|" since that means there is 	}
		{ another rule.							}
		{								}

		poptab [prodno] := popno;
		IF popno > maxpop THEN maxpop := popno;

		IF NOT parsesy ([orsy], optionalitem, []) THEN
		atendofrhs := true;

	    END;	{ of WHILE not atendofrhs }

	{ Parse ";" }
	dummy := parsesy ([semisy], errprodsemi, [semisy]);

	1:

	END ;	{ parserhs }



	BEGIN
	{ readg }
	IF trace THEN
	    BEGIN
	    writeln(' Readg') ;
	    END ;

	lastsemact := 0;
	{ Parse "BEGIN" - Assume it's present if it's missing }
	dummy := parserw (beginrw, errbegin, []);
	foundend := false;

	{						}
	{ Parse all productions until find eof or "END" }
	{						}

	WHILE (sy <> eofsy) and NOT foundend DO
	    IF parserw (endrw, optionalitem, []) THEN
	    foundend := true
	    ELSE IF parsesy ([ident,res], errnontermname, [semisy]) THEN
	    BEGIN
		p := parseid(nonterm, [nonterm,undterm,decterm]);
		IF p^.kind = decterm THEN
		    error (errdupname, sypos)
		ELSE
		    BEGIN
		    p^.kind := nonterm;
		    if p^.code = 0 then setcode(p,-1);
		    END;
	    { save the lhs for possible use as a default action name	}
	    { Default action names are the names of the lhs with a	}
	    { two digit suffix.						}
		usedsavedlhs := 0;
		savedlhsid := p^.intname;
		l := savedlhsid.len + 2;
		if l > maxname
		then l:= maxname;
		savedlhsid.ch[l] :='0';
		savedlhsid.ch[l-1] :='0';
		savedlhsid.len := l;
	    IF parsesy ([eqsy], errnontermequal, [semisy]) THEN parserhs (p);
	    END;

	{							}
	{ All productions have been parsed. Check that the first}
	{ has one rule of the form: X = Y GOALSY; 		}
	{							}

	IF foundend THEN
	    BEGIN

	    { Look for optional ";" or "." }
	    dummy := parsesy ([semisy,periodsy], optionalitem, []);

	    { Now should have end of file after any comments. Try to }
	    { parse something to flush comments. }
	    dummy := parsesy ([semisy], optionalitem, []);
	    if sy <> eofsy then error (errgarbageateof, sypos);
	    END
	ELSE
	    error (erreof, 0);

	maxprodno := prodno ;
	firstprodok := false;

	IF maxprodno = 0 THEN
	    firstprodok := true		{ no productions }
	ELSE IF (poptab [1] = 2) AND (ruletab [1]^.nxtalt = NIL) THEN
	    IF ruletab[1]^.next <> NIL THEN
	    IF ruletab[1]^.next^.next <> NIL THEN
		IF ruletab[1]^.next^.next^.action <> NIL THEN
		IF (ruletab[1]^.next^.next^.action^.name.ch ='GOALSY                         ') THEN
		    firstprodok := true;
	IF not firstprodok then error (errfirstprod, 0);


    END ;
    { readg }

{***************************************************************}
{								}
{	End of routines that read and parse the input file	}
{	and create the symbol table.				}
{								}
{***************************************************************}

PROCEDURE undeclcheck(b : boolean);
	{*******************************************************}
	{							}
	{   undeclcheck						}
	{							}
	{   This prints a warning if any undeclared terminals	}
	{   are found after parsing the input, if B is TRUE.	}
	{   It also converts the names of terminals to lower	}
	{   case so they can be seen in the listing.		}
	{*******************************************************}
    VAR count,x,y:integer;
	BEGIN
	count := 0;
	IF listfl THEN writeln(outfile);
	FOR x := 0 TO maxsymbol DO
	    IF symbolname[x]<>NIL THEN
		BEGIN
		IF b and (symbolname[x]^.kind = undterm) THEN
		    BEGIN
		    count := count+1;
		    IF listfl THEN
			writeln(outfile,'*** WARNING:  Undeclared terminal: ',symbolname[x]^.name.ch);
		    writeln('Undeclared terminal: ',symbolname[x]^.name.ch);
		    END;
		IF symbolname[x]^.kind IN [undterm,decterm] THEN
		    WITH symbolname[x]^.name DO
			FOR y := 1 TO len DO
			    IF (ch[y] >= 'A') AND (ch[y] <= 'Z') THEN
				ch[y] := chr(ord(ch[y])+32);
		END;
	IF count > 0 THEN
	    BEGIN
	    writeln(count:0,' undeclared terminals found.');
	    errorflag := true;
	    END;
	END;

PROCEDURE numbersemacts;
	{*******************************************************}
	{							}
	{   This puts pointers to the semantic action ids in	}
	{   symbolname[] so they can be found by the output	}
	{   routines.						}
	{   It sets maxsymbol to the last object (one less	}
	{   than the first action) and maxcode to the last	}
	{   semantic action.					}
	{*******************************************************}
	VAR k:integer;
	    nameentry : identptrty;
	BEGIN
	maxsymbol := maxcode;
	FOR k:= 0 TO nametabsize DO
	    BEGIN
		nameentry := nametab[k];
		WHILE nameentry <> NIL DO
		    BEGIN
			IF nameentry^.kind = semact THEN
			    setcode(nameentry, nameentry^.code + maxsymbol);
			nameentry := nameentry^.nic;
		    END;
	    END;
	END;

    PROCEDURE	printg ;
	{***********************************************************}
	{							    }
	{   printg						    }
	{							    }
	{   This procedure prints out a formatted		    }
	{	grammar onto filenam.lis.			    }
	{							    }
	{***********************************************************}
    VAR
	i,count	: integer ;



	PROCEDURE printprod ( lhs : prodptrty
	    ;prodno :	integer	) ;
	    {***********************************************************}
	    {								}
	    { print prod						}
	    {								}
	    {	This procedure prints out a production.			}
	    {								}
	    {***********************************************************}
	VAR


		w : word;
	    COLUMN : INTEGER;
	    i	    : integer;
	    rhs	    : prodptrty ;



	    PROCEDURE	printrhs    ( link  :	prodptrty ) ;
		{***********************************************************}
		{							    }
		{ printrhs						    }
		{							    }
		{   This procedure prints out the right			    }
		{	    hand side of a production.			    }
		{							    }
		{***********************************************************}

	    var
		i : integer;

		BEGIN
		{ printrhs}
		IF link<> NIL
		then
		    repeat
			w := link^.action^.name;
			if column + w.len > 78 then
			    begin
				column := 27;
				writeln(outfile);
				write(outfile,'			   ')
			    end;
			write(outfile,' ');
			for i := 1 to w.len do write(outfile,w.ch[i]);
			column := column + 1 + w.len;
			link := link^.next
		    until link = nil;
		if lhs^.action <> nil then
		    begin
			column := column div 8; {tabs now}
			for i := column+1 to 9 do write(outfile,'	');
			write(outfile,'	');	{another tab}
			write(OUTFILE,'/', lhs^.action^.name.ch)
		    end;
		writeln(outfile)
		END ;
	    {	printrhs }



	    BEGIN
	    { print prod }
	    column := 27;
	    IF lhs^.def^.firstalt = lhs THEN
	    WITH lhs^.def^.name DO BEGIN
		writeln(OUTFILE);
		write(OUTFILE,prodno:6,'	');
		for i := 1 to len do
			write(OUTFILE,ch[i]);
		IF len < 8 then write(OUTFILE,'	');
		IF len <16 then write(OUTFILE,'	');
		write(outfile,' = ');
		IF len >15 then column := len+11;
	    end
	    ELSE write(OUTFILE,prodno:6,'			 | ');
	    rhs :=  lhs^.next ;
	    printrhs (rhs)  ;
	    END ;



	BEGIN
	{ print g}
	if listfl then
	    begin
		i := 12 ;
		ch := chr (i) ;
		writeln(OUTFILE,' ',ch)	;
	    end
	else for i :=1 to 3 do writeln(outfile);
	writeln(outfile,'			Grammar Listing');
	writeln(outfile,'			------- -------');
	FOR i := 1 TO maxruleno DO
	IF ruletab[i] <> NIL
	THEN printprod (ruletab[i],i) ;
	END ;
    { print g}



    PROCEDURE	printcref ;
	{***********************************************************}
	{							    }
	{   printcref						    }
	{							    }
	{   This procedure controls the printing of the		    }
	{   cross reference listing.				    }
	{							    }
	{***********************************************************}
	TYPE
	symlist =   RECORD
			    sym : ^identry  ;
			    cdr : ^symlist
		    END ;
    VAR
	head	: symlist   ;
	i   :	integer	;
	ch  :	char	;



	procedure printnumlist(startcol : integer; q : listptrty);
	    var
		col : integer;

	    begin
		col := startcol;
		WHILE	q <> NIL DO

			BEGIN
			if col > 94 then
			begin
			    writeln(outfile);
			    col := 16;
			    write(outfile,' 		');
			end;
			write(outfile,q^.car:0,'	');
			col := col + 8;
			q := q^.cdr
			END	;
	    end;

	PROCEDURE   printvoc	(idkind : setofnamety) ;
	    {***********************************************************}
	    {								}
	    {	printvoc						}
	    {								}
	    {	    This procedure prints out cross reference		}
	    {	information for terminals ,nonterminals and		}
	    {	action labels.						}
	    {								}
	    {***********************************************************}
	VAR
	    column : 1 .. 133;
	    p : ^ symlist;
	    count : integer;
	    q :	^ list;
	    namesdif : boolean;
	    BEGIN
	    {	print voc   }
	    IF trace THEN
		BEGIN
		writeln(' Voc') ;
		END ;
	    p :=    head.cdr ;
	    WHILE   p <> NIL DO
		BEGIN
		IF p^.sym^.kind IN  idkind
		THEN with p^.sym^ do
		    BEGIN
		    writeln(outfile);
		    write(outfile,' ');
		    namesdif := (name.len <> intname.len);
		    for count := 1 to name.len do
			BEGIN
			write(outfile,name.ch[count]);
			IF not namesdif THEN
			    IF name.ch[count]<>intname.ch[count] THEN
				namesdif := TRUE;
			END;
		    IF namesdif THEN
			BEGIN
			write(outfile,', ');
			FOR count := 1 to intname.len DO
			    write(outfile,intname.ch[count]);
			END;
		    writeln(outfile,'	= ',code:0);
		    if kind = nonterm then
			begin
			    q := defin;
			    write(outfile,' defined	');
			    column := 8;
			    printnumlist(column,q);
			    writeln(outfile);
			end;
		    q := usedin ;
		    write(outfile,' used	') ;
		    printnumlist(8,q);
		    writeln(outfile);
		    END ;
		p:= p^.cdr
		END ;
	    END ;
	{   print voc	}



 PROCEDURE printterm (idkind : setofnamety) ;

	{***************************************}
	{					}
	{   printterm				}
	{					}
	{   This procedure prints out the	}
	{ terminal symbol definitions.		}
	{					}
	{***************************************}

 VAR
	p : ^ symlist ;
	namesdif : boolean;
	i : integer;

 BEGIN	{ printterm }

	p := head.cdr ;

	WHILE p <> NIL DO
	BEGIN
		IF p^.sym^.kind IN idkind
		THEN with p^.sym^ do
		    BEGIN
			writeln(terpas);
			write(terpas,' ',name.ch);
			namesdif := (name.len <> intname.len);
			IF NOT namesdif THEN			
			    FOR i := 1 to name.len DO
				IF name.ch[i]<>intname.ch[i] THEN namesdif := TRUE;
			IF namesdif THEN
			    write(terpas,', ',intname.ch);
			writeln(terpas,' = ',code,' ; ') ;
		    END ;
		p := p^.cdr ;

	END ;
 close(terpas)
 END { printterm } ;



	PROCEDURE   sort ;
	    {***************************************************}
	    {							}
	    {	sort						}
	    {							}
	    {	    This procedure sorts the symbol table.	}
	    {							}
	    {***************************************************}
	LABEL
	    1,2;
	VAR
	    i,k :   integer	;
	    x	:   ^identry	;
	    p,q,r   :	^symlist    ;
	    BEGIN
	    {	sort }
	    IF trace THEN
		BEGIN
		writeln(' Sort');
		END ;
	    head.cdr := NIL ;
	    FOR k :=	0 TO nametabsize DO
		BEGIN
		x := nametab[k] ;
		WHILE x <> NIL	DO
		    BEGIN
		    new (r)	;
		    r^.sym  := x    ;
		    r^.cdr  := NIL;
		    {	enter IN chain	}
		    q	    := head.cdr	    ;
		    IF q=NIL THEN head.cdr:= r
		    ELSE
			BEGIN
			IF r^.sym^.name.ch <q^.sym^.name.ch
			THEN
			    BEGIN
			    head.cdr := r;
			    r^.cdr := q
			    END
			ELSE
			    BEGIN
			    p	    :=	q^.cdr	    ;
			    WHILE (p <> NIL)
			    DO
				BEGIN
				IF r^.sym^.name.ch < p^.sym^.name.ch THEN GOTO 1;
				q :=p	;
				p := p^.cdr
				END ;
			    1:	q^.cdr := r ;
			    r^.cdr := p
			    END ;
			END ;
		    x:= x^.nic
		    END
		END ;
	    {	FOR }
	    2:
	    END	;
	{   sort }


	BEGIN	{   print cref	}
{	mark(heap);		*********************}
	sort ;
	i :=	12  ;
	IF cref THEN BEGIN
	ch :=	chr(i)	;
	writeln(outfile,' ',ch)	    ;
	writeln(outfile,'                      Cross Reference Listing');
	writeln(outfile,'                      ----- --------- -------');
	writeln(outfile,'      Terminals') ;
	writeln(outfile);
	printvoc ([decterm,undterm])	    ;
	writeln(outfile,ch) ;
	writeln(outfile,'     Non Terminals')	;
	printvoc ([nonterm])	;
	writeln(outfile,ch) ;
	writeln(outfile,'     Action')	;
	printvoc([semact]) ;
	END; { cref }
	IF terminals THEN printterm([decterm,undterm]);
{	release	    (heap)				    }
	END ;
    {	print cref  }




    PROCEDURE printcon (t : tranptrty) ;
	{*******************************************************}
	{							}
	{   printcon						}
	{							}
	{   This procedure print a configuration in symbolic	}
	{ form.							}
	{							}
	{   in :    t	given configuration			}
	{*******************************************************}
    VAR
	x : integer ;
	col : integer;
	i : integer ;
	red,sameline : boolean ;
	p : prodptrty ;



	BEGIN
	{ print con }
	x := t^.pos ;
	p := t^.rule ;
	with p^.def^.name DO
	    BEGIN
	    for i := 1 to len do
		write(outfile,ch[i]);
	    IF len < 8 THEN write(outfile,'	');
	    IF len <16 THEN write(outfile,'	');
	    write(outfile,' = ');
	    col := 19;
	    IF len > 15 then col := len+3;
	    END;
	WHILE x <> 0 DO
	    BEGIN
	    p := p^.next ;
	    if p^.action^.name.len+col > 120 then
		begin
		col := 19;
		writeln(outfile);
		write(outfile,'		   ');
		end;
	    for i := 1 to p^.action^.name.len do
		write(outfile,p^.action^.name.ch[i]) ;
	    write(outfile,' ');
	    col := col + 1 + p^.action^.name.len;
	    x := x-1 ;
	    END ;
	write(outfile,'. ');
	p := p^.next ;
	WHILE p <> NIL DO
	    BEGIN
	    if col + p^.action^.name.len > 120 then
		begin
		col:=19;
		writeln(outfile);
		write(outfile,'		   ');
		end;
	    for i := 1 to p^.action^.name.len do
		write(outfile,p^.action^.name.ch[i]);
	    write(outfile,' ');
	    col := col + 1 + p^.action^.name.len;
	    p := p^.next
	    END ;
	writeln(outfile);
	END {	print con }	;


    PROCEDURE dumpcon (t : tranptrty) ;
	{*******************************************************}
	{							}
	{   dumpcon						}
	{							}
	{   This procedure dumps a configuration (transition)	}
	{   in symbolic	form.					}
	{							}
	{   in :    t	given configuration			}
	{*******************************************************}
    VAR
	s : stateptrty;
	spacer : packed array [1 .. 6] of CHAR;

    BEGIN
    { dumpcon }
    spacer := ' 					'; { space and 5 tabs }
    writeln(outfile, spacer, 'Dump of transition @ ', hex(t):8);
    writeln(outfile, spacer, '  next (transition)	= ', hex(t^.next):8);
    s := t^.nextstate;
    writeln(outfile, spacer, '  nextstate (ptr)	= ', hex(s):8);

    IF s <> NIL THEN
	writeln(outfile, spacer, '  next state		= ', s^.state:5);

    IF t^.marke THEN
	writeln(outfile, spacer, '  marke			=  true')
    ELSE
	writeln(outfile, spacer, '  marke			=  false');
    IF t^.markc THEN
	writeln(outfile, spacer, '  markc			=  true')
    ELSE
	writeln(outfile, spacer, '  markc			=  false');

    writeln(outfile, spacer, '    Symbolic form of transition follows:');
    write(outfile, spacer, '    ');
    printcon(t);

    END {dumpcon} ;


    procedure printaction(t:tranptrty);
	{***********************************************}
	{						}
	{   printaction					}
	{						}
	{   Prints transition actions for each state.	}
	{						}
	{***********************************************}

	var
		p: prodptrty;
		i: integer;
	begin
	p:=t^.rule;
	for i:=1 to t^.pos+1 do p:=p^.next;
	if p = nil then {reduction}
		begin
		write(outfile,' 	Reduce to ');
		for i := 1 to t^.rule^.def^.name.len do
			write(outfile,t^.rule^.def^.name.ch[i]);
		write(outfile,'	by rule ',
			t^.rule^.production:0);
		if  t^.nextstate <> nil then
			write(outfile,' looking at state ',
				t^.nextstate^.state:0);
		writeln(outfile);
		end
	else {shift}
		if t^.nextstate <> nil then
			begin
			write(outfile,' 	Shift ');
			for i := 1 to p^.action^.name.len do
				write(outfile,p^.action^.name.ch[i]);
			writeln(outfile,' 	goto ',
				t^.nextstate^.state:0);
			end;
	end; {printaction}

    PROCEDURE printstate(s :stateptrty)	;
	{**********************************************}
	{						}
	{   printstate					}
	{						}
	{   This procedure prints a state		}
	{   IN	:   s state				}
	{						}
	{**********************************************}
    VAR
	i : integer;
	t : tranptrty ;
	st : stateptrty ;
	col : integer;

	BEGIN
	{ printstate }

	writeln(outfile) ;
	write(outfile,' ---------- ');
	    CASE s^.kind OF
	    reads	: write(outfile,' Read State ');
	    inadequate	: write(outfile,' Inadequate State ');
	    lookahead	: write(outfile,' Lookahead ') ;
	    reduction	: write(outfile,' Reduction ') ;
	    lastring	: write(outfile,' Lastring ') ;
	    subgoal	: write(outfile,' Subgoal ')
	    END ;
	writeln(outfile,s^.state:0,' ----------') ;
	writeln(outfile);
	t := s^.firsttran ;
	WHILE t <> NIL DO
	    BEGIN
	    printcon(t) ;
	    t := t^.next
	    END ;
	writeln(outfile);
	t:=s^.firsttran;
	while t <> nil do
		begin
		printaction(t);
		t := t^.next
		end;

	writeln(outfile);
	t := s^.back;
	IF t <> NIL THEN
	    writeln(outfile, '   The following states have transitions to this state');
	col := 80;

	WHILE t <> NIL DO
	    BEGIN

	    col := col + 6;

	    IF col > 80 THEN
		BEGIN
		col := 11;
		writeln(outfile);
		write(outfile, '     ');		
		END
	    ELSE
		write(outfile, ',');

	    write(outfile, t^.nextstate^.state:5);

	    t := t^.next;
	    END;

	writeln(outfile);

	END { printstate }  ;



    PROCEDURE outfsm ;
	{***********************************************}
	{						}
	{   outfsm					}
	{						}
	{   This proc outputs fsm tables		}
	{						}
	{***********************************************}

    LABEL
	1;
    VAR
	i : integer ;


	BEGIN
	{ outfsm }
	i := 12 ;
	ch := chr(i) ;
	writeln(outfile,' ',ch) ;
	writeln(outfile,'                      Characteristic States');
	writeln(outfile,'                      ---------------------');
	writeln(outfile);writeln(outfile);writeln(outfile);
	FOR i := 0 TO maxstate DO
	    BEGIN
	    IF nstate[ i ] = NIL THEN GOTO 1 ;
	    printstate (nstate[i])
	    END ;
	1:
	END	{ outfsm }  ;







    PROCEDURE printtab ;
	{***************************************************}
	{						    }
	{ printtab					    }
	{						    }
	{   This procedure prints the symbol action	    }
	{   tables.					    }
	{						    }
	{***************************************************}

    VAR
	i,j : integer ;
	t,a : integer ;



	BEGIN
	{ print tab}
	i := 12 ;
	ch := chr(i);
	writeln(outfile,' ',ch);
	i := 3*index +3 ; { number OF entries}
	writeln(outfile);
	writeln(outfile,'                parsing tables');
	writeln(outfile,'                --------------');
	writeln(outfile);writeln(outfile);writeln(outfile);
	writeln(outfile,' index	match symbol		action');
	writeln(outfile,' _______________________________________________________________________________________________________');
	FOR i := 0 TO index DO WITH table[i] DO
	    BEGIN
	    write(outfile,i:6,'	');
	    IF tran = continue THEN
		write(outfile,'continue at ',action:0)
	    ELSE BEGIN
		IF tran = elsecode THEN write(outfile,'all others		')
		ELSE WITH symbolname[tran]^ DO
		    BEGIN
			FOR j := 1 TO name.len DO
				write(outfile,name.ch[j]);
			IF name.len < 8 then write(outfile,'	');
			IF name.len <16 then write(outfile,'	');
			IF name.len <24 then write(outfile,'	')
			ELSE write(outfile,' ');
		    END;
		IF action = errorcode THEN write(outfile,'Error')
		ELSE IF action > 0 THEN write(outfile,'Push; goto ',action:0)
		ELSE BEGIN
			t := -action;
			IF t > noscancode THEN
			    BEGIN
				write(outfile,'Look ahead reduce last ');
				t := t-noscancode;
			    END
			ELSE write(outfile,'Reduce ');
			IF poptab[t] <> 1 THEN
			    write(outfile,poptab[t]:0,' ');
			write(outfile,'to ');
			WITH ruletab[t]^.def^.name DO
			    FOR j := 1 TO len DO
				write(outfile, ch[j]);
			write(outfile,' by rule ',t:0);
			IF ruletab[t]^.action <> NIL THEN
			    WITH ruletab[t]^.action^.name DO
				BEGIN
				write(outfile,', perform action ');
				FOR j := 1 TO len DO
				    write(outfile, ch[j]);
				END;
		    END;
		END;
	    writeln(outfile);
	    END;
	END { print tab} ;




    FUNCTION nexttran (s :stateptrty; t : tranptrty ):tranptrty ;

	{*******************************************************}
	{							}
	{ nexttran						}
	{							}
	{   This function gets the next unmarked		}
	{ transition from state s. the current transition is	}
	{ t. It returns nil if no next transition exists.	}
	{							}
	{   in	:   s	current state				}
	{	    t	current transition			}
	{							}
	{   out	:						}
	{							}
	{   global						}
	{							}
	{ result    ptr to next transition			}
	{							}
	{******************************************************}


    LABEL
	1;
    VAR
	curtran	    : tranptrty	;

	BEGIN
	{ nexttran }
	IF trace THEN
	    BEGIN
	    writeln(' Nexttran');
	    END;
	curtran := t ;
	IF curtran = NIL THEN
	    BEGIN
	    curtran := s^.firsttran ;
	    IF curtran = NIL THEN writeln(' System error in nexttran');
	    curtran^.marke := true
	    END
	ELSE
	    BEGIN
	    WHILE   curtran^.marke DO
		BEGIN
		curtran := curtran^.next ;
		IF curtran = NIL THEN GOTO 1
		END ;
	    1:	    IF curtran = NIL THEN
		BEGIN
		curtran := s^.firsttran ;
		    REPEAT
		    curtran^.marke := false ;
		    curtran	    := curtran^.next
		    UNTIL curtran = NIL
		END
	    ELSE curtran^.marke := true
	    END ;
	nexttran := curtran
	END	{ nexttran } ;









    FUNCTION conmatch(c1,c2 : tranptrty) : boolean ;
	{*******************************************************}
	{							}
	{   conmatch						}
	{							}
	{	This function compares two configurations	}
	{   and returns the result.				}
	{	Two configurations match if everything		}
	{   beyond the dot matches incl the action.		}
	{   for the research version. otherwise usual drf	}
	{							}
	{   in	    : c1,c2 configurations			}
	{							}
	{							}
	{   global in :    ruletab				}
	{							}
	{   result	true if matches				}
	{							}
	{*******************************************************}


    LABEL
	1;
    VAR
	i1,i2,i	    : integer ;
	x1,x2	: prodptrty ;



	BEGIN
	{ conmatch	}
	i1 :=c1^.pos ;
	i2 := c2^.pos ;
	x1 := c1^.rule ;
	x2 := c2^.rule ;
	IF NOT research THEN
	IF (i1=i2) AND (x1=x2)
	THEN conmatch := true
	ELSE conmatch := false
	ELSE
	    BEGIN
	    IF (x1^.action <> x2^.action)
	    OR (i1 <> i2)
	    THEN conmatch := false
	    ELSE
		BEGIN
		FOR i := 0 TO i1 DO
		x1:= x1^.next ;
		FOR i := 0 TO i2 DO
		x2 := x2^.next ;
		WHILE (x1<>NIL) AND (x2 <> NIL) DO
		    BEGIN
		    IF x1^.action <> x2^.action
		    THEN
			BEGIN
			conmatch := false;
			GOTO 1
			END ;
		    x1:= x1^.next;
		    x2 := x2^.next
		    END ;
		IF (x1=NIL) AND (x2 = NIL)
		THEN conmatch := true
		ELSE conmatch	:= false ;
		END ;
	    END ;
	1:






	END { conmatch	}   ;



    PROCEDURE addt( s: stateptrty ; t : tranptrty) ;
	{*******************************************************}
	{							}
	{   addt						}
	{							}
	{	This procedure adds a configuration to the	}
	{ given state. It also makes sure the does not exist	}
	{ before entering it.					}
	{	IN	:s  given state				}
	{		t   given configuration			}
	{*******************************************************}


    LABEL
	1;
    VAR
	tx,ty	: tranptrty ;



	BEGIN
	{   addt }

	tx  := s^.firsttran ;
	IF tx = NIL THEN s^.firsttran := t
	ELSE
	    BEGIN
	    WHILE tx <> NIL DO
		BEGIN
		IF conmatch(tx,t) THEN GOTO 1	;
		ty := tx ;
		tx := tx^.next
		END ;
	    ty^.next	:= t ;
	    END ;
	1:
	END { addt }	;






    FUNCTION token(pr : prodptrty ;ps : integer) : identptrty ;
	{***********************************************}
	{						}
	{   token					}
	{						}
	{ This function returns a pointer to the next	}
	{ token given a rule no. and a pos num		}
	{						}
	{   in		:   ps position			}
	{		    pr production		}
	{   out						}
	{						}
	{   result	    ptr to next token		}
	{***********************************************}

    LABEL
	1;
    VAR
	x : identptrty	;
	i : integer	;
	y : prodptrty	;



	BEGIN
	{ token	    }
	token := NIL;
	y := pr ;
	FOR i := 0 TO ps
	DO
	    BEGIN
	    y := y^.next ;
	    IF y = NIL THEN
	    GOTO 1
	    END ;
	token :=y^.action ;
	1:

	END { token } ;



    PROCEDURE addl (s :stateptrty) ;
	{***************************************************}
	{						    }
	{   addl					    }
	{						    }
	{   This procedure adds a state to the list of	    }
	{   inadequate states.				    }
	{						    }
	{   in : s state				    }
	{						    }
	{***************************************************}

    VAR
	x,y : statelistptr ;



	BEGIN
	{ addl }
	new(x)	;
	x^.car := s ;
	x^.cdr := NIL ;
	IF inadeq = NIL THEN inadeq := x
	ELSE
	    BEGIN
	    y := inadeq ;
	    WHILE y^.cdr <> NIL DO y := y^.cdr ;
	    y^.cdr := x
	    END
	END { addl }	;





    PROCEDURE	cfsm (VAR   lr0 : boolean)  ;
	{*******************************************************}
	{							}
	{	cfsm						}
	{	This procedure computes the characteristic	}
	{   finite state machine. lr0 is set to false		}
	{   if any inadequate state exists. A list of		}
	{   inadequate states is maintained by gens.		}
	{							}
	{							}
	{*******************************************************}
    VAR
	s : stateptrty ;
	s1  : stateptrty ;
	sl1 : statelistptr ;
	allreduce : boolean ;



	PROCEDURE closure (a :stateptrty;c:tranptrty;
	    VAR reduce : boolean) ;
	    {***********************************************}
	    {						    }
	    {	closure					    }
	    {						    }
	    {	    This procedure performs the closure	    }
	    { function on the given configurations.	    }
	    {						    }
	    {	in	    :	a   the given state	    }
	    {		    	c   current config	    }
	    {	out	    : reduce set to true if	    }
	    {			    reduction state	    }
	    {						    }
	    {	global					    }
	    {						    }
	    {***********************************************}


	LABEL
	    1;
	VAR
	    cpos : integer  ;
	    con,crule : prodptrty ;
	    x,tran  : tranptrty	    ;
	    nt	: identptrty	;
	    FUNCTION config(pr : prodptrty ; n: integer) : tranptrty ;






		{***************************************************}
		{						    }
		{   config					    }
		{						    }
		{	This function creaes a configuration	    }
		{						    }
		{   in		:   pr	production		    }
		{		    n	position in prod	    }
		{						    }
		{   result	    ptr to new config		    }
		{						    }
		{***************************************************}


	    VAR
		x   : tranptrty	    ;

		BEGIN
		{   config  }
		new(x) ;
		WITH x^
		DO
		    BEGIN
		    rule := pr ;
		    pos := n	;
		    marke   := false ;
		    markc   := false ;
		    next := NIL ;
		    nextstate := NIL
		    END ;
		config	:= x


		END { config }	;








	    BEGIN
	    { closure	}
	    IF trace THEN
		BEGIN
		writeln(' Closure');
		END ;
	    crule   := c^.rule ;
	    cpos    := c^.pos;
	    nt	    :=	token(crule,cpos) ;
	    IF nt <> NIL
	    THEN
		BEGIN
		allreduce := false ;
		IF nt^.kind = nonterm
		THEN
		    BEGIN
		    tran := a^.firsttran ;
			REPEAT
			IF (nt =tran^.rule^.def) AND
			(tran^.pos = 0) THEN GOTO 1;
			tran := tran^.next
			UNTIL tran = NIL ;
		    con := nt^.firstalt;
		    WHILE con <> NIL DO
			BEGIN
			x := config(con,0);
			addt(a,x) ;
			con := con^.nxtalt
			END
		    END
		END
	    ELSE reduce := true ;
	    1:
	    END { closure   }	;



	FUNCTION cis : stateptrty ;
	    {***************************************************}
	    {							}
	    {	cis						}
	    {							}
	    { This procedure sets up the initial state and	}
	    { also checks to see if the first production	}
	    { was appropriately set. if not it gives an error	}
	    { it returns a pointer to the first state		}
	    {							}
	    {	global	    in:	ruletab				}
	    {			poptab				}
	    {							}
	    {		out :	statetab			}
	    {			nstate,currentstate		}
	    {							}
	    {							}
	    {	result	    pointer to first state		}
	    {							}
	    {***************************************************}
	VAR
	    x	: stateptrty ;
	    y	: tranptrty ;
	    i	: integer ;
	    b	: boolean ;
	    BEGIN
	    {	cis }
	    IF trace THEN
		BEGIN
		writeln(' Cis');
		END ;
	    i := 0 ;
	    new(x) ;
	    new(y) ;
	    currentstate := 0 ;
	    statetab[0] := x ;
	    WITH x^ DO
		BEGIN
		firsttran := y ;
		back	    := NIL ;
		nic	    := NIL ;
		state	    := 0 ;
		kind	    := reads
		END ;
	    nstate[ 0] := x;
	    WITH y^ DO
		BEGIN
		next := NIL ;
		nextstate := NIL ;
		rule	    := ruletab[1] ;
		pos	    := 0 ;
		marke	    := false
		END ;
	    i	:= 1 ;
	    cis := x ;
	    b := false ;
	    allreduce := true ;
	    y := NIL ;
		y := nexttran(x,y) ;
		WHILE y <> NIL DO
		BEGIN
		closure( x,y,b) ;
		i := i+1 ;
		y := nexttran(x,y) ;
		END ;
	    IF b THEN IF i= 1 THEN x^.kind := reduction
	    ELSE
		BEGIN
		addl(x) ;
		x^.kind := inadequate ;
		END ;
	    END	{ cis }	    ;




	PROCEDURE gens (s : stateptrty ; VAR lr0: boolean) ;

	    {***************************************************}
	    {							}
	    {	gens						}
	    {							}
	    { This procedure generates the state machine	}
	    { recursively. It sets lr0 to false if necessary.	}
	    {							}
	    {	in	: s current state			}
	    {							}
	    {	out	: lr0	indicates if machine is lr0	}
	    {							}
	    {	global						}
	    {***************************************************}


	VAR
	    z : prodptrty   ;
	    i : integer	    ; { control	variable}
	    x : tranptrty	;
	    y : stateptrty  ;
	    b : boolean	    ;












	    FUNCTION chash( s: stateptrty) : integer ;
		{***********************************************}
		{						}
		{   chash					}
		{						}
		{	This procedure produces a key. It uses	}
		{ thash to produce the key.			}
		{						}
		{   in	: s state				}
		{						}
		{***********************************************}
	    VAR
		key	: integer   ;
		t	: tranptrty ;



		FUNCTION thash( x: tranptrty) : integer ;
		    {***************************************************}
		    {							}
		    { thash						}
		    {							}
		    {	This function returns an integer representing	}
		    { a configuration					}
		    {							}
		    {	in :	x transition				}
		    {							}
		    {							}
		    {***************************************************}

		VAR
		    y	: prodptrty ;
		    i	: integer ;
		    tkey : integer ;



		    FUNCTION number(s: identptrty): integer ;
			{***********************************************}
			{						}
			{   number					}
			{						}
			{   This function returns an integer based on	}
			{ the value of the identifier.			}
			{						}
			{   in : s identifier				}
			{***********************************************}
		    VAR
			y : word ;
			i   : integer ;
			tally : integer ;
			BEGIN
			{ number }
			tally := 0 ;
			IF s <> NIL THEN
			    BEGIN
			    y := s^.name ;
			    FOR	i := 1 TO maxname DO
			    tally := tally +ord(y.ch[i])
			    END	;
			number := tally
			END	{ number }  ;


		    BEGIN
		    { thash }
		    y := x^.rule ;
		    tkey := number(y^.action) ;
		    FOR i := 0 TO x^.pos DO y := y^.next ;
		    IF y <> NIL THEN
			tkey := tkey+number(y^.action) ;
		    thash := tkey
		    END	    { thash }	;





		BEGIN
		{ chash }
		key := 0 ;
		t := s^.firsttran ;
		WHILE t <> NIL DO
		    BEGIN
		    key := key +thash(t) ;
		    t := t^.next
		    END ;
		chash := key
		END ;
	    { chash}






	    FUNCTION complete ( s : stateptrty; t : tranptrty) :
		stateptrty ;

		{*******************************************************}
		{							}
		{ complete						}
		{							}
		{ This function creates a new state and completes it	}
		{ it returns a pointer to the completed state.		}
		{							}
		{   in	    :	s   current state			}
		{		t   transition state			}
		{   out	    :						}
		{							}
		{   global						}
		{							}
		{   result  ptr to completed state			}
		{*******************************************************}

	    VAR
		a : stateptrty ;
		c : tranptrty ;
		reduce : boolean    ;
		i   : integer	;



		FUNCTION collect (s :stateptrty;t : tranptrty):stateptrty ;
		    {***************************************************}
		    {							}
		    {	collect						}
		    {							}
		    {	This function creates a new state for the	}
		    { given transition. it also collects all		}
		    { transitions which have the same transition	}
		    { token and adds it to the state.			}
		    {							}
		    {	in	:   s	current state			}
		    {		    t	current transition		}
		    {							}
		    {	out	:					}
		    {							}
		    {	global						}
		    {							}
		    {	result	ptr to new state			}
		    {***************************************************}

		VAR
		    x,z : tranptrty ;
		    y : stateptrty ;



		    FUNCTION tranmatch(t1,t2 : tranptrty) : boolean ;
			{***********************************************}
			{						}
			{   tranmatch					}
			{						}
			{   This function checks to see if the transition}
			{   token for the given two configurations	}
			{   is the same.				}
			{						}
			{   in	    : t1,t2	configurations		}
			{						}
			{   out	    :					}
			{						}
			{   global					}
			{						}
			{   result is true if they match		}
			{***********************************************}


		    VAR
			r : prodptrty ;
			i   : integer	;
			s1,s2 : identptrty ;



			BEGIN
			{ tranmatch }
			r   := t1^.rule	;
			FOR i := 0 TO t1^.pos DO
			    r := r^.next ;
			IF r = NIL THEN s1 := NIL
			ELSE s1 := r^.action ;

			r := t2^.rule ;
			FOR i := 0 TO t2^.pos
			DO r := r^.next ;
			IF r = NIL THEN	s2 := NIL
			ELSE	s2 := r^.action	;


			tranmatch := s1=s2  ;

			END	{ tranmatch }	    ;



		    FUNCTION creates (s:stateptrty; t: tranptrty):stateptrty ;
			{*******************************************************}
			{							}
			{   creates						}
			{							}
			{   This function creates a new state with a single	}
			{ configuration t. It does not enter the state		}
			{ into	the state table	    since the state completed	}
			{ may exist already.					}
			{							}
			{   It returns a pointer to the newly created state.	}
			{							}
			{   in		:   s	parent state			}
			{		    t	current configuration		}
			{							}
			{   out		:					}
			{							}
			{   global						}
			{							}
			{   result	ptr to new state			}
			{*******************************************************}



		    VAR
			x   : stateptrty ;



			BEGIN
			{ creates   }
			new(x)	    ;
			WITH x^ DO
			    BEGIN
			    firsttran := t;
			    nic	    := NIL ;
			    back    := NIL ;
			    state := 0	    ;
			    kind	:=reads
			    END ;


			creates := x
			END	{ creates }	;




		    FUNCTION	movedot (t : tranptrty) : tranptrty ;
			{*******************************************************}
			{							}
			{   movedot						}
			{							}
			{	This function creates a new configuration	}
			{ with the position of the dot moved over.		}
			{   It returns a pointer to this transition		}
			{ It does not watchout for reductions. This id		}
			{ by the closure function.				}
			{							}
			{	in	:   t	current config			}
			{							}
			{	out	:					}
			{							}
			{	global						}
			{							}
			{	result	ptr to new configuration		}
			{*******************************************************}


		    VAR
			x   : tranptrty ;


			BEGIN
			{ movedot }
			new(x)	;
			WITH x^ DO
			    BEGIN
			    marke := false ;
			    markc := false ;
			    next := NIL ;
			    nextstate := NIL	;
			    rule := t^.rule ;
			    pos := t^.pos +1
			    END ;


			movedot	:= x
			END { move dot	}	;








		    BEGIN
		    { collect	}
		    x := movedot (t) ;
		    y := creates (s,x)	;
		    z := t^.next ;
		    WHILE z <> NIL DO
			BEGIN
			IF tranmatch (z,t)
			THEN
			    BEGIN
			    z^.marke := true;
			    x := movedot(z) ;
			    addt(y,x)
			    END ;
			z := z^.next ;
			END ;
		    collect := y
		    END	    { collect	}   ;








		BEGIN
		{ complete }
		IF trace THEN
		    BEGIN
		    writeln(' Complete');
		    END ;
		a := collect(s,t) ;
		c := nexttran( a,NIL) ;
		allreduce := true ;
		reduce := false ;
		i := 0	;
		WHILE c <> NIL DO
		    BEGIN
		    closure(a,c,reduce) ;
		    i	:= i+1 ;
		    c := nexttran(a,c)
		    END ;
		IF reduce THEN IF i=1 THEN a^.kind := reduction
		ELSE
		    BEGIN
		    a^.kind := inadequate ;
		    lr0	:= false
		    END ;
		complete := a
		END { complete }    ;

	    PROCEDURE finds(VAR s: stateptrty;VAR found:boolean) ;
		{***************************************************}
		{						    }
		{   finds					    }
		{						    }
		{   This procedure checks to see if the given	    }
		{ state already exists. If so it updates s to	    }
		{ point to the existing state. In this case found   }
		{ is set to true.				    }
		{						    }
		{	in:					    }
		{						    }
		{   out : s	points to found state		    }
		{	found	true if state is found		    }
		{						    }
		{***************************************************}

	    VAR
		key	: integer ;
		x	: stateptrty ;



		FUNCTION statematch(s1,s2:stateptrty) : boolean ;
		    {***************************************************}
		    {							}
		    {	statematch					}
		    {							}
		    {	This procedure checks to see if the given	}
		    { states match.					}
		    {							}
		    {	in:	s1,s2 the states to be matched		}
		    {							}
		    {***************************************************}

		VAR
		    x,x2    : tranptrty ;
		    a,b	: integer   ;
		    match   : boolean	;

		    BEGIN
		    { statematch }
		    statematch := false ;
		    a := 0  ;
		    x := s1^.firsttran	;
		    WHILE x <> NIL DO
			BEGIN
			a := a+1 ;
			x := x^.next ;
			END ;

		    b := 0 ;
		    x := s2^.firsttran ;
		    WHILE x <> NIL DO
			BEGIN
			b := b+1 ;
			x := x^.next ;
			END ;
		    IF a =b
		    THEN
			BEGIN
			x := s1^.firsttran ;
			match := true ;
			WHILE (x <> NIL) AND match DO
			    BEGIN
			    match := false ;
			    x2 := s2^.firsttran ;
			    WHILE (x2 <> NIL) AND NOT match DO
				BEGIN
				IF conmatch(x,x2) THEN match := true ;
				x2 := x2^.next
				END ;
			    x := x^.next
			    END ;
			statematch := match
			END
		    END	{ statematch }	;



		BEGIN
		{ finds }
		IF trace
		THEN
		    BEGIN
		    writeln(' Finds');
		    END;
		found := false ;
		key	:= chash(s) ;
		key := key MOD statetabsize ;
		x	:= statetab[key ] ;
		WHILE (x <> NIL) AND (NOT found) DO
		    BEGIN
		    IF statematch(x,s)
		    THEN
			BEGIN
			found := true ;
			s	:= x
			END ;
		    x := x^.nic
		    END
		END { finds	} ;





	    PROCEDURE links(from : stateptrty ;
		on  : tranptrty;
		dest	: stateptrty	) ;
		{*******************************************************}
		{							}
		{   links						}
		{							}
		{   This procedure links the from and to states		}
		{ bothways						}
		{							}
		{   in :	from state				}
		{		dest	state				}
		{		on  transition				}
		{							}
		{*******************************************************}

	    VAR
		x,y,z	: tranptrty ;



		BEGIN
		{ links }
		on^.nextstate := dest ;
		new(x)	    ;
		WITH x^ DO
		    BEGIN
		    next := NIL ;
		    marke := false;
		    markc := false;
		    pos	    := on^.pos ;
		    nextstate := from ;
		    rule    := on^.rule
		    END ;
		y := dest^.back ;
		IF y= NIL THEN dest^.back := x
		ELSE
		    BEGIN
		    WHILE y <> NIL DO
			BEGIN
			z := y ;
			y := y^.next
			END;
		    z^.next := x
		    END
		END { links }	;


	    PROCEDURE enters ( from:stateptrty; on:tranptrty;dest:stateptrty);
		{***************************************************}
		{						    }
		{   enters					    }
		{						    }
		{   This procedure enters a new state and then	    }
		{ links the from and dest states.		    }
		{						    }
		{   in	:   from    state			    }
		{	    dest    state			    }
		{						    }
		{	    on	    transition			    }
		{						    }
		{***************************************************}

	    VAR
		key :	integer ;
		x,z : stateptrty    ;



		BEGIN
		{ enters }
		IF trace
		THEN
		    BEGIN
		    writeln(' Enters');
		    END;
		key := chash(dest)  ;
		key := key  MOD statetabsize ;
		IF currentstate >= maxstate THEN
		    BEGIN
		    writeln(' Too many states');
		    currentstate := maxstate ;
		    END
		ELSE		    currentstate := currentstate+1 ;
		nstate[ currentstate ] := dest ;
		dest^.state := currentstate ;
		IF statetab[key] = NIL THEN statetab[key] := dest
		ELSE
		    BEGIN
		    x := statetab[key ] ;
		    WHILE x <> NIL DO
			BEGIN
			z := x ;
			x := x^.nic
			END ;
		    z^.nic := dest
		    END ;
		links( from,on,dest)
		END	{enters }   ;


	    BEGIN
	    { gens  }
	    IF trace THEN
		BEGIN
		writeln(' Gens');
		END ;
	    IF debug THEN IF fsm THEN printstate(s) ;
	    x := nexttran (s,NIL) ;
	    WHILE x <> NIL DO
		BEGIN
		z := x^.rule ;
		FOR i := 0 TO x^.pos
		DO  z := z^.next ;
		IF z <> NIL THEN
		    BEGIN
{		    mark(heap) ;		*****************}
		    y := complete (s,x) ;
		    finds(y,b) ;
		    IF b THEN
			BEGIN
{			release(heap) ; { no new state was needed }
			links (s,x,y)
			END
		    ELSE
			BEGIN
			enters(s,x,y) ;
			IF y^.kind =inadequate THEN
			    BEGIN
			    addl(y) ;
			    IF allreduce THEN y^.kind:= reduction ;
			    END ;
			IF y^.kind <> reduction THEN
			gens(y,lr0)
			END ;
		    END ;
		x := nexttran(s,x)
		END
	    END	{ gens	    }	;





	BEGIN
	IF trace THEN
	    BEGIN
	    writeln(' Cfsm') ;
	    END ;
	writeln(' Building the LR(0) machine');
	lr0 := true ;
	s   := cis ; { create initial state }
	gens(s,lr0) ;
	IF debug THEN
	    BEGIN
	    sl1 := inadeq ;
	    WHILE sl1 <> NIL DO
		BEGIN
		s1 := sl1^.car ;
		printstate(s1);
		sl1 := sl1^.cdr ;
		END ;
	    END ;
	END { cfsm } ;

 


    PROCEDURE lalr ( k: integer ; var lrk : boolean ) ;
	{***********************************************}
	{						}
	{   lalr					}
	{						}
	{   This procedure attempts to resolve		}
	{ all inadequate states, using a		}
	{ maximum of k lookahead. It sets		}
	{ lrk to false if this is not possible		}
	{ for the given k.				}
	{						}
	{						}
	{   global	in  :				}
	{		out :				}
	{						}
	{***********************************************}
    VAR
	y, resolvestate : stateptrty ;
	i : integer;
	x,z : statelistptr ;
	resolvedstatey : boolean;



	FUNCTION disjoint( x,y :stateptrty; var l:stateptrty) : boolean ;
	    {*******************************************************}
	    {							    }
	    {	disjoint					    }
	    {							    }
	    {	    This function checks to see if the lookahead    }
	    { string of y is sufficient to resolve state x	    }
	    { if so it returns true.				    }
	    {							    }
	    {	    in	:   x	inadequate state		    }
	    {		    y	lookahead string state		    }
	    {							    }
	    {	    out	:   result true if disjoint else false	    }
	    {		    l	conflicting lookahead state	    }
	    {*******************************************************}

	VAR
	    check : boolean ;
	    id,id2 : identptrty ;
	    s	: stateptrty ;
	    q,r,t : tranptrty;



	    BEGIN
	    { disjoint }
	    disjoint := true ;
	    check := true ;
	    t := y^.firsttran ;
	    WHILE ( t <> NIL) AND check DO
		BEGIN
		id := token(t^.rule,t^.pos);
		q := x^.firsttran ;
		WHILE (q <> NIL) AND check DO
		    BEGIN
		    id2 := token(q^.rule,q^.pos);
		    IF id2<> NIL THEN
			BEGIN
			IF id2=id THEN check := false;
			END
		    ELSE
			BEGIN
			s := q^.nextstate ;
			IF (s<> NIL) AND(s<>y)
			THEN
			    BEGIN
			    r := s^.firsttran ;
			    WHILE (r<> NIL) AND check DO
				BEGIN
				IF token(r^.rule,r^.pos)=id THEN
					begin
					check := false;
					l := s;
					end;
				r := r^.next ;
				END ;
			    END ;
			END ;
		    q := q^.next ;
		    END ;
		t := t^.next ;
		END ;
	    disjoint := check ;
	    END	{ disjoint  }	;


	PROCEDURE follow(a : stateptrty; b :tranptrty)
		;FORWARD;

	PROCEDURE   resolve( y : stateptrty ;	VAR b : boolean) ;

	    {***************************************************}
	    {							}
	    {	resolve						}
	    {							}
	    {	This procedure resolves any inadequacies.	}
	    { It is called for each inadequate state.		}
	    { If unable to resolve a state it sets b to false.	}
	    {							}
	    {	in  :	y   given state				}
	    {							}
	    {	out :	b   set to false if unable to resolve	}
	    {							}
	    {***************************************************}

	VAR
	    id : identptrty ;
	    x,z : statelistptr ;
	    i	: integer ;
	    t,u : tranptrty ;
	    l,q : stateptrty ;
	    saved_trace, saved_trresolve : boolean;

		PROCEDURE clear_forward_marke;

		    {***************************************************}
		    {							}
		    {	clear_forward_marke				}
		    {							}
		    {	This procedure clears the marke flag in all	}
		    {	forward transitions for which is is set during	}
		    {	the resolution of one inadequate state.		}
		    {							}
		    {***************************************************}

		BEGIN
		{ clear_forward_marke }

		    IF currentvisit >0 THEN
		    FOR i := 1 TO currentvisit DO
			BEGIN
			visit[i]^.marke := false ;
			visit [i] := NIL ;
			END ;
		    currentvisit := 0;

		END { clear_forward_marke };
		
	    
	    BEGIN
	    { resolve }
	    saved_trace := trace;
	    saved_trresolve := trresolve;
	    IF resolve_display_state = y^.state then
		BEGIN
		trace := true;
		trresolve := true;
		END;

	    IF trace THEN
		BEGIN
		writeln(' Resolve');
		END ;
	    t := y^.firsttran ;
	    WHILE t <> NIL DO
		BEGIN
		id := token( t^.rule,t^.pos) ;
		IF id = NIL THEN
		    BEGIN
		    l := nil;
		    new(current_lastring_state) ;
		    WITH current_lastring_state^ DO
			BEGIN
			firsttran := NIL ;
			back := NIL ;
			nic := NIL ;
			currentstate := currentstate+1 ;
			IF currentstate > maxstate THEN
			    BEGIN
			    writeln(' Too many states') ;
			    currentstate := maxstate ;
			    END	;
			state := currentstate ;
			kind := lastring ;
			END ;
		    nstate[ currentstate ] := current_lastring_state ;
		    if trresolve then writeln(outfile, ' Lookahead computation:');
		    follow (y,t) ;
		    clear_forward_marke;
		    t^.nextstate := current_lastring_state ;
		    if current_lastring_state^.firsttran = nil then
			begin	{ lookahead set is empty }
			writeln (' *** PAT internal error in resolve.  Lookahead state ', 
				current_lastring_state^.state:4);
			writeln (' is empty.  Attempting to resolve state ', resolvestate^.state:4);
			writeln (outfile,' *** PAT internal error in resolve.  Lookahead state ', 
				current_lastring_state^.state:4);
			writeln (outfile,' is empty.  Attempting to resolve state ', resolvestate^.state:4);

			writeln (outfile,' Retry with trace on');

			printstate(resolvestate);
			saved_trace := trace;
			saved_trresolve := trresolve;
			trace := true;
			trresolve := true;
		        if trresolve then writeln(outfile, ' Lookahead computation:');
		        follow (y,t) ;
			clear_forward_marke;
			end;
		    IF debug THEN
			BEGIN
			writeln; writeln;
			writeln(outfile,' Inadequate state ') ;
			writeln;
			printstate(y) ;
			writeln;
			writeln(outfile,' Lookahead string') ;
			printstate(current_lastring_state) ;
			END ;
		    IF NOT disjoint(y,current_lastring_state,l) THEN
			BEGIN
			b := false ;
			z := conflict ;
			new(x) ;
			x^.car := current_lastring_state ;
			x^.cdr := NIL ;
			IF z=NIL THEN conflict := x
			ELSE
			    BEGIN
			    WHILE z^.cdr <> NIL DO
			    z := z^.cdr ;
			    z^.cdr := x ;
			    END ;
			if l <> nil then
			begin
			z := conflict;
			while z <> nil do
				if z^.car = l then z := nil
				else if z^.cdr = nil then
				    begin
				    new(x);
				    x^.car := l;
				    x^.cdr := nil;
				    z^.cdr := x;
				    z	    := nil;
				    end
				else z := z^.cdr;
			end;
			END ;
		    END ;
		t := t^.next ;
		END ;
		if b then
		    y^.kind := lookahead
		else
		    y^.kind := inadequate;

	    trace := saved_trace;
	    trresolve := saved_trresolve;

	    END { resolve   } ;




	PROCEDURE reverse (a:stateptrty; b :tranptrty; n:integer);

	    {***********************************************}
	    {						    }
	    { reverse					    }
	    {						    }
	    {	This procedure executes the reduction and   }
	    { makes the appropriate transition.		    }
	    {						    }
	    {	in  :	a   	base state		    }
	    {		b   	reduction		    }
	    {		n   	number of backups to go	    }
	    {						    }
	    {***********************************************}

	LABEL 1;
	VAR
	    id : identptrty ;
	    a1 : stateptrty ;
	    t,t1 : tranptrty ;

	    FUNCTION maketran( a:stateptrty; b :tranptrty ) : stateptrty ;
		{*******************************************************}
		{							}
		{ maketran						}
		{							}
		{   This function makes the appropriate transition	}
		{ for the given reduction b and returns the resultant	}
		{ state.						}
		{							}
		{   in	:   a state					}
		{	    b given reduction				}
		{							}
		{   result  is the destination state			}
		{							}
		{*******************************************************}

	    LABEL
		1;
	    VAR
		x : tranptrty ;
		new : stateptrty ;
		id : identptrty ;



		BEGIN
		{ maketran }
		new := NIL ;
		id := b^.rule^.def ;
		x := a^.firsttran ;
		WHILE x <> NIL DO
		    BEGIN
		    IF token(x^.rule,x^.pos)= id
		    THEN
			BEGIN
			new := x^.nextstate ;
			IF new <> NIL THEN GOTO 1 ;
			END ;
		    x := x^.next ;
		    END ;
		1:
		maketran := new ;
		if trace then 
		    BEGIN
		    writeln (outfile,' Maketran:  Transition to state ', 
			new^.state:4, ' via transition @ ', hex(x):8);
		    dumpcon(x);
		    END;

		END { maketran } ;

	    BEGIN
	    { reverse	}
	    IF trace THEN
		BEGIN
		writeln(outfile);
		writeln(outfile, ' *******');
		writeln(outfile, ' Reverse');
		writeln(outfile, '  a (state number)		= ', a^.state:4);
		writeln(outfile, '  b (reduction transition)	= ', hex(b):8);
		writeln(outfile, '  n				= ', n:4);
		dumpcon(b);
		END ;

	    { Call reverse recursively until "pop" the appropriate
	      number of states.  Pops are done via back transitions
	      linked via the back field in a state (statety) and the next
	      field in transitions (tranty).   Then make the 
	      transition on the non-terminal on the left hand side
	      of a production.  }

	    IF n=0 THEN
		BEGIN
		a1 := maketran(a,b) ;
		IF a1 = NIL THEN writeln(' System error in reverse')
		ELSE
		    BEGIN

		    {	Now having done the transition - collect the
			lookahead symbols for each transition from
			the new state (a1) by calling follow.

			However, if the transition is a reduction
			(id = NIL), then simulate the reduction.  }

		    t := a1^.firsttran ;
		    WHILE t <> NIL DO
			BEGIN
			id := token(t^.rule,t^.pos);
			if trresolve then
			    writeln(outfile,'  forward from	',a^.state:4,
				' to ',a1^.state:4);
			IF id <> nil THEN
			    BEGIN
			    IF not t^.marke THEN follow(a1,t);
			    END
			ELSE
			begin

			{ Now we are going to simulate the reduction.
			  Based on parameter names, we have

				state a ---- transition ----> state a1

			  We mark the back link (that indicates how
			  we got to state a1 from a so permit it
			  to be visited again (by turning off marke).
			  Note that you may get to state a1 via other
			  paths, but you don't want to back up over
			  then since you might find lookahead symbols
			  that are not valid in the particular context.
			  Thus, the back links to states other than a
			  are marked with a true marke.

			  The markc bit is set for the link from
			  a1 back to a to avoid loops.  }

			t1 := a1^.back;
			while t1 <> nil do
				begin
				if trace THEN dumpcon(t1);
				if t1^.nextstate = a then
				    begin
				    IF trace THEN writeln(outfile, 
					'      Clear marke @ ', hex(t1):8); 
				    t1^.marke := false; { since we might be visiting the state again }
				    if t1^.markc then goto 1	{remember we have
								entered this state by way of this transition}
				    else 
					BEGIN
				        IF trace THEN writeln(outfile, 
					    '      Set   markc @ ', hex(t1):8); 
					t1^.markc := true;
					END
				    end
				else
				    BEGIN
				    IF trace THEN writeln(outfile, 
				        '      Set   marke @ ', hex(t1):8); 
				    t1^.marke := true; {remember how not to leave}
				    END;
				t1 := t1^.next;
				end;

			    IF not t^.marke THEN follow(a1,t);

			    t1 := a1^.back;
			    while t1 <> nil do
				begin
				IF trace THEN writeln(outfile, 
				    '      Clear marks @ ', hex(t1):8); 
				t1^.markc := false;
				t1^.marke := false;
				t1 := t1^.next;
				end;
			    end;
			t := t^.next ;

			END ;
		    END
		END
	    ELSE
		BEGIN
		t := a^.back ;
		WHILE t <> NIL DO
		    BEGIN
		    a1 := t^.nextstate ;
		    if	not t^.marke then
			begin
			if trresolve then 
			    BEGIN
			    writeln(outfile,'  back from	',
			        a^.state:4,' to ',a1^.state:4, 
			        ' via transition @ ', hex(t):8);
			    dumpcon(t);
			    END;
			reverse(a1,b,n-1) ;
			end;
		    t := t^.next ;
		    END ;
		END ;
	1:
	    END { reverse   } ;


	PROCEDURE follow ;
	    {***************************************************}
	    {							}
	    { follow						}
	    {							}
	    { This is a forward procedure.			}
	    {	    This procedure initializes c with the	}
	    { terminals that follow.				}
	    {							}
	    {	in  :	a   base state				}
	    {		b   given transition			}
	    {							}
	    { This routine assumes that b^.marke is false.	}
	    {							}
	    {***************************************************}


	VAR
	    id	: identptrty ;
	    x	: stateptrty ;
	    plhs, p : prodptrty ;
	    pop : integer   ;
	    t	: tranptrty ;



	    PROCEDURE addlook (q : tranptrty);
		{*******************************************************}
		{							}
		{   addlook						}
		{							}
		{	This procedure adds the given transition	}
		{ to the existing lookahead string as represented by	}
		{ the implicit input:  CURRENT_LASTRING_STATE.		}
		{							}
		{   in	:   q	new transition				}
		{							}
		{*******************************************************}

	    VAR
		id : identptrty;
		x : tranptrty ;

		BEGIN
		{ addlook }
		new(x)	;
		WITH x^ DO
		    BEGIN
		    rule := q^.rule ;
		    pos	:= q^.pos   ;
		    marke := false  ;
		    markc := false  ;
		    next    := NIL  ;
		    nextstate := NIL ;
		    END ;
		if trresolve then
		begin
		    id := token(q^.rule,q^.pos);
		    writeln(outfile,' 		look ', id^.name.ch);
		end;
		addt(current_lastring_state,x)   ;
		END { addlook	} ;



	    PROCEDURE set_forward_marke ( t : tranptrty);
		{*******************************************************}
		{							}
		{   set_forward_marke					}
		{							}
		{	This procedure sets the marke bit in a 		}
		{	transition and remembers the bit has been	}
		{	set so that it can be cleared at the end of	}
		{	resolution of the state being resolved.		}
		{							}
		{   in	:   t	transition
		{							}
		{*******************************************************}

		BEGIN
		{ set_forward_marke }

	            IF trace THEN writeln(outfile, 
		        '      Set   marke @ ', hex(t):8); 
		    t^.marke := true ;
		    currentvisit := currentvisit+1 ;
		    IF currentvisit > maxvisit THEN
		        writeln(' Overflow of visit states')
		    ELSE 
			visit[currentvisit] := t ;

		END { set_forward_marke	} ;



	    BEGIN
	    { follow }
	    IF trace THEN
		BEGIN
		writeln(outfile, ' Follow');
		writeln(outfile, '  a (state number)		= ', a^.state:4);
		writeln(outfile, '  b (reduction transition)	= ', hex(b):8);
		dumpcon(b);
		END ;
	    id := token(b^.rule,b^.pos) ;

	    {	If the marke bit is on the the (forward) transition 
		node, we have already collected the lookahead symbols.
		This routine assumes it is not called if this bit is on. }

		IF id = NIL THEN
		    BEGIN

		    {	This is a reduction.  Set the forward marke to
			prevent looping if this is a recursive production,
			as in

				NT = NT ',' T ;

			Then call reverse to see what can follow if we 
			do this reduction.  }

		    pop := 0 ;
		    plhs := b^.rule ;
		    p := plhs^.next ;
		    WHILE p <> NIL
		    DO
			BEGIN
			if p^.action = p^.def then set_forward_marke(b);
			pop := pop +1 ;
			p := p^.next ;
			END ;
		    reverse(a,b,pop);
		    END
		ELSE
		    BEGIN

		    {	Add symbol to lookahead set if it's a terminal.
			Turn on marke bit to remember that already did 
			so.  The marke bits are cleared in resolve
			after resolution of a state has been completed. }

	 	    IF id^.kind in [decterm, undterm] THEN addlook(b) ;

		    set_forward_marke(b);

		    END;

	    END { follow }  ;


	BEGIN
	{   lalr }
	IF trace THEN
	    BEGIN
	    writeln(' Lalr') ;
	    END ;
	writeln(' Resolving inadequate states');

	currentvisit := 0;
	z := inadeq ;
	x := inadeq ;
	lrk := true ;
	WHILE x <> NIL DO
	    BEGIN
	    y := x^.car ;
	    resolvestate := y;
	    if trresolve then writeln(outfile,' 		Attempting to resolve state ',y^.state);
	    resolvedstatey := true;
	    resolve(y,resolvedstatey) ;
	    if trresolve then if resolvedstatey then
		writeln(outfile,' 		Resolved state ',y^.state)
		else writeln(outfile,' 		Failed to resolve state ',y^.state);
	    if resolvedstatey then
		IF x= inadeq THEN
		    inadeq := x^.cdr
		ELSE
		    z^.cdr := x^.cdr
	    ELSE
		begin
		lrk := false;
		writeln(' State ',y^.state:4,' is inadequate');
		z := x ;
		end;
	    x := x^.cdr ;
	    END ;
	IF NOT lrk THEN
	    BEGIN
	    writeln(' Grammar is not lalr(1)');
		i := 12 ;
		ch := chr(i) ;
		writeln(outfile,ch) ;
		writeln(outfile,'                      conflict states');
		writeln(outfile,'                      ===============');
		writeln(outfile);writeln(outfile);writeln(outfile);
		x := inadeq ;
		WHILE x <> NIL DO
		    BEGIN
		    printstate(x^.car) ;
		    x := x^.cdr ;
		    END ;
		x := conflict ;
		WHILE x <> NIL DO
		    BEGIN
		    printstate(x^.car) ;
		    x := x^.cdr ;
		    END ;
	    END { lrk}	;

	END ;
    {	lalr }

    FUNCTION findrule(t: tranptrty) : integer ;

	{*******************************************************}
	{							}
	{ findrule						}
	{							}
	{	This function returns the rule number of the	}
	{ given production.					}
	{							}
	{	in  : 	t   given transition			}
	{							}
	{	out :						}
	{	result	rule no. of production			}
	{*******************************************************}

    LABEL
	1;
    VAR
	l : ^list   ;

	BEGIN
	{ findrule }
	findrule := unknown ;
	l := t^.rule^.def^.defin ;
	IF l= NIL   THEN writeln(' System error') ;
	WHILE l<> NIL DO
	    BEGIN
	    IF ruletab[l^.car]=t^.rule THEN
		BEGIN
		findrule := l^.car ;
		GOTO 1
		END ;
	    l := l^.cdr ;
	    END ;
	1:
	END { findrule	} ;



    PROCEDURE	outt	;
	{***********************************************}
	{						}
	{	outt					}
	{						}
	{ This procedure generates the parser tables,	}
	{ production number tables and the semantics	}
	{ tables.					}
	{***********************************************}



	PROCEDURE gentab ;
	    {***********************************************}
	    {						    }
	    { gentab					    }
	    {						    }
	    {	This procedure generates tables in the	    }
	    { required form viz merging symbols and the	    }
	    { the action list wherever possible.	    }
	    {						    }
	    {***********************************************}
	LABEL
	    1;
	VAR
	    i,j	: integer ;
	    s : stateptrty ;
	    entry : integer ;
	    done : boolean ;




	    PROCEDURE enterseg(t,a : integer) ;

		{***********************************************}
		{						}
		{ enterseg					}
		{						}
		{   This procedure enters a symbol action pair	}
		{ into the table.				}
		{						}
		{***********************************************}



		BEGIN
		{ enterseg }
		IF index >= maxtabsize THEN writeln(' Overflow of table')
		ELSE index := index+1 ;
		table [index].tran := t;
		table [index].action := a ;
		END { enterseg } ;



	    PROCEDURE makeseg (s :stateptrty) ;
		{***************************************************}
		{						    }
		{ makeseg					    }
		{						    }
		{   This procedure copies a segment, i.e. a state   }
		{ and formats into a symbol action pair.	    }
		{						    }
		{***************************************************}
	    LABEL
		1;
	    VAR
		act : integer ;
		id : identptrty ;
		first : boolean ;
		k,lastcount,currcount : integer ;
		p : integer ;
		last,t : tranptrty ;






		FUNCTION count (s :stateptrty) : integer ;

		    {*******************************************}
		    {						}
		    { count					}
		    {						}
		    {	This function returns the number of	}
		    { unique transitions of a given state.	}
		    {*******************************************}

		VAR
		    temp : integer ;
		    id : identptrty ;
		    q,t : tranptrty ;
		    BEGIN
		    { count }
		    temp := 0;
		    t := nexttran(s,NIL);
		    WHILE t <> NIL DO
			BEGIN
			id := token(t^.rule,t^.pos);
			q := t^.next ;
			WHILE q <> NIL DO
			    BEGIN
			    IF token(q^.rule,q^.pos)=id THEN q^.marke := true;
			    q := q^.next ;
			    END ;
			temp := temp +1 ;
			t := nexttran(s,t) ;
			END ;
		    count := temp ;
		    END { count } ;



		PROCEDURE add_segment_entry (t, a : integer);
		    {***************************************************}
		    {							}
		    { add_segment_entry					}
		    {							}
		    {	This procedure adds an entry to 		}
		    {	the segment vector.				}
		    {							}
		    {   in  :	t symbol code				}
		    {		a action code				}
		    {							}
		    {***************************************************}

		VAR
		    inserted : boolean;
		    i : integer;

		    BEGIN
		    { add_segment_entry }

		    IF segindex >= maxsegment THEN
			writeln(' Segment Overflow')
		    ELSE
			segindex := segindex + 1;

		    segment[segindex].tran := t;
		    segment[segindex].action := a;

{ Don't sort
		    i := segindex;
		    inserted := false;
		    WHILE not inserted DO
			BEGIN

			IF i <= 0 then
			    BEGIN
			    inserted := true;
			    segment[0].tran := t;
			    segment[0].action := a;
			    END
			ELSE
			    IF t > segment[i-1].tran THEN
				BEGIN
				inserted := true;
				segment[i].tran := t;
				segment[i].action := a;
				END
			    ELSE
				BEGIN
				segment[i].tran := segment[i-1].tran;
				segment[i].action := segment[i-1].action;
				i := i - 1;
				END;
			END;
}		
		    END { add_segment_entry } ;


		PROCEDURE expand ( p : tranptrty) ;
		    {***************************************************}
		    {							}
		    { expand						}
		    {							}
		    {	This procedure expands a lookahead string	}
		    { a symbol action list				}
		    {							}
		    {***************************************************}

		VAR
		    s : stateptrty ;
		    prule : integer ;
		    t,q : tranptrty ;
		    id : identptrty ;
		    BEGIN
		    s := p^.nextstate ;
		    t := nexttran (s,NIL) ;
		    prule := p^.rule^.production ;
		    WHILE t <> NIL DO
			BEGIN
			id := token (t^.rule ,t^.pos) ;
			q := t^.next ;
			WHILE q <> NIL DO
			    BEGIN
			    IF token(q^.rule,q^.pos)=id THEN q^.marke := true;
			    q := q^.next ;
			    END ;
			add_segment_entry(id^.code, -prule-noscancode);
			t := nexttran(s,t) ;
			END ;
		    END { expand }  ;



		BEGIN
		{ makeseg }
		first := true ;
		segment[0].tran := unknown ;
		segment[0].action := unknown ;
		p := unknown ;

		segindex := -1 ;
		t :=s^.firsttran ;
		WHILE t <> NIL DO
		    BEGIN
		    {WHILE}
		    IF t^.nextstate <> NIL THEN
			BEGIN
			{copy}
			id := token(t^.rule,t^.pos) ;
			IF id <> NIL THEN k := id^.code ELSE k:=unknown;
			IF t^.nextstate^.kind=lastring THEN
			    BEGIN
			    {lastring}
			    IF first THEN
				BEGIN
				{first}
				first := false;
				last := t ;
				lastcount := count(last^.nextstate) ;
				GOTO 1 ;
				END {first}
			    ELSE
				BEGIN
				{NOT first}
				currcount := count(t^.nextstate);
				IF currcount > lastcount+threshold THEN
				    BEGIN
				    {swap}
				    expand(last) ;
				    last := t ;
				    lastcount := currcount ;
				    END {swap}
				ELSE expand(t) ;
				GOTO 1 ;
				END {NOT first}
			    END { lastring}
			ELSE IF t^.nextstate^.kind=reduction THEN
			p := -t^.rule^.production
			ELSE
			p := t^.nextstate^.state ;
			add_segment_entry(k, p);
			END {copy } ;
		    1: t := t^.next ;
		    END {WHILE}	;
		IF s^.kind = lookahead THEN
		IF first THEN writeln(' System error in makeseg2')
		ELSE act := - last^.rule^.production-noscancode ;

		IF s^.kind= reads THEN act := errorcode;
		IF (s^.kind=reads)OR (s^.kind=lookahead)
		THEN
		    BEGIN
		    IF segindex>= maxsegment THEN writeln(' Segment overflow')
		    ELSE segindex := segindex+1 ;
		    segment[segindex].tran := elsecode;
		    segment[segindex].action := act ;
		    END ;
		END {makeseg} ;




	    FUNCTION segmatch(ind:integer) : integer ;
		{***************************************************}
		{						    }
		{   segmatch					    }
		{						    }
		{	This function returns true if the segment   }
		{ matches an entry in the table. If there is no	    }
		{ match it returns unknown.			    }
		{***************************************************}
	    LABEL
		1,2;
	    VAR
		i,k : integer ;
		mate : integer ;



		BEGIN
		{ segmatch}
		segmatch := unknown ;
		IF index > (segindex-ind) THEN
		FOR mate := 0 TO index-(segindex-ind) DO
		    BEGIN
		    k := mate ;
		    FOR i := ind TO segindex DO
			BEGIN
			IF table[k].tran=continue
			    THEN k := table[k].action;
			IF (segment[i].tran <> table[k].tran)
			OR (segment[i].action <> table[k].action) THEN GOTO 1;
			k := k+1;
			END ;
		    segmatch := mate ; GOTO 2;
		    1:
		    END { FOR } ;
		2:
		END { segmatch } ;




	    BEGIN
	    { gentab }
	    index := -1 ;
	    FOR i := 0 TO currentstate DO
		BEGIN
		s := nstate[i ];
		IF (s^.kind <> reduction) AND (s^.kind <> lastring) THEN
		    BEGIN
		    newstate[i] := index+1 ;
		    makeseg(s) ;
		    entry := segmatch(0);
		    IF entry <> unknown THEN newstate[i] := entry
		    ELSE
			BEGIN
			enterseg(segment[0].tran,segment[0].action) ;
			IF segindex > 0 THEN

			BEGIN
			FOR j := 1 TO segindex-1 DO
			    BEGIN
			    entry := segmatch(j);
			    IF entry <> unknown THEN
				BEGIN
				enterseg(continue,entry);
				GOTO 1 ;
				END
			    ELSE enterseg(segment[j].tran,segment[j].action);
			    END ;
			enterseg(segment[segindex].tran,segment[segindex].action);
			END ;
			1:
			END ;
		    END ;
		END { FOR } ;
	    { renumber new states }
	    FOR i := 0 TO currentstate DO
	    IF nstate[i]^.kind <> lastring THEN
	    nstate[i] ^.state := newstate[i] ;
	    FOR i := 0 TO index DO
	    IF (table[i].action >=0) AND (table[i].action <=currentstate)
	    AND (table[i].tran <> continue) THEN
	    table[i].action := newstate[table[i].action ];
	    END { gentab }  ;

	PROCEDURE writeintname(i:identptrty);
	{*******************************************************}
	{							}
	{ writeintname						}
	{							}
	{   This prints the identifier name of an object to the	}
	{   output, then tabs over two stops.			}
	{							}
	{*******************************************************}
	VAR x : integer;
	BEGIN WITH i^.intname DO
	BEGIN
	    FOR x:=1 TO len DO
		write(outpas, ch[x]);
	    IF len < 8 THEN write(outpas,'	');{tab}
	    IF len < 16 THEN write(outpas,'	');{another tab}
	END END;


	PROCEDURE outtab ;
	{***********************************************}
	{						}
	{ outtab					}
	{						}
	{   This procedure generates OUTPAS,		}
	{	    the tables.				}
	{						}
	{***********************************************}

 VAR
    num,i,k,t,a : integer ;
    x : identptrty;



 BEGIN	{ outtab }
 if not (language = oldbliss) then
    begin
{xxxxxx}	writeln(OUTPAS,continue,' ',elsecode,' ',errorcode,' ',noscancode);
	writeln(OUTPAS);
	writeln(OUTPAS,errorcode);
	writeln(OUTPAS,maxsymbol);
	writeln(OUTPAS,maxpop);
	writeln(OUTPAS,maxprodno);
	writeln(OUTPAS,index);
	writeln(OUTPAS,elsecode);
	writeln(OUTPAS,-1); {partition}
	writeln(OUTPAS);
	writeln(OUTPAS);

    end
 else
	begin
	writeln(OUTPAS,'Literal');
	writeln(OUTPAS,' 	continue	= ',continue:4,',');
	writeln(OUTPAS,' 	elsecode	= ',elsecode:4,',');
	writeln(OUTPAS,' 	errorcode	= ',errorcode:4,',');
	writeln(OUTPAS,' 	noscancode	= ',noscancode:4,',');
	writeln(OUTPAS,' 	maxcode		= ',maxsymbol:4,',');
	writeln(OUTPAS,' 	maxpop		= ',maxpop:4,',');
	writeln(OUTPAS,' 	maxprod		= ',maxprodno:4,',');
	writeln(OUTPAS,' 	index		= ',index:4,';');
	writeln(OUTPAS)
	end;
		{ generate labels}

	if language = oldbliss then
	    BEGIN
		writeln(OUTPAS,' Literal');
{		writeln(OUTPAS,' 		! Define terminals, non terminals');
		FOR num := 0 TO maxsymbol DO
		    IF symbolname[num] <> NIL then
			BEGIN
			write(outpas,' 	');
			writeintname(symbolname[num]);
			writeln(outpas,'= ',num,',');
			END;
		writeln(outpas,' 		! Define semantic actions');
}	    END;
	num := 0;
	FOR k := 0 TO nametabsize DO
	BEGIN
		x := nametab[k];
		WHILE x<> NIL DO
		BEGIN
		    IF x^.kind IN [semact]
			THEN
				if language = oldbliss then
					begin
					num := num + 1;
					write(OUTPAS,' 	');
					writeintname(x);
					writeln(outpas,'= ',num,',');
					end
				else writeln(OUTPAS,' ',x^.intname.ch);
		    x := x^.nic;
		END ;
	END ;
	if language = oldbliss then
		writeln(OUTPAS,' 	lastact		= ',num,';')
	    else writeln(OUTPAS,-1) ; { part}
	writeln(OUTPAS);writeln(OUTPAS);writeln(OUTPAS);


		{ generate table arrays}
	write(OUTPAS,' ');

	if language = oldbliss then
		begin
		writeln(OUTPAS,' global bind pat_state_table = uplit word( ');
		for i:=0 to index do
			begin
			t:= table[i].tran;
			write(OUTPAS,t:5,',');
			if (i mod 12) = 0 then
				begin
				writeln(OUTPAS);
				write(OUTPAS,' ');
				end;
			end;
		writeln(OUTPAS);
		writeln(OUTPAS,' 		0 ) :vector[,word];');
		writeln(OUTPAS);
		writeln(OUTPAS,' global bind pat_act_table = uplit word( ');
		for i := 0 to index do
			begin
			a:= table[i].action;
			write (OUTPAS,a:6,',');
			if (i mod 12) = 0 then
				begin
				writeln(OUTPAS);
				write(OUTPAS,' ');
				end;
			end;
		writeln(OUTPAS);
		writeln(OUTPAS,' 		0 ) :vector[,word,signed];');
		writeln(OUTPAS);

		writeln(OUTPAS,' global bind pat_pop_table = uplit byte( 0, ');
		for i:=1 to prodno do
			begin
			t:= poptab[i]-1;
			write(OUTPAS,t:5,',');
			if (i mod 12) = 0 then
				begin
				writeln(OUTPAS);
				write(OUTPAS,' ');
				end;
			end;
		writeln(OUTPAS);
		writeln(OUTPAS,' 		0 ) :vector[,byte,signed];');
		writeln(OUTPAS);
		writeln(OUTPAS,' global bind pat_lhs_table = uplit byte( 0, ');
		for i :=1 to prodno do
			begin
			a:= ruletab[i]^.def^.code;
			write (OUTPAS,a:5,',');
			if (i mod 12) = 0 then
				begin
				writeln(OUTPAS);
				write(OUTPAS,' ');
				end;
			end;
		writeln(OUTPAS);
		writeln(OUTPAS,' 		0 ) :vector[,byte];');
		writeln(OUTPAS);
		write(OUTPAS,' global bind pat_sem_table = uplit ');
		if prodno <= 255
		then
			write(OUTPAS,'byte')
		else
			write(OUTPAS,'word');
		writeln(OUTPAS,' ( 0 ');
		write(OUTPAS,' ');
		for i :=1 to prodno do
			begin
			x:= ruletab[i]^.action;
			if x = nil then id.ch :='0                              '
				else id.ch := x^.intname.ch;
			write (OUTPAS,',',id.ch);
			if (i mod 3) = 0 then
				begin
				writeln(OUTPAS);
				write(OUTPAS,' ');
				end;
			end;
		writeln(OUTPAS);
		write(OUTPAS,' 		,0 ) :vector[,');
		if prodno <= 255
		then
			write(OUTPAS,'byte')
		else
			write(OUTPAS,'word');
		writeln(OUTPAS,'];');
		writeln(OUTPAS);

		end
	else begin

	FOR i := 0 TO index DO
	BEGIN
	    t := table[i].tran ;
	    a := table[i].action;
	    writeln(OUTPAS,i,' ',t,' ',a);
	END ;
	writeln(OUTPAS,-1);
	writeln(OUTPAS);

		{ generate pop AND semantics}

	FOR i := 1 TO prodno DO
	BEGIN
	    t := poptab[i] -1;
	    a := ruletab[i]^.def^.code;
	    x := ruletab[i]^.action;
	    IF x = NIL THEN id.ch :='null                           '
		ELSE id :=x^.intname;
	    writeln(OUTPAS,i,' ',t,' ',a,' ',id.ch);
	END ;

	writeln(OUTPAS,-1) ; { part}

	writeln(OUTPAS);
	end;
    END {outtab} ;




PROCEDURE OUTBLISS;
	(****************************************************************)
	(*								*)
	(* OUTBLISS							*)
	(*								*)
	(*  This procedure generates OUTPAS, the BLISS require file.	*)
	(*								*)
	(****************************************************************)

	VAR
	    QUOTED_STR: INTEGER;
	    I:		INTEGER;
	    TAB:	CHAR;
	    K:		INTEGER;
	    first_term,last_term: integer;
	    NAMEENTRY:	IDENTPTRTY;
	    TMP:	IDENTPTRTY;
	    SEMACTNUM:	INTEGER;
	    WORDTMP:	WORD;
	    INTTMP:	WORD;
	    NAMEKIND:	NAMETY;

	    PROCEDURE WRITENAME(NAM: WORD);
	    (************************************************************)
	    (*								*)
	    (* WRITENAME						*)
	    (*								*)
	    (*	This procedure outputs a name without leading and	*)
	    (*	trailing apostrophes.					*)
	    (*								*)
	    (************************************************************)
	    VAR
		I: INTEGER;
		C: CHAR;
	    BEGIN (* WRITENAME *)
		IF NAM.CH[1] =''''
		THEN
		    QUOTED_STR := 1
		ELSE
		    QUOTED_STR := 0;
		I := QUOTED_STR + 1;
		WHILE (I <= NAM.LEN - QUOTED_STR) DO
		    BEGIN
		    WRITE(OUTPAS, NAM.ch[I]);
		    I := I + 1;
		    END;
	    END (* WRITENAME *);

	    FUNCTION FINDNAME(VAL: INTEGER): BOOLEAN;
	    (************************************************************)
	    (*								*)
	    (* FINDNAME							*)
	    (*								*)
	    (*	    This function finds a terminal or non-terminal	*)
	    (*	    which has a value of VAL, including ELSE_CODE and	*)
	    (*	    CONT_CODE.	Return TRUE if match exists, FALSE	*)
	    (*	    otherwise.						*)
	    (************************************************************)

	    LABEL	1;

	    VAR
		K:		INTEGER;
		NAMEENTRY:	IDENTPTRTY;
	    BEGIN (* FINDNAME *)
		FINDNAME := TRUE;
		IF VAL = CONTINUE
		THEN
		    BEGIN
		    NAMEKIND := INTERNALNAME;
		    WORDTMP.LEN := 13;
		    WORDTMP.CH :='PAT_CONT_CODE                  ';
		    INTTMP := WORDTMP;
		    GOTO 1;
		    END;
		IF VAL = ELSECODE
		THEN
		    BEGIN
		    NAMEKIND := INTERNALNAME;
		    WORDTMP.LEN := 13;
		    WORDTMP.CH :='PAT_ELSE_CODE                  ';
		    INTTMP := WORDTMP;
		    GOTO 1;
		    END;
		IF symbolname[val] <> NIL THEN
		    WITH symbolname[val]^ DO
			BEGIN
			namekind := kind;
			wordtmp := name;
			inttmp := intname;
			END
		ELSE FINDNAME := FALSE;
	    1: END (* FINDNAME *);

	BEGIN	(* OUTBLISS *)
	    I := 9;
	    TAB := CHR(I);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' %IF %DECLARED(%QUOTE PAT_DFS) AND NOT %DECLARED(%QUOTE PAT_NO_DFS)');
	    WRITELN(OUTPAS,' %THEN');
	    WRITELN(OUTPAS);

	    SEMACTNUM := maxcode - maxsymbol;
	    first_term := -1; last_term := -1;
	    FOR k := 0 TO maxsymbol DO
		IF symbolname[k] <> NIL THEN WITH symbolname[k]^ DO
		    IF kind IN [decterm,undterm] THEN
			BEGIN
			IF first_term = -1 THEN first_term := k;
			last_term := k;
			END;
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' ! Define terminals, non-terminals, and semantics actions');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' PAT_SYMBOL_DEFINITIONS (	! Terminal and non-terminal definitions');
	    FOR K := 0 TO MAXSYMBOL DO
		BEGIN
		IF FINDNAME(K)
		THEN
		    BEGIN
		    CASE namekind OF
			decterm : WRITE(OUTPAS, TAB,'PAT_DECTRMDEF(''');
			undterm : WRITE(OUTPAS, TAB,'PAT_UNDTRMDEF(''');
			nonterm : WRITE(OUTPAS, TAB,'PAT_NONTRMDEF(''');
			END;
		    WRITENAME(WORDTMP);
		    IF namekind = decterm
		    THEN
			BEGIN
			write(outpas,''',''');
			writename(inttmp);
			END;
		    WRITE(OUTPAS,''', ',K:0);
		    END
		ELSE
		    WRITE(OUTPAS, TAB,'PAT_UNUSEDNUM(', K:0);
		IF K = MAXSYMBOL THEN WRITELN (OUTPAS, ')') ELSE WRITELN (OUTPAS, '),')
		END;
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' )				! End of terminal and non-terminal defintions');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' PAT_SEMACT_DEFINITIONS (	! Semantics action defintions');
	    WRITE(OUTPAS, TAB, 'PAT_SEMACTDEF(''PAT_NULL'',0)' );
	    IF maxsymbol < maxcode then
		WRITELN(OUTPAS, ',')
	    else
		WRITELN(OUTPAS);
	    FOR k := maxsymbol+1 TO maxcode DO
		IF symbolname[k]<>NIL THEN WITH symbolname[k]^ DO
		    BEGIN
			WRITE(OUTPAS, TAB, 'PAT_SEMACTDEF(''');
			WRITENAME(INTNAME);
			WRITE(OUTPAS,''', ', K-maxsymbol:0);
			IF K = maxcode THEN WRITELN (OUTPAS, ')') ELSE WRITELN (OUTPAS, '),');
		    END;
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' )				! End of semantics action defintions');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' PAT_INTERNAL_DEFINITIONS (	! Define internal symbols');
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_CONT_CODE'', ', CONTINUE:6,'),');
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_ELSE_CODE'', ', ELSECODE:6,'),');
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_SCAN_CODE'', ', NOSCANCODE:6,'),');
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_ERROR_CODE'',', ERRORCODE:6,'),');
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_MAX_TOKEN'', ', MAXSYMBOL:6,'),' );
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_MAX_POP'',   ', MAXPOP:6,'),' );
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_MAX_REDUCT'',', MAXPRODNO:6,'),');
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_MAX_TRANS'', ', INDEX:6,'),' );
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_MAX_SEMACT'',', SEMACTNUM:6,'),' );
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_FIRST_TERM'',', FIRST_TERM:6,'),' );
	    WRITELN(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_LAST_TERM'', ', LAST_TERM:6,'),' );
	    WRITE(OUTPAS, TAB, 'PAT_INTRNLDEF(''PAT_SENTENCE_CD'',	PAT_NONTRMREF(''' );
	    WRITENAME(RULETAB[1]^.DEF^.NAME);
	    WRITELN(OUTPAS,'''))');
	    WRITELN(OUTPAS, ' )				! End of internal definitions');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' %FI	! End of %IF %DECLARED(%QUOTE PAT_DFS) ...');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' %IF %DECLARED(%QUOTE PAT_TABLES) AND NOT %DECLARED(%QUOTE PAT_NO_TABLES)');
	    WRITELN(OUTPAS,' %THEN');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' ! Define parsing tables');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' PAT_STATE_SYMBOL_TABLE (');
	    FOR I := 0 TO INDEX DO
		BEGIN
		IF I > 0 THEN
		    WRITELN(OUTPAS,',');
		WRITE(OUTPAS,' ', TAB);
		IF FINDNAME(TABLE[I].TRAN)
		THEN
		    BEGIN
		    CASE namekind OF
			decterm : WRITE(OUTPAS,'PAT_DECTRMREF(''');
			undterm : WRITE(OUTPAS,'PAT_UNDTRMREF(''');
			internalname : 	WRITE(OUTPAS,'PAT_INTRNLREF(''');
			nonterm : WRITE(OUTPAS,'PAT_NONTRMREF(''');
			END;
		    WRITENAME(INTTMP);
		    WRITE(OUTPAS,''')' );
		    END
		ELSE
		    WRITE(OUTPAS, TABLE[I].TRAN:0);
		END;
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' )				! End of PAT_END_STATE_SYMBOL_TABLE');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' PAT_STATE_ACTION_TABLE (');
	    FOR I := 0 TO INDEX DO
		BEGIN
		IF I > 0 THEN
		    WRITELN(OUTPAS,',');
		WRITE(OUTPAS,' ', TAB, TABLE[I].ACTION:6);
		END;
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' )				! End of PAT_STATE_ACTION_TABLE');
	    WRITELN(OUTPAS);
	    WRITE(OUTPAS,' PAT_RHS_COUNT_TABLE (-1');
	    FOR I := 1 TO PRODNO DO
		BEGIN
		WRITELN(OUTPAS,',');
		WRITE(OUTPAS,' ', TAB, (POPTAB[I]):6);
		END;
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' )				! End of PAT_POP_TABLE');
	    WRITELN(OUTPAS);
	    WRITE(OUTPAS,' PAT_LHS_TABLE (-1');
	    FOR I := 1 TO PRODNO DO
		BEGIN
		WRITELN(OUTPAS,',');
		WRITE(OUTPAS,' ', TAB,'PAT_NONTRMREF(''');
		WRITENAME(RULETAB[I]^.DEF^.INTNAME);
		WRITE(OUTPAS,''')' );
		END;
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' )				! End of PAT_LHS_TABLE');
	    WRITELN(OUTPAS);
	    WRITE(OUTPAS,' PAT_SEMACT_TABLE (-1');
	    FOR I := 1 TO PRODNO DO
		BEGIN
		WRITELN(OUTPAS,',');
		TMP := RULETAB[I]^.ACTION;
		IF TMP = NIL
		THEN
		    BEGIN
		    INTTMP.LEN := 8;
		    INTTMP.CH :='PAT_NULL                       ';
		    END
		ELSE
		    BEGIN
		    INTTMP.LEN := TMP^.INTNAME.LEN;
		    INTTMP.CH := TMP^.INTNAME.CH;
		    END;
		WRITE(OUTPAS,' ', TAB,'PAT_SEMACTREF(''');
		WRITENAME(INTTMP);
		WRITE(OUTPAS,''')' );
		END;
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' )				! End of PAT_SEMACT_TABLE');
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS);
	    WRITELN(OUTPAS,' %FI	! End of %IF %DECLARED(%QUOTE PAT_TABLES) ...');
	END	(* OUTBLISS *);

PROCEDURE outada;
	{***********************************************}
	{						}
	{    outada					}
	{						}
	{    Prints out OUTPAS, the Ada package.	}
	{						}
	{***********************************************}
VAR i,j,count : integer;
    first   : boolean;
    first_term, last_term, firstobj, lastobj : identptrty;

PROCEDURE outname(i : identptrty);
VAR j : integer;
BEGIN WITH i^.intname DO
	FOR j := 1 to len DO write(outpas,ch[j]);
END;

PROCEDURE writelist(kinds:setofnamety);
{ print out internal names in neat rows }
VAR i,j : integer;
BEGIN
firstobj := NIL;
FOR i := 0 TO maxcode DO
    IF symbolname[i] <> NIL THEN WITH symbolname[i]^ DO
	IF kind IN kinds THEN
	    BEGIN
		IF NOT first THEN
		    BEGIN
		    write(outpas,',');
		    IF count > 4 THEN
			BEGIN
			writeln(outpas);
			write(outpas,'	');
			count := 0;
			END
		    ELSE
			BEGIN
			IF lastobj^.intname.len < 7 THEN write(outpas,'	');
			IF lastobj^.intname.len <15 THEN write(outpas,'	');
			END;
		    END;
		FOR j := 1 TO intname.len DO
		    write(outpas,intname.ch[j]);
		lastobj := symbolname[i];
		if firstobj = NIL then firstobj := symbolname[i];
		first := FALSE;
		count := count + 1;
	    END;
	END;
BEGIN
writeln(outpas,'package PAT_DATA is');
writeln(outpas);
writeln(outpas,'    MAX_RHS_SIZE : constant := ',maxpop:0,';');
writeln(outpas,'    NUM_PRODUCTION : constant := ',maxprodno:0,';');
writeln(outpas,'    MAX_STATE_INDEX : constant := ',index:0,';');
writeln(outpas);
writeln(outpas,'    type STATE_TABLE_SYMBOL is (');
write(outpas,'	ELSE_CODE,	CONTINUE_CODE,	'); first := TRUE; count := 2;
writelist([decterm,undterm]);
first_term := firstobj;
last_term := lastobj;
writelist([nonterm]);
writeln(outpas,');');
write(outpas,'    subtype TOKEN is STATE_TABLE_SYMBOL range ');
outname(first_term);write(outpas,' .. ');outname(lastobj);writeln(outpas,';');
write(outpas,'    subtype TERMINAL is TOKEN range ');
outname(first_term);write(outpas,' .. ');outname(last_term);writeln(outpas,';');
write(outpas,'    subtype NON_TERMINAL is TOKEN range ');
outname(firstobj);write(outpas,' .. ');outname(lastobj);writeln(outpas,';');
writeln(outpas);
writeln(outpas,'    type STRING_PTR is access STRING;');
writeln(outpas,'    NAME : constant array (TERMINAL) of STRING_PTR := (');
first := TRUE;
FOR i := 0 TO maxsymbol DO
    IF symbolname[i] <> NIL THEN WITH symbolname[i]^ DO
	IF kind IN [decterm,undterm] THEN
	    BEGIN
		IF NOT first THEN writeln(outpas,',');
		write(outpas,'	');
		writeintname(symbolname[i]);
		write(outpas,'=> new STRING ("');
		WITH name DO
		    BEGIN
			IF ch[1] = '''' THEN j := 2 ELSE j := 1;
			REPEAT
			    IF ch[j] = '''' THEN j:=j+1;
			    IF ch[j] = '"' THEN write(outpas,'"');
			    IF j<=len THEN
				IF (ch[j]>='a') AND (ch[j]<='z') THEN
				    write(outpas,chr(ord(ch[j]) - 32))
				ELSE write(outpas,ch[j]);
			    j := j+1;
			UNTIL j>len;
		    END;
		write(outpas,'")');
		first := FALSE;
	    END;
writeln(outpas,');');
writeln(outpas);
writeln(outpas,'    type SEMACTION is (');
write(outpas,'	NUL,		');first := TRUE;count := 1;
writelist([semact]);
writeln(outpas,');');
writeln(outpas);
writeln(outpas,' -- Define parsing tables');
writeln(outpas);
writeln(outpas,'    subtype STATE_INDEX is INTEGER range 0 .. MAX_STATE_INDEX;');
writeln(outpas,'    subtype PRODUCTION_INDEX is INTEGER range 1 .. NUM_PRODUCTION;');
writeln(outpas);
writeln(outpas,'    ERROR : constant := -3000;');
writeln(outpas,'    SHIFT : constant := 0;');
writeln(outpas,'    REDUCE : constant := -2000;');
writeln(outpas,'    LOOK_AHEAD_REDUCE : constant := -1000;');
writeln(outpas);
writeln(outpas,'    type STATE_TABLE_ENTRY is');
writeln(outpas,'	record');
writeln(outpas,'	    MATCH : STATE_TABLE_SYMBOL := ELSE_CODE;');
writeln(outpas,'	    ACTION : INTEGER;');
writeln(outpas,'	end record;');
writeln(outpas);
writeln(outpas,'    STATE_TABLE : constant array (STATE_INDEX) of STATE_TABLE_ENTRY := (');
FOR i := 0 TO index DO WITH table[i] DO
    BEGIN
	write(outpas,'	',i:0,' =>	(');
	IF tran = continue THEN
	    write(outpas,'CONTINUE_CODE,	',action:0)
	ELSE
	    BEGIN
		IF tran = elsecode THEN write(outpas,'ELSE_CODE,	')
		ELSE BEGIN
			outname(symbolname[tran]);
			write(outpas,',');
			IF symbolname[tran]^.intname.len<6 THEN write(outpas,'	');
			IF symbolname[tran]^.intname.len<14 THEN write(outpas,'	');
		    END;
		IF action = errorcode THEN write(outpas,'ERROR')
		ELSE IF action > 0 THEN write(outpas,'SHIFT + ',action:0)
		ELSE IF action < -noscancode THEN
			write(outpas,'LOOK_AHEAD_REDUCE + ',-(action+noscancode):0)
		ELSE write(outpas,'REDUCE + ',-action:0);
	    END;
	IF i < index THEN write(outpas,'),') ELSE write(outpas,'));');
	IF action < 0 THEN
	    BEGIN
		IF action < -noscancode THEN
		    j := -(action+noscancode)
		ELSE j := -action;
		write(outpas,'	-- to ');
		outname(ruletab[j]^.def);
	    END;
	writeln(outpas);
    END;
writeln(outpas);
writeln(outpas,'    type PRODUCTION_TABLE_ENTRY is');
writeln(outpas,'	record');
writeln(outpas,'	    RHS_SIZE : INTEGER;');
writeln(outpas,'	    LHS : NON_TERMINAL;');
writeln(outpas,'	    ACTION : SEMACTION;');
writeln(outpas,'	end record;');
writeln(outpas);
writeln(outpas,'PRODUCTION_TABLE : constant array (PRODUCTION_INDEX) of PRODUCTION_TABLE_ENTRY := (');
FOR i := 1 TO maxprodno DO
    BEGIN
	write(outpas,'	',i:0,' =>	(',poptab[i]:0,', ');
	outname(ruletab[i]^.def);
	write(outpas,', ');
	IF ruletab[i]^.action = NIL THEN write(outpas,'NUL')
	ELSE outname(ruletab[i]^.action);
	IF i < maxprodno THEN writeln(outpas,'),') ELSE writeln(outpas,'));');
    END;
writeln(outpas);
writeln(outpas,'end PAT_DATA;');
END;

	BEGIN
	{   outt }
	writeln(' Generating tables') ;
	gentab ;
	IF parsetables THEN
	    printtab ;
	CASE language OF
	    ada : outada;
	    bliss : outbliss;
	    ltext, oldbliss : outtab;
	    OTHERWISE ;
	    END;
	END ;



    PROCEDURE	outfreq		;
	{***********************************************}
	{						}
	{   outfreq					}
	{						}
	{	This procedure outputs the statistics	}
	{   concerning the program, to the list file.	}
	{						}
	{***********************************************}
	VAR i:integer;
	BEGIN
	{   outfreq }
	IF trace THEN BEGIN
	writeln(' Outfreq')
	END;
	writeln(' Number of productions	= ',prodno);
	writeln(' Maximum symbol code	= ',maxcode);
	writeln(' Number of states	= ',currentstate);
	writeln(' Size of tables is	= ',index);
	i := 12;ch := chr(i);writeln(outfile,ch);
	writeln(outfile,'PAT statistics:');
	writeln(outfile,'	Number of productions	= ',prodno);
	writeln(outfile,'	Maximum symbol code	= ',maxcode);
	writeln(outfile,'	Number of states	= ',currentstate);
	writeln(outfile,'	Size of tables is	= ',index);
	END ;



    BEGIN
    {	main }
{   mark(heapbot) ;	***** seems to cause storage allocation problem }
    reset(input);
    rewrite(output);
    initp	    ;	{ initialize tables as read }


    { process input }
    parseinit;		{ initialize the parsing routines }

    { If process terminals, then process the grammar productions }
    IF terminal THEN readg;
    close(infile);
    numbersemacts;
    undeclcheck(declaredterms);
    IF maxcode > maxvocsize THEN
	BEGIN
	errorflag := TRUE;
	writeln(' Symbol table overflow by ',maxcode-maxvocsize);
	END;
    parseend;		{ done parsing input }

    IF grammar THEN printg;
    IF cref OR terminals THEN printcref;
    IF errorflag THEN writeln(' Error no tables generated')
    ELSE
    IF lr0 THEN
	BEGIN
	cfsm(lr0) ;
	IF (NOT lr0)
	THEN
	    BEGIN
	    lrk := true ;
	    lalr(maxk,lrk) ;
	    END ;
	IF fsm THEN outfsm ;
	IF lrk OR lr0 THEN outt ;
	END;
    outfreq ;
{***************************************************************}
{   mark(heaptop) ;						}
{   writeln(' Storage used ',heapbot-heaptop) ;			}
{***************************************************************}
END.
