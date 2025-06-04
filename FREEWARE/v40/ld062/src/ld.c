/*
  Facility:

	VAX/VMS Logical Disk Utility

  Abstract:

	This utility enables the user to create, setup and display
	information of the Logical Disks available in  the system,
	via the LDDRIVER.

  Author:

	Jur van der Burg 	13-OCT-1992	Version 2.0
	vdburg@utrtsc.uto.dec.com

	Rewritten from original MACRO version (by A. Sweep) for easier
	maintenance.

  Revision:

	Jur van der Burg 	17-FEB-1993	Version 3.0

	Added trace commands
	Minor bugfixes

	Jur van der Burg 	23-FEB-1993	Version 3.1

	Expand given devicespec in 'replace' mode to full spec instead
	of spec from the commandline.
	Add more trace functionality

	Jur van der Burg 	14-APR-1993	Version 4.0

	Modified command interface to use DCLTABLES
	Added cloned device support
	Reworked error handling and display.
	Add remote trace stop

	Jur van der Burg 	8-JUL-1993	Version 4.1

	Corrected bytecount display and iosb display to use longword
	instead of word field for data.
	Bumped datafile versionnumber because of change in trace data
	layout.

	Jur van der Burg 	14-OCT-1993	Version 5.0

	Add WATCH commands
	Adapted for new interface with LDdriver

	Jur van der Burg 	28-OCT-1994	Version 5.1

	Add /SYMBOL qualifier for CONNECT
	Add /SHARE qualifier to allow sharing of containerfiles clusterwide
	Add /TRACKS, /SECTORS, /CYLINDERS and /MAXBLOCKS switches to
	CONNECT to specify geometry, removed /ALLOCATED (superceeded by
	/MAXBLOCKS). Also added /ALLOCLASS to set allocation class which
	may be different from the system parameter ALLOCLASS.

	Jur van der Burg 	15-NOV-1996	Version 6.0

	Lifted restriction for contiguous container file
	Corrected problem setting virtual watchpoint on a file
        Make sure filename is terminated in open_file to prevent errors
        during LD DISCONNECT/ALL/LOG with lots of devices
	Add /FUNCTION=ALL to watchpoint to allow trapping of all I/O's

	Jur van der Burg 	19-AUG-1998	Version 6.2

	Added a way to use more controller letters to bypass the limit of
	9999 cloned devices. Do this by specifying another device on connect:
	LD CONNECT FILE.DSK LDC44. The driver can already handle this.

  Commands:

	- LD CREATE [/LOG] [/SIZE=xxx] [/BACKUP] Filespec
	- LD CONNECT [/LOG] [/SYMBOL] [/REPLACE] [/SHARE]
		[/TRACKS=xxx] [/SECTORS=xxx] [/CYLINDERS=xxx]
		[/MAXBLOCKS=xxx] [/ALLOCLASS=xxx] Filespec [LDan:]
	- LD DISCONNECT [/ALL] [/LOG] [/ABORT] LDan:
	- LD TRACE [/SIZE=xxx] [/RESET] LDan:
	- LD TRACE/STOP [/ALL] [LDan:]
	- LD NOTRACE LDan:
	- LD WATCH LDan: lbn [,lbn...] [/FUNCTION=READ,WRITE,ALL,CODE=xxx]
		[/ACTION=SUSPEND,CRASH,OPCOM,ERROR[=xxx]]
	- LD NOWATCH LDan: [lbn [,lbn...]] [/INDEX=n]
	- LD WATCH/RESUME LDan: [lbn [,lbn...]] [/INDEX=n]
	- LD SHOW [/ALL] [LDan:]
	- LD SHOW/WATCH LDan: [lbn [,lbn...]]
	- LD SHOW/TRACE [/STATUS] [/RESET] [/OUTPUT=Filespec] [/INPUT=filespec]
		[/BINARY] [/ENTRIES=[(XXX,YYY)]] [/HEADER] [/CONTINUOUS]
		[/VERSION_LIMIT=xxx] [/BLOCKS=xxx] [/WARNINGS]
		[/NUMBER] [/PID] [/LBN] [/BYTECOUNT]
		[/IOSB[=COMBINATION,TEXT,HEX,LONGHEX]]
		[/TIMESTAMP[=ABSOLUTE,ELAPSED,COMBINATION,DELTA]]
		[/FUNCTION[=TEXT,HEX]] LDan:

*/

#include "ld.h"

main()
{
    int stat;
    int length;
    char cmdbuf[1024];
    struct dsc commandline =
    {sizeof(cmdbuf), cmdbuf};
    short rlen;
    char *p;
    struct cmndtbl *q;

    ld_fab = cc$rms_fab;
    ld_rab = cc$rms_rab;
    ld_nam = cc$rms_nam;
    ld_xabfhc = cc$rms_xabfhc;
    outfile = 0;

/* Get the rest of the commandline */

    stat = lib$get_foreign(&commandline, 0, &rlen, 0);
    signal_error(stat, 0);
    commandline.len = rlen;
    p = getqualstring(&command);
    length = strlen(p) >= 4 ? 4 : strlen(p);
    q = cmnds;
    while (*(q->what)) {
	if (strncmp(p, q->what, length) == 0) {
	    (*(q->where)) ();	/* Call command */
	    exit(1);
	}
	q++;
    }
    signal_error(CLI$_NOCOMD, 0);
}


int getqual(arg)
struct dsc$descriptor_s *arg;
{
    int stat;

    stat = cli$present(arg);	/* Get qualifier presence */
    if (stat == CLI$_PRESENT ||
	stat == CLI$_LOCPRES ||
	stat == CLI$_DEFAULTED)
	return (1);		/* It's there */
    else if (stat == CLI$_NEGATED || stat == CLI$_LOCNEG)
	return (-1);		/* Negated */
    else if (stat == CLI$_ABSENT)
	return (0);		/* Not there */
    signal_error(stat, 0);
}

int getqualvalue(arg)
struct dsc$descriptor_s *arg;
{
    char value[256];
    struct dsc out =
    {sizeof(value), &value};
    int val;
    short rlen;

    *value = 0;
    if ((cli$present(arg) & 1) &&	/* If present... */
	(cli$get_value(arg, &out, &rlen) & 1)) {	/* Get it's value */
	*(value + (rlen & 0377)) = 0;
	return ((sscanf(value, "%d", &val)) ? val : 0);
    } else
	return (0);		/* Default */
}

int getmulqualvalue(arg, more)
struct dsc$descriptor_s *arg;
int *more;
{
    int stat;
    char value[256];
    struct dsc out =
    {sizeof(value), &value};
    int val;
    short rlen;

    *value = 0;
    if (*more < 0)
	if (!(cli$present(arg) & 1))	/* If not present... */
	    return (0);
    stat = cli$get_value(arg, &out, &rlen);	/* Get it's value */
    *more = 0;
    if (!(stat & 1))
	return (0);
    if (stat == CLI$_COMMA)
	*more = 1;
    *(value + (rlen & 0377)) = 0;
    return ((sscanf(value, "%d", &val)) ? val : 0);
}

char *getqualstring(arg)
struct dsc$descriptor_s *arg;
{
    char *string, *p;
    struct dsc out;
    short rlen;

    p = string = malloc(256);	/* Get some memory */
    out.addr = string;		/* Setup pointer */
    out.len = 256;		/* and size */
    if ((cli$present(arg) & 1) &&	/* If present... */
	(cli$get_value(arg, &out, &rlen) & 1)) {	/* Get it's value */
	p += rlen;		/* Point to end of string */
	*p = '\0';		/* Place terminator */
	return (string);
    } else
	return (0);		/* Default */
}

/*
  Show connection between LD device and corresponding file
*/

void show()
{
    int stat;
    int loop = 0;
    char matchname[256];
    struct dsc matchdev =
    {sizeof(matchname), matchname};
    short rlen;
    int context[2] =
    {0, 0};

    if (getqual(&sw_trace) > 0)
	show_trace();		/* Special processing for trace */
    else if (getqual(&sw_watch) > 0)
	show_watch();		/* Special processing for watch */
    else {
	s_device = getqualstring(&sw_device);	/* Get devicename */

	if (getqual(&sw_all) > 0) {	/* /ALL ? */
	    stat = 1;
	    while (stat & 1) {	/* Loop until we're done */
		stat = sys$device_scan(&matchdev, &rlen, &wildname, 0, &context);
		if (stat == SS$_NOMOREDEV) {
		    if (loop) {
			if (loop == 1)
			    lib$signal(&ld_nounitsfound, 1, matchname);
			else
			    return;	/* No more, quit */
		    } else
			signal_error(SS$_NOSUCHDEV, 0);	/* No device at all */
		}
		loop++;
		signal_error(stat, 0);
		matchname[rlen] = '\0';	/* Place terminator */
		stat = show_one(matchname, 0);	/* Show this device */
		if (stat == SS$_DEVINACT) {	/* If not connected... */
		    lib$signal(&ld_notconnected, 1, matchname);	/* Flag it */
		    stat = 1;	/* and continue */
		}
	    }
	} else {		/* Only one specified device */
	    stat = show_one(fulldevspec(s_device), 1);	/* Show it */
	    if (stat == SS$_DEVINACT) {	/* If not connected... */
		lib$signal(&ld_notconnected, 1, fulldevspec(s_device));	/* Flag it */
		stat = &ld_notconnected;	/* Convert to better message */
		stat |= STS$M_INHIB_MSG;	/* Don't show it again */
	    }
	}
    }
}

/*
  Show one specific device
*/

int show_one(which, single)
char *which;
int single;
{
    int stat;
    short chan;
    struct iosb iosb;
    char ldfile[256];
    char lddev[64];
    struct fiddef fid;
    struct dsc sfilename =
    {strlen(which), which};

    if (get_unit(which, 1) == 0) {	/* Don't use unit 0 */
	if (single)
	    lib$signal(&ld_badunit, 1, which);	/* Show it to the world */
	else
	    return (1);
    }
    stat = sys$assign(&sfilename, &chan, 0, 0, 0);	/* Assign chan to LD device */
    if (stat == SS$_DEVALLOC) {	/* Allocated on remote node ? */
	lib$signal(&ld_remotealloc, 1, which);
    } else {
	signal_error(stat, 0);
	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,	/* Inquire stat */
	    	    lddev, sizeof(lddev), &fid, 0, 0, LDIO_GET_CONNECTION);
	signal_error(stat, iosb.status);
	stat = sys$dassgn(chan);	/* Deassign channel */
	signal_error(stat, 0);
	if (!iosb_flags.connected)	/* Check 'connected' flag from driver */
	    return (&ld_notconnected);
	lddev[iosb.size] = '\0';	/* Terminate device spec */
	if (iosb_flags.replaced) {
	    strcpy(ldfile,lddev);
	    strcat(ldfile, " (Replaced)");
	} else
	    create_filename(lddev,&fid,ldfile);
	if (iosb_flags.protect)
	    strcat(ldfile, " (Write protect)");
	if (iosb_flags.share)
	    strcat(ldfile, " (Shared)");
	lib$signal(&ld_connected, 2, which, ldfile);	/* Show it to the world */
    }
    return (1);
}

/*
  Connect LD device and file together
*/

