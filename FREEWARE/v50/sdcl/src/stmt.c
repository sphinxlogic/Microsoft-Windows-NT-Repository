/*
**  Copyright Dick Munroe, 1985-2000.
**  Rights to distribution and modification are granted as specified in the GNU
**  Public License.
**
**	this file contains all the semantic analysis functions to be
**	used in generating sdcl output.  All of these functions are
**	ultimately called by statement(), depending on what the 
**	current input token is.  All of these functions then process
**	a line of input by calling the lexical analysis function and,
**	depending on what token is returned, generating correct DCL
**	output lines.
**
**  0.001 Dick Munroe 07-Aug-90
**	There is a bug in condition that winds up incorrectly parsing
**	conditional statements of the form:
**
**	if ( (cond) .op. (cond) ... ) ...
**
**  0.002 Dick Munroe 08-Aug-90
**	There is a bug in for statement processing that, incorrectly,
**	stops when the first closing parenthesis in the input stream
**	occurs rather than the first UNBALANCED closing parenthesis.
**
**  0.003 Dick Munroe 03-Sep-90
**	Make $ equivalent to #.
**
**  0.004 Dick Munroe 15-Sep-90
**	Get quoted string as first token on line output correctly.
**
**  0.005 Dick Munroe 30-May-95
**	Preserve quoted strings in conditionals.
**
**  0.006 Dick Munroe 30-May-95
**	Preserve variable substitution
**
**  0.007 Dick Munroe 01-Jun-95
**	Add on blocks.
**
**  0.008 Dick Munroe 01-Jun-95
**	Add definition of BREAK and NEXT symbols to allow integration of
**	SDCL end of loop statements with things like read.
*/

#include <stdio.h>
#include "tcodes.h"
#include "defs.h"

int  tokencode;
char token[MAXTOKENLEN];

/* 
 *	The following serve as forward declarations of internal and 
 *	external to this file.
 *
 */

extern void condition(), statement(int);
extern int lex();
extern void push() ;
extern int pop(), genlab(), errmsg();
extern void emitlabel(), emitqstring(), emittarget();

void breakstmt(int);
void compstmt(int);
void condition();
void dowhilestmt(int);
void forstmt(int);
void ifstatement(int) ;
void initialize(int);
void nextstmt(int);
void onstmt(int) ;
void other(int);
void reinitialize();
void statement(int);
void whilestmt(int);
void popBreakNext() ;

scan()
/*
 *	Repeatedly call lex until a token that is not a COMMENT,
 *	WSPACE, NEWLINE can be returned. 
 */
    {
	do 
	{
		tokencode = lex(token);
	} while	 (tokencode == COMMENT || tokencode == WSPACE ||
		  tokencode == NEWLINE );
	return(tokencode);
    }

void statement(int beginning)
/*
 *	Based on current token, invoke the appropriate
 *	routine to process a statement beginning with that token.
 */
    {
	switch (tokencode) {
		case IF: ifstatement(beginning);  
                         break;

		case WHILE: whilestmt(beginning);
                            break;

		case FOR: forstmt(beginning);
                          break;

		case DO: dowhilestmt(beginning);
                          break;

		case BREAK: breakstmt(beginning);  
                            break;

		case NEXT: nextstmt(beginning);
			   break;

		case ON: onstmt(beginning) ;
			 break;
			 
		case OBRACE: compstmt(beginning);  
                             break;

		default: other(beginning);
                         break;
	}
}

void ifstatement(beginning)
/*
 *	Process an if stmt. In the process generate any code associated
 *	with the if/else control structure.  When this function is called
 *	the current value of tokencode should be IF.
 *
 */
    {
	int looptype, lab1, lab2;

	scan();   
	if( tokencode == OPAREN )
		/* skip over OPAREN so conditon() gets the next token. */
		scan();
	else
		errmsg("Error--missing a '(' in if condition\n");

	lab1 = genlab();
	lab2 = genlab();  /* reserve this in case there is an else */
	if (beginning) emitstring("$ ");
	emitstring("if (.not.(");

	/* Parse the condition from the input stream. */
	condition();
	
	if( tokencode == CPAREN)
		scan();
	else
		errmsg("Error--missing a ')' in if condition\n");
	/* 
	 *	Build the remainder of the if-()-then-goto line and 
	 *	output it.  Function emittarget() is almost exactly like
	 *	emitlabel() except that no colon is appended to the
	 *	label.
	 */
	emitstring(")) then goto ");
	emittarget(lab1);
	emitstring("\n");
	/* 
	 *	Process the action part if the if-statement with a
	 *	recursive call to statement().
	 */
	statement(1);

	/* Check for an else part. */
	if (tokencode == ELSE ){
		scan();       
		emitstring("$ goto ");
		emittarget(lab2);
		emitstring("\n");
		emitstring("$ ");
		emitlabel(lab1);
		emitstring("\n");

		statement(1);
		/* 
		 *	Finally, build the target for the transfer from
		 *	the if portion of the statement if the statement
		 *	is an if-then-else.
		 */
		emitstring("$ ");
		emitlabel(lab2);
		emitstring("\n");
	}
	else{
		/*
		 *	Build the target for a transfer from the
		 *	if condition when there is no else clause.
		 */
		emitstring("$ ");
		emitlabel(lab1);
		emitstring("\n");
	}
    }

