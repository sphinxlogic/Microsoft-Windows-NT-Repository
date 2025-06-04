/*

FINDLOCKS, System Management, Reports all RMS record locks on cluster.
This program scans the Distributed Lock Manager searching for RMS record
locks. For each lock found it displays the process ID, and name, the user
name, terminal name and the name of the image being executed. It also 
displays the name of the file being locked, the VBN and ID of the locked
record and the first 50 bytes of the record. If the file is indexed, all
the keys are displayed. Unprintable characters within displayed data
are output as dots.

This program should just compile and link using:

$ cc/pref=all/opt=level=5 findlocks
$ link findlocks

Author: Nigel White <NigelWhite@bagbourne.freeserve.co.uk>

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ints.h>
#include <builtins.h>
#include <rms.h>
#include <jpidef.h>
#include <lkidef.h>
#include <lckdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <descrip.h>
#include <lib$routines.h>
#include <dvidef.h>
#include <dcdef.h>
#include <dvsdef.h>

#define MAXDEVICE  200
#define MAXDEVNAMLEN  64
#define MAXDEVLOCKNAM 13

#define min(__v1, __v2) (((__v1) < (__v2)) ? (__v1) : (__v2))
#define terminator (__int16)0,(__int16)0,(void *)0,(unsigned short *)0
#define clear(__X) memset((void *)&__X, 0, sizeof(__X));
#ifdef __ALPHA
#define populate_descrip(dp, cptr, len) *((uint64 *)(dp)) = ((len) | (DSC$K_DTYPE_T << 16) | (DSC$K_CLASS_S << 24) | (((uint64)(cptr)) << 32))
#else
#define populate_descrip(dp, cptr, len) *((uint32 *)(dp)) = ((len) | (DSC$K_DTYPE_T << 16) | (DSC$K_CLASS_S << 24)); \
                                        *(((char **)(dp))+1) = cptr;
#endif

typedef struct
{
    __int16		buflen, code;
    void		*bufadr;
    unsigned short	*retlenadr;
} itemlst;

typedef union
{
    struct
    {
	__int32	    id,
		    vbn;
	char	    fill[25];
    } record_lock;
    struct
    {
	__int32	    rms$;
	struct
	{
	    unsigned    __int16
                        file_num,
                        file_seq;
	    unsigned    char
	                rvn,
                        nmx;
	} file_id;
	char	    lockname[22];
    } file_lock;
} resnam;

static __int32
	status,
	no_of_disks,
	disk_number,
	lkef,
	found_lockid,
	parent_lockid,
	wildcard,
	locking_pid,
	i,
	devlog_len,
	file_org,
	wildcard_args[8],
	parent_args[8];

static __int16
	iosb[4],
	filespec_len,
	username_len,
	image_name_len,
	process_name_len,
	terminal_name_len,
	phydevnam_len,
	node_name_len;

static itemlst
	scan_itemlist[5],
	parent_itemlist[3],
	getjpi_itemlist[7];

static resnam
	found_resnam,
	parent_resnam;

static struct
{	 
    char    state_requested,
            state_granted,
	    state_queue;
} lock_state;

static char disk_name_list[MAXDEVICE][MAXDEVNAMLEN];	    /* counted string */
static char disk_lockname_list[MAXDEVICE][MAXDEVLOCKNAM+1];

static char
	filespec_b[255],
	out_line_b[132],
	username_b[65],
	image_name_b[65],
	process_name_b[65],
	terminal_name_b[65],
	phydevnam_b[65],
	node_name_b[65];

static $DESCRIPTOR(filespec, filespec_b);
static $DESCRIPTOR(out_line, out_line_b);
static $DESCRIPTOR(username, username_b);
static $DESCRIPTOR(image_name, image_name_b);
static $DESCRIPTOR(process_name, process_name_b);
static $DESCRIPTOR(terminal_name, terminal_name_b);
static $DESCRIPTOR(phydevnam, phydevnam_b);
static $DESCRIPTOR(node_name, node_name_b);
static $DESCRIPTOR(l1mask, "Process !XL, \"!AS\", User: !AS on node: !AS");
static $DESCRIPTOR(l2mask, "running !AS at !AS");
static $DESCRIPTOR(l3mask, "Has a lock on !AS");
static $DESCRIPTOR(l4mask, "VBN = !UL, ID = !UL");
static $DESCRIPTOR(keymask, "Key !2UL, \"!AS\"");
static $DESCRIPTOR(search_devnam, "*D*");
static $DESCRIPTOR(devnam_desc, disk_name_list[0]);

