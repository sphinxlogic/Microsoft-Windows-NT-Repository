#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define windowW 896
#define windowH 768

#define radius 14
#define diameter (radius*2)

#define border (diameter)
#define pocket_size (diameter+radius)

#define left_edge (windowW/20)
#define right_edge (windowW-left_edge)
#define bottom_edge (windowH/5)
#define top_edge (windowH-bottom_edge)
#define break_offset ((right_edge-left_edge)/4)
#define break_point (left_edge+break_offset)
#define spot_offset ((right_edge-left_edge)/4)
#define spot_point (right_edge-spot_offset)

#define max_balls 16

#define STRAIGHT_POOL 1
#define EIGHT_BALL    2
#define NINE_BALL     3
#define ROTATION      4

#define TABLE         1
#define BALLRACK      2
#define RESPOT        4
#define FELT          8
#define HEAD          16

#define OVER_TABLE    1
#define OVER_BALLRACK 2
#define OVER_RESPOT   3
#define OVER_FELT     4
#define OVER_HEAD     5
#define OVER_ABORT    6

#define HUMAN    1
#define COMPUTER 2
#define NETWORK  3
#define SELF     4
#define AUTONET  5
#define AUTOLOCK 6

#define ABORT_GAME	1
#define ABORT_PROGRAM	2
#define ABORT_REMOTE	3

#define NOVALUE -9999

#define LOCAL	0
#define REMOTE	1

#define MAX_SCREENS	2

#define system system_type