void connect_unit()
{
    struct dsc sdev;
    int stat;
    short chan;
    int replacefl;
    int shflag;
    int unit;
    int q_log;
    int func;
    int tracks;
    int sectors;
    int cylinders;   
    int alloclass;
    int tmp;
    struct iosb iosb;
    $DESCRIPTOR(symbol, "LD_UNIT");
    char tmpstr[10];
    char *tmpptr;

    s_file = getqualstring(&sw_file);	/* Get filename */
    s_device = getqualstring(&sw_device);	/* Get LD device name */
    q_log = getqual(&sw_log);	/* Get /LOG */

    if ((alloclass = getqualvalue(&sw_alloclass)) > 0) {	/* /ALLOCLASS ? */
	if (alloclass > 255)
	    lib$signal(&ld_badalloclass);	/* Bad allocation class */
    }
    replacefl = getqual(&sw_replace);
    if (!replacefl) {
	open_file(s_file);	/* Attempt to open */
	tracks = getqualvalue(&sw_tracks);
	sectors = getqualvalue(&sw_sectors);
	cylinders = getqualvalue(&sw_cylinders);
	if ((tmp = getqualvalue(&sw_maxblocks)) > 0)
	    maxblocks = tmp;	/* Set new value if specified */
	else {
/* No maxblocks specified, check if geometry specified. If true set maxblocks
   according to that */
	    if ((tracks != 0) || (sectors != 0) || (cylinders != 0))
		maxblocks = ((tracks == 0) ? 1 : tracks) *
			    ((sectors == 0) ? 1 : sectors) *
			    ((cylinders == 0) ? 1 : cylinders);
	}
    } else {
	realdev_dsc.addr = fulldevspec(s_file);	/* Set real devicename */
	realdev_dsc.len = strlen(realdev_dsc.addr);
    }
    shflag = 0;
    sdev.addr = "LDA0:";
    sdev.len = 5;
    if (!s_device) {		/* No device specified */
	s_device = malloc(256);
	stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
	signal_error(stat, 0);
        if (alloclass > 0) {
	    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,/* Init LD dev. */
		    alloclass, 0, 0, 0, 0, LDIO_SET_ALLOCLASS);
	    signal_error(stat, iosb.status);
        }
	getdevnam(chan, s_device);
	shflag++;
    } else {
	unit = get_unit(s_device, 0);
	if ((unit == 0) || (unit > 9999))		/* Check range */
	    lib$signal(&ld_badunit, 1, s_device);
	set_seed(unit,s_device);
        strcpy(tmpstr,"LDA0:");
	tmpptr = s_device+2;
        tmpstr[2] = *tmpptr;
        sdev.addr = tmpstr;
	stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
	signal_error(stat, 0);
        if (alloclass > 0) {
	    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,/* Init LD dev. */
		    alloclass, 0, 0, 0, 0, LDIO_SET_ALLOCLASS);
	    signal_error(stat, iosb.status);
        }
	getdevnam(chan, s_device);
	if (get_unit(s_device, 1) != unit)
	    lib$signal(&ld_dupunit);	/* Duplicate unit */
    }
    if (getqual(&sw_share) > 0)		/* /SHARE ? */
	func = LDIO_CONNECT | LDIO_M_SHARE;
    else
	func = LDIO_CONNECT;			/* Normal function */
    if (!replacefl)		/* Only if not replacing drive */
	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,	/* Init LD dev. */
		    &sbk, maxblocks, tracks, sectors, cylinders, func);
    else
	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,	/* Init LD dev. */
		    &realdev_dsc, 0, 0, 0, 0, func | LDIO_M_REPLACE);
    signal_error(stat, iosb.status);
    stat = sys$dassgn(chan);	/* Deassign channel */
    signal_error(stat, 0);
    if (!replacefl)		/* Only if not replacing drive */
	close_file(s_file);
    if (getqual(&sw_symbol)) {
	sprintf(tmpstr,"%d",get_unit(s_device, 1)); /* Get unit number */
	sdev.addr = tmpstr;
	sdev.len = strlen(tmpstr);
	stat = lib$set_symbol(&symbol, &sdev, 0); /* Create local symbol */
	signal_error(stat, 0);
    }
    if (q_log > 0) {		/* /LOG ? */
	stat = show_one(fulldevspec(s_device), 1);	/* Show this one's now connected */
	signal_error(stat, 0);
    }
    else {
	if (shflag)
	    lib$signal(&ld_unit, 1, fulldevspec(s_device)); /* Show it to the world */
    }
}

void getdevnam(chan, s_device)
short chan;
char *s_device;
{
    int stat;
    short rlen;
    struct dsc dev =
    {256, s_device};

    stat = lib$getdvi(&DVI$_ALLDEVNAM, &chan, 0, 0, &dev, &rlen);
    signal_error(stat, 0);
    s_device[rlen] = '\0';	/* Terminator */
}

/* Get unitnumber from unit which may not exist */

int get_unit(str, exist)
char *str;
int exist;
{
    char *p, prev;
    int num, stat;
    short rlen;
    struct dsc dev =
    {strlen(str), str};

    if (exist) {		/* If it's there use getdvi */
	stat = lib$getdvi(&DVI$_UNIT, 0, &dev, &num, 0, 0);
	signal_error(stat, 0);
	return (num);
    } else {			/* If it's not there find it ourselves */
	p = str;
	prev = '\0';
	while (*p) {
	    if (isdigit(*p)) {
		if (prev == '$') {
		    while (isdigit(*p))
			p++;
		} else
		    return (atoi(p));	/* Return unitnumber */
	    }
	    prev = *p++;
	}
	lib$signal(&ld_baddevsyntax, 1, str);	/* Bad syntax */
    }
}

void set_seed(num,device)
int num;
char *device;
{
    struct dsc sdev;
    int stat;
    short chan;
    struct iosb iosb;
    char *tmpstr = "LDA0:";
    char *tmpptr;

    if (strncmp(device,"LD",2) ||
        ((device[2] < 'A') || (device[2] > 'Z')))
        lib$signal(&ld_baddevsyntax, 1, device);	/* Bad syntax */
    tmpptr = device+2;
    tmpstr[2] = *tmpptr;			/* Insert controller letter */
    sdev.addr = tmpstr;
    sdev.len = 5;
    stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
    signal_error(stat, 0);
    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,	/* Init LD dev. */
		    num - 1, 0, 0, 0, 0, LDIO_SET_SEED);	/* minus one so next one will be correct */
    signal_error(stat, iosb.status);
    stat = sys$dassgn(chan);	/* Deassign channel */
    signal_error(stat, 0);
}

/*
  Enable trace
*/

void trace()
{
    struct dsc sdev;
    int stat;
    short chan;
    int numbuf;
    int func;
    struct iosb iosb;

    if (getqual(&sw_stop) > 0)
	stop_trace();
    else {
	s_device = getqualstring(&sw_device);	/* Get LD device name */
	if (get_unit(s_device, 1) == 0)	/* Don't use unit 0 */
	    lib$signal(&ld_badunit, 1, fulldevspec(s_device));	/* Show it to the world */
	sdev.addr = s_device;
	sdev.len = strlen(s_device);
	if ((numbuf = getqualvalue(&sw_size)) == 0)	/* Get number of tracebuffers */
	    numbuf = 512;	/* Default */
	func = LDIO_ENABLE_TRACE;	/* Normal function */
	if (getqual(&sw_reset) > 0)	/* /RESET ? */
	    func = LDIO_RESET_TRACE;
	stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
	signal_error(stat, 0);
	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			numbuf, 0, 0, 0, 0, func);	/* Enable trace */
	signal_error(stat, iosb.status);
	stat = sys$dassgn(chan);/* Deassign channel */
	signal_error(stat, 0);
    }
}

/*
  Disable trace
*/

void notrace()
{
    struct dsc sdev;
    int stat;
    short chan;
    struct iosb iosb;

    s_device = getqualstring(&sw_device);	/* Get LD device name */
    if (get_unit(s_device, 1) == 0)	/* Don't use unit 0 */
	lib$signal(&ld_badunit, 1, fulldevspec(s_device));	/* Show it to the world */
    sdev.addr = s_device;
    sdev.len = strlen(s_device);
    stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
    signal_error(stat, 0);
    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
		    0, 0, 0, 0, 0, LDIO_DISABLE_TRACE);	/* Disable trace */
    signal_error(stat, iosb.status);
    stat = sys$dassgn(chan);	/* Deassign channel */
    signal_error(stat, 0);
}

/*
  Enable write protect
*/

void protect()
{
    struct dsc sdev;
    int stat;
    short chan;
    struct iosb iosb;

    s_device = getqualstring(&sw_device);	/* Get LD device name */
    if (get_unit(s_device, 1) == 0)	/* Don't use unit 0 */
	lib$signal(&ld_badunit, 1, fulldevspec(s_device));	/* Show it to the world */
    sdev.addr = s_device;
    sdev.len = strlen(s_device);
    stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
    signal_error(stat, 0);
    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
		    0, 0, 0, 0, 0, LDIO_ENABLE_PROTECT);
    signal_error(stat, iosb.status);
    stat = sys$dassgn(chan);	/* Deassign channel */
    signal_error(stat, 0);
}

/*
  Disable write protect
*/

void noprotect()
{
    struct dsc sdev;
    int stat;
    short chan;
    struct iosb iosb;

    s_device = getqualstring(&sw_device);	/* Get LD device name */
    if (get_unit(s_device, 1) == 0)	/* Don't use unit 0 */
	lib$signal(&ld_badunit, 1, fulldevspec(s_device));	/* Show it to the world */
    sdev.addr = s_device;
    sdev.len = strlen(s_device);
    stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
    signal_error(stat, 0);
    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
		    0, 0, 0, 0, 0, LDIO_DISABLE_PROTECT);
    signal_error(stat, iosb.status);
    stat = sys$dassgn(chan);	/* Deassign channel */
    signal_error(stat, 0);
}

/*
  Enable watch
*/

void watch()
{
    struct dsc sdev;
    int stat;
    short chan;
    int func;
    int cnt, i, size;
    struct iosb iosb;
    struct watchpt *wpt, *wpt1, wptindex;
    int *blkpnt, wptcnt;
    int function_read, function_write, function_code, function_all,
     action_suspend, action_crash, action_error, action_opcom;
    int function_code_val, action_error_val;
    int resume, index, index_val, file;
    char *filename;

/* Get switches */

    resume = getqual(&sw_resume);
    index = getqual(&sw_index);
    function_read = getqual(&sw_function_read);
    function_write = getqual(&sw_function_write);
    function_code = getqual(&sw_function_code);
    function_all = getqual(&sw_function_all);
    action_suspend = getqual(&sw_action_suspend);
    action_crash = getqual(&sw_action_crash);
    action_opcom = getqual(&sw_action_opcom);
    action_error = getqual(&sw_action_error);
    file = getqual(&sw_file);

    action_error_val = SS$_BUGCHECK;	/* Default return */
    if (action_error) {
	action_error_val = getqualvalue(&sw_action_error);
	if (action_error_val == 0)
	    action_error_val = SS$_BUGCHECK;
    }
    if (function_code)
	function_code_val = getqualvalue(&sw_function_code);

    s_device = getqualstring(&sw_device);	/* Get LD device name */
    if (get_unit(s_device, 1) == 0)	/* Don't use unit 0 */
	lib$signal(&ld_badunit, 1, fulldevspec(s_device));	/* Show it to the world */
    sdev.addr = s_device;
    sdev.len = strlen(s_device);
    stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
    signal_error(stat, 0);

    if (file) {
	filename = getqualstring(&sw_file);
	open_file(filename);	/* Open file */
    }

    if (index) {		/* /INDEX? (for WATCH/RESUME) */
	index_val = getqualvalue(&sw_index);
	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			0, 0, 0, 0, 0, LDIO_GET_WATCH | LDIO_M_INQUIRE);	/* Get watch size */
	signal_error(stat, iosb.status);
	wptcnt = iosb_lw;	/* Number of watchpoints */
	if ((index_val < 1) || (index_val > wptcnt))
	    lib$signal(&ld_wptnotfound);	/* Watchpoint not found */
	if (wptcnt) {
	    size = wptcnt * sizeof(struct watchpt);
	    if (!(wpt = malloc(size)))
		signal_error(SS$_INSFMEM, 0);
	    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			    wpt, size, 0, 0, 0, LDIO_GET_WATCH);	/* Get watch data */
	    signal_error(stat, iosb.status);
	    wpt += index_val - 1;	/* Point to correct data */
	    wptindex.lbn = wpt->lbn;	/* Copy block */
	    wptindex.action = wpt->action;
	    wptindex.func = wpt->func;
	    wptindex.retcode = wpt->retcode;
	    wptindex.flags = wpt->flags;
	    wpt = &wptindex;
	    cnt = 1;		/* One entry */
	}
    } else {
	blkpnt = getlist(&sw_lblock, &cnt);	/* Get LBN's to handle */
	if (cnt) {
	    if (!(wpt = malloc(cnt * sizeof(struct watchpt))))
		 signal_error(SS$_INSFMEM, 0);
	    for (i = 0, wpt1 = wpt; i < cnt; i++, wpt1++) {
		wpt1->lbn = *blkpnt++;	/* Fill lbn */
		if (function_all)
		    wpt1->func = 0xffff;
		else if (function_read)
		    wpt1->func = IO$_READPBLK;
		else if (function_write)
		    wpt1->func = IO$_WRITEPBLK;
		else if (function_code) {	/* User specified function */
		    wpt1->func = function_code_val;
		    switch (function_code_val & IO$M_FCODE) {	/* Function without modifiers */
		    case IO$_READPBLK:
		    case IO$_WRITEPBLK:
		    case IO$_WRITECHECK:
		    case IO$_DSE:
			wpt1->flags = 0;		/* Function with transfer */
			break;
		    default:
			if (file)
				lib$signal(&ld_noreadwrite, 0);
			wpt1->flags = FLAGS_NOLBN;	/* Special case */
			wpt1->lbn = 0;			/* Zap lbn */
			break;				/* For functions without a lbn */
		    }
		}
		if (file) {
			wpt1->flags = FLAGS_FILE;	/* Signal 'file' Function */
			wpt1->sbk = &sbk;		/* Point to sbk */
			if (wpt1->lbn == 0)		/* VBN starts at 1 */
				lib$signal(&ld_vbnerror, 0);
		}
		if (action_suspend)	/* Set action */
		    wpt1->action = WATCH_ACTION_SUSPEND;
		else if (action_crash)
		    wpt1->action = WATCH_ACTION_CRASH;
		else if (action_opcom)
		    wpt1->action = WATCH_ACTION_OPCOM;
		else if (action_error) {
		    wpt1->action = WATCH_ACTION_ERROR;
		    wpt1->retcode = action_error_val;	/* Error to return */
		} else
		    wpt1->retcode = 0;
	    }
	}
    }
    if (resume)
	func = LDIO_RESUME_WATCH;	/* Resume suspended thread */
    else
	func = LDIO_ENABLE_WATCH;	/* Enable watchpoint */
    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
		    wpt, cnt, 0, 0, 0, func);
    signal_error(stat, iosb.status);
    stat = sys$dassgn(chan);	/* Deassign channel */
    signal_error(stat, 0);
    if (file)
	close_file(filename);	/* Close file */
}

