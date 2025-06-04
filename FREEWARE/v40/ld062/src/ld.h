/*
  Facility:

        VAX/VMS Logical Disk Utility

  Abstract:

	Include file for LD management utility

  Author:

        Jur van der Burg
        vdburg@utrtsc.uto.dec.com

*/

#include <stdio.h>
#include <stsdef.h>
#include <rms.h>
#include <xabfhcdef.h>
#include <fibdef.h>
#include <fiddef.h>
#include <sbkdef.h>
#include <atrdef.h>
#include <lnmdef.h>
#include <lkidef.h>
#include <lckdef.h>
#include <dvidef.h>
#include <syidef.h>
#include <jpidef.h>
#include <libdef.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <iodef.h>
#include <descrip.h>
#include <climsgdef.h>
#include <file.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <prvdef.h>
#include <psldef.h>
#include <starlet.h>
#include "ld_defines.h"

#ifndef SS$_NOMOREDEV
#define SS$_NOMOREDEV 2648	/* Not in my headers ... */
#endif
#ifndef FCH$M_NOMOVE
#define FCH$M_NOMOVE	0x200000	/* May not in be there */
#endif
#ifndef FCH$M_NOBACKUP
#define FCH$M_NOBACKUP	0x2		/* May not in be there */
#endif
#ifndef IO$M_TRUSTED
#define IO$M_TRUSTED	0x80000		/* May not in be there */
#endif
#ifndef IO$M_NOVCACHE
#define IO$M_NOVCACHE	0x20000		/* May not in be there */
#endif
#ifndef IO$M_EXFUNC
#define IO$M_EXFUNC	0x2000		/* May not in be there */
#endif

#define DATA_FILE_VERSION	0x04

/*
  Structure definitions and declarations
*/

struct dsc {
            int len;
            char *addr;
           };

struct itmlst {
               short buflen;
               short item;
               int addr;
               int rlength;
              };

struct iosb {
            short status;
            short size;
            union {
                int lw;
                struct {
                    unsigned int connected:1,
                                 replaced:1,
                                 decram:1,
                                 protect:1,
                                 share:1;
                } flags;
            } u;
           };

#define iosb_lw    iosb.u.lw
#define iosb_flags iosb.u.flags

struct ovrrun {
	    int recnum;
	    int overrun;
	   };

struct cmndtbl {
            char *what;
            int (*where)();
           };

struct lksb {
            short status;
            short reserved;
            int lockid;
            int valblk[4];
           };

struct FAB ld_fab;
struct RAB ld_rab;
struct NAM ld_nam;
struct XABFHC ld_xabfhc;

struct atrdef atr[2];
struct sbkdef sbk;
struct fibdef fib;

/*
  Global variables
*/

int maxblocks;
short fchan;
short outband_chan = 0;
char *s_file, *s_device, *outfilename;
struct lksb ld_lksb;
$DESCRIPTOR(wildname,"*LD*");
FILE *outfile = -1;
int outfilei = -1;
char realspec[NAM$C_MAXRSS];
char resspec[NAM$C_MAXRSS];
char expspec[NAM$C_MAXRSS];
struct dsc realspec_dsc;
struct dsc realdev_dsc;
struct dsc ldfilename_dsc;
struct dsc fib_dsc;

/*
  Global descriptors for command parsing
*/

