/* VMS mapping of data and alloc arena for GNU Emacs.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.
   
   This file is part of GNU Emacs.

GNU Emacs is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU Emacs is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Emacs; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Written by Mukesh Prasad.  */

#ifdef VMS

#if 0
#define VMS_DEBUG
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <config.h>
#include "lisp.h"
#include "getpagesize.h"
#include "puresize.h"
#include <rab.h>
#include <fab.h>
#include <rmsdef.h>
#include <secdef.h>
#include <descrip.h>

/* RMS block size */
#define	BLOCKSIZE	512

/* Maximum number of bytes to be written in one RMS write.
 * Must be a multiple of BLOCKSIZE.
 */
#define	MAXWRITE	(BLOCKSIZE * 30)

/* This funniness is to ensure that sdata occurs alphabetically BEFORE the
   $DATA psect and that edata occurs after ALL Emacs psects.  This is
   because the VMS linker sorts all psects in a cluster alphabetically
   during the linking, unless you use the cluster_psect command.  Emacs
   uses the cluster command to group all Emacs psects into one cluster;
   this keeps the dumped data separate from any loaded libraries. */

#ifdef __GNUC__
/* We need a large sdata array because otherwise the impure storage will end up
   in low memory, and this will screw up garbage collection (Emacs will not
   be able to tell the difference between a string length and an address).
   This array guarantees that the impure storage is at a sufficiently high
   address so that this problem will not occur. */
char sdata[1] asm("_$$PsectAttributes_NOOVR$$$$$$DATA") = { 'x' };
char edata[1] asm("_$$PsectAttributes_NOOVR$$____DATA") = { 'x' };
#else
globaldef {"$$$$DATA"} char sdata[1] = { 'x' }; /* Start of saved data area */
globaldef {"____DATA"} char edata[1] = { 'x' }; /* End of saved data area */
#endif

/* It looks like the AXP/OpenVMS linker puts the psects like this:
	$DATA$	containing all initialized data.
	$CODE$	containing the code
	$BSS$	containing uninitialized data.
   This means we have to put two sections in the dump file. The sdata and
   edata up there find their way into the initialized part of the data.
   We thus need to add a second section to the dump file. Boy, will this
   look hairy! */

#ifndef __GNUC__
#ifdef __DECC
globaldef {"$$$$BSS"} char BSS_sdata[1]; /* Start of saved data area */
globaldef {"____BSS"} char BSS_edata[1]; /* End of saved data area */
#endif
#endif

static void fill_fab (), fill_rab ();
static int write_data ();

extern char *start_of_data ();
extern char *end_of_data ();
extern char *start_of_BSS ();
extern char *end_of_BSS ();
extern char *start_of_heap ();
extern char *end_of_heap ();
extern char *start_of_initial_brk_area ();
extern char *end_of_initial_brk_area ();
extern char *start_of_pure ();
extern char *end_of_pure ();

typedef char * (*prgptr1)();
static const prgptr1 data_routines1 [] =
{
  start_of_data, end_of_data,
#ifndef __GNUC__
#ifdef __DECC
  start_of_BSS, end_of_BSS,
#endif
#endif
  start_of_initial_brk_area, end_of_initial_brk_area,
  start_of_pure, end_of_pure,
  0, 0
};

#define FLAGS SEC$M_CRF | SEC$M_WRT
static int normal_mapin (int *inadr, int *retadr,
			 struct FAB *fab, int starting_block, unsigned long p1)
{
#ifdef VMS_DEBUG
  printf ("In normal_mapin:\n");
#endif
  return sys$crmpsc (inadr, retadr, 0, FLAGS, 0, 0, 0,
		     fab->fab$l_stv, 0,
		     starting_block, 0, 0);
}

#undef FLAGS
#define FLAGS (SEC$M_GBL | SEC$M_SYSGBL)
static int pure_mapin (int *inadr, int *retadr,
		       struct FAB *fab, int starting_block, unsigned long p1)
{
  $DESCRIPTOR (dsc_name, "EMACS_PURE");
  unsigned long ident[2];
  int status;

