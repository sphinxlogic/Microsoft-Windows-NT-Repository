23-Oct-85 18:14:33-MDT,15303;000000000001
Return-Path: <unix-sources-request@BRL.ARPA>
Received: from BRL-TGR.ARPA by SIMTEL20.ARPA with TCP; Wed 23 Oct 85 18:13:44-MDT
Received: from usenet by TGR.BRL.ARPA id a017613; 23 Oct 85 18:42 EDT
From: Bruce Robertson <bruce@stride.uucp>
Newsgroups: net.sources
Subject: GNU Emacs unexec() function for System-V
Message-ID: <522@stride.stride.UUCP>
Date: 23 Oct 85 17:24:19 GMT
To:       unix-sources@BRL-TGR.ARPA


This should run under any System-V - just defined USG to get the System-V
code.  Let me know if you have any problems, please.


	Bruce Robertson
	UUCP: cbosgd!utah-cs!utah-gr!stride!bruce


--------- cut here -------------
/* 
 * unexec.c - Convert a running program into an a.out file.
 * 
 * Author:	Spencer W. Thomas
 * 		Computer Science Dept.
 * 		University of Utah
 * Date:	Tue Mar  2 1982
 * Copyright (c) 1982 Spencer W. Thomas
 *
 * Synopsis:
 *	unexec (new_name, a_name, data_start, bss_start, entry_address)
 *	char *new_name, *a_name;
 *	unsigned data_start, bss_start, entry_address;
 *
 * Takes a snapshot of the program and makes an a.out format file in the
 * file named by the string argument new_name.
 * If a_name is non-NULL, the symbol table will be taken from the given file.
 * 
 * The boundaries within the a.out file may be adjusted with the data_start 
 * and bss_start arguments.  Either or both may be given as 0 for defaults.
 * 
 * Data_start gives the boundary between the text segment and the data
 * segment of the program.  The text segment can contain shared, read-only
 * program code and literal data, while the data segment is always unshared
 * and unprotected.  Data_start gives the lowest unprotected address.  Since
 * the granularity of write-protection is on 1k page boundaries on the VAX, a
 * given data_start value which is not on a page boundary is rounded down to
 * the beginning of the page it is on.  The default when 0 is given leaves the
 * number of protected pages the same as it was before.
 * 
 * Bss_start indicates how much of the data segment is to be saved in the
 * a.out file and restored when the program is executed.  It gives the lowest
 * unsaved address, and is rounded up to a page boundary.  The default when 0
 * is given assumes that the entire data segment is to be stored, including
 * the previous data and bss as well as any additional storage allocated with
 * break (2).
 *
 * The new file is set up to start at entry_address.
 *
 * If you make improvements I'd like to get them too.
 * harpo!utah-cs!thomas, thomas@Utah-20
 *
 */

#ifndef emacs
#define PERROR perror
#define ERROR fprintf
#define ERRORF stderr,
#else
#include "config.h"
#define PERROR(file) error ("Failure operating on %s", file)
#define ERROR error
#define ERRORF
#endif

#if defined(USG) || defined(STRIDE)
#include <sys/types.h>
#endif
#include <sys/param.h>
#include <stdio.h>
/* #include <sys/dir.h> */
#include <sys/stat.h>
#include <a.out.h>
#include <errno.h>

#if defined(USG) || defined(STRIDE)

static struct filehdr filehdr;
static struct filehdr old;
static struct aouthdr aouthdr;
static struct scnhdr text, data, bss;

static int pagemask;
extern char etext[];


/* ****************************************************************
 * unexec
 *
 * driving logic.
 */
unexec (new_name, a_name, data_start, bss_start, entry_address)
     char *new_name, *a_name;
     unsigned data_start, bss_start, entry_address;
{
  int new, a_out = -1;

  if (a_name && (a_out = open( a_name, 0 )) < 0)
    {
      PERROR (a_name);
      return -1;
    }
  if ((new = creat (new_name, 0666)) < 0)
    {
      PERROR( new_name );
      return -1;
    }

  pagemask = MMU_PAGESIZE - 1;

  filehdr.f_magic = MC68MAGIC;
  filehdr.f_nscns = 3;		/* text, data and bss */
  time(&filehdr.f_timdat);
  filehdr.f_opthdr = sizeof aouthdr;
  filehdr.f_flags = F_RELFLG | F_EXEC | F_LNNO | F_AR32WR;

  if (make_hdr( new, a_out, data_start, bss_start, entry_address) < 0 ||
      copy_text_and_data( new ) < 0 ||
      copy_sym( new, a_out ) < 0)
    {
      close (new);
      /* unlink( new_name );	    	/* Failed, unlink new a.out */
      return -1;	
    }

  close (new);
  if (a_out >= 0)
    close (a_out);
  mark_x (new_name);
  return 0;
}

