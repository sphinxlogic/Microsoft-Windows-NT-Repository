// lin-city.h   Part of lincity.
// Copyright (c) I J Peters 1995,1997.  Please read the file 'COPYRIGHT'.

#define MOUSE_SENSITIVITY 1
//   ** This is only relevant for linux-svgalib. **
// The mouse sensitivity can be an integer such as 1, 2, 3 etc.
// The higher the number the less you have to move the mouse.
// It's a bit crude, it just multiples the mouse movement by this
// amount. IMHO 3 is about the maximum useful value, 2 is quite
// comfortable.

#define MONEY_SEPARATOR ','
// When your money reaches 1 million or more, to make it easier to
// read, it is split into two parts; a millions part, and the rest.
//  For example  12,355232    You can choose any character you like,
//  but I have given some other examples. Just comment out the one 
//  above, and uncomment one below. IT MUST NOT BE NULL.
// #define MONEY_SEPARATOR '.'
// #define MONEY_SEPARATOR ' '
// #define MONEY_SEPARATOR ':'

// #define MP_SANITY_CHECK

//      ************************************************
//       NO USER CONFIGURABLE OPTIONS BEYOND THIS POINT
//      ************************************************


//                       ********   NOTE    VERSION
#define VERSION 109
//                       ********   NOTE    VERSION

#define MIN_LOAD_VERSION 97

#define MM_MS_C_VER 97

#define THMO_TRANSPORT
#define THMO_POLLUTION
#define THMO_GP_STUFF
#define USE_X11_PIXMAPS
#define THMO_SCREEN_STUFF
// #define THOMMY_MAY_BE_WRONG
//         -- no, I think he's right.

// comment out the the next three lines for _slightly_ faster X drawing.
#ifdef LC_X11
#define ALLOW_PIX_DOUBLING
#endif

// #define CATCH_U_P_SIZE_FAULT

// Gamma correction. The numbers GAMMA_CORRECT_X are between 0.0 and 1.0
// The following values are enabled with the -w option. This seems quite
// good for 'washed out' monitors that lack red.
// You can also set the values from the command line.
//  [x]lincity -R 1.0 -G 0.0 -B 0.4  is the same as [x]lincity -w
#define GAMMA_CORRECT_RED   1.0
#define GAMMA_CORRECT_GREEN 0.0
#define GAMMA_CORRECT_BLUE  0.4

#define PROFILE_COUNTDOWN 10000

// hof is 70 from 28/12

#define DEBUG
// #define ALLOW_TCORE_DUMP
// #define DEBUG_ROCKETS
// #define DEBUGMOUSE
// #define DEBUG_MOUSE
// #define DEBUG_MAIN_SCREEN
// #define DEBUG_ENGINE
// #define DEBUG_GET_POWER
// if DEBUG_KEYS is defined, pressing certain keys add money, tech points etc.
// #define DEBUG_KEYS 1

// #define CS_PROFILE

#ifdef LC_X11
#define BORDERX 30
#define BORDERY 30
#endif

#define SELECT_BUTTONS_NEED_TECH
//#define USE_EXPANDED_FONT

#define TEXT_FG_COLOUR  (white(24))
#define TEXT_BG_COLOUR  105
#define YN_DIALBOX_BG_COLOUR (red(10))
#define LOAD_BG_COLOUR       (cyan(10))
#define SAVE_BG_COLOUR       (magenta(10))

#define FLAG_LEFT          (1)
#define FLAG_UP            (2)
#define FLAG_RIGHT         (4)
#define FLAG_DOWN          (8)
#define FLAG_POWERED       (0x10)
#define FLAG_FED           (0x20)
#define FLAG_EMPLOYED      (0x40)
#define FLAG_IS_TRANSPORT  (0x80)
#define FLAG_MB_FOOD       (0x100)
#define FLAG_MS_FOOD       (0x200)
#define FLAG_MB_JOBS       (0x400)
#define FLAG_MS_JOBS       (0x800)
#define FLAG_MB_COAL       (0x1000)
#define FLAG_MS_COAL       (0x2000)
#define FLAG_MB_ORE        (0x4000)
#define FLAG_MS_ORE        (0x8000)
#define FLAG_MB_GOODS      (0x10000)
#define FLAG_MS_GOODS      (0x20000)
#define FLAG_MB_STEEL      (0x40000)
#define FLAG_MS_STEEL      (0x80000)
#define FLAG_FIRE_COVER    (0x100000)
#define FLAG_HEALTH_COVER  (0x200000)
#define FLAG_CRICKET_COVER (0x400000)
#define FLAG_IS_RIVER      (0x800000)
#define FLAG_HAD_POWER     (0x1000000)

#define T_FOOD  0
#define T_JOBS  1
#define T_COAL  2
#define T_GOODS 3
#define T_ORE   4
#define T_STEEL 5
#define T_WASTE 6

#define MT_FAIL     0
#define MT_START    1
#define MT_CONTINUE 2
#define MT_SUCCESS  3

#define HOF_START 845830134
//#define HOF_STOP  851880038
#define HOF_STOP 857843038

#define MINI_SCREEN_POL_FLAG     (1)
#define MINI_SCREEN_UB40_FLAG    (2)
#define MINI_SCREEN_STARVE_FLAG  (4)
#define MINI_SCREEN_PORT_FLAG    (8)
#define MINI_SCREEN_POWER_FLAG   (16)

#define SEED_RAND
#ifndef VMS
#define OLD_LC_SAVE_DIR "Lin-city"
#define LC_SAVE_DIR ".Lin-city"
#define RESULTS_FILENAME "results"
#else
#define OLD_LC_SAVE_DIR "LINCITY_SAVE_OLD"
#define LC_SAVE_DIR "LINCITY_SAVE_DIR"
#define RESULTS_FILENAME "results."
#endif

#define NUM_OF_TYPES 400
#define MAX_ICON_LEN 4096
#define NUMOF_SELECT_BUTTONS 32
#define NUMOF_GROUPS         34
#define WORLD_SIDE_LEN 100
#define NUMOF_DAYS_IN_MONTH 100
#define NUMOF_DAYS_IN_YEAR (NUMOF_DAYS_IN_MONTH*12)
#define FAST_TIME_FOR_YEAR 1
#define MED_TIME_FOR_YEAR  20
#define SLOW_TIME_FOR_YEAR  60

// interest rate *10  ie 10 is 1%
#define INTEREST_RATE 15

#define PAUSE_BUTTON_X 544
#define PAUSE_BUTTON_Y 416
#define PAUSE_BUTTON_H 16
#define PAUSE_BUTTON_W 32
#define SLOW_BUTTON_X 544
#define SLOW_BUTTON_Y 432
#define SLOW_BUTTON_H 16
#define SLOW_BUTTON_W 32
#define MED_BUTTON_X 544
#define MED_BUTTON_Y 448
#define MED_BUTTON_H 16
#define MED_BUTTON_W 32
#define FAST_BUTTON_X 544
#define FAST_BUTTON_Y 464
#define FAST_BUTTON_H 16
#define FAST_BUTTON_W 32

#define HELP_BUTTON_X 608
#define HELP_BUTTON_Y 448
#define HELP_BUTTON_W 32
#define HELP_BUTTON_H 32
#define QUIT_BUTTON_X 608
#define QUIT_BUTTON_Y 416
#define QUIT_BUTTON_W 32
#define QUIT_BUTTON_H 32
#define LOAD_BUTTON_X 576
#define LOAD_BUTTON_Y 416
#define LOAD_BUTTON_W 32
#define LOAD_BUTTON_H 32
#define SAVE_BUTTON_X 576
#define SAVE_BUTTON_Y 448
#define SAVE_BUTTON_W 32
#define SAVE_BUTTON_H 32

#define TESTM_X 50
#define TESTM_Y 428

// #define HELPPATH "/usr/local/games/lin-city/help/"
#define HELPERRORPAGE "error.hlp"
#define HELPBACKGROUNDCOLOUR (white(8))
#define HELPBUTTON_COLOUR (white(25))
#define MAX_NUMOF_HELP_BUTTONS 40
#define MAX_LENOF_HELP_FILENAME 40
#define MAX_HELP_HISTORY 20
#define CS_MOUSE_BUTTON_DELAY 5
#define RIGHT_MOUSE_MOVE_VAL 5