static char *format_unprintable();
static struct fabdef *openinput();
static void readrfa();
static void fileclose();
static struct xabsum *get_xabsum(struct fabdef *);
static int make_device_name_list();

int main()
{

    make_device_name_list(&search_devnam);

    clear(scan_itemlist);
    scan_itemlist[0].buflen = sizeof(found_resnam);
    scan_itemlist[0].code = LKI$_RESNAM;
    scan_itemlist[0].bufadr = &found_resnam;
    scan_itemlist[1].buflen = sizeof(found_lockid);
    scan_itemlist[1].code = LKI$_LOCKID;
    scan_itemlist[1].bufadr = &found_lockid;
    scan_itemlist[2].buflen = sizeof(parent_lockid);
    scan_itemlist[2].code = LKI$_PARENT;
    scan_itemlist[2].bufadr = &parent_lockid;
    scan_itemlist[3].buflen = sizeof(lock_state);
    scan_itemlist[3].code = LKI$_STATE;
    scan_itemlist[3].bufadr = &lock_state;

    clear(wildcard_args);
    wildcard = -1;
    wildcard_args[0] = 7;
    status = lib$get_ef(&wildcard_args[1]);
    if (!(status & 1))
    	lib$stop(status);
    wildcard_args[2] = (__int32)&wildcard;
    wildcard_args[3] = (__int32)&scan_itemlist;
    wildcard_args[4] = (__int32)&iosb;

    clear(parent_itemlist);
    parent_itemlist[0].buflen = sizeof(parent_resnam);
    parent_itemlist[0].code = LKI$_RESNAM;
    parent_itemlist[0].bufadr = &parent_resnam;
    parent_itemlist[1].buflen = sizeof(locking_pid);
    parent_itemlist[1].code = LKI$_PID;
    parent_itemlist[1].bufadr = &locking_pid;

    clear(parent_args);
    parent_args[0] = 7;
    parent_args[1] = (__int32)wildcard_args[1];
    parent_args[2] = (__int32)&parent_lockid;
    parent_args[3] = (__int32)&parent_itemlist;
    parent_args[4] = (__int32)&iosb;

    clear(getjpi_itemlist);
    getjpi_itemlist[0].buflen = sizeof(username_b);
    getjpi_itemlist[0].code = JPI$_USERNAME;
    getjpi_itemlist[0].bufadr = &username_b;
    getjpi_itemlist[0].retlenadr = &username.dsc$w_length;
    getjpi_itemlist[1].buflen = sizeof(image_name_b);
    getjpi_itemlist[1].code = JPI$_IMAGNAME;
    getjpi_itemlist[1].bufadr = &image_name_b;
    getjpi_itemlist[1].retlenadr = &image_name.dsc$w_length;
    getjpi_itemlist[2].buflen = sizeof(process_name_b);
    getjpi_itemlist[2].code = JPI$_PRCNAM;
    getjpi_itemlist[2].bufadr = &process_name_b;
    getjpi_itemlist[2].retlenadr = &process_name.dsc$w_length;
    getjpi_itemlist[3].buflen = sizeof(terminal_name_b);
    getjpi_itemlist[3].code = JPI$_TERMINAL;
    getjpi_itemlist[3].bufadr = &terminal_name_b;
    getjpi_itemlist[3].retlenadr = &terminal_name.dsc$w_length;
    getjpi_itemlist[4].buflen = sizeof(phydevnam_b);
    getjpi_itemlist[4].code = JPI$_TT_PHYDEVNAM;
    getjpi_itemlist[4].bufadr = &phydevnam_b;
    getjpi_itemlist[4].retlenadr = &phydevnam.dsc$w_length;
    getjpi_itemlist[5].buflen = sizeof(node_name_b);
    getjpi_itemlist[5].code = JPI$_NODENAME;
    getjpi_itemlist[5].bufadr = &node_name_b;
    getjpi_itemlist[5].retlenadr = &node_name.dsc$w_length;

    for (;;)
    {
        status = sys$cmexec(sys$getlkiw, wildcard_args);
        if (status == SS$_NOMORELOCK)
            break;
        if (status & 1)
            status = iosb[0];
        if (!(status & 1))
            lib$stop(status);
/*      Find next lock in system, requesting it's ID, state, parent,
        and resource name. */

        status = sys$cmexec(sys$getlkiw, parent_args);
        if (status & 1)
            status = iosb[0];
        if (!(status & 1))
            continue;
/*      Find its parent to see if it is an RMS lock.			*/

        if ((parent_lockid) && (parent_resnam.file_lock.rms$ == 'RMS$') && (lock_state.state_granted == LCK$K_EXMODE))
        {

            struct fabdef
                   *fab;
            struct namdef
                   *nam;
            struct rabdef
                   *rab;

get_proc_data:

            status = sys$getjpiw(wildcard_args[1], &locking_pid, 0,
            getjpi_itemlist, &iosb, 0, 0);
            if (status == SS$_SUSPENDED)
                goto get_proc_data;
            if (status & 1)
                status = iosb[0];
            if (!(status & 1))
                lib$stop(status);
            if (!image_name.dsc$w_length)
            {
                memcpy(image_name_b, "(DCL)", 5);
                image_name.dsc$w_length = 5;
            }
            process_name_b[process_name.dsc$w_length] = '\0';
            while (username_b[username.dsc$w_length - 1] == ' ')
                username.dsc$w_length--;

            for (disk_number = 0;
            strcmp ((char *)disk_lockname_list[disk_number], parent_resnam.file_lock.lockname);
            disk_number++)
            {
                if (disk_number == no_of_disks)
                    continue;
            }

            filespec.dsc$a_pointer = filespec_b;
            filespec.dsc$w_length = sizeof(filespec_b);
            devnam_desc.dsc$a_pointer = &disk_name_list[disk_number][1];
            devnam_desc.dsc$w_length = disk_name_list[disk_number][0];
            status = lib$fid_to_name(&devnam_desc, &parent_resnam.file_lock.file_id,
            &filespec, &filespec.dsc$w_length);
            if (!(status & 1))
                lib$stop(status);
/*          Get file name							*/

            fab = openinput(filespec_b, filespec.dsc$w_length);
            if (fab)
            {
                rab = (struct rabdef *)(fab + 1);
                readrfa(rab, found_resnam.record_lock.vbn, found_resnam.record_lock.id);
/*              Read the locked record by its RFA */

                nam = (struct namdef *)(rab + 1);
                filespec.dsc$a_pointer = nam->nam$l_rsa;
                filespec.dsc$w_length = nam->nam$b_rsl;
            }

            out_line.dsc$w_length = sizeof(out_line_b);
            sys$fao(&l1mask, &out_line.dsc$w_length, &out_line, 
                locking_pid, &process_name, &username, &node_name);
            lib$put_output(&out_line);

            out_line.dsc$w_length = sizeof(out_line_b);
            sys$fao(&l2mask, &out_line.dsc$w_length, &out_line, &image_name, &phydevnam);
            lib$put_output(&out_line);

            out_line.dsc$w_length = sizeof(out_line_b);
            sys$fao(&l3mask, &out_line.dsc$w_length, &out_line, &filespec);
            lib$put_output(&out_line);

            out_line.dsc$w_length = sizeof(out_line_b);
            sys$fao(&l4mask, &out_line.dsc$w_length, &out_line, found_resnam.record_lock.vbn, found_resnam.record_lock.id);
            lib$put_output(&out_line);

            if (fab)
            {
                struct dsc$descriptor
                        keystring;
                uint32 rec_size;

                rec_size = (fab->fab$b_rfm == FAB$C_FIX) ? rab->rab$w_usz : rab->rab$w_rsz;
                rec_size = min(rec_size, 50);
                populate_descrip(&keystring, format_unprintable(rab->rab$l_ubf, rec_size), rec_size);
                lib$put_output(&keystring);

                if (fab->fab$b_org == FAB$C_IDX)
                {
                    struct  XABSUM
                            *xabsum;
                    struct  XABKEY
                            *keyptr;
                    char    *keyvalue,
                            *p;
                    __int32 lensum,
                            seglen,
                            k;

#undef xab$w_pos
#undef xab$b_siz
#define xab$w_pos xab$r_pos_overlay.xab$w_pos
#define xab$b_siz xab$r_siz_overlay.xab$b_siz

                    xabsum = (struct XABSUM *)fab->fab$l_xab;
                    keyptr  = (struct XABKEY *)xabsum->xab$l_nxt;
                    for (k = 1; k <= xabsum->xab$b_nok; k++)
                    {
                        if (keyptr->xab$b_cod != XAB$C_KEY)
                            break;
                        keyvalue = malloc(keyptr->xab$b_tks + 1);
                        keyvalue[keyptr->xab$b_tks] = '\0';

                        p = keyvalue;
                        lensum = 0;
                        i = 0;
                        do
                        {
                            seglen = (keyptr->xab$b_siz[i] < (keyptr->xab$b_tks - lensum))
                            ? keyptr->xab$b_siz[i]
                            : (keyptr->xab$b_tks - lensum);
/*                          Work out segment size in case the sum of the segments
                            is greater than the whole. Yeah, right. This is VMS! */

                            if (keyptr->xab$w_pos[i] > fab->fab$w_mrs)
                                continue;
/*                          Skip this bit if it points beyond record.	*/

                            memcpy(p, rab->rab$l_ubf + keyptr->xab$w_pos[i], seglen);
                            lensum += seglen;
                            p += seglen;
                            ++i;
                        } while ((i < keyptr->xab$b_nsg) && (lensum < keyptr->xab$b_tks));

                        if (lensum < keyptr->xab$b_tks)
                            keyvalue[lensum] = '\0';
/*                      If we didn't fill it					*/

                        populate_descrip(&keystring, format_unprintable(keyvalue, lensum), lensum);
                        out_line.dsc$w_length = sizeof(out_line_b);
                        sys$fao(&keymask, &out_line.dsc$w_length, &out_line, k, &keystring);
                        lib$put_output(&out_line);

                        free(keyvalue);
                        keyptr = (struct XABKEY *)keyptr->xab$l_nxt;
                    }

                }
                fileclose(fab);
            }
            printf("\n");
        }

    }

}