  ident[0] = SEC$K_MATEQU;
  ident[1] = p1;
#ifdef VMS_DEBUG
  printf ("In pure_mapin:\n");
  printf ("  ident = {%d, %d}\n", ident[0], ident[1]);
#endif
  status = sys$mgblsc (inadr, retadr, 0, FLAGS, &dsc_name, ident, 0);
#ifdef VMS_DEBUG
  printf ("sys$mgblsc returned status %%X%X\n\"%s\"\n", status,
	  strerror (EVMSERR, status));
#endif
  if (status & 1)
    return status;
  status = sys$crmpsc (inadr, retadr, 0, FLAGS, &dsc_name, ident, 0,
		       fab->fab$l_stv, 0,
		       starting_block, 0, 0);
#ifdef VMS_DEBUG
  printf ("sys$crmpsc returned status %%X%X\n\"%s\"\n", status,
	  strerror (EVMSERR, status));
#endif
  if (status & 1)
    return status;
  return normal_mapin (inadr, retadr, fab, starting_block, p1);
}

typedef int (*prgptr2)(int *inadr, int *retadr,
		       struct FAB *fab, int starting_block, unsigned long p1);
static const prgptr2 data_routines2 [] =
{
  normal_mapin,
#ifndef __GNUC__
#ifdef __DECC
  normal_mapin,
#endif
#endif
  normal_mapin,
  pure_mapin
};

/* Structure to write into first block of map file.
 */

union map_data
{
  struct
  {
    int nblocks;		/* Total # of mappings */
    unsigned long ident;	/* Major, minor Emacs version and
				   edit number */
  } first;
  struct
  {
    int  nblocks;		/* # of blocks in this section (incl. head) */
    char * sdata;		/* Start of data area */
    char * edata;		/* End of data area */
    int  nremain;		/* Number of sections after this one */
    int  seqnr;			/* Sequence number */
  } data;
};
#define MAX_MAPS	9

/* Finds the data according to the current session */
int
get_map_data (map_data, datasize, sum_blocks, cnt, data_routines)
     union map_data *map_data;
     int *datasize, *sum_blocks;
     prgptr1 *data_routines;
{
  int n_mappings, i;

  for (i = 0, n_mappings = 1,
	 *sum_blocks = ((sizeof (union map_data)*cnt + BLOCKSIZE - 1)
		       / BLOCKSIZE);
       i < cnt; i++)
    {
      if (data_routines[i*2] == 0)
	break;

      map_data[n_mappings].data.sdata = (*data_routines[i*2])();
      map_data[n_mappings].data.edata = (*data_routines[i*2+1])();
      map_data[n_mappings].data.seqnr = i;
#ifdef VMS_DEBUG
      {
	printf ("checking out this map_data (# %d):\n", n_mappings);
	printf ("  sdata: %p\n", map_data[n_mappings].data.sdata);
	printf ("  edata: %p\n", map_data[n_mappings].data.edata);
	printf ("  seqnr: %p\n", map_data[n_mappings].data.seqnr);
      }
#endif
      if (n_mappings > 1)
	{
	  int j;

	  for (j = n_mappings; j-- > 1;)
	    {
	      /* data and j are disjunct */
	      if (map_data[n_mappings].data.sdata >= map_data[j].data.edata
		  || map_data[n_mappings].data.edata <= map_data[j].data.sdata)
		{
#ifdef VMS_DEBUG
		  printf (" Not sharing space with map_data # %d!\n", j);
#endif
		  continue;
		}

	      /* data contains j */
	      if (map_data[n_mappings].data.sdata <= map_data[j].data.sdata
		  && map_data[n_mappings].data.edata >= map_data[j].data.edata)
		{
#ifdef VMS_DEBUG
		  printf (" moved to map_data # %d, which it contained\n",
			  j);
#endif
		  map_data[j].data.sdata = map_data[n_mappings].data.sdata;
		  map_data[j].data.edata = map_data[n_mappings].data.edata;
		}
	      else
#ifdef VMS_DEBUG
		printf (" part of map_data # %d, or equal to it\n", j);
#endif

	      /* For all other cases, just do nothing */
	      goto skip_this;
	    }
	}
      datasize[n_mappings] =
	map_data[n_mappings].data.edata - map_data[n_mappings].data.sdata + 1;
      map_data[n_mappings].data.nblocks =
	((datasize[n_mappings] + BLOCKSIZE - 1) / BLOCKSIZE);
      *sum_blocks += map_data[n_mappings].data.nblocks;
      n_mappings++;

  skip_this: ;
    }
  map_data[0].first.nblocks = --n_mappings;
#ifdef VMS_DEBUG
  printf ("Results:\n");
#endif
  for (i = 1; i <= n_mappings; i++)
    {
      map_data[i].data.nremain = n_mappings - i;
#ifdef VMS_DEBUG
      {
	printf ("map_data # %d (out of %d):\n", i, n_mappings);
	printf ("  nblocks: %d\n", map_data[i].data.nblocks);
	printf ("  sdata: %p\n", map_data[i].data.sdata);
	printf ("  edata: %p\n", map_data[i].data.edata);
	printf ("  nremain: %d\n", map_data[i].data.nremain);
	printf ("  seqnr: %d\n", map_data[i].data.seqnr);
      }
#endif
    }
#ifdef VMS_DEBUG
  printf ("----------\n");
#endif
  return 0;
}

