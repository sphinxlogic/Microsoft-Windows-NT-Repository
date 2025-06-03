$
$! P1 = Compiled C source name
$! P2 = Optional "/DEBUG"
$!
$! RMS_TRACE tool. This command file will link against (and create if
$! need be) a shareable image with all RMS entrypoints. When called,
$! an identification of the operation and common inputs such are FAB (RAB)
$! FOP (ROP) and IFI (ISI) are printed. Then RMS operation is requested
$! and upon return the result such as IFI (ISI) or RFA is displayed.
$!
$! Desirable enhancements would be a logical name to control how much to
$! be printed (FOP? IFI?), adding a timestamp, a sequence number, and 
$! a logical name to redirect the output and or provide binary output.
$! The ultimate enhencement would be a privved flavor to be integrated
$! with SET WATCH and which would redirect the P1 vector thus not 
$! requiring relinks and support shareable image RTLs
$!
$! Hein van den Heuvel, August 1993.
$
$this_file = F$ENVIRONMENT ( "PROCEDURE" )
$trace_nam = F$PARSE(this_file,,,"NAME","SYNTAX_ONLY")
$trace_dir = F$PARSE(this_file,,,"DEVICE","SYNTAX_ONLY") + -
	     F$PARSE(this_file,,,"DIRECTORY","SYNTAX_ONLY")
$trace_exe = trace_dir + trace_nam + ".EXE"
$if f$trnlnm("RMS_TRACE").EQS."" then DEFINE RMS_TRACE 'trace_exe
$shareable = f$search("RMS_TRACE","SYS$SYSTEM:.EXE")
$if shareable.eqs."" then goto make_shareable
$
$! Must resolve all VAXCIO reference from the VAXCRTL.OLB to allow 
$! it to resolve its RMS references from the provided trace tool.
$! For VAX COBOL, MACRO, and BLISS, this is not needed.
$! FORTAN, BASIC and PASCAL are not supported as they do not provide 
$! an RTL in OLB form and already have the RMS references resolved.
$
$make_executable:
$link/nosysshr 'p2' 'p1',SYS$LIBRARY:VAXCRTL/LIB,SYS$INPUT/OPT
RMS_TRACE/SHARE
SYS$LIBRARY:LIBRTL/SHARE
$
$EXIT
$
$make_shareable:
$
$trace_macro = trace_dir + trace_nam + "_VECTOR"
$trace_macro_source = trace_macro + ".MAR"
$trace_macro_object = trace_macro + ".OBJ" 
$trace_c = trace_dir + trace_nam + "_DISPLAY"
$trace_c_source = trace_c + ".C"
$trace_c_object = trace_c + ".OBJ"
$if f$search(trace_macro_object).nes."" then goto make_display
$!
$make_vector:
$!
$if f$search(trace_macro_source).nes."" then goto compile_vector
$create 'trace_macro_source'

; RMS_TRACE_VECTOR.MAR 
; Hein van den Heuvel, January 1991
;
; By using a transfer vector, the linker is willing to allow the 
; same name to be used as provided entry point and as called routine.
; without getting confused (loop).
;
.MACRO	VECTOR  NAME
.TRANSFER	NAME
.WORD		^M<R2,R3,R4,R5,R6,R7,R8,R9,R10,R11>
JSB		TRANSFER
.ENDM	VECTOR

TABLE::
	VECTOR	SYS$DELETE
	VECTOR	SYS$FIND
	VECTOR	SYS$FREE
	VECTOR	SYS$GET
	VECTOR	SYS$PUT
	VECTOR	SYS$READ
	VECTOR	SYS$RELEASE
	VECTOR	SYS$UPDATE
	VECTOR	SYS$WAIT
	VECTOR	SYS$WRITE
	VECTOR	SYS$CLOSE
	VECTOR	SYS$CONNECT
	VECTOR	SYS$CREATE
	VECTOR	SYS$DISCONNECT
	VECTOR	SYS$DISPLAY
	VECTOR	SYS$ERASE
	VECTOR	SYS$EXTEND
	VECTOR	SYS$FLUSH
	VECTOR	SYS$MODIFY
	VECTOR	SYS$NXTVOL
	VECTOR	SYS$OPEN
	VECTOR	SYS$REWIND
	VECTOR	SYS$SPACE
	VECTOR	SYS$TRUNCATE
	VECTOR	SYS$ENTER
	VECTOR	SYS$PARSE
	VECTOR	SYS$REMOVE
	VECTOR	SYS$RENAME
	VECTOR	SYS$SEARCH
	VECTOR	SYS$SETDDIR
	VECTOR	SYS$SETDFPROT
	VECTOR	SYS$RMSRUNDWN
	VECTOR	SYS$FILESCAN