void compstmt(int beginning)
/*
 *	Called in response to encountering a OBRACE token, 
 *	signalling a compound statement.  What can
 *	legally follow is one or several statements.  These 
 *	statements can in turn be compound statements as well as any
 *	other statement type.  All this function need do is call 
 *	function statement recursively until a CBRACE is encountered
 *	(or the end of file).
 */
    {
	int flag = 1 ;

	scan();
	while (tokencode != CBRACE && tokencode != FILEEND)
		if (flag)
		{
		    flag = 0 ;
		    statement(beginning) ;
		}
		else
		    statement(1);

	/* Get the next token following the CBRACE for statement() to use. */
	scan();
    }

void onstmt(int beginning)
/*
 *	Process an on stmt. In the process generate any code associated
 *	with the on control structure.  When this function is called
 *	the current value of tokencode should be ON.
 *
 */
    {
	char onCondition[MAXTOKENLEN] ;
	int looptype,
	    lab1,   /* Name of the on block. */
	    lab2,   /* Exit label of the on block. */
	    lab3;   /* Branch around the on block. */

	lab1 = genlab();
	lab2 = genlab();
	lab3 = genlab();
	push(ON, lab1);

	scan();   
	switch (tokencode)
	{
	    case ON_WARNING:
	    case ON_ERROR:
	    case ON_SEVERE_ERROR:
	    case ON_CONTROL_Y:
		strcpy (onCondition, token) ;
		break ;

	    default:
		errmsg("Error--Invalid on condition\n") ;
		break ;
	}

	scan() ;
	switch (tokencode)
	{
	    case THEN:
		if (beginning) emitstring("$ ") ;
		emitstring("on ") ;
		emitstring(onCondition) ;
		emitstring(" then ") ;
		scan() ;
		pop(&looptype, &lab1);
		statement(0) ;
		return ;
		
	    case OBRACE:
		if (beginning) emitstring("$ ") ;
		emitstring("on ") ;
		emitstring(onCondition) ;
		emitstring(" then gosub ") ;
		emittarget(lab1) ;
		emitstring("\n") ;
		emitstring("$ goto ") ;
		emittarget(lab3) ;
		emitstring("\n") ;
		emitstring("$") ;
		emitlabel(lab1) ;
		emitstring("\n") ;
		compstmt(1) ;
		break ;

	    default:
		errmsg("Error--Invalid argument for ON condition\n") ;
		break ;
	}

	emitstring("$") ;
	emitlabel(lab2) ;
	emitstring("\n") ;

	emitstring("$ on ") ;
	emitstring(onCondition) ;
	emitstring(" then gosub ") ;
	emittarget(lab1) ;
	emitstring("\n") ;
	
	emitstring("$ return\n") ;
	
	emitstring("$") ;
	emitlabel(lab3) ;
	emitstring("\n") ;

	/*
	 *	It is necessary to pop the stack although the values 
	 *	returned by the operation will not be used.
	 */
	pop(&looptype, &lab1);
	popBreakNext() ;
    }

void other(int beginning)

