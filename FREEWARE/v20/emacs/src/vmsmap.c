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

/* #define VMS_DEBUG */

#include <config.h>
#include "lisp.h"
#include "getpagesize.h"
#include <rab.h>
#include <fab.h>
#include <rmsdef.h>
#include <secdef.h>

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

/* Structure to write into first block of map file.
 */

struct map_data
{
  char * sdata;			/* Start of data area */
  char * edata;			/* End of data area */
  int  datablk;			/* Block in file to map data area from/to.
				   This is relative to the start of section */
  int  nblocks;			/* # of blocks in this section (incl. head) */
  int  nremain;			/* Number of sections after this one */
};

static void fill_fab (), fill_rab ();
static int write_data ();

extern char *start_of_data ();
extern char *end_of_data ();
extern char *start_of_BSS ();
extern char *end_of_BSS ();
extern int vms_out_initial;	/* Defined in malloc.c */

#define NSECTS 1
#ifndef __GNUC__
#ifdef __DECC
#undef NSECTS
#define NSECTS 2
#endif
#endif

#if NSECTS > 2
#error HEY! VMSMAP.C needs code for NSECTS > 2!
#endif

typedef char * (*prgptr)();
static const prgptr data_routines[NSECTS*2] =
{
  start_of_data, end_of_data
#if NSECTS > 1
  ,start_of_BSS, end_of_BSS
#endif
};

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
  struct map_data map_data;
  int starting_block, i;

  for (i = 0, starting_block = 0; i < NSECTS; i++)
    {
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
      rab.rab$l_bkt = starting_block;
      status = sys$read (&rab);
      if (status != RMS$_NORMAL)
	lib$stop (status);
      status = sys$close (&fab);
      if (status != RMS$_NORMAL)
	lib$stop (status);
#ifdef VMS_DEBUG
      {
	printf ("map_data # %d:\n", i);
	printf ("  sdata: %p\n", map_data.sdata);
	printf ("  edata: %p\n", map_data.edata);
	printf ("  datablk: %d\n", map_data.datablk);
	printf ("  nblocks: %d\n", map_data.nblocks);
	printf ("  nremain: %d\n", map_data.nremain);
      }
#endif
      if (map_data.sdata != (*data_routines[i*2])() /* start_of_data () */)
	{
	  printf ("Start of data area has moved: cannot map in data.\n");
	  return 0;
	}
      if (map_data.edata != (*data_routines[i*2+1])() /* end_of_data () */)
	{
	  printf ("End of data area has moved: cannot map in data.\n");
	  return 0;
	}
      if (map_data.nremain != NSECTS - 1 - i)
	{
	  printf ("Unexpected amount of remaining sections (%d, expected %d): cannot map in data.\n",
		  map_data.nremain, NSECTS - 1 - i);
	  return 0;
	}
      fab.fab$l_fop |= FAB$M_UFO;
      status = sys$open (&fab);
      if (status != RMS$_NORMAL)
	lib$stop (status);
      /* Map data area. */
      inadr[0] = (int) map_data.sdata;
      inadr[1] = (int) map_data.edata;
#if 0 /* This is bogus, until the contrary is proven */
#ifdef __DECC
#define FLAGS SEC$M_CRF | SEC$M_WRT | SEC$M_EXPREG
#else /* not __DECC */
#define FLAGS SEC$M_CRF | SEC$M_WRT
#endif /* __DECC */
#else /* not 0 */
#define FLAGS SEC$M_CRF | SEC$M_WRT
#endif /* 0 */
      {
#ifdef VMS_DEBUG
#define RETADR retadr
	volatile int retadr[2], i;
	printf ("The input address was (2 items): \n");
	for (i = 0; i < 2; i++)
	  printf (" %u", inadr[i]);
	printf ("\n");
#else
#define RETADR 0
#endif

	status = sys$crmpsc (inadr, RETADR, 0, FLAGS, 0, 0, 0,
			     fab.fab$l_stv, 0,
			     starting_block + map_data.datablk, 0, 0);

#ifdef VMS_DEBUG
	printf ("The return address was (2 items): \n");
	for (i = 0; i < 2; i++)
	  printf (" %u", retadr[i]);
	printf ("\n");
#endif
      }
      if (! (status & 1))
	lib$stop (status);
      starting_block += map_data.nblocks;
    }

  /* cooperate with VMSGMALLOC.C */
  {
#ifdef GNU_MALLOC
    void vms_clear_data (), malloc_clear_hooks ();
    malloc_clear_hooks ();
    vms_clear_data (1);
#endif
  }
}

