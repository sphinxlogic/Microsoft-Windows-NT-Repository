/****************************************************************************/
/*									    */
/*  FACILITY:	Routine Analyzer					    */
/*									    */
/*  MODULE:	Common Parser Header					    */
/*									    */
/*  AUTHOR:	Steve Branam, Network Product Support Group, Digital	    */
/*		Equipment Corporation, Littleton, MA, USA.		    */
/*									    */
/*  DESCRIPTION: This module contains trace macros for the source parsers.  */
/*									    */
/*  REVISION HISTORY:							    */
/*									    */
/*  V0.1-00 24-AUG-1994 Steve Branam					    */
/*									    */
/*	Original version.						    */
/*									    */
/****************************************************************************/

					    /* Trace strings.		    */
#define PLZERO	"\nTRACE: parenthesis level set to zero\n"
#define PLINC	"\nTRACE: parenthesis level incremented to %d\n"
#define PLDEC	"\nTRACE: parenthesis level decremented to %d\n"
#define BLZERO	"\nTRACE: block level set to zero\n"
#define BLINC	"\nTRACE: block level incremented to %d\n"
#define BLDEC	"\nTRACE: block level decremented to %d\n"
#define BLEND	"\nTRACE: block end found\n"

					    /* Guarded trace macros.	    */
#define trace_plmsg(m)   if(trace_plevel_enabled()) trace_parser(m)
#define trace_plint(m,i) if(trace_plevel_enabled()) trace_parser_int(m,i)
#define trace_blmsg(m)	 if(trace_blevel_enabled()) trace_parser(m)
#define trace_blint(m,i) if(trace_blevel_enabled()) trace_parser_int(m,i)
#define trace_state(t,f) if(trace_parser_enabled()) trace_parser_state(t,f)
#define trace_msg(m)	 if(trace_parser_enabled()) trace_parser(m)

					    /* Traceable action macros.	    */
#define change_pstate(n) trace_state(mPSNames[state], mPSNames[n]); state = n
#define block_level_zero()   blevel = 0; trace_blmsg(BLZERO)
#define block_level_inc()    blevel++; trace_blint(BLINC, blevel)
#define block_level_dec()    blevel--; trace_blint(BLDEC, blevel)
#define paren_level_zero()   plevel = 0; trace_plmsg(PLZERO)
#define paren_level_inc()    plevel++; trace_plint(PLINC, plevel)
#define paren_level_dec()    plevel--; trace_plint(PLDEC, plevel)