TRANSFER::
	MOVAL		TABLE+8, R0	;start of table
	SUBL2		R0, (SP)	;change return address to offset.
	DIVL2		#8, (SP)	;change offset to function code.
	PUSHL		AP		;push callers argument list.
	CALLS		#2, G^RMS_TRACE_DISPLAY ;and off we go.
	RET
	.END
$
$compile_vector:
$macro'p2'/object='trace_macro_object'/list  'trace_macro_source'
$
$!
$
$make_display:
$if f$search(trace_c_object).nes."" then goto link_shareable
$if f$search(trace_c_source).nes."" then goto compile_display
$create 'trace_c_source'
/*
** RMS_TRACE_DISPLAY.C
**
** Desirable enhancements would be a logical name to control how much to
** be printed (FOP? IFI?), adding a timestamp, a sequence number, and 
** a logical name to redirect the output and or provide binary output.
** The ultimate enhancement would be a privved flavor to be integrated
** with SET WATCH and which would redirect the P1 vector thus not 
** requiring relinks and support shareable image RTLs
** 
** This source must avoid using any C RTL IO functions as to avoid 
** creating a reference to VAXCRTL.EXE which makes the ultimate 
** link of the application program it too tricky.
**
** Hein van den Heuvel, January 1991, 
** Modified for C usage September 1993
**
*/
#define descriptor(name,string)	const noshare struct desc name = {
sizeof(string)-1, string }
#include <FAB>
#include <RAB>

/* Standard include files unfortunatly generate a strong (not weak) reference
**  to cc$rms_xxx. This module does not need those, so it provides dummy
*/
globaldef struct FAB cc$rms_fab;
globaldef struct RAB cc$rms_rab;

extern  int 	SYS$DELETE(), SYS$FIND(),   SYS$FREE(),   SYS$GET(),    
		SYS$PUT(),    SYS$READ(),   SYS$RELEASE(),SYS$UPDATE(),
		SYS$WAIT(),   SYS$WRITE(),  SYS$CLOSE(),  SYS$CONNECT(),
		SYS$CREATE(), SYS$DISCONNECT(),		  SYS$DISPLAY(),
		SYS$ERASE(),  SYS$EXTEND(), SYS$FLUSH(),  SYS$MODIFY(),
		SYS$NXTVOL(), SYS$OPEN(),   SYS$REWIND(), SYS$SPACE(),
		SYS$TRUNCATE(),		    SYS$ENTER(),  SYS$PARSE(),
		SYS$REMOVE(), SYS$RENAME(), SYS$SEARCH(), SYS$SETDDIR(),
		SYS$SETDFPROT(),	SYS$RMSRUNDWN(),  SYS$FILESCAN();

noshare int	retlen;
noshare struct desc { int buflen; char *buffer; } work, out;

descriptor (d_rfa, " (!5XL,!3XW)");
descriptor (d_ubf, " S=!4XW, A=!XL");
descriptor (d_dump, " S=!4XW, A=!XL !AF");
descriptor (d_ifisi, "- ISI/IFI = !XW ");
descriptor (d_rac1, " SEQ");
descriptor (d_rac2, " RFA");
descriptor (d_rac3, " KEY");
descriptor (d_rac4, " ?? RAC = !XB");
descriptor (d_rop, " ROP/FOP = !XL");
descriptor (d_fnm_dnm1, " FNM=<!AF>");
descriptor (d_fnm_dnm2, " <NO default/filename>");
descriptor (d_fnm_dnm3, " DNM=<!AF>");
descriptor (d_main, "-!AD !XL !XL !XW");
descriptor (d_stat, "- Returned status was !XL, STV = !XL");

void special ( struct RAB *rab )
{
    return;
}

