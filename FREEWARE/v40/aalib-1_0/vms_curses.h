#define KEY_BREAK       0401  
#define KEY_DOWN        0402    
#define KEY_UP          0403
#define KEY_LEFT        0404
#define KEY_RIGHT       0405 
#define KEY_HOME        0406 
#define KEY_BACKSPACE   0407
/**/
#define  A_STANDOUT      0x01    /* For wattron,wattroff,wattrset     */
#define  A_UNDERLINE     0x02
#define  A_REVERSE       0x04
#define  A_BLINK         0x08
#define  A_BOLD          0x10
#define  A_DIM           A_BOLD  /* dim not avaiable, make it bold    */
#define  A_ALTCHARSET    0x20    /* Use alternate character set       */
#define  A_NORMAL        0x00
/**/
#define _maxx _max_x
#define _maxy _max_y
/**/
#define attrset setattr