$DESCRIPTOR(command,"COMMAND");
$DESCRIPTOR(sw_file,"FILE");
$DESCRIPTOR(sw_device,"DEVICE");
$DESCRIPTOR(sw_lblock,"LBLOCK");
$DESCRIPTOR(sw_trace,"TRACE");
$DESCRIPTOR(sw_watch,"WATCH");
$DESCRIPTOR(sw_resume,"RESUME");
$DESCRIPTOR(sw_abort,"ABORT");
$DESCRIPTOR(sw_stop,"STOP");
$DESCRIPTOR(sw_size,"SIZE");
$DESCRIPTOR(sw_tracks,"TRACKS");
$DESCRIPTOR(sw_sectors,"SECTORS");
$DESCRIPTOR(sw_cylinders,"CYLINDERS");
$DESCRIPTOR(sw_maxblocks,"MAXBLOCKS");
$DESCRIPTOR(sw_index,"INDEX");
$DESCRIPTOR(sw_share,"SHARE");
$DESCRIPTOR(sw_all,"ALL");
$DESCRIPTOR(sw_alloclass,"ALLOCLASS");
$DESCRIPTOR(sw_version_limit,"VERSION_LIMIT");
$DESCRIPTOR(sw_blocks,"BLOCKS");
$DESCRIPTOR(sw_backup,"BACKUP");
$DESCRIPTOR(sw_log,"LOG");
$DESCRIPTOR(sw_continuous,"CONTINUOUS");
$DESCRIPTOR(sw_reset,"RESET");
$DESCRIPTOR(sw_replace,"REPLACE");
$DESCRIPTOR(sw_status,"STATUS");
$DESCRIPTOR(sw_header,"HEADER");
$DESCRIPTOR(sw_warnings,"WARNINGS");
$DESCRIPTOR(sw_binary,"BINARY");
$DESCRIPTOR(sw_symbol,"SYMBOL");
$DESCRIPTOR(sw_entries,"ENTRIES");
$DESCRIPTOR(sw_pid,"PID");
$DESCRIPTOR(sw_lbn,"LBN");
$DESCRIPTOR(sw_bytecount,"BYTECOUNT");
$DESCRIPTOR(sw_iosb,"IOSB");
$DESCRIPTOR(sw_function,"FUNCTION");
$DESCRIPTOR(sw_timestamp,"TIMESTAMP");
$DESCRIPTOR(sw_number,"NUMBER");
$DESCRIPTOR(sw_input,"INPUT");
$DESCRIPTOR(sw_output,"OUTPUT");
$DESCRIPTOR(sw_time_absolute,"TIMESTAMP.ABSOLUTE");
$DESCRIPTOR(sw_time_delta,"TIMESTAMP.DELTA");
$DESCRIPTOR(sw_time_elapsed,"TIMESTAMP.ELAPSED");
$DESCRIPTOR(sw_time_combination,"TIMESTAMP.COMBINATION");
$DESCRIPTOR(sw_iosb_hex,"IOSB.HEX");
$DESCRIPTOR(sw_iosb_longhex,"IOSB.LONGHEX");
$DESCRIPTOR(sw_iosb_text,"IOSB.TEXT");
$DESCRIPTOR(sw_iosb_combination,"IOSB.COMBINATION");
$DESCRIPTOR(sw_function_hex,"FUNCTION.HEX");
$DESCRIPTOR(sw_function_text,"FUNCTION.TEXT");
$DESCRIPTOR(sw_function_read,"FUNCTION.READ");
$DESCRIPTOR(sw_function_write,"FUNCTION.WRITE");
$DESCRIPTOR(sw_function_code,"FUNCTION.CODE");
$DESCRIPTOR(sw_function_all,"FUNCTION.ALL");
$DESCRIPTOR(sw_action,"ACTION");
$DESCRIPTOR(sw_action_suspend,"ACTION.SUSPEND");
$DESCRIPTOR(sw_action_crash,"ACTION.CRASH");
$DESCRIPTOR(sw_action_error,"ACTION.ERROR");
$DESCRIPTOR(sw_action_opcom,"ACTION.OPCOM");

/*
  Routine declarations
*/

int getqual();
int getqualvalue();
int getmulqualvalue();
char *getqualstring();
int *getlist();
void show();
int show_one();
void connect_unit();
void disconnect_unit();
int disconnect_one();
void open_file();
void close_file();
void create();
void trace();
void stop_trace();
void stop_one();
void notrace();
void watch();
void show_watch();
void display_watch();
void nowatch();
void protect();
void noprotect();
void process_trace();
void show_trace_data();
void show_trace();
void process_input();
void setfilechar();
void set_outband();
void ld_exit();
char *ssdef();
char *decode_func();
char *cvttime();
char *nodename();
char *build_lock();
void purge_file();
void chk_mod();
int convert_error();
void create_filename();
void setup_fab();
char *fulldevspec();
char *fullfilespec();
void signal_error();