void rfa ( struct RAB *rab )
{
    sys$fao ( &d_rfa, &retlen, &work,  rab->rab$l_rfa0, rab->rab$w_rfa4 );
    return;
}

void dump ( char *buf, int size)
{
    int s;
    s = size;
    if (size > 20) s = 20;
    sys$fao ( &d_dump, &retlen, &work, size, buf, s, buf);
    return;
}

void rbf ( struct RAB *rab )
{
    dump ( rab->rab$l_rbf, rab->rab$w_rsz);
    return;
}

void ubf ( struct RAB *rab )
{
    sys$fao ( &d_ubf, &retlen, &work, rab->rab$w_usz, rab->rab$l_ubf );
    return;
}

void ifisi ( struct FAB *fab )
{
    sys$fao ( &d_ifisi, &retlen, &work, fab->fab$w_ifi );
    return;
}

void rop ( struct FAB *fab )
{
    sys$fao ( &d_rop, &retlen, &work, fab->fab$l_fop );
    return;
}

void rac ( struct RAB *rab )
{
    void    *p;
    switch (rab->rab$b_rac)
    {
	case RAB$C_SEQ:
	    p = &d_rac1;
	    break;
	case RAB$C_RFA:
	    p = &d_rac2;
	    break;
	case RAB$C_KEY:
	    p = &d_rac3;
	    break;
	default:
	    p = &d_rac4;
	    break;
    }
    sys$fao ( p, &retlen, &work, rab->rab$b_rac);

    switch (rab->rab$b_rac)
    {
	case RAB$C_RFA:
	    work.buffer += retlen;
	    out.buflen += retlen;
	    rfa( rab);
	    return;
	case RAB$C_KEY:
	    work.buffer += retlen;
	    out.buflen += retlen;
	    dump (rab->rab$l_kbf, rab->rab$b_ksz);
	    return;
	default:
	    return;
    }
}

void rfa_rbf ( struct RAB *rab )
{
    rfa ( rab );
    work.buffer += retlen;
    out.buflen += retlen;
    rbf ( rab );
    return;
}

void rac_rbf ( struct RAB *rab )
{
    rac( rab );
    work.buffer += retlen;
    out.buflen += retlen;
    rbf ( rab );
    return;
}

void rfa_ubf ( struct RAB *rab )
{
    rfa( rab );
    work.buffer += retlen;
    out.buflen += retlen;
    ubf( rab );
    return;
}

void rac_ubf ( struct RAB *rab )
{
    rac( rab );
    work.buffer += retlen;
    out.buflen += retlen;
    ubf( rab );
    return;
}

void fnm_dnm ( struct FAB *fab )
{
    if (fab->fab$b_fns) 
    {
	sys$fao ( &d_fnm_dnm1, &retlen, &work, fab->fab$b_fns, fab->fab$l_fna);
    }
    else    
        if (fab->fab$b_dns == 0)  sys$fao ( &d_fnm_dnm2, &retlen, &work);

    if (fab->fab$b_dns) 
	{
	work.buffer += retlen;
	out.buflen += retlen;
	sys$fao ( &d_fnm_dnm3, &retlen, &work, fab->fab$b_dns, fab->fab$l_dna);
	}
    return;
}