/*
  Disable watch
*/

void nowatch()
{
    struct dsc sdev;
    int stat;
    int chan;
    struct watchpt *wpt, *wpt1, wptindex;
    struct iosb iosb;
    int cnt;
    int i, size;
    int *blkpnt, wptcnt;
    int index, index_val;

    s_device = getqualstring(&sw_device);	/* Get LD device name */
    if (get_unit(s_device, 1) == 0)	/* Don't use unit 0 */
	lib$signal(&ld_badunit, 1, fulldevspec(s_device));	/* Show it to the world */
    sdev.addr = s_device;
    sdev.len = strlen(s_device);
    stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
    signal_error(stat, 0);
    index = getqual(&sw_index);
    if (index) {		/* /INDEX? */
	index_val = getqualvalue(&sw_index);
	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			0, 0, 0, 0, 0, LDIO_GET_WATCH | LDIO_M_INQUIRE);	/* Get watch size */
	signal_error(stat, iosb.status);
	wptcnt = iosb_lw;
	if ((index_val < 1) || (index_val > wptcnt))
	    lib$signal(&ld_wptnotfound);	/* Watchpoint not found */
	if (wptcnt) {
	    size = wptcnt * sizeof(struct watchpt);
	    if (!(wpt = malloc(size)))
		signal_error(SS$_INSFMEM, 0);
	    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			    wpt, size, 0, 0, 0, LDIO_GET_WATCH);	/* Get watch data */
	    signal_error(stat, iosb.status);
	    wpt += index_val - 1;	/* Point to correct data */
	    wptindex.lbn = wpt->lbn;	/* Copy block */
	    wptindex.action = wpt->action;
	    wptindex.func = wpt->func;
	    wptindex.retcode = wpt->retcode;
	    wptindex.flags = wpt->flags;
	    wpt = &wptindex;
	    cnt = 1;		/* One entry */
	}
    } else {
	blkpnt = getlist(&sw_lblock, &cnt);	/* Get lbn list */
	if (cnt) {
	    if (!(wpt = malloc(cnt * sizeof(struct watchpt))))
		 signal_error(SS$_INSFMEM, 0);
	    for (i = 0, wpt1 = wpt; i < cnt; i++, wpt1++) {
		wpt1->flags = FLAGS_REMOVE_ALL;	/* Flag all of this lbn */
		wpt1->lbn = *blkpnt++;	/* Fill lbn to kill */
	    }
	}
    }
    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
		    wpt, cnt, 0, 0, 0, LDIO_DISABLE_WATCH);	/* Disable watch */
    signal_error(stat, iosb.status);
    stat = sys$dassgn(chan);	/* Deassign channel */
    signal_error(stat, 0);
}

/*
  Show watch info
*/

void show_watch()
{
    struct dsc sdev;
    int stat;
    int chan;
    struct watchpt *wpt;
    struct suspend_list *slist;
    struct iosb iosb;
    int wptcnt, blkcnt, slistcnt, size;
    int *blkpnt;

    s_device = getqualstring(&sw_device);	/* Get LD device name */
    if (get_unit(s_device, 1) == 0)	/* Don't use unit 0 */
	lib$signal(&ld_badunit, 1, fulldevspec(s_device));	/* Show it to the world */
    sdev.addr = s_device;
    sdev.len = strlen(s_device);
    blkpnt = getlist(&sw_lblock, &blkcnt);
    stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
    signal_error(stat, 0);
    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
		    0, 0, 0, 0, 0, LDIO_GET_WATCH | LDIO_M_INQUIRE);	/* Get watch size */
    signal_error(stat, iosb.status);
    if (wptcnt = iosb_lw) {	/* Something to do ? */
	size = wptcnt * sizeof(struct watchpt);
	if (!(wpt = malloc(size)))
	    signal_error(SS$_INSFMEM, 0);
	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			wpt, size, 0, 0, 0, LDIO_GET_WATCH);	/* Get watch data */
	signal_error(stat, iosb.status);
    }
    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
		  0, 0, 0, 0, 0, LDIO_GET_SUSPEND_LIST | LDIO_M_INQUIRE);	/* Get suspend list size */
    signal_error(stat, iosb.status);
    if (slistcnt = iosb_lw) {	/* Something suspended ? */
	size = slistcnt * sizeof(struct suspend_list);
	if (!(slist = malloc(size)))
	    signal_error(SS$_INSFMEM, 0);
	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			slist, size, 0, 0, 0, LDIO_GET_SUSPEND_LIST);	/* Get suspend list */
	signal_error(stat, iosb.status);
    }
    stat = sys$dassgn(chan);	/* Deassign channel */
    signal_error(stat, 0);
    display_watch(wpt, wptcnt, blkpnt, blkcnt, slist, slistcnt);	/* Show it */
}

void display_watch(wpt, wptcnt, blkpnt, blkcnt, slist, slistcnt)
struct watchpt *wpt;
int wptcnt;
int blkpnt[];
int blkcnt;
struct suspend_list *slist;
int slistcnt;
{
    int i, j;
    int index, index_val;
    int *bpt;
    int found, printed;
    struct suspend_list *spt;
    char albn[9];
    char filename[256];

    if (index = getqual(&sw_index))	/* /INDEX? */
	index_val = getqualvalue(&sw_index);	/* get value */
    if (wptcnt) {		/* Something to do ? */
	printed = 0;
	for (i = 0; i < wptcnt; i++, wpt++) {	/* For all watchpoints */
	    found = 0;
	    if (blkcnt) {	/* Check if in list the user specified */
		for (j = 0, bpt = blkpnt; j < blkcnt; j++, bpt++)
		    if (*bpt == wpt->lbn) {
			found++;
			break;
		    }
	    } else {
		if (index) {	/* Or check by index */
		    if (index_val == i + 1)
			found++;
		} else
		    found++;
	    }
	    if (found) {
		if (!printed) {	/* Print header only once */
		    printf("Index LBN     Action     Function         Error return code\n");
		    printf("--------------------------------------------------------------------\n");
		    printed = 1;
		}
		if (wpt->flags & FLAGS_FILE) {
			create_filename(s_device,&wpt->fid,filename);
			printf("             %s:\n",filename);
		}
		if (wpt->flags & FLAGS_NOLBN)
		    sprintf(albn, "%8s", "");	/* Blank for watchpoint without lbn */
		else
		    sprintf(albn, "%8d", wpt->lbn);
		switch (wpt->action) {
		case WATCH_ACTION_SUSPEND:
		    printf("%2d %s  Suspend   %-18s\n", i + 1, albn,
			   decode_func(wpt->func));
		    for (j = 0, spt = slist; j < slistcnt; j++, spt++)
			if ((spt->lbn == wpt->lbn) &&
			    (spt->func == wpt->func))
			    printf("             Suspended process: %08.8X\n", spt->pid);
		    break;
		case WATCH_ACTION_ERROR:
		    printf("%2d %s  Error     %-18s   %04.4X (%s)\n",
			   i + 1, albn, decode_func(wpt->func),
			   wpt->retcode, ssdef(wpt->retcode));
		    break;
		case WATCH_ACTION_CRASH:
		    printf("%2d %s  Crash     %-18s\n", i + 1, albn,
			   decode_func(wpt->func));
		    break;
		case WATCH_ACTION_OPCOM:
		    printf("%2d %s  Opcom     %-18s\n", i + 1, albn,
			   decode_func(wpt->func));
		    break;
		default:
		    break;
		}
	    }
	}
	if (!printed)
	    lib$signal(&ld_wptnotfound);	/* Watchpoint not found */
    } else
	lib$signal(&ld_nowatchdata);	/* No watch data available */
}

/*
 Get a list of lbn's from the commandline
*/

int *getlist(sw, cnt)
char *sw;
int *cnt;
{
    int more;
    int *blklist, *blkpnt;

    if (!(blklist = malloc(sizeof(int))))	/* get mem for 1 entry */
	 signal_error(SS$_INSFMEM, 0);	/* Not enough memory */
    *cnt = 0;
    more = -1;
    blkpnt = blklist;
    for (;;) {
	*blkpnt = getmulqualvalue(sw, &more);	/* Get entry */
	if (more < 0)		/* Not specified ? */
	    return (blklist);
	*cnt += 1;
	if (more) {		/* more coming up? */
	    /* Get new block to accomodate size */
	    if (!(blklist = realloc(blklist, (*cnt + 1) * sizeof(int))))
		 signal_error(SS$_INSFMEM, 0);	/* Not enough memory */
	    blkpnt = blklist + *cnt;
	} else
	    break;
    }
    return blklist;
}

/*
  Stop running trace
*/

void stop_trace()
{
    int stat;
    int loop = 0;
    char matchname[256];
    struct dsc matchdev =
    {sizeof(matchname), matchname};
    short rlen;
    int context[2] =
    {0, 0};

    s_device = getqualstring(&sw_device);	/* Get devicename */

    if (getqual(&sw_all) > 0) {	/* /ALL ? */
	stat = 1;
	while (stat & 1) {	/* Loop until we're done */
	    stat = sys$device_scan(&matchdev, &rlen, &wildname, 0, &context);
	    if (stat == SS$_NOMOREDEV) {
		if (loop) {
		    if (loop == 1)
			lib$signal(&ld_nounitsfound, 1, matchname);
		    else
			return;	/* No more, quit */
		} else
		    signal_error(SS$_NOSUCHDEV, 0);	/* No device at all */
	    }
	    loop++;
	    signal_error(stat, 0);
	    matchname[rlen] = '\0';	/* Place terminator */
	    stop_one(matchname, 0);	/* Stop trace for this device */
	}
    } else			/* Only one specified device */
	stop_one(fulldevspec(s_device), 1);	/* Stop it */
}