/*	This function is called when an input line doesn't begin with
**	a keyword token or an OBRACE.  Assume the line ends when a 
**	newline or EOF is encountered. 
**
**	If the current token begins with a POUND it is a special
**	sdcl preprocessor directive to strip off the POUND and
**	pass whatever follows it to the output.  
**
**	If the current token begins with a DOLLAR it is a special
**	sdcl preprocessor directive to pass the whatever follows
**	it to the output.
**
**	If there is no beginning POUND the output line has a 
**	DOLLAR prepended to it then the first, and all subsequent tokens are
**	passed to the output stream.
**
**	Certain tokens cause special actions to be taken.  A BACKSLASH
**	tokencode is an sdcl line-continuation signal.  What it means is
**	that the next input line is to be considered to be a continuation
**	of the current line.  When a backslash is encountered parse
**	until the following newline character is encountered and
**	discard the newline.  Continue the parse appending the
**	tokens from the next line until another newline, semicolon,
**	of EOF token is found.
**
**	A STRING token causes a call to emitqstring() instead of
**	emitstring().  Emitqstring() merely checks to see if the
**	STRING token will fit on the current output line.  If it
**	won't a dash (DCL continuation character) and a newline are
**	appended to the current output line.  This will then flush
**	the current output line.  Then the STRING token is output
**	without a prepended dollar sign.
*/
    {
	/* If no directive to pass the input line directly... */

	switch (tokencode)
	{
		default	:	if (beginning) emitstring("$ ") ;
				emitstring(token) ;
				break ;

		case GRAVE :
		case STRING :	if (beginning) emitstring("$ ") ;
				emitqstring(token) ;
				break ;

		case POUND : 	tokencode = lex(token) ;
		case DOLLAR :	while ((tokencode != NEWLINE) &&
				       (tokencode != FILEEND))
				{
					emitlstring(token) ;
					tokencode = lex(token) ;
				} ;
				emitstring("\n") ;
				scan() ;
				return ;
	} ;

	tokencode = lex(token);

	while (tokencode != NEWLINE && tokencode != FILEEND){
		/* 
		 *	If a backslash include the next line of input 
		 *	with the current one.
		 */
		if (tokencode == BACKSLASH){
			while (tokencode != NEWLINE)
				tokencode = lex(token);
			scan();
			/* Add a space for readability. */
			emitstring(" ");
		}
		/*
		 *	Insure that the current STRING token will not
		 *	be broken by a newline by outputting the 
		 *	string via calling emitqstring() instead of
		 *	emitstring().
		 */
		else if ((tokencode == STRING) || (tokencode == GRAVE)){
			emitqstring(token);
			tokencode = lex(token);
		}
		else if ( tokencode != COMMENT ) {
			emitstring(token);
			tokencode = lex(token);
		}
		else  /* don't emit sdcl comment, just get next token */
			tokencode = lex(token);
	}
	emitstring("\n");      /* cause a output line flush */
	scan();
    }

void whilestmt(int beginning)
/*
 * 	This function processes a while statement in a manner 
 * 	similar to how if statements are handled.  This function 
 *	will be invoked in response to finding the keyword "while"
 *	in the input stream.
 *
 *	While the semantic parse is being performed the intermediate
 *	code for this statement is also being generated.
 */	
    {
	int looptype, lab1, lab2;

	/* Skip the keyword and get two labels. */
	scan();
	lab1 = genlab();
	lab2 = genlab();
	/* 
	 *	Begin building the output string.  This will be
	 *	the destination label for any next statements.
	 */
	if (beginning) emitstring ("$ ");
	emitlabel(lab1);
	emitstring("if (.not.(");

	if (tokencode == OPAREN)
		scan();
	else
		errmsg("Error--missing a '(' in while condition\n");
	/*
	 *	Call condition.  When it returns the current token
	 *	should be a CPAREN.
	 */
	condition();
	if (tokencode == CPAREN)
		scan();
	else
		errmsg("Error--missing a ')' in while condition\n");

	 emitstring(")) then goto ");
	 emittarget(lab2);
	 emitstring("\n");

	emitstring("$ BREAK = \"") ;
	emittarget(lab2) ;
	emitstring("\"\n") ;

	emitstring("$ NEXT = \"") ;
	emittarget(lab1) ;
	emitstring("\"\n") ;

	/*
	 *	Do the necessary stack operations to preserve the
	 *	labels used with this while stmt. These will be used
	 *	to target any next or break stmts.
	 */
	push(WHILE, lab1);
	statement(1);

	/*
	 *	After the statement code has been generated the transfer
	 *	statement to the beginning of the condition must be 
	 *	generated($ goto lab1).  Then the destination for 
	 *	condition failure must be inserted.  This value will 
	 *	be lab + 1 (lab2).  Lab2 will *	be the destination 
	 *	address for any break statements.
	 */
	emitstring("$ goto ");
	emittarget(lab1);
	emitstring("\n");
	emitstring("$ ");
	emitlabel(lab2);
	emitstring("\n");
	/*
	 *	It is necessary to pop the stack although the values 
	 *	returned by the operation will not be used.
	 */
	pop(&looptype, &lab1);
	popBreakNext() ;
    }

