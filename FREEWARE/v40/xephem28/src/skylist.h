/* include file to hook skyviewmenu.c and skylist.c together.
 * we are just trying this split to keep skyviewmenu.c from growing endlessly.
 */


/* Obj.flags or TSky flags values */
#define OBJF_ONSCREEN   0x1     /* bit set if obj is on screen */
#define OBJF_LABEL      0x2     /* set if label is to be on */


/* skyviewmenu.c */
extern void sv_getcenter P_((int *aamode, double *fov,
    double *altp, double *azp, double *rap, double *decp));


/* skylist.c */
extern void sv_list_cb P_((Widget w, XtPointer client, XtPointer call));