/*
  Stop trace for one specific device
*/

void stop_one(which, single)
char *which;
int single;
{
    struct dsc lknam;
    int stat;
    char *qname;
    struct iosb iosb;
    int lockid;
    int retlen;
    int count;
    int i;
    int found;
    int grp, mem;
    union prvdef oldpriv;
    struct dsc sfilename =
    {strlen(which), which};
    struct lkidef locks[10];
    struct lkidef *p;
    struct itmlst item[] =
    {sizeof(locks), LKI$_LOCKS, &locks, &retlen,
     0, 0, 0, 0};
    struct itmlst jpi_item[] =
    {8, JPI$_CURPRIV, &oldpriv, 0,
     4, JPI$_GRP, &grp, 0,
     4, JPI$_MEM, &mem, 0,
     0, 0, 0, 0};

    if (get_unit(which, 1) == 0) {	/* Don't use unit 0 */
	if (single)
	    lib$signal(&ld_badunit, 1, which);	/* Show it to the world */
	else
	    return;
    }
    lknam.addr = build_lock(which);	/* Build resource name */
    lknam.len = strlen(lknam.addr);
    stat = sys$enqw(0, LCK$K_NLMODE, &ld_lksb, LCK$M_SYSTEM | LCK$M_VALBLK,
		    &lknam, 0, 0, 0, 0, 0, 0);	/* Enqueue null lock */
    signal_error(stat, 0);
    stat = sys$getlkiw(0, &ld_lksb.lockid, &item, &iosb, 0, 0, 0);	/* Get info of other locks on this resource */
    signal_error(stat, iosb.status);
    count = (retlen & 0xffff) / ((retlen >> 16) & 0x7fff);	/* Lock count */
    p = locks;
    found = 0;
    for (i = 0; i < count; i++, p++) {
	if (p->lki$l_lkid == ld_lksb.lockid)	/* Disregard our own lock */
	    continue;
	found = 1;
	stat = sys$getjpiw(0, 0, 0, &jpi_item, &iosb, 0, 0);	/* Get privs+ group + member */
	signal_error(stat, iosb.status);
	if (!((grp == ld_lksb.valblk[0]) &&	/* Group the same */
	      (mem == ld_lksb.valblk[1]))) {	/* as well as the member ? */
	    if ((grp != ld_lksb.valblk[0]) &&	/* Group the same ? */
		(mem != ld_lksb.valblk[1])) {	/* Member the same ? */
		if (!oldpriv.prv$v_world)	/* We need WORLD */
		    lib$signal(&ld_noworldpriv, 1, which);
	    } else if (grp == ld_lksb.valblk[0]) {	/* Group the same ? */
		if (!oldpriv.prv$v_group)	/* We need GROUP */
		    lib$signal(&ld_nogrouppriv, 1, which);
	    }
	}
	stat = sys$enqw(0, LCK$K_EXMODE, &ld_lksb,	/* Convert to fire blocking ast */
			LCK$M_SYSTEM | LCK$M_CONVERT | LCK$M_VALBLK,
			0, 0, 0, 0, 0, 0, 0);
	signal_error(stat, 0);
	stat = sys$deq(ld_lksb.lockid, 0, 0, 0);	/* Get rid of our lock */
	signal_error(stat, 0);
	break;
    }
    if (!found)
	lib$signal(&ld_conttracenotact, 1, fulldevspec(which));
}

/*
  Show trace info
*/

void show_trace()
{
    struct dsc sdev;
    struct dsc lknam;
    int stat;
    int chan;
    int trcsize;
    int trcnum;
    int contin;
    int qstat;
    int func;
    int inited;
    int overrun;
    struct iosb iosb;
    char *trcbuf;
    struct itmlst item[] =
    {4, JPI$_GRP, &ld_lksb.valblk[0], 0,
     4, JPI$_MEM, &ld_lksb.valblk[1], 0,
     0, 0, 0, 0};

    if (getqual(&sw_input) > 0)
	process_input();	/* Process input file */
    else {
	s_device = getqualstring(&sw_device);	/* Get devicename */
	if (!s_device)
	    lib$signal(&ld_confqual);	/* Conflicting qualifiers */
	s_device = fulldevspec(s_device);	/* Get full devicespec */
	if (get_unit(s_device, 1) == 0)	/* Don't use unit 0 */
	    lib$signal(&ld_badunit, 1, s_device);	/* Show it to the world */
	sdev.addr = s_device;
	sdev.len = strlen(s_device);
	qstat = getqual(&sw_status);
	stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* Assign channel */
	signal_error(stat, 0);

/* Get number of tracebuffers */

	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			0, 0, 0, 0, 0, LDIO_GET_TRACE | LDIO_M_INQUIRE);
	signal_error(stat, iosb.status);
	trcnum = iosb_lw;	/* Size of tracebuffer (in entries) */
	trcsize = trcnum * sizeof(struct trace_ent);
	trcbuf = malloc(trcsize);	/* Get memory for trace buffer */
	if (trcbuf == 0)
	    signal_error(SS$_INSFMEM, 0);	/* Not enough memory */
	func = LDIO_GET_TRACE | LDIO_M_NOWAIT;	/* Normal function */
	if (getqual(&sw_reset) > 0)	/* /RESET ? */
	    func |= LDIO_M_RESET;
	if (contin = (getqual(&sw_continuous) > 0)) {	/* /CONTINUOUS ? */
	    func = LDIO_GET_TRACE | LDIO_M_RESET;
	    set_outband();	/* Set out-of-band escape */
	    lknam.addr = build_lock(s_device);	/* Build lock resource name */
	    lknam.len = strlen(lknam.addr);
	    stat = sys$getjpiw(0, 0, 0, &item, &iosb, 0, 0);	/* Get group + member */
	    signal_error(stat, iosb.status);
	    stat = sys$enqw(0, LCK$K_PWMODE, &ld_lksb,
			    LCK$M_SYSTEM | LCK$M_NOQUEUE,
			    &lknam, 0, 0, 0, 0, 0, 0);
	    if (stat == SS$_NOTQUEUED)
		lib$signal(&ld_conttraceact, 1, fulldevspec(s_device));
	    signal_error(stat, 0);
	    stat = sys$enqw(0, LCK$K_PRMODE, &ld_lksb,	/* Write lock value block */
			    LCK$M_SYSTEM | LCK$M_CONVERT | LCK$M_VALBLK,
			    0, 0, 0, 0, 0, 0, 0);
	    signal_error(stat, 0);
	    stat = sys$enqw(0, LCK$K_PWMODE, &ld_lksb,	/* Convert up again */
			    LCK$M_SYSTEM | LCK$M_CONVERT | LCK$M_VALBLK,
			    0, 0, 0, 0, &ld_exit, 0, 0);
	    signal_error(stat, 0);
	}
	inited = (contin > 0) ? 0 : -1;
	do {
	    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
			    trcbuf, trcsize, 0, 0, 0, func);	/* Get trace info */
	    signal_error(stat, 0);
	    stat = iosb.status;
	    if (stat == SS$_DATAOVERUN) {
		overrun = iosb_lw;	/* Save number of overruns */
		iosb_lw = trcnum;
	    } else {
		overrun = 0;
		signal_error(stat, 0);	/* Test for other error than DATAOVERRUN */
	    }
	    if (qstat) {
		lib$signal(&ld_status, 3, fulldevspec(s_device),
			   trcnum, iosb_lw);	/* Show it to the world */
		break;
	    } else {
		if (iosb_lw)	/* Anything there ? */
		    process_trace(trcbuf, iosb_lw, 0, overrun,	/* Process the contents */
				  nodename(), cvttime(0, 0, 0), inited);
		else
		    lib$signal(&ld_notrcdata);	/* No trace data available */
	    }
	    inited = 1;
	}
	while (contin);
	stat = sys$dassgn(chan);/* Deassign channel */
	signal_error(stat, 0);
	free(trcbuf);		/* Get rid of buffer */
    }
}

/*
  Process the trace buffer
*/

void process_trace(buf, size, ovrbuf, overrun, nname, when, inited)
char *buf;
int size;
struct ovrrun *ovrbuf;
int overrun;
char *nname;
char *when;
int inited;
{
    static int q_bin, q_entries;
    static int q_blocks, q_version_limit, q_vsw;
    static int v_end1, v_end2;
    static int re_open = 0;
    static int written = 0;
    struct trace_ent *p;
    static int outflg;
    int i;
    int version, recnum;
    int end1, end2;
    int entr, numpack;

    if ((inited <= 0) || (re_open <= 0)) {
	if (re_open == 0) {
	    q_entries = getqual(&sw_entries);	/* /ENTRIES ? */
	    if (q_entries > 0) {
		v_end1 = getqualvalue(&sw_entries);	/* Get #of entries to show */
		v_end2 = getqualvalue(&sw_entries);	/* Get second part (if any) */
	    }
	    q_bin = getqual(&sw_binary);	/* /BINARY ? */
	    q_blocks = getqual(&sw_blocks);	/* /BLOCKS ? */
	    if (q_blocks)
		q_blocks = getqualvalue(&sw_blocks);	/* Get value */
	    q_vsw = getqual(&sw_version_limit);	/* /VERSION_LIMIT ? */
	    if (q_vsw) {
		q_version_limit = getqualvalue(&sw_version_limit);	/* Get value */
		if (q_version_limit >= 32767)
		    q_version_limit = 32767;
		if (q_version_limit <= 0)
		    q_version_limit = 1;
		q_version_limit--;
	    }
	    outflg = getqual(&sw_output);	/* /OUTPUT ? */
	    if (outflg > 0)
		outfilename = getqualstring(&sw_output);	/* Get filename */
	}
	if (outflg > 0) {
	    if (q_bin)
		outfilename = fullfilespec(outfilename, "LD_TRACE.DAT");	/* Provide default */
	    else
		outfilename = fullfilespec(outfilename, "LD_TRACE.LOG");	/* Provide default */
	    if (re_open < 0) {	/* New open, close old file */
		if (outfile >= 0) {	/* Closing outfile closes outfilei as well */
		    if (fclose(outfile) < 0)
			lib$signal(&ld_closerr, 1, outfilename, vaxc$errno, 0);
		}
		if (q_vsw)
		    purge_file(q_version_limit, outfilename);
	    }
/*
   We want to create a normal file with some nice RMS attributes. Since
   this can't be done with fopen() we will create the file first, and use
   fdopen to setup the correct file pointers. This takes also care of the
   substitution of the defaults filename parts.
*/
	    if (q_bin)
		outfilei = creat(outfilename, 0, "rfm=var", "ctx=bin");
	    else
		outfilei = creat(outfilename, 0, "rfm=var", "rat=cr");
	    if ((outfilei < 0) ||
		((outfile = fdopen(outfilei, "a+")) == NULL))
		lib$signal(&ld_outfilerr, 1, outfilename, vaxc$errno, 0);	/* Can't open output file */
	    re_open = 1;
	} else if (outflg < 0)	/* /NOOUTPUT ? */
	    return;
	else
	    outfile = stdout;	/* Default output */
    }
    end2 = 0;
    if (q_entries > 0) {	/* /ENTRIES ? */
	end1 = v_end1;		/* Get #of entries to show */
	end2 = v_end2;		/* Get second part (if any) */
	if (end2 != 0) {	/* There's a second part */
	    if ((end1 <= 0) ||
		(end2 <= 0) ||
		(end2 < end1))
		lib$signal(&ld_badentparam);	/* Bad /ENTRIES parameter */
	}
	if (end1 == 0)
	    return;		/* Nothing to show */
	if (end1 > 0) {
	    p = (struct trace_ent *) buf;	/* Start of buffer */
	    recnum = 1;		/* First one to display */
	    if (end2 > 0) {
		p += (end1 - 1);/* New start, first parameter */
		recnum = end1;	/* Adjust recordnumber */
	    }
	} else {
	    end1 = -end1;	/* Make absolute */
	    i = ((size - end1) < 0) ? 0 : (size - end1);
	    p = (struct trace_ent *) buf + i;
	    recnum = i + 1;	/* First one to display */
	}
    } else {
	p = (struct trace_ent *) buf;	/* Start of buffer */
	end1 = size;		/* Default = all */
	recnum = 1;		/* First one to display */
    }
    if (end1 > size) {		/* More than we've got? */
	if (end2 > 0)
	    lib$signal(&ld_pastdata);	/* Past end of data */
	end1 = end2 = size;	/* Enforce limit */
    }
    if (end2 > size)
	end2 = size;		/* And another limit */
    if (end2 > 0)
	numpack = end2 - end1 + 1;	/* Total number of packets to show */
    else
	numpack = end1;
    if (q_bin) {		/* Binary mode */
	version = DATA_FILE_VERSION << 24;
	if ((inited <= 0) || ((written == 0) && q_blocks)) {
	    if ((fwrite(&version, sizeof(int), 1, outfile) != 1) ||	/* Data file version */
		 (fwrite(fulldevspec(s_device), 32, 1, outfile) != 1) ||	/* Devicename */
		 (fwrite(nname, 32, 1, outfile) != 1) ||	/* Nodename */
		 (fwrite(when, 24, 1, outfile) != 1))	/* Timestamp */
		 lib$signal(&ld_filwrterr, 1, outfilename, vaxc$errno, 0);	/* Can't write output file */
	    if (q_blocks)
		written += sizeof(int) + 32 + 32 + 24 + (4 * 2);	/* Header size */
	}
	if ((fwrite(&numpack, sizeof(int), 1, outfile) != 1) ||	/* Number of entries */
	     (fwrite(&overrun, sizeof(int), 1, outfile) != 1) ||	/* Number of overruns */
	     (fwrite(p, sizeof(struct trace_ent), numpack, outfile) != numpack))	/* Data */
	     lib$signal(&ld_filwrterr, 1, outfilename, vaxc$errno, 0);	/* Can't write output file */
	if (q_blocks) {
	    written += (2 * (sizeof(int) + 2)) +
	     ((sizeof(struct trace_ent) + 2) * numpack);
	    if (written >= ((q_blocks - 1) * 512)) {
		re_open = -1;	/* Set flag for new file */
		written = 0;
	    }
	}
    } else
	show_trace_data(p, numpack, recnum, outfile, nname,
			s_device, when, inited, ovrbuf, overrun);
    if ((outfile != stdout) && (inited < 0)) {	/* Close output file */
	if (fclose(outfile) < 0)
	    lib$signal(&ld_closerr, 1, outfilename, vaxc$errno, 0);
    }
}