void dowhilestmt(int beginning)
/*
 *	Parse and generate the contruct
 *	  do
 *	      statement
 *	  while ( condition )
 */
    {
        int lab1, lab2, lab3, looptype;

	scan();
	/*
	 * reserve all the labels needed for do-while and record loop
	 * on stack.
	 */
	lab1 = genlab();
	lab2 = genlab();
	lab3 = genlab();
	push( DO, lab1 );

	/*
	 * emit the label where the true exit from the condition
	 * will come to. 
	*/
	if (beginning) emitstring( "$ " );
	emitlabel( lab1 );
	emitstring( "\n" );
	
	emitstring("$ BREAK = \"") ;
	emittarget(lab3) ;
	emitstring("\"\n") ;

	emitstring("$ NEXT = \"") ;
	emittarget(lab2) ;
	emitstring("\"\n") ;

	statement(1);
	if ( tokencode == WHILE )
	    scan();
	else
	    errmsg("Error -- missing keyword 'while' in do-while\n" );
	if ( tokencode == OPAREN )
	    scan();
	else
	    errmsg("Error -- missing ( in do-while condition\n" );

	/*
	 * emit code for condition test and branch 
	 */
	emitstring( "$ " );
	emitlabel( lab2 );
	emitstring("if( " );

	condition();
	if ( tokencode == CPAREN )
	    scan();
	else
	    errmsg("Error -- missing ) in do-while condition\n" );

	/*
	 * branch to start of loop, and label for any break statements.
	 */
	emitstring(" ) then goto ");
	emittarget( lab1 );
	emitstring( "\n" );
	emitstring( "$ ");
	emitlabel( lab3 );
	emitstring( "\n" );
	/*
	 * pop stack now that we are done.
	 */
	pop( &looptype, &lab1 );
	popBreakNext() ;
    }

void breakstmt(int beginning)
/*
 *	As its name implies this small function handles a break
 *	statement.  It is called by function statement() whenever
 *	the token BREAK is encountered.  All this function need
 *	do is call scan until a NEWLINE, or FILEEND
 *	is encountered.  When it is found the appropriate goto
 *	string must be generated.  Then one more call to scan will be 
 *	performed to prepare for the next call to function statement().
 */
    { 
	int looptype, label;

	while (tokencode != NEWLINE && tokencode !=FILEEND )
		tokencode = lex(token);	
	/*
	 *	Examine the top of the stack to see what action should
	 *	be taken.  If currently within a for loop, generate
	 *	a "$ goto (label + 2)."  If in a while, generate a
	 *	"$ goto (label + 1)."
	 */
	if( peek(&looptype, &label) ){
	    if (beginning) emitstring("$ ");
	    emitstring("goto ");
	    if (looptype == FOR || looptype == DO)
		emittarget(label + 2);
	    else
		emittarget(label + 1);
	    emitstring("\n");
	}
	else
	    errmsg("Error -- break statement is not within any loop\n");

	scan();
    }

void nextstmt(int beginning)
/* 
 *	Called by statement() whenever the NEXT keyword is found.  
 *	This function repeatedly calls scan() until it finds a 
 *	NEWLINE, or the end of the input file is encountered.  
 *	After any of these tokens are found the appropriate goto 
 *	string is generated to continue loop execution.  
 *	Then one final call to scan is performed to prepare for 
 *	any further calls to statement().
 */
    {
	int looptype, label;

	while (tokencode != NEWLINE && tokencode != FILEEND )
		tokencode = lex(token);
	/*
	 *	Examine the top of the stack to see what action should
	 *	be taken.  If currently within a for loop, generate
	 *	a "$ goto (label + 1)."  If in a while, generate a
	 *	"$ goto label."
	 */
	if( peek(&looptype, &label) ) {
	    if (beginning) emitstring("$ ");
	    emitstring("goto ");
	    if (looptype == FOR || looptype == DO)
		emittarget(label + 1);
	    else if (looptype == WHILE)
		emittarget(label);
	    else
		errmsg("Error -- next statement is not within any loop\n") ;
	    emitstring("\n");
	}
	else
	    errmsg("Error -- next statement is not within any loop\n");

	scan();
    }

