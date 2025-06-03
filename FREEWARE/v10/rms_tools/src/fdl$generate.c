/*
** fdl_generate_like.c	Hein van den Heuvel, June 1993
**		
**	Template program to show how to generate most rms control blocks
**	for specified files and uses them as input for FDL$GENERATE
**	Feel free to modify to display more param, add LIB$FIND_FILE etc...
**	Usage: Define as external DCL command and pass filespec as param.
**	Have fun.
*/


#include <rms>
#include <stdio>
#include <descrip>

main (int argc, char *argv[])
{
struct FAB	*fab;
struct RAB	*rab;
struct XABSUM	*sum;
struct XABKEY	*key;
struct XABALL	*area;
void		**xab_chain;
void		*malloc();
int 		STRLEN(), sys$open(), sys$display(), fdl$generate();
int		i, stat;

$DESCRIPTOR (output_fdl, "SYS$OUTPUT:" )
;
fab = (void *) malloc ( (sizeof *fab) + (sizeof *rab) + (sizeof *sum ));
*fab = cc$rms_fab;
rab = (void *)(fab + 1);
*rab = cc$rms_rab;
sum = (void *)(rab + 1);
*sum = cc$rms_xabsum;
xab_chain = (void *) &fab->fab$l_xab;
fab->fab$b_shr = FAB$M_UPD; 
fab->fab$l_fna = argv[1];
fab->fab$b_fns = STRLEN( argv[1] );
fab->fab$l_xab = (void *) sum;
stat = sys$open ( fab );
if (!(stat&1)) return stat;
if (fab->fab$b_org!=FAB$C_IDX) return RMS$_ORG;

/* 
** Init Xab Key for each defined key and insert in list (order!).
*/
i = sum->xab$b_nok;
key = malloc ( i * sizeof *key );
while ( --i >= 0 )
    {
    *key = cc$rms_xabkey;
    key->xab$b_ref = i;
    key->xab$l_nxt = *xab_chain;
    *xab_chain = key;
    key++;
    }

/* 
** Init Xab Area for each defined area and insert in list (order!).
*/
i = sum->xab$b_noa;
area = malloc ( i * sizeof *area );
while ( --i >= 0 )
    {
    *area = cc$rms_xaball;
    area->xab$b_aid = i;
    area->xab$l_nxt = *xab_chain;
    *xab_chain = area;
    area++;
    }

/*
** Ask RMS to fill in the XABs hooked off the FAB.
*/
stat = sys$display ( fab );
if (!(stat&1)) return stat;

return fdl$generate ( &0, &fab, &rab, &output_fdl );
}