/*
  Format all tracedata
*/
void show_trace_data(p, numpack, from, outfile, node,
		     device, when, inited, ovrbuf, overrun)
struct trace_ent *p;
int numpack;
int from;
FILE *outfile;
char *node;
char *device;
char *when;
int inited;
struct ovrrun *ovrbuf;
int overrun;
{
    struct ovrrun *op;
    int i, j, stat, func, ovrcnt;
    static int pid, lbn, bytecount;
    static int iosb, function, timestamp;
    static int time_elapsed, time_absolute, time_combination, time_delta;
    static int function_hex, function_text;
    static int iosb_hex, iosb_longhex;
    static int iosb_text, iosb_combination;
    static int number, header, linenum;
    static int prev_tim[2], warnings;
    int restim[2];
    char record[256];
    char tmp[256];

/*
  Parse various qualifiers
*/

    if (inited <= 0) {
	pid = getqual(&sw_pid);
	lbn = getqual(&sw_lbn);
	bytecount = getqual(&sw_bytecount);
	timestamp = getqual(&sw_timestamp);
	time_elapsed = getqual(&sw_time_elapsed);
	time_absolute = getqual(&sw_time_absolute);
	time_combination = getqual(&sw_time_combination);
	time_delta = getqual(&sw_time_delta);
	function = getqual(&sw_function);
	function_hex = getqual(&sw_function_hex);
	function_text = getqual(&sw_function_text);
	iosb = getqual(&sw_iosb);
	iosb_hex = getqual(&sw_iosb_hex);
	iosb_longhex = getqual(&sw_iosb_longhex);
	iosb_text = getqual(&sw_iosb_text);
	iosb_combination = getqual(&sw_iosb_combination);
	number = getqual(&sw_number);
	header = getqual(&sw_header);
	warnings = getqual(&sw_warnings);
	linenum = from;
	prev_tim[0] = 0;
	prev_tim[1] = 0;
    }
    if ((header > 0) && (inited <= 0)) {	/* /HEADER ? */
	if (fprintf(outfile, "         I/O trace for device %s\n    %s",
		    device, when) <= 0)
	    lib$signal(&ld_filwrterr, 1, outfilename, vaxc$errno, 0);	/* Can't write output file */
	if (node)		/* Nodename available ? */
	    stat = fprintf(outfile, " on node %s\n\n", node);
	else
	    stat = fprintf(outfile, "\n\n");
	if (stat <= 0)
	    lib$signal(&ld_filwrterr, 1, outfilename, vaxc$errno, 0);	/* Can't write output file */
	*record = '\0';		/* Initially nothing */
	if (number > 0)
	    strcpy(record, "Entry ");
	if (timestamp > 0) {
	    if (time_elapsed)
		strcat(record, "Elaps ");
	    else if (time_absolute)
		strcat(record, "Start Time   End Time   ");
	    else if (time_combination)
		strcat(record, "Start Time  Elaps ");
	    else if (time_delta)
		strcat(record, " Delta Time Elaps ");
	}
	if (pid > 0)
	    strcat(record, "  Pid    ");
	if (lbn > 0)
	    strcat(record, "    Lbn    ");
	if (bytecount > 0)
	    strcat(record, " Bytes ");
	if (iosb > 0) {
	    if (iosb_hex)
		strcat(record, " Iosb[0] ");
	    else if (iosb_longhex)
		strcat(record, " Iosb[0]  Iosb[1] ");
	    else if (iosb_text)
		strcat(record, " Iosb   ");
	    else if (iosb_combination)
		strcat(record, " Iosb    Count ");
	}
	if (function > 0)
	    strcat(record, " Function ");
	if (!(*record))
	    return;		/* Nothing to display, quit */
	j = strlen(record);
	record[j - 1] = '\n';	/* Zap trailing space */
	record[j] = '\0';
	for (i = 0; i < j - 1; i++)	/* Form string of dashes */
	    tmp[i] = '-';
	tmp[i++] = '\n';	/* Trailing newline */
	tmp[i] = '\0';
	if ((fprintf(outfile, "%s", record) <= 0) ||
	    (fprintf(outfile, "%s", tmp) <= 0))
	    lib$signal(&ld_filwrterr, 1, outfilename, vaxc$errno, 0);	/* Can't write output file */
    }
    for (i = 0; i < numpack; i++) {	/* For all records to show */
	if ((overrun != 0) && (warnings > 0)) {	/* Signal lost data */
	    if (ovrbuf == 0) {	/* Called with number of overruns */
		ovrcnt = overrun;
		overrun = 0;	/* Prevent display for every record */
	    } else {
		op = ovrbuf;	/* Called after reading input file */
		ovrcnt = 0;	/* Check overrun records */
		for (j = 0; j < overrun; j++, op++)	/* For all overrunrecords */
		    if (op->recnum == linenum - 1) {
			ovrcnt = op->overrun;	/* Found match */
			break;
		    }
	    }
	    if (ovrcnt > 0)
		if (fprintf(outfile, "[%d trace record%slost]\n",
			    ovrcnt, ovrcnt == 1 ? " " : "s ") <= 0)
		    lib$signal(&ld_filwrterr, 1, outfilename, vaxc$errno, 0);	/* Can't write output file */
	}
	*record = '\0';
	func = p->func & IO$M_FCODE;	/* Strip function modifiers */
	if ((func == IO$_SETMODE) ||	/* These functions don't have a lbn */
	    (func == IO$_PACKACK) ||
	    (func == IO$_AVAILABLE) ||
	    (func == IO$_UNLOAD))
	    p->lbn = 0;		/* Clear for better readout */
	if (number > 0)		/* Line numbers */
	    sprintf(record, "%5d ", linenum);
	linenum++;		/* One more line */
	if (timestamp > 0) {	/* Time display */
	    if (time_elapsed) {
		stat = lib$sub_times(p->end_time, p->start_time, restim);
		signal_error(stat, 0);
		strcat(record, cvttime(&restim, 1, 1));
	    } else if (time_absolute) {
		strcat(record, cvttime(&p->start_time, 0, 1));
		strcat(record, " ");
		strcat(record, cvttime(&p->end_time, 0, 1));
	    } else if (time_combination) {
		stat = lib$sub_times(p->end_time, p->start_time, restim);
		signal_error(stat, 0);
		strcat(record, cvttime(&p->start_time, 0, 1));
		strcat(record, " ");
		strcat(record, cvttime(&restim, 1, 1));
	    } else if (time_delta) {
		if ((prev_tim[0] == 0) && (prev_tim[1] == 0)) {
		    strcat(record, " 0:00:00.00");
		} else {
		    stat = lib$sub_times(p->start_time, prev_tim, restim);
		    if (!(stat & 1)) {
			if (stat == LIB$_NEGTIM) {
			    stat = lib$sub_times(prev_tim, p->start_time, restim);
			    signal_error(stat, 0);
			    strcat(record, "-");
			} else
			    signal_error(stat, 0);
		    } else
			strcat(record, " ");
		    strcat(record, cvttime(&restim, 1, 0));
		}
		strcat(record, " ");
		stat = lib$sub_times(p->end_time, p->start_time, restim);
		signal_error(stat, 0);
		strcat(record, cvttime(&restim, 1, 1));
		prev_tim[0] = p->start_time[0];
		prev_tim[1] = p->start_time[1];
	    }
	    strcat(record, " ");
	}
	if (pid > 0) {		/* Pid */
	    sprintf(tmp, "%08X ", p->pid);
	    strcat(record, tmp);
	}
	if (lbn > 0) {		/* Logical block number */
	    sprintf(tmp, "%10d ", p->lbn);
	    strcat(record, tmp);
	}
	if (bytecount > 0) {	/* Bytecount */
	    sprintf(tmp, "%6d ", p->bcnt);
	    strcat(record, tmp);
	}
	if (iosb > 0) {		/* I/O status block */
	    if (iosb_hex)
		sprintf(tmp, "%08.8X ", p->iosb[0]);
	    else if (iosb_longhex)
		sprintf(tmp, "%08.8X %08.8X ", p->iosb[0], p->iosb[1]);
	    else if (iosb_text)
		sprintf(tmp, "%-7s ", ssdef((p->iosb[0]) & 0xffff));
	    else if (iosb_combination) {
		if (func == IO$_SENSEMODE)
		    j = 0;
		else
		    j = ((p->iosb[0] >> 16) & 0xffff) +
			((p->iosb[1] & 0xffff) << 16);
		sprintf(tmp, "%-7s %6d ", ssdef((p->iosb[0]) & 0xffff), j);
	    }
	    strcat(record, tmp);
	}
	if (function > 0) {	/* I/O function code */
	    if (function_hex)
		sprintf(tmp, "%04.4X ", p->func);
	    else if (function_text)
		sprintf(tmp, "%s ", decode_func(p->func));
	    strcat(record, tmp);
	}
	if (!(*record))
	    break;		/* Nothing to display, quit */
	j = strlen(record);
	record[j - 1] = '\n';	/* Zap trailing space */
	record[j] = '\0';
	if ((stat = fprintf(outfile, "%s", record)) <= 0)
	    lib$signal(&ld_filwrterr, 1, outfilename, vaxc$errno, 0);	/* Can't write output file */
	p++;			/* Next record */
    }
}