// #define MESSAGE_PATH "/usr/local/games/lin-city/messages/"
#define GOOD 1
#define BAD (-1)
#define RESULTS 0

#define PROGBOXX 170
#define PROGBOXY 180
#define PROGBOXW 300
#define PROGBOXH 120
#define PROGBOX_BG_COLOUR 114
#define PROGBOX_DONE_COL 4
#define PROGBOX_NOTDONE_COL 2

#define POWER_LINE_CAPACITY 1000000
#define POWERS_SOLAR_OUTPUT 1800
#define WINDMILL_POWER      450
#define WINDMILL_JOBS       10
// WINDMILL_RCOST is days per quid
#define WINDMILL_RCOST      3
#define WINDMILL_ANIM_SPEED 80

#define OLD_MAX_NUMOF_SUBSTATIONS 100
#define MAX_NUMOF_SUBSTATIONS 512
#define SUBSTATION_RANGE 10
#define POWER_USE_PER_PERSON 3
#define POWER_RES_OVERHEAD 30
#define POWERS_COAL_OUPUT 22000
#define MAX_COAL_AT_POWER_STATION 100000
#define MAX_ORE_AT_INDUSTRY_L 20000
#define MAX_JOBS_AT_INDUSTRY_L 500
#define MAX_GOODS_AT_INDUSTRY_L 65000
#define INDUSTRY_L_GET_JOBS    120
#define INDUSTRY_L_GET_ORE     600
#define INDUSTRY_L_GET_STEEL   60
#define INDUSTRY_L_JOBS_LOAD_ORE 1
#define INDUSTRY_L_JOBS_LOAD_STEEL 4
#define INDUSTRY_L_ORE_USED  125
#define INDUSTRY_L_STEEL_USED  12
#define INDUSTRY_L_JOBS_USED       30
#define INDUSTRY_L_MAKE_GOODS 225
#define MAX_STEEL_AT_INDUSTRY_L 2000
#define MIN_JOBS_AT_INDUSTRY_L (INDUSTRY_L_JOBS_LOAD_ORE\
+INDUSTRY_L_JOBS_LOAD_STEEL+INDUSTRY_L_JOBS_USED)
#define INDUSTRY_L_ANIM_SPEED 290

#define MAX_ORE_AT_INDUSTRY_H 10000
#define MAX_COAL_AT_INDUSTRY_H 1000
#define MAX_MADE_AT_INDUSTRY_H MAX_ORE_AT_INDUSTRY_H/16
#define POWER_MAKE_STEEL 200
#define INDUSTRY_H_ANIM_SPEED  290

#define DAYS_PER_POLLUTION      14
#define POWERS_COAL_POLLUTION   20
#define INDUSTRY_L_POLLUTION    10
#define INDUSTRY_H_POLLUTION    10
#define COALMINE_POLLUTION      3
#define PORT_POLLUTION          1
#define RAIL_POLLUTION          1
#define DAYS_PER_RAIL_POLLUTION 30
#define ROAD_POLLUTION          1
#define DAYS_PER_ROAD_POLLUTION 20
#define UNNAT_DEATHS_COST 500

#define FINANCE_X 91
#define FINANCE_W 120
#define FINANCE_Y 200
#define FINANCE_H 56
#define INCOME_TAX_RATE 8
#define COAL_TAX_RATE 20
#define GOODS_TAX_RATE 1
#define DOLE_RATE 15
#define TRANSPORT_COST_RATE 14
#define IM_PORT_COST_RATE 1
#define PORT_FOOD_RATE    1
#define PORT_JOBS_RATE    5
#define PORT_COAL_RATE    50
#define PORT_ORE_RATE     1
#define PORT_GOODS_RATE   3
#define PORT_STEEL_RATE   100
// These next two control the stuff bought or sold as a % of what's on the
// transport.  1000=100%  500=50% etc.
#define PORT_EXPORT_RATE  500
#define PORT_IMPORT_RATE  500

#define NUMOF_DISCOUNT_TRIGGERS 6
#define EX_DISCOUNT_TRIGGER_1   25000
#define EX_DISCOUNT_TRIGGER_2   50000
#define EX_DISCOUNT_TRIGGER_3  100000
#define EX_DISCOUNT_TRIGGER_4  200000
#define EX_DISCOUNT_TRIGGER_5  400000
#define EX_DISCOUNT_TRIGGER_6  800000

#define MIN_RES_POPULATION 10
#define MAX_TECH_LEVEL	   1000000
#define TECH_LEVEL_LOSS_START 11000
// tech-level/TECH_LEVEL_LOSS is lost every month when
// above TECH_LEVEL_LOSS_START
#define TECH_LEVEL_LOSS    500
#define TECH_LEVEL_UNAIDED 2
#define MODERN_WINDMILL_TECH 450000

#define BUILD_MONUMENT_JOBS     350000
#define MONUMENT_GET_JOBS       100
#define MONUMENT_DAYS_PER_TECH  3
#define MONUMENT_TECH_EXPIRE    400

#define BLACKSMITH_JOBS    35
#define BLACKSMITH_GET_COAL 6
#define MAX_GOODS_AT_BLACKSMITH 200
#define MAX_COAL_AT_BLACKSMITH  100
#define BLACKSMITH_STEEL_USED 1
#define BLACKSMITH_COAL_USED  1
#define GOODS_MADE_BY_BLACKSMITH 50
#define BLACKSMITH_BATCH (GOODS_MADE_BY_BLACKSMITH*100)
#define BLACKSMITH_ANIM_SPEED    200

#define MILL_JOBS          35
#define MAX_GOODS_AT_MILL  200
#define MAX_FOOD_AT_MILL   500
#define MAX_COAL_AT_MILL   50
#define GOODS_MADE_BY_MILL 75
#define MILL_GET_FOOD      51
#define MILL_GET_COAL      4
#define MILL_POWER_PER_COAL 60
#define FOOD_USED_BY_MILL  (GOODS_MADE_BY_MILL/2)
#define COAL_USED_BY_MILL  1
#define MILL_ANIM_SPEED    300

#define SCHOOL_JOBS        50
#define SCHOOL_GOODS	   75
#define JOBS_MAKE_TECH_SCHOOL  200
#define GOODS_MAKE_TECH_SCHOOL  75
#define TECH_MADE_BY_SCHOOL    2
#define MAX_JOBS_AT_SCHOOL     400
#define MAX_GOODS_AT_SCHOOL    200
#define SCHOOL_RUNNING_COST    2

#define DAYS_BETWEEN_COVER (NUMOF_DAYS_IN_MONTH*3)
#define DAYS_BETWEEN_FIRES (NUMOF_DAYS_IN_YEAR*2)
#define FIRESTATION_JOBS   6
#define FIRESTATION_GET_JOBS 7
#define MAX_JOBS_AT_FIRESTATION (FIRESTATION_JOBS*NUMOF_DAYS_IN_YEAR)
#define FIRESTATION_GOODS  2
#define FIRESTATION_GET_GOODS 3
#define MAX_GOODS_AT_FIRESTATION (FIRESTATION_GOODS*NUMOF_DAYS_IN_YEAR)
#define FIRESTATION_RANGE  18
#define FIRESTATION_RUNNING_COST 1
#define FIRESTATION_RUNNING_COST_MUL 6
#define FIRESTATION_ANIMATION_SPEED 250
#define FIRE_ANIMATION_SPEED 350
#define FIRE_DAYS_PER_SPREAD (NUMOF_DAYS_IN_YEAR/8)
#define FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*5)
#define AFTER_FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*10)

#define CRICKET_JOBS   8
#define CRICKET_GET_JOBS 9
#define MAX_JOBS_AT_CRICKET (CRICKET_JOBS*NUMOF_DAYS_IN_YEAR)
#define CRICKET_GOODS  2
#define CRICKET_GET_GOODS 3
#define MAX_GOODS_AT_CRICKET (CRICKET_GOODS*NUMOF_DAYS_IN_YEAR)
#define CRICKET_RANGE  10
#define CRICKET_RUNNING_COST 1
#define CRICKET_ANIMATION_SPEED 750

