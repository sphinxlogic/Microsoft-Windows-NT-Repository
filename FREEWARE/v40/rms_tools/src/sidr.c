/*
** sidr.c		Hein van den Heuvel, June 1993
**			(last mod, to reduce _BUG messages Apr/95)
**	Program to list sidr key values and some stats such as the 
**	highest number of duplicates per key for the selected file.
** 
** 	It can also DUMP per key sidr data buckets into fake indexed files.
**	This allows one to reduce data to analyze/ship and/or can be
**	used if there is a confidentiality issue with the main record data.
**
**	Potential uses:
**	- Aid to find out whether a convert is in order for secondary keys.
**	- Collect essential RMS support information for 'confidential' files.
**	- Provider of raw data to help guestimating buffer requirements.
**	- Quick & Dirty checklist to use alonside ANA/RMS/INT
**	- Framework to build tool to squish out deleted and ru_deleted records.
**	    (not currently implemented)
**
**	Usage: Define as external DCL command and pass filespec as parameter.
**	    $SIDR [-l] [-b=rfa.dat] indexed.dat
**	    	-l      Log each sidr record found
**	    	-k=n    Key number to use. Zero base. Default all.
**	    	-b=x.y  Binary SIDR & UDR pointers into file.
**              -d=x.y  Dump Data Buckets into file
*/
#define xab$m_initidx 16
#define xab$m_key_ncmpr 64
#define XAB$C_COL  8
#define XAB$C_DSTG 32
#include <rms>
#include <stdio>
#include <string>
#define DUMPEXTEND 10
#define MAXKEY 20
#define MAXREC 10
#define MAXKEYSIZ 40
#define LOG_FORMAT_STRING "sidr %4d, vbn %6d, dups %4d  %s\n"

struct IRC$R_FLG_BITS {
	unsigned irc$v_vnb_pointer_size: 2;
	unsigned irc$v_deleted : 1;
	unsigned irc$v_rrv : 1;
	unsigned irc$v_noptrsz : 1;
	unsigned irc$v_ru_delete : 1;
	unsigned irc$v_ru_update : 1;
	unsigned irc$v_first_key : 1;
	} ;

struct {char	check, area;
        short	sample, free, next_id;
	int	next;
	char	level, flags, sidr_data[63*512-15]; } bucket;

struct  {
	int	dups;
	int	chain;
	char	key[MAXKEYSIZ+1];
	    }	top_ten[MAXREC];

struct {
	int	sidr_vbn;
	short	udr_id;
	long	udr_vbn;
	struct IRC$R_FLG_BITS flags;
	    } bin;

int	binary=0, log=0, dump=0;
struct	RAB	rab, binrab;
struct	FAB	binfab;
int	sys$create(), sys$open(), sys$connect(), sys$close();
int	sys$read(), sys$write(), sys$put(), sys$extend(), sys$display();

void open_binary()
{
int stat;
binrab.rab$l_rbf = (void *) &bin;
binrab.rab$w_rsz = sizeof bin;
binrab.rab$l_rop = RAB$M_WBH;
binary = 0;
stat = sys$create ( &binfab);
if ( stat & 1 ) stat = sys$connect ( & binrab ) ;
if ( stat & 1 ) binary = 2;
}

void close_binary()
{
int stat;
binary = 0;
stat = sys$close ( &binfab);
if ( stat & 1 ) binary = 1;
}

void format_key (char *keybuf, char *key, int type, 
		int size, int new_size, int old_size)
{
char current_char, *pos, *buf;
switch (type)
    {
    case XAB$C_STG:			/* string                           */
    case XAB$C_COL:
	pos = key;
	buf = keybuf + old_size;
	while (pos < key + new_size) 
		{
		current_char = *pos++;
		if (current_char < ' ') current_char = '.';
		*buf++ = current_char;
		}
	while (buf < keybuf + size) *buf++ = current_char;
	*buf = 0;
	break;		
    case XAB$C_IN2:			/* signed 15 bit integer (2 bytes)  */
	sprintf( keybuf, "%5d", *((short *) key) );
	break;
    case XAB$C_BN2:			/* 2 byte binary                    */
	sprintf( keybuf, "%4X", *((short *) key) );
	break;
    case XAB$C_IN4:			/* signed 31 bit integer (4 bytes)  */
	sprintf( keybuf, "%10d", *((int *) key) );
	break;
    case XAB$C_BN4:			/* 4 byte binary                    */
	sprintf( keybuf, "%08X", *((int *) key) );
	break;
    case XAB$C_PAC:			/* packed decimal (1-16 bytes)      */
	{
	unsigned char *p;
	char *k;
	int i;
	k = keybuf;
	p = (void *) key;
	for (i = 0; i < size ; i++)
	    {
	    *k++ = (*p >> 4) + '0';
	    *k++ = (*p & 15) + '0';
	    p++;
	    }
	k--;
	if ((*k == '=') || (*k == ';')) 
	    {
	    *k = '-';
	    } else {
	    *k = '+';
	    }
	}
	break;
    case XAB$C_IN8:			/* signed 63 bit integer (4 bytes)  */
    case XAB$C_BN8:			/* 8 byte binary                    */
	{
	int (*p)[2];
	p = (void *) key;
	sprintf( keybuf, "%08X %08X", (*p)[0], (*p)[1]);
	break;
	}
    }
return;
}

