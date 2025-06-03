/*
** Astrolog (Version 2.25) File: data.c
**
** IMPORTANT: The planetary calculation routines used in this program
** have been Copyrighted and the core of this program is basically a
** conversion to C of the routines created by James Neely as listed in
** Michael Erlewine's 'Manual of Computer Programming for Astrologers',
** available from Matrix Software. The copyright gives us permission to
** use the routines for our own purposes but not to sell them or profit
** from them in any way.
**
** IN ADDITION: the graphics database and chart display routines used in
** this program are Copyright (C) 1991-1992 by Walter D. Pullen. Permission
** is granted to freely use and distribute these routines provided one
** doesn't sell, restrict, or profit from them in any way. Modification
** is allowed provided these notices remain with any altered or edited
** versions of the program.
*/

#include "astrolog.h"

/*
*******************************************************************************
** Global variables
*******************************************************************************
*/

FILE *data;
char *filename, *filename2;

int total    = TOTAL;
int objects  = OBJECTS;
int stars    = STARS;
int aspects  = 5;
int universe = FALSE;
int siderial = FALSE;
int right    = FALSE;
int centerplanet   = 1;
int todisplay      = 0;
int housesystem    = DEFAULT_SYSTEM;
int cusp           = FALSE;
int uranian        = FALSE;
int multiplyfactor = 1;
int onasc     = 0;
int flip      = FALSE;
int decan     = FALSE;
int geodetic  = FALSE;
int graphstep = 5;
int progress  = FALSE;
int autom     = FALSE;
int relation  = FALSE;
int operation = 0;
double progday = 365.25;
double defzone = DEFAULT_ZONE;
double deflong = DEFAULT_LONG;
double deflat  = DEFAULT_LAT;
double modulus = DEGREES;

double SD = 0.0, Mon, Day, Yea, Tim, Zon, Lon, Lat, Mon2, Yea2, Delta = 0.0,
  M = 11.0, D = 19.0, Y = 1971.0, F = 11.01, X = 8.0, L5 = 122.20, LA = 47.36,
  T, JD, Jdp;

double planet[TOTAL+1], planetalt[TOTAL+1], house[SIGNS+1], ret[TOTAL+1],
  spacex[BASE+1], spacey[BASE+1], spacez[BASE+1];
int inhouse[TOTAL+1], grid[TOTAL+1][TOTAL+1],
  gridname[TOTAL+1][TOTAL+1], wheel[SIGNS][WHEELROWS],
  inhouse1[TOTAL+1], inhouse2[TOTAL+1], starname[STARS+1];
int ignore[TOTAL+1]
  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                /* Planets  */
#ifdef ASTEROIDS
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   /* Minors   */
#else
     1, 1, 1, 1, 1, 0, 1, 0, 0, 1,
#endif
     0, 0, 0, 0,                                     /* Cusps    */
     0, 0, 0, 0, 0, 0, 0, 0,                         /* Uranians */
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* Stars    */
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


/*
*******************************************************************************
** Table data
*******************************************************************************
*/

char *signname[]
  = {"???", "Aries", "Taurus", "Gemini", "Cancer",
       "Leo", "Virgo", "Libra", "Scorpio",
       "Sagittarius", "Capricorn", "Aquarius", "Pisces"};

char signabbrev[][3]
  = {"??",
       "Ar", "Ta", "Ge", "Cn", "Le", "Vi", "Li", "Sc", "Sg", "Cp", "Aq", "Pi"};

char objectname[][12]
  = {"Earth", "Sun", "Moon", "Mercury", "Venus", "Mars",      /* Planets   */
       "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto",
       "Chiron", "Ceres", "Pallas", "Juno", "Vesta",          /* Asteroids */
       "Node", "Fortune", "Midheaven", "Ascendant", "Vertex", /* Others    */
       "11th Cusp", "12th Cusp", "2nd Cusp", "3rd Cusp",      /* Cusps     */
       "Cupido", "Hades", "Zeus", "Kronos",                   /* Uranians  */
       "Apollo", "Admetos", "Vulkanus", "Poseidon",

       "Achernar", "Polaris", "Zeta Retic.", "Pleiades",      /* Stars     */
       "Aldebaran", "Capella", "Rigel", "Bellatrix", "Alnath",
       "Orion", "Betelgeuse", "Menkalinan", "Murzim",
       "Canopus", "Alhena", "Sirius", "Adara", "Wezen",
       "Castor", "Procyon", "Pollux", "Suhail", "Avior",
       "Miaplacidus", "Alphard", "Regulus", "Dubhe", "Acrux",
       "Gacrux", "Becrux", "Alioth", "Spica", "Alkaid",
       "Agena", "Arcturus", "Kentaurus", "Antares", "Shaula",
       "Sargas", "Australis", "Vega", "Altair", "Peacock",
       "Deneb", "Alnair", "Fomalhaut"};

char *systemname[]
  = {"Placidus", "Koch", "Equal", "Campanus", "Meridian",
       "Regiomontanus", "Porphyry", "Morinus", "Topocentric", "Null"};

char *aspectname[]
  = {"???", "Conjunct", "Opposite", "Square", "Trine", "Sextile",
       "Inconjunct", "Semisextile", "Semisquare", "Sesquiquadrate",
       "Quintile", "Biquintile",
       "Semiquintile", "Septile", "Novile",
       "Binovile", "Biseptile", "Triseptile", "Quatronovile"};

