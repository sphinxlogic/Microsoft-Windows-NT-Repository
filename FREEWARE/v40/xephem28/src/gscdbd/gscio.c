/* code to handle the details of the GSC CD-ROMs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "gscdbd.h"

/* index by dec band and get the number of large regions in it and its first
 * large region number. these are for nothern bands; southern have the same
 * number of large regions but the first is the value in the table + 366.
 */
typedef struct {
    short nlrg;
    short firstlg;
} LargeRegionTable;
static LargeRegionTable lg_reg[24] = {
    {48,	1},
    {47,	49},
    {45,	96},
    {43,	141},
    {40,	184},
    {36,	224},
    {32,	260},
    {27,	292},
    {21,	319},
    {15,	340},
    {9,		355},
    {3,		364},
};

/* index by large region number (which start at 1, not 0) and get its first
 * small region number and the number of ra and dec subdivisions.
 */
typedef struct {
    short firstsm;
    short nsub;
} SmallRegionTable;
static SmallRegionTable sm_reg[733] = {
    {0,		0},
    {1,		3},
    {10,	3},
    {19,	3},
    {28,	3},
    {37,	3},
    {46,	3},
    {55,	3},
    {64,	3},
    {73,	3},
    {82,	4},
    {98,	4},
    {114,	4},
    {130,	4},
    {146,	4},
    {162,	4},
    {178,	4},
    {194,	4},
    {210,	4},
    {226,	3},
    {235,	3},
    {244,	3},
    {253,	3},
    {262,	3},
    {271,	3},
    {280,	3},
    {289,	3},
    {298,	3},
    {307,	3},
    {316,	3},
    {325,	3},
    {334,	4},
    {350,	4},
    {366,	4},
    {382,	4},
    {398,	4},
    {414,	4},
    {430,	4},
    {446,	4},
    {462,	4},
    {478,	4},
    {494,	4},
    {510,	4},
    {526,	4},
    {542,	4},
    {558,	3},
    {567,	3},
    {576,	3},
    {585,	3},
    {594,	3},
    {603,	3},
    {612,	3},
    {621,	3},
    {630,	3},
    {639,	3},
    {648,	3},
    {657,	3},
    {666,	4},
    {682,	4},
    {698,	4},
    {714,	4},
    {730,	4},
    {746,	4},
    {762,	4},
    {778,	4},
    {794,	4},
    {810,	3},
    {819,	3},
    {828,	3},
    {837,	3},
    {846,	3},
    {855,	3},
    {864,	3},
    {873,	3},
    {882,	3},
    {891,	3},
    {900,	3},
    {909,	3},
    {918,	3},
    {927,	4},
    {943,	4},
    {959,	4},
    {975,	4},
    {991,	4},
    {1007,	4},
    {1023,	4},
    {1039,	4},
    {1055,	4},
    {1071,	4},
    {1087,	4},
    {1103,	4},
    {1119,	4},
    {1135,	4},
    {1151,	3},
    {1160,	3},
    {1169,	3},
    {1178,	3},
    {1187,	3},
    {1196,	3},
    {1205,	3},
    {1214,	3},
    {1223,	3},
    {1232,	4},
    {1248,	4},
    {1264,	4},
    {1280,	4},
    {1296,	4},
    {1312,	4},
    {1328,	4},
    {1344,	4},
    {1360,	4},
    {1376,	4},
    {1392,	3},
    {1401,	3},
    {1410,	3},
    {1419,	3},
    {1428,	3},
    {1437,	2},
    {1441,	2},
    {1445,	2},
    {1449,	3},
    {1458,	3},
    {1467,	3},
    {1476,	3},
    {1485,	3},
    {1494,	3},
    {1503,	4},
    {1519,	4},
    {1535,	4},
    {1551,	4},
    {1567,	4},
    {1583,	4},
    {1599,	4},
    {1615,	4},
    {1631,	4},
    {1647,	4},
    {1663,	4},
    {1679,	4},
    {1695,	4},
    {1711,	3},
    {1720,	3},
    {1729,	3},
    {1738,	3},
    {1747,	3},
    {1756,	3},
    {1765,	4},
    {1781,	4},
    {1797,	4},
    {1813,	4},
    {1829,	4},
    {1845,	4},
    {1861,	4},
    {1877,	4},
    {1893,	4},
    {1909,	4},
    {1925,	4},
    {1941,	3},
    {1950,	3},
    {1959,	3},
    {1968,	3},
    {1977,	2},
    {1981,	2},
    {1985,	2},
    {1989,	2},
    {1993,	2},
    {1997,	3},
    {2006,	3},
    {2015,	3},
    {2024,	3},
    {2033,	3},
    {2042,	4},
    {2058,	4},
    {2074,	4},
    {2090,	4},
    {2106,	4},
    {2122,	4},
    {2138,	4},
    {2154,	4},
    {2170,	4},
    {2186,	4},
    {2202,	4},
    {2218,	4},
    {2234,	4},
    {2250,	3},
    {2259,	4},
    {2275,	4},
    {2291,	4},
    {2307,	4},
    {2323,	4},
    {2339,	4},
    {2355,	4},
    {2371,	4},
    {2387,	4},
    {2403,	4},
    {2419,	4},
    {2435,	4},
    {2451,	4},
    {2467,	4},
    {2483,	3},
    {2492,	3},
    {2501,	3},
    {2510,	3},
    {2519,	2},
    {2523,	2},
    {2527,	2},
    {2531,	2},
    {2535,	3},
    {2544,	3},
    {2553,	3},
    {2562,	3},
    {2571,	3},
    {2580,	3},
    {2589,	4},
    {2605,	4},
    {2621,	4},
    {2637,	4},
    {2653,	4},
    {2669,	4},
    {2685,	4},
    {2701,	4},
    {2717,	4},
    {2733,	4},
    {2749,	4},
    {2765,	4},
    {2781,	4},
    {2797,	4},
    {2813,	4},
    {2829,	4},
    {2845,	4},
    {2861,	4},
    {2877,	4},
    {2893,	4},
    {2909,	4},
    {2925,	4},
    {2941,	4},
    {2957,	4},
    {2973,	3},
    {2982,	3},
    {2991,	3},
    {3000,	3},
    {3009,	2},
    {3013,	2},
    {3017,	2},
    {3021,	2},
    {3025,	3},
    {3034,	3},
    {3043,	3},
    {3052,	3},
    {3061,	3},
    {3070,	4},
    {3086,	4},
    {3102,	4},
    {3118,	4},
    {3134,	4},
    {3150,	4},
    {3166,	4},
    {3182,	4},
    {3198,	4},
    {3214,	4},
    {3230,	4},
    {3246,	4},
    {3262,	4},
    {3278,	4},
    {3294,	4},
    {3310,	4},
    {3326,	4},
    {3342,	4},
    {3358,	4},
    {3374,	4},
    {3390,	4},
    {3406,	3},
    {3415,	3},
    {3424,	3},
    {3433,	3},
    {3442,	3},
    {3451,	2},
    {3455,	2},
    {3459,	2},
    {3463,	3},
    {3472,	3},
    {3481,	3},
    {3490,	3},
    {3499,	3},
    {3508,	4},
    {3524,	4},
    {3540,	4},
    {3556,	4},
    {3572,	4},
    {3588,	4},
    {3604,	4},
    {3620,	4},
    {3636,	4},
    {3652,	4},
    {3668,	4},
    {3684,	4},
    {3700,	4},
    {3716,	4},
    {3732,	4},
    {3748,	4},
    {3764,	4},
    {3780,	4},
    {3796,	3},
    {3805,	3},
    {3814,	3},
    {3823,	3},
    {3832,	3},
    {3841,	3},
    {3850,	3},
    {3859,	3},
    {3868,	3},
    {3877,	3},
    {3886,	4},
    {3902,	4},
    {3918,	4},
    {3934,	4},
    {3950,	4},
    {3966,	4},
    {3982,	4},
    {3998,	4},
    {4014,	4},
    {4030,	4},
    {4046,	4},
    {4062,	4},
    {4078,	4},
    {4094,	4},
    {4110,	4},
    {4126,	3},
    {4135,	3},
    {4144,	3},
    {4153,	3},
    {4162,	3},
    {4171,	3},
    {4180,	3},
    {4189,	3},
    {4198,	4},
    {4214,	4},
    {4230,	4},
    {4246,	4},
    {4262,	4},
    {4278,	4},
    {4294,	4},
    {4310,	4},
    {4326,	4},
    {4342,	4},
    {4358,	4},
    {4374,	3},
    {4383,	3},
    {4392,	3},
    {4401,	3},
    {4410,	3},
    {4419,	3},
    {4428,	4},
    {4444,	4},
    {4460,	4},
    {4476,	4},
    {4492,	4},
    {4508,	4},
    {4524,	4},
    {4540,	3},
    {4549,	3},
    {4558,	3},
    {4567,	4},
    {4583,	4},
    {4599,	4},
    {4615,	4},
    {4631,	4},
    {4647,	4},
    {4663,	3},
    {4672,	3},
    {4681,	2},
    {4685,	2},
    {4689,	3},
    {4698,	3},
    {4707,	3},
    {4716,	3},
    {4725,	3},
    {4734,	4},
    {4750,	4},
    {4766,	4},
    {4782,	4},
    {4798,	4},
    {4814,	4},
    {4830,	4},
    {4846,	4},
    {4862,	4},
    {4878,	4},
    {4894,	3},
    {4903,	3},
    {4912,	3},
    {4921,	3},
    {4930,	3},
    {4939,	3},
    {4948,	3},
    {4957,	3},
    {4966,	3},
    {4975,	3},
    {4984,	4},
    {5000,	4},
    {5016,	4},
    {5032,	4},
    {5048,	4},
    {5064,	4},
    {5080,	4},
    {5096,	4},
    {5112,	4},
    {5128,	4},
    {5144,	4},
    {5160,	4},
    {5176,	4},
    {5192,	4},
    {5208,	4},
    {5224,	3},
    {5233,	3},
    {5242,	3},
    {5251,	3},
    {5260,	3},
    {5269,	2},
    {5273,	2},
    {5277,	2},
    {5281,	2},
    {5285,	3},
    {5294,	3},
    {5303,	3},
    {5312,	3},
    {5321,	3},
    {5330,	4},
    {5346,	4},
    {5362,	4},
    {5378,	4},
    {5394,	4},
    {5410,	4},
    {5426,	4},
    {5442,	4},
    {5458,	4},
    {5474,	4},
    {5490,	3},
    {5499,	3},
    {5508,	3},
    {5517,	3},
    {5526,	3},
    {5535,	3},
    {5544,	3},
    {5553,	3},
    {5562,	4},
    {5578,	4},
    {5594,	4},
    {5610,	4},
    {5626,	4},
    {5642,	4},
    {5658,	4},
    {5674,	4},
    {5690,	4},
    {5706,	4},
    {5722,	4},
    {5738,	4},
    {5754,	4},
    {5770,	4},
    {5786,	4},
    {5802,	3},
    {5811,	3},
    {5820,	3},
    {5829,	3},
    {5838,	3},
    {5847,	2},
    {5851,	2},
    {5855,	2},
    {5859,	2},
    {5863,	3},
    {5872,	3},
    {5881,	3},
    {5890,	3},
    {5899,	4},
    {5915,	4},
    {5931,	4},
    {5947,	4},
    {5963,	4},
    {5979,	4},
    {5995,	4},
    {6011,	4},
    {6027,	4},
    {6043,	4},
    {6059,	4},
    {6075,	3},
    {6084,	3},
    {6093,	3},
    {6102,	3},
    {6111,	3},
    {6120,	4},
    {6136,	4},
    {6152,	4},
    {6168,	4},
    {6184,	4},
    {6200,	4},
    {6216,	4},
    {6232,	4},
    {6248,	4},
    {6264,	4},
    {6280,	4},
    {6296,	4},
    {6312,	4},
    {6328,	4},
    {6344,	4},
    {6360,	4},
    {6376,	3},
    {6385,	3},
    {6394,	3},
    {6403,	3},
    {6412,	3},
    {6421,	2},
    {6425,	2},
    {6429,	2},
    {6433,	2},
    {6437,	3},
    {6446,	3},
    {6455,	3},
    {6464,	3},
    {6473,	4},
    {6489,	4},
    {6505,	4},
    {6521,	4},
    {6537,	4},
    {6553,	4},
    {6569,	4},
    {6585,	4},
    {6601,	4},
    {6617,	4},
    {6633,	4},
    {6649,	4},
    {6665,	4},
    {6681,	4},
    {6697,	4},
    {6713,	4},
    {6729,	4},
    {6745,	4},
    {6761,	4},
    {6777,	4},
    {6793,	4},
    {6809,	4},
    {6825,	4},
    {6841,	4},
    {6857,	4},
    {6873,	4},
    {6889,	4},
    {6905,	4},
    {6921,	4},
    {6937,	4},
    {6953,	3},
    {6962,	3},
    {6971,	3},
    {6980,	3},
    {6989,	3},
    {6998,	2},
    {7002,	2},
    {7006,	2},
    {7010,	3},
    {7019,	3},
    {7028,	3},
    {7037,	3},
    {7046,	3},
    {7055,	4},
    {7071,	4},
    {7087,	4},
    {7103,	4},
    {7119,	4},
    {7135,	4},
    {7151,	4},
    {7167,	4},
    {7183,	4},
    {7199,	4},
    {7215,	4},
    {7231,	4},
    {7247,	4},
    {7263,	4},
    {7279,	4},
    {7295,	4},
    {7311,	4},
    {7327,	4},
    {7343,	4},
    {7359,	4},
    {7375,	4},
    {7391,	4},
    {7407,	4},
    {7423,	4},
    {7439,	4},
    {7455,	4},
    {7471,	4},
    {7487,	3},
    {7496,	3},
    {7505,	3},
    {7514,	3},
    {7523,	3},
    {7532,	3},
    {7541,	3},
    {7550,	3},
    {7559,	3},
    {7568,	3},
    {7577,	3},
    {7586,	3},
    {7595,	4},
    {7611,	4},
    {7627,	4},
    {7643,	4},
    {7659,	4},
    {7675,	4},
    {7691,	4},
    {7707,	4},
    {7723,	4},
    {7739,	4},
    {7755,	4},
    {7771,	4},
    {7787,	4},
    {7803,	4},
    {7819,	4},
    {7835,	4},
    {7851,	4},
    {7867,	4},
    {7883,	4},
    {7899,	4},
    {7915,	4},
    {7931,	4},
    {7947,	4},
    {7963,	4},
    {7979,	4},
    {7995,	3},
    {8004,	3},
    {8013,	3},
    {8022,	3},
    {8031,	3},
    {8040,	3},
    {8049,	3},
    {8058,	3},
    {8067,	3},
    {8076,	3},
    {8085,	4},
    {8101,	4},
    {8117,	4},
    {8133,	4},
    {8149,	4},
    {8165,	4},
    {8181,	4},
    {8197,	4},
    {8213,	4},
    {8229,	4},
    {8245,	4},
    {8261,	4},
    {8277,	4},
    {8293,	4},
    {8309,	4},
    {8325,	4},
    {8341,	4},
    {8357,	4},
    {8373,	4},
    {8389,	4},
    {8405,	4},
    {8421,	4},
    {8437,	3},
    {8446,	3},
    {8455,	3},
    {8464,	3},
    {8473,	3},
    {8482,	3},
    {8491,	3},
    {8500,	3},
    {8509,	3},
    {8518,	4},
    {8534,	4},
    {8550,	4},
    {8566,	4},
    {8582,	4},
    {8598,	4},
    {8614,	4},
    {8630,	4},
    {8646,	4},
    {8662,	4},
    {8678,	4},
    {8694,	4},
    {8710,	4},
    {8726,	4},
    {8742,	4},
    {8758,	4},
    {8774,	4},
    {8790,	4},
    {8806,	4},
    {8822,	3},
    {8831,	3},
    {8840,	3},
    {8849,	3},
    {8858,	3},
    {8867,	3},
    {8876,	4},
    {8892,	4},
    {8908,	4},
    {8924,	4},
    {8940,	4},
    {8956,	4},
    {8972,	4},
    {8988,	4},
    {9004,	4},
    {9020,	4},
    {9036,	4},
    {9052,	4},
    {9068,	4},
    {9084,	4},
    {9100,	4},
    {9116,	3},
    {9125,	3},
    {9134,	3},
    {9143,	3},
    {9152,	3},
    {9161,	4},
    {9177,	4},
    {9193,	4},
    {9209,	4},
    {9225,	4},
    {9241,	4},
    {9257,	4},
    {9273,	4},
    {9289,	4},
    {9305,	4},
    {9321,	4},
    {9337,	3},
    {9346,	4},
    {9362,	4},
    {9378,	4},
    {9394,	4},
    {9410,	4},
    {9426,	4},
    {9442,	4},
    {9458,	4},
    {9474,	4},
    {9490,	4},
    {9506,	4},
    {9522,	4},
};