#define MAX_GOODS_AT_POTTERY     200
#define POTTERY_ORE_MAKE_GOODS    11
#define POTTERY_COAL_MAKE_GOODS    2
#define POTTERY_JOBS              35
#define MAX_ORE_AT_POTTERY        (POTTERY_ORE_MAKE_GOODS*60)
#define POTTERY_GET_ORE           (POTTERY_ORE_MAKE_GOODS*20)
#define MAX_COAL_AT_POTTERY       (POTTERY_COAL_MAKE_GOODS*60)
#define POTTERY_GET_COAL          (POTTERY_COAL_MAKE_GOODS*21)
#define MAX_JOBS_AT_POTTERY       (POTTERY_JOBS*20)
#define POTTERY_GET_JOBS          (POTTERY_JOBS*2)
#define POTTERY_MADE_GOODS        35
#define POTTERY_ANIM_SPEED        280
#define POTTERY_CLOSE_TIME        20

#define UNIVERSITY_JOBS   250
#define UNIVERSITY_JOBS_STORE 5000
#define UNIVERSITY_GOODS  750
#define UNIVERSITY_GOODS_STORE 7000
#define UNIVERSITY_RUNNING_COST 23
#define UNIVERSITY_TECH_MADE    4

#define ROCKET_PAD_JOBS         1000
#define ROCKET_PAD_JOBS_STORE   10000
#define ROCKET_PAD_GOODS        10000
#define ROCKET_PAD_GOODS_STORE  500000
#define ROCKET_PAD_STEEL	500
#define ROCKET_PAD_STEEL_STORE  12000
#define ROCKET_PAD_RUNNING_COST 200
#define ROCKET_PAD_LAUNCH       100
#define ROCKET_ANIMATION_SPEED  450

//#define RECYCLE_GOODS_GET    12500
//#define GET_USED_GOODS_JOBS  (RECYCLE_GOODS_GET/50)
#define MAX_WASTE_AT_RECYCLE 20000
#define BURN_WASTE_AT_RECYCLE (MAX_WASTE_AT_RECYCLE/200)
#define MAX_ORE_AT_RECYCLE   (MAX_ORE_ON_RAIL*2)
#define GOODS_RECYCLED       500
#define RECYCLE_GOODS_JOBS   (GOODS_RECYCLED/50)
//#define MAX_WASTE_GOODS      1000000
#define RECYCLE_RUNNING_COST 3

#define HEALTH_CENTRE_JOBS   6
#define HEALTH_CENTRE_GET_JOBS 7
#define MAX_JOBS_AT_HEALTH_CENTRE (HEALTH_CENTRE_JOBS*NUMOF_DAYS_IN_YEAR)
#define HEALTH_CENTRE_GOODS  40
#define HEALTH_CENTRE_GET_GOODS 41
#define MAX_GOODS_AT_HEALTH_CENTRE (HEALTH_CENTRE_GOODS*NUMOF_DAYS_IN_YEAR)
#define HEALTH_RUNNING_COST  2
#define HEALTH_RUNNING_COST_MUL 9
#define HEALTH_CENTRE_RANGE  15

#define OLD_MAX_NUMOF_MARKETS 100
#define MAX_NUMOF_MARKETS 512
#define MARKET_RANGE      10
#define MAX_FOOD_ON_TRACK 2048
#define MAX_FOOD_ON_RIVER (MAX_FOOD_ON_TRACK*2)
#define MAX_FOOD_ON_ROAD (MAX_FOOD_ON_TRACK*4)
#define MAX_FOOD_ON_RAIL (MAX_FOOD_ON_ROAD*4)
#define MAX_FOOD_IN_MARKET (MAX_FOOD_ON_RAIL*8)
#define MARKET_FOOD_SEARCH_TRIGGER (MAX_FOOD_IN_MARKET/5)
#define MAX_JOBS_ON_TRACK 1024
#define MAX_JOBS_ON_RIVER (MAX_JOBS_ON_TRACK*2)
#define MAX_JOBS_ON_ROAD (MAX_JOBS_ON_TRACK*5)
#define MAX_JOBS_ON_RAIL (MAX_JOBS_ON_ROAD*5)
#define MAX_JOBS_IN_MARKET (MAX_JOBS_ON_RAIL*3)
#define MARKET_JOBS_SEARCH_TRIGGER (MAX_JOBS_IN_MARKET/5)
#define EMPLOYER_RANGE 10
#define WORKING_POP_PERCENT 45
#define JOB_SWING 15
#define HC_JOB_SWING 10
#define CRICKET_JOB_SWING 4
#define ORGANIC_FARM_FOOD_OUTPUT 550
#define FARM_JOBS_USED 13
#define ORG_FARM_RANGE 10
#define ORG_FARM_POWER_REC 50
// gets waste only when powered
#define ORG_FARM_WASTE_GET 6
#define MIN_FOOD_SOLD_FOR_ANIM 200
#define DAYS_PER_STARVE 20

#define MAX_WASTE_ON_TRACK 1024
#define MAX_WASTE_ON_ROAD (MAX_WASTE_ON_TRACK*5)
#define MAX_WASTE_ON_RAIL (MAX_WASTE_ON_ROAD*5)
#define MAX_WASTE_IN_MARKET (MAX_WASTE_ON_RAIL*3)
#define MARKET_WASTE_SEARCH_TRIGGER (MAX_WASTE_IN_MARKET/5)
#define MAX_WASTE_AT_TIP  10000000
#define WASTE_BURN_ON_TRANSPORT 20
#define TRANSPORT_BURN_WASTE_COUNT 75000

#define NUMOF_COAL_RESERVES 100
#define COAL_RESERVE_SIZE 10000
#define ORE_RESERVE       1000
#define MAX_COAL_AT_MINE 100000
#define MAX_ORE_AT_MINE 100000
// COAL_RESERVE_SEARCH_LEN acts in both directions, so 5 is 10*10
#define COAL_RESERVE_SEARCH_LEN 8
#define JOBS_DIG_COAL 900
#define JOBS_DIG_ORE  200
#define JOBS_COALPS_GENERATE 100
#define MAX_JOBS_AT_COALPS 2000
#define JOBS_LOAD_COAL 18
#define JOBS_LOAD_ORE  9
#define JOBS_LOAD_STEEL 15
#define JOBS_AT_COMMUNE_GATE 4
#define COMMUNE_ANIM_SPEED 750
#define COMMUNE_POP  5

#define DIG_MORE_COAL_TRIGGER (MAX_COAL_AT_MINE)
#define DIG_MORE_ORE_TRIGGER  (MAX_ORE_AT_MINE)
#define MAX_COAL_ON_TRACK 64
#define MAX_COAL_ON_RIVER (MAX_COAL_ON_TRACK*2)
#define MAX_COAL_ON_ROAD (MAX_COAL_ON_TRACK*8)
#define MAX_COAL_ON_RAIL (MAX_COAL_ON_ROAD*8)
#define MAX_COAL_IN_MARKET (MAX_COAL_ON_RAIL*2)
#define MARKET_COAL_SEARCH_TRIGGER (MAX_COAL_IN_MARKET/5)

#define MAX_GOODS_ON_TRACK 2048
#define MAX_GOODS_ON_RIVER (MAX_GOODS_ON_TRACK*2)
#define MAX_GOODS_ON_ROAD (MAX_GOODS_ON_TRACK*5)
#define ROAD_GOODS_USED_MASK 0x1f
#define MAX_GOODS_ON_RAIL (MAX_GOODS_ON_ROAD*5)
#define RAIL_GOODS_USED_MASK 0xf
#define MAX_GOODS_IN_MARKET (MAX_GOODS_ON_RAIL*4)
#define MARKET_GOODS_SEARCH_TRIGGER (MAX_GOODS_IN_MARKET/5)
#define MAX_ORE_ON_TRACK 4096
#define MAX_ORE_ON_RIVER (MAX_ORE_ON_TRACK*2)
#define MAX_ORE_ON_ROAD (MAX_ORE_ON_TRACK*4)
#define MAX_ORE_ON_RAIL (MAX_ORE_ON_ROAD*4)
#define MAX_ORE_IN_MARKET (MAX_ORE_ON_RAIL*2)
#define MARKET_ORE_SEARCH_TRIGGER (MAX_ORE_IN_MARKET/5)


