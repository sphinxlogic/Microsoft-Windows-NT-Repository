/*****************************************************************************
 *
 * FACILITY:
 *   XPool	DECwindows Pool game
 *	
 * ABSTRACT:
 *   This module contains the routines which perform the pool ball and
 *   user interaction.  It also contains the main entry point.
 *
 * AUTHOR:
 *   Doug Stefanelli
 *
 * CREATION DATE: 1-October-1989
 *
 * Edit History 
 *
 *  DS	1-Oct-1989	(v1.0) Original version
 *  DS	4-Dec-1989	(v2.0) Modified to run from XtMainLoop().  Created
 *			pool_table().
 *  DS	2-Jan-1990	(v2.1) Improve computer play.  Fix cue ball bank shots
 *			and improve shot evaluation and scratch checking.
 *  DS	13-Mar-1990	(v3.0) Support 2 displays.  Add Ultrix support.  Add
 *			game - rotation.  Fix rules - 8-ball on break
 *			is win, leave last ball on table in straight pool.
 *  DS	11-Apr-1990	(v3.1) Misc. bug fixes.  Make computer skill levels
 *			more consistent.  Enforce hitting selected ball first
 *			in 8-ball.
 *  DS	10-May-1990	(v4.0) Widen table, reduce ball overlap, fix momentum
 *			transfer at collisions, make friction and spin
 *			more realistic, tighten the ball rack.
 */

/*
 * Routines in this module
 */
int gtime();			/* return time in seconds since 1970 */
void pool_table();		/* main state table */
static int shooting_8ball();	/* returns true if the 8 ball is the target */
static int rightmost_target();	/* returns rightmost legal ball on table */
void number_balls();		/* number all balls on the table except cue */
void unnumber_balls();		/* remove numbers from balls on the table */
void draw_numbered_ball();	/* draw a numbered ball */
void replace_ball();		/* move a ball to a specified position */
void spot_ball();		/* respot an illegally sunk ball */
void spot_cue();		/* let the user spot the cue ball */
static void get_game_string();	/* return a string identifying the game */
static void display_call();	/* display the target ball and target pocket */
static float determine_shot();	/* determine the best shot for the computer */
static int blocked();		/* returns true if a shot is blocked */
static int scratch();		/* returns true if a shot might scratch */
static void delay();		/* preprocess routine to handle delays */
static void start_remote_read();/* preprocess routine to handle remote reads */
static void start_remote_write();/* preprocess routine to handle remote writes */
static void wait_for_opponent();/* preprocess routine to wait for opponent */

/*
 * Include files
 */
#include <stdio.h>
#include <math.h>
#include "xpool.h"
#ifdef ultrix
#include <sys/time.h>
#endif

#define time_between_frames 40		/* in milliseconds */
/*
 * Math macros
 */
#define absolute(x) (float)fabs(x)
#define square(x) ((x)*(x))
#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))
#define sqroot(x) (float)sqrt((double)(x))
#define alog10(x) (float)log10((double)(x))
#define random_number ((float)rand()/(float)(0x7fffffff))
#define magnitude(x,y) sqroot((x)*(x) + (y)*(y))
#define distance(x1,y1,x2,y2) sqroot(square((x2)-(x1)) + square((y2)-(y1)))
#define distance2(x1,y1,x2,y2,x3,y3) (distance(x1,y1,x2,y2) + \
				      distance(x2,y2,x3,y3))
#define distance3(x1,y1,x2,y2,x3,y3,x4,y4) (distance2(x1,y1,x2,y2,x3,y3) + \
					    distance(x3,y3,x4,y4))
#define overlap(index) (square(pointer_x - (int)px[index]) + \
			square(pointer_y - (int)py[index]) < \
			square(diameter))

#define shooting_low_balls (selection_made && (im_high ^ my_turn))
#define shooting_high_balls (selection_made && (!im_high ^ my_turn))
#define cushion_friction .8		/* v3.1 was .9 */
#define contact_friction .75		/* v3.1 was .9 */
#define felt_friction .986		/* v3.1 was .985 */
#define high_felt_friction .94		/* v3.1 was .94 */
#define backoff_t .05
#define velocity_scale .55
#define max_skill (100./800.)
/*
 * Local variables
 */
#define y_center ((top_edge + bottom_edge)/2)
#define cue_x (break_point - 3*diameter)
#define cue_y (y_center - diameter)
#define pxinit_offset 24	/* v3.1 was 25 */
#define pyinit_offset 15	/* v3.1 was 19 */
static int pxinit0[]  = {0, 0, 1, 2, 3, 3, 4, 4, 2, 1, 2, 3, 3, 4, 4, 4};
static int pyinit0[]  = {0, 0, 1, 0, 3, 1, 3, 1, 1, 0, 2, 2, 0, 4, 2, 0};
static int pxinit15[] = {0, 0, 4, 4, 1, 4, 3, 3, 4, 1, 2, 4, 2, 3, 3, 2};
static int pyinit15[] = {0, 0, 0, 4, 0, 1, 3, 0, 3, 1, 0, 2, 2, 1, 2, 1};
static int pxinit9[]  = {0, 0, 1, 1, 2, 2, 3, 3, 4, 2};
static int pyinit9[]  = {0, 0, 1, 0, 2, 0, 2, 1, 2, 1};

static float pxinit[max_balls], pyinit[max_balls];
static float psavex[max_balls], psavey[max_balls];
static float vsavex[max_balls], vsavey[max_balls];
static float tsave[max_balls];
static float skill;
static float spin_factor;
static float roll_x, roll_y;
static float pocket[3], dist[3];
static float pocketx[6], pockety[6];
static float buffer[20];

static int stop_count;
static int ball_selected;
static int ball_hit_first;
static int first_ball, first_pos;
static int number_sunk;
static int sunken_ball[max_balls];
static int sunken_pocket[max_balls];
static int nbuffer[20];
static int lock_size;
static int status;
static int ball_index;
static int ball_count;
static int buffer_index;

static char *lock_buffer;

static unsigned char after_scratch;
static unsigned char respot_cue;
static unsigned char confirmed;
static unsigned char any_contact;
static unsigned char far_wall_hit;
static unsigned char first_shot;
static unsigned char last_shot[2];
static unsigned char hit_wall[2];
static unsigned char humans_turn;
static unsigned char computers_turn;
static unsigned char remote_turn;
static unsigned char selected_ball_hit_first;

static char nodename[80];
static char username[80];
/*
 * Valid states within pool_table()
 */
static int state;
static int next_state;

#define GAME_INIT			1
#define CHALLENGE_RESPONSE		2
#define NET_INIT			3
#define GAME_SELECT			4
#define GAME_SETUP			5
#define LAG_FOR_BREAK_SETUP		6
#define LAG_FOR_BREAK			7
#define GOT_LAG_VELOCITY		8
#define DO_LAG				9
#define LAG_WINNER			10
#define START_PLAY			11
#define BALLS_STOPPED			12
#define OPPONENTS_TURN			13
#define CHECK_RESPOT			14
#define HILO_SELECT			15
#define HILO_DISPLAY			16
#define HILO_RESPONSE			17
#define HILO_SET			18
#define GET_INPUT			19
#define COMPUTER_SPOT_CUE		20
#define COMPUTER_DETERMINE_SHOT		21
#define COMPUTER_REPORT_SHOT		22
#define COMPUTER_CONFIRM_SHOT		23
#define COMPUTER_WAIT_BEFORE_SHOT	24
#define READ_POSITION			25
#define POSITION_READ			26
#define DISPLAY_CALL			27
#define READ_VELOCITY			28
#define VELOCITY_READ			29
#define CONFIRM_VELOCITY		30
#define PLACE_BALL			31
#define GET_CUE_POSITION		32
#define REPORT_POSITION			33
#define CALL_SHOT			34
#define GET_TARGET_BALL			35
#define GET_TARGET_POCKET		36
#define WAIT_BEFORE_SHOT		37
#define SET_DIRECTION			38
#define GOT_POSITION			39
#define DO_RESPOT			40
#define CHECK_RESPOT_BALL		41
#define CHECK_RESPOT_OVERLAP		42
#define SET_VELOCITY			43
#define GOT_VELOCITY			44
#define CONFIRM_SHOT			45
#define CONFIRM_RESPONSE		46
#define FRAME_SETUP			47
#define MOVE_BALLS			48
#define DELAY_OVER			49
#define CHECK_NET_READ			50
#define CHECK_LOCK_READ			51
#define CHECK_LOCK_WRITE		52
#define WAIT_FOR_OPPONENT		53
#define GAME_LOST			54
#define GAME_OVER			55
#define BAD_NODE			56
#define LOST_NET			57
#define LOST_LOCK			58
#define END_GAME			59
#define RESTART_GAME			60
#define SYNC_POSITIONS			61
#define SEND_POSITIONS			62
#define RECEIVE_POSITIONS		63
#define POSITION_RECEIVED		64
#define GAME_DISPLAY			65
#define GET_NAME			66
#define SEND_NAME			67
#define READ_NAME			68
#define NAME_READ			69

/*
 * Global variables
 */
float px[max_balls], py[max_balls];
float vx[max_balls], vy[max_balls];
int system;
int leftx, rightx;
int num_balls;
int wins, losses;
int my_score, his_score;
int straight_limit;
int target_ball, target_pocket;
int balls_on_table;
unsigned char on_table[max_balls];
unsigned char off_table[max_balls];
unsigned char net_game;
unsigned char lock_game;
unsigned char alternating;
unsigned char im_computer;
unsigned char ur_computer;
unsigned char im_remote;
unsigned char remote_process;
unsigned char action_started;
unsigned char balls_numbered;
unsigned char intro_displayed;
unsigned char my_turn;
unsigned char im_high;
unsigned char selection_made;
char *player_name[2];
#define MAX_NAME_LENGTH 12
char my_name[MAX_NAME_LENGTH];
char his_name[MAX_NAME_LENGTH];

/*
 * External variables
 */
extern int num_screens;
extern int game;
extern int opponent;
extern int network_type;
extern int computer_skill;
extern int limit;
extern int menu_selection;
extern int velocity;
extern int roll;
extern int spin;
extern int remote_velocity;
extern unsigned char preset_velocity[];
extern unsigned char confirm_shots[];
extern unsigned char sound[];
extern unsigned char abort_flag;
extern unsigned char challenge_accepted;
extern char node[];
extern int position;
extern int pointer_x, pointer_y;

/*
 * Wide area network routines (in XPOOL_NET.C)
 */
extern int get_local_user();
extern int get_remote_user();
extern void net_open_remote();
extern void net_open_local();
extern void net_read();
extern int net_read_complete();
extern int net_write();
extern void net_close();
/*
 * Cluster lock routines (in XPOOL_ENQ.C)
 */
extern int grab_lock();
extern int lock_grabbed();
extern int lock_read();
extern int lock_write();
extern void lock_close();
/*
 * Xlib routines (in XPOOL_XLIB.C)
 */
extern void init_pool_graphics();
extern void draw_table();
extern void draw_ball();
extern void erase_ball();
extern void number_ball();
extern void add_to_ballrack();
extern void redraw_ballrack();
extern void save_text();
extern void draw_centered_text();
extern void draw_centered_text2();
extern void draw_remote_text();
extern void draw_remote_text2();
extern void clear_message_area();
extern void clear_remote_area();
extern void display_score();
/*
 * X toolkit routines (in XPOOL_DWT.C)
 */
extern int init_screen();
extern void remove_menus();
extern void destroy_remote_screen();
extern void enable_button();
extern void disable_button();
extern void accept_challenge();
extern void select_menu();
extern void get_velocity();
extern void set_frame_timeout();
extern void set_delay_timeout();
extern void cancel_delay_timeout();
extern void cancel_frame_timeout();

int gtime()
{
#ifdef VMS
    return(time());
#else
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);
    return(tp.tv_sec);
#endif
}