/*
	Simple function to substitute unprintable characters
	in a string for dots. Returns a maximum length of
	79 bytes. This is for displaying undefined data; it's
	only ever going to be a "sample" of what's there, so
	79 bytes is a good as it gets!

*/
char *
format_unprintable(instring, len)
char    *instring;
uint32  len;
{
	if (len > 79)
	    len = 79;
	instring[len] = '\0';
	while (len)
	{
	    len--;
	    if (!isprint(instring[len]))
    		instring[len] = 46;
	};
	return instring;
}


/*

	Open a file read only.
	Return pointer to contiguous FAB+NAM+RAB+XABSUM+expanded_filename area.
	user buffer is preallocated to the max record size.
	A XABSUM block is allocated as part of the
	FAB+RAB allocation. A memory area large enough
	to hold XABKEY blocks for each key is allocated
	and hung off the XABSUM.

*/
static struct fabdef *
openinput(file_name, file_name_len)
char	*file_name;
char	file_name_len;
{

__int32	status;

struct fabdef   *fab;
struct rabdef	*rab;
struct namdef	*nam;
struct XABKEY	*xabkey;
struct XABSUM	*xabsum;

    fab = malloc(sizeof(struct fabdef) + sizeof(struct rabdef) + sizeof(struct namdef) + sizeof(struct XABSUM) + 255);
    rab = (struct rabdef *)(fab + 1);
    nam = (struct namdef *)(rab + 1);
    xabsum = (struct XABSUM *)(nam + 1);

    *fab = cc$rms_fab;
    *nam = cc$rms_nam;
    *rab = cc$rms_rab;
    *xabsum = cc$rms_xabsum;

    fab->fab$b_fac = FAB$M_GET;
    fab->fab$l_fna = file_name;
    fab->fab$b_fns = file_name_len;
    fab->fab$l_fop = 0;
    fab->fab$b_shr = -1 & (~FAB$M_NIL);
    fab->fab$b_rtv = 255;
    fab->fab$l_xab = (char *)xabsum;

    status = sys$open(fab, 0, 0);
    if (!(status & 1))
    {
    	free(fab);
    	return NULL;
    }

    if (fab->fab$b_org == FAB$C_IDX)
    {

/*      Allocate a chunk of space for all XAKBEY blocks for every key	*/
        ((struct XABSUM *)fab->fab$l_xab)->xab$l_nxt = malloc(sizeof(struct XABKEY) * ((struct XABSUM *)fab->fab$l_xab)->xab$b_nok);

        for (i = 1, xabkey = (struct XABKEY *)((struct XABSUM *)fab->fab$l_xab)->xab$l_nxt;
        i <= ((struct XABSUM *)fab->fab$l_xab)->xab$b_nok; 
        i++, xabkey = (struct XABKEY *)xabkey->xab$l_nxt)
        {
            *xabkey = cc$rms_xabkey;
            xabkey->xab$b_ref = i - 1;
            if (i < ((struct XABSUM *)fab->fab$l_xab)->xab$b_nok)
                xabkey->xab$l_nxt = (char *)(xabkey + 1);
        }
    }

    fab->fab$l_nam = nam;
    nam->nam$l_rsa = (char *)(xabsum + 1);
    nam->nam$b_rss = 255;
    nam->nam$b_nop = NAM$M_NOCONCEAL;

    sys$display(fab);

    rab->rab$l_fab = fab;
    rab->rab$b_mbf = 0;
    rab->rab$l_rop = RAB$M_ASY;
    status = sys$connect(rab, 0, 0);
    if (!(status & 1))
    {
    	free(((struct XABSUM *)fab->fab$l_xab)->xab$l_nxt);
    	free(fab);
    	return NULL;
    }

/*  Set up buffer to hold longest record, or 80 chars if it's a variable l length record */
    if (!fab->fab$w_mrs)
        fab->fab$w_mrs = 80;
    rab->rab$l_ubf = malloc(fab->fab$w_mrs + 1);
    rab->rab$l_ubf[fab->fab$w_mrs] = '\0';
    rab->rab$w_usz = fab->fab$w_mrs;

    return fab;
}