#define MAX_STEEL_ON_TRACK 128
#define MAX_STEEL_ON_RIVER (MAX_STEEL_ON_TRACK*2)
#define MAX_STEEL_ON_ROAD (MAX_STEEL_ON_TRACK*4)
#define MAX_STEEL_ON_RAIL (MAX_STEEL_ON_ROAD*4)
#define RAIL_STEEL_USED_MASK 0x7f
#define MAX_STEEL_AT_INDUSTRY_H (MAX_STEEL_ON_RAIL*10)
// JOBS_MAKE_STEEL is the steel made per job at the steel works
// what's it doing here?
#define JOBS_MAKE_STEEL 70
// ORE_MAKE_STEEL is the ore used per unit of STEEL at a steel works
// what's it doing here?
#define ORE_MAKE_STEEL 17
#define MAX_STEEL_IN_MARKET (MAX_STEEL_ON_RAIL*2)
#define MARKET_STEEL_SEARCH_TRIGGER (MAX_STEEL_IN_MARKET/5)



#define MAIN_WIN_X (640-400-8)
#define MAIN_WIN_W 400
#define MAIN_WIN_Y 8
#define MAIN_WIN_H 400

#define MINI_SCREEN_X 111
#define MINI_SCREEN_Y 306

#define SUST_SCREEN_X 140
#define SUST_SCREEN_Y 440
#define SUST_SCREEN_W (60+8)
#define SUST_SCREEN_H 20
#define SUST_ORE_COAL_COL 34
#define SUST_PORT_COL     white(24)
#define SUST_MONEY_COL    green(26)
#define SUST_POP_COL      cyan(24)
#define SUST_TECH_COL     yellow(26)
#define SUST_FIRE_COL     red(26)
#define SUST_ORE_COAL_YEARS_NEEDED 180
#define SUST_PORT_YEARS_NEEDED     180
#define SUST_MONEY_YEARS_NEEDED    60
#define SUST_POP_YEARS_NEEDED      60
#define SUST_TECH_YEARS_NEEDED     120
#define SUST_FIRE_YEARS_NEEDED     60
//#define SUST_YEARS_NEEDED 180

#define SUST_MIN_POPULATION 5000
#define SUST_MIN_TECH_LEVEL (MAX_TECH_LEVEL/2)

#define MS_NORMAL_BUTTON_X MINI_SCREEN_X
#define MS_NORMAL_BUTTON_Y (MINI_SCREEN_Y-24)
#define MS_POLLUTION_BUTTON_X MINI_SCREEN_X+16
#define MS_POLLUTION_BUTTON_Y (MINI_SCREEN_Y-24)
#define MS_FIRE_COVER_BUTTON_X MINI_SCREEN_X+16
#define MS_FIRE_COVER_BUTTON_Y (MINI_SCREEN_Y-40)
#define MS_UB40_BUTTON_X (MINI_SCREEN_X+32)
#define MS_UB40_BUTTON_Y (MINI_SCREEN_Y-24)
#define MS_HEALTH_COVER_BUTTON_X (MINI_SCREEN_X+32)
#define MS_HEALTH_COVER_BUTTON_Y (MINI_SCREEN_Y-40)
#define MS_COAL_BUTTON_X (MINI_SCREEN_X+48)
#define MS_COAL_BUTTON_Y (MINI_SCREEN_Y-24)
#define MS_CRICKET_COVER_BUTTON_X (MINI_SCREEN_X+48)
#define MS_CRICKET_COVER_BUTTON_Y (MINI_SCREEN_Y-40)
#define MS_POWER_BUTTON_X (MINI_SCREEN_X+64)
#define MS_POWER_BUTTON_Y (MINI_SCREEN_Y-40)
#define MS_STARVE_BUTTON_X (MINI_SCREEN_X+64)
#define MS_STARVE_BUTTON_Y (MINI_SCREEN_Y-24)
#define MS_OCOST_BUTTON_X (MINI_SCREEN_X+80)
#define MS_OCOST_BUTTON_Y (MINI_SCREEN_Y-24)


#define STATS_X 232
#define STATS_Y 428
#define STATS_W 304
#define STATS_H 32

#define RESULTSBUT_X 1
#define RESULTSBUT_Y 394
#define RESULTSBUT_W 32
#define RESULTSBUT_H 16

#define TOVERBUT_X 1
#define TOVERBUT_Y (394+18)
#define TOVERBUT_W 32
#define TOVERBUT_H 16

#define CONFINEBUT_X 38
#define CONFINEBUT_Y 394

#define MONTHGRAPH_X 91
#define MONTHGRAPH_W 120
#define MONTHGRAPH_Y 116
#define MONTHGRAPH_H 64
#define DIFFGRAPH_POWER_COLOUR (yellow(28))
#define DIFFGRAPH_COAL_COLOUR  (white(18))
#define DIFFGRAPH_GOODS_COLOUR (cyan(24))
#define DIFFGRAPH_ORE_COLOUR   (red(22))
#define DIFFGRAPH_POPULATION_COLOUR 7
#define GRAPHS_B_COLOUR (white(6))

#define MARKET_CB_X (79)
#define MARKET_CB_W (18*8)
#define MARKET_CB_Y 0
#define MARKET_CB_H (23*8)

#define SELECT_BUTTON_WIN_X 1
#define SELECT_BUTTON_WIN_W 56
#define SELECT_BUTTON_WIN_Y 1
#define SELECT_BUTTON_WIN_H 392
#define NUMOF_SELECT_BUTTONS_DOWN 16
#define SELECT_BUTTON_MESSAGE_X 0
#define SELECT_BUTTON_MESSAGE_Y (480-9)

#define PBAR_SIZE_X 16
#define PBAR_SIZE_Y 56
#define PBAR_POP_X 60
#define PBAR_POP_Y 1
#define PBAR_TECH_X 60
#define PBAR_TECH_Y PBAR_POP_Y+PBAR_SIZE_Y+4
#define PBAR_FOOD_X 60
#define PBAR_FOOD_Y PBAR_POP_Y+(PBAR_SIZE_Y+4)*2
#define PBAR_JOBS_X 60
#define PBAR_JOBS_Y PBAR_POP_Y+(PBAR_SIZE_Y+4)*3
#define PBAR_MONEY_X 60
#define PBAR_MONEY_Y PBAR_POP_Y+(PBAR_SIZE_Y+4)*4
#define PBAR_COAL_X 60
#define PBAR_COAL_Y PBAR_POP_Y+(PBAR_SIZE_Y+4)*5
#define PBAR_GOODS_X 60
#define PBAR_GOODS_Y PBAR_POP_Y+(PBAR_SIZE_Y+4)*6
#define PBAR_ORE_X   81
#define PBAR_ORE_Y   PBAR_POP_Y+(PBAR_SIZE_Y+4)*5
#define PBAR_STEEL_X 81
#define PBAR_STEEL_Y PBAR_POP_Y+(PBAR_SIZE_Y+4)*6

#define SCROLL_LONG_COUNT 5
#define SCROLL_RIGHT_BUTTON_X 100
#define SCROLL_RIGHT_BUTTON_W 32
#define SCROLL_RIGHT_BUTTON_Y 100
#define SCROLL_RIGHT_BUTTON_H 32
#define SCROLL_LEFT_BUTTON_X 100
#define SCROLL_LEFT_BUTTON_W 32
#define SCROLL_LEFT_BUTTON_Y 132
#define SCROLL_LEFT_BUTTON_H 32
#define SCROLL_DOWN_BUTTON_X 100
#define SCROLL_DOWN_BUTTON_W 32
#define SCROLL_DOWN_BUTTON_Y 164
#define SCROLL_DOWN_BUTTON_H 32
#define SCROLL_UP_BUTTON_X 100
#define SCROLL_UP_BUTTON_W 32
#define SCROLL_UP_BUTTON_Y 196
#define SCROLL_UP_BUTTON_H 32

#define SHUFFLE_MAPPOINT_COUNT 4
#define MAPPOINT_STATS_X 91
// MAPPOINT_STATS_W must be == MONTHGRAPH_W because of the graph drawing.
#define MAPPOINT_STATS_W MONTHGRAPH_W
#define MAPPOINT_STATS_Y 8
#define MAPPOINT_STATS_H (11*8)