main(argv,argc)
char *argv[];
int argc;
{
char string[80];
char *ptr;

    srand(gtime());	/* initialize random # generator */

    /* define player names */
    if (!(player_name[0] = getenv("XPOOL$PLAYER1")))
	player_name[0] = "PLAYER 1";
    if (!(player_name[1] = getenv("XPOOL$PLAYER2")))
	player_name[1] = "PLAYER 2";
    if (!(ptr = getenv("XPOOL$ME"))) {
	ptr = getenv("SYS$NODE");
	if (!ptr)
	    strcpy(my_name, "ME");
	else {
	    strncpy(my_name, ptr, 12);
	    my_name[min(strlen(ptr)-2,MAX_NAME_LENGTH-1)] = '\0';
	}
    } else {
	strcpy(my_name, ptr);
	my_name[MAX_NAME_LENGTH-1] = '\0';
    }

    /* define pocket positions */
    pocket[0] = left_edge;
    pocket[1] = (right_edge + left_edge)/2;
    pocket[2] = right_edge;

    pocketx[0] = pocket[0] + radius;
    pocketx[1] = pocket[1];
    pocketx[2] = pocket[2] - radius;
    pocketx[3] = pocket[0] + radius;
    pocketx[4] = pocket[1];
    pocketx[5] = pocket[2] - radius;
    pockety[0] = top_edge - radius;
    pockety[1] = top_edge - radius;
    pockety[2] = top_edge - radius;
    pockety[3] = bottom_edge + radius;
    pockety[4] = bottom_edge + radius;
    pockety[5] = bottom_edge + radius;

    /* define acceptable distances from pockets for balls to fall in */
    dist[0] = diameter;
    dist[1] = 1.25*radius;
    dist[2] = diameter;

    im_remote = get_remote_user(nodename, username);

    if (!init_screen(0)) {
	printf("Unable to access target display...\n");
	exit(1);
    }
    if (!im_remote) init_pool_graphics(LOCAL);

    action_started = FALSE;
    balls_numbered = FALSE;

    if (!im_remote) {
	num_balls = 0;
	intro_displayed = TRUE;
	state = GAME_INIT;
	select_play();
    } else {
	intro_displayed = FALSE;
	my_turn = FALSE;
	system = LOCAL;
	net_game = TRUE;
	net_open_local();
	sprintf(string, "You are being challenged by %s::%s, Do you accept?",
		nodename, username);
	state = NET_INIT;
	accept_challenge(LOCAL, string);
    }
}


