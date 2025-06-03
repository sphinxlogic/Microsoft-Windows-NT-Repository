/*
 * Provide support routines for managing index file used for counting
 * accesses:
 *
 *   void *ifopen ( char *fname, char *mode );
 *   int ifclose ( void *ifile );
 *   int iferror ( void *ifile );
 *   int ifread_rec ( char *buf, size_t bufsize, size_t *len, void *ifile,
 *		char *key, int key_size );
 *   int ifwrite_rec ( char *buf, size_t bufsize, void *ifile );
 *   int ifupdate_rec ( char *buf, size_t bufsize, void *ifile );
 *   int ifdlcreate ( char *fld_str, char *fname, char *defname );
 *   int icheck_access ( char *fname, int uic );
 *
 * Base on indexio.c originally supplied by Ken Covert.
 *
 * Revised: 23-APR-1995		Added ifdlcreate and icheck_access. (DLJ)
 * Revised: 24-APR-1995		Add defaul ACL if empty.
 * Revised: 25-APR-1995		Cleanup up RMS calls, delete unused routine.
 */
#include <stdio.h>
#include <rms.h>
#include <string.h>
#include <starlet.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <acldef.h>
#include <armdef.h>
#include <chpdef.h>
#include <fdldef.h>
#include <jpidef.h>
#include <descrip.h>
#include <stdlib.h>
#include <stsdef.h>
typedef struct {
  struct FAB fab;
  struct RAB rab;
  union {
    int rc;
    struct {
      unsigned severity:1;
      unsigned return_condition:31;
    } status_parts;
  } status;
} IFILE;
#define IFILE_T 1
#include "access_db.h"

/*****************************************************************************/
/* Since ifdlcreate and icheck_access are called extremely rarely, dynamically
 * load FDLSHR and SECURESHR so we don't burden every run of the program
 * with 3 additional shareable image activations.
 */
static int (*fdl_create)(), (*sys_change_acl)();
static int images_loaded = 0;
static int load_images()
{
    $DESCRIPTOR(fdlshr,"FDLSHR");
    $DESCRIPTOR(securshr,"SECURESHR");
    $DESCRIPTOR(fdlcreate,"FDL$CREATE");
    $DESCRIPTOR(change_acl,"SYS$CHANGE_ACL");
    int status; 

    status = lib$find_image_symbol ( &fdlshr, &fdlcreate, &fdl_create );
    if ( (status&1) == 0 ) return status;

    status = lib$find_image_symbol ( &securshr, &change_acl, &sys_change_acl );
    if ( (status&1) == 0 ) return status;

    images_loaded = 1;
    return status;
}
/****************************************************************************/
int ifdlcreate ( char *fdl, char *fname, char *defname )
{
    int status, flags;
    $DESCRIPTOR(fdl_desc,"");
    $DESCRIPTOR(filename,"");
    $DESCRIPTOR(default_name,"");
    /*
     * Set descriptors to point to passed strings and call FDL$CREATE.
     */
    if ( !images_loaded ) load_images();
    fdl_desc.dsc$w_length = strlen ( fdl );
    fdl_desc.dsc$a_pointer = fdl;
    filename.dsc$w_length = strlen ( fname );
    filename.dsc$a_pointer = fname;
    default_name.dsc$w_length = strlen(defname);
    default_name.dsc$a_pointer = defname;
    flags = FDL$M_FDL_STRING | FDL$M_SIGNAL;

    status = fdl_create (&fdl_desc, &filename, &default_name, 0, 0, &flags);
    return status;
}
/****************************************************************************/
/* Retrieve ACL on indicated filename and see if it permits read access
 * to specified UIC.
 */
