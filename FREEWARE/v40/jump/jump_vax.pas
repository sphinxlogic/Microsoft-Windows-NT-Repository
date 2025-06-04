{************************************************************************
*									*
*  J U M P _ V A X							*
*									*
*  JUMP is a program which allows selected users (Operators, Systems	*
*  Programmers and other specifically authorised users) to either	*
*  change elements of their process to those of another user, or to	*
*  actually become a given user in a separate process attached to a	*
*  pseudo-terminal.							*
*									*
*  JUMP_VAX contains VAX architecture-specific code for JUMP.		*
*.......................................................................*
*  Author:  Jonathan Ridler,						*
*	    Information Technology Services,				*
*	    The University of Melbourne,				*
*	    Parkville, Victoria,					*
*	    AUSTRALIA, 3052.						*
*									*
*	    Internet: jonathan@unimelb.edu.au				*
*.......................................................................*
*  History:								*
*	05-Aug-1997	JER	Original version for JUMP v2.9.		*
************************************************************************}

MODULE Ucb_Vax ;

TYPE

Unsigned_Ptr = ^UNSIGNED ;

VAR

CCB$L_UCB ,
CCB$K_LENGTH ,
UCB$L_TL_PHYUCB : [EXTERNAL,VALUE] UNSIGNED ;

CTL$GL_CCBBASE : [EXTERNAL] UNSIGNED ;

PROCEDURE Getmem (VAR Location ,
		  Pointer  : [VOLATILE,UNSAFE] Unsigned_Ptr) ; EXTERNAL ;

[GLOBAL] PROCEDURE Get_Ucb (Chan_Num : UNSIGNED ;
			    VAR Ucb : [VOLATILE] Unsigned_Ptr) ;

{ Use the logical UCB address in the CCB to get to the physical UCB. }

  VAR Inx : UNSIGNED := 0 ;

  BEGIN
  Inx := Chan_Num DIV 16 * CCB$K_LENGTH ;
  Ucb::UNSIGNED := CTL$GL_CCBBASE - Inx - CCB$L_UCB ;	{ Logical UCB address }

  Getmem (Ucb,Ucb) ;				{ Logical UCB }
  Ucb::UNSIGNED := Ucb::UNSIGNED +
		   UCB$L_TL_PHYUCB ;		{ Physical UCB address }
  Getmem (Ucb,Ucb) ;				{ Physical UCB }
  END ;

END.