void pool_table()
{
int i,j;
int bytes_read;
unsigned char bad;
unsigned char last_turn;
int respot_count;
int locount, hicount;
float ax, ay, bx, by, b_size_2, a_dot_b;
float vx1, vy1, vx2, vy2, vxtrans, vytrans, vsize;
float dpx, dpy, dpx1, dpy1;
float x1, y1, x2, y2;
float a, b, c, d, t;
float scale;
char string[80];
static char *hilo_strings[] = {"High", "Low"};
static char *confirm_strings[] = {"Proceed", "Redo"};

    if (abort_flag)
	state = END_GAME;

    for (;;) {			/* main state table loop */

    switch (state) {
	case GAME_INIT:
	    if (intro_displayed) {
		clear_message_area();
		intro_displayed = FALSE;
	    }
	    draw_table();
	    straight_limit = limit;
	    net_game = (opponent == NETWORK || opponent == AUTONET);
	    lock_game = (opponent == AUTOLOCK);
	    remote_process = lock_game;
	    system = LOCAL;
	    if (net_game) {
		my_turn = TRUE;
		state = CHALLENGE_RESPONSE;
		if (network_type == LOCAL) {
		    sprintf(string, "%s::0.0", node);
		    if (!init_screen(string)) {
			state = BAD_NODE;
			break;
		    }
		    get_local_user(nodename, username);
		    sprintf(string,
			"You are being challenged by %s::%s, Do you accept?",
			nodename, username);
		    accept_challenge(REMOTE, string);
		} else {
		    remote_process = TRUE;
		    net_open_remote(node);
		    start_remote_read(&challenge_accepted, 1);
		}
		return;
	    } else if (lock_game) {
		im_remote = 0;
		switch (grab_lock()) {
		case 0:
		    my_turn = 1;
		    draw_centered_text("WAITING FOR AN OPPONENT");
		    state = WAIT_FOR_OPPONENT;
		    delay(1);
		    return;
		case 1:
		    my_turn = 0;
		    im_remote = 1;
		    state = GAME_SELECT;
		    start_remote_read(nbuffer, 12);
		    return;
		case 2:
		    draw_centered_text("GAME IN PROGRESS");
		    state = END_GAME;
		    delay(6);
		    return;
		default:
		    state = LOST_LOCK;
		    break;
		}
	    } else {
		my_turn = (random_number < .5 ? 1 : 0);
		state = GAME_SETUP;
	    }
	    break;

	case CHALLENGE_RESPONSE:
	    if (!challenge_accepted) {
		draw_centered_text("THE CHALLENGE WAS NOT ACCEPTED");
		state = END_GAME;
		delay(6);
		return;
	    }
	    if (num_screens > 1) {
		init_pool_graphics(REMOTE);
		state = GAME_DISPLAY;
	    } else {
		state = SEND_NAME;
		nbuffer[0] = game;
		nbuffer[1] = straight_limit;
		nbuffer[2] = opponent;
		start_remote_write(nbuffer, 12);
	    }
	    break;

	case NET_INIT:
	    if (net_write(&challenge_accepted, 1) <= 0) {
		if (challenge_accepted) printf("Network connection lost...\n");
		exit(1);
	    }
	    if (!challenge_accepted) exit(1);
	    init_pool_graphics();
	    num_balls = 0;
	    state = GAME_SELECT;
	    start_remote_read(nbuffer, 12);
	    return;

	case GAME_SELECT:
	    game = nbuffer[0];
	    straight_limit = nbuffer[1];
	    opponent = nbuffer[2];
	    state = GET_NAME;
	    start_remote_read(his_name, MAX_NAME_LENGTH);	/* other player's name */
	    return;

	case GET_NAME:
	    his_name[MAX_NAME_LENGTH-1] = '\0';
	    player_name[0] = his_name;
	    player_name[1] = my_name;
	    state = GAME_DISPLAY;
	    start_remote_write(my_name, MAX_NAME_LENGTH);
	    break;

	case SEND_NAME:
	    state = READ_NAME;
	    start_remote_write(my_name, MAX_NAME_LENGTH);
	    break;

	case READ_NAME:
	    state = NAME_READ;
	    start_remote_read(his_name, MAX_NAME_LENGTH);
	    return;

	case NAME_READ:
	    his_name[MAX_NAME_LENGTH-1] = '\0';
	    player_name[0] = my_name;
	    player_name[1] = his_name;
	    state = GAME_DISPLAY;
	    break;

	case GAME_DISPLAY:
	    get_game_string(string);
	    draw_centered_text(string);
	    if (num_screens > 1)
		draw_remote_text(string);
	    state = GAME_SETUP;
	    delay(5);
	    return;

	case GAME_SETUP:
	    alternating = FALSE;
	    im_computer = FALSE;
	    ur_computer = FALSE;
	    skill = 0.0;

	    switch (opponent) {
		case COMPUTER:
		    im_computer = alternating = TRUE;
		    skill = (float)(100-computer_skill)*(max_skill/100.);
		    if (game != STRAIGHT_POOL) skill *= .75;
		    break;
		case SELF:
		case AUTONET:
		case AUTOLOCK:
		    ur_computer = TRUE;
		    im_computer = TRUE;
		    break;
	    }

	    if (game == NINE_BALL)
		num_balls = 10;
	    else
		num_balls = 16;

	    wins = 0; losses = 0;		/* no score yet */
	    my_score = 0; his_score = 0;	/* no balls sunk yet */

	    if (opponent == NETWORK)
		state = LAG_FOR_BREAK_SETUP;
	    else
		state = START_PLAY;
	    break;

	case LAG_FOR_BREAK_SETUP:
	    for (i=0; i<num_balls; i++)
		on_table[i] = FALSE;
	    px[0] = px[8] = left_edge + 5.*radius;
	    py[0] = bottom_edge + (top_edge-bottom_edge)/3.;
	    py[8] = bottom_edge + 2.*(top_edge-bottom_edge)/3.;
	    on_table[0] = on_table[8] = TRUE;
	    draw_table();

	    draw_centered_text2("LAG FOR BREAK",
				"YOU ARE WHITE");

	    if (num_screens > 1)
		draw_remote_text2("LAG FOR BREAK",
				  "YOU ARE BLACK");

	    state = LAG_FOR_BREAK;
	    delay(5);
	    return;

	case LAG_FOR_BREAK:
	    vy[0] = 0.;
	    vy[8] = 0.;

	    draw_centered_text("SET VELOCITY WITH MOUSE");
	    get_velocity(0);
	    if (num_screens > 1) {
	        draw_remote_text("SET VELOCITY WITH MOUSE");
	        get_velocity(1);
	    }
	    state = GOT_LAG_VELOCITY;
	    return;

	case GOT_LAG_VELOCITY:
	    hit_wall[0] = FALSE;
	    hit_wall[1] = FALSE;
	    state = DO_LAG;

	    clear_message_area();
	    scale = (float)velocity*velocity_scale;
	    vx[0] = scale + 3 - 6*random_number;	/* randomize a little */
	    vx[0] = max(0,vx[0]);
	    if (num_screens > 1) {
		clear_remote_area();
		scale = (float)remote_velocity*velocity_scale;
		vx[8] = scale + 3 - 6*random_number;	/* randomize a little */
		vx[8] = max(0,vx[8]);
	    } else {
		if (net_write(&vx[0], 4) <= 0) state = LOST_NET;
		start_remote_read(&vx[8], 4);
	    }
	    break;

	case DO_LAG:
	    if (vx[0] == 0. && vx[8] == 0.) {
		state = LAG_WINNER;
		delay(3);
		return;
	    } else {
		set_frame_timeout(time_between_frames);
		for (i=0; i<9; i+=8) {
		    if (vx[i] != 0.) {
			erase_ball(px[i],py[i]);
			px[i] += vx[i];
			if (px[i] > right_edge-radius) {
			    px[i] = right_edge-radius;
			    vx[i] *= -cushion_friction;
			    hit_wall[i/8] = TRUE;
			} else if (px[i] < left_edge+radius) {
			    px[i] = left_edge+radius;
			    vx[i] *= -cushion_friction;
			}
			if (absolute(vx[i]) < 4.)
			    if (absolute(vx[i]) < .5)
				vx[i] = 0.;
			    else
				vx[i] *= high_felt_friction;
			else
			    vx[i] *= felt_friction;
			draw_ball(i);
		    }
		}
		return;
	    }

	case LAG_WINNER:	/* determine who won the lag */
	    if (px[0] < px[8] && (hit_wall[0] ^ !hit_wall[1]) ||
		(hit_wall[0] && !hit_wall[1]))
		my_turn = TRUE;
	    else if (px[0] > px[8] && (hit_wall[0] ^ !hit_wall[1]) ||
		(!hit_wall[0] && hit_wall[1]))
		my_turn = FALSE;
	    if (num_screens > 1) system = (my_turn ? LOCAL : REMOTE);
	    state = START_PLAY;
	    break;

	case START_PLAY:

	    /* Setup ball positions and speeds */

	    balls_on_table = num_balls;	/* set count of balls on table */
	    first_shot = TRUE;		/* no shots taken yet */
	    last_shot[0] = FALSE;	/* neither player shooting the 8-ball */
	    last_shot[1] = FALSE;
	    leftx = left_edge;		/* leftmost position in ballrack */
	    rightx = right_edge;	/* rightmost position in ballrack */
	    target_ball = -1;		/* no target ball yet */
	    first_pos = right_edge;	/* assume 1st contact at right wall */
	    respot_cue = after_scratch = FALSE; /* not after a scratch */
	    humans_turn = (my_turn && !im_computer ||
			  !my_turn && !ur_computer && !remote_process);
	    remote_turn = (!my_turn && remote_process || num_screens > 1);
	    computers_turn = (my_turn && im_computer ||
			     !my_turn && (ur_computer && !remote_process));

	    for (i=0; i<num_balls; i++) {
		on_table[i] = TRUE;
		off_table[i] = FALSE;
		if (i == 0) {
		    px[i] = pxinit[i] = cue_x;
		    py[i] = pyinit[i] = cue_y;
		} else if (game == NINE_BALL) {
		    px[i] = pxinit[i] = (float)(spot_point + pxinit_offset*pxinit9[i]);
		    py[i] = pyinit[i] = (float)(y_center + (2*pyinit9[i] - pxinit9[i])*pyinit_offset);
		} else if (game == ROTATION) {
		    px[i] = pxinit[i] = (float)(spot_point + pxinit_offset*pxinit15[i]);
		    py[i] = pyinit[i] = (float)(y_center + (2*pyinit15[i] - pxinit15[i])*pyinit_offset);
		} else {
		    px[i] = pxinit[i] = (float)(spot_point + pxinit_offset*pxinit0[i]);
		    py[i] = pyinit[i] = (float)(y_center + pyinit_offset*(2*pyinit0[i] - pxinit0[i]));
		}
		vx[i] = 0.;		/* all balls stopped */
		vy[i] = 0.;
	    }

	    selection_made = FALSE;	/* no selection made for 8-ball */
	    action_started = TRUE;	/* action started (redraw ballrack) */
	    balls_numbered = TRUE;	/* when handling exposures, # balls */
	    draw_table();
	    display_score();

	    if (remote_turn)
		if (num_screens > 1)
		    draw_remote_text("OPPONENT'S TURN");
		else
		    draw_centered_text("OPPONENT'S TURN");

	    stop_count = 0;		/* set no balls stopped for now */
	    state = GET_INPUT;
	    break;

#define MAX_POSITIONS 5

	case SYNC_POSITIONS:
	    if (remote_process) {
		ball_index = -1;
		if (my_turn) {
		    buffer_index = 0;
		    state = SEND_POSITIONS;
		} else {
		    ball_count = 0;
		    for (i=0; i<num_balls; i++) if (on_table[i]) ball_count++;
		    state = RECEIVE_POSITIONS;
		}
	    } else
		state = BALLS_STOPPED;
	    break;

	case SEND_POSITIONS:
	    if (++ball_index == num_balls) {
		state = BALLS_STOPPED;
		if (buffer_index > 0) {
		    start_remote_write(buffer, buffer_index*4);
		    buffer_index = 0;
		}
	    } else if (on_table[ball_index]) {
		buffer[buffer_index++] = px[ball_index];
		buffer[buffer_index++] = py[ball_index];
		if (buffer_index == MAX_POSITIONS*2) {
		    start_remote_write(buffer, buffer_index*4);
		    buffer_index = 0;
		}
	    }
	    break;

	case RECEIVE_POSITIONS:
	    if (ball_count <= 0) {
		for (i=0; i<num_balls; i++)
		    if (on_table[i] && (psavex[i] != px[i] || psavey[i] != py[i])) {
			erase_ball(psavex[i],psavey[i]);
			draw_ball(i);
		    }
		state = BALLS_STOPPED;
	    } else {
		state = POSITION_RECEIVED;
		start_remote_read(buffer, 8*min(MAX_POSITIONS, ball_count));
		return;
	    }
	    break;

	case POSITION_RECEIVED:
	    i = 0;
	    while(ball_count > 0  && i < MAX_POSITIONS*2) {
		if (on_table[++ball_index]) {
		    px[ball_index] = buffer[i++];
		    py[ball_index] = buffer[i++];
		    ball_count--;
		}
	    }
	    state = RECEIVE_POSITIONS;
	    break;

	case BALLS_STOPPED:
	    if (game == EIGHT_BALL)
		selected_ball_hit_first =
			!selection_made && ball_hit_first != 8 ||
			last_shot[my_turn] && ball_hit_first == 8 ||
			shooting_low_balls &&
			(ball_hit_first >= 1 && ball_hit_first <= 7) ||
			shooting_high_balls &&
			(ball_hit_first >= 9 && ball_hit_first <= 15);
	    /*
	     * Check criteria for end of game.  If 8-ball and the 8 ball is off
	     * the table, or 9-ball and the 9 ball is off the table after a
	     * legal shot, or the table is cleared.
	     */
	    if (game == EIGHT_BALL && (!on_table[8] ||
		last_shot[my_turn] && !on_table[0]) ||
		game == NINE_BALL && !on_table[9] &&
		first_ball == ball_hit_first && on_table[0]) {
		bad = FALSE;
		if (game == EIGHT_BALL) {
		    if (!on_table[0])
			bad = TRUE;
		    else if (!first_shot) {
			if (!selected_ball_hit_first)
			    bad = TRUE;
			else if (last_shot[my_turn])
			    for (i=0; i<number_sunk; i++) {
				if (sunken_ball[i] == target_ball &&
				    sunken_pocket[i] != target_pocket) bad = TRUE;
			    }
			else if (!shooting_8ball())
			    bad = TRUE;
		    }
		}
		if (my_turn ^ bad) {
		    wins++;
		    my_turn = TRUE;
		    if (num_screens > 1) system = LOCAL;
		} else {
		    losses++;
		    my_turn = FALSE;
		    if (num_screens > 1) system = REMOTE;
		}
		if (bad)
		    state = GAME_LOST;
		else
		    state = GAME_OVER;
		break;
	    }

	    /* Another shot has been taken */

	    first_shot = FALSE;
	    respot_cue = FALSE;
	    number_balls();
	    for (i=0; i<num_balls; i++)	/* reset velocities to 0 */
		vx[i] = vy[i] = 0.;
	    /*
	     * Check for player change and for sunken balls that need to be
	     * respotted
	     */
	    last_turn = my_turn;
	    if (!on_table[0] ||
		game != NINE_BALL && game != ROTATION && after_scratch &&
		first_pos < break_point && !far_wall_hit)
		my_turn = !my_turn;
	    else if (game == EIGHT_BALL) {
		if (!selected_ball_hit_first)
		    my_turn = !my_turn;
		else if (selection_made) {
		    for (i=0; i<number_sunk; i++)
			if (shooting_high_balls) {
			    if (sunken_ball[i] > 8) break;
			} else {
			    if (sunken_ball[i] < 8) break;
			}
		    if (i == number_sunk) my_turn = !my_turn;
		} else if (number_sunk == 0)
		    my_turn = !my_turn;
	    } else if (game == NINE_BALL || game == ROTATION) {
		if (number_sunk == 0 || first_ball != ball_hit_first)
		    my_turn = !my_turn;
	    } else if (game == STRAIGHT_POOL) {
		if (number_sunk == 0 || target_ball != -1) {
		    for (i=0; i<number_sunk; i++) {
			if (sunken_ball[i] == target_ball &&
			    sunken_pocket[i] == target_pocket) break;
		    }
		    if (i == number_sunk) my_turn = !my_turn;
		}
	    }

	    state = CHECK_RESPOT;
	    if (last_turn != my_turn) {
		if (num_screens > 1) system = (my_turn ? LOCAL : REMOTE);
		humans_turn = (my_turn && !im_computer ||
			      !my_turn && !ur_computer && !remote_process);
		remote_turn = (!my_turn && remote_process || num_screens > 1);
		computers_turn = (my_turn && im_computer ||
				 !my_turn && (ur_computer && !remote_process));
		if (remote_turn)
		    state = OPPONENTS_TURN;
		display_score();
		draw_centered_text("PLAYER CHANGE");
		if (num_screens > 1)
		    draw_remote_text("PLAYER CHANGE");
		delay(1);
		return;
	    }
	    break;

	case OPPONENTS_TURN:
	    if (num_screens > 1)
		draw_remote_text("OPPONENT'S TURN");
	    else
		draw_centered_text("OPPONENT'S TURN");
	    state = CHECK_RESPOT;
	    break;

	case CHECK_RESPOT:

	    /* Check to see if any balls need to be put back on the table */

	    if (game == EIGHT_BALL && (!on_table[0] ||
		after_scratch && first_pos < break_point && !far_wall_hit ||
		!selected_ball_hit_first)) {
		if (!selection_made) {
		    for (i=0; i<number_sunk; i++)
			spot_ball(sunken_ball[i]);
		} else if (im_high ^ my_turn) {
		    respot_count = 0;
		    for (i=0; i<number_sunk; i++)
			if (sunken_ball[i] > 8) {
			    spot_ball(sunken_ball[i]);
			    respot_count++;
			}
		    if (respot_count == 0 && !on_table[0])
			for (i=9; i<num_balls; i++)
			    if (!on_table[i]) {
				spot_ball(i);
				break;
			    }
		} else {
		    respot_count = 0;
		    for (i=0; i<number_sunk; i++)
			if (sunken_ball[i] < 8 && sunken_ball[i] != 0) {
			    spot_ball(sunken_ball[i]);
			    respot_count++;
			}
		    if (respot_count == 0 && !on_table[0])
			for (i=1; i<8; i++)
			    if (!on_table[i]) {
				spot_ball(i);
				break;
			    }
		}
	   } else if (game == NINE_BALL || game == ROTATION) {
		if (!on_table[0] || first_ball != ball_hit_first)
		    for (i=0; i<number_sunk; i++)
			spot_ball(sunken_ball[i]);
		else if (game == ROTATION) {
		    if (my_turn) {
			for (i=0; i<number_sunk; i++)
			    my_score += sunken_ball[i];
			if (my_score >= 61) {
			    wins++;
			    state = GAME_OVER;
			    break;
			}
		    } else {
			for (i=0; i<number_sunk; i++)
			    his_score += sunken_ball[i];
			if (his_score >= 61) {
			    losses++;
			    state = GAME_OVER;
			    break;
			}
		    }
		    display_score();
		    if (balls_on_table == 1) {
			my_score = his_score = 0;
			draw_centered_text("THE GAME IS A DRAW");
			if (num_screens > 1)
			    draw_remote_text("THE GAME IS A DRAW");
			state = START_PLAY;
			delay(5);
			return;
		    }
		}
	   } else if (game == STRAIGHT_POOL) {
		bad = TRUE;
		if (target_ball > 0)
		    for (i=0; i<number_sunk; i++) {
			if (sunken_ball[i] == target_ball &&
			    sunken_pocket[i] == target_pocket) bad = FALSE;
		    }
		else if (number_sunk > 0)
		    bad = FALSE;
		if (!on_table[0]) bad = TRUE;
		if (bad)
		    for (i=0; i<number_sunk; i++)
			spot_ball(sunken_ball[i]);
		else if (my_turn) {
		    my_score += number_sunk;
		    if (my_score >= straight_limit) {
			wins++;
			state = GAME_OVER;
			break;
		    }
		} else {
		    his_score += number_sunk;
		    if (his_score >= straight_limit) {
			losses++;
			state = GAME_OVER;
			break;
		    }
		}
		if (balls_on_table <= 2 && on_table[0]) {
		    int last_ball = -1;
		    balls_on_table = num_balls;	/* set count of balls on table */
		    leftx = left_edge;		/* leftmost position in ballrack */
		    rightx = right_edge;	/* rightmost position in ballrack */
		    for (i=1; i<num_balls; i++) {
			int index = i;
			if (on_table[i]) {
			    last_ball = i;
			    if (i == 1) continue;
			    index = 1;
			}
			on_table[index] = TRUE;
			off_table[index] = FALSE;
			px[index] = pxinit[i];
			py[index] = pyinit[i];
		    }
		    for (i=1; i<num_balls; i++)
			if (square(pxinit[i]-px[0])+square(pyinit[i]-py[0])
			    < square(diameter)) {
			    respot_cue = TRUE;
			    px[0] = pxinit[0];
			    py[0] = pyinit[0];
			    break;
			}
		    if (last_ball > 0) {
			for (i=1; i<num_balls; i++)
			    if (square(pxinit[i]-px[last_ball])+
				square(pyinit[i]-py[last_ball])
				< square(diameter)) {
				px[last_ball] = pxinit[1];
				py[last_ball] = pyinit[1];
				break;
			    }
		    }
		    draw_table();
		}
		display_score();
	    }

	    /* high/low selection for eight ball */

	    if (game == EIGHT_BALL && !selection_made &&
		on_table[0] && number_sunk > 0 &&
		(!after_scratch || first_pos > break_point || far_wall_hit)) {
		state = HILO_SELECT;
	    } else
		state = GET_INPUT;
	    break;

	case HILO_SELECT:
	    im_high = FALSE;
	    if (humans_turn) {
		draw_centered_text("SELECT HIGH OR LOW BALLS");
		select_menu(hilo_strings);
		state = HILO_RESPONSE;
		return;
	    } else {
		state = HILO_DISPLAY;
		selection_made = TRUE;
		if (computers_turn) {
		    hicount = locount = 0;
		    for (i=0; i< number_sunk; i++)
			if (sunken_ball[i] < 8) locount++; else hicount++;
		    if (hicount > locount ||
			hicount == locount && random_number < .5)
			im_high = TRUE;
		    if (remote_process) {
			nbuffer[0] = !im_high;
			start_remote_write(nbuffer, 1);
		    }
		    break;
		} else {	/* remote_turn */
		    start_remote_read(&im_high, 1);
		    return;
		}
	    }

	case HILO_DISPLAY:
	    strcpy(string, (im_computer && ur_computer && !my_turn ? "MY OPPONENT WILL" : "I'LL"));
	    strcat(string, (shooting_high_balls ? " BE HIGH" : " BE LOW"));
	    draw_centered_text(string);
	    if (num_screens > 1) {
		strcpy(string, (im_computer && ur_computer && my_turn ? "MY OPPONENT WILL" : "I'LL"));
		strcat(string, (shooting_high_balls ? " BE HIGH" : " BE LOW"));
		draw_remote_text(string);
	    }
	    state = HILO_SET;
	    delay(3);
	    return;

	case HILO_RESPONSE:
	    if ((menu_selection == 1) ^ !my_turn) im_high = TRUE;
	    selection_made = TRUE;
	    state = HILO_SET;
	    if (num_screens > 1)
		draw_remote_text(shooting_high_balls ?
				 "I'LL BE HIGH" :
				 "I'LL BE LOW");
	    if (remote_process) {
		nbuffer[0] = !im_high;
		start_remote_write(nbuffer, 1);
	    }
	    break;

	case HILO_SET:
	    display_score();
	    if (remote_turn)
		if (num_screens > 1)
		    draw_remote_text("OPPONENT'S TURN");
		else
		    draw_centered_text("OPPONENT'S TURN");
	    state = GET_INPUT;
	    break;

	case GET_INPUT:		/* select next shot */
	    after_scratch = !on_table[0];
	    respot_cue = respot_cue || after_scratch ||
			(game == NINE_BALL || game == ROTATION) &&
			first_ball != ball_hit_first;
	    first_pos = right_edge;	/* assume 1st contact at right wall */

	    /* get lowest ball */
	    for (i=1; i<num_balls; i++)
		if (on_table[i]) {first_ball = i; break;}

	    /* are we shooting the 8-ball ? */
	    if (game == EIGHT_BALL)
		last_shot[my_turn] = shooting_8ball();

	    if (respot_cue) {
		if (game == STRAIGHT_POOL) {
		    j = rightmost_target(1, 15);
		    if (px[j] < break_point)
			spot_ball(j);
		}
/*
		else if (game == EIGHT_BALL && selection_made) {
		    if (last_shot[my_turn])
			j = 8;
		    else if (my_turn ^ im_high)
			j = rightmost_target(1, 7);
		    else
			j = rightmost_target(9, 15);
		    if (px[j] < break_point)
			spot_ball(j);
		}
*/
	    }

	    if (humans_turn)
		state = PLACE_BALL;
	    else if (computers_turn)
		state = COMPUTER_SPOT_CUE;
	    else
		state = READ_POSITION;
	    break;

	case COMPUTER_SPOT_CUE:
	    state = COMPUTER_DETERMINE_SHOT;
	    if (respot_cue || first_shot)
		spot_cue();
	    if (remote_process) {
		buffer[0] = px[0];
		buffer[1] = py[0];
		start_remote_write(buffer, 8);
	    }
	    break;

	case COMPUTER_DETERMINE_SHOT:
	    determine_shot();
	    display_call(my_turn || num_screens == 1 ? LOCAL : REMOTE);
	    state = COMPUTER_REPORT_SHOT;
	    if (game == STRAIGHT_POOL && !first_shot ||
		game == EIGHT_BALL && last_shot[my_turn]) {
		if (remote_process) {
		    nbuffer[0] = target_ball;	/* calling shot */
		    nbuffer[1] = target_pocket;
		    start_remote_write(nbuffer, 8);
		} else if (num_screens > 1)
		    display_call(my_turn ? REMOTE : LOCAL);
	    }
	    break;

	case COMPUTER_REPORT_SHOT:
	    state = COMPUTER_CONFIRM_SHOT;
	    if (remote_process) {
		buffer[0] = vx[0];
		buffer[1] = vy[0];
		buffer[2] = roll_x;
		buffer[3] = roll_y;
		buffer[4] = spin_factor;
		start_remote_write(buffer, 20);
	    }
	   break;

	case COMPUTER_CONFIRM_SHOT:
	    state = COMPUTER_WAIT_BEFORE_SHOT;
	    if (remote_process) {
		nbuffer[0] = TRUE;
		start_remote_write(nbuffer, 1);
	    }
	    break;

	case COMPUTER_WAIT_BEFORE_SHOT:
	    state = FRAME_SETUP;
	    if (net_game) {
		delay(2);
		return;
	    }
	    break;

	case READ_POSITION:
	    state = POSITION_READ;
	    start_remote_read(buffer, 8);
	    return;

	case POSITION_READ:
	    x1 = buffer[0];
	    y1 = buffer[1];
	    replace_ball(x1, y1, 0);
	    if (game == STRAIGHT_POOL && !first_shot ||
		game == EIGHT_BALL && last_shot[my_turn]) {
		state = DISPLAY_CALL;
		start_remote_read(nbuffer, 8);
		return;
	    } else {
		state = READ_VELOCITY;
		break;
	    }

	case DISPLAY_CALL:
	    target_ball = nbuffer[0];
	    target_pocket = nbuffer[1];
	    display_call(LOCAL);
	    state = READ_VELOCITY;
	    break;

	case READ_VELOCITY:
	    state = VELOCITY_READ;
	    start_remote_read(buffer, 20);
	    return;

	case VELOCITY_READ:
	    vx[0] = buffer[0];
	    vy[0] = buffer[1];
	    roll_x = buffer[2];
	    roll_y = buffer[3];
	    spin_factor = buffer[4];
	    state = CONFIRM_VELOCITY;
	    start_remote_read(&confirmed, 1);
	    return;

	case CONFIRM_VELOCITY:
	    if (confirmed)
		state = FRAME_SETUP;
	    else
		state = READ_VELOCITY;
	    break;

	case PLACE_BALL:
	    if (!respot_cue && !first_shot) {
		state = REPORT_POSITION;
		break;
	    } else {
	        draw_centered_text2("POSITION CUE BALL WITH MOUSE",
				    "THEN PRESS LEFT BUTTON");
		enable_button(first_shot || game == STRAIGHT_POOL ||
			game == EIGHT_BALL ? HEAD : FELT);
		state = GET_CUE_POSITION;
		return;
	    }

	case GET_CUE_POSITION:
	    for (i=1; i<num_balls; i++) {
		if (on_table[i] && overlap(i)) {
		    draw_centered_text2("POSITION OVERLAPS ANOTHER BALL",
					"TRY AGAIN");
		    return;
		}
	    }
	    replace_ball((float)pointer_x, (float)pointer_y, 0);
	    clear_message_area();
	    state = REPORT_POSITION;
	    break;

	case REPORT_POSITION:
	    state = CALL_SHOT;
	    if (remote_process) {
		buffer[0] = px[0];
		buffer[1] = py[0];
		start_remote_write(buffer, 8);
	    }
	    break;

	case CALL_SHOT:
	    if (game == EIGHT_BALL && last_shot[my_turn]) {
		pointer_x = (int)px[8];
		pointer_y = (int)py[8];
		state = GET_TARGET_BALL;
		break;
	    } else if (game != STRAIGHT_POOL || first_shot) {
		position = 0;
		state = SET_DIRECTION;
		break;
	    } else {
		draw_centered_text2("CALL TARGET BALL WITH MOUSE",
				    "THEN PRESS LEFT BUTTON");
		enable_button(FELT);
		state = GET_TARGET_BALL;
		return;
	    }

	case GET_TARGET_BALL:
	    for (i=1; i<num_balls; i++)
		if (on_table[i] && overlap(i)) {
		    target_ball = i;
		    draw_centered_text2("CALL TARGET POCKET WITH MOUSE",
					"THEN PRESS LEFT BUTTON");
		    enable_button(TABLE);
		    state = GET_TARGET_POCKET;
		    return;
		}
	    return;

	case GET_TARGET_POCKET:
	    target_pocket = -1;
	    for (i=0; i<3; i++) {
		if (square(pointer_x-(int)pocket[i]) + square(pointer_y-top_edge) <
		    square(2*diameter)) {
		    target_pocket = i;
		    break;
		} else if (square(pointer_x-(int)pocket[i]) + square(pointer_y-bottom_edge) <
		    square(2*diameter)) {
		    target_pocket = 3+i;
		    break;
		}
	    }
	    if (target_pocket != -1) {
		state = WAIT_BEFORE_SHOT;
		disable_button();
		display_call(LOCAL);
		if (num_screens > 1)
		    display_call(REMOTE);
		if (remote_process) {
		    nbuffer[0] = target_ball;
		    nbuffer[1] = target_pocket;
		    start_remote_write(nbuffer, 8);
		}
		break;
	    }
	    return;

	case WAIT_BEFORE_SHOT:
	    position = 0;
	    enable_button(im_computer || net_game ? TABLE : TABLE | RESPOT);
	    state = SET_DIRECTION;
	    delay(remote_turn ? 5 : 2);
	    return;

	case SET_DIRECTION:
	    if (position != 0) {
		state = GOT_POSITION;
		break;
	    }
	    draw_centered_text2("SET TARGET WITH MOUSE",
				"THEN PRESS LEFT BUTTON");
	    enable_button(im_computer || net_game ? TABLE : TABLE | RESPOT);
	    state = GOT_POSITION;
	    return;

	case GOT_POSITION:
	    clear_message_area();
	    if (position == OVER_RESPOT)
		state = DO_RESPOT;
	    else {
		disable_button();
		vx[0] = (float)pointer_x - px[0];
		vy[0] = (float)pointer_y - py[0];
		state = SET_VELOCITY;
	    }
	    break;

	case DO_RESPOT:
	    draw_centered_text2("SELECT BALL TO SPOT",
				"THEN PRESS LEFT BUTTON");
	    enable_button(FELT | BALLRACK);
	    state = CHECK_RESPOT_BALL;
	    return;

        case CHECK_RESPOT_BALL:
	    for (i=0; i<num_balls; i++) {
		if (overlap(i)) {
		    if (position == OVER_BALLRACK) {
			spot_ball(i);
			display_score();
			position = 0;
			state = SET_DIRECTION;
			break;
		    } else {
			draw_centered_text2("POSITION BALL WITH MOUSE",
					    "THEN PRESS LEFT BUTTON");
			enable_button(FELT);
			ball_selected = i;
			state = CHECK_RESPOT_OVERLAP;
			return;
		    }
		}
	    }
	    return;

	case CHECK_RESPOT_OVERLAP:
	    for (i=0; i<num_balls; i++)
		if (on_table[i] && i != ball_selected && overlap(i)) {
		    draw_centered_text2("POSITION OVERLAPS ANOTHER BALL",
					"TRY AGAIN");
		    return;
		    }
	    replace_ball((float)pointer_x, (float)pointer_y, ball_selected);
	    position = 0;
	    state = SET_DIRECTION;
	    break;

	case SET_VELOCITY:
	    if (preset_velocity[system]) {
		roll = 50;
		spin = 50;
		velocity = (first_shot ? 100 : 50);
		state = GOT_VELOCITY;
		break;
	    } else {
		draw_centered_text2("SET VELOCITY AND SPIN",
				    "WITH MOUSE");
		get_velocity(0);
		state = GOT_VELOCITY;
		return;
	    }

	case GOT_VELOCITY:
	    clear_message_area();
	    vsize = magnitude(vx[0],vy[0]);
	    if (vsize == 0.)
		vsize = vx[0] = vy[0] = 1.;
	    roll_x = vx[0]*(float)(roll-50)/(40*vsize);
	    roll_y = vy[0]*(float)(roll-50)/(40*vsize);
	    if (roll > 0) {
		roll_x *= (first_shot ? .1 : .5);
		roll_y *= (first_shot ? .1 : .5);
	    }
	    spin_factor = (float)(velocity*(50-spin))/2000.;
	    scale = (float)velocity*velocity_scale;
	    vx[0] *= scale/vsize;
	    vy[0] *= scale/vsize;

	    state = CONFIRM_SHOT;
	    if (remote_process) {
		buffer[0] = vx[0];
		buffer[1] = vy[0];
		buffer[2] = roll_x;
		buffer[3] = roll_y;
		buffer[4] = spin_factor;
		start_remote_write(buffer, 20);
	    }
	    break;

	case CONFIRM_SHOT:
	    if (confirm_shots[system]) {
		draw_arrow();
		draw_centered_text("CONFIRM SHOT");
		select_menu(confirm_strings);
		state = CONFIRM_RESPONSE;
		return;
	    } else {
		state = FRAME_SETUP;
		if (remote_process) {
		    nbuffer[0] = TRUE;
		    start_remote_write(nbuffer, 1);
		}
		break;
	    }

	case CONFIRM_RESPONSE:
	    clear_message_area();
	    erase_arrow();
	    if (menu_selection == 2) {
		position = 0;
		state = SET_DIRECTION;
	    } else
		state = FRAME_SETUP;

	    if (remote_process) {
		nbuffer[0] = (menu_selection == 1);
		start_remote_write(nbuffer, 1);
	    }
	    break;

	case FRAME_SETUP:
	    any_contact = FALSE;		/* no contact yet */
	    far_wall_hit = FALSE;		/* far wall hasn't been hit */
	    ball_hit_first = 0;			/* nothing hit yet */
	    number_sunk = 0;			/* no balls sunk */
	    stop_count = 0;			/* reset stop count */
	    unnumber_balls();
	    state = MOVE_BALLS;
	    break;

	case MOVE_BALLS:
	    if (stop_count == balls_on_table) {
		state = SYNC_POSITIONS;
		break;
	    } else {
		set_frame_timeout(time_between_frames);
		stop_count = 0;
		for (i=0; i<num_balls; i++) {
		    if (!off_table[i]) {
			if (!on_table[i]) {
			    off_table[i] = TRUE;
			    balls_on_table--;
			    erase_ball(psavex[i],psavey[i]);
			    add_to_ballrack(i);
			    continue;
			}

			/* Calculate new position of the ball */
	
			psavex[i] = px[i];		/* old position */
			psavey[i] = py[i];
			px[i] += vx[i];			/* new position */
			py[i] += vy[i];
			vsavex[i] = vx[i];		/* save velocity */
			vsavey[i] = vy[i];
			tsave[i] = 1.;			/* assume full movement */

			/* Determine if the ball came in contact with any border */

			if (px[i] >= right_edge-radius) {	/* right wall ? */
			    vx[i] *= -cushion_friction;
			    vy[i] *= cushion_friction;
			    if (!any_contact) far_wall_hit = TRUE;
			    if (vsavex[i] != 0) {
				tsave[i] = (right_edge - radius - psavex[i])/vsavex[i];
				py[i] = vsavey[i]*tsave[i] + psavey[i];
				tsave[i] += backoff_t;
			    }
			    px[i] = right_edge - radius;

			    /* Did the ball go in a pocket on the right wall ?? */

			    if (absolute(px[i]-pocket[2]) <= dist[2] &&
				(absolute(py[i]-top_edge) <= dist[2] ||
				absolute(py[i]-bottom_edge) <= dist[2])) {
				sunken_ball[number_sunk] = i;
				if (absolute(py[i]-top_edge) <= dist[2])
				    sunken_pocket[number_sunk] = 2;
				else
				    sunken_pocket[number_sunk] = 5;
				number_sunk++;
				on_table[i] = FALSE;
				continue;
			    }
			    if (i == 0 && spin_factor != 0) {
				vy[0] -= 15*spin_factor;
				spin_factor *= .1;
			    }
			} else if (px[i] <= left_edge+radius) {	/* left wall ? */
			    vx[i] *= -cushion_friction;
			    vy[i] *= cushion_friction;
			    if (vsavex[i] != 0) {
				tsave[i] = (left_edge + radius - psavex[i])/vsavex[i];
				py[i] = vsavey[i]*tsave[i] + psavey[i];
				tsave[i] += backoff_t;
			    }
			    px[i] = left_edge + radius;

			    /* Did the ball go in a pocket on the left wall ?? */

			    if (absolute(px[i]-pocket[0]) <= dist[0] &&
				(absolute(py[i]-top_edge) <= dist[0] ||
				absolute(py[i]-bottom_edge) <= dist[0])) {
				sunken_ball[number_sunk] = i;
				if (absolute(py[i]-top_edge) <= dist[0])
				    sunken_pocket[number_sunk] = 0;
				else
				    sunken_pocket[number_sunk] = 3;
				number_sunk++;
				on_table[i] = FALSE;
				continue;
			    }
			    if (i == 0 && spin_factor != 0) {
				vy[0] += 15*spin_factor;
				spin_factor *= .1;
			    }
			}

			if (py[i] >= top_edge-radius) {		/* top wall ? */
			    vx[i] *= cushion_friction;
			    vy[i] *= -cushion_friction;
			    if (vsavey[i] != 0) {
				tsave[i] = (top_edge - radius - psavey[i])/vsavey[i];
				px[i] = vsavex[i]*tsave[i] + psavex[i];
				tsave[i] += backoff_t;
			    }
			    py[i] = top_edge - radius;

			    /* Did the ball go in a pocket on the top wall ?? */

			    for (j=0; j<3; j++) {
				if (absolute(px[i]-pocket[j]) <= dist[j]) {
				    vsize = magnitude(vx[i],vy[i]);
				    if (j != 1 || vsize == 0. ||
					(vsize <= 5. && absolute(vy[i]/vsize) > .07) ||
					absolute(vy[i]/vsize) > .2) {
					sunken_ball[number_sunk] = i;
					sunken_pocket[number_sunk] = j;
					number_sunk++;
					on_table[i] = FALSE;
					break;
				    }
				}
			    }
			    if (j < 3) continue;
			    if (i == 0 && spin_factor != 0) {
				vx[0] += 15*spin_factor;
				spin_factor *= .1;
			    }
			} else if (py[i] <= bottom_edge+radius) { /* bottom wall ? */
			    vx[i] *= cushion_friction;
			    vy[i] *= -cushion_friction;
			    if (vsavey[i] != 0) {
				tsave[i] = (bottom_edge + radius - psavey[i])/vsavey[i];
				px[i] = vsavex[i]*tsave[i] + psavex[i];
				tsave[i] += backoff_t;
			    }
			    py[i] = bottom_edge + radius;

			    /* Did the ball go in a pocket on the bottom wall ?? */

			    for (j=0; j<3; j++) {
				if (absolute(px[i]-pocket[j]) <= dist[j]) {
				    vsize = magnitude(vx[i],vy[i]);
				    if (j != 1 || vsize == 0. ||
					(vsize <= 5. && absolute(vy[i]/vsize) > .07) ||
					absolute(vy[i]/vsize) > .2) {
					sunken_ball[number_sunk] = i;
					sunken_pocket[number_sunk] = j + 3;
					number_sunk++;
					on_table[i] = FALSE;
					break;
				    }
				}
			    }
			    if (j < 3) continue;
			    if (i == 0 && spin_factor != 0) {
				vx[0] -= 15*spin_factor;
				spin_factor *= .1;
			    }
			}

			/* factor in backspin and sidespin */

			if (i == 0) {
			    if (tsave[0] < 1.) {   /* the cue ball hit a wall */
				if (roll_x != 0. || roll_y != 0.) {
				    vx[0] += roll_x*5;
				    vy[0] += roll_y*5;
				    roll_x *= .25;
				    roll_y *= .25;
				}
			    } else {
				if (absolute(spin_factor) > .05)
				    spin_factor *= .98;
				else
				    spin_factor = 0.;
				vx[0] += roll_x;
				vy[0] += roll_y;
				if (absolute(roll_x)+absolute(roll_y) > .1) {
				    roll_x *= .97;
				    roll_y *= .97;
				} else
				    roll_x = roll_y = 0.;
			    }
			}

			/* Reduce velocity due to friction */

			if (absolute(vx[i])+absolute(vy[i]) < 4.) {
			    if (absolute(vx[i])+absolute(vy[i]) < .5) {
				if (i != 0 || roll_x == 0. && roll_y == 0.) {
				    stop_count++;
				    vx[i] = vy[i] = 0.;
				}
			    } else {
				vx[i] *= high_felt_friction;
				vy[i] *= high_felt_friction;
			    }
			} else {
			    vx[i] *= felt_friction;
			    vy[i] *= felt_friction;
			}
		    }	/* !off_table[i] */
		}	/* for i=1 to num_balls */

		/*
		 * Check for collisions
		 *
		 * This section of code relies on the fact the projection of
		 * one vector (the current velocity vector) onto another
		 * (the angle of incidence) is given by the formula:
		 *
		 * 			(a.b)    b
		 * 	projection =    ----- x ---
		 * 			 |b|    |b|
		 *
		 * All of the momentum of one ball in the direction of the
		 * angle of incidence is transferred to the other ball and
		 * vice versa.
		 */
		for (i=0; i<num_balls-1; i++)
		for (j=i+1; j<num_balls; j++) {
		    if (on_table[i] && on_table[j]) {
			/*
			 * Since the balls move more than one pixel per frame, their
			 * current position is not necessarily their initial point of
			 * contact.  To calculate the initial point of contact we must
			 * parameterize the motion of the two balls in contact:
			 *
			 * 			x = dx*t + x0
			 * 			y = dy*t + y0
			 *
			 * The distance between the two balls is given by:
			 *
			 * 		    2		 2	      2
			 * 		dist  = (x2 - x1)  + (y2 - y1)
			 *
			 * Using the parametric equations you can calculate the first
			 * point at which the balls meet by setting 'dist' to the
			 * diameter of one ball, substituting the parameterized
 			 * equations into the distance formula and solving for 't'.
			 */
			x1 = psavex[i];
			y1 = psavey[i];
			x2 = psavex[j];
			y2 = psavey[j];
			dpx = vsavex[i];
			dpy = vsavey[i];
			dpx1 = vsavex[j];
			dpy1 = vsavey[j];
			a = square(dpx1-dpx) + square(dpy1-dpy);
			if (a == 0) continue;		/* both not moving??? */
			c = square(x2-x1) + square(y2-y1) - square(diameter);
			if (c > 10000.) continue;	/* too far away to touch */
			b = 2*((dpx1-dpx)*(x2-x1)+(dpy1-dpy)*(y2-y1));
			/*
			 * When we solve for t in the above equation, we get the formula:
			 *
			 * 		a*t**2 + b*t + c = 0
			 *
			 * By using the quadratic formula, we can solve for t.  There are
			 * some special cases.
			 *
			 * 	-- If a=0, the velocity of the 2 balls are exactly
			 * 	   the same in which case no collision should occur.
			 * 	-- If b**2-4ac < 0, the solution is imaginary in which
			 * 	   case the balls would never touch given their current
			 * 	   directions.
			 * 	-- If t < 0, the balls were already touching before
			 *	   this frame.
			 * 	-- If t > 1, the balls will not touch until after this
			 *	   frame.
			 */
			d = square(b) - 4*a*c;
			if (d < 0) continue;
			t = .5*(-b - sqroot(d))/a;
			if (t < 0. || t >= 1.) continue;

			if (!any_contact) {	/* record 1st ball hit */
			    ball_hit_first = j;
			    any_contact = TRUE;
			    first_pos = (int)px[j];
			}
			x1 += dpx*t;
			y1 += dpy*t;
			x2 += dpx1*t;
			y2 += dpy1*t;
			/*
			 * Reset the position of both balls to the initial
			 * point of contact unless they have already been
			 * reset to an earlier position.
			 */
			if (t < tsave[i]) tsave[i] = t;
			if (t < tsave[j]) tsave[j] = t;

			ax = vsavex[i];
			ay = vsavey[i];
			bx = x2 - x1;
			by = y2 - y1;
			b_size_2 = square(bx) + square(by);
			if (b_size_2 == 0) continue;
			a_dot_b = ax*bx + ay*by;
			vx1 = a_dot_b*bx/b_size_2;
			vy1 = a_dot_b*by/b_size_2;
			ax = vsavex[j];
			ay = vsavey[j];
			a_dot_b = ax*bx + ay*by;
			vx2 = a_dot_b*bx/b_size_2;
			vy2 = a_dot_b*by/b_size_2;

			/* Reset velocities after collision */

			vxtrans = vx2 - vx1;
			vytrans = vy2 - vy1;
			vx[i] += vxtrans;
			vy[i] += vytrans;
			vx[j] -= vxtrans;
			vy[j] -= vytrans;
			vx[i] *= contact_friction;
			vy[i] *= contact_friction;
			vx[j] *= contact_friction;
			vy[j] *= contact_friction;

			/* factor in spin */

			if (i == 0) {
			    if ((roll_x != 0. || roll_y != 0.) &&
				(vx[0] != 0. || vy[0] != 0.)) {
				float scale = max(1.,
					(square(vxtrans)+square(vytrans))/
					(square(vsavex[0])+square(vsavey[0])));
				vx[0] += roll_x*10*scale;
				vy[0] += roll_y*10*scale;
				roll_x *= (1.-.6*scale);
				roll_y *= (1.-.6*scale);
			    }
			    if (spin_factor != 0) {
				dpx = py[j] - py[0];
				dpy = px[0] - px[j];
				vsize = magnitude(dpx,dpy);
				if (vsize > 0) {
				    vx[0] += dpx*7*spin_factor/vsize;
				    vy[0] += dpy*7*spin_factor/vsize;
				    spin_factor *= .3;
				}
			    }
			}

			/* Make a little noise for the collision ????? */

		    }	/* if (on_table[i] && on_table[j]) */
		}	/* for (i...) & for (j...) */
		/*
		 * Now, back up any balls that collided this frame to
		 * their collision points.
		 */
		for (i=0; i<num_balls; i++)
		    if (on_table[i] && tsave[i] < 1.) {
			t = max(0., tsave[i] - backoff_t);
			px[i] = psavex[i] + vsavex[i]*t;
			py[i] = psavey[i] + vsavey[i]*t;
		    }
		/*
		 * Check for any overlapping balls.  If there are, back both
		 * balls up to the earliest time of any collision for the two.
		 */
		for (i=0; i<num_balls-1; i++)
		    if (on_table[i] && (vx[i] != 0. || vy[i] != 0))
			for (j=i+1; j<num_balls; j++)
			    if (on_table[j] && (vx[j] != 0. || vy[j] != 0.) &&
				square(px[i]-px[j])+square(py[i]-py[j]) <
				square(diameter)) {
				int index1, index2;
				if (tsave[i] == tsave[j]) continue;
				if (tsave[i] < tsave[j]) {
				    index1 = i; index2 = j;
				} else {
				    index1 = j; index2 = i;
				}
				t = max(0., tsave[index1] - backoff_t);
				px[index2] = psavex[index2] + vsavex[index2]*t;
				py[index2] = psavey[index2] + vsavey[index2]*t;
			    }

		/* Redraw those balls that have moved */

		for (i=0; i<num_balls; i++)
		    if (on_table[i] && (psavex[i] != px[i] || psavey[i] != py[i])) {
			erase_ball(psavex[i],psavey[i]);
			draw_ball(i);
		    }
		return;
	    }

	case DELAY_OVER:
	    cancel_delay_timeout();
	    state = next_state;
	    break;

	case CHECK_NET_READ:
	    if (!net_read_complete(&bytes_read)) {
		set_delay_timeout(1);		/* check back in a bit */
		return;
	    }
	    if (bytes_read <= 0)
		state = LOST_NET;
	    else
		state = next_state;
	    break;

	case CHECK_LOCK_READ:
	    status = lock_read(lock_buffer, lock_size);
	    if (status == 0) {
		set_delay_timeout(1);		/* check back in a bit */
		return;
	    }
	    if (status < 0)
		state = LOST_LOCK;
	    else
		state = next_state;
	    break;

	case CHECK_LOCK_WRITE:
	    status = lock_write(lock_buffer, lock_size);
	    if (status == 0) {
		set_delay_timeout(1);		/* check back in a bit */
		return;
	    }
	    if (status < 0)
		state = LOST_LOCK;
	    else
		state = next_state;
	    break;

	case WAIT_FOR_OPPONENT:
	    if (lock_grabbed()) {
		state = SEND_NAME;
		nbuffer[0] = game;
		nbuffer[1] = straight_limit;
		nbuffer[2] = opponent;
		start_remote_write(nbuffer, 12);
		break;
	    } else {
		set_delay_timeout(1);
		return;
	    }

	case GAME_LOST:
	    sprintf(string, "SORRY %s, YOU LOSE", (my_turn^im_remote ? player_name[1] : player_name[0]));
	    draw_centered_text(string);
	    if (num_screens > 1) draw_remote_text(string);
	    state = GAME_OVER;
	    delay(5);
	    return;

	case GAME_OVER:
	    display_score();
	    my_score = 0; his_score = 0;	/* reset balls sunk count */
	    sprintf(string, "CONGRATULATIONS %s", (my_turn^im_remote ? player_name[0] : player_name[1]));
	    draw_centered_text(string);
	    if (num_screens > 1) draw_remote_text(string);
	    state = START_PLAY;
	    delay(5);
	    return;

	case BAD_NODE:
	    draw_centered_text("UNABLE TO ACCESS TARGET DISPLAY");
	    state = END_GAME;
	    delay(6);
	    return;

	case LOST_NET:
	    draw_centered_text2("NETWORK CONNECTION LOST",
				"GAME OVER");
	    state = END_GAME;
	    delay(6);
	    return;

	case LOST_LOCK:
	    draw_centered_text2("ERROR ACCESSING CLUSTER LOCK",
				"GAME OVER");
	    state = END_GAME;
	    delay(6);
	    return;

	case END_GAME:
	    erase_arrow();
	    disable_button();
	    remove_menus();
	    cancel_delay_timeout();
	    cancel_frame_timeout();
	    if (net_game)
		if (num_screens > 1) {
		    destroy_remote_screen();
		    system = LOCAL;
		} else
		    net_close();
	    else if (lock_game)
		lock_close(im_remote);
	    if (net_game && im_remote || abort_flag == ABORT_PROGRAM)
		exit(1);
	    else {
		state = RESTART_GAME;
		if (abort_flag == ABORT_REMOTE) {
		    draw_centered_text("YOUR OPPONENT HAS WITHDRAWN");
		    abort_flag = 0;
		    delay(6);
		    return;
		} else {
		    abort_flag = 0;
		    break;
		}
	    }

	case RESTART_GAME:
	    action_started = FALSE;
	    balls_numbered = FALSE;
	    num_balls = 0;
	    draw_table();
	    draw_centered_text2("*** GAME OVER ***",
				"USE MENU BAR TO RESTART");
	    enable_select_options();
	    state = GAME_INIT;
	    return;
    }			/* switch (state) */
    }			/* for (;;) loop */
}