/* ****************************************************************
 * make_hdr
 *
 * Make the header in the new a.out from the header in core.
 * Modify the text and data sizes.
 */
static int
make_hdr( new, a_out, data_start, bss_start, entry_address)
int new, a_out;
unsigned data_start, bss_start, entry_address;
{
    /* Adjust data/bss boundary. */
    if ( bss_start != 0 )
    {
/*	bss_start = (bss_start + pagemask) & ~pagemask;	      /* (Up) to page bdry. */
	if ( bss_start > sbrk (0))
	{
	    ERROR( ERRORF
		"unexec: Specified bss_start( %u ) is past end of program.\n",
		bss_start );
	    return -1;
	}
    }
    else
      bss_start = sbrk (0);

    /* Adjust text/data boundary. */
    if (!data_start)
      data_start = (int) etext;
    data_start = data_start & ~pagemask; /* (Down) to page boundary. */

    if ( data_start > bss_start )	/* Can't have negative data size. */
    {
	ERROR( ERRORF
	    "unexec: data_start(%u) can't be greater than bss_start( %u ).\n",
	    data_start, bss_start );
	return -1;
    }

    aouthdr.magic = 0410;
    aouthdr.vstamp = 0;
    aouthdr.text_start = TEXT_START;
    aouthdr.data_start = data_start;
    aouthdr.entry = entry_address;
    aouthdr.bsize = sbrk (0) - bss_start;
    aouthdr.dsize = bss_start - data_start;
    aouthdr.tsize = data_start - TEXT_START;

    strcpy(text.s_name, _TEXT);
    text.s_paddr = text.s_vaddr = aouthdr.text_start;
    text.s_size = aouthdr.tsize;
    text.s_scnptr = sizeof filehdr + sizeof aouthdr + 3 * SCNHSZ;
    text.s_relptr = text.s_lnnoptr = text.s_nreloc = text.s_nlnno = 0;
    text.s_flags = STYP_REG | STYP_TEXT;

    strcpy(data.s_name, _DATA);
    data.s_paddr = data.s_vaddr = aouthdr.data_start;
    data.s_size = aouthdr.dsize;
    data.s_scnptr = text.s_scnptr + text.s_size;
    data.s_relptr = data.s_lnnoptr = data.s_nreloc = data.s_nlnno = 0;
    data.s_flags = STYP_REG | STYP_DATA;

    strcpy(bss.s_name, _BSS);
    bss.s_paddr = bss.s_vaddr = data.s_paddr + data.s_size;
    bss.s_size = aouthdr.bsize;
    bss.s_scnptr = 0;
    bss.s_relptr = bss.s_lnnoptr = bss.s_nreloc = bss.s_nlnno = 0;
    bss.s_flags = STYP_REG | STYP_BSS;

    /* Get symbol table info from header of a.out file if given one. */
    if ( a_out >= 0 )
    {
	if ( read( a_out, &old, sizeof old ) != sizeof old )
	{
	    PERROR( "Couldn't read header from a.out file" );
	    return -1;
	}

	if (old.f_magic != MC68MAGIC)
	{
	    ERROR( ERRORF "a.out file doesn't have legal magic number\n" );
	    return -1;
	}
	filehdr.f_nsyms = old.f_nsyms;
	filehdr.f_symptr = data.s_scnptr + data.s_size;
    }
    else
    {
	filehdr.f_nsyms = 0;		/* No a.out, so no symbol info. */
	filehdr.f_symptr = 0;
    }

    if (write(new, (char *) &filehdr, sizeof filehdr) != sizeof filehdr
     || write(new, (char *) &aouthdr, sizeof aouthdr) != sizeof aouthdr
     || write(new, (char *) &text, sizeof text) != sizeof text
     || write(new, (char *) &data, sizeof data) != sizeof data
     || write(new, (char *) &bss,  sizeof bss)  != sizeof bss)
    {
	PERROR( "Couldn't write header to new a.out file" );
	return -1;
    }

    return 0;
}

