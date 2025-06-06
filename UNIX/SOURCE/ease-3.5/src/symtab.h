/*
 *	symtab.h    -- Definitions related to the "et" symbol table. 
 *
 *	author	    -- James S. Schoner, Purdue University Computing Center,
 *					 West Lafayette, Indiana  47907
 *
 *	date	    -- July 1, 1985
 *
 *	Copyright (c) 1985 by Purdue Research Foundation
 *
 *	All rights reserved.
 *
 * $Log: symtab.h,v $
 * Revision 3.0  1991/02/22  18:50:27  barnett
 * Added support for HP/UX and IDA sendmail.
 *
 * Revision 2.1  1990/01/30  15:58:35  jeff
 * Added SunOS/Ultrix/IDA extensions  Jan 24, 1989 Bruce Barnett
 *
 * Revision 2.0  88/06/15  14:43:08  root
 * Baseline release for net posting. ADR.
 */

#define TRUE      1
#define FALSE     0
#define SST       101		/* size of hash table (symbol table) 	     */
#define RSNMAX    5		/* size of a ruleset number character buffer */
#define VALRSNMAX 9999		/* max value of ruleset number		     */


/* identifier types */
#define ID_UNTYPED 0
#define ID_MACRO   01
#define ID_CLASS   02
#define ID_RULESET 04
#define ID_FIELD   010
#define ID_PREC	   020
#define ID_MAILER  040

/* identifier type macros */
#define ISTYPED(x) (x|ID_UNTYPED)
#define ISMACRO(x) (x&ID_MACRO)
#define ISCLASS(x) (x&ID_CLASS)
#define ISRULESET(x) (x&ID_RULESET)
#define ISFIELD(x) (x&ID_FIELD)
#define ISPREC(x) (x&ID_PREC)
#define ISMAILER(x) (x&ID_MAILER)

/* block definition types */
enum bdefs {def_macro, def_class, def_option, def_prec, def_trusted, 
	    def_header, def_mailer, def_ruleset};

/* option types */
enum opts {opt_A, opt_a, opt_B, opt_C, opt_c, opt_D, opt_d, opt_e, opt_F,
	   opt_f, opt_g, opt_H, opt_i, opt_L, opt_m, opt_N, opt_n, opt_o,
	   opt_P, opt_Q, opt_q, opt_r, opt_S, opt_s, opt_T, opt_t, opt_u,
	   opt_v, opt_W, opt_x, opt_X, opt_Y, opt_y, opt_Z, opt_z,
	   opt_b, opt_R, opt_SL, opt_h, opt_I,
	   d_opt_i, d_opt_b, d_opt_q,
	   e_opt_p, e_opt_e, e_opt_m, e_opt_w, e_opt_z};

/* flag types */
enum flgs {flg_f, flg_r, flg_S, flg_n, flg_l, flg_s, flg_m, flg_F, flg_D,
	   flg_M, flg_x, flg_P, flg_u, flg_h, flg_A, flg_U, flg_e, flg_X,
	   flg_H, flg_V, flg_B,
	   flg_L, flg_p, flg_I, flg_C, flg_E, flg_R};

/* mailer parameters */
enum mats {mat_path, mat_flags, mat_sender, mat_recipient, mat_argv, 
	   mat_eol, mat_maxsize};

struct he {	/* hash entry structure for symbol table node 	*/
	unsigned   idtype;	/* identifier type 		*/
	unsigned   idd;	  	/* identifier definition flag 	*/
	char      *psb;		/* identifier string buffer 	*/
	union {
		char rsn[RSNMAX]; 	/* ruleset number   	      */
		int prec;	  	/* precedence value 	      */
		char idc;		/* one char id representation */
		char *fstring;    	/* field string     	      */
	} idval;
	struct he *phe;		/* next hash entry 		*/
};