static int shooting_8ball()
{
    int i, start;

    if (!selection_made) return(FALSE);
    start = 9;
    if (my_turn ^ im_high) start = 1;
	for (i=start; i<start+7; i++)
	    if (on_table[i]) return(FALSE);
    return(TRUE);
}

static int rightmost_target(start, end)
{
    int i;
    int rightmost = -1;

    for (i=start; i<=end; i++)
	if (on_table[i])
	    if (rightmost == -1 || px[i] > px[rightmost]) rightmost = i;
    return(rightmost);
}

void number_balls()
{
    int i;

    for (i=1; i<num_balls; i++)
	if (on_table[i]) number_ball(i);

    balls_numbered = TRUE;
}

void unnumber_balls()
{
    int i;

    for (i=1; i<num_balls; i++)
	if (on_table[i]) draw_ball(i);

    balls_numbered = FALSE;
}

void draw_numbered_ball(index)
{
    draw_ball(index);
    if (index != 0) number_ball(index);
}

void replace_ball(x,y,index)
float x, y;
{
    if (on_table[index]) {
	erase_ball(px[index],py[index]);
	px[index] = x;
	py[index] = y;
    } else {
	px[index] = x;
	py[index] = y;
	balls_on_table++;
	on_table[index] = TRUE;
	off_table[index] = FALSE;
	redraw_ballrack();
    }
    draw_numbered_ball(index);
}

