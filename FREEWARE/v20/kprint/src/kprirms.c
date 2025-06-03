#module KPRI$RMS "V4.5-000"
/****************************************************************************
!*                                                                          *
!*  COPYRIGHT (c) 1983, 1984, 1985 BY                                       *
!*  NIHON DIGITAL EQUIPMENT CORPORATION, TOKYO, JAPAN.                      *
!*  ALL RIGHTS RESERVED.                                                    *
!*                                                                          *
!*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
!*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
!*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
!*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
!*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
!*  TRANSFERRED.                                                            *
!*                                                                          *
!*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
!*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
!*  CORPORATION.                                                            *
!*                                                                          *
!*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
!*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.                 *
!*                                                                          *
!*                                                                          *
!***************************************************************************/
/*++
! FACILITY: KPRI Version 4.5-000
!
! FUNCTIONAL DESCRIPTION:
!
! ENVIRONMENT: VAX/VMS
!
! AUTHOR: Inasawa, Yuichi       CREATION DATE: 22-May-1985
!
! MODIFIED BY:
!
! V5.2-000
!	antonietta	Jul 26 1989
!	VFC support: initialize static date by ZERO.
! V4.5-000
!	Y. Kozono	28-Nov-1986
!	/COVER_PAGE support
!	/HEADER replace print symbiont to print command
--*/

#include descrip
#include iodef
#include rms
#include ssdef
#include stsdef
#include "kpridef"

static struct RAB *kpri$ga_input_rab;
static struct RAB *kpri$ga_saved_input_rab;
static struct RAB *kpri$ga_output_rab;
static char        kpri$ab_rab_rhb[255];

kpri$rms_init_input(fab, rab, nam, xabfhc)
struct FAB *fab;
struct RAB *rab;
struct NAM *nam;
struct XABFHC *xabfhc;
{
/*
** build FAB block
*/
   *fab = cc$rms_fab;
   fab -> fab$b_fac = FAB$M_GET;
   fab -> fab$l_nam = nam;
   fab -> fab$b_rat |= FAB$M_CR;
   fab -> fab$l_xab = xabfhc;

/*
** build RAB block
*/
   *rab = cc$rms_rab;
   rab -> rab$l_fab = fab;
   rab -> rab$l_rhb = kpri$ab_rab_rhb;
   rab -> rab$b_rac = RAB$C_SEQ;

/*
** build NAM block
*/
   *nam = cc$rms_nam;
}

kpri$rms_init_output(fab, rab, nam)
struct FAB *fab;
struct RAB *rab;
struct NAM *nam;
{
/*
** build FAB block
*/
   *fab = cc$rms_fab;
   fab -> fab$b_fac = FAB$M_PUT;
   fab -> fab$b_rat |= FAB$M_CR;
   fab -> fab$b_rfm = FAB$C_VAR;
   fab -> fab$l_nam = nam;

/*
** build RAB block
*/
   *rab = cc$rms_rab;
   rab -> rab$l_fab = fab;
   rab -> rab$l_rhb = kpri$ab_rab_rhb;
   /*
    * V5.2-000:	VFC support
    *		Initialize Record Header area TO keep same action between
    *		single file job and multi file job.
    */
   {
	   char	*c;
	   int	i;

	   for(c = rab->rab$l_rhb, i = 0; i < 256; *c = '\0', i ++)
		   ;
   }
   rab -> rab$b_rac = RAB$C_SEQ;

/*
** build NAB block
*/
   *nam = cc$rms_nam;
}

kpri$rms_open_input(fab, rab, nam, file_name, default_name, result_name)
struct FAB            *fab;
struct RAB            *rab;
struct NAM            *nam;
struct dsc$descriptor *file_name;
struct dsc$descriptor *default_name;
struct dsc$descriptor *result_name;
{
   int status;
   char expand_buff[NAM$C_MAXRSS];
   char result_buff[NAM$C_MAXRSS];