readonly struct {
		int	(*sys)();
		void	(*pre)();
		void	(*post)();
		char	name[4];
		} functions[] = {

		    SYS$DELETE,	    rfa,	0,	    "DELE",
		    SYS$FIND,	    rac,	rfa,	    "FIND",
		    SYS$FREE,	    rfa,	0,	    "FREE",
		    SYS$GET,	    rac_ubf,	rfa_rbf,    "GET ",
		    SYS$PUT,	    rac_rbf,	rfa,	    "PUT ",
		    SYS$READ,	    rfa_ubf,	rfa_rbf,    "READ",
		    SYS$RELEASE,    0,		0,	    "RELE",
		    SYS$UPDATE,	    rbf,	0,	    "UPDA",
		    SYS$WAIT,	    0,		0,	    "WAIT",
		    SYS$WRITE,	    rfa_rbf,	rfa,	    "WRIT",
		    SYS$CLOSE,	    0,		0,	    "CLOS",
		    SYS$CONNECT,    0,		ifisi,	    "CONN",
		    SYS$CREATE,	    fnm_dnm,	ifisi,	    "CREA",
		    SYS$DISCONNECT, 0,		0,	    "DISC",
		    SYS$DISPLAY,    0,		0,	    "DSIP",
		    SYS$ERASE,	    0,		0,	    "ERAS",
		    SYS$EXTEND,	    0,		0,	    "EXTE",
		    SYS$FLUSH,	    0,		0,	    "FLUS",
		    SYS$MODIFY,	    0,		0,	    "MODI",
		    SYS$NXTVOL,	    0,		0,	    "NEXT",
		    SYS$OPEN,	    fnm_dnm,	ifisi,	    "OPEN",
		    SYS$REWIND,	    0,		0,	    "REWI",
		    SYS$SPACE,	    0,		0,	    "SPAC",
		    SYS$TRUNCATE,   rfa,	0,	    "TRUN",
		    SYS$ENTER,	    fnm_dnm,	0,	    "ENTE",
		    SYS$PARSE,	    fnm_dnm,	0,	    "PARS",
		    SYS$REMOVE,	    0,		0,	    "REMO",
		    SYS$RENAME,	    fnm_dnm,	0,	    "RENA",
		    SYS$SEARCH,	    fnm_dnm,	0,	    "SEAR",
		    SYS$SETDDIR,    special,	0,	    "SDIR",
		    SYS$SETDFPROT,  special,	0,	    "SPRO",
		    SYS$RMSRUNDWN,  special,	0,	    "RUND",
		    SYS$FILESCAN,   special,	0,	    "SCAN"} ;
typedef struct  {
		short argcount;
		short filler;
		struct FAB *fab;
		void	(*err)();
		void	(*suc)();
		} rms_arg_list_type;

int RMS_TRACE_DISPLAY  ( rms_arg_list_type *args, int code)
{
    int	    status, len;
    char    *(*func)();
    char    print_line[255];
    char    *p;
    
    work.buflen = 50;
    work.buffer = &print_line;
    out.buflen = 0;
    out.buffer = &print_line;

    func = functions[code].pre;		/* pick up pre-process function      */

/* 
** Construct common trace line. Only use FAB/RAB if there is one! 
*/
    if (func == special )
	{
	sys$fao ( &d_main, &retlen, &work, 4, functions[code].name, 
	      args->fab, args->err, args->argcount);
	}
    else
	{
	sys$fao ( &d_main, &retlen, &work, 4, functions[code].name, 
	      args->fab, args->fab->fab$l_fop, args->fab->fab$w_ifi );

	if (func)			/* was there a pre-processor? */
	    {
	    work.buffer += retlen;
	    out.buflen += retlen;
	    (func)(args->fab);		/* call pre-processor */
	    }
	}

/*
** Print pre call trace line and reset print buffer.
*/
    out.buflen += retlen;
    lib$put_output ( &out);
    work.buffer = &print_line;
    out.buflen = 0;


/*
** Call actual RMS function requested passing users argument list.
** Simple RMS FAB/RAB only could use: stat = (functions[code].sys)(args->fab);
*/
    status = lib$callg ( args, functions[code].sys);

/*
** Check status printing message upon failure. Could consider printing 
** any status other then RMS$_NORMAL including alternate success values.
*/
    if (!(status&1))
	    {
	    if (func == special )
		{
		sys$fao ( &d_stat, &out.buflen, &work, 
			    status, 0);
		}
	    else
		sys$fao ( &d_stat, &out.buflen, &work, 
			    status, args->fab->fab$l_stv);
	    lib$put_output ( &out);
	    out.buflen = 0;
	    }	

/*
** Deal with post call trace line. Calling function if there is one.
*/
    if (func = functions[code].post)
	{
	(func)(args->fab);
	out.buflen += retlen;
	lib$put_output ( &out);
	}

    return status; 
}
$
$compile_display:
$CC/LIST/MACH/OBJ='trace_c_object''P2' 'trace_c_source'
$
$!
$
$link_shareable:
$
$!sys$library:vaxcrtl/share
$link/share=rms_trace/map'p2' 
'trace_macro_object','trace_c_object',sys$input:/opt
sys$library:vaxcrtl/lib
gsmatch=lequal,1,0
$
$goto make_executable
