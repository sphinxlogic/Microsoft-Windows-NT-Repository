/*
 * FAST_IO_COPY.C - disk file copy Fast I/O example.
 * This program requires OpenVMS V7.0 (VAX or Alpha).
 * The DEC C V5.2 (or newer) compiler is preferred.
 */
#include <atrdef.h>
#include <descrip.h>

#if __ALPHA
#include <fatdef.h> /* +sys$library:sys$lib_c.tlb/library */
#else
#include "fatdef.h" /* extract this header for VAX from an Alpha system */
#endif

#include <fibdef.h>
#include <iodef.h>
#include <iosadef.h>
#include <lib$routines.h>
#include <rms.h>
#include <ssdef.h>
#include <starlet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NBYTES_PER_BLOCK 512
#define MAX_BUFFERS 16
#define MAX_BLOCKS_PER_XFER 127
#define MAX_BYTES_PER_XFER (NBYTES_PER_BLOCK*MAX_BLOCKS_PER_XFER)
#define MAX_BYTES_PER_BUFFER (NBYTES_PER_BLOCK+MAX_BYTES_PER_XFER)

#pragma extern_model save
#pragma extern_model common_block shr
#pragma nostandard
extern volatile _align (page) char buffer[MAX_BUFFERS][MAX_BYTES_PER_BUFFER];
#pragma standard
extern volatile fandle ifan[MAX_BUFFERS];
extern volatile fandle ofan[MAX_BUFFERS];
extern volatile int iohan[MAX_BUFFERS][2];
extern struct FAB ifab;
extern struct NAM inam;
extern char iexpand[NAM$C_MAXRSS+1];
extern char iresult[NAM$C_MAXRSS+1];
extern struct FAB ofab;
extern struct NAM onam;
extern char oexpand[NAM$C_MAXRSS+1];
extern char oresult[NAM$C_MAXRSS+1];
extern struct XABFHC xabfhc;
extern struct XABSUM xabsum;
extern struct XABALL xaball[255];
extern struct XABKEY xabkey[255];
extern char knm[255][32];
extern volatile int ivbn;
extern volatile int iefn;
extern volatile int nreads_outstanding;
extern volatile int nwrites_outstanding;
extern volatile int max_reads_outstanding;
extern volatile int max_writes_outstanding;
extern volatile int nbuffers;
extern volatile int nblocks_per_xfer;
extern volatile int nbytes_per_xfer;
extern volatile int nbytes_per_buffer;
#pragma extern_model restore

void read_completion_ast(iosa *piosa)
{
  register int status = piosa->iosa$l_status;
  register int nbytes = piosa->iosa$ih_count;
  register int ibuf   = piosa->iosa$q_context_q[0];

  if (nreads_outstanding > max_reads_outstanding)
    max_reads_outstanding = nreads_outstanding;

  nreads_outstanding--;

  if ((status != SS$_ENDOFFILE) && !(status & 1))
  {
    int msgvec[2] = {1,0};
    msgvec[1] = status;
    printf("Could not perform input from VBN %0d to buffer #%0d.\n",
	piosa->iosa$q_context_q[1], ibuf);
    sys$putmsg(msgvec,0,0,0);
    goto read_return;
  }

  if (nbytes)
  {
    ifab.fab$l_ctx++;

    status = sys$io_perform(ofan[ibuf], ofab.fab$l_stv,
	&buffer[ibuf][0], &buffer[ibuf][NBYTES_PER_BLOCK],
	nbytes, piosa->iosa$q_context_q[1]);
    if (!(status & 1))
    {
      int msgvec[2] = {1,0};
      msgvec[1] = status;
      printf(
	"Could not start output (%0d bytes) to VBN %0d from buffer #%0d.\n",
	nbytes, piosa->iosa$q_context_q[1], ibuf);
      sys$putmsg(msgvec,0,0,0);
      goto read_return;
    }
    nwrites_outstanding++;
  }

read_return:
  if ((nreads_outstanding == 0) && (nwrites_outstanding == 0))
    sys$setef(iefn);
  return;
} /* read_completion_ast */

