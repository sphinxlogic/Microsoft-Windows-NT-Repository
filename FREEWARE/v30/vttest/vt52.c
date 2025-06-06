/* $Id: vt52.c,v 1.2 1996/09/02 23:33:04 tom Exp $ */

#include <vttest.h>
#include <ttymodes.h>
#include <esc.h>

int
tst_vt52(MENU_ARGS)
{
  static struct rtabl {
      char *rcode;
      char *rmsg;
  } resptable[] = {
      { "\033/K", " -- OK (means Standard VT52)" },
      { "\033/Z", " -- OK (means VT100 emulating VT52)" },
      { "",       " -- Unknown response"}
  };

  int i,j;
  char *response;

  rm("?2");  /* Reset ANSI (VT100) mode, Set VT52 mode  */
  esc("H");  /* Cursor home     */
  esc("J");  /* Erase to end of screen  */
  esc("H");  /* Cursor home     */
  for (i = 0; i <= max_lines-1; i++) {
    for (j = 0; j <= 9; j++)
    printf("%s", "FooBar ");
    println("Bletch");
  }
  esc("H");  /* Cursor home     */
  esc("J");  /* Erase to end of screen  */

  vt52cup(7,47);
  printf("nothing more.");
  for (i = 1; i <= 10; i++) printf("THIS SHOULD GO AWAY! ");
  for (i = 1; i <= 5; i++) {
    vt52cup(1,1);
    printf("%s", "Back scroll (this should go away)");
    esc("I");           /* Reverse LineFeed (with backscroll!)  */
  }
  vt52cup(12,60);
  esc("J");  /* Erase to end of screen  */
  for (i = 2; i <= 6; i++) {
    vt52cup(i,1);
    esc("K");           /* Erase to end of line */
  }

  for (i = 2; i <= max_lines-1; i++) {
    vt52cup(i,70); printf("%s", "**Foobar");
  }
  vt52cup(max_lines-1,10);
  for (i = max_lines-1; i >= 2; i--) {
    printf("%s", "*");
    printf("%c", 8);    /* BS */
    esc("I");           /* Reverse LineFeed (LineStarve)        */
  }
  vt52cup(1,70);
  for (i = 70; i >= 10; i--) {
    printf("%s", "*");
    esc("D"); esc("D"); /* Cursor Left */
  }
  vt52cup(max_lines,10);
  for (i = 10; i <= 70; i++) {
    printf("%s", "*");
    printf("%c", 8);    /* BS */
    esc("C");           /* Cursor Right */
  }
  vt52cup(2,11);
  for (i = 2; i <= max_lines-1; i++) {
    printf("%s", "!");
    printf("%c", 8);    /* BS */
    esc("B");           /* Cursor Down  */
  }
  vt52cup(max_lines-1,69);
  for (i = max_lines-1; i >= 2; i--) {
    printf("%s", "!");
    printf("%c", 8);    /* BS */
    esc("A");           /* Cursor Up    */
  }
  for (i = 2; i <= max_lines-1; i++) {
    vt52cup(i,71);
    esc("K");           /* Erase to end of line */
  }

  vt52cup(10,16);
  printf("%s", "The screen should be cleared, and have a centered");
  vt52cup(11,16);
  printf("%s", "rectangle of \"*\"s with \"!\"s on the inside to the");
  vt52cup(12,16);
  printf("%s", "left and right. Only this, and");
  vt52cup(13,16);
  holdit();

  esc("H");  /* Cursor home     */
  esc("J");  /* Erase to end of screen  */
  printf("%s", "This is the normal character set:");
  for (j =  0; j <=  1; j++) {
    vt52cup(3 + j, 16);
    for (i = 0; i <= 47; i++)
    printf("%c", 32 + i + 48 * j);
  }
  vt52cup(6,1);
  printf("%s", "This is the special graphics character set:");
  esc("F");     /* Select Special Graphics character set        */
  for (j =  0; j <=  1; j++) {
    vt52cup(8 + j, 16);
    for (i = 0; i <= 47; i++)
    printf("%c", 32 + i + 48 * j);
  }
  esc("G");     /* Select ASCII character set   */
  vt52cup(12,1);
  holdit();

  esc("H");  /* Cursor home     */
  esc("J");  /* Erase to end of screen  */
  println("Test of terminal response to IDENTIFY command");
  esc("Z");     /* Identify     */
  response = get_reply();
  println("");
  printf("Response was");
  esc("<");  /* Enter ANSI mode (VT100 mode) */
  chrprint(response);
  for(i = 0; resptable[i].rcode[0] != '\0'; i++)
    if (!strcmp(response, resptable[i].rcode))
      break;
  printf("%s", resptable[i].rmsg);
  println("");
  println("");
  return MENU_HOLD;
}