void score_previous_key ( int dup_count, int chain_count, char *save_key )
{
int score, i;
/*
** found a new dupplicate count key value. Process it.
*/
if (dup_count > top_ten[MAXREC-1].dups)
    {
    /* 
    ** Higher than lowest high score, gotta have a place for it in top ten.
    */
    for (score=0; dup_count <= top_ten[score].dups; score++); 

    /*
    ** Make room moving down earlier entries.
    */
    for (i=MAXREC-1; i > score ; i--) top_ten[i] = top_ten[i-1]; 

    /*
    ** Move in current value.
    */
    top_ten[score].dups = dup_count;
    top_ten[score].chain = chain_count;
    strncpy ( top_ten[score].key, save_key, MAXKEYSIZ);
    }
}

int walk_sidr_buckets (int start_vbn, int key_type, 
		    int max_key_size, int max_buckets, int compression)
{
int		i, stat, vbn, buckets=0, dup_count=0, sidr_records=0;
int		udr_records=0, ru_updated=0, ru_deleted=0, deleted=0;
int		key_new_size, key_old_size=0, chain_count = 0;
char		*sidr, *next_sidr, *key;
char		save_key[255+1];

for (i=0; i<MAXREC; i++) top_ten[i].dups = 0;

key_new_size = max_key_size;
rab.rab$l_bkt = start_vbn;
stat = sys$read ( &rab) ;

/*
** Walk sidr records while 
**	- RMS status is success and 
**	- NOT last bucket in chain.
*/
while ((stat & 1) && rab.rab$l_bkt )			 /* bucket loop */
    {
    buckets++;
    vbn = rab.rab$l_bkt;
    for (sidr =  (void *) &bucket.sidr_data;		/* sidr loop */
	 sidr <  (char *) &bucket + bucket.free;
	 sidr =  next_sidr)
	{
	if (binary == 1) open_binary();
	sidr_records++;
	next_sidr = sidr + *(short *)sidr + sizeof (short);
	sidr += sizeof (short);
	key_new_size = max_key_size;
	if (compression) 
	    {
	    key_new_size = *sidr++;
	    key_old_size = *sidr++;
	    };
	key = sidr;
	sidr += key_new_size;

	if (log && dup_count) printf ( LOG_FORMAT_STRING, 
		    sidr_records,  vbn , dup_count, save_key);

	if (!( (struct IRC$R_FLG_BITS *) sidr )->irc$v_first_key) chain_count++;

	while ( sidr < next_sidr )		/* pointer loop */
	    {
	    struct IRC$R_FLG_BITS irc_flags;
	    irc_flags = * (struct IRC$R_FLG_BITS *) sidr++; /* grab and skip */

	    if (irc_flags.irc$v_first_key)
		{
		/* 
		** Have a key marked as first. Finish last key first
		*/
		if (dup_count > 1) 
		    score_previous_key ( dup_count, chain_count, save_key );

		/*
		** prepare for this time round
		*/
		dup_count = 0;
		chain_count = 1;
		format_key ( save_key, key, key_type, 
			      max_key_size, key_new_size, key_old_size );
		} /* first key */

	    dup_count++;
	    if (irc_flags.irc$v_ru_update) 
		 {
		 ru_updated++ ;
		 }
	    else if (irc_flags.irc$v_ru_delete) 
		 {
		 ru_deleted++ ;
		 }
	    else if (irc_flags.irc$v_deleted) 
		 {
		 deleted++;
		 }
	    else udr_records++;

	    
	    if (!(irc_flags.irc$v_noptrsz)) /* skip past pointer	*/
		{
		char *udr_rfa;
		int rfa_size;
		rfa_size = 4 + irc_flags.irc$v_vnb_pointer_size;
		if (binary)
		    {
		    bin.sidr_vbn = vbn;
		    bin.udr_vbn = 0;
		    udr_rfa = (void *) &bin.udr_id;
		    while (rfa_size--) *udr_rfa++ = *sidr++;
		    bin.flags = irc_flags;
		    sys$put (&binrab);
		    }
		else
		    sidr += rfa_size;
		}
	    }				/* while more pointers in sidr  */
	}				/* while more sidrs in bucket   */

    /*
    ** Set up to read next bucket and Sanity check.
    */
    rab.rab$l_bkt = bucket.next;
    if (bucket.flags & 1)
	{
	if (rab.rab$l_bkt != start_vbn) 
	    {
	    printf ("VBN %d marked LAST points to %d instead of %d (start).\n", 
		vbn, rab.rab$l_bkt, start_vbn);
	    stat = RMS$_BUG;
	    }
	rab.rab$l_bkt = 0;	    /* flag as done */
	}
    else if (buckets > max_buckets) 
	{
	printf ("VBN %d. LOOP! More buckets read than in the file\n", vbn );
	stat = RMS$_BUG;
	}
    else if (rab.rab$l_bkt == start_vbn) 
	{
	printf ("VBN %d. LOOP! Pointing to %d (start).\n", vbn, start_vbn );
	stat = RMS$_BUG;
	}
    else stat = sys$read ( &rab);
    }					/* while not last bucket in chain */

if (binary) close_binary();
if (sidr_records > 0)
    {
    /*
    **	Print result for this key after processing last values.
    */
    if (log && dup_count) printf ( LOG_FORMAT_STRING, 
		    sidr_records,  rab.rab$l_bkt, dup_count, save_key);
    if (dup_count > 1) score_previous_key ( dup_count , chain_count, save_key );
    printf ("\nFound %d sidr records in %d buckets\n", sidr_records, buckets);
    printf ("There were %d user data record pointers.\n", udr_records);
    printf ("There were %d deleted, %d RU_delete and %d RU_update pointers.\n",
	    deleted, ru_deleted, ru_updated);
        
    if (top_ten[0].dups) 
	{
        printf ("\n  Top Ten Table of Sidrs with more than 1 duplicate\n");
        printf ("\n  Duplicate count, Buckets, Key value" );
	printf ("\n--------------------------------------------------------\n");

	for (i=0;  i<MAXREC;  i++)
	    if (top_ten[i].dups) 
		printf ("  %8d         %3d      %s\n", 
		    top_ten[i].dups, top_ten[i].chain, top_ten[i].key);
	}
    else
	printf ( "No duplicate key values found.\n");
    }
else 
    {
    printf ( "No alternate keys found !\n");
    }    
return stat;
}