void write_completion_ast(iosa *piosa)
{
  register int status = piosa->iosa$l_status;
  register int nbytes = piosa->iosa$ih_count;
  register int ibuf   = piosa->iosa$q_context_q[0];

  if (nwrites_outstanding > max_writes_outstanding)
    max_writes_outstanding = nwrites_outstanding;

  nwrites_outstanding--;

  if (!(status & 1))
  {
    int msgvec[2] = {1,0};
    msgvec[1] = status;
    printf(
	"Could not perform output (%0d bytes) to VBN %0d from buffer #%0d.\n",
	nbytes, piosa->iosa$q_context_q[1], ibuf);
    sys$putmsg(msgvec,0,0,0);
    goto write_return;
  }
  ofab.fab$l_ctx++;

  if (ivbn <= ifab.fab$l_alq)
  {
    piosa->iosa$ih_count = nbytes_per_xfer;
    piosa->iosa$q_context_q[1] = ivbn;
    status = sys$io_perform(ifan[ibuf], ifab.fab$l_stv,
	&buffer[ibuf][0], &buffer[ibuf][NBYTES_PER_BLOCK],
	nbytes_per_xfer, ivbn);
    if (!(status & 1))
    {
      int msgvec[2] = {1,0};
      msgvec[1] = status;
      printf(
	"Could not start input from VBN %0d to buffer #%0d.\n",
	ivbn, ibuf);
      sys$putmsg(msgvec,0,0,0);
      goto write_return;
    }
    ivbn += nblocks_per_xfer;
    nreads_outstanding++;
  }

write_return:
  if ((nreads_outstanding == 0) && (nwrites_outstanding == 0))
    sys$setef(iefn);
  return;
} /* write_completion_ast */