#define GROUP_BARE 	   0
#define GROUP_BARE_COLOUR  (green(12))
#define GROUP_BARE_COST    0
#define GROUP_BARE_COST_MUL 1
#define GROUP_BARE_BUL_COST 1
#define GROUP_BARE_TECH    0
#define GROUP_BARE_FIREC   0

#define GROUP_POWER_LINE   1
#define GROUP_POWER_LINE_COLOUR (yellow(26))
#define GROUP_POWER_LINE_COST 100
#define GROUP_POWER_LINE_COST_MUL 2
#define GROUP_POWER_LINE_BUL_COST 100
#define GROUP_POWER_LINE_TECH 200
#define GROUP_POWER_LINE_FIREC 0

// GROUP_POWER_SOURCE is a solar ps - change the names  - real soon
#define GROUP_POWER_SOURCE 2
#define GROUP_POWER_SOURCE_COLOUR (yellow(22))
#define GROUP_POWER_SOURCE_COST 500000
#define GROUP_POWER_SOURCE_COST_MUL 5
#define GROUP_POWER_SOURCE_BUL_COST 100000
#define GROUP_POWER_SOURCE_TECH 500
#define GROUP_POWER_SOURCE_NO_CREDIT
#define GROUP_POWER_SOURCE_FIREC 33

#define GROUP_SUBSTATION   3
#define GROUP_SUBSTATION_COLOUR (yellow(18))
#define GROUP_SUBSTATION_COST 500
#define GROUP_SUBSTATION_COST_MUL 2
#define GROUP_SUBSTATION_BUL_COST 100
#define GROUP_SUBSTATION_TECH 200
#define GROUP_SUBSTATION_FIREC 50

#define RESIDENCE_BASE_BR     100
#define RESIDENCE_BRM_HEALTH  5
#define RESIDENCE_BASE_DR     (68*12)
#define GROUP_RESIDENCE    4
#define GROUP_RESIDENCE_COLOUR (cyan(24))
#define GROUP_RESIDENCE_COST   0
#define GROUP_RESIDENCE_COST1  1000
#define RESIDENCE1_BRM (RESIDENCE_BASE_BR*12)
#define RESIDENCE1_DRM ((RESIDENCE_BASE_DR*3)/4)
#define GROUP_RESIDENCE_COST2  2000
#define RESIDENCE2_BRM 0
#define RESIDENCE2_DRM 0
#define GROUP_RESIDENCE_COST3  4000
#define RESIDENCE3_BRM (RESIDENCE_BASE_BR+RESIDENCE_BASE_BR/4)
#define RESIDENCE3_DRM ((RESIDENCE_BASE_DR)/2)
#define GROUP_RESIDENCE_COST4  800
#define RESIDENCE4_BRM (RESIDENCE_BASE_BR*18)
#define RESIDENCE4_DRM ((RESIDENCE_BASE_DR*3)/4)
#define GROUP_RESIDENCE_COST5  1600
#define RESIDENCE5_BRM (RESIDENCE_BASE_BR/2)
#define RESIDENCE5_DRM ((RESIDENCE_BASE_DR)/4)
#define GROUP_RESIDENCE_COST6  3200
#define RESIDENCE6_BRM 0
#define RESIDENCE6_DRM ((RESIDENCE_BASE_DR)/4)
#define GROUP_RESIDENCE_COST_MUL 25
#define GROUP_RESIDENCE_BUL_COST 1000
#define GROUP_RESIDENCE_TECH 0
#define GROUP_RESIDENCE_FIREC 75
// RESIDENCE_PPM is the people_pool mobitily. Higher number=less mobile.
#define RESIDENCE_PPM   20

#define GROUP_ORGANIC_FARM 5
#define GROUP_ORGANIC_FARM_COLOUR (green(30))
#define GROUP_ORGANIC_FARM_COST 1000
#define GROUP_ORGANIC_FARM_COST_MUL 20
#define GROUP_ORGANIC_FARM_BUL_COST 100
#define GROUP_ORGANIC_FARM_TECH 0
#define GROUP_ORGANIC_FARM_FIREC 20

#define GROUP_MARKET       6
#define GROUP_MARKET_COLOUR (blue(28))
#define GROUP_MARKET_COST 100
#define GROUP_MARKET_COST_MUL 25
#define GROUP_MARKET_BUL_COST 100
#define GROUP_MARKET_TECH 0
#define GROUP_MARKET_FIREC 80

#define GROUP_TRACK	   7
// 32 is a brown
#define GROUP_TRACK_COLOUR 32
#define GROUP_TRACK_COST   1
#define GROUP_TRACK_COST_MUL 25
#define GROUP_TRACK_BUL_COST   10
#define GROUP_TRACK_TECH   0
#define GROUP_TRACK_FIREC  4

#define GROUP_COALMINE     8
#define GROUP_COALMINE_COLOUR 0
#define GROUP_COALMINE_COST   10000
#define GROUP_COALMINE_COST_MUL 25
#define GROUP_COALMINE_BUL_COST   10000
#define GROUP_COALMINE_TECH   85
#define GROUP_COALMINE_FIREC  85

#define GROUP_RAIL         9
#define GROUP_RAIL_COLOUR  (magenta(18))
#define GROUP_RAIL_COST    500
#define GROUP_RAIL_COST_MUL 10
#define GROUP_RAIL_BUL_COST    1000
#define GROUP_RAIL_TECH    180
#define GROUP_RAIL_FIREC   6

#define GROUP_POWER_SOURCE_COAL 10
#define GROUP_POWER_SOURCE_COAL_COLOUR 0
#define GROUP_POWER_SOURCE_COAL_COST   100000
#define GROUP_POWER_SOURCE_COAL_COST_MUL 5
#define GROUP_POWER_SOURCE_COAL_BUL_COST   200000
#define GROUP_POWER_SOURCE_COAL_TECH   200
#define GROUP_POWER_SOURCE_COAL_FIREC  80

#define GROUP_ROAD         11
#define GROUP_ROAD_COLOUR  (white(18))
#define GROUP_ROAD_COST    100
#define GROUP_ROAD_COST_MUL 25
#define GROUP_ROAD_BUL_COST    50
#define GROUP_ROAD_TECH    50
#define GROUP_ROAD_FIREC   4

#define GROUP_INDUSTRY_L   12
#define GROUP_INDUSTRY_L_COLOUR (cyan(18))
#define GROUP_INDUSTRY_L_COST 20000
#define GROUP_INDUSTRY_L_COST_MUL 25
#define GROUP_INDUSTRY_L_BUL_COST 20000
#define GROUP_INDUSTRY_L_TECH 160
#define GROUP_INDUSTRY_L_FIREC 70

#define GROUP_UNIVERSITY   13
#define GROUP_UNIVERSITY_COLOUR (blue(22))
#define GROUP_UNIVERSITY_COST 20000
#define GROUP_UNIVERSITY_COST_MUL 25
#define GROUP_UNIVERSITY_BUL_COST 20000
#define GROUP_UNIVERSITY_TECH 150
#define GROUP_UNIVERSITY_NO_CREDIT
#define GROUP_UNIVERSITY_FIREC 40

#define GROUP_COMMUNE      14
#define GROUP_COMMUNE_COLOUR (green(30))
#define GROUP_COMMUNE_COST  1
#define GROUP_COMMUNE_COST_MUL 2
#define GROUP_COMMUNE_BUL_COST  1000
#define GROUP_COMMUNE_TECH  0
#define GROUP_COMMUNE_FIREC 30

#define GROUP_OREMINE      15
#define GROUP_OREMINE_COLOUR (red(18))
#define GROUP_OREMINE_COST 500
#define GROUP_OREMINE_COST_MUL 10
#define GROUP_OREMINE_BUL_COST 500000
#define GROUP_OREMINE_TECH 0
#define GROUP_OREMINE_FIREC 0

#define GROUP_TIP      16
#define GROUP_TIP_COLOUR (white(16))
#define GROUP_TIP_COST 10000
#define GROUP_TIP_COST_MUL 25
#define GROUP_TIP_BUL_COST 1000000
#define GROUP_TIP_TECH 0
#define GROUP_TIP_FIREC 50