/* Maps in the data and alloc area from the map file.
 */

int
mapin_data (name)
     char * name;
{
  struct FAB fab;
  struct RAB rab;
  int status, size;
  int inadr[2];
  union map_data current_map_data[MAX_MAPS+1];
  union map_data map_data[MAX_MAPS+1];
  int dummy1[MAX_MAPS+1], dummy2;
  int starting_block, i, seqnr;

  get_map_data (current_map_data, dummy1, &dummy2, MAX_MAPS, data_routines1);

  /* Open map file. */
  fab = cc$rms_fab;
  fab.fab$b_fac = FAB$M_BIO|FAB$M_GET;
  fab.fab$l_fna = name;
  fab.fab$b_fns = strlen (name);
  status = sys$open (&fab);
  if (status != RMS$_NORMAL)
    {
      printf ("Map file not available, running bare Emacs....\n");
      return 0;			/* Map file not available */
    }
  /* Connect the RAB block */
  rab = cc$rms_rab;
  rab.rab$l_fab = &fab;
  rab.rab$b_rac = RAB$C_SEQ;
  rab.rab$l_rop = RAB$M_BIO;
  status = sys$connect (&rab);
  if (status != RMS$_NORMAL)
    lib$stop (status);
  /* Read the header data */
  rab.rab$l_ubf = (char *) &map_data;
  rab.rab$w_usz = sizeof (map_data);
  rab.rab$l_bkt = 0;
  status = sys$read (&rab);
  if (status != RMS$_NORMAL)
    lib$stop (status);
  status = sys$close (&fab);
  if (status != RMS$_NORMAL)
    lib$stop (status);

#ifdef VMS_DEBUG
  printf ("Number of maps: %d\n", map_data[0].first.nblocks);
  printf ("Pure identity: %d (%d.%d.%d)\n", map_data[0].first.ident,
	  (map_data[0].first.ident >> 24) & 0xFF,
	  (map_data[0].first.ident >> 16) & 0xFF,
	  map_data[0].first.ident & 0xFFFF);
#endif

  for (i = 1,
	 starting_block = ((sizeof (map_data) + BLOCKSIZE - 1) / BLOCKSIZE) + 1;
       i <= map_data[0].first.nblocks; i++)
    {
#ifdef VMS_DEBUG
      {
	printf ("map_data # %d (out of %d):\n", i, map_data[0].first.nblocks);
	printf ("  nblocks: %d\n", map_data[i].data.nblocks);
	printf ("  sdata: %p\n", map_data[i].data.sdata);
	printf ("  edata: %p\n", map_data[i].data.edata);
	printf ("  nremain: %d\n", map_data[i].data.nremain);
	printf ("  seqnr: %d\n", map_data[i].data.seqnr);
      }
#endif
      if (map_data[i].data.sdata != current_map_data[i].data.sdata)
	  {
	    fprintf (stderr,
		     "Start of data area has moved: cannot map in data.");
	    fprintf (stderr,
		     "  (expected start = 0x%p,  current start = 0x%p\n",
		     map_data[i].data.sdata, current_map_data[i].data.sdata);
	    abort ();
	  }	    
      if (map_data[i].data.edata != current_map_data[i].data.edata)
	  {
	    fprintf (stderr,
		     "End of data area has moved: cannot map in data.");
	    fprintf (stderr,
		     "  (expected end = 0x%p,  current end = 0x%p\n",
		     map_data[i].data.edata, current_map_data[i].data.edata);
	    abort ();
	  }	    
      if (map_data[i].data.nremain != map_data[0].first.nblocks - i)
	{
	  static buf[512];
	  sprintf (buf, "Unexpected amount of remaining sections (%d, expected %d): cannot map in data.",
		   map_data[i].data.nremain, map_data[0].first.nblocks - i);
	  fprintf (stderr, buf);
	  abort ();
	}
      fab.fab$l_fop |= FAB$M_UFO;
      status = sys$open (&fab);
      if (status != RMS$_NORMAL)
	lib$stop (status);
      /* Map data area. */
      inadr[0] = (int) map_data[i].data.sdata;
      inadr[1] = (int) map_data[i].data.edata;
      seqnr = map_data[i].data.seqnr;
      {
	volatile int retadr[2], i;
#ifdef VMS_DEBUG
	printf ("The input address was (2 items): \n");
	for (i = 0; i < 2; i++)
	  printf (" %u", inadr[i]);
	printf ("\n");
#endif

#if 0
	status = sys$crmpsc (inadr, retadr, 0, FLAGS, 0, 0, 0,
			     fab.fab$l_stv, 0,
			     starting_block, 0, 0);
#else
	status = (*data_routines2[seqnr])(inadr, retadr, &fab,
					  starting_block,
					  map_data[0].first.ident);
#endif

#ifdef VMS_DEBUG
	printf ("The return address was (2 items): \n");
	for (i = 0; i < 2; i++)
	  printf (" %u", retadr[i]);
	printf ("\n");
#endif
	if (inadr[0] != retadr[0])
	  {
	    fprintf (stderr,
		     "Start of data area has moved: cannot map in data.");
	    fprintf (stderr,
		     "  (expected start = 0x%p,  resulting start = 0x%p\n",
		     inadr[0], retadr[0]);
	    abort ();
	  }	    
      }
      if (! (status & 1))
	lib$stop (status);
      starting_block += map_data[i].data.nblocks;
    }

  /* cooperate with VMSGMALLOC.C */
  {
#ifdef GNU_MALLOC
    void malloc_clear_hooks ();
#if 0
    void vms_clear_data ();
    vms_clear_data (1);
#endif
    malloc_clear_hooks ();
#endif
  }
}

