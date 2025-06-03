#module UAF_XXXUAI "X-1"

/*
**++
**  FACILITY:	Authorization record maintenance utility
**
**  MODULE DESCRIPTION:
**
**		This routine contains the routines needed to support the
**		creation and deletion of authorization records.
**
**  AUTHORS:	L. Mark Pilant		CREATION DATE:	12-Oct-1993
**
**  MODIFICATION HISTORY:
**
**
**--
*/

/*
**
**  INCLUDE FILES
**
*/

#include	<PSLDEF>

#include	descrip
#include	fab
#include	rab
#include	rmsdef
#include	string

#include	"uaf_header"

#pragma noinline (OPEN_UAF)
#pragma noinline (SYS$CREUAI)
#pragma noinline (SYS$DELUAI)

/* Define the macro used to insure the authorization file is closed as
** necessary prior to returning an error to the caller. */

#define		RETURN_ERROR(_error_code)							\
		    {										\
		    if (close_file != 0) sys$close (&local_uaf_fab);				\
		    return _error_code;								\
		    }										\

unsigned int OPEN_UAF (ifi, isi)

unsigned short int		*ifi;
unsigned short int		*isi;
{
/*
**++
**  FUNCTIONAL DESCRIPTION:
**
**	This routine is calles to open the system authorization file for
**	use by the $CREUAI and $DELUAI routines.  (This parallels the
**	code in [LOADSS]SYSUAISRV.B32.)
**
**  FORMAL PARAMETERS:
**
**	IFI		- Address of a cell to contain the IFI of the file
**	ISI		- Address of a cell to contain the ISI of the file
**
**  RETURN VALUE:
**
**	Status of the RMS $OPEN.
**
**  SIDE EFFECTS:
**
**	None
**
**--
*/

/* Global references. */

globalref char			*default_sysuaf_file_name;	/* Authorization file default file name */
globalref char			*default_sysuaf_file_type;	/* Authorization file default file type */
globalref char			*default_sysuaf_file_spec;	/* Authorization filed default spec */

/* Local storage. */

unsigned int		status;				/* Routine exit status */
struct FAB		uaf_fab;			/* Authorization file FAB */
struct RAB		uaf_rab;			/* Authorization file RAB */

/* Clear out the IFI and ISI arguments to indicate nothing was accessed. */

*ifi = 0;
*isi = 0;

/* Initialize the RMS data structures needed to open the user authorization file. */

/* Authorization File Access Block. */

uaf_fab = cc$rms_fab;			/* Prototype information */
uaf_fab.fab$b_fac = FAB$M_GET | FAB$M_PUT | FAB$M_DEL | FAB$M_UPD;
uaf_fab.fab$b_shr = FAB$M_GET | FAB$M_PUT | FAB$M_DEL | FAB$M_UPD;

/* If the SYSUAF_LOGICAL symbol is defined, set up the FAB to allow
** the authorization file to be accessed via the appropriate logical
** name.  Otherwise, set up the FAB to allow the authorization file
** to be accessed via a full file specification. */

#ifdef	SYSUAF_LOGICAL
    uaf_fab.fab$l_fna = default_sysuaf_file_name;		/* For the logical name */
    uaf_fab.fab$b_fns = strlen (default_sysuaf_file_name);
    uaf_fab.fab$l_dna = default_sysuaf_file_type;
    uaf_fab.fab$b_dns = strlen (default_sysuaf_file_type);

/* Since it is possible for the authorization file to be access via a
** logical name, insure only exec mode logical name translations are
** allowed.  This also means the $xxxUAI routines in this module will
** open the file in the same way as the $GETUAI and $SETUAI services. */

    uaf_fab.fab$v_lnm_mode = PSL$C_EXEC;
#else
    uaf_fab.fab$l_fna = default_sysuaf_file_spec;		/* For a file spec */
    uaf_fab.fab$b_fns = strlen (default_sysuaf_file_spec);
#endif	/* SYSUAF_LOGICAL */

/* Authorization file Record Access Block. */

uaf_rab = cc$rms_rab;			/* Prototype information */
uaf_rab.rab$l_fab = &uaf_fab;
uaf_rab.rab$b_rac = RAB$C_KEY;
uaf_rab.rab$l_rop = RAB$M_WAT;

/* Open the authorization file, and return any errors. */

CHECK_RETURN (SYS$OPEN (&uaf_fab));

/* Connect up a record stream to the authorization file. */

CHECK_RETURN (SYS$CONNECT (&uaf_rab));

/* Since the file was successfully opened, save the RMS IFI and
** ISI for later. */

*ifi = uaf_fab.fab$w_ifi;
*isi = uaf_rab.rab$w_isi;

return SS$_NORMAL;
}