/*

	Read a file by key.

*/
static void readkey(struct rabdef *rab, char *key, int32 idx)
{

__int32	status;

    rab->rab$l_rop = RAB$M_NLK | RAB$M_RRL | RAB$M_NXR;
    rab->rab$b_krf = idx;
    rab->rab$b_rac = RAB$C_KEY;
    status = sys$get(rab);
    if (!(status & 1))
    	*rab->rab$l_ubf = '\0';

    return;
}


/*

	Read a file by RFA. An rfa has 2 parts, VBN and ID.

*/
static void
readrfa(rab, vbn, id)
struct rabdef	*rab;
__int32		vbn;
__int32		id;
{

__int32	status;

    rab->rab$l_rop = RAB$M_NLK | RAB$M_RRL;
    rab->rab$b_rac = RAB$C_RFA;
    rab->rab$l_rfa0 = vbn;
    rab->rab$w_rfa4 = id;
    status = sys$get(rab);
    if (!(status & 1))
    	*rab->rab$l_ubf = '\0';

    return;
}


/*

	Close a file that has been allocated with a contiguous
	FAB+RAB area, a preallocated user buffer, and a contiguous
	block of xab blocks after the initial XABSUM


*/
static void
fileclose(fab)
struct fabdef *fab;
{

__int32	status;
struct rabdef
		*rab;

