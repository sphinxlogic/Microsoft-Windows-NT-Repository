/*
** COPYFILE.C     This program copies the input file to the output file.
*/
#define REC_SIZE 132
#define INPUT_NAME "INFILE"
#define OUTPUT_NAME "OUTFILE"
#define DEFAULT_NAME ".DAT"

#include	<rms>		/* defines for rabs and fabs	*/
#include	<stdio>		/* defins printf...		*/
#include	<starlet> 	/* defines sys$open et al 	*/

COPYFILE ()
{
struct FAB	infab, outfab, *fab;	/* Allocate fabs and a pointer to fab */
struct RAB	inrab, outrab, *rab;	/* Allocate fabs and a pointer to fab */
int		lib$signal();
int		stat;
char		rec_buff[REC_SIZE];	/* maximum record size		      */

infab = cc$rms_fab;			/* Make this a real FAB	(bid and bln) */
infab.fab$l_fna = (char *) &INPUT_NAME;	/* Primary file name: (logical) name..*/
infab.fab$b_fns = sizeof INPUT_NAME -1;	/* .. and its size		      */
infab.fab$l_dna = (char *) &DEFAULT_NAME; /* Default name: here file type..     */
infab.fab$b_dns = sizeof DEFAULT_NAME -1; /* .. and its size		      */

inrab = cc$rms_rab;			/* Make this a real RAB (bid and bln) */
inrab.rab$l_fab = &infab;		/* Point to FAB for $CONNECT	      */
inrab.rab$v_rah = 1;			/* Set bitVield to request read-ahead */
inrab.rab$l_ubf = rec_buff;		/* Point to buffer area..	      */
inrab.rab$w_usz = REC_SIZE;		/* and indicate its size	      */

outfab = cc$rms_fab;			/* Make this a real FAB	(bid and bln) */
outfab.fab$v_ctg = 1;			/* Allocate contigeously	      */
outfab.fab$v_put = 1;			/* Write access (default on create)   */
outfab.fab$v_nil = 1;			/* No sharing (default on create)     */
outfab.fab$b_rat = FAB$M_CR;		/* Set option using bitMask	      */
outfab.fab$w_mrs = REC_SIZE;
outfab.fab$l_fna = (char *) &OUTPUT_NAME;
outfab.fab$b_fns = sizeof OUTPUT_NAME -1;
outfab.fab$l_dna = (char *) &DEFAULT_NAME;
outfab.fab$b_dns = sizeof DEFAULT_NAME -1; 

outrab = cc$rms_rab;
outrab.rab$l_fab = &outfab;
outrab.rab$v_wbh = 1;			/* Write Ahead			      */
outrab.rab$l_rbf = rec_buff;		/* Same buffer address as before      */

fab = &infab;				/* for error handling		    */
stat = sys$open ( fab );		/* Actual open (could use &infab)   */
if (stat & 1)				/* $OPEN Success ? */
{
    outfab.fab$l_alq = infab.fab$l_alq;	/* Set proper size for output	    */
    fab = &outfab;			/* for error handling		    */
    stat = sys$create ( fab );		/* Try to create the file	    */
}

if (stat & 1)				/* Both open & create success ?	    */
{
    rab = &outrab;			/* for error handling		    */
    stat = sys$connect ( rab );		/* get some rms internal buffers    */
    if (stat & 1)			/* output $CONNECT Success ?	    */
    {
	rab = &inrab;			/* for error handling		    */
	stat = sys$connect ( rab );	/* input $CONNECT Success ?	    */
    }
    if (stat & 1)			/* CONNECTs succes? then prime loop */
	stat = sys$get ( rab );		/* setting stat for while	    */
    
    while (stat & 1)			/* success on record operation ? */
    {
	/* 
	** Main Code. Opened and connected files and buffer
	** First $GET done and inrab is current. Copy records. 
	*/
	outrab.rab$w_rsz = inrab.rab$w_rsz;	/* set correct size	*/
	rab = &outrab;				/* error handler	*/
	stat = sys$put ( rab );			/* Actual copy		*/
	if (stat & 1)				/* $PUT success?	*/
	{
	    rab = &inrab;			/* for error handling   */
	    stat = sys$get ( rab );		/* $GET next , set stat */
	}
    } /* while */


    /*
    ** Fallen through while. stat must be EOF if copy was succesful. 
    ** if not, signal error from connect, get or put. Using stat instead 
    ** of rab->rab$l_sts to handle (programming) error providing RAB.
    */
    
    if (stat != RMS$_EOF)
	stat = lib$signal( stat, rab->rab$l_stv );

    stat = sys$close ( &infab );   
    stat = sys$close ( &outfab );
    }
else
{
    /* Failed to open input or output file */
    stat = lib$signal( stat, fab->fab$l_stv );
}

return stat;			/* Using output close stat to return */    
}