extern unsigned int SYS$CREUAI (null1, context, user_name, itmlst, null3, null4, null5)

unsigned int			null1;
unsigned short int		(*context)[];
struct dsc$descriptor		*user_name;
struct ITMDEF			(*itmlst)[];
unsigned int			null3;
unsigned int			null4;
unsigned int			null5;
{
/*
** FUNCTIONAL DESCRIPTION:
** 
**	This routine is used to create a user authorization entry.
**
**					NOTE
**
**	This routine is not expected to be used outside the DECwindows
**	Authorize utility environment.  As such, it does not do the all
**	the necessary argument probing to be secure.  Although this could
**	be added at a later time.
**
**					NOTE
** 
** FORMAL PARAMETERS:
** 
**	NULL1:		Unused parameter
**	CONTEXT:	Address of a context longword
**	USER_NAME:	Address of the username descriptor
**	ITMLST:		Address of a list of item descriptors
**	NULL3:		Unused parameter
**	NULL4:		Unused parameter
**	NULL5:		Unused parameter
** 
** 
** RETURN VALUE:
** 
**	1 if the record successfully added
**	error code otherwise (various RMS and system error codes)
** 
** SIDE EFFECTS:
** 
**	None
**
**--
*/

/* Global references. */

globalref struct ITMTABDEF	uai_itm_tab[];			/* $xxxUAI item code information table */

/* Local storage. */

unsigned short int		close_file;			/* Set to close file on exit */
unsigned char			encrypt_algorithm;		/* Password encryption algorithm */
unsigned int			hashed_pwd[2];			/* Hashed password */
int				index;				/* General index */
short int			item_code;			/* Item list entry item code */
short int			item_size;			/* Item list entry item entry size */
unsigned char			*item_addr;			/* Item list entry item info pointer */
struct FAB			local_uaf_fab;			/* Local authorization file FAB */
struct RAB			local_uaf_rab;			/* Local authorization file RAB */
char				local_username[MAX_USERNAME];	/* Local username */
struct dsc$descriptor		local_username_desc;		/* Local username descriptor */
struct dsc$descriptor		password_desc;			/* Local password descriptor */
unsigned int			status;				/* Routine exit status */
short int			uaf_field_size;			/* Size of UAF field (from table) */
unsigned short int		uaf_ifi;			/* UAF (RMS) IFI */
unsigned short int		uaf_isi;			/* UAF (RMS) ISI */
struct UAFDEF			uaf_record;			/* Storage for authorization file record */
unsigned char			*uaf_record_byte_stream;	/* UAF record as a byte stream */
char				*username_text;			/* Supplied username text */

/* Initialize needed storage */

memset (hashed_pwd, 0, sizeof hashed_pwd);
memset (&local_username_desc, 0, sizeof local_username_desc);
memset (&password_desc, 0, sizeof password_desc);

close_file = -1;
uaf_ifi = -1;
uaf_isi = -1;

/* Copy the supplied username to local storage, blank filling in the process. */

if (user_name != 0)
    {
    username_text = user_name->dsc$a_pointer;

/* Convert each character in the username string to upper case and blank
** fill any unused portion of the local username storage. */

    for (index = 0; index < sizeof local_username; ++index)
	{
	if (index >= user_name->dsc$w_length)
	    local_username[index] = ' ';
	else
	    local_username[index] = toupper (username_text[index]);
	}
    }
else return SS$_INSFARG;

local_username_desc.dsc$w_length = sizeof local_username;
local_username_desc.dsc$a_pointer = local_username;

/* If a context parameter was supplied, get it. */

if (context != 0)
    {
    uaf_ifi = (*context)[0];
    uaf_isi = (*context)[1];
    }

/* If the UAF has not already been opened, as indicated by a -1 value for the
** UAF IFI, attempt to open it now.  This is done without privileges (at the
** moment) to allow normal file accessing protection processing to apply. */

if (uaf_ifi == 0xFFFF)
    CHECK_RETURN (OPEN_UAF (&uaf_ifi, &uaf_isi))
else close_file = 0;				/* Note file is not to be closed */

/* Since the authorization file is now known to be open, fabricate the needed
** RMS data structured to allow the authorization records to be created. */

memset (&local_uaf_fab, 0, sizeof local_uaf_fab);
memset (&local_uaf_rab, 0, sizeof local_uaf_rab);

local_uaf_fab.fab$b_bid = FAB$C_BID;
local_uaf_fab.fab$b_bid = FAB$C_BID;
local_uaf_fab.fab$b_bln = FAB$C_BLN;
local_uaf_fab.fab$w_ifi = uaf_ifi;

local_uaf_rab.rab$b_bid = RAB$C_BID;
local_uaf_rab.rab$b_bln = RAB$C_BLN;
local_uaf_rab.rab$w_isi = uaf_isi;
local_uaf_rab.rab$b_rac = RAB$C_KEY;

/* Clear out the authorization record being built. */

memset (&uaf_record, 0, sizeof uaf_record);

/* Set up the version and record type in the new record. */

uaf_record.UAF$B_VERSION = UAF$C_VERSION1;
uaf_record.UAF$B_RTYPE = UAF$C_USER_ID;

/* Move over the username to the new authorization record. */

memmove (uaf_record.UAF$r_fill_0.UAF$T_USERNAME, local_username, sizeof local_username);

/* Fill in the information in the local authorization record from the information
** supplied in the item list. */

for (index = 0; (*itmlst)[index].itm$w_itmcod != 0; ++index)
    {

/* Get the necessary information for the current item from the item list. */

    item_code = (*itmlst)[index].itm$w_itmcod;
    item_size = (*itmlst)[index].itm$w_bufsiz;
    item_addr = (*itmlst)[index].itm$l_bufadr;

/* Range check the item code supplied. */

    if (item_code <= 0 || item_code >= UAI$_MAX_ITEM_CODE) RETURN_ERROR (SS$_BADPARAM);

/* Validate the size of the item list entry. */

    if ((TRUE ((uai_itm_tab)[item_code].uaf$r_fill_0.uaf$r_fill_1.uaf$v_itm_flg_var) &&
	 item_size > (uai_itm_tab)[item_code].uaf$w_itm_ent_size) ||
	(FALSE ((uai_itm_tab)[item_code].uaf$r_fill_0.uaf$r_fill_1.uaf$v_itm_flg_var) &&
	 item_size != (uai_itm_tab)[item_code].uaf$w_itm_ent_size))
	RETURN_ERROR (SS$_BADPARAM);

/* Note how big the field in the authorization record is.  In the usual case,
** this is used with the (above) FILL to copy the information supplied by the
** caller in the item list. */

    uaf_field_size = uai_itm_tab[item_code].uaf$w_itm_ent_size;

/* If a plaintext password is specified, encrypt it. */

    if ((item_code == UAI$_PASSWORD) || (item_code == UAI$_PASSWORD2))
	{

/* It is worth noting that the cleartext password will be encrypted using the
** current algorithm if the UAF$B_ENCRYPTx field has not been set in the record.
** This means the algorithm and encryption fields are order sensitive in the
** supplied item list. */

/* Set the appropriate password encyption algorithm. */

	if (item_code == UAI$_PASSWORD)
	     encrypt_algorithm = uaf_record.UAF$B_ENCRYPT;
	else encrypt_algorithm = uaf_record.UAF$B_ENCRYPT2;

/* If the encryption algorithm code is within the reserved to Digital range
** but is not the latest, upgrade it.  Also note the change in the authorization
** record. */


	if (encrypt_algorithm < UAF$C_CURRENT_ALGORITHM)
	    {
	    encrypt_algorithm = UAF$C_CURRENT_ALGORITHM;
	    if (item_code == UAI$_PASSWORD)			/* Choose primary or secondary algorithm */
		 uaf_record.UAF$B_ENCRYPT = encrypt_algorithm;
	    else uaf_record.UAF$B_ENCRYPT2 = encrypt_algorithm;
	    }

/* If there is a non-null password given, hash it. */

	if (item_size > 0)
	    {
	    password_desc.dsc$w_length = item_size;
	    password_desc.dsc$a_pointer = item_addr;
	    status = SYS$HASH_PASSWORD (&password_desc,
					encrypt_algorithm,
					uaf_record.UAF$W_SALT,
					&local_username_desc,
					hashed_pwd);
	    if (FALSE (status)) RETURN_ERROR (status);
	    if (item_code == UAI$_PASSWORD)
		 memmove (&uaf_record.UAF$r_fill_4.UAF$Q_PWD, hashed_pwd, sizeof uaf_record.UAF$r_fill_4.UAF$Q_PWD);
	    else memmove (&uaf_record.UAF$Q_PWD2, hashed_pwd, sizeof uaf_record.UAF$Q_PWD2);
	    }

/* Since the plaintext password has been hashed, modify the item information
** to point to the internal storage (for this routine) rather than the
** caller supplied storage. */

	item_addr = hashed_pwd;
	item_size = sizeof hashed_pwd;
	uaf_field_size = uai_itm_tab[UAI$_PWD].uaf$w_itm_ent_size;

/* Mark the appropriate password as pre-expired. */

/*	if ((item_code == UAI$_PWD) || (item_code == UAI$_PASSWORD))
**	    memset (local_uaf_record.UAF$Q_PWD_DATE, 0xff, sizeof uaf_record.UAF$Q_PWD_DATE);
**	if ((.item_code == UAI$_PWD2) || (item_code == UAI$_PASSWORD2))
**	    memset (local_uaf_record.UAF$Q_PWD2_DATE, 0xff, sizeof uaf_record.UAF$Q_PWD2_DATE);
*/
	}

/* Copy the supplied information to the authorization record currently being
** built, if necessary. */

    if (FALSE ((uai_itm_tab)[item_code].uaf$r_fill_0.uaf$r_fill_1.uaf$v_itm_flg_nop) &&
	uaf_field_size != 0)
	{

/* Make the UAF record available as a byte stream for the offsets from the UAI
** item table.  This could be done with casting, but copying pointers around is
** easier. */

	uaf_record_byte_stream = &uaf_record;

/* Preset with the fill character if necessary.  Since the buffer is set to zero
** to begin with, it is only necessary to fill if the fill character is something
** other than zero. */

	if (TRUE ((uai_itm_tab)[item_code].uaf$r_fill_0.uaf$r_fill_1.uaf$v_itm_flg_spfill))
	    memset (&uaf_record_byte_stream[uai_itm_tab[item_code].uaf$w_itm_ent_base], ' ', uaf_field_size);

/* Move the supplied information */

	memmove (&uaf_record_byte_stream[uai_itm_tab[item_code].uaf$w_itm_ent_base], item_addr, item_size);
	}
    }

/* Now that all the information has been added to the authorization record, write
** it out to the authorization file. */

local_uaf_rab.rab$w_rsz = UAF$C_FIXED;
local_uaf_rab.rab$l_rbf = &uaf_record;
status = SYS$PUT (&local_uaf_rab);
SYS$RELEASE (&local_uaf_rab);				/* Release for future use. */

/* Since the operation has been completed successfully, see if it is necessary
** to return the RMS context. */

if (context != 0)
    {
    (*context)[0] = uaf_ifi;
    (*context)[1] = uaf_isi;
    }

/* Return with the status from the $PUT.  Using the macro will insure the file is
** closed if it is necessary. */

RETURN_ERROR (status);
} 

