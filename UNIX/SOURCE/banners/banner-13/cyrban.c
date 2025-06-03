/* 
 *  ####  #   #  ####   ####     #    #   # 
 * #      #  ##  #   #   #      # #   #   # 
 * #      # # #  ####    ####  #####  ##### 
 * #      ##  #  #       #  #  #   #  #   # 
 *  ####  #   #  #      #####  #   #  #   # 
 * 
 * cyrban.c by Matt Welsh: a big hack to do Cyrillic banners.
 * Handles all Cyrillic letters except Yo (because the dots won't fit--
 * Russians usu. don't use the dots anyway!).
 * The keyboard mapping is very straighforward, except:
 *  ~ = eh              / = myaknii znak    w = sha 
 *  ` = yoo             | = ih              x = scha
 *  \ = tvordnii zhak   q = tse             > = i-kratkoe
 *
 * Problems: doesn't do numbers and some punctuation, or
 * capital letters. This program can easily be generalized
 * into a 'multibanner' program to read in a BUAF character
 * file, however, this was a quickie to churn out alt.fan.warlord bait.
 *
 * v1.1  welshm@dg-rtp.dg.com
 * (c)1992 Matt Welsh
 * This program is free software.
 * You may distribute copies of the source and executable for this program
 * free of charge, as long as the copyright notice is kept intact and
 * any modifactions bear the original copyright notice.
 */

#include <stdio.h>
#include <string.h>

enum {lA=0, lB, lC, lD, lE, lF, lG, lH, lI, lJ, lK, lL, lM, lN, lO, lP,
      lQ, lR, lS, lT, lU, lV, lW, lX, lY, lZ, 
      lBS, lFS, lPI, lTL, lBQ, lGR, lSP};
#define NUMLETTERS lSP           /* Keep lSP as last letter in series! */
char *letters[NUMLETTERS][5];

void printline(c,line) char c; int line; {
  
  if ((c<='z') && (c>='a')) {
    printf("%s",letters[(int)(c-'a')][line]);
  } else {
    switch(c) {
      case '\\': printf("%s",letters[lBS][line]); break;
      case '/': printf("%s",letters[lFS][line]); break;
      case '|': printf("%s",letters[lPI][line]); break;
      case '~': printf("%s",letters[lTL][line]); break;
      case '`': printf("%s",letters[lBQ][line]); break;
      case ' ': printf("%s",letters[lSP][line]); break;
      case '>': printf("%s",letters[lGR][line]); break;
    }
  }
}


void printstring(s) char *s; { 
  
  int n,i,j,c;
  n=strlen(s); if (n>11) n=11;
  for (i=0; i<5; i++) {
    for (j=0; j<n; j++) {
      c=tolower(s[j]);
      printline(c,i);
    }
   printf("\n");
  }
}

void initletters();

void main(argc,argv) int argc; char **argv; {
  int i;
  initletters();
  if (argc<2) exit(1);
  for (i=1; i<argc; i++) {
    printstring(argv[i]);
   printf("\n\n");
  }
}