#define GROUP_EX_PORT      17
#define GROUP_EX_PORT_COLOUR (blue(28))
#define GROUP_EX_PORT_COST 100000
#define GROUP_EX_PORT_COST_MUL 2
#define GROUP_EX_PORT_BUL_COST 1000
#define GROUP_EX_PORT_TECH 35
#define GROUP_EX_PORT_FIREC 50
#define GROUP_PORT GROUP_EX_PORT

#define GROUP_INDUSTRY_H   18
// colour 7 is an orange, sort of
#define GROUP_INDUSTRY_H_COLOUR (7)
#define GROUP_INDUSTRY_H_COST 50000
#define GROUP_INDUSTRY_H_COST_MUL 20
#define GROUP_INDUSTRY_H_BUL_COST 70000
#define GROUP_INDUSTRY_H_TECH 170
#define GROUP_INDUSTRY_H_FIREC 80

#define GROUP_PARKLAND     19
#define GROUP_PARKLAND_COLOUR (green(31))
#define GROUP_PARKLAND_COST   1000
#define GROUP_PARKLAND_COST_MUL 25
#define GROUP_PARKLAND_BUL_COST   1000
#define GROUP_PARKLAND_TECH   2
#define GROUP_PARKLAND_NO_CREDIT
#define GROUP_PARKLAND_FIREC 1

#define GROUP_RECYCLE      20
#define GROUP_RECYCLE_COLOUR   (green(28))
#define GROUP_RECYCLE_COST    100000
#define GROUP_RECYCLE_COST_MUL 5
#define GROUP_RECYCLE_BUL_COST    1000
#define GROUP_RECYCLE_TECH    232
#define GROUP_RECYCLE_NO_CREDIT
#define GROUP_RECYCLE_FIREC 10

#define GROUP_WATER	   21
#define GROUP_WATER_COLOUR    (blue(31))
#define GROUP_WATER_COST      1000000
#define GROUP_WATER_COST_MUL 2
#define GROUP_WATER_BUL_COST      1000000
#define GROUP_WATER_TECH      0
// oops
#define GROUP_RIVER (GROUP_WATER)
#define GROUP_WATER_FIREC 0

#define GROUP_HEALTH       22
#define GROUP_HEALTH_COLOUR   (green(24))
#define GROUP_HEALTH_COST     100000
#define GROUP_HEALTH_COST_MUL 2
#define GROUP_HEALTH_BUL_COST     1000
#define GROUP_HEALTH_TECH     110
#define GROUP_HEALTH_FIREC 2

#define GROUP_ROCKET	   23
#define GROUP_ROCKET_COLOUR   (magenta(20))
#define GROUP_ROCKET_COST     700000
#define GROUP_ROCKET_COST_MUL 2
#define GROUP_ROCKET_BUL_COST     1
#define GROUP_ROCKET_TECH     750
#define GROUP_ROCKET_NO_CREDIT
#define GROUP_ROCKET_FIREC 0

#define GROUP_WINDMILL     24
#define GROUP_WINDMILL_COLOUR (green(25))
#define GROUP_WINDMILL_COST   20000
#define GROUP_WINDMILL_COST_MUL 25
#define GROUP_WINDMILL_BUL_COST   1000
#define GROUP_WINDMILL_TECH   30
#define GROUP_WINDMILL_FIREC  10

#define GROUP_MONUMENT     25
#define GROUP_MONUMENT_COLOUR (white(15))
#define GROUP_MONUMENT_COST   10000
#define GROUP_MONUMENT_COST_MUL 25
#define GROUP_MONUMENT_BUL_COST   1000000
#define GROUP_MONUMENT_TECH   0
#define GROUP_MONUMENT_FIREC  0

#define GROUP_SCHOOL     26
#define GROUP_SCHOOL_COLOUR (white(15))
#define GROUP_SCHOOL_COST   10000
#define GROUP_SCHOOL_COST_MUL 25
#define GROUP_SCHOOL_BUL_COST   10000
#define GROUP_SCHOOL_TECH   1
#define GROUP_SCHOOL_FIREC 40

#define GROUP_BLACKSMITH     27
#define GROUP_BLACKSMITH_COLOUR (white(15))
#define GROUP_BLACKSMITH_COST   5000
#define GROUP_BLACKSMITH_COST_MUL 25
#define GROUP_BLACKSMITH_BUL_COST   1000
#define GROUP_BLACKSMITH_TECH   3
#define GROUP_BLACKSMITH_FIREC 60

#define GROUP_MILL     28
#define GROUP_MILL_COLOUR (white(15))
#define GROUP_MILL_COST   10000
#define GROUP_MILL_COST_MUL 25
#define GROUP_MILL_BUL_COST   1000
#define GROUP_MILL_TECH   25
#define GROUP_MILL_FIREC  60

#define GROUP_POTTERY  29
#define GROUP_POTTERY_COLOUR (red(12))
#define GROUP_POTTERY_COST   1000
#define GROUP_POTTERY_COST_MUL 25
#define GROUP_POTTERY_BUL_COST 1000
#define GROUP_POTTERY_TECH  0
#define GROUP_POTTERY_FIREC 50

#define GROUP_FIRESTATION 30
#define GROUP_FIRESTATION_COLOUR (red(20))
#define GROUP_FIRESTATION_COST   20000
#define GROUP_FIRESTATION_COST_MUL 10
#define GROUP_FIRESTATION_BUL_COST 1000
#define GROUP_FIRESTATION_TECH     22
#define GROUP_FIRESTATION_FIREC 0

#define GROUP_CRICKET 31
#define GROUP_CRICKET_COLOUR (white(20))
#define GROUP_CRICKET_COST   2000
#define GROUP_CRICKET_COST_MUL 3
#define GROUP_CRICKET_BUL_COST 1000
#define GROUP_CRICKET_TECH     12
#define GROUP_CRICKET_FIREC 20

// maybe this needs a higher number as there is no button for this.
#define GROUP_BURNT        32
#define GROUP_BURNT_COLOUR    (red(30))
#define GROUP_BURNT_BUL_COST  1000
#define GROUP_FIRE (GROUP_BURNT)
#define GROUP_BURNT_FIREC 0

#define GROUP_SHANTY        33
#define GROUP_SHANTY_COLOUR    (red(22))
#define GROUP_SHANTY_BUL_COST  100000
#define GROUP_SHANTY_FIREC     25
#define SHANTY_MIN_PP     150
#define SHANTY_POP        50
#define DAYS_BETWEEN_SHANTY (NUMOF_DAYS_IN_MONTH*5)
#define SHANTY_GET_FOOD   50
#define SHANTY_GET_JOBS   5
#define SHANTY_GET_GOODS  50
#define SHANTY_GET_COAL   1
#define SHANTY_GET_ORE    10
#define SHANTY_GET_STEEL  1

#define MOUSE_TYPE_NORMAL 1
#define MOUSE_TYPE_SQUARE 2
#define MOUSE_BUTTON_REPEAT 4

#define red(x) (32 + x)
#define green(x) (64 + x)
#define yellow(x) (96 + x)
#define blue(x) (128 + x)
#define magenta(x) (160 + x)
#define cyan(x) (192 + x)
#define white(x) (224 + x)

struct TYPE {
	unsigned short group;  // the group it belongs to eg power line or residence ...
	unsigned short size;
	char *graphic;
	};

struct MAPPOINT {
	int population;
	int flags;
	unsigned short coal_reserve;
	unsigned short ore_reserve;
// general ints
	int int_1;
	int int_2;
	int int_3;
	int int_4;
	int int_5;
	int int_6;
	int int_7;
	};
			
//  GLOBAL VARIABLES
//  ****************
//extern int selected_type; // the type selected by buttons to draw in main win
//extern int select_button_type[]= // button number => selected type
//	 { CST_POWERL_H_L, CST_POWERS_SOLAR };
//extern int cs_mouse_x,cs_mouse_y,cs_mouse_button; // current mouse status
//extern int cs_mouse_xmax,cs_mouse_ymax;


