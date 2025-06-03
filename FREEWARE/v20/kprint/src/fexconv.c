#include descrip
#include stsdef
#include ssdef
#include "jsy$library:jsydef.h"

#include "fextbl.h"

#define	DEFFONTSIZE	40
#define CUTBASE		100
static unsigned char	bitmap[DEFFONTSIZE+1][DEFFONTSIZE+1];
static unsigned int	bitmap_exp[DEFFONTSIZE][DEFFONTSIZE];

struct font_data {
    unsigned short	code;
    unsigned char	map[250];
    char		size[4];
};

static struct font_data	*inbuff;
static struct font_data	outbuff;

/*
**	Convert One Font
*/
int	jlb$fex_convert(srcdsc, dstdsc, dstlen, flag)
struct dsc$descriptor	*srcdsc, *dstdsc;
unsigned		*dstlen, flag;
{
int		status, size, cutoff;
unsigned char	*fact;
register int	v, h;

    if (flag > FACTTBLMAX)
	return JSY$_INVARG;
    inbuff = srcdsc->dsc$a_pointer;
    outbuff.code = inbuff->code;
    for (v=0; v<fact_table[flag].from; v++) {
	for (h=0; h<fact_table[flag].from; h++)
	    bitmap[v][h] = (inbuff->map[v*(fact_table[flag].from/8)+h/8] >> 7-h%8) & 0x01;
	bitmap[v][fact_table[flag].from] = 0;
    }
    for (h=0; h<=fact_table[flag].from; h++)
	bitmap[fact_table[flag].from][h] = 0;

    bitmap_expand(fact_table[flag].to, fact_table[flag].fact);

    cutoff = fact_table[flag].cutoff;
    for (v=0; v<fact_table[flag].to; v++)
	for (h=0; h<fact_table[flag].to; h+=8)
	    outbuff.map[v*(fact_table[flag].to/8) + h/8] = 
			       ((bitmap_exp[v][h+7] > cutoff) << 0 |
				(bitmap_exp[v][h+6] > cutoff) << 1 |
				(bitmap_exp[v][h+5] > cutoff) << 2 |
				(bitmap_exp[v][h+4] > cutoff) << 3 |
				(bitmap_exp[v][h+3] > cutoff) << 4 |
				(bitmap_exp[v][h+2] > cutoff) << 5 |
				(bitmap_exp[v][h+1] > cutoff) << 6 |
				(bitmap_exp[v][h+0] > cutoff) << 7);
    if (fact_table[flag].to == 24)
	*dstlen = 128;
    else {
	outbuff.size[0] = outbuff.size[2] = fact_table[flag].to / 10 + '0';
	outbuff.size[1] = outbuff.size[3] = fact_table[flag].to % 10 + '0';
	*dstlen = 256;
    }
    return (lib$scopy_r_dx(dstlen, &outbuff, dstdsc));
}

static	bitmap_expand(size, fact)
unsigned	size;
unsigned char	fact[][3];
{
register int	v, h;

    for (v=0; v < size; v++)
	for (h=0;  h < size; h++)
	    bitmap_exp[v][h] = 
	    	(bitmap[fact[v][0]  ][fact[h][0]  ] * fact[v][1] * fact[h][1] + 
		 bitmap[fact[v][0]  ][fact[h][0]+1] * fact[v][1] * fact[h][2] + 
		 bitmap[fact[v][0]+1][fact[h][0]  ] * fact[v][2] * fact[h][1] + 
		 bitmap[fact[v][0]+1][fact[h][0]+1] * fact[v][2] * fact[h][2]);
}
