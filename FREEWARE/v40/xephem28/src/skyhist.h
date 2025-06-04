/* include file to hook skyviewmenu.c and skyhist.c together.
 * we are just doing this split to keep skyviewmenu.c from growing endlessly.
 */


/* history record */
typedef struct {
    double fov;			/* sv_fov value */
    double azra;		/* sv_azra value */
    double altdec;		/* sv_altdec value */
    int aa_mode;		/* aa_mode value */
    int flip_lr, flip_tb;	/* flip values */
    int fmag, bmag;		/* magnitude limits */
    int lbl_n, lbl_m;		/* name/mags label options */
    int lbl_a, lbl_b;		/* all/brightest label options */
} SvHistory;


/* skyviewmenu.c */
extern void svh_goto P_((SvHistory *hp));
extern void svh_get P_((SvHistory *hp));


/* skyaux.c */
extern void svh_create P_((Widget mb));