/* ****************************************************************
 * copy_text_and_data
 *
 * Copy the text and data segments from memory to the new a.out
 */
static int
copy_text_and_data( new )
int new;
{
    int nwrite, ret;
    int end;
    int i;
    int ptr;
    char buf[80];
    extern int errno;

    end = aouthdr.tsize + aouthdr.dsize;
    for (i = 0, ptr = TEXT_START; i < end;)
      {
	nwrite = 128;
	if (nwrite > end - i) nwrite = end - i;
	ret = write (new, ptr, nwrite);
	if (ret == -1 && errno == EFAULT)
	  {
	    lseek (new, text.s_scnptr + i + nwrite, 0);
	  }
	else if (nwrite != ret)
	  {
	    sprintf(buf, "Write failure in unexec: ptr 0x%x size 0x%x nwrite 0x%x errno %d",
			 ptr, nwrite, ret, errno);
	    PERROR(buf);
	    return -1;
	  }
	i += nwrite;
	ptr += nwrite;
      }

    return 0;
}

/* ****************************************************************
 * copy_sym
 *
 * Copy the relocation information and symbol table from the a.out to the new
 */
static int
copy_sym( new, a_out )
int new, a_out;
{
    char page[1024];
    int n;

    if ( a_out < 0 )
	return 0;

    lseek(a_out, old.f_symptr, 0);
    while ( (n = read( a_out, page, sizeof page )) > 0 )
    {
	if ( write( new, page, n ) != n )
	{
	    PERROR( "Error writing symbol table to new a.out" );
	    ERROR( ERRORF "new a.out should be ok otherwise\n" );
	    return 0;
	}
    }
    if ( n < 0 )
    {
	PERROR( "Error reading symbol table from a.out,\n" );
	ERROR( ERRORF "new a.out should be ok otherwise\n" );
    }
    return 0;
}

/* ****************************************************************
 * mark_x
 *
 * After succesfully building the new a.out, mark it executable
 */
static
mark_x( name )
char *name;
{
    struct stat sbuf;
    int um;

    um = umask( 777 );
    umask( um );
    if ( stat( name, &sbuf ) == -1 )
    {
	PERROR ( "Can't stat new a.out" );
	ERROR( ERRORF "Setting protection to %o\n", 0777 & ~um );
	sbuf.st_mode = 0777;
    }
    sbuf.st_mode |= 0111 & ~um;
    if ( chmod( name, sbuf.st_mode ) == -1 )
	PERROR( "Couldn't change mode of new a.out to executable" );

}

#else	/* USG || STRIDE */

extern char etext;

static struct exec hdr, ohdr;
static int pagemask;

/* ****************************************************************
 * unexec
 *
 * driving logic.
 */
unexec (new_name, a_name, data_start, bss_start, entry_address)
     char *new_name, *a_name;
     unsigned data_start, bss_start, entry_address;
{
  int new, a_out = -1;

  if (a_name && (a_out = open( a_name, 0 )) < 0)
    {
      PERROR (a_name);
      return -1;
    }
  if ((new = creat (new_name, 0666)) < 0)
    {
      PERROR( new_name );
      return -1;
    }

  pagemask = getpagesize () - 1;

  if (make_hdr( new, a_out, data_start, bss_start, entry_address) < 0 ||
      copy_text_and_data( new ) < 0 ||
      copy_sym( new, a_out ) < 0)
    {
      close (new);
      /* unlink( new_name );	    	/* Failed, unlink new a.out */
      return -1;	
    }

  close (new);
  if (a_out >= 0)
    close (a_out);
  mark_x (new_name);
  return 0;
}

/* ****************************************************************
 * make_hdr
 *
 * Make the header in the new a.out from the header in core.
 * Modify the text and data sizes.
 */