int dump_buckets (int start_vbn, int bucket_size, int max_buckets)
{
int		i, stat, buckets=0;

/*
** Create Dump file and copy prologue to help ana/rms a little 
*/

binfab.fab$l_alq = bucket_size * DUMPEXTEND ;
binfab.fab$b_fac = FAB$M_BIO;
binfab.fab$b_org = FAB$C_IDX;
binrab.rab$l_rbf = (char *) &bucket;
stat = sys$create ( &binfab);
if (!( stat & 1 )) return stat;
stat = sys$connect ( & binrab ) ;
if (!( stat & 1 )) return stat;

rab.rab$l_bkt = 1;
stat = sys$read ( &rab) ;
if (!( stat & 1 )) return stat;
binrab.rab$w_rsz = rab.rab$w_rsz;
stat = sys$write ( &binrab );
if (!( stat & 1 )) return stat;

rab.rab$l_bkt = start_vbn;
rab.rab$w_usz = bucket_size * 512;
binrab.rab$w_rsz = bucket_size * 512;
stat = sys$read ( &rab);	    /* read first bucket */

/*
** Now walk buckets while 
**	- RMS status is success and 
**	- NOT last bucket in chain.
*/
while ((stat & 1) && rab.rab$l_bkt )			 /* bucket loop */
    {
    buckets++;
    if (!(buckets % DUMPEXTEND)) 
	{
    	stat = sys$extend ( &binfab);	    
    	if (!( stat & 1 )) return stat;
	}
		
    stat = sys$write ( &binrab);	    /* copy */
    if (!( stat & 1 )) return stat;

    /*
    ** Set up to read next bucket and Sanity check.
    */
    rab.rab$l_bkt = bucket.next;
    if (bucket.flags & 1)
	{
	if (rab.rab$l_bkt != start_vbn) stat = RMS$_BUG;
	rab.rab$l_bkt = 0;
	}
    else
	{		
	if ((buckets > max_buckets) || (rab.rab$l_bkt == start_vbn)) 
	    {
	    stat = RMS$_BUG;
	    }
	else
	    stat = sys$read ( &rab);
	}
    }					/* while not last bucket in chain */

return sys$close ( &binfab);
}