/*
  Convert status to readable code
*/
char *ssdef(code)
int code;
{
    static char msgbuf[64];
    struct dsc msgdsc =
    {sizeof(msgbuf), msgbuf};
    int stat;
    short rlen;

    stat = sys$getmsg(code, &rlen, &msgdsc, 2, 0);
    signal_error(stat, 0);
    msgbuf[rlen] = '\0';	/* Trailing zero */
    return &msgbuf[1];		/* Remove leading % */
}

/*
  Process input file
*/
void process_input()
{
    char *buf, *savbuf;
    struct ovrrun *ovrbuf, *savovrbuf;
    char *infilename;
    int infilei;
    FILE *infile;
    char namebuf[32];
    char nodebuf[32];
    char timestmp[24];
    int i;
    int size;
    int recnum;
    int overruncnt;
    int version;
    int overrun;

    if ((infilename = getqualstring(&sw_input)) == 0)	/* Get input filename */
	infilename = fullfilespec("", "LD_TRACE.DAT");	/* Provide default */
    else
	infilename = fullfilespec(infilename, "LD_TRACE.DAT");	/* Provide default */
    if (((infilei = open(infilename, O_RDONLY, 0, "dna=LD_TRACE.DAT")) < 0) ||
	((infile = fdopen(infilei, "r")) == NULL))
	lib$signal(&ld_infilerr, 1, infilename, vaxc$errno, 0);	/* Can't open input file */
    if (fread(&version, sizeof(int), 1, infile) != 1)	/* Get datafile version */
	 lib$signal(&ld_filreaderr, 1, infilename, vaxc$errno, 0);	/* Can't read input file */
    version = (version >> 24) & 0xff;
    if ((version == 0) ||	/* Old version ? */
	(version != DATA_FILE_VERSION))	/* Incompatible version ? */
	lib$signal(&ld_cantreadoldfmt, 1, version);	/* Can't read old input file */
    if ((fread(namebuf, 32, 1, infile) != 1) ||	/* Get devicename */
	(fread(nodebuf, 32, 1, infile) != 1) ||	/* Get nodename */
	(fread(timestmp, 24, 1, infile) != 1))	/* Get timestamp */
	lib$signal(&ld_filreaderr, 1, infilename, vaxc$errno, 0);	/* Can't read input file */
    s_device = namebuf;		/* Point to devicename */
    recnum = 0;
    overruncnt = 0;
    buf = savbuf = 0;
    ovrbuf = savovrbuf = 0;
    while (1) {
	if ((fread(&size, sizeof(int), 1, infile) != 1) ||	/* Get number of records */
	     (fread(&overrun, sizeof(int), 1, infile) != 1))	/* Get number of overruns */
	     lib$signal(&ld_filreaderr, 1, infilename, vaxc$errno, 0);	/* Can't read input file */
/*
  We will call realloc to increase the size of our databuffer. The data
  remains valid up till the address where we've written it before.
*/
	if ((savbuf = realloc(buf, (size + recnum) * sizeof(struct trace_ent))) == 0)
	     signal_error(SS$_INSFMEM, 0);	/* Not enough memory */
	buf = savbuf;		/* Save new address */
	savbuf += (recnum * sizeof(struct trace_ent));	/* Start writing from here */
	if (overrun != 0) {
	    if ((savovrbuf = realloc(ovrbuf, (overruncnt + 1)
				     * sizeof(struct ovrrun))) == 0)
		 signal_error(SS$_INSFMEM, 0);	/* Not enough memory */
	    ovrbuf = savovrbuf;	/* Save new address */
	    savovrbuf += overruncnt;	/* Count total number of overruns */
	    savovrbuf->recnum = recnum;	/* Save recordnumber with overflow */
	    savovrbuf->overrun = overrun;	/* Save count of overruns for this record */
	    overruncnt++;	/* One more */
	}
	recnum += size;		/* Total number of data records */
	if (fread(savbuf, sizeof(struct trace_ent), size, infile) != size)	/* Get data */
	     lib$signal(&ld_filreaderr, 1, infilename, vaxc$errno, 0);	/* Can't read input file */
/*
  fread returns a 0 on EOF as well as when an error occurred. Since we want
  to distinguish these two we try to read one more byte to find the difference
*/
	if ((i = fgetc(infile)) == EOF)
	    break;		/* Ready for processing */
	else
	    ungetc(i, infile);	/* Return character and try again */
    }
    process_trace(buf, recnum, ovrbuf, overruncnt, nodebuf, timestmp, 0);
    free(buf);			/* Return buffer */
    if (ovrbuf != 0)
	free(ovrbuf);		/* Return buffer */
    if (fclose(infile) < 0)	/* Close input file */
	lib$signal(&ld_closerr, 1, infilename, vaxc$errno, 0);
}

/*
 Decode the I/O function to readable text
*/
char *decode_func(what)
int what;
{
    static char funcstr[80];
    char str[80];
    int modifier;
    int function;

    if (what == 0xffff)
        return ("All");
    modifier = what & ~IO$M_FCODE;	/* Get modifier bits */
    function = what & IO$M_FCODE;	/* Get functioncode bits */
    if (!modifier)		/* No modifiers present */
	return cvttbl[function];/* Return pointer to function */
    else {
	*str = '\0';;
	switch (function) {
	case IO$_PACKACK:
	    chk_mod(&modifier, IO$M_INHERLOG, "INHERLOG", str);
	    chk_mod(&modifier, IO$M_MSCP_FORMAT, "MSCP_FORMAT", str);
	    break;
	case IO$_AVAILABLE:
	    chk_mod(&modifier, IO$M_INHERLOG, "INHERLOG", str);
	    chk_mod(&modifier, IO$M_ALLHOSTS, "ALLHOSTS", str);
	    chk_mod(&modifier, IO$M_DISSOLVE, "DISSOLVE", str);
	    chk_mod(&modifier, IO$M_NOCLEANUP, "NOCLEANUP", str);
	    break;
	case IO$_REMSHAD:
	    chk_mod(&modifier, IO$M_SPINDOWN, "SPINDOWN", str);
	    break;
	case IO$_SENSECHAR:
	    chk_mod(&modifier, IO$M_SHADOW, "SHADOW", str);
	    break;
	case IO$_SETMODE:
	    chk_mod(&modifier, IO$M_LINE_ON, "LINE_ON", str);
	    chk_mod(&modifier, IO$M_LINE_OFF, "LINE_OFF", str);
	    chk_mod(&modifier, IO$M_NEWLINE, "NEWLINE", str);
	    chk_mod(&modifier, IO$M_ABORT, "ABORT", str);
	    break;
	case IO$_UNLOAD:
	    chk_mod(&modifier, IO$M_CLSEREXCP, "CLSEREXCP", str);
	    break;
	case IO$_DSE:
	    chk_mod(&modifier, IO$M_DATACHECK, "DATACHECK", str);
	    chk_mod(&modifier, IO$M_ERASE, "ERASE", str);
	    break;
	case IO$_WRITEVBLK:
	case IO$_WRITELBLK:
	case IO$_WRITEPBLK:
	    chk_mod(&modifier, IO$M_TRUSTED, "TRUSTED", str);
	    chk_mod(&modifier, IO$M_EXFUNC, "EXFUNC", str);
	    chk_mod(&modifier, IO$M_INHERLOG, "INHERLOG", str);
	    chk_mod(&modifier, IO$M_DATACHECK, "DATACHECK", str);
	    chk_mod(&modifier, IO$M_CLSEREXCP, "CLSEREXCP", str);
	    chk_mod(&modifier, IO$M_INHRETRY, "INHRETRY", str);
	    chk_mod(&modifier, IO$M_ERASE, "ERASE", str);
	    chk_mod(&modifier, IO$M_MSCPMODIFS, "MSCPMODIFS", str);
	    break;
	case IO$_READVBLK:
	case IO$_READLBLK:
	case IO$_READPBLK:
	    chk_mod(&modifier, IO$M_NOVCACHE, "NOVCACHE", str);
	    chk_mod(&modifier, IO$M_TRUSTED, "TRUSTED", str);
	    chk_mod(&modifier, IO$M_EXFUNC, "EXFUNC", str);
	    chk_mod(&modifier, IO$M_INHERLOG, "INHERLOG", str);
	    chk_mod(&modifier, IO$M_DATACHECK, "DATACHECK", str);
	    chk_mod(&modifier, IO$M_INHRETRY, "INHRETRY", str);
	    break;
	default:
	    break;
	}
	sprintf(funcstr, "%s%s", cvttbl[function], str);	/* Form function string */
	if (modifier & ~IO$M_FCODE)	/* Any modifiers left ? */
	    sprintf(funcstr, "%s (%08.8X)", funcstr, modifier);	/* Show excess modifiers */
	return (funcstr);	/* Return pointer to string */
    }
}

/*
 Check if bit set in modifier function, and copy function to
 output string if true.
*/
void chk_mod(modifier, mask, modifstr, outstr)
int *modifier;
int mask;
char *modifstr;
char *outstr;
{
    if (*modifier & mask) {	/* Check if mask bit set */
	strcat(outstr, "|");	/* Concatenate bar */
	strcat(outstr, modifstr);	/* Concatenate inpout string */
	*modifier &= ~mask;	/* Flags this bit as processed */
    }
}

/*
  Convert binary time to ascii,
  return only the time and not the date
*/
char *cvttime(time, how, full)
int time[2];
int how;
int full;
{
    short rlen;
    int stat;
    struct dsc timdsc;
    static char timstr[256];

    timdsc.len = 256;
    timdsc.addr = timstr;
    stat = sys$asctim(&rlen, &timdsc, time, full);	/* Convert binary time to ascii */
    signal_error(stat, 0);
    timstr[rlen] = '\0';	/* Add trailing zero */
    if (how)			/* Display only seconds */
	return (&timstr[6]);
    return timstr;
}

/*
  Open specified file
*/