// main functions
// **************
extern void test(void);
extern void do_error(char *);
extern void clear_data(void);
extern void do_save_city(void);
extern void remove_scene(char *);
extern void save_city(char *);
extern void do_load_city(void);
extern void load_op_city(char *);
extern void load_city(char *);
extern void reset_animation_times(void);
extern void coal_reserve_setup(void);
extern void ore_reserve_setup(void);
extern void setup_river(void);
extern void setup_river2(int,int,int);
extern void check_savedir(void);
extern void draw_save_dir(int);
extern void input_save_filename(char *);
extern int verify_city(char *);
extern void load_start_image(void);
extern void start_image_text(void);
extern void si_scroll_text(void);
extern char si_next_char(FILE *);
extern void get_real_time(void);
extern void time_for_year(void);
extern void debug_writeval(int);
extern int cheat(void);
extern void print_cheat(void);
extern void unprint_cheat(void);
extern void order_select_buttons(void);
extern void lincityrc(void);
extern void check_for_old_save_dir(void);
extern int count_groups(int);
extern void count_all_groups(void);
extern int compile_results(void);
extern void print_results(void);
extern void mail_results(void);
extern void window_results(void);
extern void random_start(void);
extern void quick_start_add(int,int,int,int);
extern void init_path_strings(void);
extern void lc_usleep(unsigned long);
extern void dump_tcore(void);
#ifdef MP_SANITY_CHECK
extern void sanity_check(void);
#endif
#ifndef LC_X11
extern void parse_args(int, char **);
#endif
extern void sustainability_test(void);
extern int sust_fire_cover(void);
extern void check_endian(void);
extern void eswap32(int *);
extern void eswap16(unsigned short *);

// type init functions
// *******************
extern void init_types(void);
extern char *load_graphic(char *);

// screen functions
// ****************
#ifdef CS_PROFILE
extern void FPgl_write(int,int,char *);
extern void FPgl_getbox(int,int,int,int,void *);
extern void FPgl_putbox(int,int,int,int,void *);
extern void FPgl_fillbox(int,int,int,int,int);
extern void FPgl_hline(int,int,int,int);
extern void FPgl_line(int,int,int,int,int);
extern void FPgl_setpixel(int,int,int);
#endif
extern void check_power_line_graphics(void);
extern void check_water_graphics(void);
extern void check_track_graphics(void);
extern void check_rail_graphics(void);
extern void check_road_graphics(void);
extern int select_power_line_type(int,int,int,int);
extern int select_water_type(int,int,int,int);
extern int select_track_type(int,int,int,int);
extern int select_rail_type(int,int,int,int);
extern int select_road_type(int,int,int,int);
extern void screen_setup(void);
extern void update_select_buttons(void);
extern void draw_main_window_box(int);
extern void draw_select_button_graphic(int,char *);
extern void setcustompalette(void);
extern void refresh_main_screen(void);
extern void update_main_screen(void);
extern void clip_main_window(void);
extern void unclip_main_window(void);
extern void initfont(void);
extern void load_scroll_buttons(void);
extern void draw_scroll_buttons(void);
extern void update_mini_screen(void);
extern void draw_mini_screen(void);
extern void draw_big_mini_screen(void);
extern void draw_mini_screen_pollution(void);
extern void draw_mini_screen_fire_cover(void);
extern void draw_mini_screen_cricket_cover(void);
extern void draw_mini_screen_health_cover(void);
extern void draw_mini_screen_ub40(void);
extern void draw_mini_screen_starve(void);
extern void draw_mini_screen_coal(void);
extern void draw_mini_screen_power(void);
extern void draw_mini_screen_ocost(void);
extern void draw_mini_screen_port(void);
extern void draw_mini_screen_cursor(void);
extern void print_stats(void);
extern void print_total_money(void);
extern void do_monthgraph(void);
extern void draw_diffgraph(void);
extern void draw_market_cb(void);
extern void clicked_market_cb(int,int);
extern void close_market_cb(void);
extern void draw_port_cb(void);
extern void clicked_port_cb(int,int);
extern void close_port_cb(void);
extern int yn_dial_box(char *,char *,char *,char *);
extern void ok_dial_box(char *,int,char *);
extern int inv_sbut(int);
extern void call_select_change_up(int);
extern void beg_space_pad(char *,int);
extern void draw_up_pbar(int,int,int,int);
extern void draw_down_pbar(int,int,int,int);
extern void do_pbar_population(int);
extern void do_pbar_tech(int);
extern void do_pbar_food(int);
extern void do_pbar_jobs(int);
extern void do_pbar_coal(int);
extern void do_pbar_goods(int);
extern void do_pbar_ore(int);
extern void do_pbar_steel(int);
extern void do_pbar_money(int);
extern void prog_box(char *,int);
extern void draw_sustainable_window(void);
extern void draw_sustline(int,int,int,int);

// mappoint stats functions
// ************************
extern void mappoint_stats(int,int,int);
extern void mps_res_setup(void);
extern void mps_res(int,int);
extern void mps_transport_setup(void);
extern void mps_road(int,int);
extern void mps_rail(int,int);
extern void mps_track(int,int);
extern void mps_farm_setup(void);
extern void mps_farm(int,int);
extern void mps_market_setup(void);
extern void mps_market(int,int);
extern void mps_indl_setup(void);
extern void mps_indl(int,int);
extern void mps_indh_setup(void);
extern void mps_indh(int,int);
extern void mps_coalmine_setup(void);
extern void mps_coalmine(int,int);
extern void mps_power_source_coal_setup(void);
extern void mps_power_source_coal(int,int);
extern void mps_power_source_setup(void);
extern void mps_power_source(int,int);
extern void mps_university_setup(void);
extern void mps_university(int,int);
extern void mps_recycle_setup(void);
extern void mps_recycle(int,int);
extern void mps_oremine_setup(void);
extern void mps_oremine(int,int);
extern void mps_substation_setup(void);
extern void mps_substation(int,int);
extern void mps_rocket_setup(void);
extern void mps_rocket(int,int);
extern void mps_windmill_setup(int,int);
extern void mps_windmill(int,int);
extern void mps_monument_setup(void);
extern void mps_monument(int,int);
extern void mps_school_setup(void);
extern void mps_school(int,int);
extern void mps_blacksmith_setup(void);
extern void mps_blacksmith(int,int);
extern void mps_mill_setup(void);
extern void mps_mill(int,int);
extern void mps_pottery_setup(void);
extern void mps_pottery(int,int);
extern void mps_water(int,int);
extern void mps_port_setup(void);
extern void mps_port(int,int);
extern void mps_tip_setup(void);
extern void mps_tip(int,int);
extern void mps_commune_setup(void);
extern void mps_commune(int,int);
extern void mps_right_setup(void);
extern void mps_right(int,int);
extern void mps_firestation_setup(void);
extern void mps_firestation(int,int);
extern void mps_cricket_setup(void);  
extern void mps_cricket(int,int);
extern void mps_health_setup(void);
extern void mps_health(int,int);

// mouse functions
// ***************
extern void mouse_setup(void);
extern void cs_mouse_repeat(void);
extern void cs_mouse_handler(int,int,int);
extern void move_mouse(int,int);
extern void do_mouse_main_win(int,int,int);
extern int no_credit_build(void);
extern void hide_mouse(void);
extern void redraw_mouse(void);
extern void draw_square_mouse(int,int,int);
extern void hide_square_mouse(void);
extern void redraw_square_mouse(void);
extern void draw_normal_mouse(int,int);
extern void hide_normal_mouse(void);
extern void redraw_normal_mouse(void);
extern void do_mouse_select_buttons(int,int,int);
extern void do_select_button(int,int);
extern void highlight_select_button(int);
extern void unhighlight_select_button(int);
extern void set_mappoint_ints(int,int,int,int);
extern void do_mouse_other_buttons(int,int,int);
extern void bulldoze_area(int,int);
extern void fire_area(int,int);
extern void do_bulldoze_area(int,int,int);
extern void clear_mappoint(int,int,int);
extern void do_market_cb_mouse(int,int);
extern void do_port_cb_mouse(int,int);
extern void do_db_mouse(int,int);
extern void do_db_okmouse(int,int);
extern void connect_rivers(void);
extern int is_real_river(int,int);
extern void select_pause(void);
extern void select_fast(void);
extern void select_medium(void);
extern void select_slow(void);
extern void choose_residence(void);
extern void do_multi_transport(int,int,int);
extern int mt_draw(int,int);

