/*
** Show_roots.c		Hein van den Heuvel, October 1990
**		
**	Template program to show index root level(s) for selected file.
**	Feel free to modify to display more param, add LIB$FIND_FILE etc...
**	Usage: Define as external DCL command and pass filespec as param.
**	Have fun.
*/


#include <rms>
#include <stdio>
#define MAXKEY 10

main (int argc, char *argv[])
{
struct FAB	fab;
struct XABSUM	sum;
struct XABKEY	xab[MAXKEY];
int		i, stat, lvl, keys;
int		strlen(), sys$display(), sys$open();

fab = cc$rms_fab;
sum = cc$rms_xabsum;
fab.fab$b_shr = FAB$M_SHRPUT;
fab.fab$b_fac = FAB$M_GET;
fab.fab$l_fna = argv[1];
fab.fab$b_fns = strlen( argv[1] );
fab.fab$l_xab = (void *) &sum;
stat = sys$open ( &fab );
if (!(stat&1)) return stat;
if (fab.fab$b_org!=FAB$C_IDX) return RMS$_ORG;
keys = sum.xab$b_nok;
fab.fab$l_xab = (void *) &xab[0];
for (i=0; i<keys; i++)
    {
    /* 
    ** Init Xab Key for each defined key
    ** Point previous to current except first.
    */
    xab[i] = cc$rms_xabkey;
    xab[i].xab$b_ref = i;
    if (i) xab[i-1].xab$l_nxt = (void *) &xab[i];
    }
/*
** Ask RMS to fill in the XABs hooked off the FAB.
*/
stat = sys$display ( &fab );
if (!(stat&1)) return stat;

printf ("File %s, Root levels: %d", argv[1], xab[0].xab$b_lvl);
for (i=1; i<keys; i++) printf (", %d", xab[i].xab$b_lvl);
printf (".\n");
return stat;
}