main (int argc, char *argv[])
{
struct FAB	fab;
struct XABSUM	sum;
struct XABKEY	key[MAXKEY], *alt;
int		i, key_number = 0, stat, key_type, key_size , key_compression;
int		max_buckets;
char		c, *arg;

binrab = cc$rms_rab;
binfab = cc$rms_fab;
binrab.rab$l_fab = &binfab;

while (--argc > 0 &&  **++argv == '-' )
    { 
    arg = *argv;
    switch (*++arg) {
    case 'l':
	log = 1;
	break;
    case 'k':
	c = *++arg;
	if ((c != '=') && (c != ':'))
	    {
	    argc = 0;
	    } 
	else sscanf( ++arg, "%d", &key_number);
	break;
    case 'b':
	c = *++arg;
	if ((c != '=') && (c != ':'))
	    {
	    argc = 0;
	    } 
	binary = 1;
	binfab.fab$l_fna = ++arg;
	binfab.fab$b_fns = strlen ( arg );
	break;
    case 'd':
	c = *++arg;
	if ((c != '=') && (c != ':'))
	    {
	    argc = 0;
	    } 
	dump = 1;
	binfab.fab$l_fna = ++arg;
	binfab.fab$b_fns = strlen ( arg );
	break;
    default:
	printf ("SIDR: Illegal option -%c.\n", *arg);
	argc = 0;
	break;
	}
    }
if ( argc != 1 ) 
	    {
	    printf ("Usage: $SIDR [-l] [-b=rfa.dat] indexed.dat\n");
	    printf ("	-l      Log each sidr record found.\n");
	    printf ("	-k=n    Key number to use. Zero base. Default all.\n");
	    printf ("	-b=x.y  Binary SIDR & UDR pointers into file.\n");
	    printf ("	-d=x.y  Binary data buckets into file.\n");
	    return 268435456; /* trivia... go figure */
	    }
/*
** Fill in the fab, rab and keyxab
*/
fab = cc$rms_fab;
rab = cc$rms_rab;
key[0] = cc$rms_xabkey;
sum = cc$rms_xabsum;
fab.fab$l_fna = *argv;
fab.fab$b_fns = strlen( *argv );
fab.fab$b_shr = FAB$M_UPI | FAB$M_GET | FAB$M_PUT;
fab.fab$b_fac = FAB$M_GET | FAB$M_BRO ;
fab.fab$l_xab = (void *) &key[0];
rab.rab$l_fab = (void *) &fab;
rab.rab$l_ubf = (void *) &bucket;
rab.rab$l_rop = RAB$M_BIO;
key[0].xab$l_nxt = (void *) &sum;

/*
** Open file, which fills in XABSUM or XABKEY and call work routine.
*/
stat = sys$open ( &fab );
if (!(stat & 1 ))		return stat;
if (fab.fab$b_org != FAB$C_IDX)	return RMS$_ORG;
if (key[0].xab$b_prolog != 3)	return RMS$_PLV; 
if (sum.xab$b_nok > MAXKEY) sum.xab$b_nok = MAXKEY;
for (i = 1; i < sum.xab$b_nok; i++)
    {
    key[i] = cc$rms_xabkey;
    key[i].xab$b_ref = i;
    key[i-1].xab$l_nxt = (void *) &key[i];
    }

/*
** Ask RMS to fill in the KEY and AREA XABs hooked off the FAB.
** We need the position, size and type for nice display.
** (Tried to use just 1 xabkey and re-display in the key loop,
** but RMS refuses to fill in XABKEY after block mode access?!)
*/
stat = sys$display ( &fab );
if (stat & 1) stat = sys$connect ( &rab );	
if (!(stat & 1 ))		return stat;

if (key_number) 
    {
    if (key_number >= sum.xab$b_nok) return RMS$_KRF;
    sum.xab$b_nok = key_number + 1;	    /* force just 1 pass true loop */
    }
else 
    {
    if (dump) 
	{
	printf (" Usage error. Must specify key for dump.\n");
	}
    else
	key_number = 1;
    }

for ( ; (key_number < sum.xab$b_nok) && (stat & 1) ; key_number++ )
    {
    alt = &key[key_number];
    key_type = alt->xab$b_dtp & ~(-XAB$C_DSTG); /* use corresponding ascending type */
    key_size = alt->xab$b_tks;
    key_compression  = 2 ;
    max_buckets = fab.fab$l_alq / alt->xab$b_dbs ;
    if ( alt->xab$b_flg & xab$m_key_ncmpr ) key_compression = 0;
    /*    if (alt->xab$v_dup) */
    if (dump) 
	{
	rab.rab$w_usz = (key[0].xab$l_dvb - 1) * 512; /* prologue size */
	stat = 	dump_buckets (  alt->xab$l_dvb, alt->xab$b_dbs, max_buckets);
	}
    else if ( alt->xab$b_flg & xab$m_initidx ) {
	printf ( "\n Key %d NOT Initialized.\n", key_number);
	} else {
	    printf ( "\n Key %d \n", key_number);
	    rab.rab$w_usz = alt->xab$b_dbs * 512;
	    stat = 	walk_sidr_buckets (alt->xab$l_dvb, key_type, key_size, 
	    				    max_buckets, key_compression);
	}
    }    
return stat;
}