void condition()
/*
 *	This function will be called whenever a DCL condition needs
 *	to be evaluated.  The current token should be an OPAREN, but
 *	it may not be.  DCL conditions may have nested sets of 
 *	parentheses and these must be handled by this function.
 *	Initialize a variable, paren_count, to one.
 *	Whenever another OPAREN is encountered increment paren_count.
 *	Decrement paren_count when a CPAREN is found.  Repeat until
 *	paren_count goes to zero and a CPAREN has been found.  
 *	Be sure to return with CPAREN as the current token.
 *
 *	Since this condition can also appear in a for statement of
 *	sdcl condition can also be terminated by a SEMICOLON.
 *
 *	For the code generation portion part of this function merely
 *	pass anything encountered to emistring(), except for the
 *	closing, CPAREN.
 *
 */
    {
	short paren_count = 1;

	/* 
	** Output the current token and update the state of paren_count
	** appropriately.  This is necessary to catch parenthesized conditional
	** forms:
	**
	** 	if ( (cond1) .op. (cond2) ...) ...
	*/

	switch (tokencode)
	{
		case NEWLINE	: break ; /* eatup newlines */

		case CPAREN	:	/* A condition of the form:
					** ().  This should probably be a
					** syntax error. */
		case SEMICOLON  : return ;  /* in a for loop */

		case GRAVE :
		case STRING :	emitqstring(token) ;
				break ;	/* Quoted strings in conditionals   */
					/* should be preserved.		    */

		case OPAREN	: paren_count++;
		default		: emitstring(token);
				  break;
	}

	/*
	 *	Output all tokens returned by lex() except for
	 *	the last CPAREN.  Maintain level of parentheses
	 *	nesting via increments and decrements to paren_count.
	 *	The loop will terminate whenever either a SEMICOLON
	 *	is found or a CPAREN is found when paren_count is
	 *	zero.
	 */
	do{
		tokencode = lex(token);
		switch (tokencode)
		{
			case OPAREN	: paren_count++;
					  emitstring(token);
					  break;

			case CPAREN	: paren_count--;
					  if (paren_count)
					  	emitstring(token);
					  break;

			case NEWLINE	:  /* eatup newlines */
			case SEMICOLON  : break;  /* in a for loop */

			case GRAVE :
		        case STRING :	emitqstring(token) ;
				        break ;	
                                        /* Quoted strings in conditionals   */
					/* should be preserved.		    */

			default		: emitstring(token);
					  break;
		}
	} while (paren_count && tokencode != SEMICOLON &&
		 tokencode != FILEEND );

	/* 
	 *	At the end of this function the current token will be
	 *	either a CPAREN or a SEMICOLON.
	 */
}

