
/*
** COPYRIGHT (c) 1999 BY
** DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.
** ALL RIGHTS RESERVED.
**
** THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED
** ONLY  IN  ACCORDANCE  OF  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE
** INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER
** COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
** OTHER PERSON.  NO TITLE TO AND  OWNERSHIP OF THE  SOFTWARE IS  HEREBY
** TRANSFERRED.
**
** THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE
** AND  SHOULD  NOT  BE  CONSTRUED  AS A COMMITMENT BY DIGITAL EQUIPMENT
** CORPORATION.
**
** DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS
** SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
*/
/*
**++
**  Facility:
**
**      Examples
**
**  Version: V1.0
**
**  Abstract:
**
**      Example of working with the IO$_ACPCONTROL sys$qio call on NET:.
**
**      This program demonstrates the (undocumented) IO$_ACPCONTROL 
**      mechanism used by SHOW NETWORK with DECnet Phase IV on OpenVMS.
**
**      This example assumes OpenVMS V7.1 or later (strictly for the
**      EFN$_ENF support), DECnet Phase IV, and DEC C for OpenVMS.
**
**      For details of what this program is up to, you will need access
**      to the OpenVMS source listings, such as the INITUSER module in
**      the LOGINOUT facility.  NET: IO$_ACPCONTROL calls are scattered
**      throughout OpenVMS.
**
**  Author:
**      Steve Hoffman
**
**  Creation Date:  10-Jun-1999
**
**  Modification History:
**--
*/

/*
//  $ cc nfb+sys$share:sys$lib_c/lib
//  $ link nfb
*/

#include <ctype.h>
#include <descrip.h>
#include <efndef.h>
#include <iodef.h>
#include <nfbdef.h>
#include <string.h>
#include <ssdef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stsdef.h>
#include <starlet.h>

#define NODEBUFSIZ 1024

/*
//  Brute-force routine to unpack and print the nodes in the buffer...
*/
void
UnpackBuf( unsigned char *Buf, int RetLen )
  {
  unsigned short int BufSiz;
  unsigned short int NodeNameSiz;
  unsigned long int RecBas = 0;
  unsigned long int NotDone = TRUE;
#define NODENAMESIZ 256
  char NodeName[NODENAMESIZ];

  BufSiz = RetLen < NODEBUFSIZ ? RetLen : NODEBUFSIZ;
  
  while ( NotDone )
    {
    if ( NodeNameSiz = (unsigned short int) Buf[RecBas] )
      {
      if ( NotDone && !RecBas )
        printf("Nodes currently reachable:\n");
      NodeNameSiz = NodeNameSiz < ( NODENAMESIZ - 1) ? 
        NodeNameSiz : NODENAMESIZ - 1;
      strncpy( NodeName, (void *) &(Buf[RecBas + 2]), NodeNameSiz );
      NodeName[NodeNameSiz] = '\0';
      RecBas += NodeNameSiz + 2;
      printf("  %s\n", NodeName );
      if (RecBas >= BufSiz )
        NotDone = FALSE;
      }
    else
      NotDone = FALSE;
    }
  return;
  }
main()
  {
  struct _nfb *Nfb;
  int RetStat;
  unsigned short int Chan;
  struct dsc$descriptor KeyDsc = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL };
  struct dsc$descriptor NfbDsc = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL };
  struct dsc$descriptor BufDsc = { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL };
  unsigned short int Iosb[4];
  unsigned char Buf[NODEBUFSIZ];
  int RetLen = 0;
  int NfbSiz;
  int i = 0;
  unsigned int *NfbFld;
  $DESCRIPTOR( NetDsc, "NET:");
  struct nfbkey 
    {
    unsigned long int p4_count, srch, srch2;
    unsigned short int start;
    unsigned char ctx[NFB$C_CTX_SIZE];
    } Key;

  /*
  //  The NFB plus some field requests
  */
#define NFB_FLDREQ  20
  NfbSiz = sizeof( struct _nfb ) + (NFB_FLDREQ * sizeof( int ));
  Nfb = calloc( 1, NfbSiz );

  /*
  //  Set up the various string descriptors necessary...
  */
  KeyDsc.dsc$w_length = sizeof( struct nfbkey );
  KeyDsc.dsc$a_pointer = (void *) &Key;

  NfbDsc.dsc$w_length = NfbSiz;
  NfbDsc.dsc$a_pointer = (void *) Nfb;

  BufDsc.dsc$w_length = NODEBUFSIZ;
  BufDsc.dsc$a_pointer = (void *) &Buf;

  /*
  //  Set up the (full) NFB block for a search of reachable nodes,
  //  while also specifically excluding any loopback nodes.
  */
  Nfb->nfb$b_fct = NFB$C_FC_SHOW;
  Nfb->nfb$b_database = NFB$C_DB_NDI;
  Nfb->nfb$l_srch_key = NFB$C_NDI_REA;
  Nfb->nfb$b_oper = NFB$C_OP_EQL;
  Nfb->nfb$l_srch2_key = NFB$C_NDI_LOO;
  Nfb->nfb$b_oper2 = NFB$C_OP_NEQ;
  Nfb->nfb$b_mbz1 = 0;
  Nfb->nfb$w_cell_size = 0;
  NfbFld = &Nfb->nfb$l_fldid;
  NfbFld[i++] = NFB$C_NDI_NNA;
  NfbFld[i++] = NFB$C_ENDOFLIST;

  /*
  //  Set up the (wildcard) search key context...
  */
  Key.p4_count = 0;
  Key.srch = 1;
  Key.srch2 = 1;
  Key.start = 0;

  /*
  //  Assign the channel, lob the request at the NETACP, unpack and
  //  format the results, deassign the channel, and bail out...
  */
  RetStat = sys$assign( &NetDsc, &Chan, 0, 0 );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
     return RetStat;

  RetStat = sys$qiow( EFN$C_ENF, Chan, IO$_ACPCONTROL, Iosb, 0, 0,
    &NfbDsc, &KeyDsc, &RetLen, &BufDsc, 0, 0 );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
     return RetStat;
  
  UnpackBuf( Buf, RetLen );

  RetStat = sys$dassgn( Chan );
  if (!$VMS_STATUS_SUCCESS( RetStat ))
     return RetStat;

  return SS$_NORMAL;
  }