/* Writes the data and alloc area to the map file.
 */
mapout_data (into, major, minor, edit)
     char * into;
     int major, minor, edit;
{
  struct FAB fab;
  struct RAB rab;
  int status, i;
  unsigned long n_mappings;
  union map_data map_data[MAX_MAPS+1];
  int datasize[MAX_MAPS+1], msize, sum_blocks, starting_block;
  extern int vms_malloc_overflow ();

  if (vms_malloc_overflow ())
    {
      static char buf[512];
      extern char * start_of_real_heap ();
      extern char * start_of_brk_area ();
      extern char * end_of_brk_area ();
      extern char * current_end_of_brk_area ();
      sprintf (buf, "Out of initial allocation.  Must rebuild Emacs with more memory (VMS_ALLOCATION_SIZE).\n(VMS_ALLOCATION_SIZE is currently %u, but should be %u)",
	       end_of_initial_brk_area () - start_of_initial_brk_area () + 1,
	       (unsigned int) end_of_brk_area ()
	       < (unsigned int) start_of_real_heap ()
	       ? current_end_of_brk_area () - start_of_brk_area () + 1
	       : (current_end_of_brk_area () - start_of_real_heap () + 1)
	       + (end_of_initial_brk_area () - start_of_initial_brk_area ()
		  + 1));
      error (buf);
      return 0;
    }

  get_map_data (map_data, datasize, &sum_blocks, MAX_MAPS, data_routines1);
  n_mappings = map_data[0].first.nblocks;
  map_data[0].first.ident = (major << 24) + (minor << 16) + edit;

#ifdef VMS_DEBUG
  printf ("Number of maps: %d\n", map_data[0].first.nblocks);
  printf ("Pure identity: %d (%d.%d.%d)\n", map_data[0].first.ident,
	  (map_data[0].first.ident >> 24) & 0xFF,
	  (map_data[0].first.ident >> 16) & 0xFF,
	  map_data[0].first.ident & 0xFFFF);
#endif

  /* Create map file. */
  fab = cc$rms_fab;
  fab.fab$b_fac = FAB$M_BIO|FAB$M_PUT;
  fab.fab$l_fna = into;
  fab.fab$b_fns = strlen (into);
  fab.fab$l_fop = FAB$M_CBT;
  fab.fab$b_org = FAB$C_SEQ;
  fab.fab$b_rat = 0;
  fab.fab$b_rfm = FAB$C_VAR;
  fab.fab$l_alq = sum_blocks;
  status = sys$create (&fab);
  if (status != RMS$_NORMAL)
    {
      error ("Could not create map file: %s", strerror (EVMSERR, status));
      return 0;
    }
  /* Connect the RAB block */
  rab = cc$rms_rab;
  rab.rab$l_fab = &fab;
  rab.rab$b_rac = RAB$C_SEQ;
  rab.rab$l_rop = RAB$M_BIO;
  status = sys$connect (&rab);
  if (status != RMS$_NORMAL)
    {
      error ("RMS connect to map file failed: %s", strerror (EVMSERR, status));
      return 0;
    }
  /* Write the header */
  rab.rab$l_rbf = (char *) map_data;
  rab.rab$w_rsz = sizeof (map_data);
  rab.rab$l_bkt = 0;
  status = sys$write (&rab);
  if (status != RMS$_NORMAL)
    {
      error ("RMS write (header) to map file failed: %s", strerror (EVMSERR, status));
      status = sys$close (&fab);
      if (status != RMS$_NORMAL)
	error ("RMS close on map file failed: %s", strerror (EVMSERR, status));
      return 0;
    }
  for (i = 1,
	 starting_block = ((sizeof (map_data) + BLOCKSIZE - 1) / BLOCKSIZE) + 1;
       i <= n_mappings; i++)
    {
      if (! write_data (&rab, starting_block, map_data[i].data.sdata, datasize[i]))
	{
	  status = sys$close (&fab);
	  if (status != RMS$_NORMAL)
	    error ("RMS close on map file failed: %s", strerror (EVMSERR, status));
	  return 0;
	}
      starting_block += map_data[i].data.nblocks;
    }
  status = sys$close (&fab);
  if (status != RMS$_NORMAL)
    {
      error ("RMS close on map file failed: %s", strerror (EVMSERR, status));
      return 0;
    }
  return 1;
}