void spot_ball(index)
{
float x0,y0,t1,t2,earliest_t,value;
unsigned char free_space;
int i;

    if (index == 0) return;		/*  don't spot the cue ball */
    x0 = spot_point;
    y0 = (top_edge + bottom_edge)/2;
    earliest_t = 0.;
    free_space = FALSE;
    while (!free_space) {
	free_space = TRUE;
	for (i=0; i<num_balls; i++)
	    if (on_table[i]) {
		value = square(diameter) - square(py[i] - y0);
		if (value > 0) {
		    t1 = (px[i] - x0) - sqroot(value);
		    t2 = (px[i] - x0) + sqroot(value);
		    if (t1 < earliest_t && t2 > earliest_t) {
			earliest_t = t2;
			free_space = FALSE;
		    }
		}
	    }
	if (earliest_t > spot_offset-radius) {
	    y0 += radius;
	    earliest_t = 0.;
	    free_space = FALSE;
	}
    }
    if (on_table[index])
	erase_ball(px[index],py[index]);
    else {
	on_table[index] = TRUE;
	off_table[index] = FALSE;
	balls_on_table++;
	redraw_ballrack();
    }
    px[index] = earliest_t + x0;
    py[index] = y0;
    draw_numbered_ball(index);
}

void spot_cue()
{
    float best_shot, maxmag, save_x, save_y, old_x, old_y;
    float width, height, left, bottom;
    int tries;
    unsigned char overlaps;
    int i, j;

    if (on_table[0]) {
	old_x = px[0];
	old_y = py[0];
    }
    best_shot = -10000.;
    if ((game == NINE_BALL || game == ROTATION) && !first_shot) {
	left = max(left_edge+radius+1, px[first_ball]-125);
	bottom = max(bottom_edge+radius+1, py[first_ball]-125);
	width = min(right_edge-px[first_ball]-radius-2, 250);
	height = min(top_edge-py[first_ball]-radius-2, 250);
	tries = 10;
    } else {
	left = left_edge + radius + 1;
	bottom = bottom_edge + radius + 1;
	width = break_point - left_edge - radius - 2;
	height = top_edge - bottom_edge - diameter - 2;
	tries = 2;
	if (game == EIGHT_BALL) tries = 5;
    }
    if (first_shot) {
	px[0] = left + width*random_number;
	py[0] = bottom + height*random_number;
    } else {
	for (i=0; i<tries; i++) {
	    do {
		overlaps = FALSE;
		px[0] = left + width*random_number;
		py[0] = bottom + height*random_number;
		for (j=1; j<num_balls; j++)
		    if (on_table[j] && square(px[0]-px[j]) +
			square(py[0]-py[j]) < square(diameter)) {
			overlaps = TRUE;
			break;
		    }
	    } while (overlaps);

	    maxmag = determine_shot();
	    if (maxmag > best_shot) {
		best_shot = maxmag;
		save_x = px[0];
		save_y = py[0];
	    }
	}
	px[0] = save_x;
	py[0] = save_y;
    }
    if (!on_table[0]) {
	balls_on_table++;
	on_table[0] = TRUE;
	off_table[0] = FALSE;
	redraw_ballrack();
	display_score();
    } else
	erase_ball(old_x,old_y);

    draw_ball(0);
}