/* given an ra and dec, each in rads fill in the *rp GSCRegion with the name of
 *   the file (relative to cdpath), n/s hemisphere, region id and the
 *   origin ra/dec of the region (also in rads).
 * This is all based upon the comments found on the CD-ROM files in tables/
 *   sm_reg_x.tbl, lg_reg_x.tbl and regions.tbl.
 */
void
gscPickRegion (double ra, double dec, GSCRegion *rp)
{
	int decband;	/* which 7.5-degree dec band: 0..11 */
	double dra;	/* change in ra in large region */
	double smdra;	/* change in ra in small region */
	double smddec;	/* change in dec in small region */
	int lgregno;	/* large region number */
	int rareg;	/* which dra-degree ra bad: 0..lg_reg[decband].nlrg-1 */
	int firstsm;	/* first small region number in this large region */
	int nsub;	/* number of subdivisions in this large region */
	int nsmdec, nsmra;

	/* convert to degs for remainder of algoorithm */
	ra = raddeg(ra);
	dec = raddeg(dec);

	/* insure 0 <= ra < 360 */
	range (&ra, 360.0);

	/* insure -90 < dec < 90 */
	if (dec <= -90.0)
	    dec = (-180.0 + 0.01/3600.0) - dec;
	else if (dec >= 90.0)
	    dec = ( 180.0 - 0.01/3600.0) - dec;

	/* work in the nothern hemisphere for the most part and
	 * start building dir.
	 */
	if (dec < 0.0) {
	    rp->south = 1;
	    dec = -dec;
	    rp->dir[0] = 's';
	} else {
	    rp->south = 0;
	    rp->dir[0] = 'n';
	}

	/* find which 7.5-degree dec band and add more to dir */
	decband = (int)floor(dec/7.5);
	(void) sprintf (rp->dir+1, "%04d", decband*750);
	if (rp->dir[3] == '5')
	    rp->dir[3] = '3';

	/* find the large region number */
	dra = 360.0/lg_reg[decband].nlrg;
	rareg = (int)floor(ra/dra);
	lgregno = lg_reg[decband].firstlg + rareg;
	if (rp->south)
	    lgregno += 366;

	/* find the size of the subdivisions */
	firstsm = sm_reg[lgregno].firstsm;
	nsub = sm_reg[lgregno].nsub;
	smdra = dra/nsub;
	smddec = 7.5/nsub;

	/* find the origin of the subdivision */
        nsmdec = (int)floor((dec - decband*7.5)/smddec);
	rp->dec0 = decband*7.5 + nsmdec*smddec;
	rp->dec0 = degrad(rp->dec0);
	nsmra  = (int)floor((ra - rareg*dra)/smdra);
	rp->ra0 = rareg*dra + nsmra*smdra;
	rp->ra0 = degrad(rp->ra0);

	/* small region number is then the first in this large region plus
	 * the number of subdivisions away from the origin.
	 * the large region is broken up into a square array of nsub*nsub.
	 */
	rp->id = firstsm + nsub*nsmdec + nsmra;

	/* fill in the filename */
	(void) sprintf (rp->file, "%04d.gsc", rp->id);
}

