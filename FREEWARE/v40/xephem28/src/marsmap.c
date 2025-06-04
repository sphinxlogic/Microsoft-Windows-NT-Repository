#include "map.h"

static MCoord mc0[] = {
    {325,-11}, {9,-10}, {10,5}, {5,2}, {0,0}, {352,2}, {348,-5}, {330,6}
};
static MCoord mc1[] = {
    {160,-40}, {200,-40}, {200,-35}, {240,-15}, {235,-4}, {195,-32}, {170,-35}
};
static MCoord mc2[] = {
    {220,-25},{228,-35},{240,-35},{250,-26},{260,-30},{280,-25},{282,-11},
    {280,-3},{260,-7},{245,-20},{240,-15}
};
static MCoord mc3[] = {
    {281,5}, {285,1}, {298,2}, {290,22}, {283,22}
};
static MCoord mc4[] = {
    {120,-32}, {140,-35}, {150,-33}, {165,-25}, {165,-22}, {162,-23},
    {142,-31}, {138,-30}
};
static MCoord mc5[] = {
    {40,21}, {52,30}, {55,20}, {61,19}, {61,26}, {68,29}, {68,30}, {42,50},
    {22,48}, {10,40}, {15,35}, {20,25}
};
static MCoord mc6[] = {
    {72,-28}, {82,-29}, {90,-34}, {100,-22}, {90,-17}, {80,-20}, {72,-20}
};
static MCoord mc7[] = {
    {58,-28}, {68,-24}, {64,-19}, {56,-20}
};
static MCoord mc8[] = {
    {20,-31}, {30,-35}, {55,-26}, {53,-20}, {30,-20}
};
static MCoord mc9[] = {
    {30,-20}, {53,-20}, {65,-15}, {57,-6}, {38,-5}, {29,-16}
};
static MCoord mc10[] = {
    {5,-26}, {38,-8}, {21,0}
};

#define	ASIZ(a)		(sizeof(a)/sizeof(a[0]))

MRegion mreg[] = {
    {"Meridiani Sinus",		mc0, ASIZ(mc0)},
    {"Cimmerium Mare",		mc1, ASIZ(mc1)},
    {"Tyrrhenum Mare",		mc2, ASIZ(mc2)},
    {"Syrtis Major",		mc3, ASIZ(mc3)},
    {"Sirenum Mare",		mc4, ASIZ(mc4)},
    {"Niliacus Lacus",		mc5, ASIZ(mc5)},
    {"Solis Lacus",		mc6, ASIZ(mc6)},
    {"Nectar",			mc7, ASIZ(mc7)},
    {"Erythraeum Mare",		mc8, ASIZ(mc8)},
    {"Aurorae Sinus",		mc9, ASIZ(mc9)},
    {"Margaritifer Sinus",	mc10, ASIZ(mc10)},
};

int nmreg = ASIZ(mreg);