void forstmt(int beginning)
/*
 *	The code generation porion of this function is somewhat involved.
 *	The reinitialization portion of the code is saved in a temporary
 *	buffer called rstring[].  It is put into rsring by a call to
 *	reinitialize().  Then the statement portion of the input stream
 *	is read and processed.  Then the reinitialization code is
 *	output after the proper transfer label is output.
 */
    {
	int lab1, lab2, lab3, looptype;
	char rstring[MAXCONDLEN];

	scan();
	if (tokencode == OPAREN)
		scan();
	else
		errmsg("Error--missing a '(' in for statement\n");
	/*  Get the three labels necessary for a for-statement. */
	lab1 = genlab();
	lab2 = genlab();
	lab3 = genlab();

	/*  
	 *	If the next token is not a SEMICOLON then an initialization
	 *	statement exists for this for statement.  Process it via
	 *	a call to initialize().
	 */
	if (tokencode != SEMICOLON)
	{
		initialize(beginning);
		beginning = 1 ;
	}

	if (beginning) emitstring("$ ");
	emitlabel(lab1);
	emitstring("BREAK = \"") ;
	emittarget(lab3) ;
	emitstring("\"\n") ;

	emitstring("$ NEXT = \"") ;
	emittarget(lab2) ;
	emitstring("\"") ;

	scan();

	/*
	 *	If the next token isn't a SEMICOLON assume that a
	 *	condition statement exists.  Begin building the
	 *	string to test this condition.  Call condition()
	 *	to parse and output the relational expression, then
	 *	finish building the statement by adding the "goto"
	 *	clause with the appropriate transfer address for
	 *	when the condition fails.
	 */
	if (tokencode != SEMICOLON){
		emitstring("\n") ;
		emitstring("$ if (.not.(");
		condition();
		emitstring(")) then goto ");
		emittarget(lab3);
	}
	emitstring("\n");
	scan();
	/*
	 *	If the next token is not a CPAREN then we know that
	 *	we don't have a null reinitialization statement.  
	 *	Call reinitialize() to parse and save this statement.
	 *	The statement will be saved in rstring[] for later
	 *	use after the statement portion of the for-loop
	 *	has been processed.
	 */
	rstring[0] = '\0';
	if (tokencode != CPAREN) 
		reinitialize(rstring);

	if (tokencode == CPAREN)
		scan();
	else
		errmsg("Error--missing a ')' in for statement\n");
	/*
	 *	Push lab1 onto the stack to enable processing of break
	 *	and next statements.  Process the action portion of 
	 *	this while-loop. 
	 */
	push(FOR, lab1);
	statement(1);
	/*
	 *	Build the label that preceedes the reinitialization stuff,
	 *	and, if this stuff isn't NULL, output it after the label.
	 *	Finish this line by then appending a newline character.
	 *
	 *	This label will be the destination for any next statements.
	 */
	emitstring("$ ");
	emitlabel(lab2);
	if (rstring[0])
		emitstring(rstring);
	emitstring("\n");
	/*
	 *	Build the statement to transfer back to the beginning
	 *	of the conditional test, output it, then insert the
	 *	target label for when the condition fails.  This last target
	 *
	 */
	emitstring("$ goto ");
	emittarget(lab1);
	emitstring("\n");
	emitstring("$ ");
	emitlabel(lab3);
	emitstring("\n");

	pop(&looptype, &lab1);
    }

void initialize(beginning)
/*
 *	Parses the initialization section of a for statement.  All
 *	that is necessary is to scan until a SEMICOLON is found.
 *
 *	While parsing pass on any non-SEMICOLON tokens to the
 *	output stream.  After a SEMICOLON has been found flush
 *	the output buffer by appending a newline character to
 *	the output stream.
 */
    {
	if (beginning) emitstring("$ ");
	while (tokencode != SEMICOLON && tokencode != FILEEND ) {
		emitstring(token);
		tokencode = lex(token);
	}
	emitstring("\n");
    }

void reinitialize(str)
    char *str;
/*
 *	Parses the reinitialization section of a for statement.  All
 *	that is necessary is to scan until the last CPAREN is found.
 *	For the code generation all tokens must be saved in the
 *	input character string, str.  So for each token found, add
 *	it to str.
 */
    {
	short i = 0, j, paren_count = 0 ;

	/*
	** Make sure that a [potential] opening parenthesis is counted.
	*/

	if (tokencode == OPAREN)
		paren_count++ ;

	/*
	** This loop termiates when either the end of file is hit or
	** the first unbalanced closing parenthesis is hit.  This makes sure
	** that things like lexical function calls get correctly processed,
	** e.g.:
	**
	**	for ( ; ; foo = f$search(file,1)) {}
	**
	** in the increment.
	*/

	while (((tokencode != CPAREN) || (paren_count >= 0)) && 
	       (tokencode != FILEEND)) {
		j = 0;
		/* Move all of token, except for the NULL, into str. */
		while( token[j] )
			str[i++] = token[j++];

		tokencode = lex(token);

		switch (tokencode)
		{
			case OPAREN	: paren_count++ ;
					  break ;

			case CPAREN	: paren_count-- ;
					  break ;

			default		: break ;
		} ;
	}
	/* When done append a NULL to str. */
	str[i] = '\0';
    }

void popBreakNext()
{
    int looptype, label ;
    if (peek(&looptype, &label) )
    {
	if ((looptype == FOR) || (looptype == DO))
	{
	    emitstring("$ BREAK = \"") ;
	    emittarget(label + 2) ;
	    emitstring("\"\n") ;

	    emitstring("$ NEXT = \"") ;
	    emittarget(label + 1) ;
	    emitstring("\"\n") ;
	}
	else if (looptype == WHILE)
	{
	    emitstring("$ BREAK = \"") ;
	    emittarget(label + 1) ;
	    emitstring("\"\n") ;

	    emitstring("$ NEXT = \"") ;
	    emittarget(label) ;
	    emitstring("\"\n") ;
	}
    }
}