char *aspectglyph[]
 = {"???", "Circle with extending line", "Two circles joined by line",
      "Quadrilateral", "Triangle", "Six pointed asterisk", "'K' rotated right",
      "'K' rotated left", "Acute angle", "Square with extending lines",
      "Letter 'Q'", "'+' over '-'", "'-' over '+'", "Number '7'", "Number '9'",
      "'9' under Roman 'II'", "'7' under Roman 'II'", "'7' under Roman 'III'",
      "'9' under Roman 'IV'"};

char aspectabbrev[][4]
  = {"   ", "Con", "Opp", "Squ", "Tri", "Sex",
       "Inc", "SSx", "SSq", "Ses", "Qui", "BQn",
       "SQn", "Sep", "Nov", "BNv", "BSp", "TSp", "QNv"};

double aspectangle[ASPECTS+1]
  = {0, 0.0, 180.0, 90.0, 120.0, 60.0, 150.0, 30.0, 45.0, 135.0, 72.0, 144.0,
       36.0, DEGREES/7.0, 40.0, 80.0, 720.0/7.0, 1080.0/7.0, 160.0};

double aspectorb[ASPECTS+1]
  = {0, 7.0, 7.0, 7.0, 7.0, 6.0, 3.0, 3.0, 3.0, 3.0, 2.0, 2.0,
       1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

int ruler1[OBJECTS+1]
  = {0, 5, 4,  3,  7,  1,  9, 10, 11, 12, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ruler2[OBJECTS+1]
  = {0, 0, 0,  6,  2,  8, 12, 11,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int exalt[OBJECTS+1]
  = {0, 1, 2, 11, 12, 10,  4,  7,  8,  9, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

char *element[] = {"Fire", "Earth", "Air", "Water"};
char *mode[] = {"Cardinal", "Fixed", "Mutuable"};

char post[][3]
  = {"",
       "st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th", "th", "th"};

char *monthname[]
  = {"", "January", "February", "March", "April", "May", "June", "July",
       "August", "September", "October", "November", "December"};

#ifdef X11
char *signdraw[] = {"",
  "ND4HU2HLGDFBR6EUHLGD2G", "BL3D2F2R2E2U2H2NE2L2NH2G2",
  "BLU3LHBR7GLNL3D6NL3RFBL7ERU3", "BGNDHLGDFRNEFR2EREBU3NHDGLHUENRHL2GLG",
  "BF4H2UEU2H2L2G2D2FDGH", "BF4HNGNHEU5GHND5HGND6HGND6H",
  "BGNL3HUER2FDGR3BD2L8", "BH4FND6EFND6EFD6FREU",
  "BG4E3NH2NF2E5NL2D2", "BH3NLNUD3FND2EU2ENF2UFERFDGLF2D2G",
  "BG4EUEDFDEUEDFDEUEBU5GDGUHUGDGUHUGDG", "NL4NR4BH4F2D4G2BR8H2U4E2"};

char *objectdraw[] = {"ND4NL4NR4U4LGLDGD2FDRFR2ERUEU2HULHL", /* Earth */
  "U0BH3DGD2FDRFR2ERUEU2HULHL2GL", /* Sun */
  "BG3E2U2H2ER2FRDFD2GDLGL2H", /* Moon */
  "BD4UNL2NR2U2REU2HNEL2NHGD2FR", /* Mercury */
  "LHU2ER2FD2GLD2NL2NR2D2", /* Venus */
  "HLG2DF2RE2UHE4ND2L2", /* Mars */
  "BH3RFDGDGDR5NDNR2U6E", /* Jupiter */
  "BH3R2NUNR2D3ND3RERFDGDF", /* Saturn */
  "BD4NEHURBFULU3NUNR2L2NU2DGBU5NFBR6GD3F", /* Uranus */
  "BD4U2NL2NR2U5NUNRLBL2NUNLDF2R2E2UNRU", /* Neptune */
  "D2NL2NR2D2BU8GFEHBL3D2F2R2E2U2", /* Pluto */
  "BG2LDFEULU3NURFRFBU5GLGLU2", /* Chiron */
  "BD4UNL3NR3U2RE2UH2L2G", /* Ceres */
  "BD4UNL3NR3UE2HUHNUGDGF2", /* Pallas Athena */
  "BD4UNL2NR2U4NL4NR4NE3NF3NG3NH3U3", /* Juno */
  "BU4DBG3NLFDF2E2UERBH2GDGHUH", /* Vesta */
  "BG2LGFEU2HU2E2R2F2D2GD2FEHL", /* North Node */
  "NE2NF2NG2H2GD2F2R2E2U2H2L2G", /* Part of Fortune */
  "BG4U8F4E4D8", /* Midheaven */
  "NR4L4ND4UE3R2F3D5", /* Ascendant */
  "U2NHNEBD4NGNFU2L2NHNGR4NEF" /* Vertex */};

char *housedraw[] = {"",
  "BD2NLNRU4L", "BHBUR2D2L2D2R2", "BHBUR2D2NL2D2L2",
  "BHBUD2R2NU2D2", "BEBUL2D2R2D2L2", "NLRD2L2U4R2",
  "BHBUR2DG2D", "NRLU2R2D4L2U2", "NRLU2R2D4L2",
  "BH2NLD4NLRBR2U4R2D4L2", "BH2NLD4NLRBR2RNRU4L", "BH2NLD4NLRBR2NR2U2R2U2L2"};

char *aspectdraw[] = {"",
  "HLG2DF2RE2UHE4", "BGL2GDFREU2E2U2ERFDGL2", "BH4R8D8L8U8",
  "BU4GDGDGDGDR8UHUHUHUH", "BLNH3NG3RNU4ND4RNE3F3",
  "BG4EUEUEUEUNL4NR4BDFDFDFDF", "BH4FDFDFDFDNL4NR4BUEUEUEUE", "BE4G8R8",
  "BD2L3U6R6D6L3D2R2", "F4BU3U2HULHL2GLDGD2FDRFR2E3", "BD2U3NR3NU3L3BD5R6",
  "BU2D3NR3ND3L3BU5R6", "BH3R6G6", "BR3L5HUER4FD4GL4H",
  "BF2UHL2GFR3DGL3BE6LNLU2NRLBL4LNLD2NLR", "BL2R4G4BE6LNLU2NRLBL4LNLD2NLR",
  "BL2R4G4BE6L7NLU2NLR3ND2R3ND2R", "BF2UHL2GFR3DGL3BU6LNLU2NLRBR2F2E2"};
#endif

double starbright[] = {0,
  0.46,2.02,5.24,5.09,0.85,0.08,0.12,1.64,1.65,1.70,0.50,1.90,1.98,-0.72,1.93,
  -1.46,1.50,1.84,1.59,0.38,1.14,1.78,1.86,1.68,1.98,1.35,1.79,1.58,1.63,1.25,
  1.77,0.98,1.86,0.61,-0.04,-0.01,0.96,1.63,1.87,1.85,0.03,0.77,1.94,1.25,1.74,
  1.16};

double stardata[] = {
 1,37,42.9,-57,14,12,   2,31,50.5, 89,15,51,   3,17,46.1,-62,34,32,
 3,49,11.1, 24, 8,12,   4,35,55.2, 16,30,33,   5,16,41.3, 45,59,53,
 5,14,32.2, -8,12, 6,   5,25, 7.8,  6,20,59,   5,26,17.5, 28,36,27,
 5,36,12.7, -1,12, 7,   5,55,10.3,  7,24,25,   5,59,31.7, 44,56,51,
 6,22,41.9,-17,57,22,   6,23,57.2,-52,41,44,   6,37,42.7, 16,23,57,
 6,45, 8.9,-16,42,58,   6,58,37.5,-28,58,20,   7, 8,23.4,-26,23,35,
 7,34,35.9, 31,53,18,   7,39,18.1,  5,13,30,   7,45,18.9, 28, 1,34,
 8, 9,31.9,-47,20,12,   8,22,30.8,-59,30,34,   9,13,12.1,-69,43, 2,
 9,27,35.2, -8,39,31,  10, 8,22.3, 11,58,2,   11, 3,43.6, 61,45, 3,
12,26,35.9,-63, 5,56,  12,31, 9.9,-57, 6,47,  12,47,43.3,-59,41,19,
12,54, 1.7, 55,57,35,  13,25,11.5,-11, 9,41,  13,47,32.3, 49,18,48,
14, 3,49.4,-60,22,22,  14,15,39.6, 19,10,57,  14,39,36.2,-60,50, 7,
16,29,24.4,-26,25,55,  17,33,36.4,-37, 6,13,  17,37,19.0,-42,59,52,
18,24,10.3,-34,23, 5,  18,36,56.2, 38,47, 1,  19,50,46.9,  8,52, 6,
20,25,38.8,-56,44, 7,  20,41,25.8, 45,16,49,  22, 8,13.9,-46,57,40,
22,57,39.0,-29,37,20};

int errorcount[PLANETS+1] = {0, 0, 0, 0, 0, 0, 11, 5, 4, 4, 4};

double planetdata[] = {
358.4758,35999.0498,-.0002,.01675,-.4E-4,0,1,101.2208,1.7192,.00045,0,0,0,0,0,
0, /* Sun/Earth */
102.2794,149472.515,0,.205614,.2E-4,0,.3871,28.7538,.3703,.0001,47.1459,1.1852,
0.0002,7.009,.00186,0, /* Mercury */
212.6032,58517.8039,.0013,.00682,-.5E-4,0,.7233,54.3842,.5082,-.14E-2,75.7796,
0.8999,.4E-3,3.3936,.1E-2,0, /* Venus */
319.5294,19139.8585,.2E-3,.09331,.9E-4,0,1.5237,285.4318,1.0698,.1E-3,48.7864,
0.77099,0,1.8503,-.7E-3,0, /* Mars */
225.4928,3033.6879,0,.04838,-.2E-4,0,5.2029,273.393,1.3383,0,99.4198,1.0583,0,
1.3097,-.52E-2,0, /* Jupiter */
-.001,-.0005,.0045,.0051,581.7,-9.7,-.0005,2510.7,-12.5,-.0026,1313.7,-61.4,
0.0013,2370.79,-24.6,-.0013,3599.3,37.7,-.001,2574.7,31.4,-.00096,6708.2,
-114.5,-.0006,5499.4,-74.97,-.0013,1419,54.2,.0006,6339.3,-109,.0007,4824.5,
-50.9,.0020,-.0134,.0127,-.0023,676.2,.9,.00045,2361.4,174.9,.0015,1427.5,
-188.8,.0006,2110.1,153.6,.0014,3606.8,-57.7,-.0017,2540.2,121.7,-.00099,
6704.8,-22.3,-.0006,5480.2,24.5,.00096,1651.3,-118.3,.0006,6310.8,-4.8,.0007,
4826.6,36.2, /* Jupiter error */
174.2153,1223.50796,0,.05423,-.2E-3,0,9.5525,338.9117,-.3167,0,112.8261,.8259,
0,2.4908,-.0047,0, /* Saturn */
-.0009,.0037,0,.0134,1238.9,-16.4,-.00426,3040.9,-25.2,.0064,1835.3,36.1,
-.0153,610.8,-44.2,-.0015,2480.5,-69.4,-.0014,.0026,0,.0111,1242.2,78.3,-.0045,
3034.96,62.8,-.0066,1829.2,-51.5,-.0078,640.6,24.2,-.0016,2363.4,-141.4,.0006,
-.0002,0,-.0005,1251.1,43.7,.0005,622.8,13.7,.0003,1824.7,-71.1,.0001,2997.1,
78.2, /* Saturn error */
74.1757,427.2742,0,.04682,.00042,0,19.2215,95.6863,2.0508,0,73.5222,.5242,0,
0.7726,.1E-3,0, /* Uranus */
-.0021,-.0159,0,.0299,422.3,-17.7,-.0049,3035.1,-31.3,-.0038,945.3,60.1,-.0023,
1227,-4.99,.0134,-.02186,0,.0317,404.3,81.9,-.00495,3037.9,57.3,.004,993.5,
-54.4,-.0018,1249.4,79.2,-.0003,.0005,0,.0005,352.5,-54.99,.0001,3027.5,54.2,
-.0001,1150.3,-88, /* Uranus error */
30.13294,240.45516,0,.00913,-.00127,0,30.11375,284.1683,-21.6329,0,130.68415,
1.1005,0,1.7794,-.0098,0, /* Neptune */
0.1832,-.6718,.2726,-.1923,175.7,31.8,.0122,542.1,189.6,.0027,1219.4,178.1,
-.00496,3035.6,-31.3,-.1122,.166,-.0544,-.00496,3035.3,58.7,.0961,177.1,-68.8,
-.0073,630.9,51,-.0025,1236.6,78,.00196,-.0119,.0111,.0001,3049.3,44.2,-.0002,
893.9,48.5,.00007,1416.5,-25.2, /* Neptune error */
229.781,145.1781,0,.24797,.002898,0,39.539,113.5366,.2086,0,108.944,1.3739,0,
17.1514,-.0161,0, /* Pluto */
-.0426,.073,-.029,.0371,372,-331.3,-.0049,3049.6,-39.2,-.0108,566.2,318.3,
0.0003,1746.5,-238.3,-.0603,.5002,-.6126,.049,273.97,89.97,-.0049,3030.6,61.3,
0.0027,1075.3,-28.1,-.0007,1402.3,20.3,.0145,-.0928,.1195,.0117,302.6,-77.3,
0.00198,528.1,48.6,-.0002,1000.4,-46.1, /* Pluto error */
34.6127752,713.5756219,0,.382270369,-.004694073,0,13.66975144,337.407213,
2.163306646,0,208.1482658,1.247724355,0,6.911179715,.011236955,0, /* chiron */
108.2925,7820.36556,0,.0794314,0,0,2.7672273,71.0794444,0,0,80.23555556,
1.3960111,0,10.59694444,0,0, /* Ceres */
106.6641667,7806.531667,0,.2347096,0,0,2.7704955,310.166111,0,0,172.497222,
1.39601111,0,34.81416667,0,0, /* Pallas athena */
267.685,8256.081111,0,.2562318,0,0,2.6689897,245.3752778,0,0,170.137777,
1.396011111,.0003083333,13.01694444,0,0, /* Juno */
138.7733333,9924.931111,0,.0902807,0,0,2.360723,149.6386111,0,0,103.2197222,
1.396011111,.000308333,7.139444444,0,0, /* Vesta */
104.5959,138.5369,0,0,0,0,40.99837, 0,0,0,0,0,0,0,0,0, /* Cupido */
337.4517,101.2176,0,0,0,0,50.667443,0,0,0,0,0,0,0,0,0, /* Hades */
104.0904,80.4057, 0,0,0,0,59.214362,0,0,0,0,0,0,0,0,0, /* Zeus */
17.7346, 70.3863, 0,0,0,0,64.816896,0,0,0,0,0,0,0,0,0, /* Kronos */
138.0354,62.5,    0,0,0,0,70.361652,0,0,0,0,0,0,0,0,0, /* Apollo */
-8.678,  58.3468, 0,0,0,0,73.736476,0,0,0,0,0,0,0,0,0, /* Admetos */
55.9826, 54.2986, 0,0,0,0,77.445895,0,0,0,0,0,0,0,0,0, /* Vulkanus */
165.3595,48.6486, 0,0,0,0,83.493733,0,0,0,0,0,0,0,0,0  /* Poseidon */
};

#ifdef X11
char *worlddata[] = {
"-031+70",
"LLRRHLLLLDULLGLLLDULGLLLGLRREDEGGLGGLGLGLLGDRLDRLFFRRERFDFRRREUEEHLUERERUERRF\
GLGLDDFRRRRREFRLGLLLLLGEFDLHGDDLGHLGLLHGLHURDLRRELLLRHUGLDFDLGLLFHGGLGLLLDLLLD\
RRFFDDGLLLLLLGDFGDDRRFRERREEUEREUEFRRERRFFFRFRDDLLLLRFRUREURULHLHHHEF",
"5EUROPE",
"+006+50", "RRERRRRUELLUHHLLREULLELLDGHDUFDEGRDRRLFDLLRGRRGGL", "5ENGLAND",
"+008+55", "GLFGRRREUULL", "5IRELAND",
"+023+64", "RRFRERRREHLLLLLGHLLRFLLRFL", "5ICELAND",
"-011+80", "DDURFRERLGRRLLFRRREEFRRRLHGELLLHRRFRRRRERLLLLLLLLLLLDHGULLL",
"5SVALBARD",
"-014+45",
"FRFRFDDFRDRRLLFRURFHHUERRRRRHUUEERRRRGRDERRLHLRRERRGGRFRFFGLLLLHLLLLGLLDLLLFG\
RFFRERFRERDDDGDGLLDFFEUDDFFDFFDDFFFDFDDDRRERRERRRUERRERURUEEHHLHUGGLLLUUGUHUHU\
RRFFRFRRRDRRFRRRRRRRF",
"5MIDDLE EAST",
"-009+41", "DDRUULEUGD", "5SARDINIA",
"-024+35", "RRLL", "5CRETE",
"-032+35", "RRLL", "5CYPRUS",
"-052+37", "LLHUURHUHUHERERRRDDLLLFFDDURFLLDFDDL", "0CASPAIN SEA",
"-060+44", "LLUEERDFLDL", "0ARAL SEA",
"-068+24",
"FRGFRREDDDDDFDFDDFDDFERUEUUUUEEEEEREURRREFDFRDDDDRREFDDFDDGDDRFDDFDFFRUHUUHHH\
ULUEUUURDRFDFRDEEREUUUHHHUUEERRDDEURRERREREEEUEULLREUHUHLEERRHLGLULUREERDLDRER\
RFGRFDGRRREUHHUREUE",
"6ASIA S",
"-140+36",
"DEUUEUHURREREEGLLHHDDGLDRGDDGGLGLLLGGLDLRDFEUHRRGEERDLLRGLRERRERRE",
"6JAPAN",
"-121+25", "GDFUEUL", "6TAIWAN",
"-080+10", "DDDDREUHH", "6SRI LANKA",
"-121+18", "LDDDRDDRHRRFFDDDLLEHDULRHDFDDGERDDREUUULUUHHLHEUUL",
"2PHILIPPINES",
"-131+43",
"EFREEREEEUUUEUHLLUDLULEERERERRRRRRERRFLRRRRLUERERRRDRERURRGDLGLGLGLGGDDFDFEUR\
RUERUURULEEREDERRFRERERRRERRHLHLRRRREURDRRFRFRUURRHLLLDHHLLHLLHLLLLLLLDLLHRLLL\
LLLLGHULLLLLLLLLLULLLGL",
"6SIBERIA",
"-145+71",
"RELLRHLLLLGDHGHLLLLGLLHUHLLLLLDLLLLHLLLLLDULUDLGLLLLRRERERRRELHLLLLLLLELLLLGD\
LLLLLUDLLLLLGLLLDLLLLLLLDFRDDHELLLLLLDRRLLHUDLGFGRRRRFRLHLLDGLGLLHRRREUHUUULLG\
GLDRFGHLLLHLLLLRFGHLGLLLULGLLLGLLHRHLDDDLLLLDLLLFLLHUHLRRFRRRREHLLHLLLHLL",
"6RUSSIA",
"-143+54", "GDDDDDDDEDUUURUUHUU", "6SAKHALIN",
"-180+72", "GRRRRULLL", "6WRANGEL I.",
"-137+76", "DRRRRRRRELLLLLLLL", "6SIBERIAN I.",
"-091+80", "FERDRRRRRRULLLLLRRULLLLGL", "6SEVERNAYA",
"-101+79", "GRRRRELLLL", "6ZEMLYA",
"-068+77", "LLGLLLLLLGLLGGLGLRFRRRRLHERERERRRERRRREL", "6NOVAYA",
"+123+49",
"FGULLFDDDGFDDDFFDFRFRFDFFFDLFFRDFFEHHHHUHHUFRDFFFRDFFFDFGFRFRFRRFRRRRFFRRFRFF\
DRFFRFEUUGLHHUUEUHLLLLLEUUEULLLGDLLGLHHUHUUUEHEERERRFRRHRREFRRFDFDFEUUHUUUEERE\
RUUUHFDEUHFEURRRELUERRE",
"4NORTH AMERICA S",
"+113+42", "FH", "0SALT LAKE",
"+156+20", "DRULHLHL", "4HAWAII",
"+085+22", "RERFRRFRGRRRRHLHLHLLLLLG", "4CUBA",
"+070+18", "RRHHLLLFLLLFRRRRRR", "4HAITI",
"+078+18", "RRHLLF", "4JAMAICA",
"+066+18", "ELLDR", "4PUERTO RICO",
"+078+24", "UD", "4NASSAU",
"+067+45",
"REFLGDERERREHDLLLHUELLLGLGLREEERRRRRRREERRGGDGRRRFEFUUHLLLEUUHHGLRELLHHUHHHDG\
LGHHULLHLLLLLDFGFDDGLLFDDGHHUULLLLHLLHLLLUHUUEREEREERRRREUUHLLLDDGHULLLHLUHLGD\
RFGGULLLLLLLLLHLLGFLHLLLLLRHLLLLLHLLLLLLHGLLLLGUGLLLHLL",
"4CANADA",
"+088+49",
"LGLGRRRRRRRFLLLGRGDDREUURUFRGRFGFERERREEREERLGGLGLLLGRLLGLEUERHLLLHULHL",
"0GREAT LAKES",
"+117+61", "REHRFRRERGLGLLLL", "0SLAVE LAKE",
"+125+66", "RRERRRGREDLFHGLLLERLLLL", "0BEAR LAKE",
"+097+50", "UULHURFDFG", "0LAKE WINNIPEG",
"+090+72",
"FRRLLFRRRRRRRRRRFRRGLLGRREEFRFLGLFLLLLFRERFRFRRFRRHLHFRRRUHLHRRFRURELLHLLLHRR\
HLHLHGHLHLLGLLEHFRRRHLLLLLLGLDFHLUELLGG",
"4BAFFIN I.",
"+125+72",
"RFRREERRRLLGFFRRRRRLLLLLFRRRRRRRREFRRRRHRRLHLHHLRRULGLFLHLDLLULLLLHLLLLLLLDG",
"4VICTORIA I.",
"+141+70",
"LLLLLLLLHGLHLLLHGLLGLLGLLDRRFRRDLLLULGLLFRRRRRRDLGLLGFDRRRDRRRRRGGGLLGLLGGLLR\
RERERRRERREERRELEERRRLLGDRERRURRFRRRRRFRRFUDRUDDHFDURDURLURDDLFRULURDHFFRGFEGR\
FFRFRFLHLHLFFRFE",
"4ALASKA",
"+045+60",
"REUEREUERRRRERERRRERRRRERLLLLLLHRRRGERHFRRRRHLUDLLHLRERFRERLEUHRRHLEERLLURRRR\
RRRRELLLLLLLLLLGLLLRERHGLRELLLLLLLELLLLLLLLLLGLLLLLLGLLLLLLGLULLLLLLLFRLLLLLGL\
RRRGLLLLLLLGRRRRRRRGLLLLRRFRRRRRRRRRRFDFDLFREFRDLLLDERRFGLLGFFDRFFFRRRF",
"4GREENLAND",
"+080+10",
"DRFDFDDGGGDDGRDGDDFFDFDFFDFFRFFFDDDDDDGDDDDGDDDDGDGFGDDDEUDDDGUDDLDRGDDDFDFRF\
RRFERRLHLUHUURUEELHEREURULURREURREREUHUUDFRREEEEEUEUUEERERRREUEUEUUUUUEEEEUUUH\
LHLHLLLLHLHLGEHLGEUHUUHLHLLLHHLHULEDLLELLGHLLHLGDDHUELLGLGDGHHL",
"3SOUTH AMERICA",
"+060-51", "LDRRELL", "3FALKLAND ISLANDS",
"+092+00", "FUL", "3GALAPAGOS I.",
"-032+32",
"LLGLHLLLLHLGDGHLLHHLLHLEUULLLLLLLLLGLGLLLLHDGLGDGDGGLDGGGDGDFDDDDGDDFFFFDFRFF\
RRRRRRRRERERRFFRRFFDDDGDFFFDFDDDFDGDGDDDFDFDFDDDFDFDFDDFFERRRRREEEEEEEUUEREUUH\
UEEEREEUUUUHUUUHUEUEEEEEREEUEUEEUUULLLLGLLHUHHLHUHHUUHHUUHUHHUU",
"1AFRICA",
"-049-12", "DGGGLGDDDDGDDFFREUEUEUUUEUUUUH", "1MADAGASCAR",
"-032+00", "DDDREUELLL", "0LAKE VICTORIA",
"-014+14", "LRFLU", "0LAKE CHAD",
"-124-16",
"LGDGGLGLLGLDDDGFDDFDFDGFRRRERRRRURERRRRRRRFFFEEDDRFDFRFREFRERRUUEUEEUUUUUUUHH\
HHHHHUUHHHUULDDDDGDGHLHLHEUELLLHLFLLULDRGDDLLHLGG",
"2AUSTRALIA",
"-173-35", "FFDGFDREURULHHHL", "2NEW ZEALAND N",
"-174-41", "LLDGLGLGGRFREEUREEU", "2NEW ZEALAND S",
"-145-41", "DFRRUUUDLLL", "2TASMANIA",
"-178-17", "GRRURUGDH", "2FIJI",
"-130+00", "FRFRLGFEFRFRFDGRRFRRUERFFFRRRLHHHHRHLHHLHLLHGGLHUHLGH",
"2NEW GUINEA",
"-115-04", "RUUEEURHUUEHHGGGGLLDDHLDDFDDRRDERF", "2BORNEO",
"-095+06", "DFFFFFFDFFFFRUUUHFRHLHLUHHHHHLLH", "2SUMATRA",
"-106-06", "GRFRRRRRRFRRHLHLLLLLHL", "2JAVA",
"-120+00", "DGDDRDFHUEDFRHUHREFHLGHURRRRELLLLG", "2CELEBES",
"+000-70",
"ULDLLLLLLLLGLLGLLLGLLGLLLLGLGLLGLLLLGLLLLLHLGLLLLLHLLLLLHLLLLHLLUERLEUUUUUUEE\
RRRULLGLLLLGLGGLLLDRUDRDLGHLLGLLFGRRFLLLLLLLDHLLLLHLLLLLGLLLLHLLLLLLLGRFDLLLUL\
LLGHLLLLLLLLLLHGHLLGLLLLLLLGLLLLLLLLLLLGLLLGLLLLLLLLGLLLLLLLLLLLLLLLLLLLLL",
"7ANTARCTICA W",
"+180-78",
"LLLLLLLHLLGHLLGHLUEERRERREHLLLLHLLLLLLHLLLLLLLLLLLHLHLLLLLHLLULDLLLLLDLLHLLLL\
GHFLLLLLHLLLLLLGLHLLHLGLLLLHLGLLGLLLULLLGLLHDFLLLGLGLLLELLLLHLLLLLLLLLLHLLLHLL\
LLGGHGHGLLLGLDLLLLHLLGHGLLLLLLLLLLLLLLHLGLLLLLLLLLLLLLL",
"7ANTARCTICA E",
"E", "", ""};
#endif


/*
*******************************************************************************
** General and numeric routines
*******************************************************************************
*/

void swapint(i1, i2)
int *i1, *i2;
{
  int temp;
  temp = *i1; *i1 = *i2; *i2 = temp;
}

void swapdoub(f1, f2)
double *f1, *f2;
{
  double temp;
  temp = *f1; *f1 = *f2; *f2 = temp;
}

int stringlen(line)
char *line;
{
  int i;
  for (i = 0; line[i] != 0; i++)
    ;
  return i;
}

double dabs(A)
double A;
{
  return A < 0.0 ? -A : A;
}

double sgn(A)
double A;
{
  return A < 0.0 ? -1.0 : (A > 0.0 ? 1.0 : 0.0);
}

double dectodeg(A)
double A;
{
  return sgn(A)*(floor(dabs(A))+(dabs(A)-floor(dabs(A)))*100.0/60.0);
}

double mod(A)
double A;
{
  return ((A)-(floor((A)/modulus)*modulus));
}

int mod12(A)
int A;
{
  return A > SIGNS ? A-SIGNS : (A < 1 ? A+SIGNS : A);
}

double mindistance(deg1, deg2)
double deg1, deg2;
{
  double i;
  i = dabs(deg1-deg2);
  return i < 180 ? i : DEGREES - i;
}

double midpoint(deg1, deg2)
double deg1, deg2;
{
  double mid;
  mid = (deg1+deg2)/2.0;
  return mindistance(deg1, mid) < 90 ? mid : mod(mid+180);
}

char dignify(body, sign)
int body, sign;
{
  if (body > 10)
    return ' ';
  if (ruler1[body] == sign || ruler2[body] == sign)
    return 'R';
  if (ruler1[body] == mod12(sign+6) || ruler2[body] == mod12(sign+6))
    return 'F';
  if (exalt[body] == sign)
    return 'e';
  if (exalt[body] == mod12(sign+6))
    return 'd';
  return '-';
}

int dayinmonth(month, year)
int month, year;
{
  return (month == 9 || month == 4 || month == 6 || month == 11 ? 30 :
	  (month != 2 ? 31 :
	   (year/4*4==year && (year/100*100!=year || year/400*400==year) ?
	    29 : 28)));
}

double orb(body1, body2, aspect)
int body1, body2, aspect;
{
  static int max = 1;
  double orb;
  if (aspect == 0)
    return (max = 1-max);
  if (max && (body1 == 16 || body1 == 17 || body1 == 20 || body1 > BASE ||
	      body2 == 16 || body2 == 17 || body2 == 20 || body2 > BASE))
    orb = 2.0;
  else
    orb = aspectorb[aspect];
  if (body1 <= 2) orb += 1.0;
  if (body2 <= 2) orb += 1.0;
  return orb;
}


/*
*******************************************************************************
** IO routines
*******************************************************************************
*/

void toofew(option)
char *option;
{
  fprintf(stderr, "Too few options to switch -%s\n", option);
  exit(1);
}

char *stringlocation(Lon, Lat, norm)
double Lon, Lat, norm;
{
  static char string[14];
  int i, j;
  i = (int) ((dabs(Lon)-floor(dabs(Lon)))*norm+0.1);
  j = (int) ((dabs(Lat)-floor(dabs(Lat)))*norm+0.1);
  sprintf(string, "%3.0f'%d%d%c %2.0f'%d%d%c",
	  floor(dabs(Lon)), i/10, i%10, Lon < 0.0 ? 'E' : 'W',
	  floor(dabs(Lat)), j/10, j%10, Lat < 0.0 ? 'S' : 'N');
  return string;
}

double readstardata(reset)
int reset;
{
  static int datapointer = 0;
  if (!reset)
    return stardata[datapointer++];
  return (double) (datapointer = 0);
}

double readplanetdata(reset)
int reset;
{
  static int datapointer = 0;
  if (!reset)
    return planetdata[datapointer++];
  return (double) (datapointer = 0);
}

#ifdef X11
int readworlddata(nam, loc, lin)
char **nam, **loc, **lin;
{
  static int datapointer = 0;
  *loc = worlddata[datapointer++];
  *lin = worlddata[datapointer++];
  *nam = worlddata[datapointer++];
  if (*loc[0] != 'E')
    return 1;
  return (datapointer = 0);
}
#endif

/*void printsecond(deg)
double deg;
{
  int sign, d, m, s;
  deg = mod(deg + 1.0/60.0/60.0/2.0);
  sign = (int) (deg / 30.0);
  d = (int) deg - sign*30;
  m = (int) ((deg - floor(deg))*60.0);
  s = (int) (((deg - floor(deg))*60 - (double) m)*60.0);
  printf("%s%d%c%c%c%s%d'%s%d\"",
	 d < 10 ? "0" : "", d, SIGNAM(sign + 1),
	 m < 10 ? "0" : "", m, s < 10 ? "0" : "", s);
}

void printtenth(deg)
double deg;
{
  int sign, d, m;
  deg = mod(deg + 1.0/60.0/10.0/2.0);
  sign = (int) (deg / 30.0);
  d = (int) deg - sign*30;
  m = (int) ((deg - floor(deg))*600.0);
  printf("%s%d%c%c%c%s%.1f",
	 d < 10 ? "0" : "", d, SIGNAM(sign + 1),
	 m < 100 ? "0" : "", (double) m / 10.0);
}*/

void printminute(deg)
double deg;
{
  int sign, d, m;
  if (!right) {
    deg = mod(deg + 1.0/60.0/2.0);
    sign = (int) (deg / 30.0);
    d = (int) deg - sign*30;
    m = (int) ((deg - floor(deg))*60.0);
    printf("%s%d%c%c%c%s%d",
	   d < 10 ? "0" : "", d, SIGNAM(sign + 1), m < 10 ? "0" : "", m);
  } else {
    deg = mod(deg + 1.0/4.0/2.0);
    d = (int) (deg / 15.0);
    m = (int) ((deg - (double)d*15.0)*60.0/24.0);
    printf("%s%dh,%s%dm",
	   d < 10 ? "0" : "", d, m < 10 ? "0" : "", m);
  }
}

void printaltitude(deg)
double deg;
{
  int d, m;
  while (deg > 90.0)
    deg -= 180.0;
  while (deg < -90.0)
    deg += 180.0;
  putchar(deg < 0.0 ? '-' : '+');
  deg = dabs(deg) + 1.0/60.0/2.0;
  d = (int) deg;
  m = (int) ((deg - floor(deg))*60.0);
  printf("%s%d %s%d'", d < 10 ? "0" : "", d, m < 10 ? "0" : "", m);
}

double input(prompt, low, high)
char prompt[];
double low, high;
{
  char line[80];
  double x;
  while (1) {
    printf("%s > ", prompt);
    if (gets(line) == (char *) NULL) {
      printf("\nAstrolog terminated.\n");
      exit(1);
    }
    sscanf(line, "%lf", &x);
    if (x >= low && x <= high)
      return x;
    printf("Value out of range of from %.0f to %.0f.\n", low, high);
  }
}

void inputdata(filename)
char *filename;
{
  char name[STRING], c;
  int i;
  double k, l, m;
#ifdef TIME
  struct tm curtime;
  int curtimer;
  if (filename[0] == 'n' && filename[1] == 'o' && filename[2] == 'w' &&
      filename[3] == 0) {
    autom = 1;
    curtimer = (int) time((int *) 0);
    curtime = *localtime(&curtimer);
    M = (double) curtime.tm_mon + 1.0;
    D = (double) curtime.tm_mday;
    Y = (double) curtime.tm_year + 1900.0;
    F = (double) curtime.tm_hour + (double) curtime.tm_min / 100.0 +
      (double) curtime.tm_sec / 6000.0;
    X = defzone; L5 = deflong; LA = deflat;
    return;
  }
#endif
  if (filename[0] == 't' && filename[1] == 't' && filename[2] == 'y' &&
      filename[3] == 0) {
    printf("** Astrolog version %s (cruiser1@milton.u.washington.edu) **\n",
      VERSION);
#ifdef SWITCHES
    printf("   Invoke as 'astrolog -H' for brief list of command options.\n");
#endif
    M = input("Enter month of birth [MM]  ", 1.0, 12.0);
    D = input("Enter date  of birth [DD]  ", 1.0, 31.0);
    Y = input("Enter year  of birth [YYYY]", 0.0, 4000.0);
    printf("Assuming a 24 hour clock below (0.00=midnight, 18.00=6pm, etc)\n");
    printf("Subtract 1 from hour entered if Daylight Saving time then.\n");
    F = input("Time of birth [HR.MIN - Add 12 for PM times]", -2.0, 24.0);
    printf("Enter negative value below for time zones east of Greenwich.\n");
    X = input("Time zone in hours off GMT [HR.MIN - 5=Eastern, 8=Pacific]",
	      -24.0, 24.0);
    printf("Enter negative values below for eastern or southern locations.\n");
    L5 = input("Longitude west of place: DEG.MIN", -180.0, 180.0);
    LA = input("Latitude north of place: DEG.MIN", -90.0, 90.0);
    printf("\n");
    return;
  }
  autom = 1;
  data = fopen(filename, "r");
  if (data == NULL) {
    sprintf(name, "%s%s", DEFAULT_DIR, filename);
    data = fopen(name, "r");
    if (data == NULL) {
      fprintf(stderr, "File '%s' not found.\n", filename);
      exit(1);
    }
  }
  if ((c = getc(data)) != 'S') {
    ungetc(c, data);
    fscanf(data, "%lf%lf%lf%lf%lf%lf%lf", &M, &D, &Y, &F, &X, &L5, &LA);
  } else {
    M = -1.0;
    for (i = 1; i <= objects; i++) {
      fscanf(data, "%s%lf%lf%lf", name, &k, &l, &m);
      planet[i] = (l-1.0)*30.0+k+m/60.0;
      fscanf(data, "%s%lf%lf", name, &k, &l);
      planetalt[i] = k+l/60.0;
      ret[i] = DTOR(name[1] == 'D' ? 1.0 : -1.0);
    }
    for (i = 1; i <= SIGNS/2; i++) {
      fscanf(data, "%s%lf%lf%lf", name, &k, &l, &m);
      house[i+6] = mod((house[i] = mod((l-1.0)*30.0+k+m/60.0))+180.0);
    }
  }
  fclose(data);
}

/**/
