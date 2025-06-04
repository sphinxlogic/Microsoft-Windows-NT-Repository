/*
** This program illustrate the use of extended read QIO 
** for terminal input 
*/

#include <descrip.h>
#include <iodef.h>
#include <trmdef.h>

#define OK(_stat) ((_stat) & 1)

#define ESC 27
#define CSI 155
#define SS3 143

#define UP_ARROW "$A"
#define DOWN_ARROW "$B"
#define LEFT_ARROW "$C"
#define RIGHT_ARROW "$D"
#define DO "$29~"
#define PF1 "$P"

static void read_key(short chan, unsigned char *key_str);

main()
{
    short chan;
    $DESCRIPTOR(dev_desc, "SYS$COMMAND");
    int status;
    char key_str[10];

    status = sys$assign(&dev_desc, &chan, 0, 0);
    if (!OK(status)) exit(status);

    for (;;)
    {
	read_key(chan, key_str);
	
	printf("key_str: '%s'\n", key_str);

	if (strlen(key_str)==1)
	    printf ("simple char (%c)\n", key_str[0]);
	else if (0==strcmp(key_str, UP_ARROW))
	    printf("UP\n");
	else if (0==strcmp(key_str, DOWN_ARROW))
	    printf("DOWN\n");
	else if (0==strcmp(key_str, LEFT_ARROW))
	    printf("LEFT\n");
	else if (0==strcmp(key_str, RIGHT_ARROW))
	    printf("RIGHT\n");
	else if (0==strcmp(key_str, DO))
	    printf("DO\n");
	else if (0==strcmp(key_str, PF1))
	    printf("PF1\n");
	else printf("Unexpected key\n");
    }
}

static void read_key(
    short chan,              /* in  */
    unsigned char *key_str   /* out */
    )
{
    int status;
    struct
    {
        short status;
	short offset_to_terminator;
	char  terminator_char;
	char  reserved;
	char  terminator_length;
	char  cursor_position_from_eol;
    } iosb;

    static struct 
    {
	short buf_length;
	short item_code;
	char *buf_address;
	char *return_address;
    } itmlst[]= 
	{
	    { 0, TRM$_ESCTRMOVR, 9, 0 },
            { 0, 
	      TRM$_MODIFIERS, 
	      TRM$M_TM_ESCAPE | TRM$M_TM_TRMNOECHO | 
	      TRM$M_TM_NOFILTR | TRM$M_TM_NORECALL,
	      0 
	    }
	};

    status = sys$qiow(0, chan, IO$_READVBLK | IO$M_EXTEND, &iosb, 0, 0,
		    key_str, 10, 0, 0, itmlst, sizeof(itmlst));

    if (!OK(status)) exit(status);

    key_str[iosb.offset_to_terminator + iosb.terminator_length]='\0';
    
    /* 
    ** convert any escape sequence prefix (ESC,ESC [,CSI,SS3,SS3 O) to '$'
    */
    if (key_str[0]==ESC || key_str[0]==CSI || key_str[0]==SS3)
    {
	key_str[0]='$';
	if (key_str[1]=='[' || key_str[1]=='O')
	    strcpy(key_str+1, key_str+2);
    }
}