   fab -> fab$b_fns = file_name    -> dsc$w_length;
   fab -> fab$l_fna = file_name    -> dsc$a_pointer;
   fab -> fab$b_dns = default_name -> dsc$w_length;
   fab -> fab$l_dna = default_name -> dsc$a_pointer;
   nam -> nam$b_rss = sizeof result_buff;
   nam -> nam$l_rsa = result_buff;
   nam -> nam$b_ess = sizeof expand_buff;
   nam -> nam$l_esa = expand_buff;

   status = sys$open(fab);
   if (nam -> nam$b_rsl > 0)
      ots$scopy_r_dx(nam -> nam$b_rsl, result_buff, result_name);
   else
      ots$scopy_r_dx(nam -> nam$b_esl, expand_buff, result_name);
   if (!(status & STS$M_SUCCESS))
      return status;

   if (!((status = sys$connect(rab)) & STS$M_SUCCESS))
      return status;

   return status;
}

kpri$rms_open_output(fab, rab, nam, file_name, default_name, result_name)
struct FAB            *fab;
struct RAB            *rab;
struct NAM            *nam;
struct dsc$descriptor *file_name;
struct dsc$descriptor *default_name;
struct dsc$descriptor *result_name;
{
   int status;
   char result_buff[NAM$C_MAXRSS];
   char expand_buff[NAM$C_MAXRSS];

   fab -> fab$b_fns = file_name    -> dsc$w_length;
   fab -> fab$l_fna = file_name    -> dsc$a_pointer;
   fab -> fab$b_dns = default_name -> dsc$w_length;
   fab -> fab$l_dna = default_name -> dsc$a_pointer;
   nam -> nam$b_rss = sizeof result_buff;
   nam -> nam$l_rsa = result_buff;
   nam -> nam$b_ess = sizeof expand_buff;
   nam -> nam$l_esa = expand_buff;

   status = sys$create(fab);
   if (nam -> nam$b_rsl > 0)
      ots$scopy_r_dx(nam -> nam$b_rsl, result_buff, result_name);
   else
      ots$scopy_r_dx(nam -> nam$b_esl, expand_buff, result_name);
   if (!(status & STS$M_SUCCESS))
      return status;

   if (!((status = sys$connect(rab)) & STS$M_SUCCESS))
      return status;

   return status;
}

kpri$rms_set_input_rab(rab)
struct RAB *rab;
{
   kpri$ga_saved_input_rab = kpri$ga_input_rab;
   kpri$ga_input_rab = rab;
}

kpri$rms_old_input_rab()
{
   kpri$ga_input_rab = kpri$ga_saved_input_rab;
}

kpri$rms_set_output_rab(rab)
struct RAB *rab;
{
   kpri$ga_output_rab = rab;
}

kpri$rms_default_get(buff, size, leng)
char *buff;
int   size;
int  *leng;
{
   return kpri$rms_get(kpri$ga_input_rab, buff, size, leng);
}

kpri$rms_get(rab, buff, size, leng)
struct RAB *rab;
char       *buff;
int         size;
int        *leng;
{
   int status;

   rab -> rab$l_ubf = buff;
   rab -> rab$w_usz = size;

   status = sys$get(rab);

   *leng = rab -> rab$w_rsz;

   return status;
}

kpri$rms_default_put(buff, leng)
char       *buff;
int         leng;
{
   return kpri$rms_put(kpri$ga_output_rab, buff, leng);
}

kpri$rms_put(rab, buff, leng)
struct RAB *rab;
char       *buff;
int         leng;
{
   int status;

   rab -> rab$l_rbf = buff;
   rab -> rab$w_rsz = leng;

   if (!((status = sys$put(rab)) & STS$M_SUCCESS))
      lib$stop(status);

   return status;
}

kpri$rms_error(error_code, rms_status, file_name)
int error_code;
int rms_status;
struct dec$descriptor *file_name;
{
   int msg_vec[5];

   msg_vec[0] = 4;
   msg_vec[1] = error_code;
   msg_vec[2] = 1;
   msg_vec[3] = file_name;
   msg_vec[4] = rms_status;
   sys$putmsg(&msg_vec, 0, 0, 0);
}