static int
write_data (rab, firstblock, data, length)
     struct RAB * rab;
     char * data;
{
  int status;
  int cnt = 0;
  int total = length;
  
  rab->rab$l_bkt = firstblock;
  while (length > 0)
    {
      rab->rab$l_rbf = data;
      rab->rab$w_rsz = length > MAXWRITE ? MAXWRITE : length;
      status = sys$write (rab, 0, 0);
      if (status != RMS$_NORMAL)
	{
	  fprintf (stderr,
		   "RMS write to map file failed with status %%X0%X (below) (data = %%X0%X, %d bytes chunk #%d, %d bytes out of %d left to write)",
		   status, data,
		   MAXWRITE, cnt, length, total);
	  lib$signal (status);
	  return 0;
	}
      data = &data[MAXWRITE];
      cnt++;
      length -= MAXWRITE;
      rab->rab$l_bkt = 0;
    }
  return 1;
}				/* write_data */

/* The following code should probably really reside in sysdep.c, but there's a
   bug in the DEC C v1.3 compiler, concerning globaldef and globalref */

#define FIRST_PAGE_BYTE(p,ps) \
  (((unsigned long) (p)) & ~((ps) - 1))

#define LAST_PAGE_BYTE(p,ps) \
  ((((unsigned long) (p)) & ~((ps) - 1)) + ((ps) - 1))