extern unsigned int SYS$DELUAI (null1, context, user_name, itmlst, null3, null4, null5)

unsigned int			null1;
unsigned short int		(*context)[];
struct dsc$descriptor		*user_name;
struct ITMDEF			(*itmlst)[];
unsigned int			null3;
unsigned int			null4;
unsigned int			null5;
{
/*
** FUNCTIONAL DESCRIPTION:
** 
**	This routine is used to delete a user authorization entry.  If an item
**	list is supplied, a call to $GETUAI is done prior to the deletion to
**	allow the existing information to be returned.
**
**					NOTE
**
**	This routine is not expected to be used outside the DECwindows
**	the necessary argument probing to be secure.  Although this could
**	be added at a later time.
**
**					NOTE
** 
** FORMAL PARAMETERS:
** 
**	NULL1:		Unused parameter
**	CONTEXT:	Address of a context longword
**	USER_NAME:	Address of the username descriptor
**	ITMLST:		Address of a list of item descriptors (to be returned)
**	NULL3:		Unused parameter
**	NULL4:		Unused parameter
**	NULL5:		Unused parameter
** 
** RETURN VALUE:
** 
**	1 if the record successfully added
**	error code otherwise (various RMS and system error codes)
** 
** SIDE EFFECTS:
** 
**	None
** 
*/

/* Local storage. */

unsigned short int		close_file;			/* Set to close file on exit */
int				index;				/* General index */
unsigned short int		local_context[2];		/* Local copy of UAI context */
struct FAB			local_uaf_fab;			/* Local authorization file FAB */
struct RAB			local_uaf_rab;			/* Local authorization file RAB */
char				local_username[MAX_USERNAME];	/* Local username */
struct dsc$descriptor		local_username_desc;		/* Local username descriptor */
unsigned int			status;				/* Routine exit status */
unsigned short int		uaf_ifi;			/* UAF (RMS) IFI */
unsigned short int		uaf_isi;			/* UAF (RMS) ISI */
char				*username_text;			/* Supplied username text */

/* Initialize needed storage */

memset (&local_username_desc, 0, sizeof local_username_desc);

close_file = -1;
uaf_ifi = -1;
uaf_isi = -1;

/* Copy the supplied username to local storage, blank filling in the process. */

if (user_name != 0)
    {
    username_text = user_name->dsc$a_pointer;

/* Convert each character in the username string to upper case and blank
** fill any unused portion of the local username storage. */

    for (index = 0; index < sizeof local_username; ++index)
	{
	if (index >= user_name->dsc$w_length)
	    local_username[index] = ' ';
	else
	    local_username[index] = toupper (username_text[index]);
	}
    }
else return SS$_INSFARG;

local_username_desc.dsc$w_length = sizeof local_username;
local_username_desc.dsc$a_pointer = local_username;

/* If a context parameter was supplied, get it. */

if (context != 0)
    {
    uaf_ifi = (*context)[0];
    uaf_isi = (*context)[1];
    }

/* If the UAF has not already been opened, as indicated by a -1 value for the
** UAF IFI, attempt to open it now.  This is done without privileges (at the
** moment) to allow normal file accessing protection processing to apply. */

if (uaf_ifi == 0xFFFF)
    CHECK_RETURN (OPEN_UAF (&uaf_ifi, &uaf_isi))
else close_file = 0;				/* Note file is not to be closed */

/* Since the authorization file is now known to be open, fabricate the needed
** RMS data structured to allow the authorization records to be created. */

memset (&local_uaf_fab, 0, sizeof local_uaf_fab);
memset (&local_uaf_rab, 0, sizeof local_uaf_rab);

local_uaf_fab.fab$b_bid = FAB$C_BID;
local_uaf_fab.fab$b_bid = FAB$C_BID;
local_uaf_fab.fab$b_bln = FAB$C_BLN;
local_uaf_fab.fab$w_ifi = uaf_ifi;

local_uaf_rab.rab$b_bid = RAB$C_BID;
local_uaf_rab.rab$b_bln = RAB$C_BLN;
local_uaf_rab.rab$w_isi = uaf_isi;
local_uaf_rab.rab$b_rac = RAB$C_KEY;

/* If an item list is supplied, call $GETUAI to get the existing infomation.
** Otherwise locate the record to be deleted. */

if (itmlst != 0)
    {

/* Make sure the context information is in a form $GETUAI can use. */

    local_context[0] = uaf_ifi;
    local_context[1] = uaf_isi;

/* Get the information requested. */

    status = SYS$GETUAI (NULL,
#ifdef NONSHARED_UAI_CONTEXT
			 NULL,
#else
			 &local_context,
#endif /* NONSHARED_UAI_CONTEXT */
			 &local_username_desc,
			 itmlst,
			 NULL,
			 NULL,
			 NULL);

/* If the $GETUAI fails, bail out now. */

    if (FALSE (status)) RETURN_ERROR (status);
    }

/* If the record position has not been set (or kept) by $GETUAI, it will be
** necessary to set the record position using $FIND.  The following situations
** require an explicit $FIND:
**
**  1)	On a Version 5.x system, the context parameter is not used in the
**	call to $GETUAI, thus the record position is not known outside the
**	call to $GETUAI.
**
**  2)	On a Version 6.x system where an item list has not been supplied to
**	this routine.
**
** In other words, if this is a Version 5.x system do the $FIND regardless;
** if this is a Verison 6.x system only do the $FIND if no item list was
** supplied. */

#ifndef NONSHARED_UAI_CONTEXT
if (itmlst == 0)
    {
#endif /* NONSHARED_UAI_CONTEXT */

    local_uaf_rab.rab$l_kbf = local_username_desc.dsc$a_pointer;
    local_uaf_rab.rab$b_ksz = local_username_desc.dsc$w_length;

    status = SYS$FIND (&local_uaf_rab);
    if (FALSE (status)) RETURN_ERROR (status);

#ifndef NONSHARED_UAI_CONTEXT
    }
#endif /* NONSHARED_UAI_CONTEXT */

/* Now that the record position has been set, delete the record. */

status = SYS$DELETE (&local_uaf_rab);

/* Since the operation has been completed successfully, see if it is necessary
** to return the RMS context. */

if (context != 0)
    {
    (*context)[0] = uaf_ifi;
    (*context)[1] = uaf_isi;
    }

/* Return with the status from the $DELETE.  Using the macro will insure the
** file is closed if it is necessary. */

RETURN_ERROR (status);
}