static void get_game_string(string)
char *string;
{
    strcpy(string, "THE GAME IS ");
    switch (game) {
	case STRAIGHT_POOL:
	    sprintf(string, "%sSTRAIGHT POOL,  LIMIT %d",
		    string, straight_limit);
	    break;
	case NINE_BALL:
	    strcat(string, "NINE BALL");
	    break;
	case EIGHT_BALL:
	    strcat(string, "EIGHT BALL");
	    break;
	case ROTATION:
	    strcat(string, "ROTATION");
	    break;
    }
}

static void display_call(where)
{
    char string[35];

    if (target_ball == -1 || target_pocket == -1) return;

    sprintf(string, "%d BALL IN THE %s POCKET", target_ball,
	(target_pocket == 1 || target_pocket == 4) ? "SIDE" : "CORNER");
    if (where == LOCAL)
	draw_centered_text(string);
    else
	draw_remote_text(string);
}

static float determine_shot()
{
    float dpx, dpy, dpx1, dpy1, pfx, pfy;
    float psize, psize1, maxmag, dot, newmax, vsize, newvx, newvy, scale;
    float ax, ay, bx, by, amag2, bmag2, adotb, vx1, vy1;
    float newpfx, newpfy, newdpx, newdpy, savemax, newsize, savepfx, savepfy;
    float savedpx, savedpy, savesize, newpos1, newpos2, t, amag, bmag;
    float newvx1, newvy1, bad_value;
    static unsigned char bank[6][4] = {
			0, 0, 1, 1,
			1, 0, 1, 1,
			1, 0, 0, 1,
			0, 1, 1, 0,
			1, 1, 1, 0,
			1, 1, 0, 0
			};
    unsigned char none_of_mine;
    int i, j, k, jj;

    /* special case for the break */

    if (first_shot) {
	pfx = px[1] - diameter;
	pfy = py[1] + 10*(.5-random_number);
	vx1 = pfx - px[0];
	vy1 = pfy - py[0];
	vsize = magnitude(vx1,vy1);
	scale = (45.+8.*random_number)/vsize;
	vx[0] = scale*vx1;
	vy[0] = scale*vy1;
	return(0.);
    }

    maxmag = -1e20;

    /* check if any of my balls on table */

    if (game == EIGHT_BALL && selection_made) {
	none_of_mine = TRUE;
	if (shooting_high_balls) {
	    for (i=9; i<16; i++)
		if (on_table[i]) none_of_mine = FALSE;
	} else {
	    for (i=1; i<8; i++)
		if (on_table[i]) none_of_mine = FALSE;
	}
    }
    for (i=1; i<num_balls; i++) {
	if (on_table[i]) {

	    /* Check for special object ball rules */

	    if (game == EIGHT_BALL) 
		if (!selection_made) {
		    if (i == 8) continue;
		} else if (none_of_mine) {
		    if (i != 8) continue;
		} else if (shooting_high_balls) {
		    if (i <= 8) continue;
		} else {
		    if (i >= 8) continue;
		}

	    dpx = px[i] - px[0];
	    dpy = py[i] - py[0];
	    psize = magnitude(dpx,dpy);

	    for (j=0; j<6; j++) {
		/*
		 * Find the position to aim at to sink the current ball
		 * (pfx,pfy)
		 */
		dpx1 = pocketx[j]-px[i];
		dpy1 = pockety[j]-py[i];
		psize1 = magnitude(dpx1,dpy1);
		pfx = px[i] - diameter*dpx1/psize1;
		pfy = py[i] - diameter*dpy1/psize1;

		/*
		 * Set newmax to the cosine of the angle between the
		 * cue ball's line of travel and the line of collision.
		 */
		dot = dpx*dpx1 + dpy*dpy1;
		newmax = dot/(psize*psize1);

		/*
		 * If the cosine is small, then the angle is sharp so
		 * downgrade its value.
		 */
		if (newmax < 0.25) newmax -= 4;

		/*
		 * Are there any balls between the cue ball and the target
		 * ball ?
		 */
		if (blocked(px[0], py[0], pfx, pfy, i, 0))
		    newmax -= (game == NINE_BALL || game == ROTATION ? 12 : 4);

		bad_value = 0;

		/* Side pockets are hard for big angles */

		if (j == 1 || j == 4)
		    bad_value -= .75*(1 - absolute(dpy1/psize1));
		/*	
		 * Are there any balls between the target ball and the
		 * pocket ?
		 */
		if (blocked(px[i], py[i], pocketx[j], pockety[j], i, 0))
		    bad_value -= 2;

		newmax += bad_value;

		/* Check for special target ball rules */

		if ((game == NINE_BALL || game == ROTATION) && first_ball != i)
		    newmax -= 100;
		if (game != NINE_BALL && game != ROTATION && after_scratch &&
		    px[i] < break_point)
		    newmax -= 100;

		newvx = pfx - px[0];
		newvy = pfy - py[0];

		if (scratch(newvx, newvy, pfx, pfy, i))
		    newmax -= 10;

		if (newmax > maxmag) {
		    vsize = distance2(	pocketx[j], pockety[j],
					px[i], py[i],
					px[0], py[0]);
		    newmax -= vsize/1500.;
		    if (newmax > maxmag) {
			scale = vsize;
			vx1 = newvx;
			vy1 = newvy;
			maxmag = newmax;
			target_pocket = j;
			target_ball = i;
		    }
		}

		/*******************/
		/* Look for combos */
		/*******************/

		for (k=1; k<num_balls; k++) {
		   if (k != i && on_table[k] &&
			(game != EIGHT_BALL || i != 8 &&
			(!selection_made || shooting_high_balls && k > 8 ||
			shooting_low_balls && k < 8)) &&
			(game != NINE_BALL && game != ROTATION || first_ball == k) &&
			(game == NINE_BALL || game == ROTATION || !after_scratch ||
			 px[k] >= break_point)) {
			bx = pfx - px[k];
			by = pfy - py[k];
			bmag = magnitude(bx,by);
			newmax = 1;		/* assume maximum */
			if (bmag == 0) {
			    newpfx = pfx - diameter*dpx1/psize1;
			    newpfy = pfy - diameter*dpy1/psize1;
			} else {
			    newpfx = px[k] - diameter*bx/bmag;
			    newpfy = py[k] - diameter*by/bmag;
			    ax = newpfx - px[0];
			    ay = newpfy - py[0];
			    amag = magnitude(ax,ay);
			    if (amag != 0) {
				adotb = ax*bx + ay*by;
				newmax = adotb/(amag*bmag);
				if (newmax < 0) continue;
			    }
			}
			ax = pocketx[j] - px[i];
			ay = pockety[j] - py[i];
			amag = magnitude(ax,ay);
			bx = pfx - px[k];
			by = pfy - py[k];
			bmag = magnitude(bx,by);
			/*
			 * Set the value for this shot based on the angle
			 * of collision of the cue and the first ball and
			 * the first and the second ball.  Use the formula:
			 *
			 *   value = (1+cos(angle1))*(1+cos(angle2))/2 - 1
			 *
			 * This will give a value in the range -1.5 to 1.
			 */
			if (bmag != 0) {
			    adotb = ax*bx + ay*by;
			    newmax = .5*(newmax+1)*(1+adotb/(amag*bmag))-1;
			}

			/* Sharp angles are tough */

			if (newmax < -.5) continue;
			if (newmax < 0.2) newmax -= 4;

			if (blocked(px[0], py[0], newpfx, newpfy, i, k))
			    newmax -= (game == NINE_BALL || game == ROTATION ? 12 : 4);
			if (blocked(px[k], py[k], pfx, pfy, i, k))
			    newmax -= 2;
			newmax += bad_value;

			newmax -= .4;		/* reduce combo attempts */

			/* If playing 9-ball, a 9-ball combo is good */
			if (game == NINE_BALL && i == 9)
			    newmax += .5;

			/* If playing rotation, a high # combo is good */
			if (game == ROTATION)
			    newmax += (float)(i-first_ball)*.05;

			newvx = newpfx - px[0];
			newvy = newpfy - py[0];

			if (scratch(newvx, newvy, newpfx, newpfy, k))
			    newmax -= 10;

			/* Gets worse with skill level */

			if (alternating) newmax -= skill*10;

			if (newmax > maxmag) {
			    vsize = distance3(	pocketx[j], pockety[j],
						px[i], py[i],
						px[k], py[k],
						px[0], py[0]);
			    newmax -= vsize/1500.;
			    if (newmax > maxmag) {
				scale = vsize;
				vx1 = newvx;
				vy1 = newvy;
				maxmag = newmax;
				target_pocket = j;
				target_ball = i;
			    }
			}
		    }
		}	/* looking for combos */

		/* Don't try any more if invalid nine ball shot */

		if ((game == NINE_BALL || game == ROTATION) && first_ball != i)
		    continue;

		/********************************/
		/* Look for cue ball bank shots */
		/********************************/

		for (k=0; k<4; k++) {
		    if (k != 2 && game != NINE_BALL && game != ROTATION &&
			after_scratch && px[i] < break_point) continue;
		    if (bank[j][k]) {
			if (k == 0) {
			   newpfx = left_edge + radius;
			   newpfy = pfy - (pfy-py[0])*(newpfx-pfx)/
				    (2*newpfx-pfx-px[0]);
			} else if (k == 1) {
			   newpfy = top_edge - radius;
			   newpfx = pfx - (pfx-px[0])*(newpfy-pfy)/
				    (2*newpfy-pfy-py[0]);
			} else if (k == 2) {
			   newpfx = right_edge - radius;
			   newpfy = pfy - (pfy-py[0])*(newpfx-pfx)/
				    (2*newpfx-pfx-px[0]);
			} else if (k == 3) {
			   newpfy = bottom_edge + radius;
			   newpfx = pfx - (pfx-px[0])*(newpfy-pfy)/
				    (2*newpfy-pfy-py[0]);
			}
			newdpx = pfx - newpfx;
			newdpy = pfy - newpfy;
			newsize = magnitude(newdpx,newdpy);
			dot = newdpx*dpx1 + newdpy*dpy1;
			if (newsize != 0 && psize1 != 0) {
			    newmax = dot/(newsize*psize1);

			    /* Sharp angles are tough */

			    if (newmax < 0) continue;
			    if (newmax < 0.25) newmax -= 4;

			    /* Don't try these too often */
			    newmax -= 1;

			    /* Gets worse with skill level */

			    if (alternating) newmax -= skill*25;

			    if (blocked(px[0], py[0], newpfx, newpfy, i, 0))
				newmax -= (game == NINE_BALL || game == ROTATION ? 12 : 4);
			    if (blocked(newpfx, newpfy, pfx, pfy, i, 0))
				newmax -= 2;
			    newmax += bad_value;

			    newvx = pfx - newpfx;
			    newvy = pfy - newpfx;
			    if (scratch(newvx, newvy, pfx, pfy, i))
				newmax -= 10;

			    for (jj=0; jj<6; jj++)
				if (square(pocketx[jj]-newpfx) +
				    square(pockety[jj]-newpfy) <
				    square(diameter)) {
				    newmax -= 10;
				    break;
				}

			    if (newmax > maxmag) {
				vsize = distance3(pocketx[j], pockety[j],
						  px[i], py[i],
						  newpfx, newpfy,
						  px[0], py[0]);
				newmax -= vsize/1500.;
				if (newmax > maxmag) {
				    scale = vsize;
				    vx1 = newpfx - px[0];
				    vy1 = newpfy - py[0];
				    maxmag = newmax;
				    target_pocket = j;
				    target_ball = i;
				}
			    }
			}
		    }
		}	/* cue ball bank shots */

		if (game != NINE_BALL && game != ROTATION && after_scratch &&
		    px[i] < break_point) break;

		/***********************************/
		/* Look for target ball bank shots */
		/***********************************/

		for (k=0; k<4; k++) {
		   if (bank[j][k]) {
			if (k == 0) {
			    newpfx = left_edge + radius;
			    newpfy = pockety[j] - (pockety[j]-py[i])*
				     (newpfx-pocketx[j])/
				     (2*newpfx-pocketx[j]-px[i]);
			} else if (k == 1) {
			    newpfy = top_edge - radius;
			    newpfx = pocketx[j] - (pocketx[j]-px[i])*
				     (newpfy-pockety[j])/
				     (2*newpfy-pockety[j]-py[i]);
			} else if (k == 2) {
			    newpfx = right_edge - radius;
			    newpfy = pockety[j] - (pockety[j]-py[i])*
				     (newpfx-pocketx[j])/
				     (2*newpfx-pocketx[j]-px[i]);
			} else if (k == 3) {
			    newpfy = bottom_edge + radius;
			    newpfx = pocketx[j] - (pocketx[j]-px[i])*
				     (newpfy-pockety[j])/
				     (2*newpfy-pockety[j]-py[i]);
			}
			newdpx = newpfx - px[i];
			newdpy = newpfy - py[i];
			newsize = magnitude(newdpx,newdpy);
			if (newsize == 0) continue;
			savepfx = px[i] - diameter*newdpx/newsize;
			savepfy = py[i] - diameter*newdpy/newsize;
			savedpx = savepfx - px[0];
			savedpy = savepfy - py[0];
			savesize = magnitude(savedpx,savedpy);
			if (savesize == 0) continue;
			dot = newdpx*savedpx + newdpy*savedpy;
			newmax = dot/(newsize*savesize);

			/* Sharp angles are tough */

			if (newmax < 0) continue;
			if (newmax < 0.25) newmax -= 4;

			/* Side pockets are hard for big angles */

			if ((j == 1 || j == 4) && (k == 0 || k == 2))
			    newmax -= 1.5;
			if (blocked(px[0], py[0], savepfx, savepfy, i, 0))
			    newmax -= (game == NINE_BALL || game == ROTATION ? 12 : 4);
			if (blocked(savepfx, savepfy, newpfx, newpfy, i, 0))
			    newmax -= 2;
			if (blocked(newpfx, newpfy, pocketx[j], pockety[j], i, 0))
			    newmax -= 2;

			newvx = savepfx - px[0];
			newvy = savepfy - py[0];
			if (scratch(newvx, newvy, savepfx, savepfy, i))
			    newmax -= 10;

			/* Check for collisions on the rebound */

			bx = px[i] - savepfx;
			by = py[i] - savepfy;
			bmag2 = bx*bx + by*by;
			dot = newvx*bx + newvy*by;
			newvx1 = dot*bx/bmag2;
			newvy1 = dot*by/bmag2;
			if (k == 0 || k == 2) {
			    if (newvx1 == 0) {
				newpos1 = py[i];
				newpos2 = py[i];
			    } else {
				t = absolute((newpfx-px[i])/newvx1);
				newpos1 = savepfy + (newvy-newvy1)*t;
				newpos2 = py[i] + newvy1*t;
			    }
			} else if (newvy1 == 0) {
			    newpos1 = px[i];
			    newpos2 = px[i];
			} else {
			    t = absolute((newpfy-py[i])/newvy1);
			    newpos1 = savepfx + (newvx-newvx1)*t;
			    newpos2 = px[i] + newvx1*t;
			}
			if (absolute(newpos1-newpos2) <= 3*radius)
			    newmax -= 3;

			/* Gets worse with skill level */

			if (alternating) newmax -= skill*10;

			if (newmax > maxmag) {
			    vsize = distance3(	pocketx[j], pockety[j],
						newpfx, newpfy,
						px[i], py[i],
						px[0], py[0]);
			    newmax -= vsize/1500.;
			    if (newmax > maxmag) {
				scale = vsize;
				vx1 = newvx;
				vy1 = newvy;
				maxmag = newmax;
				target_pocket = j;
				target_ball = i;
			    }
			}
		    }
		}	/* target ball bank shots */
	    }	    /* for each pocket... */
	}	/* if on_table... */	
    }	    /* for each ball... */

    maxmag += scale/750.;		/* don't factor in length here */
    vsize = magnitude(vx1,vy1);
    scale = (5. + 12.*alog10(scale+1.))/vsize;
    if (maxmag < -8.)
	scale *= .6;			/* if real bad shot, reduce speed */
    else if (maxmag < -1.)
	scale *= 1.4;			/* if somewhat bad, then hit hard */

    vx[0] = scale*vx1;
    vy[0] = scale*vy1;
    if (alternating) {			/* add randomness if alternating */
	float rand1, rand2, r1, r2;

	r1 = .5 - random_number;
	r2 = .5 - random_number;
	if (absolute(r2) > absolute(r1)) r1 = r2;
	rand1 = 10.*skill*r1;
	r1 = .5 - random_number;
	r2 = .5 - random_number;
	if (absolute(r2) > absolute(r1)) r1 = r2;
	rand2 = 10.*skill*r1;
	if (random_number < skill/max_skill) {
	    rand1 *= 1.5;
	    rand2 *= 1.5;
	}
	if (maxmag < -10.) {		/* extra for tough shots */
	    rand1 *= 6.;
	    rand2 *= 6.;
	} else if (maxmag < .5) {
	    rand1 *= (1.5 - maxmag*.4);
	    rand2 *= (1.5 - maxmag*.4);
	}
	vx[0] += rand1;
	vy[0] += rand2;
    }
    return(maxmag);
}