/* open and setup rp->fp and nrows from an existing cdrom file off path[] for
 *   the given region so it is ready to return successive entries.
 * return 0 if ok, else write error message to stderr and outfp and return -1.
 */
int
gscOpenRegion (char path[], GSCRegion *rp)
{
	typedef enum {FINDNAXIS2, FINDEND, SKIPTOBLOCK, FOUNDTABLE} State;
	State state;
	char buf[81];	/* each FITS line is exactly 80 chars */
	char fullpath[1024];
	int n;

	/* open the cdrom file */
	(void) sprintf (fullpath, "%s/gsc/%s/%s", path, rp->dir, rp->file);
	rp->fp = fopen (fullpath, "r");
	if (!rp->fp) {
	    fprintf(stderr, "Can not open %s: %s\n", fullpath, strerror(errno));
	    if (outfp)
		fprintf(outfp, "!Can not open %s: %s\n", fullpath,
							    strerror(errno));
	    return (-1);
	}

	/* find start of table and its length.
	 * set rp->fp to its beginning.
	 */
	for (n = 1, state = FINDNAXIS2; state != FOUNDTABLE; n++) {
	    if (fgets (buf, 81, rp->fp) == NULL) {
		fprintf(stderr, "%s: unexpected EOF\n", fullpath);
		if (outfp)
		    fprintf(outfp, "!%s: unexpected EOF\n", fullpath);
		fclose (rp->fp);
		rp->fp = NULL;
		return (-1);
	    }

	    switch (state) {
	    case FINDNAXIS2:
		if (sscanf (buf, "NAXIS2  =%d", &rp->nrows) == 1)
		    state = FINDEND;
		break;
	    case FINDEND:
		if (strncmp (buf, "END", 3) == 0) {
		    if (((n*80)%2880) == 0)
			state = FOUNDTABLE;
		    else
			state = SKIPTOBLOCK;
		}
		break;
	    case SKIPTOBLOCK:
		if (((n*80)%2880) == 0)
		    state = FOUNDTABLE;
		break;
	    case FOUNDTABLE:
		break;
	    }

	}

	return (0);
}

