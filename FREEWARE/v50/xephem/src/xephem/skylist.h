/* include file to hook skyviewmenu.c and skylist.c together.
 * we are just trying this split to keep skyviewmenu.c from growing endlessly.
 */


/* Obj.flags or TSky flags values */
#define OBJF_ONSCREEN	FUSER0	/* bit set if obj is on screen */
#define OBJF_RLABEL	FUSER1	/* set if right-label is to be on */
#define OBJF_LLABEL	FUSER4	/* set if left-label is to be on */
#define OBJF_PERSLB	(OBJF_RLABEL|OBJF_LLABEL) /* either means persistent */
#define OBJF_NLABEL	FUSER5	/* set if name-label is to be on */
#define OBJF_MLABEL	FUSER6	/* set if name-label is to be on */


/* skyviewmenu.c */
extern void sv_all P_((Now *np));
extern void sv_getcenter P_((int *aamode, double *fov,
    double *altp, double *azp, double *rap, double *decp));
extern void sv_getfldstars P_((ObjF **fsp, int *nfsp));

/* skylist.c */
extern void sv_list_cb P_((Widget w, XtPointer client, XtPointer call));

typedef struct {
    double ra, dec, alt, az;	/* location */
    double eyepw, eyeph;	/* width and height, rads */
    int round;			/* true if want round, false if square */
    int solid;			/* true if want solid, else just border */
} EyePiece;

extern int se_ismanaged P_((void));
extern void se_manage P_((void));
extern void se_unmanage P_((void));
extern void se_add P_((double ra, double dec, double alt, double az));
extern int se_getlist P_((EyePiece **ep));

/* For RCS Only -- Do Not Edit
 * @(#) $RCSfile: skylist.h,v $ $Date: 1999/10/15 20:11:46 $ $Revision: 1.2 $ $Name:  $
 */