static int blocked(x1, y1, x2, y2, index1, index2)
float x1, y1;		/* the starting position */
float x2, y2;		/* the ending position */
int index1;		/* the source ball */
int index2;		/* the target ball */
{
    float ax, ay, bx, by, amag2, bmag2, adotb;
    int i;

    /*
     * Set vector a (ax,ay) to point from starting point to ending point.
     */
    ax = x2 - x1;
    ay = y2 - y1;
    amag2 = ax*ax + ay*ay;		/* size of a squared */
    if (amag2 == 0) return(0);
    for (i=1; i<num_balls; i++)		/* for each ball */
	if (i != index1 && i != index2 && on_table[i]) {
	    /*
	     * Set vector b to point from the starting point to this ball.
	     */
	    bx = px[i] - x1;
	    by = py[i] - y1;
	    adotb = ax*bx + ay*by;
	    bmag2 = bx*bx + by*by;	/* size of b squared */
	    /*
	     * See if this ball is close enough to the line of travel
	     * to cause a collision.  Use the following formula to
	     * determine closeness:
	     *
	     *    |closeness|**2 = |b|**2 - |b*cosine(ab)|**2
	     *
	     * Note that cosine(ab) = a.b/(|a|*|b|).  Do a quick check to
	     * ensure cosine(ab) > 0 which implies the two vectors are
	     * within 90 degrees of each other (pointing in the same
	     * general direction).
	     */
	    if (adotb > 0 &&
		absolute(bmag2-adotb*adotb/amag2) < square(diameter+2) &&
		bmag2 < amag2) return(1);
	}
    return(0);
}