void open_file(name)
char *name;
{
    int stat;
    int i;
    struct iosb iosb;

    ld_fab.fab$l_dna = ".DSK";	/* Default extension */
    ld_fab.fab$b_dns = 4;	/* and length */
    ld_fab.fab$l_fop = FAB$M_NAM;	/* Use NAM block */
    ld_fab.fab$l_nam = &ld_nam;	/* Pointer to NAM block */
    ld_fab.fab$l_fna = name;	/* File name address */
    ld_fab.fab$b_fns = strlen(name);	/* and length */
    ld_fab.fab$l_xab = &ld_xabfhc;	/* Point to FHC XAB */
    ld_fab.fab$b_rtv = 255;		/* Open with cathedral windows */
    ld_nam.nam$b_rss = NAM$C_MAXRSS;	/* Max. resultant size */
    ld_nam.nam$l_rsa = resspec;	/* Resultant address */
    ld_nam.nam$b_ess = NAM$C_MAXRSS;	/* Max. expanded size */
    ld_nam.nam$l_esa = expspec;	/* Expanded address */
    ld_nam.nam$b_nop = NAM$M_NOCONCEAL;	/* Don't hide concealed devices */
    stat = sys$parse(&ld_fab);	/* Parse this one */
    if (!(stat & 1))
	lib$signal(&ld_openerr, 1, name, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
/*
 Copy devicename + directoryname. This is done to include possible
 rooted devicenames which we will lose after the open
*/
    i = ld_nam.nam$b_dev + ld_nam.nam$b_dir;
    strncpy(realspec, ld_nam.nam$l_dev, i);
    realspec[i] = '\0';		/* Make sure it's terminated */
    stat = sys$open(&ld_fab);	/* And open it */
    if (!(stat & 1))
	lib$signal(&ld_openerr, 1, name, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
    strncat(&realspec[i], ld_nam.nam$l_name, ld_nam.nam$b_name +	/* Append rest */
	    ld_nam.nam$b_type + ld_nam.nam$b_ver);
    realspec_dsc.addr = realspec;	/* Get real filespec */
    realspec_dsc.len = strlen(realspec);
    ldfilename_dsc.addr = &realspec[ld_nam.nam$b_dev];	/* Get filename without device */
    ldfilename_dsc.len = strlen(realspec) - ld_nam.nam$b_dev;
    realdev_dsc.addr = ld_nam.nam$l_dev;	/* Get real devicename */
    realdev_dsc.len = ld_nam.nam$b_dev;
    maxblocks = ld_xabfhc.xab$l_ebk;	/* Get current eof pointer */
    if (ld_xabfhc.xab$w_ffb == 0)	/* Adjust if necessary */
	maxblocks--;
#ifdef __DECC_MODE_VAXC
    fib.fib$w_fid[0] = ld_nam.nam$w_fid[0];	/* Get file-id */
    fib.fib$w_fid[1] = ld_nam.nam$w_fid[1];
    fib.fib$w_fid[2] = ld_nam.nam$w_fid[2];
#else
    fib.fib$r_fid_overlay.fib$w_fid[0] = ld_nam.nam$w_fid[0];	/* Get file-id */
    fib.fib$r_fid_overlay.fib$w_fid[1] = ld_nam.nam$w_fid[1];
    fib.fib$r_fid_overlay.fib$w_fid[2] = ld_nam.nam$w_fid[2];
#endif
    stat = sys$assign(&realdev_dsc, &fchan, 0, 0, 0);	/* Assign channel to device */
    signal_error(stat, 0);	/* where file resides */
    atr[0].atr$w_size = SBK$K_LENGTH;
    atr[0].atr$w_type = ATR$C_STATBLK;	/* Get statistics block */
    atr[0].atr$l_addr = &sbk;
    atr[1].atr$w_size = 0;	/* Terminator */
    atr[1].atr$w_type = 0;
#ifdef __DECC_MODE_VAXC
    fib.fib$v_notrunc = 1;	/* Truncate not allowed */
    fib.fib$b_wsize = -1;	/* Open with cathedral windows */
#else
    fib.fib$r_acctl_overlay.fib$r_acctl_bits0.fib$v_notrunc = 1;	/* Truncate not allowed */
    fib.fib$r_acctl_overlay.fib$r_acctl_fields2.fib$b_wsize = -1;	/* Open with cathedral windows */
#endif
    fib_dsc.addr = &fib;	/* Point to FIB */
    fib_dsc.len = FIB$K_LENGTH;
    stat = sys$qiow(0, fchan, IO$_ACCESS | IO$M_ACCESS, &iosb, 0, 0,
		    &fib_dsc, 0, 0, 0, &atr, 0);	/* Access the file */
    signal_error(stat, iosb.status);
}

/*
  Close file opened with 'open_file'
*/

void close_file(filename)
char *filename;
{
    int stat;
    struct iosb iosb;

    stat = sys$qiow(0, fchan, IO$_DEACCESS, &iosb, 0, 0,
		    &fib_dsc, 0, 0, 0, 0, 0);	/* Deaccess file */
    signal_error(stat, iosb.status);
    stat = sys$dassgn(fchan);	/* And get rid of channel */
    signal_error(stat, 0);
    stat = sys$close(&ld_fab);	/* Close RMS file */
    if (!(stat & 1))
	lib$signal(&ld_closerr, 1, filename, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
    signal_error(stat, 0);
}

/*
  Disconnect file from LD device
*/

void disconnect_unit()
{
    int stat;
    int q_abort;
    int loop = 0;
    char matchname[256];
    struct dsc matchdev =
    {sizeof(matchname), matchname};
    short rlen;
    int context[2] =
    {0, 0};

    s_device = getqualstring(&sw_device);	/* Get device name */
    q_abort = getqual(&sw_abort) > 0;	/* Get 'abort' qualifier */
    if (getqual(&sw_all) > 0) {	/* /ALL ? */
	stat = 1;
	while (stat & 1) {	/* Loop until we're done */
	    stat = sys$device_scan(&matchdev, &rlen, &wildname, 0, &context);
	    if (stat == SS$_NOMOREDEV) {
		if (loop) {
		    if (loop == 1)
			lib$signal(&ld_nounitsfound, 1, matchname);
		    else
			return;	/* No more, quit */
		} else
		    signal_error(SS$_NOSUCHDEV, 0);	/* No device at all */
	    }
	    loop++;
	    signal_error(stat, 0);
	    matchname[rlen] = '\0';	/* Place terminator */
	    stat = disconnect_one(matchname, q_abort, 0);
	    /* Disconnect this device */
	    if (stat == SS$_DEVINACT) {	/* If not connected... */
		lib$signal(&ld_notconnected, 1, matchname);	/* Flag it */
		stat = 1;	/* and continue */
	    }
	}
    } else {
	stat = disconnect_one(s_device, q_abort, 1);
	if (stat == SS$_DEVINACT) {	/* If not connected... */
	    lib$signal(&ld_notconnected, 1, fulldevspec(s_device));	/* Flag it */
	    stat = &ld_notconnected;	/* Convert to better message */
	    stat |= STS$M_INHIB_MSG;	/* Don't show it again */
	}
    }
}

int disconnect_one(dev, abortflag, single)
char *dev;
int abortflag;
int single;
{
    char ldfile[256];
    char lddev[64];
    struct dsc sdev;
    char *devstr;
    int stat;
    int chan;
    int func;
    int replacefl;
    struct iosb iosb;
    struct fiddef fid;

    devstr = fulldevspec(dev);	/* Save devicename while it's still there */
    if (get_unit(devstr, 1) == 0) {	/* Don't use unit 0 */
	if (single)
	    lib$signal(&ld_badunit, 1, devstr);	/* Show it to the world */
	else
	    return (1);
    }
    sdev.addr = dev;
    sdev.len = strlen(dev);
    stat = sys$assign(&sdev, &chan, 0, 0, 0);	/* assign channel */
    if (stat == SS$_DEVALLOC) {	/* Allocated on remote node ? */
	lib$signal(&ld_remotealloc, 1, devstr);
    } else {
	signal_error(stat, 0);
	if (!abortflag) {		/* No /ABORT ? */
	    stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,
		   lddev, sizeof(lddev), &fid, 0, 0, LDIO_GET_CONNECTION);	/* Get current status */
	    signal_error(stat, 0);
	    if (!iosb_flags.connected)	/* Check 'connected' flag from driver */
		return (&ld_notconnected);
	    lddev[iosb.size] = '\0';	/* Terminate string */
	    if (!(replacefl = iosb_flags.replaced)) {
		create_filename(lddev,&fid,ldfile);
		open_file(ldfile);	/* Open file */
	    }
	    func = LDIO_DISCONNECT;	/* Normal function */
	} else {
	    func = LDIO_DISCONNECT | LDIO_M_ABORT;	/* Some more for abort */
        }

	stat = sys$qiow(0, chan, IO$_LD_CONTROL, &iosb, 0, 0,	/* Issue disconnect */
		    &sbk, 0, 0, 0, 0, func);
	signal_error(stat, iosb.status);
	stat = sys$dassgn(chan);	/* Get rid of channel */
	signal_error(stat, 0);
	if (!abortflag && !replacefl)	/* No /ABORT ? */
	    close_file(ldfile);	/* Only close if not abort */
	if (getqual(&sw_log) > 0)	/* /LOG ? */
	    lib$signal(&ld_nowdisconn, 1, devstr);	/* Show what we did */
    }
    return (1);
}

/*
  Create file for LD
*/

void create()
{
    int stat;
    int q_size;
    char dummy[512];

    s_file = getqualstring(&sw_file);	/* Get filespec */
    s_file = fullfilespec(s_file, ".DSK");
    if ((q_size = getqualvalue(&sw_size)) == 0)	/* Get filesize */
	q_size = 512;		/* Default */
    setup_fab(s_file, &q_size);	/* Init for RMS */
    stat = sys$parse(&ld_fab);	/* Parse filename */
    if (!(stat & 1))
	lib$signal(&ld_createrr, 1, s_file, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
    if (ld_nam.nam$v_node)	/* May not contain node spec */
	lib$signal(&ld_createrr, 1, s_file, RMS$_SUPPORT);
    if (ld_nam.nam$v_wildcard)	/* or wildcard */
	lib$signal(&ld_createrr, 1, s_file, RMS$_WLD);
    stat = sys$create(&ld_fab);	/* Create the file */
    if (!(stat & 1))
	lib$signal(&ld_createrr, 1, s_file, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
    setfilechar();		/* Set NOMOVE and NOBACKUP characteristic */
    stat = sys$connect(&ld_rab);/* Hook up RAB */
    if (!(stat & 1))
	lib$signal(&ld_createrr, 1, s_file, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
    ld_rab.rab$l_rbf = dummy;	/* Dummy buffer */
    stat = sys$put(&ld_rab);	/* Put dummy block at end of file */
    /* to get EOF pointer at right place */
    if (!(stat & 1))
	lib$signal(&ld_filwrterr, 1, s_file, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
    stat = sys$close(&ld_fab);	/* Close file */
    if (!(stat & 1))
	lib$signal(&ld_closerr, 1, s_file, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
    if (getqual(&sw_log) > 0) {	/* /LOG ? */
	open_file(s_file);	/* Open again */
	lib$signal(&ld_created, 1, realspec);	/* Show it */
	close_file(s_file);
    }
}

/*
  Init FAB for create
*/

void setup_fab(filname, size)
char *filname;
int *size;
{
    ld_fab.fab$l_alq = *size;	/* Filesize */
    ld_fab.fab$l_dna = ".DSK";	/* Default extension */
    ld_fab.fab$b_dns = 4;	/* and length */
/*    ld_fab.fab$l_fop = FAB$M_NAM | FAB$M_CBT | FAB$M_OFP; */	/* NAM */
    ld_fab.fab$l_fop = FAB$M_NAM | FAB$M_OFP;	/* NAM */
    ld_fab.fab$b_org = FAB$C_SEQ;	/* Sequential */
    ld_fab.fab$b_rfm = FAB$C_FIX;	/* Fixed */
    ld_fab.fab$w_mrs = 512;	/* 512 byte records */
    ld_fab.fab$l_nam = &ld_nam;	/* NAM block */
    ld_fab.fab$l_fna = filname;	/* Filename */
    ld_fab.fab$b_fns = strlen(filname);	/* and length */

    ld_rab.rab$l_fab = &ld_fab;	/* Point to FAB */
    ld_rab.rab$b_rac = RAB$C_KEY;	/* Keyed access */
    ld_rab.rab$l_kbf = size;	/* Pointer to record (last in file) */
    ld_rab.rab$w_rsz = 512;	/* Record size */

    ld_nam.nam$b_rss = NAM$C_MAXRSS;	/* Max. resultant size */
    ld_nam.nam$l_rsa = resspec;	/* Resultant address */
    ld_nam.nam$b_ess = NAM$C_MAXRSS;	/* Max. expanded size */
    ld_nam.nam$l_esa = expspec;	/* Expanded address */
}

void setfilechar()
{
    int stat;
    struct iosb iosb;
    int attrib;

/*
   Use QIO to modify the attributes since RMS won't support NOMOVE
   until VMS V6.0
*/

#ifdef __DECC_MODE_VAXC
    fib.fib$w_fid[0] = ld_nam.nam$w_fid[0];	/* Get file-id */
    fib.fib$w_fid[1] = ld_nam.nam$w_fid[1];
    fib.fib$w_fid[2] = ld_nam.nam$w_fid[2];
#else
    fib.fib$r_fid_overlay.fib$w_fid[0] = ld_nam.nam$w_fid[0];	/* Get file-id */
    fib.fib$r_fid_overlay.fib$w_fid[1] = ld_nam.nam$w_fid[1];
    fib.fib$r_fid_overlay.fib$w_fid[2] = ld_nam.nam$w_fid[2];
#endif
    realdev_dsc.addr = ld_nam.nam$l_dev;	/* Get real devicename */
    realdev_dsc.len = ld_nam.nam$b_dev;
    stat = sys$assign(&realdev_dsc, &fchan, 0, 0, 0);	/* Assign channel to device */
    signal_error(stat, 0);	/* where file resides */
    atr[0].atr$w_size = ATR$S_UCHAR;
    atr[0].atr$w_type = ATR$C_UCHAR;	/* Get user file characteristics */
    atr[0].atr$l_addr = &attrib;
    atr[1].atr$w_size = 0;	/* Terminator */
    atr[1].atr$w_type = 0;
    fib_dsc.addr = &fib;	/* Point to FIB */
    fib_dsc.len = FIB$K_LENGTH;
    stat = sys$qiow(0, fchan, IO$_ACCESS, &iosb, 0, 0,
		    &fib_dsc, 0, 0, 0, &atr, 0);	/* Access the file */
    signal_error(stat, iosb.status);
    attrib |= FCH$M_NOMOVE;	/* Mark no move */
    if (getqual(&sw_backup) < 0)
	attrib |= FCH$M_NOBACKUP;	/* Mark no backup */
    stat = sys$qiow(0, fchan, IO$_MODIFY, &iosb, 0, 0,
		    &fib_dsc, 0, 0, 0, &atr, 0);	/* Access the file */
    signal_error(stat, iosb.status);
    stat = sys$dassgn(fchan);	/* Get rid of channel */
    signal_error(stat, 0);
}

/*
  Get full filename from specified one
*/

char *fullfilespec(name, defname)
char *name, *defname;
{
    static char tmpnam[NAM$C_MAXRSS];
    int stat;
    int len;

    if (name)
	len = strlen(name);
    else
	len = 0;

    ld_nam.nam$b_nop = NAM$M_NOCONCEAL;	/* Name options */
    ld_nam.nam$b_ess = NAM$C_MAXRSS;	/* Max. expanded size */
    ld_nam.nam$l_esa = tmpnam;	/* Expanded address */

    ld_fab.fab$l_nam = &ld_nam;	/* NAM block */
    ld_fab.fab$l_dna = defname;	/* Default name address */
    ld_fab.fab$b_dns = strlen(defname);	/* Default name size */
    ld_fab.fab$b_fns = len;	/* Filename length */
    ld_fab.fab$l_fna = name;	/* Filename address */

    stat = sys$parse(&ld_fab);	/* Parse this name */
    if (!(stat & 1))
	lib$signal(&ld_detectederr, 0, ld_fab.fab$l_sts, ld_fab.fab$l_stv);
    tmpnam[ld_nam.nam$b_esl] = '\0';	/* Add terminator */
    return (tmpnam);
}

/*
  Get full device name from specified one
*/

char *fulldevspec(name)
char *name;
{
    char *tmpnam;
    struct dsc resdsc;
    struct dsc namdsc = {strlen(name), name};
    short rlen;
    int stat;

    tmpnam = malloc(64);	/* Get memory for devicename */
    if (tmpnam == 0)
	signal_error(SS$_INSFMEM, 0);	/* Not enough memory */
    resdsc.addr = tmpnam;
    resdsc.len = 64;
    stat = lib$getdvi(&DVI$_FULLDEVNAM, 0, &namdsc, 0, &resdsc, &rlen);	/* Get full name */
    signal_error(stat, 0);
    tmpnam[rlen] = '\0';	/* Terminator */
    return (tmpnam + 1);	/* Skip leading '_' */
}

/*
  Get full filespec from file-id
*/

void create_filename(device,fid,outbuf)
char *device;
struct fiddef *fid;
char *outbuf;
{
    struct dsc outdsc;
    struct dsc devdsc;
    int stat;
    short rlen;
    char tmpbuf[256];
    char *p;

    devdsc.len = strlen(device);	/* Input devicename */
    devdsc.addr = device;
    outdsc.len = sizeof(tmpbuf);	/* Output buffer length */
    outdsc.addr = tmpbuf;
    stat = lib$fid_to_name(&devdsc, fid, &outdsc, &rlen, 0, 0);
    signal_error(stat, 0);
    tmpbuf[rlen]='\0';

/* Modify the devicename (as returned from lib$fid_to_name) from
   DISK$LABEL: to _SYSTEM$BLA0: */

    strcpy(outbuf,fulldevspec(device));	/* Get real devicename */
    p = strchr(tmpbuf,':') + 1;
    strcat(outbuf,p);
}

/*
  Get nodename
*/

char *nodename()
{
    static char tmpnam[256];
    static struct dsc resdsc =
    {sizeof(tmpnam), tmpnam};
    struct dsc tbl =
    {12, "LNM$FILE_DEV"};
    struct dsc lognam =
    {8, "SYS$NODE"};
    short rlen;
    struct itmlst item[2];
    int stat;

    tmpnam[0] = '\0';
    stat = lib$getsyi(&SYI$_NODENAME, 0, &resdsc, &rlen, 0, 0);	/* Get nodename */
    if (stat & 1) {		/* Nodename found */
	tmpnam[rlen] = '\0';	/* Terminator */
	strcat(tmpnam, "::");
    } else {			/* No nodename, try the logical SYS$NODE */
	item[0].item = LNM$_STRING;
	item[0].buflen = 256;
	item[0].addr = tmpnam;
	item[0].rlength = &rlen;
	item[1].item = 0;	/* End of itemlist */
	item[1].buflen = 0;
	stat = sys$trnlnm(0, &tbl, &lognam, 0, &item);
	signal_error(stat, 0);
	tmpnam[rlen] = '\0';	/* Terminator */
    }
    return (tmpnam);
}
/*
  Build lock for trace
*/

char *build_lock(dev)
char *dev;
{
    static char locknam[256];

    strcpy(locknam, "$LOGDISK_");	/* First part of lockname */
    strcat(locknam, nodename());/* Attach nodename */
    locknam[strlen(locknam) - 2] = '\0';	/* Zap trailing "::" */
    strcat(locknam, fulldevspec(dev));	/* Attach devicename */
    return locknam;
}

void set_outband()
{
    int stat;
    int type;
    int mask[2];
    struct iosb iosb;
    $DESCRIPTOR(outbdev, "SYS$COMMAND");

    stat = lib$getdvi(&DVI$_TRM, 0, &outbdev, &type, 0, 0);
    signal_error(stat, 0);
    if (!type)			/* Not a terminal, don't bother */
	return;
    stat = sys$assign(&outbdev, &outband_chan, 0, 0, 0);	/* assign channel */
    signal_error(stat, 0);
    mask[0] = 0;
    mask[1] = (1 << ('C' - 64)) | (1 << ('Z' - 64));
    stat = sys$qiow(0, outband_chan, IO$_SETMODE | IO$M_OUTBAND, &iosb, 0, 0,
		    &ld_exit, &mask, 0, 0, 0, 0);	/* Get current status */
    signal_error(stat, iosb.status);
}

void purge_file(limit, name)
int limit;
char *name;
{
    int stat;
    char *pos;
    char tmpnam[256];
    struct dsc fildsc;
    $DESCRIPTOR(deflt, ".DAT");

    pos = strrchr(name, ';');	/* Find trailing ; */
    if (pos != 0) {
	if (name[strlen(name) - 1] != ';')
	    return;		/* Exact filespec speficied */
	else
	    *pos = '\0';	/* Zap trailing ; */
    }
    if (limit == 0)
	sprintf(tmpnam, "%s;%d", name, limit);	/* Delete lastest version */
    else
	sprintf(tmpnam, "%s;-%d", name, limit);	/* Delete old version */
    fildsc.addr = tmpnam;
    fildsc.len = strlen(tmpnam);

    stat = 1;
    while (stat == 1)		/* until out of files */
	stat = lib$delete_file(&fildsc, &deflt, 0, 0, 0, 0, 0, 0, 0);
    if (stat != RMS$_FNF)	/* No such file is no error */
	signal_error(stat, 0);
}

void ld_exit(param)
int param;
{
    int stat;

    if ((outfile != 0) && (outfile != stdout)) {	/* Close output file */
	if (fclose(outfile) < 0)
	    lib$signal(&ld_closerr, 1, outfilename, vaxc$errno, 0);
    }
    stat = sys$deq(ld_lksb.lockid, 0, 0, 0);
    signal_error(stat, 0);
    if (outband_chan != 0) {
	stat = sys$dassgn(outband_chan);	/* Get rid of channel */
	signal_error(stat, 0);
    }
    if (param == 0) {		/* Called by blocking ast routine */
	lib$signal(&ld_tracestopped);
	exit(&ld_tracestopped);
    } else
	exit(1);
}

void signal_error(st1, st2)
int st1, st2;
{
    int stat1, stat2;
    int modified1, modified2;

    modified1 = convert_error(st1, &stat1);	/* Return reasonable error */
    modified2 = convert_error(st2, &stat2);
    if ((!(stat1 & 1)) || (st2 && (!(stat2 & 1)))) {
	if (modified1 || modified2) {
	    if (stat1 == 1)
		lib$signal(stat2, 0, 0);
	    else
		lib$signal(stat1, 0, stat2, 0, 0);
	} else {
	    if (stat1 == 1)
		lib$signal(&ld_detectederr, 0, stat2, 0, 0);
	    else
		lib$signal(&ld_detectederr, 0, stat1, 0, stat2, 0, 0);
	}
    }
}

/*
  Return a reasonable error for some returned by the driver
*/

int convert_error(oldstatus, newstatus)
int oldstatus;
int *newstatus;
{
    int code = 0;

    if (oldstatus == SS$_DEVASSIGN)
	code = &ld_devassign;	/* 'Device has channels assigned' */
    else if (oldstatus == SS$_DEVACTIVE)
	code = &ld_alrdyconn;	/* 'Already connected' */
    else if (oldstatus == SS$_DEVINACT)
	code = &ld_notconn;	/* 'Not connected' */
    else if (oldstatus == SS$_NODATA)
	code = &ld_trcdisabl;	/* 'Tracing already disabled' */
    else if (oldstatus == SS$_TOOMUCHDATA)
	code = &ld_trcenabl;	/* 'Tracing already enabled' */
    else if (oldstatus == SS$_DATALOST)
	code = &ld_nowatchdata;	/* 'No watchpoint data available' */
    else if (oldstatus == SS$_DATACHECK)
	code = &ld_wptnotfound;	/* 'Watchpoint not found' */
    else if (oldstatus == SS$_NOTVOLSET)
	code = &ld_fileonvolset;/* 'File watchpoint on volumeset not allowed' */
    else if (oldstatus == SS$_FILALRACC)
	code = &ld_fileinuse;	/* 'File already in use' */
    else if (oldstatus == SS$_DEVALLOC)
	code = &ld_deviceinuse;	/* 'Device already in use' */
    else if (oldstatus == SS$_DEVREQERR)
	code = &ld_fileonother;	/* 'File not allowed to be on other device' */
    else if (oldstatus == SS$_ILLBLKNUM)
	code = &ld_vbnerror;	/* 'Illegal virtual block number specified' */
    else if (oldstatus == SS$_UNSUPPORTED)
	code = &ld_nocluster;	/* 'No cluster code loaded' */
    else if (oldstatus == SS$_ACCONFLICT)
	code = &ld_notvisible;	/* 'Device is not visible on other nodes' */
    else if (oldstatus == SS$_BADPARAM)
	code = &ld_invgeometry;	/* 'Invalid geometry specified' */
    else if (oldstatus == SS$_UNSAFE)
	code = &ld_devconnected;/* 'Cannot set allocation class with active LD devices' */
    else if (oldstatus == SS$_WRONGACP)
	code = &ld_notods2;	/* 'Container file must be on an ODS-2 volume' */
    else {
	*newstatus = oldstatus;	/* Return original status */
	return 0;		/* Not modified */
    }
    *newstatus = code;		/* Return new status */
    return 1;			/* Modified */
}
