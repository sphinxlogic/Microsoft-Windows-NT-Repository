/* include file to hook skyviewmenu.c and skyhist.c together.
 */


/* history record */
#define	GRIDLEN		9	/* useable chars in grid size TF */
typedef struct {
    double fov;			/* sv_fov value */
    double azra;		/* sv_azra value */
    double altdec;		/* sv_altdec value */

    int stmag, ssmag, dsmag;	/* star, solsys and deep sky limiting mag */
    int magstp;			/* magnitude step size */

    int grid;			/* grid option */
    int autogrid;		/* auto grid option */
    int aagrid;			/* alt-az grid option */
    int fullgrid;		/* full grid option */
    char vgrid[GRIDLEN+1];	/* vertical grid size text */
    char hgrid[GRIDLEN+1];	/* vertical grid size text */

    int aa_mode;		/* aa_mode value */
    int flip_lr, flip_tb;	/* flip values */
    int justd; 			/* justdots option */
    int eclip, galac, eq;	/* ecliptic, galactic and equator options */
    int hznmap;			/* horizon map */
    int conn, conf, conb;	/* constellatin name, figures, boundaries opts*/
    int eyep;			/* eyepiece option */
    int magscale;		/* mag scale option */
    int automag;		/* auto mag option */

    int lbl_lst, lbl_lss, lbl_lds;	/* name/mags label flags */
    int lbl_bst, lbl_bss, lbl_bds;	/* star/solsys/deep N brightest */

    char type_table[NOBJTYPES];	/* copy of skyfilt's type table */
    char fclass_table[NCLASSES];/* copy of skyfilt's fclass table */
} SvHistory;


/* skyviewmenu.c */
extern void svh_goto P_((SvHistory *hp));
extern void svh_get P_((SvHistory *hp));


/* skyhist.c */
extern void svh_create P_((Widget mb));

/* For RCS Only -- Do Not Edit
 * @(#) $RCSfile: skyhist.h,v $ $Date: 1999/10/15 20:11:46 $ $Revision: 1.4 $ $Name:  $
 */