/*
 *	Return the address of the start of the data segment prior to
 *	doing a memory dump.
 */
 
char *
start_of_data ()
{
#ifdef VMS_DEBUG
  printf ("Start of data : %%X0%X (page size = %d)\n",
	  FIRST_PAGE_BYTE ((char *) &sdata, getpagesize ()), getpagesize ());
  printf ("%p & %x (~(getpagesize () - 1)) = %p\n",
	  (char *) &edata, ~(getpagesize () - 1),
	  ((unsigned long)((char *) &sdata)) & ~(getpagesize () - 1));
#endif

  /* We arrange for this to always start on a CPU-specific page boundary.  */
  return FIRST_PAGE_BYTE ((char *) &sdata, getpagesize ());
}

/*
 *	Return the address of the end of the data segment prior to
 *	doing a memory dump.
 */

char *
end_of_data ()
{
#ifdef VMS_DEBUG
  printf ("End of data : %%X0%X (page size = %d)\n",
	  LAST_PAGE_BYTE ((char *) &edata, getpagesize ()), getpagesize ());
  printf ("%p & %x (~(getpagesize () - 1)) = %p\n",
	  (char *) &edata, ~(getpagesize () - 1),
	  ((unsigned long)((char *) &edata)) & ~(getpagesize () - 1));
#endif

  /* We return the last byte of the last page. This works on VAX/VMS,
     and is required on Alpha/VMS.  */
  return LAST_PAGE_BYTE ((char *) &edata, getpagesize ());
}

#ifndef __GNUC__
#ifdef __DECC
/* The two following are the same as the two above, but for the uninitialized
   variables */

char *
start_of_BSS ()
{
#ifdef VMS_DEBUG
  printf ("Start of BSS : %%X0%X (page size = %d)\n",
	  FIRST_PAGE_BYTE ((char *) &BSS_sdata, getpagesize ()),
	  getpagesize ());
  printf ("%p & %x (~(getpagesize () - 1)) = %p\n",
	  (char *) &BSS_sdata, ~(getpagesize () - 1),
	  ((unsigned long)((char *) &BSS_sdata)) & ~(getpagesize () - 1));
#endif

  /* We arrange for this to always start on a CPU-specific page boundary.  */
  return FIRST_PAGE_BYTE ((char *) &BSS_sdata, getpagesize ());
}

char *
end_of_BSS ()
{
#ifdef VMS_DEBUG
  printf ("End of BSS : %%X0%X (page size = %d)\n",
	  LAST_PAGE_BYTE ((char *) &BSS_edata, getpagesize ()), getpagesize ());
  printf ("%p & %x (~(getpagesize () - 1)) = %p\n",
	  (char *) &BSS_edata, ~(getpagesize () - 1),
	  ((unsigned long)((char *) &BSS_edata)) & ~(getpagesize () - 1));
#endif

  /* We return the last byte of the last page. This works on VAX/VMS,
     and is required on Alpha/VMS.  */
  return LAST_PAGE_BYTE ((char *) &BSS_edata, getpagesize ());
}

#endif /* __DECC */
#endif /* ! __GNUC__ */

char *
start_of_heap ()
{
  extern char *start_of_brk_area ();
  return start_of_brk_area ();
}

char *
end_of_heap ()
{
  extern char *end_of_brk_area ();
  return LAST_PAGE_BYTE((char *) end_of_brk_area (), getpagesize ());
}

char *
start_of_initial_heap ()
{
  extern char *start_of_brk_area ();
  return start_of_brk_area ();
}

char *
end_of_initial_heap ()
{
  extern char *end_of_brk_area ();
  return LAST_PAGE_BYTE((char *) end_of_brk_area (), getpagesize ());
}

char *
start_of_pure ()
{
  extern char *real_start_of_pure ();
  return real_start_of_pure ();
}

char *
end_of_pure ()
{
  extern char *real_end_of_pure ();
  return LAST_PAGE_BYTE((real_end_of_pure ()) - 1, getpagesize ());
}
#endif /* VMS */