static int
make_hdr( new, a_out, data_start, bss_start, entry_address)
int new, a_out;
unsigned data_start, bss_start, entry_address;
{
    /* Get symbol table info from header of a.out file if given one. */
    if ( a_out >= 0 )
    {
	if ( read( a_out, &ohdr, sizeof hdr ) != sizeof hdr )
	{
	    PERROR( "Couldn't read header from a.out file" );
	    return -1;
	}

	if N_BADMAG( ohdr )
	{
	    ERROR( ERRORF "a.out file doesn't have legal magic number\n" );
	    return -1;
	}
	hdr.a_syms = ohdr.a_syms;
    }
    else
	hdr.a_syms = 0;			/* No a.out, so no symbol info. */

    /* Construct header from user structure. */
    hdr.a_magic = ZMAGIC;
    hdr.a_trsize = 0;
    hdr.a_drsize = 0;
    hdr.a_entry = entry_address;

    /* Adjust data/bss boundary. */
    if ( bss_start != 0 )
    {
	bss_start = (bss_start + pagemask) & ~pagemask;	      /* (Up) to page bdry. */
	if ( bss_start > sbrk (0))
	{
	    ERROR( ERRORF
		"unexec: Specified bss_start( %u ) is past end of program.\n",
		bss_start );
	    return -1;
	}
    }
    else
      bss_start = (sbrk (0) + pagemask) & ~pagemask;

    /* Adjust text/data boundary. */
    if (!data_start)
      data_start = (int) &etext;
#ifdef SUN
    data_start = data_start & ~(SEGSIZ - 1); /* (Down) to segment boundary. */
#else
    data_start = data_start & ~pagemask; /* (Down) to page boundary. */
#endif

    if ( data_start > bss_start )	/* Can't have negative data size. */
    {
	ERROR( ERRORF
	    "unexec: data_start(%u) can't be greater than bss_start( %u ).\n",
	    data_start, bss_start );
	return -1;
    }

    hdr.a_bss = sbrk (0) - bss_start;
    hdr.a_data = bss_start - data_start;
    hdr.a_text = data_start - TEXT_START;

    if ( write( new, &hdr, sizeof hdr ) != sizeof hdr )
    {
	PERROR( "Couldn't write header to new a.out file" );
	return -1;
    }
    return 0;
}

/* ****************************************************************
 * copy_text_and_data
 *
 * Copy the text and data segments from memory to the new a.out
 */
static int
copy_text_and_data( new )
int new;
{
    int nwrite, ret;
    int end;
    int i;
    int ptr;
    char buf[80];
    extern int errno;

    lseek (new, (long) N_TXTOFF (hdr), 0);

    end = hdr.a_text + hdr.a_data;
    for (i = 0, ptr = TEXT_START; i < end;)
      {
	nwrite = 128;
	if (nwrite > end - i) nwrite = end - i;
	ret = write (new, ptr, nwrite);
	if (ret == -1 && errno == EFAULT)
	  {
	    lseek (new, (long) (N_TXTOFF (hdr) + i + nwrite), 0);
	  }
	else if (nwrite != ret)
	  {
	    sprintf(buf, "Write failure in unexec: ptr 0x%x size 0x%x nwrite 0x%x errno %d",
			 ptr, nwrite, ret, errno);
	    PERROR(buf);
	    return -1;
	  }
	i += nwrite;
	ptr += nwrite;
      }

    return 0;
}

/* ****************************************************************
 * copy_sym
 *
 * Copy the relocation information and symbol table from the a.out to the new
 */
static int
copy_sym( new, a_out )
int new, a_out;
{
    char page[1024];
    int n;

    if ( a_out < 0 )
	return 0;

    lseek( a_out, (long)N_SYMOFF(ohdr), 0 );	/* Position a.out to symtab.*/
    while ( (n = read( a_out, page, sizeof page )) > 0 )
    {
	if ( write( new, page, n ) != n )
	{
	    PERROR( "Error writing symbol table to new a.out" );
	    ERROR( ERRORF "new a.out should be ok otherwise\n" );
	    return 0;
	}
    }
    if ( n < 0 )
    {
	PERROR( "Error reading symbol table from a.out,\n" );
	ERROR( ERRORF "new a.out should be ok otherwise\n" );
    }
    return 0;
}

/* ****************************************************************
 * mark_x
 *
 * After succesfully building the new a.out, mark it executable
 */
static
mark_x( name )
char *name;
{
    struct stat sbuf;
    int um;

    um = umask( 777 );
    umask( um );
    if ( stat( name, &sbuf ) == -1 )
    {
	PERROR ( "Can't stat new a.out" );
	ERROR( ERRORF "Setting protection to %o\n", 0777 & ~um );
	sbuf.st_mode = 0777;
    }
    sbuf.st_mode |= 0111 & ~um;
    if ( chmod( name, sbuf.st_mode ) == -1 )
	PERROR( "Couldn't change mode of new a.out to executable" );

}
#endif /* USG || STRIDE */
-- 

	Bruce Robertson
	UUCP: cbosgd!utah-cs!utah-gr!stride!bruce