    rab = (struct rabdef *)(fab + 1);
    status = sys$close(fab);
    if (!(status & 1))
        lib$stop(status);
    free(rab->rab$l_ubf);
    free(((struct XABSUM *)fab->fab$l_xab)->xab$l_nxt);
    free(fab);
}


static int
make_device_name_list(search_devnam)
struct dsc$descriptor *search_devnam;
{
    uint32  stat,
		        i,
		        context[] = {0,0},
		        devclass = DC$_DISK;
    __int16	retlen;

#pragma message disable ADDRCONSTEXT
    itemlst device_items[] = { (__int16)4, (__int16)DVS$_DEVCLASS, (void *)&devclass, (unsigned short *)0, terminator};
#pragma message enable ADDRCONSTEXT
    itemlst getdvi_items[] = { (__int16)MAXDEVLOCKNAM, (__int16)DVI$_DEVLOCKNAM, (void *)0, (unsigned short *)0, terminator};
    /*
    ** Build two arrays with DISK device names and their lock names.
    */

    no_of_disks = 0;

    for (i = 0; i < MAXDEVICE; i++)
    {
        devnam_desc.dsc$w_length = MAXDEVNAMLEN-1;
        devnam_desc.dsc$a_pointer = &disk_name_list[i][1];

        stat = sys$device_scan(&devnam_desc, &devnam_desc.dsc$w_length, 
            search_devnam, &device_items, &context);
        disk_name_list[i][0]=devnam_desc.dsc$w_length;
        if (stat & 1)
        {
            disk_name_list[i][devnam_desc.dsc$w_length + 1] = 0;    /* Null terminate */
            getdvi_items[0].bufadr = disk_lockname_list[i];
            getdvi_items[0].retlenadr = (unsigned short *)&retlen;

            stat = sys$getdviw(0, 0, &devnam_desc, &getdvi_items, 0, 0, 0, 0);
            if (!(stat & 1))
                lib$stop(stat);
            disk_lockname_list[i][retlen] = 0;
        }
        else if (stat == SS$_NOMOREDEV)
        {
            no_of_disks = i;
            break;
        }
        else
        {
            lib$stop(stat);
        }
    }
    return stat;
}