int fast_io_copy(int argc, char *argv[])
#pragma nostandard
main_program
#pragma standard
{
  register iosa *piosa = 0;
  register int status = 0;
  register int ibuf = 0;
  volatile char *inadr[2] = {0,0};
  char *retadr[2] = {0,0};
  unsigned short int iosb[4];
  struct atrdef atrlst[2];
  struct fibdef fib;
  struct dsc$descriptor_s fibdsc;
  FAT fat;

/*
 * Require input and output file specifications.
 */
  if (argc < 3)
  {
    printf("Usage: %s <input-file> <output-file>\n", argv[0]);
    return 1;
  }

/*
 * Allow additional arguments to change either
 * the buffer count or the transfer size in blocks.
 */
  nbuffers = MAX_BUFFERS;
  nblocks_per_xfer = MAX_BLOCKS_PER_XFER;

  for (ibuf=3; ibuf<argc; ibuf++)
  {
    if (!strncmp(argv[ibuf], "-nbuffers", 9))
    {
      ibuf++;
      if (ibuf < argc)
      {
	nbuffers = atoi(argv[ibuf]);
	if ((nbuffers == 0) || (nbuffers > MAX_BUFFERS))
	  nbuffers = MAX_BUFFERS;
	else if (nbuffers < 0)
	  nbuffers = 1;
      }
    }
    else if (!strncmp(argv[ibuf], "-nblocks", 8))
    {
      ibuf++;
      if (ibuf < argc)
      {
	nblocks_per_xfer = atoi(argv[ibuf]);
	if ((nblocks_per_xfer == 0) || (nblocks_per_xfer > MAX_BLOCKS_PER_XFER))
	  nblocks_per_xfer = MAX_BLOCKS_PER_XFER;
	else if (nblocks_per_xfer < 0)
	  nblocks_per_xfer = 1;
      }
    }
    else
    {
      printf("Unrecognized argument #%0d: \"%s\".\n", ibuf, argv[ibuf]);
    }
  }

  nbytes_per_xfer = NBYTES_PER_BLOCK * nblocks_per_xfer;
  nbytes_per_buffer = NBYTES_PER_BLOCK + nbytes_per_xfer;

  printf("Using %0d buffers, with %0d blocks (%0d bytes) per transfer.\n",
	nbuffers, nblocks_per_xfer, nbytes_per_xfer);

  for (ibuf=0; ibuf<nbuffers; ibuf++)
  {
    inadr[0] = &buffer[ibuf][0];
    inadr[1] = &buffer[ibuf][nbytes_per_buffer-1];

    status = sys$create_bufobj(inadr, retadr, 0, 0, iohan[ibuf]);
    if (!(status & 1))
    {
      printf("Could not create buffer object #%0d.\n", ibuf);
      return status;
    }

    status = sys$io_setup(IO$_READVBLK,
	iohan[ibuf], iohan[ibuf], &read_completion_ast, 0, &ifan[ibuf]);
    if (!(status & 1))
    {
      printf("Could not set up input for buffer #%0d.\n", ibuf);
      return status;
    }

    status = sys$io_setup(IO$_WRITEVBLK,
	iohan[ibuf], iohan[ibuf], &write_completion_ast, 0, &ofan[ibuf]);
    if (!(status & 1))
    {
      printf("Could not set up output for buffer #%0d.\n", ibuf);
      return status;
    }
  }

/*
 * Fill in two RMS extended attribute blocks.
 */
  bzero((char*)&xabfhc, sizeof(xabfhc));
  xabfhc.xab$b_cod = XAB$C_FHC;
  xabfhc.xab$b_bln = XAB$C_FHCLEN;
  xabfhc.xab$l_nxt = (char*)&xabsum;

  bzero((char*)&xabsum, sizeof(xabsum));
  xabsum.xab$b_cod = XAB$C_SUM;
  xabsum.xab$b_bln = XAB$C_SUMLEN;

/*
 * Fill in the input file RMS blocks.
 */
  bzero((char*)&ifab, sizeof(ifab));
  ifab.fab$b_bid = FAB$C_BID;
  ifab.fab$b_bln = FAB$C_BLN;
  ifab.fab$l_nam = &inam;
  ifab.fab$l_fop = FAB$M_NAM | FAB$M_UPI;
  ifab.fab$l_fna = argv[1];
  ifab.fab$b_fns = strlen(argv[1]);
  ifab.fab$l_dna = 0;
  ifab.fab$b_dns = 0;
  ifab.fab$b_fac = FAB$M_GET;
  ifab.fab$b_shr = FAB$M_SHRGET;

  bzero((char*)&inam, sizeof(inam));
  inam.nam$b_bid = NAM$C_BID;
  inam.nam$b_bln = NAM$C_BLN;
  inam.nam$l_esa = iexpand;
  inam.nam$b_ess = NAM$C_MAXRSS;
  inam.nam$l_rsa = iresult;
  inam.nam$b_rss = NAM$C_MAXRSS;

/*
 * Fill in the output file RMS blocks.
 */
  bzero((char*)&ofab, sizeof(ofab));
  ofab.fab$b_bid = FAB$C_BID;
  ofab.fab$b_bln = FAB$C_BLN;
  ofab.fab$l_nam = &onam;
  ofab.fab$l_fop = FAB$M_CBT | FAB$M_MXV | FAB$M_OFP;
  ofab.fab$l_fna = argv[2];
  ofab.fab$b_fns = strlen(argv[2]);
  ofab.fab$l_dna = 0;
  ofab.fab$b_dns = 0;
  ofab.fab$b_fac = FAB$M_BIO | FAB$M_PUT | FAB$M_TRN;
  ofab.fab$b_shr = FAB$M_NIL;
  ofab.fab$l_alq = ifab.fab$l_alq;
  ofab.fab$w_deq = ifab.fab$w_deq;
  ofab.fab$b_org = ifab.fab$b_org;
  ofab.fab$b_rat = ifab.fab$b_rat;
  ofab.fab$b_rfm = ifab.fab$b_rfm;
  ofab.fab$w_mrs = ifab.fab$w_mrs;
  ofab.fab$l_mrn = ifab.fab$l_mrn;
  ofab.fab$b_bks = ifab.fab$b_bks;
  ofab.fab$b_fsz = ifab.fab$b_fsz;
  ofab.fab$w_gbc = ifab.fab$w_gbc;

  bzero((char*)&onam, sizeof(onam));
  onam.nam$b_bid = NAM$C_BID;
  onam.nam$b_bln = NAM$C_BLN;
  onam.nam$l_esa = oexpand;
  onam.nam$b_ess = NAM$C_MAXRSS;
  onam.nam$l_rsa = oresult;
  onam.nam$b_rss = NAM$C_MAXRSS;
  onam.nam$l_rlf = &inam;

/*
 * Fill in the invariant data for truncating output files.
 */
  atrlst[0].atr$w_size = ATR$S_RECATTR;
  atrlst[0].atr$w_type = ATR$C_RECATTR;
#if __ALPHA
  atrlst[0].atr$l_addr = &fat;
#else
  atrlst[0].atr$l_addr = (unsigned int)&fat;
#endif

  atrlst[1].atr$w_size = 0;
  atrlst[1].atr$w_type = 0;
  atrlst[1].atr$l_addr = 0;

  fibdsc.dsc$w_length  = FIB$C_LENGTH;
  fibdsc.dsc$b_dtype   = DSC$K_DTYPE_T;
  fibdsc.dsc$b_class   = DSC$K_CLASS_S;
  fibdsc.dsc$a_pointer = (char*)&fib;

/*
 * Get an event flag.
 */
  lib$get_ef(&iefn);
  if (iefn < 0) iefn = 32;

/*
 * Parse the input file specification.
 */
  status = sys$parse(&ifab);
  if (!(status & 1))
  {
    unsigned int msgvec[3];
    msgvec[0] = 2;
    msgvec[1] = ifab.fab$l_sts;
    msgvec[2] = ifab.fab$l_stv;
    printf("Could not parse input file specification \"%s\".\n", argv[1]);
    sys$putmsg(msgvec,0,0,0);
    return status;
  }

/*
 * Search for the next input file.
 */
  iexpand[inam.nam$b_esl] = '\0';
  printf("Search: %s\n", iexpand);
  while ((status = sys$search(&ifab)) & 1)
  {
    iresult[inam.nam$b_rsl] = '\0';
    printf("Reading %s\n", iresult);

/*
 * Open the input file (with implicit display).
 */
    xabsum.xab$l_nxt = 0;
    ifab.fab$l_xab = (char*)&xabfhc;
    ifab.fab$l_ctx = 0;
    ifab.fab$l_fop = FAB$M_NAM | FAB$M_UPI;
    status = sys$open(&ifab);
    if (!(status & 1))
    {
      unsigned int msgvec[3];
      msgvec[0] = 2;
      msgvec[1] = ifab.fab$l_sts;
      msgvec[2] = ifab.fab$l_stv;
      printf("Could not open input file.\n");
      sys$putmsg(msgvec,0,0,0);
      continue;
    }

/*
 * Get all available information on file allocation and areas.
 */
    ifab.fab$l_xab = xabsum.xab$l_nxt = (char*)&xaball[0];
    for (ibuf=0; (ibuf<xabsum.xab$b_noa) || (ibuf == 0); ibuf++)
    {
      bzero((char*)&xaball[ibuf], sizeof(xaball[ibuf]));
      xaball[ibuf].xab$b_cod = XAB$C_ALL;
      xaball[ibuf].xab$b_bln = XAB$C_ALLLEN;
      xaball[ibuf].xab$b_aid = ibuf;
      xaball[ibuf].xab$l_nxt = (char*)&xaball[ibuf+1];
    }
    if (xabsum.xab$b_noa)
      xaball[xabsum.xab$b_noa-1].xab$l_nxt = 0;
    else
      xaball[0].xab$l_nxt = 0;

/*
 * If there are any indexes, get all of the information about them.
 */
    if (xabsum.xab$b_nok)
    {
      if (xabsum.xab$b_noa)
	xaball[xabsum.xab$b_noa-1].xab$l_nxt = (char*)&xabkey[0];
      else
	xaball[0].xab$l_nxt = (char*)&xabkey[0];

      for (ibuf=0; ibuf<xabsum.xab$b_nok; ibuf++)
      {
	bzero((char*)&xabkey[ibuf], sizeof(xabkey[ibuf]));
	xabkey[ibuf].xab$b_cod = XAB$C_KEY;
	xabkey[ibuf].xab$b_bln = XAB$C_KEYLEN;
	xabkey[ibuf].xab$b_ref = ibuf;
	xabkey[ibuf].xab$l_knm = knm[ibuf];
	xabkey[ibuf].xab$l_nxt = (char*)&xabkey[ibuf+1];
      }
      xabkey[xabsum.xab$b_nok-1].xab$l_nxt = 0;
    }

/*
 * Display the additional information about allocation, areas, indexes.
 */
    status = sys$display(&ifab);
    if (!(status & 1))
    {
      unsigned int msgvec[3];
      msgvec[0] = 2;
      msgvec[1] = ifab.fab$l_sts;
      msgvec[2] = ifab.fab$l_stv;
      printf("Could not display input file information.\n");
      sys$putmsg(msgvec,0,0,0);
      sys$close(&ifab);
      continue;
    }

/*
 * Close the input file.
 */
    ifab.fab$l_xab = 0;
    status = sys$close(&ifab);
    if (!(status & 1))
    {
      unsigned int msgvec[3];
      msgvec[0] = 2;
      msgvec[1] = ifab.fab$l_sts;
      msgvec[2] = ifab.fab$l_stv;
      printf("Could not close input file.\n");
      sys$putmsg(msgvec,0,0,0);
      continue;
    }

/*
 * Show area and index information, if any.
 */
    for (ibuf=0; ibuf<xabsum.xab$b_noa; ibuf++)
      printf("Area %03u; %0u blocks; %0u-block buckets.\n",
	xaball[ibuf].xab$b_aid,
	xaball[ibuf].xab$l_alq,
	xaball[ibuf].xab$b_bkz);

    for (ibuf=0; ibuf<xabsum.xab$b_nok; ibuf++)
      printf("Key  %03u; LAN %03u; IAN %03u; \"%.*s\".\n",
	ibuf, xabkey[ibuf].xab$b_lan, xabkey[ibuf].xab$b_ian, 32, knm[ibuf]);

/*
 * Re-open the input file, UFO this time (accessed on user-mode channel).
 */
    ifab.fab$l_fop |= FAB$M_UFO;
    status = sys$open(&ifab);
    if (!(status & 1))
    {
      unsigned int msgvec[3];
      msgvec[0] = 2;
      msgvec[1] = ifab.fab$l_sts;
      msgvec[2] = ifab.fab$l_stv;
      printf("Could not access input file on channel.\n");
      sys$putmsg(msgvec,0,0,0);
      continue;
    }

/*
 * Create the output file, using the RMS extended attribute blocks.
 */
    ofab.fab$l_xab = (char*)&xabfhc;
    ofab.fab$l_ctx = 0;
    ofab.fab$l_fop = FAB$M_CBT | FAB$M_MXV | FAB$M_OFP;
    ofab.fab$l_alq = ifab.fab$l_alq;
    ofab.fab$b_org = ifab.fab$b_org;
    ofab.fab$b_rat = ifab.fab$b_rat;
    ofab.fab$b_rfm = ifab.fab$b_rfm;
    ofab.fab$w_mrs = ifab.fab$w_mrs;
    status = sys$create(&ofab);
    if (!(status & 1))
    {
      unsigned int msgvec[3];
      msgvec[0] = 2;
      msgvec[1] = ofab.fab$l_sts;
      msgvec[2] = ofab.fab$l_stv;
      printf("Could not create output file.\n");
      sys$putmsg(msgvec,0,0,0);
      sys$close(&ifab);
      continue;
    }

/*
 * If the total allocation of the output file is less than that of the
 * input file, extend it to match the input file.  This can happen for
 * non-sequential files when the total blocks specifically allocated to
 * each area do not result in a large enough file.
 */
    ofab.fab$l_xab = 0;
    if (ofab.fab$l_alq < ifab.fab$l_alq)
    {
      printf("Extending output file allocation from %0u to %0u blocks.\n",
	ofab.fab$l_alq, ifab.fab$l_alq);
      ofab.fab$l_alq = ifab.fab$l_alq - ofab.fab$l_alq;
      status = sys$extend(&ofab);
      if (!(status & 1))
      {
	unsigned int msgvec[3];
	msgvec[0] = 2;
	msgvec[1] = ofab.fab$l_sts;
	msgvec[2] = ofab.fab$l_stv;
	printf("Could not extend output file.\n");
	sys$putmsg(msgvec,0,0,0);
	sys$close(&ifab);
	sys$close(&ofab);
	continue;
      }
    }

/*
 * Close the output file.
 */
    status = sys$close(&ofab);
    if (!(status & 1))
    {
      unsigned int msgvec[3];
      msgvec[0] = 2;
      msgvec[1] = ofab.fab$l_sts;
      msgvec[2] = ofab.fab$l_stv;
      printf("Could not close output file.\n");
      sys$putmsg(msgvec,0,0,0);
      sys$close(&ifab);
      continue;
    }

    oresult[onam.nam$b_rsl] = '\0';
    printf("Writing %s\n", oresult);

/*
 * Re-open the output file by NAM block and
 * UFO this time (accessed on user-mode channel).
 */
    ofab.fab$l_fop |= FAB$M_NAM | FAB$M_UFO;
    status = sys$open(&ofab);
    if (!(status & 1))
    {
      unsigned int msgvec[3];
      msgvec[0] = 2;
      msgvec[1] = ofab.fab$l_sts;
      msgvec[2] = ofab.fab$l_stv;
      printf("Could not access output file on channel.\n");
      sys$putmsg(msgvec,0,0,0);
      sys$close(&ifab);
      continue;
    }

/*
 * Set the completion event flag.  Clear outstanding I/O counts.
 */
    sys$setef(iefn);
    nreads_outstanding = 0;
    nwrites_outstanding = 0;
    max_reads_outstanding = 0;
    max_writes_outstanding = 0;

/*
 * Start as many as <nbuffers> read requests.
 */
    for (	ibuf=0, ivbn=1;
		((ibuf<nbuffers) && (ivbn <= ifab.fab$l_alq));
		ibuf++)
    {
      sys$setast(0);
      if (ivbn == 1)
      {
	sys$clref(iefn);
	lib$init_timer(0);
      }

      piosa = (iosa*)&buffer[ibuf][0];
      piosa->iosa$l_status = 1;
      piosa->iosa$ih_count = nbytes_per_xfer;
      piosa->iosa$q_context_q[0] = ibuf;
      piosa->iosa$q_context_q[1] = ivbn;

      status = sys$io_perform(ifan[ibuf], ifab.fab$l_stv,
	&buffer[ibuf][0], &buffer[ibuf][NBYTES_PER_BLOCK],
	nbytes_per_xfer, ivbn);
      if (!(status & 1))
      {
	sys$setast(1);
	printf("Could not start input from VBN %0d to buffer #%0d.\n",
		ivbn, ibuf);
	return status;
      }
      ivbn += nblocks_per_xfer;
      nreads_outstanding++;
      sys$setast(1);
    }

/*
 * Wait for the disk reads and writes to complete.
 */
    sys$waitfr(iefn);
    lib$show_timer(0);
    printf("     Total Fast I/O Reads: %0d; Writes: %0d.\n",
	ifab.fab$l_ctx, ofab.fab$l_ctx);
    printf("Maximum Outstanding Reads: %0d; Writes: %0d.\n",
	max_reads_outstanding, max_writes_outstanding);

/*
 * Deassign the input channel.
 */
    status = sys$dassgn(ifab.fab$l_stv);
    if (!(status & 1))
    {
      printf("Could not deassign input channel.\n");
      return status;
    }

/*
 * Completely re-initialize the file information block.  The file ID and
 * directory ID fields must be zeroed, or the XQP will attempt to truncate
 * and modify the attributes of the *original* or *previous* file, *not*
 * the file that is currently accessed on the channel.
 */
    bzero((char*)&fib, sizeof(fib));
    fib.fib$l_acctl = FIB$M_WRITETHRU;
    fib.fib$w_exctl = FIB$M_TRUNC;
    fib.fib$l_exsz  = 0;
    fib.fib$l_exvbn = xabfhc.xab$l_ebk;
    if (xabfhc.xab$w_ffb) fib.fib$l_exvbn++;
    fib.fib$w_verlimit = 0;
    fib.fib$l_acl_status = 0;

/*
 * Copy the file header characteristics to the file attributes.
 */
    fat.fat$b_rtype    = xabfhc.xab$b_rfo;
    fat.fat$b_rattrib  = xabfhc.xab$b_atr;
    fat.fat$w_rsize    = xabfhc.xab$w_lrl;
    fat.fat$w_hiblkl   = xabfhc.xab$w_hbk0; /* ignored */
    fat.fat$w_hiblkh   = xabfhc.xab$w_hbk2; /* ignored */
    fat.fat$w_efblkl   = xabfhc.xab$w_ebk0;
    fat.fat$w_efblkh   = xabfhc.xab$w_ebk2;
    fat.fat$w_ffbyte   = xabfhc.xab$w_ffb;
    fat.fat$b_bktsize  = xabfhc.xab$b_bkz;
    fat.fat$b_vfcsize  = xabfhc.xab$b_hsz;
    fat.fat$w_maxrec   = xabfhc.xab$w_mrz;
    fat.fat$w_defext   = xabfhc.xab$w_dxq;
    fat.fat$w_gbc      = xabfhc.xab$w_gbc;
    fat.fat$w_versions = xabfhc.xab$w_verlimit;

/*
 * Modify and truncate the file.
 */
    status = sys$qiow(iefn, ofab.fab$l_stv, IO$_MODIFY, iosb, 0, 0,
		&fibdsc, 0, 0, 0, &atrlst, 0);
    if (status & 1) status = iosb[0];
    if (status & 1)
    {
      printf("Output file truncated at VBN %0u by %0u blocks.\n",
		fib.fib$l_exvbn, fib.fib$l_exsz);
      printf("New end-of-file block is %0u, first free byte is %0u.\n",
		xabfhc.xab$l_ebk, xabfhc.xab$w_ffb);
    }
    else
    {
      printf("Could not truncate output file to EBK %0u, FFB %0u.\n",
		xabfhc.xab$l_ebk, xabfhc.xab$w_ffb);
    }

/*
 * Deassign the output channel.
 */
    status = sys$dassgn(ofab.fab$l_stv);
    if (!(status & 1))
    {
      printf("Could not deassign output channel.\n");
      return status;
    }
  } /* input file search loop */

  if ((status != RMS$_NMF) && !(status & 1))
  {
    unsigned int msgvec[3];
    msgvec[0] = 2;
    msgvec[1] = ifab.fab$l_sts;
    msgvec[2] = ifab.fab$l_stv;
    printf("Could not search for input file.\n");
    sys$putmsg(msgvec,0,0,0);
  }

/*
 * Clean up after <nbuffers> buffer objects.
 * This apparently gets done at image exit anyway.
 */
  for (ibuf=0; ibuf<nbuffers; ibuf++)
  {
    status = sys$io_cleanup(ifan[ibuf]);
    if (!(status & 1))
    {
      printf("Could not clean up input for buffer #%0d.\n", ibuf);
      return status;
    }

    status = sys$io_cleanup(ofan[ibuf]);
    if (!(status & 1))
    {
      printf("Could not clean up output for buffer #%0d.\n", ibuf);
      return status;
    }

    status = sys$delete_bufobj(iohan[ibuf]);
    if (!(status & 1))
    {
      printf("Could not delete buffer object #%0d.\n", ibuf);
      return status;
    }
  }
  return status;
} /* fast_io_copy() */
