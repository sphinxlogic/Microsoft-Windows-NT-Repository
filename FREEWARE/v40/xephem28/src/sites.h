/* interface to e_read_sites(). */

/* this is to form a list of sites */
typedef struct {
    float si_lat;	/* lat (+N), rads */
    float si_lng;	/* long (+E), rads */
    float si_elev;	/* elevation above sea level, meters (-1 means ?) */
    char si_name[1];	/* site name, used as a varable-length member */
} Site;

extern int sites_get_list P_((Site ***sippp));
extern int sites_search P_((int startpos, char *str));
extern void sites_query P_((void));
