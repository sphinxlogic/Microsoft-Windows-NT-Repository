#ifndef __board_h_ 
#define __board_h_
//#define LEVELS 3
 #include"object.h"
#include"gboard.h"

class Board;

#include"pac.h"
#include"types.h"
#include"element.h"
#include"blank.h"
#include"walls.h"
#include"specwall.h"
#include"food.h"
#include"supefood.h"
#include"sizes.h"
#include"usrinter.h"

#include"dynamiel.h"

static int foods=168;	//how many food items there are on each board

#define initen {0}

//static char boards[][][];

class DynamicElement;
class G_Board; 

//the following is the list structure for the DynamicElements, the sprites.
//it includes a pointer to the element, what graphical id it has
//the coordinates and a pointer to the next element of the structure

struct liststruct {DynamicElement/* Moveables*/ *g; GID_TYPE gid; int x,y; struct liststruct*next;};

class Board : public Object {

static liststruct* list;	//pointer to list of recently display sprites
static liststruct* oldlist;	//pointer to list of next most recently

static Element *board[BOARDWIDTH][BOARDHEIGHT];	//matrix pointer to elements

static G_Board* gboard;		//pointer to the class which handles
				//the graphical elements of the board

static void replace(Element*);	//?????????

static UserInterface* u;	//pointer to the user interface
static Board* _instance;	//pointer to the instance of the board
 
 protected:

 Board();	//constructor

 public:
 
 static Board* instance();	//returns the pointer to the instance
 ~Board();			//destructor
 static void start(int);	//initialize board for a given level
 static void draw(void);	//draw entire board
 static void delta_draw(void);	//draw changes of the board
 static void sprite(DynamicElement *);	//let dynamicelement be a sprite
 static void eat(int,int);	//removes whats at coordinates
 static typ what_is(int,int); 	//what "class" element at coordinates is 
};

#endif  






