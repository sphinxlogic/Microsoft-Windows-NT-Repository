#include descrip
#include stsdef
#include "jsy$library:jsydef.h"

#define	ON			1
#define	OFF			0
#define	MAX_FONT_SIZE		40
#define	MAX_BUFF_SIZE		293
#define	CHR_START		0x90
#define	CHR_SEP1		';'
#define	CHR_SEP2		'x'
#define	CHR_END			0x9C

static struct FONT_REC {
    unsigned short	code;
    unsigned char	map[250];
    char		size[4];
};

/*
**	Preload expaneded font
*/
jlb$preload(indsc, inlen, outdsc, outlen)
struct discripter	*indsc;
struct discripter	*outdsc;
unsigned int		inlen;
unsigned int		*outlen;
{
unsigned int	poly = 0xa001,		/* CRC-16 */
		crc;
int		crc_table[16];
static struct dsc$descriptor crc_desc = {0, DSC$K_DTYPE_T,DSC$K_CLASS_D, 0};

struct FONT_REC	*inbuff;
unsigned short	dsclen;
unsigned char	buffer[MAX_BUFF_SIZE];
unsigned char	*bp, *cp;
unsigned int	cl;
unsigned int	size, vsize;
unsigned int	pitch;
unsigned char	bitmap[MAX_FONT_SIZE+2][MAX_FONT_SIZE];
register int	stat, v, h, m;


    if (!((stat = lib$analyze_sdesc(indsc,&dsclen,&inbuff)) & STS$M_SUCCESS))
        return stat;
/*
    if (inlen > dsclen)
	return JSY$_INVFORMAT;
*/
    if (inlen == 128) {
	size = 24;
	cl = 98;
	pitch = 8*4;
    }
    else if (inlen == 256) {
	size = (inbuff->size[0] - '0') * 10 + inbuff->size[1] - '0';
	if (size == 32) {
	    cl = 178;
	    pitch = 11*4;
	}
	else if (size == 40) {
	    cl = 282;
	    pitch = 14*4;
	}
	else
	    return JSY$_INVFORMAT;
    }
    else
	return JSY$_INVFORMAT;
    vsize = ((size + 2) / 3) * 3;

    lib$crc_table(&poly, &crc_table);
    ots$move5(0, 0, OFF, sizeof(bitmap), &bitmap);
    for (v=0; v<size; v++)
	for (h=0; h<size; h++)
	    bitmap[v][h] = (inbuff->map[size/8*v+h/8] >> (7-h%8)) & 0x01;
    bp = buffer;
    *(bp++) = CHR_START;
    *(bp++) = inbuff->code & 0x8000 ? '6' : '7';
    if (size != 24) {
	*(bp++) = CHR_SEP1;
	*(bp++) = (size == 32) ? '2' : '3';
	*(bp++) = CHR_SEP1;
	*(bp++) = '0';
    }	
    *(bp++) = CHR_SEP2;
    *(short *)bp = inbuff->code & 0x7f7f;
    cp = bp;
    bp += 2;
    for (h=0; h<size/8; h++)
	for (v=0; v<vsize; v++) {
	    if (v % 3 == 0)
		*(bp + h*pitch+v/3*4) = 0x20;
	    *(bp + h*pitch+v/3*4) += (bitmap[v][h*8+1] << 4-v%3*2) +
				     (bitmap[v][h*8  ] << 5-v%3*2);
	    *(bp + h*pitch+v*4/3+1) = 0x20;
	    for (m=0; m<6; m++)
		*(bp + h*pitch+v*4/3+1) += bitmap[v][h*8+7-m] << m;
	}
    bp += (cl - 2);
    if (!((stat = lib$scopy_r_dx(&cl, cp, &crc_desc)) & STS$M_SUCCESS))
	return stat;
    crc = lib$crc(&crc_table, &0, &crc_desc);
    *(bp++) = ((crc      ) & 0x3f) + 0x20;
    *(bp++) = ((crc >>  6) & 0x3f) + 0x20;
    *(bp++) = ((crc >> 12) & 0x0f) + 0x20;
    *(bp++) = CHR_END;
    *outlen = bp - buffer;
    return (lib$scopy_r_dx(outlen, buffer, outdsc));
}
