/* interface to e_read_sites(). */

/* this is to form a list of sites */
typedef struct {
    float si_lat;	/* lat (+N), rads */
    float si_lng;	/* long (+E), rads */
    float si_elev;	/* elevation above sea level, meters (-1 means ?) */
    char si_tzdefn[32];	/* timezone info.. same format as UNIX tzset(3) */
    char si_name[1];	/* site name, used as a varable-length member */
} Site;

extern int sites_get_list P_((Site ***sippp));
extern int sites_search P_((int startpos, char *str));
extern void sites_query P_((void));
extern void sites_abbrev P_((char *full, char ab[], int maxn));


/* For RCS Only -- Do Not Edit
 * @(#) $RCSfile: sites.h,v $ $Date: 1998/11/26 03:15:33 $ $Revision: 1.2 $ $Name:  $
 */