// engine functions
// ****************
extern void do_time_step(void);
extern void do_residence(int,int);
extern void debug_print(int);
extern void do_power_source(int,int);
extern void do_power_source_coal(int,int);
extern void do_industry_l(int,int);
extern void do_industry_h(int,int);
extern void do_power_substation(int,int);
extern int get_power(int,int,int,int);
extern int add_a_substation(int,int);
extern void remove_a_substation(int,int);
extern void do_organic_farm(int,int);
extern void shuffle_mappoint_array(void);
extern void shuffle_substations(void);
extern void do_coalmine(int,int);
extern void do_oremine(int,int);
extern void do_commune(int,int);
extern void do_port(int,int);
extern int buy_food(int,int);
extern int buy_jobs(int,int);
extern int buy_coal(int,int);
extern int buy_ore(int,int);
extern int buy_goods(int,int);
extern int buy_steel(int,int);
extern int sell_food(int,int);
extern int sell_jobs(int,int);
extern int sell_coal(int,int);
extern int sell_ore(int,int);
extern int sell_goods(int,int);
extern int sell_steel(int,int);

extern void do_pollution(void);
extern void do_parkland(int,int);
extern void do_university(int,int);
extern void do_recycle(int,int);
extern void do_health_centre(int,int);
extern void do_rocket_pad(int,int);
extern void launch_rocket(int x,int y);
extern void remove_people(int);
extern void do_windmill(int,int);
extern void do_monument(int,int);
extern void do_school(int,int);
extern void do_blacksmith(int,int);
extern void do_mill(int,int);
extern void do_pottery(int,int);
extern void do_firestation(int,int);
extern void do_cricket(int,int);
extern void clear_fire_health_and_cricket_cover(void);
extern void do_fire_health_and_cricket_cover(void);
extern void do_fire_cover(int,int);
extern void do_health_cover(int,int);
extern void do_cricket_cover(int,int);
extern void do_random_fire(int,int,int);
extern void do_fire(int,int);
extern int spiral_find_group(int,int,int);
extern int spiral_find_2x2(int,int);
extern void add_a_shanty(void);
extern void remove_a_shanty(void);
extern void update_shanty(void);
extern void do_shanty(int,int);
extern void do_tip(int,int);

// transport functions
// *******************
extern void do_power_line(int,int);
extern void do_track(int,int);
extern void do_rail(int,int);
extern void do_road(int,int);
#ifndef THMO_TRANSPORT
extern void t_av_l(int,int);
extern void t_av_u(int,int);
extern void t_av_lu(int,int);
extern void t_av_r(int,int);
extern void t_av_lr(int,int);
extern void t_av_ur(int,int);
extern void t_av_lur(int,int);
extern void t_av_d(int,int);
extern void t_av_ld(int,int);
extern void t_av_ud(int,int);
extern void t_av_lud(int,int);
extern void t_av_rd(int,int);
extern void t_av_lrd(int,int);
extern void t_av_urd(int,int);
extern void t_av_lurd(int,int);
#else
extern void general_transport(struct MAPPOINT  *,int *,int,int *);
#endif

// market functions
// ****************
extern int get_food(int,int,int);
extern int put_food(int,int,int);
extern int get_jobs(int,int,int);
extern int put_jobs(int,int,int);
extern int get_goods(int,int,int);
extern int put_goods(int,int,int);
extern int get_ore(int,int,int);
extern int put_ore(int,int,int);
extern int get_coal(int,int,int);
extern int put_coal(int,int,int);
extern int add_a_market(int,int);
extern void remove_a_market(int,int);
extern void do_market(int,int);
extern void shuffle_markets(void);
extern int deal_with_transport(int,int,int,int);
extern int get_steel(int,int,int);
extern int put_steel(int,int,int);
extern int get_waste(int,int,int);
extern int put_waste(int,int,int);
extern int get_stuff(int,int,int,int);
#ifdef THMO_GP_STUFF
extern int get_stuff2(struct MAPPOINT *,int,int);
extern int get_stuff3(struct MAPPOINT *,int,int);
extern int get_stuff4(struct MAPPOINT *,int,int);
#else
extern int get_stuff2(int,int,int,int);
extern int get_stuff3(int,int,int,int);
extern int get_stuff4(int,int,int,int);
#endif
extern int put_stuff(int,int,int,int);
#ifdef THMO_GP_STUFF
extern int put_stuff2(struct MAPPOINT *,short *,int,int);
extern int put_stuff3(struct MAPPOINT *,short *,int,int);
extern int put_stuff4(struct MAPPOINT *,short *,int,int);
#else
extern int put_stuff2(int,int,int,int);
extern int put_stuff3(int,int,int,int);
extern int put_stuff4(int,int,int,int);
#endif

// help functions
// **************
extern void activate_help(char *);
extern void do_help_mouse(int,int,int);
extern void draw_help_page(char *);
extern void parse_tcolourline(char *);
extern void parse_helpline(char *);
extern void parse_textline(char *);
extern void parse_iconline(char *);
extern void draw_help_icon(int,int,char *);
extern void parse_buttonline(char *);
extern void do_help_buttons(int,int);
extern void parse_tbuttonline(char *);

#ifdef LC_X11
// X11 functions
// *************
extern void Fgl_write(int,int,char *);
extern void open_write(int,int,char *);
extern void Fgl_getbox(int,int,int,int,void *);
extern void Fgl_putbox(int,int,int,int,void *);
extern void Fgl_fillbox(int,int,int,int,int);
extern void Fgl_hline(int,int,int,int);
extern void Fgl_line(int,int,int,int,int);
extern void Fgl_setpixel(int,int,int);
extern void Fgl_setfontcolors(int,int);
extern void Fgl_enableclipping(void);
extern void Fgl_setclippingwindow(int,int,int,int);
extern void Fgl_disableclipping(void);
extern void open_setcustompalette(XColor *);

#ifdef USE_X11_PIXMAPS
#define USE_PIXMAPS
extern void init_pixmaps(void);
extern void init_icon_pixmap(int);
extern void update_pixmap(int,int,int,int,int,int,int,char *);
#endif
#endif


// profiling
#ifdef CS_PROFILE

#define Fgl_write(a,b,c) FPgl_write(a,b,c)
#define Fgl_getbox(a,b,c,d,e) FPgl_getbox(a,b,c,d,e)
#define Fgl_putbox(a,b,c,d,e) FPgl_putbox(a,b,c,d,e)
#define Fgl_fillbox(a,b,c,d,e) FPgl_fillbox(a,b,c,d,e)
#define Fgl_hline(a,b,c,d) FPgl_hline(a,b,c,d)
#define Fgl_line(a,b,c,d,e) FPgl_line(a,b,c,d,e)
#define Fgl_setpixel(a,b,c) FPgl_setpixel(a,b,c)
#define Fgl_setfontcolors(a,b) gl_setfontcolors(a,b)
#define Fgl_enableclipping()   gl_enableclipping()
#define Fgl_setclippingwindow(a,b,c,d)   gl_setclippingwindow(a,b,c,d)
#define Fgl_disableclipping()   gl_disableclipping() 

#else

#ifndef LC_X11
#define Fgl_write(a,b,c) gl_write(a,b,c)
#define Fgl_getbox(a,b,c,d,e) gl_getbox(a,b,c,d,e)
#define Fgl_putbox(a,b,c,d,e) gl_putbox(a,b,c,d,e)
#define Fgl_fillbox(a,b,c,d,e) gl_fillbox(a,b,c,d,e)
#define Fgl_hline(a,b,c,d) gl_hline(a,b,c,d)
#define Fgl_line(a,b,c,d,e) gl_line(a,b,c,d,e)
#define Fgl_setpixel(a,b,c) gl_setpixel(a,b,c)
#define Fgl_setfontcolors(a,b) gl_setfontcolors(a,b)
#define Fgl_enableclipping()   gl_enableclipping()
#define Fgl_setclippingwindow(a,b,c,d)   gl_setclippingwindow(a,b,c,d)
#define Fgl_disableclipping()   gl_disableclipping()
#endif
#endif

// *******   end of lin-city.h   ***********
// *****************************************