void initletters() {      
/* I know this is ugly and redundant. As I said, this can
 * easily be read into a file, but I didn't feel like messing
 * with it at 3 in the morning. 
 */ 
  letters[lA][0]="   #   ";      
  letters[lA][1]="  # #  ";
  letters[lA][2]=" ##### ";
  letters[lA][3]=" #   # ";
  letters[lA][4]=" #   # ";
 
  letters[lB][0]=" ####  ";      
  letters[lB][1]="  #    ";
  letters[lB][2]="  #### ";
  letters[lB][3]="  #  # ";
  letters[lB][4]=" ##### ";

  letters[lC][0]=" #   # ";      
  letters[lC][1]=" #   # ";
  letters[lC][2]="  #### ";
  letters[lC][3]="     # ";
  letters[lC][4]="     # ";
  
  letters[lD][0]=" ##### ";      
  letters[lD][1]="  #  # ";
  letters[lD][2]="  #  # ";
  letters[lD][3]="  #### ";
  letters[lD][4]=" #    #";
  
  letters[lE][0]=" ##### ";     
  letters[lE][1]=" #     ";
  letters[lE][2]=" ####  ";
  letters[lE][3]=" #     ";
  letters[lE][4]=" ##### ";
  
  letters[lF][0]="   #   ";      
  letters[lF][1]="  ###  ";
  letters[lF][2]=" # # # ";
  letters[lF][3]="  ###  ";
  letters[lF][4]="   #   ";
  
  letters[lG][0]=" ##### ";      
  letters[lG][1]=" #   # ";
  letters[lG][2]=" #     ";
  letters[lG][3]=" #     ";
  letters[lG][4]=" #     ";
  
  letters[lH][0]=" #   # ";      
  letters[lH][1]="  # #  ";
  letters[lH][2]="   #   ";
  letters[lH][3]="  # #  ";
  letters[lH][4]=" #   # ";
  
  letters[lI][0]=" #   # ";      
  letters[lI][1]=" #  ## ";
  letters[lI][2]=" # # # ";
  letters[lI][3]=" ##  # ";
  letters[lI][4]=" #   # ";
  
  letters[lJ][0]=" # # # ";      
  letters[lJ][1]="  ###  ";
  letters[lJ][2]="   #   ";
  letters[lJ][3]="  ###  ";
  letters[lJ][4]=" # # # ";
  
  letters[lK][0]=" #   # ";      
  letters[lK][1]=" #  #  ";
  letters[lK][2]=" ###   ";
  letters[lK][3]=" #  #  ";
  letters[lK][4]=" #   # ";
  
  letters[lL][0]=" ##### ";     
  letters[lL][1]="  #  # ";
  letters[lL][2]="  #  # ";
  letters[lL][3]="  #  # ";
  letters[lL][4]=" #   # ";
  
  letters[lM][0]=" #   # ";      
  letters[lM][1]=" ## ## ";
  letters[lM][2]=" # # # ";
  letters[lM][3]=" #   # ";
  letters[lM][4]=" #   # ";
  
  letters[lN][0]=" #   # ";     
  letters[lN][1]=" #   # ";
  letters[lN][2]=" ##### ";
  letters[lN][3]=" #   # ";
  letters[lN][4]=" #   # ";
  
  letters[lO][0]="  ###  ";     
  letters[lO][1]=" #   # ";
  letters[lO][2]=" #   # ";
  letters[lO][3]=" #   # ";
  letters[lO][4]="  ###  ";
  
  letters[lP][0]=" ##### ";     
  letters[lP][1]=" #   # ";
  letters[lP][2]=" #   # ";
  letters[lP][3]=" #   # ";
  letters[lP][4]=" #   # ";
  
  letters[lQ][0]=" #   # ";     
  letters[lQ][1]=" #   # ";
  letters[lQ][2]=" #   # ";
  letters[lQ][3]=" ##### ";
  letters[lQ][4]="     # ";
  
  letters[lR][0]=" ####  ";     
  letters[lR][1]=" #   # ";
  letters[lR][2]=" ####  ";
  letters[lR][3]=" #     ";
  letters[lR][4]=" #     ";
  
  letters[lS][0]="  #### ";     
  letters[lS][1]=" #     ";
  letters[lS][2]=" #     ";
  letters[lS][3]=" #     ";
  letters[lS][4]="  #### ";
  
  letters[lT][0]=" ##### ";     
  letters[lT][1]="   #   ";
  letters[lT][2]="   #   ";
  letters[lT][3]="   #   ";
  letters[lT][4]="   #   ";
  
  letters[lU][0]=" #   # ";     
  letters[lU][1]="  # #  ";
  letters[lU][2]="   #   ";
  letters[lU][3]="  #    ";
  letters[lU][4]=" #     ";
  
  letters[lV][0]=" ####  ";     
  letters[lV][1]=" #   # ";
  letters[lV][2]=" ####  ";
  letters[lV][3]=" #   # ";
  letters[lV][4]=" ####  ";
  
  letters[lW][0]=" #   # ";     
  letters[lW][1]=" # # # ";
  letters[lW][2]=" # # # ";
  letters[lW][3]=" # # # ";
  letters[lW][4]="  ###  ";
  
  letters[lX][0]=" #   # ";     
  letters[lX][1]=" # # # ";
  letters[lX][2]=" # # # ";
  letters[lX][3]=" # # # ";
  letters[lX][4]="  #####";
  
  letters[lY][0]="  #### ";     
  letters[lY][1]=" #   # ";
  letters[lY][2]="  #### ";
  letters[lY][3]="  #  # ";
  letters[lY][4]=" #   # ";
  
  letters[lZ][0]=" ####  ";     
  letters[lZ][1]="     # ";
  letters[lZ][2]="  ###  ";
  letters[lZ][3]="     # ";
  letters[lZ][4]=" ####  ";
  
  letters[lBS][0]=" ##  ";    
  letters[lBS][1]=" #   ";
  letters[lBS][2]=" ##  ";
  letters[lBS][3]=" # # ";
  letters[lBS][4]=" ##  ";
  
  letters[lFS][0]=" #   ";     
  letters[lFS][1]=" #   ";
  letters[lFS][2]=" ##  ";
  letters[lFS][3]=" # # ";
  letters[lFS][4]=" ##  ";
  
  letters[lPI][0]=" #   # ";     
  letters[lPI][1]=" #   # ";
  letters[lPI][2]=" ##  # ";
  letters[lPI][3]=" # # # ";
  letters[lPI][4]=" ##  # ";
  
  letters[lTL][0]=" ####  ";     
  letters[lTL][1]="     # ";
  letters[lTL][2]=" ##### ";
  letters[lTL][3]="     # ";
  letters[lTL][4]=" ####  ";
  
  letters[lBQ][0]=" #  ##  ";     
  letters[lBQ][1]=" # #  # ";
  letters[lBQ][2]=" ###  # ";
  letters[lBQ][3]=" # #  # ";
  letters[lBQ][4]=" #  ##  ";

  letters[lGR][0]=" # # # ";
  letters[lGR][1]=" #  ## ";
  letters[lGR][2]=" # # # ";
  letters[lGR][3]=" ##  # ";
  letters[lGR][4]=" #   # ";

  letters[lSP][0]="      ";
  letters[lSP][1]="      ";
  letters[lSP][2]="      ";
  letters[lSP][3]="      ";
  letters[lSP][4]="      ";
}
