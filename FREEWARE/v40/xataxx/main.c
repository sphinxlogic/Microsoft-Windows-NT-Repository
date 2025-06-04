#include <stdio.h>
#include <X11/Xlib.h>
#include <math.h>
#include "struct.h"
#include "patchlevel.h"

extern Bool flumping, wflump, fflump, jflump, perfection;
int botmode; /* 1 for 2 player, 0 for 1 vs bot */
int boards;
int botlevel;

#ifdef VMS
#define random rand
#define srandom srand
#endif /* VMS */

char *itoa(); /* gross version */
char *rand_board();
int get_num_boards();

char disp[90];
Bool fb;
char fbd[90];

showopts()
{
	fprintf(stderr, "Syntax: xataxx <-jnw> <-d displayname> <-l level number>\n");
	fprintf(stderr, "	-d display name: name of opponents xhosted display.\n");
	fprintf(stderr, "	-j zot only on moves\n");
	fprintf(stderr, "	-l level number: number of look-ahead turns for robot (0-9).\n");
	fprintf(stderr, "	-n no zotting\n");
	fprintf(stderr, "	-p make it perfect\n");
	fprintf(stderr, "	-w will zot always\n");
}

process_flags(arg, n)
char **arg;
int n;
{
	int x;

	for(x=1;x<strlen(arg[n]);x++) {
		switch(arg[n][x]) {
			case 'b':
  				strcpy(fbd, arg[n+1]);
  				strcat(fbd, BOARDEXT);  
				fb = False;
				break;
			case 'd':
				botmode=1;
				strcpy(disp, arg[n+1]);
				if (arg[n+1][strlen(arg[n+1])-2] != ':')
					strcat(disp, ":0");
				break;
			case 'f':
				fflump = False;
				break;
			case 'h':
				showopts();
				exit(0);
				break;
			case 'l':
				botlevel = arg[n+1][0] - '0';
				if (botlevel < 0 || botlevel > 9) {
					showopts();
					exit(0);
				}
				break;
			case 'j':
				jflump = False;
				break;
			case 'n':
				wflump = False;
				break;
			case 'p':
				perfection = True;
				break;
			case 'w':
				wflump = True;
				jflump = True;
				flumping = True;
				break;
			default:
				showopts();
				exit(0);
				break;
		}
	}
}

main(argc,argv)
int argc;
char *argv[];
{
  int n, q;

  botmode = 0;
  botlevel = 2;
  flumping = True;
  wflump = True;
  fflump = True;
  jflump = False;
  perfection = False;

  fb = True;
  if (argc>1)
    for(n=1;n<argc;n++) {
      if (argv[n][0] == '-')
      process_flags(argv, n);
    }

  chdir(DATADIR);

  boards = get_num_boards();

/* Load the input board */
  if (fb) {
    if (load_board(rand_board()))
      exit(1);
  }
  else {
    if (load_board(fbd)) {
      fprintf(stderr, "You twit- that's not a valid board #.\n");
      fprintf(stderr, "If you're gonna try that, talk to Jim to find out how.\n");
      exit(1);
    }
  }

/* Start up the windows */
  initialize_display(disp);

  readbitmaps();

  show_title();

  while(1) {
    if (waitferkey(8)) {
      if (!demo())
      waitferkey(5);
      show_title();
    }
    else {
      init_pieces();
      play_loop();
    }
    if (load_board(rand_board()))
      exit(1);
  }
}

char *rand_board()
{
  int bd;
  static char fname[200];

  srandom(time(NULL));
  bd = random()%boards;
  bd++;
  strcpy(fname, itoa(bd));
  strcat(fname, BOARDEXT);  
  return(fname);
}

char *itoa(x) /* gross version - replace this hack ASAP */
int x;
{     
  int z1, z2, z3, z4, z=0;
  char nn[6];

  z1 = (int) (x)/1000;
  z2 = (int) (x)/100-(10*z1);
  z3 = (int) (x)/10-(100*z1)-(10*z2);
  z4 = (int) (x)-(1000*z1)-(100*z2)-(10*z3);

  if (z1)
    nn[z++]=(z1+'0');
  if (z2)
    nn[z++]=(z2+'0');
  if (z3)
    nn[z++]=(z3+'0');
  if (z4)
    nn[z++]=(z4+'0');
  nn[z++]='\0';
  return(nn);
}

int get_num_boards()
{
  FILE *bdfile;
  int bdx;

  if((bdfile = fopen(BOARDNUMFILE, "r")) == NULL) {
    fprintf(stderr, "Cannot open %s for reading.\n", BOARDNUMFILE);
    exit(1);
  }
  boards = 0;
  fscanf(bdfile, "%d", &bdx);
  fclose(bdfile);
  if (bdx<1) {
    fprintf(stderr, "Invalid number of allowable boards.\n");
    exit(1);
  }
  return(bdx);
}