void getdevnam();
int get_unit();
void set_seed();

/*
  VMS Routines
*/

int cli$present();
int cli$get_value();

/*
 Other routines
*/

char *malloc();

struct cmndtbl cmnds[] = {
         "CREA",&create,
         "CONN",&connect_unit,
         "DISC",&disconnect_unit,
         "SHOW",&show,
         "TRAC",&trace,
         "NOTR",&notrace,
         "WATC",&watch,
         "NOWA",&nowatch,
         "PROT",&protect,
         "NOPR",&noprotect,
         "",0
    };

/*
  External condition codes defined in message file
*/

globalref int ld_connected,
              ld_nowdisconn,
              ld_notconnected,
              ld_alrdyconn,
              ld_devassign,
              ld_notconn,
              ld_fileonvolset,
              ld_fileinuse,
              ld_deviceinuse,
              ld_created,
              ld_filwrterr,
              ld_filreaderr,
              ld_createrr,
              ld_openerr,
              ld_closerr,
              ld_infilerr,
              ld_outfilerr,
              ld_notrcdata,
              ld_noworldpriv,
              ld_nogrouppriv,
              ld_conttraceact,
              ld_conttracenotact,
              ld_tracestopped,
              ld_trcenabl,
              ld_trcdisabl,
              ld_status,
              ld_pastdata,
              ld_badentparam,
              ld_confqual,
              ld_cantreadoldfmt,
              ld_unit,
              ld_dupunit,
              ld_badunit,
              ld_nounitsfound,
              ld_baddevsyntax,
              ld_detectederr,
              ld_nowatchdata,
              ld_wptnotfound,
              ld_noreadwrite,
              ld_vbnerror,
              ld_fileonother,
              ld_nocluster,
              ld_notvisible,
              ld_invgeometry,
              ld_badalloclass,
              ld_devconnected,
              ld_remotealloc,
              ld_notods2;

/*
  Conversiontable to convert I/O functioncodes to human readable text
*/
char *cvttbl[] = {
        "NOP",
        "UNLOAD",
        "SEEK",
        "RECAL",
        "DRVCLR",
        "RELEASE",
        "OFFSET",
        "RETCENTER",
        "PACKACK",
        "READRCT",
        "WRITECHECK",
        "WRITEPBLK",
        "READPBLK",
        "CRESHAD",
        "ADDSHAD",
        "COPYSHAD",
        "REMSHAD",
        "AVAILABLE",
        "SETPRFPATH",
        "DISPLAY",
        "FUNC_20",
        "DSE",
        "REREADN",
        "REREADP",
        "WRITERET",
        "STARTSPNDL",
        "SETCHAR",
        "SENSECHAR",
        "WRITEMARK",
        "SHADMV",
        "FORMAT",
        "PHYSICAL",
        "WRITELBLK",
        "READLBLK",
        "READRCTL",
        "SETMODE",
        "REWIND",
        "SKIPFILE",
        "SKIPRECORD",
        "SENSEMODE",
        "WRITEOF",
        "TTY_PORT",
        "FLUSH",
        "READLCHUNK",
        "WRITELCHUNK",
        "FUNC_45",
        "FUNC_46",
        "LOGICAL",
        "WRITEVBLK",
        "READVBLK",
        "ACCESS",
        "CREATE",
        "DEACCESS",
        "DELETE",
        "MODIFY",
        "READPROMPT",
        "ACPCONTROL",
        "MOUNT",
        "TTYREADALL",
        "TTYREADPALL",
        "CONINTREAD",
        "CONINTWRITE",
        "FUNC_62",
        "VIRTUAL"
    };