/* return the next entry from the GSC region.
 * decrement rp->nrows as we go and return -1 when it reaches 0, else 0.
 */
int
gscGetNextEntry (GSCRegion *rp, GSCEntry *ep)
{
	char buf[32];

	/* can't use scanf because we are breaking on columns not whitespace
	 * so just read in in pieces and generate output as we go.
	 * I5	ID within region
	 * F9	RA and Dec, degrees
	 * F5	position error, arc seconds
	 * F5	magnitude
	 * F4	magnitude error
	 * I2	magnitude band
	 * I1	classification
	 * A4	internal plate number
	 * A1	T/F whether there are more
	 */

	if (rp->nrows <= 0)
	    return (-1);

	(void) fgets (buf, 6, rp->fp);		/* ID */
	ep->id = atoi (buf);
	(void) fgets (buf, 10, rp->fp);		/* RA */
	(void) sscanf (buf, "%lf", &ep->ra);
	ep->ra = degrad(ep->ra);
	(void) fgets (buf, 10, rp->fp);		/* Dec */
	(void) sscanf (buf, "%lf", &ep->dec);
	ep->dec = degrad(ep->dec);
	(void) fgets (buf, 6, rp->fp);		/* poserr */
	(void) fgets (buf, 6, rp->fp);		/* mag */
	(void) sscanf (buf, "%lf", &ep->mag);
	(void) fgets (buf, 5, rp->fp);		/* mag error */
	(void) fgets (buf, 3, rp->fp);		/* mag band */
	(void) fgets (buf, 2, rp->fp);		/* class */
	ep->class = atoi (buf);
	(void) fgets (buf, 6, rp->fp);		/* plate no and more */

	rp->nrows -= 1;
	return (0);
}

/* do whatever cleanup might be required when finished with rp
 */
void
gscCloseRegion (GSCRegion *rp)
{
	fclose (rp->fp);
	rp->fp = NULL;
}