static int scratch(newvx, newvy, cx, cy, index)
float newvx, newvy;	/* the direction of the shot */
float cx, cy;		/* the point of collision */
int index;		/* the target ball */
{
    float ax, ay, bx, by, amag2, bmag2, adotb, scale, amag, cosine;
    int i;

    /*
     * Set vector a (ax,ay) to the direction of the shot.
     * Set vector b (bx,by) to point from the collision point through
     * the target ball.
     */
    ax = newvx;
    ay = newvy;
    bx = px[index] - cx;
    by = py[index] - cy;
    bmag2 = bx*bx + by*by;		/* size of b squared */
    if (bmag2 == 0) return(0);
    adotb = ax*bx + ay*by;
    scale = adotb/bmag2;		/* size of projection of a on b */

    /*
     * Save angle between cue ball direction and collision direction.
     */
    cosine = absolute(adotb/(magnitude(ax,ay)*sqroot(bmag2)));

    /*
     * Now set vector a to the direction of the cue ball after the
     * collision.
     */
    ax = newvx - scale*bx;
    ay = newvy - scale*by;
    amag2 = ax*ax + ay*ay;		/* size of a squared */
    if (amag2 == 0) return(0);
    amag = sqroot(amag2);

    for (i=0; i<6; i++) {		/* check each pocket */
	/*
	 * Set vector b to point from the collision point to a pocket.
	 */
	bx = pocketx[i] - cx;
	by = pockety[i] - cy;
	bmag2 = bx*bx + by*by;
	if (bmag2 == 0) continue;
	adotb = ax*bx + ay*by;
	/*
	 * Compare the vector from the collision point to the pocket
	 * with the direction vector of the cue ball after collision.
	 * If they are close enough, then a scratch is possible.  Get
	 * the projection of vector b onto vector a.  Then compare the
	 * size of the vector between vector b and the new vector to
	 * the size of the pocket.
	 *
	 *    |distance from scratch|**2 = |b|**2 - |b*cosine(ab)|**2
	 *
	 * In addition, make sure the cue will be going fast enough
	 * after the collision.  This depends on the directness of
	 * the collision and the distance from the pocket.
	 */
	if (absolute(bmag2 - adotb*adotb/amag2) < square(3.*radius)) {
	    if (blocked(cx, cy, pocketx[i], pockety[i], 0, index)) break;
	    if (bmag2 > 90000. && cosine > .8 ||
		bmag2 > 10000. && cosine > .87 ||
		bmag2 > 1000. && cosine > .92 ||
		bmag2 > 500. && adotb/(amag*sqroot(bmag2)) < .5 ||
		cosine > .95) break;
	    return(1);
	}
    }
    return(0);
}

static void delay(n)
{
    next_state = state;
    set_delay_timeout(n);
    state = DELAY_OVER;
}

static void start_remote_read(addr, size)
char *addr;
int size;
{
    next_state = state;
    set_delay_timeout(1);
    if (lock_game) {
	lock_buffer = addr;
	lock_size = size;
	state = CHECK_LOCK_READ;
    } else {
	net_read(addr, size);
	state = CHECK_NET_READ;
    }
}

static void start_remote_write(addr, size)
char *addr;
int size;
{
    if (lock_game) {
	next_state = state;
	lock_buffer = addr;
	lock_size = size;
	state = CHECK_LOCK_WRITE;
    } else {
	if (net_write(addr, size) <= 0)
	    state = LOST_NET;
    }
}