/* Writes the data and alloc area to the map file.
 */
mapout_data (into)
     char * into;
{
  struct FAB fab;
  struct RAB rab;
  int status, i;
  struct map_data map_data[NSECTS];
  int datasize[NSECTS], msize, sum_blocks, starting_block;

  if (vms_out_initial)
    {
      error ("Out of initial allocation. Must rebuild emacs with more memory (VMS_ALLOCATION_SIZE).");
      return 0;
    }

  for (i = 0, sum_blocks = 0; i < NSECTS; i++)
    {
      map_data[i].sdata = (*data_routines[i*2])(); /* start_of_data () or start_of_BSS () */
      map_data[i].edata = (*data_routines[i*2+1])(); /* start_of_data () or start_of_BSS () */
      datasize[i] = map_data[i].edata - map_data[i].sdata + 1;
      map_data[i].datablk = 2
	+ (sizeof (struct map_data) + BLOCKSIZE - 1) / BLOCKSIZE;
      map_data[i].nblocks = 1 + map_data[i].datablk
	+ ((datasize[i] + BLOCKSIZE - 1) / BLOCKSIZE);
      map_data[i].nremain = NSECTS - 1 - i;
      sum_blocks += map_data[i].nblocks;
    }

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
      error ("Could not create map file");
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
      error ("RMS connect to map file failed");
      return 0;
    }
  for (i = 0, starting_block = 0; i < NSECTS; i++)
    {
      /* Write the header */
      rab.rab$l_rbf = (char *) &map_data[i];
      rab.rab$w_rsz = sizeof (struct map_data);
      rab.rab$l_bkt = starting_block;
      status = sys$write (&rab);
      if (status != RMS$_NORMAL)
	{
	  error ("RMS write (header) to map file failed");
	  status = sys$close (&fab);
	  if (status != RMS$_NORMAL)
	    error ("RMS close on map file failed");
	  return 0;
	}
      if (! write_data (&rab, starting_block + map_data[i].datablk,
			map_data[i].sdata, datasize[i]))
	{
	  status = sys$close (&fab);
	  if (status != RMS$_NORMAL)
	    error ("RMS close on map file failed");
	  return 0;
	}
      starting_block += map_data[i].nblocks;
    }
  status = sys$close (&fab);
  if (status != RMS$_NORMAL)
    {
      error ("RMS close on map file failed");
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
  
  rab->rab$l_bkt = firstblock;
  while (length > 0)
    {
      rab->rab$l_rbf = data;
      rab->rab$w_rsz = length > MAXWRITE ? MAXWRITE : length;
      status = sys$write (rab, 0, 0);
      if (status != RMS$_NORMAL)
	{
	  char buf[100];

	  /* We double the %% part, because error () does it's own
	     processing of % */
	  sprintf( buf,
		  "RMS write to map file failed with status %%%%X0%X (data = %%%%X0%X, length = %d", status, data, length);
	  error (buf);
	  return 0;
	}
      data = &data[MAXWRITE];
      length -= MAXWRITE;
      rab->rab$l_bkt = 0;
    }
  return 1;
}				/* write_data */

/* The following code should probably really reside in sysdep.c, but there's a
   bug in the DEC C v1.3 compiler, concerning globaldef and globalref */

/*
 *	Return the address of the start of the data segment prior to
 *	doing a memory dump.
 */
 
char *
start_of_data ()
{
#ifdef VMS_DEBUG
  printf ("Start of data : %%X0%X\n", (char *) &sdata);
#endif

  /* We arrange for this to always start on a CPU-specific page boundary.  */
  return ((char *) &sdata);
}

/*
 *	Return the address of the end of the data segment prior to
 *	doing a memory dump.
 */

#define LAST_PAGE_BYTE(p,ps) \
  ((((unsigned long) (p)) & ~((ps) - 1)) + ((ps) - 1))

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
  printf ("Start of data : %%X0%X\n", (char *) &BSS_sdata);
#endif

  /* We arrange for this to always start on a CPU-specific page boundary.  */
  return ((char *) &BSS_sdata);
}

char *
end_of_BSS ()
{
#ifdef VMS_DEBUG
  printf ("End of data : %%X0%X (page size = %d)\n",
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
#endif /* VMS */