int icheck_access ( char *fname, int uic )
{
    struct { short len, code; char *buffer; int *retlen; } itemlist[10];
#define SETITEM(itm,a,b,c,d) \
	itm.len=a; itm.code=b; itm.buffer=(char *) c; itm.retlen=(int *) d;
    int status, acllen,  context, objtype;
    long access, rights[2], privs[2];
    char *acl;
    $DESCRIPTOR(filename,"");
    /*
     * Get length of ACL.
     */
    if ( !images_loaded ) load_images();
    SETITEM(itemlist[0],4,ACL$C_ACLLENGTH, &acllen, 0)
    itemlist[1].len = itemlist[1].code = 0;

    objtype = ACL$C_FILE;
    context = 0;
    filename.dsc$w_length = strlen ( fname );
    filename.dsc$a_pointer = fname;

    status = (*sys_change_acl)( 0, &objtype, &filename, &itemlist,
		0, 0, 0, 0, 0 );
    if ( (status&1) == 0 ) return status;
    /*
     * Allocate buffer and read entire ACL.
     */
    acl = malloc ( acllen );
    if ( !acl ) return 12;
    SETITEM(itemlist[0],acllen,ACL$C_READACL,acl,&acllen)

    status = (*sys_change_acl)( 0, &objtype, &filename, &itemlist,
		0, 0, 0, 0, 0 );
    if ( status == SS$_ACLEMPTY ) {
	/*
	 * Make phony ACL granting access to current process UIC.
	 */
	int iosb[2], pid, proc_uic;
	SETITEM(itemlist[0],4,JPI$_UIC,&proc_uic, 0);
        itemlist[1].len = itemlist[1].code = 0;		/* null list */
	pid = 0;
	status = sys$getjpiw ( 0, &pid, 0, &itemlist, iosb, 0, 0 );
	if ( (status&1) == 1 ) status = iosb[0];
	if ( (status&1) == 1 ) {
	    if ( proc_uic != uic ) status = SS$_NOPRIV;
	}
    }
    else if ( (status&1) == 1 ) {
	/*
	 * We have ACL, construct item list for $CHKPRO.
	 */
	access = ARM$M_READ;
	rights[0] = uic;
	rights[1] = 0;
	privs[0] = privs[1] = 0;
	SETITEM(itemlist[0],4,CHP$_ACCESS,&access,0)
	SETITEM(itemlist[1],8,CHP$_RIGHTS,rights,0)
	SETITEM(itemlist[2],8,CHP$_PRIV,privs,0)
	SETITEM(itemlist[3],acllen,CHP$_ACL,acl,0)
        itemlist[4].len = itemlist[4].code = 0;

	status = sys$chkpro ( &itemlist );
    }
    free ( acl );
    return status;
}
/****************************************************************************/
IFILE* ifopen(char *fn, char *mode)
{
  IFILE *ifile;

  ifile = malloc(sizeof(IFILE));
  ifile->fab = cc$rms_fab;
  if (strcmp(mode,"r") == 0) ifile->fab.fab$b_fac = FAB$M_GET;
  else if (strcmp(mode,"r+") == 0)
          ifile->fab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_UPD;
    else {
      fprintf(stderr,"I-file open error - invalid mode \"%s\"",mode);
      exit(0);
    }
  ifile->fab.fab$l_fna = fn;
  ifile->fab.fab$b_fns = strlen(fn);
  ifile->fab.fab$b_shr = FAB$V_MSE | FAB$V_SHRPUT | FAB$V_SHRGET |
    FAB$V_SHRDEL | FAB$V_SHRUPD;
  ifile->status.rc = sys$open(&(ifile->fab));
  if (ifile->status.status_parts.severity != STS$K_SUCCESS) {
    free(ifile);
    return(NULL);
  }

  ifile->rab = cc$rms_rab;
  ifile->rab.rab$l_fab = &(ifile->fab);
  ifile->status.rc = sys$connect(&(ifile->rab));
  if (ifile->status.status_parts.severity != STS$K_SUCCESS) {
    free(ifile);
    return(NULL);
  }
  return ifile;
}

/****************************************************************************/
int ifclose(IFILE *ifile)
{
  ifile->status.rc = sys$close(&(ifile->fab));
  if (ifile->status.status_parts.severity != STS$K_SUCCESS) return EOF;
  free(ifile);
  return 0;
}

/****************************************************************************/
/* Check status of last ifile call and signal error.
 */
int iferror(IFILE *ifile)
{
  if (ifile->status.status_parts.severity == STS$K_SUCCESS)
    return 0;
  else {
    lib$signal(ifile->status.rc);
    return ifile->status.rc;
  }
}

/****************************************************************************/
/* Read keyed record from input file, locking for update.
 */
int ifread_rec(void *ptr, size_t size_of_item, size_t *size_read,
  IFILE *ifile, int index, void *key, int key_size)
{

  int i;
  if (key_size==0) {
    ifile->rab.rab$b_rac = RAB$C_SEQ;
    ifile->rab.rab$l_rop = RAB$M_WAT;
  }
  else {
    ifile->rab.rab$b_rac = RAB$C_KEY;
    ifile->rab.rab$l_rop = RAB$M_WAT | RAB$M_RLK;
    ifile->rab.rab$l_kbf = key;
    ifile->rab.rab$b_ksz = key_size;
  }
  ifile->rab.rab$l_ubf = ptr;
  ifile->rab.rab$w_usz = size_of_item;
  ifile->rab.rab$b_krf = index;

  ifile->status.rc = sys$get(&(ifile->rab));
  if (ifile->status.status_parts.severity == STS$K_SUCCESS) {
      *size_read = ifile->rab.rab$w_rsz;
  } else {
       *size_read = 0;
  }
  return ifile->status.rc;
}

/****************************************************************************/
/* write indexed file record.  Return value is VMS status of $UPDATE call.
 */
int ifwrite_rec(void *ptr, size_t size_of_item, IFILE *ifile)
{
  ifile->rab.rab$b_rac = RAB$C_KEY;
  ifile->rab.rab$l_rbf = ptr;
    ifile->rab.rab$w_rsz = size_of_item;
    ifile->rab.rab$l_rop = RAB$M_UIF | RAB$M_WAT;
    ifile->status.rc = sys$put(&(ifile->rab));
    return ifile->status.rc;
}
/****************************************************************************/
/* Update indexed file record.  Return value is VMS status of $UPDATE call.
 */
int ifupdate_rec(void *ptr, size_t size_of_item, IFILE *ifile)
{
  ifile->rab.rab$b_rac = RAB$C_KEY;
  ifile->rab.rab$l_rbf = ptr;
    ifile->rab.rab$w_rsz = size_of_item;
    ifile->rab.rab$l_rop = RAB$M_UIF | RAB$M_WAT;
    ifile->status.rc = sys$update(&(ifile->rab));
    return ifile->status.rc;
}
