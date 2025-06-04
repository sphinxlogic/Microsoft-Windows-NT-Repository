/*
**
**  Project:	GUESS_PASS
**  Author:	Brian VandeMerwe
**  Date:	21-Sep-1995
**  E-Mail:	brivan@spire.com
**
**	BVM	21-Sep-1995	Original code.
**
**	BVM	25-Sep-1995	Found a minor logic error which doubled the
**				amount of time to scan for a password.  (Minor,
**				huh!)
**
**	JDW	27-Sep-1995	Jake Woolstenhulme (and I thought VandeMerwe
**				was bad) added another method to search for
**				passwords.
**
**	BVM	04-Oct-1995	Added timming routines.
**
**	BVM	12-Oct-1995	Added dictionary search.
**
**	BVM	18-Oct-1995	Added CLI interface.
**
*/

/* #define FORWARDS */	    /* Adjust JDW's search method.		    */

#include <descrip>	    /* Used for descriptors			    */
#include <stdio>	    /* standard i/o routines			    */
#include <stdlib>	    /* Used mostly for the EXIT(s) routine	    */
#include <types>
#include <signal>
#include <string>
#include <starlet>
#include <str$routines>
#include <ssdef>
#include <uaidef>	    /* User Authorization stuff			    */
#include <time>		    /* Used for calculating max time		    */
#include <clidef>	    /* CLI$ calls				    */
#include <climsgdef>	    /* CLI messages				    */

extern long CLI$PRESENT();
extern long CLI$GET_VALUE();

typedef enum _nums {
	    ZERO,
	    ONE,
	    TWO,
	    THREE
	} NUMS;

#define _STATUS(x) if (((stat = x) & 7) != 1) return stat

#define true	    1
#define false	    0

#define BVM	    1
#define JDW	    2

#define START	    48		/* start with the number 0		    */
#define END	    90		/* end with the letter Z		    */

#define MAX_LENGTH  32		/* max length of a password		    */

#define DISPLAY	    1		/* number of seconds to update the display  */

static unsigned char
    alg;			/* alg used in password encryption	    */

char
    pwdaz[MAX_LENGTH + 1],	/* password to send to the HASHER	    */
    pwdaz2[MAX_LENGTH + 1];	/* string to cat to pwdaz		    */

unsigned short
    found,			/* did we find it			    */
    ok,				/* loop control				    */
    salt,			/* a little salt in your hash? ha	    */
    show_guess = 0;		/* show estimated time			    */

static long
    avg_kntr = 0,		/* Used to calculate the average crypts	    */
    do_doubleup,		/* Double up the dictionary words	    */
    kntr,			/* misc counter				    */
    kntr1,			/* misc counter				    */
    kntr2,			/* misc counter				    */
    hash[2],			/* hash used to encrypt password	    */
    max_kntr = 0,		/* most crypts/delta			    */
    password[2],		/* encrypted password			    */
    do_reverse,			/* Reverse the passwords		    */
    stat,			/* status of system calls		    */
    total_kntr = 0,		/* Used to calculate the number of pwds/sec */
    total_entries = 0;		/* Number of times show_time was called	    */

int
    dMaxadd,			/* Maximum number of additions to the pw    */
    dMaxcol,			/* Maximum number of columns for the pw	    */
    using = 99;			/* who's code to use			    */

long
    delta_q[2];			/* QUADword for SYS$BINTIM		    */

unsigned int
    dMaxit = 0,
    total_secs;

$DESCRIPTOR(delta_t, "0 :01:                   "); /* Update the display    */
$DESCRIPTOR(cli_COUNTWORDS, "COUNTWORDS");  /* Used for CLI COUNTWORDS		*/
$DESCRIPTOR(cli_DICTIONARY, "DICTIONARY");  /* Used for CLI DICTIONARY	    */
$DESCRIPTOR(cli_DOUBLEUP, "DOUBLEUP");	    /* Used for CLI DOUBLEUP	    */
$DESCRIPTOR(cli_INTERVAL, "INTERVAL");	    /* Used for CLI INTERVAL	    */
$DESCRIPTOR(cli_MAXADD, "MAXADDITIONS");    /* Used for CLI MAXADDITIONS    */
$DESCRIPTOR(cli_MAXCOL, "MAXCOLUMNS");	    /* Used for CLI MAXCOLUMNS	    */
$DESCRIPTOR(cli_METHOD, "METHOD");	    /* Used for CLI METHOD	    */
$DESCRIPTOR(cli_REVERSE, "REVERSE");	    /* Used for CLI REVERSE	    */
$DESCRIPTOR(cli_USERNAME, "USERNAME");	    /* Used for CLI USERNAME	    */
$DESCRIPTOR(pwd,	"                                ");
$DESCRIPTOR(usrnam,	"            ");
$DESCRIPTOR(maxadd,	"    ");
$DESCRIPTOR(maxcol,	"   ");
$DESCRIPTOR(method,	"   ");
$DESCRIPTOR(time_piece, "        ");
$DESCRIPTOR(ele_T,	"   ");
$DESCRIPTOR(SPACE,	" ");
$DESCRIPTOR(COLON,	":");

/*
** Item list for system calls.
*/
struct ITEM_LIST_3 {
    unsigned short	buflen,	itmcod;
    unsigned long	bufadr,	reslen;

    } itmlst1[] = {
	{ 1, UAI$_ENCRYPT,  &alg,	0 },
	{ 2, UAI$_SALT,	    &salt,	0 },
	{ 8, UAI$_PWD,	    &password,	0 },
	{ 0, 0, 0, 0 }
    };

FILE	*fptr1,				    /* File pointer		    */
	*fptr2;				    /* File pointer		    */

void	check_username();		    /* check for username as PW	    */
int	DO_BVM ( void );		    /* BVM's algorithm		    */
int	DO_JDW ( void );		    /* JDW's algorithm		    */
int	inc_ltr ( int k1 );		    /* recursive counter for DO_JDW */

void	show_time ( void );		    /* display current password	    */
					    /* being checked. Called by	    */
					    /* SETIMR			    */

int	hash_pw ( void );		    /* Hash the password and return */
					    /* the status in the global	    */
					    /* 'found'			    */

void	itoa ( int n, char s[] );	    /* convert integer to asciz	    */

void	reverse (char *s);		    /* reverse string array s	    */

time_t	t1, t2;				    /* used to time the procedure   */


main(int nargs, char *argv[])
{
    char    temp[5];			    /* Temp array for strncpy	    */

    /*
    ** Get the username from the CLI
    */
    stat = CLI$GET_VALUE(&cli_USERNAME, &usrnam);

    /*
    ** Get the method from the CLI
    */
    stat = CLI$GET_VALUE(&cli_METHOD, &method);

    /*
    ** Get the update interval from the CLI
    */
    stat = CLI$GET_VALUE(&cli_INTERVAL, &delta_t);

    /*
    ** See if the user want's to double up the dictionary words.
    */
    stat = CLI$PRESENT(&cli_DOUBLEUP);
    if (stat == CLI$_PRESENT || stat == CLI$_DEFAULTED)
	do_doubleup = true;
    else
	do_doubleup = false;

    /*
    ** See if the user want's to double up the dictionary words.
    */
    stat = CLI$PRESENT(&cli_REVERSE);
    if (stat == CLI$_PRESENT || stat == CLI$_DEFAULTED)
	do_reverse = true;
    else
	do_reverse = false;

    stat = CLI$GET_VALUE(&cli_MAXADD, &maxadd);
    strncpy (temp, maxadd.dsc$a_pointer, maxadd.dsc$w_length);
    dMaxadd = atoi(temp);

    stat = CLI$GET_VALUE(&cli_MAXCOL, &maxcol);
    strncpy (temp, maxcol.dsc$a_pointer, maxcol.dsc$w_length);
    dMaxcol = atoi(temp);

    /*
    ** Parse the interval time.
    */
    stat = str$element(&time_piece, &ONE, &SPACE, &delta_t);
    stat = str$element(&ele_T, &ZERO,  &COLON, &time_piece); *(ele_T.dsc$a_pointer + 2) = 0;
    total_secs = atoi(ele_T.dsc$a_pointer) * 60 * 60;
    stat = str$element(&ele_T, &ONE,   &COLON, &time_piece); *(ele_T.dsc$a_pointer + 2) = 0;
    total_secs += atoi(ele_T.dsc$a_pointer) * 60;
    stat = str$element(&ele_T, &TWO,   &COLON, &time_piece); *(ele_T.dsc$a_pointer + 2) = 0;
    total_secs += atoi(ele_T.dsc$a_pointer);

    /*
    ** find an algorithm to use.
    */
    if (strncmp(method.dsc$a_pointer, "BVM", method.dsc$w_length) == 0) using = BVM;
    if (strncmp(method.dsc$a_pointer, "JDW", method.dsc$w_length) == 0) using = JDW;

    /*
    ** remove the spaces from the pwd string
    */
    for (kntr1 = 0; *(usrnam.dsc$a_pointer + kntr1) != 32 && kntr1 < usrnam.dsc$w_length; kntr1++);
    usrnam.dsc$w_length = kntr1;

    printf("Guess Pass V1.0\n\n");

    printf("Scaning %.*s's account...\n", usrnam.dsc$w_length, usrnam.dsc$a_pointer);

    /*
    ** Get the HASH, SALT, and ENCRYPTION method
    */
    stat = sys$getuai(0, 0, &usrnam, &itmlst1, 0, 0, 0);
    _STATUS(stat);

    /*
    ** Count the words in the dictionary.
    */
    stat = CLI$PRESENT(&cli_COUNTWORDS);
    if (stat == CLI$_PRESENT || stat == CLI$_DEFAULTED) {
	printf("Scaning the dictionary...  ");

	/*
	** Open the file.
	*/
	fptr1 = fopen ("tools:guess_pass.dic", "r");
	if (!fptr1) {
	    printf("Can't open TOOLS:GUESS_PASS.DIC\n");
	    exit(1);
	}

	kntr = 0;
	do {
	    fgets (pwdaz, MAX_LENGTH, fptr1);
	    kntr++;
	} while (!feof(fptr1));

	fclose(fptr1);

	kntr--;

	printf("%d words were found\n", kntr);

	dMaxit = kntr + (kntr * (do_doubleup * kntr));
	dMaxit += do_reverse * dMaxit;
	dMaxit += (kntr * (dMaxadd + 1));

	printf("%u total crypts from the dictionary with %d additions per word.\n\n",
	    dMaxit, dMaxadd + 1);

    } else
	printf("\n");

    /*
    ** Start the interrupt timer.
    */
    stat = sys$bintim (&delta_t, delta_q);
    stat = sys$setimr (0, delta_q, show_time, 0, 0);

    t1 = time(0);			    /* get the start time	    */

    if (using == BVM) stat = DO_BVM();	    /* call brians code		    */
    if (using == JDW) stat = DO_JDW();	    /* call jakes code		    */

    t2 = time(0);			    /* get the stop time	    */

    printf("Elapsed time: %d min %d sec\n", (t2 - t1)/60, (t2 - t1)%60);

    /*
    ** show the status if BAD.
    */
    if (stat != 0) {
	printf("stat = %d\n", stat);
	exit(stat);
    }

}


int DO_BVM (void)
{
    char
	    temp[4];

    int
	    cntr,
	    loc_pt1,
	    loc_pt2;

    /*
    **
    ** Test the username as the password.
    **
    */

    check_username(); total_kntr = 0;
    if (found) goto SKIP_TEST;

    /*
    **
    ** Section to test against a file.
    **
    */

    /*
    ** Open the file.
    */
    fptr1 = fopen ("tools:guess_pass.dic", "r");
    if (!fptr1) {
	printf("Can't open TOOLS:GUESS_PASS.DIC\n");
	exit(1);
    }

    /*
    ** Open the file for second stream.
    */
    fptr2 = fopen ("tools:guess_pass.dic", "r");
    if (!fptr2) {
	printf("Can't open TOOLS:GUESS_PASS.DIC\n");
	exit(1);
    }

    show_guess = 1;			    /* show guess time		    */
    found = false;			    /* ain't got it yet		    */

    /*
    ** Does the user want to use the dictionary or not?
    */
    stat = CLI$PRESENT(&cli_DICTIONARY);
    if (stat == CLI$_PRESENT || stat == CLI$_DEFAULTED) {
	cntr = 0;			    /* first time through don't cat */
					    /* strings together		    */

	do {
	    rewind(fptr1);		    /* start at the top of the list */
					    /* again			    */

	    fgets (pwdaz, MAX_LENGTH, fptr1);

	    /*
	    ** Scan through the data file and HASH and MASH the passwords.
	    */
	    do {
		kntr2 = strlen(pwdaz) - 1;

		if (cntr == 1) {
		    /*
		    ** first cat the two strings together, then copy the newly
		    ** created string to pwdaz so it can be sent to the hasher
		    ** and masher
		    */
		    memcpy (&pwdaz2[loc_pt1], &pwdaz, kntr2);
		    kntr2 = loc_pt1 + kntr2;
		    memcpy (&pwdaz, &pwdaz2, kntr2);
		    pwdaz[kntr2] = 0;
		    pwdaz2[loc_pt1] = 0;
		}

		pwd.dsc$a_pointer = &pwdaz;	    /* put 'pwdaz' in the descrip   */
		pwd.dsc$w_length = kntr2;	    /* how long is it... he he he   */

		/*
		** Assume the dictionary is in uppercase.  If your not sure, change
		** the following line of code from "#if 0" to "#if 1".
		*/
#if 0
		stat = str$upcase(&pwd, &pwd);	    /* convert to upcase and check  */
		_STATUS(stat);
#endif

		/*
		** Hash the new password and update 'found'
		*/
		hash_pw();

		/*
		** if it's not found, do some surgery on the string to see if
		** that will help.
		*/
		if (!found) {
		    /*
		    ** Add numbers 0 to dMaxadd to the username.
		    */
		    for (kntr = 0; kntr <= dMaxadd && !found; kntr++) {
			itoa(kntr, temp);
			kntr1 = strlen(temp);
			memcpy (&pwdaz[kntr2], &temp, kntr1);
			pwdaz[kntr2 + kntr1] = 0;

			pwd.dsc$a_pointer = &pwdaz; /* update descriptor    */
			pwd.dsc$w_length = strlen(pwdaz);

			/*
			** Hash the new password and update 'found'
			*/
			hash_pw();
		    }

		    /*
		    ** Reverse the password
		    */
		    if (!found && do_reverse) {
			pwdaz[kntr2] = 0;	    /* get rid of the	    */
						    /* trailing numbers	    */

			reverse(&pwdaz);	    /* reverse pwdaz	    */
			pwd.dsc$a_pointer = &pwdaz; /* update the descrip   */
			pwd.dsc$w_length = kntr2;

			/*
			** Hash the new password and update 'found'
			*/
			hash_pw();
		    }
		}

		if (!found)
		    fgets (pwdaz, MAX_LENGTH, fptr1);	/* get next record  */

	    } while (!feof(fptr1) && !found);

	    if (!found) {
		if (cntr == 0) cntr = 1;
		fgets (pwdaz2, MAX_LENGTH, fptr2);
		loc_pt1 = strlen(pwdaz2) - 1;
	    }
	} while (!feof(fptr2) && !found && do_doubleup);

	fclose(fptr1);
	fclose(fptr2);
    }

    show_guess = 0;			    /* don't show guess time	    */

    /*
    ** Jump if ya need to.  I HATE GOTO'S, but when ya gotta -- ya gotta.
    */
    if (found) goto SKIP_TEST;

    /*
    **
    ** Section to test against a calculated password.
    **
    */

    kntr = kntr2 = 0;
    pwdaz[0] = START;			    /* start the Pword to check	    */

    found = false;			    /* ain't got it yet		    */

    for (kntr2 = 0; kntr2 < dMaxcol;) {	    /* build the password */
	pwd.dsc$a_pointer = &pwdaz;	    /* put 'pwdaz' in the descrip   */
	pwd.dsc$w_length = kntr2 + 1;	    /* how long is it... he he he   */

	/*
	** Hash the new password and update 'found'
	*/
	hash_pw();
	if (!found)

	    /*
	    ** update the password.  the algorithm should work like this...
	    **
	    ** first:  we should start with '0'.  then increase to '1' and keep
	    ** increasing to '9'.  then skip ':;<=>?@' and goto 'A'.  keep
	    ** increasing to 'Z'.  once there we then increase the next
	    ** position: ex. '00'.  then goto '10', '20' ... 'Z0', '01' ...
	    ** 'ZZ', '000' and so on.
	    */
	    for (kntr = 0; kntr <= kntr2; kntr++) {
		if (pwdaz[kntr] < END) {
		    pwdaz[kntr]++;
		    if (pwdaz[kntr] == 58) pwdaz[kntr] = 65;
		    break;
		} else {
		    if (kntr == kntr2) {
			kntr2++;
			pwdaz[kntr2] = START;
		    }
		    pwdaz[kntr] = START;
		}
	    }
	else
	    break;
    }

SKIP_TEST:

    if (found)
	/*
	** show it.
	*/
	printf("\npassword is: %.*s\n", pwd.dsc$w_length, pwd.dsc$a_pointer);
    else
	printf("\ncould'nt find the password\n");	    /* oops,	    */
							    /* sump-n-wrong */

    return 0;				    /* return 'no error'	    */
}


int DO_JDW (void)
{
	found = false;

	for (kntr1 = 0; kntr1 < dMaxcol && !found; kntr1++)
	{
		pwdaz [kntr1 + 1] = 0;

#ifdef FORWARDS
		for (kntr2 = 0; kntr2 <= kntr1 && !found; kntr2++) {
#else
		for (kntr2 = kntr1; kntr2 >=0 && !found; kntr2--) {
#endif
			inc_ltr (kntr2);
			if (found) break;
		}
	}

	printf("\nCould not find password\n");
	return 0;
}


int inc_ltr (int k1)
{
	int k2;

	if (found) return 0;

	for (k2 = START ; k2 <= END && !found; k2++)
	{
		if (k2 == 58) k2 = 65;
		pwdaz [k1] = k2;
	
		pwd.dsc$a_pointer = &pwdaz;
		pwd.dsc$w_length = strlen(pwdaz);

		/*
		** Hash password and update found.
		*/
		hash_pw();
		if (found)
		{
			t2 = time(0);
			printf ("\nI found : %s\n", pwdaz);
			found = true;
			break;
		}
#ifdef FORWARDS
		if ((k1 > 0) && (k1 <= kntr1)) inc_ltr (k1 - 1); 
#else
		if ((k1 < kntr1) && (k1 >= 0)) inc_ltr (k1 + 1); 
#endif
	}
}


void show_time ( void )
{
    int
	    days = 0,
	    hrs = 0,
	    mins = 0,
	    secs = 0;

    /*
    ** Get the timer going again.
    */
    stat = sys$setimr (0, delta_q, show_time, 0, 0);

    total_entries++;
    avg_kntr += total_kntr;

    secs = dMaxit / ((avg_kntr / total_secs) / total_entries) - (time(0) - t1);
    mins = secs / 60;
    secs -= mins * 60;

    hrs = mins / 60;
    mins -= hrs * 60;

    if (hrs > 24) {
	days = hrs / 24;
	hrs -= days * 24;
    }

    /*
    ** Get the maximum number of passwords searched for this "delta" time.
    */
    if (total_kntr > max_kntr) max_kntr = total_kntr;

    /*
    ** Show the user at the helm what were incrypting.
    */
    if (show_guess)
	printf("checking: %.*s %.*s@ %d crypts [avg: %d, max: %d, total: %d, guess time: %d %d:%d:%d]\n",
	    pwd.dsc$w_length, pwd.dsc$a_pointer,
	    20 - pwd.dsc$w_length, "                    ",
	    total_kntr, avg_kntr / total_entries, max_kntr, avg_kntr,
	    days, hrs, mins, secs);
    else 
	printf("checking: %.*s %.*s@ %d crypts [avg: %d, max: %d, total: %d, guess time: %d %d:%d:%d]\n",
	    pwd.dsc$w_length, pwd.dsc$a_pointer,
	    20 - pwd.dsc$w_length, "                    ",
	    total_kntr, avg_kntr / total_entries, max_kntr, avg_kntr,
	    days, hrs, mins, secs);

    total_kntr = 0;		    /* reset the total counter		    */

}


void check_username(void)
{
    char
	    temp[5];

    int
	    duh_loop,
	    loc_pt;

    for (duh_loop = 0; duh_loop < 3 && !found; duh_loop++) {
	switch (duh_loop) {
	    case 0:
		/*
		**
		** test the username as the password.
		**
		*/
		pwd.dsc$a_pointer = usrnam.dsc$a_pointer;
		pwd.dsc$w_length = usrnam.dsc$w_length;

		/*
		** Hash the new password and update 'found'
		*/
		hash_pw();

		break;

	    case 1:
		if (do_reverse) {
		    /*
		    ** put the password in pwdaz to reverse it.  i know, i
		    ** know, just reverse it using the pointers...  i'll do it
		    ** later.
		    */
		    kntr2 = pwd.dsc$w_length;
		    strncpy(pwdaz, pwd.dsc$a_pointer, pwd.dsc$w_length);
		    pwdaz[kntr2] = 0;

		    /*
		    **
		    ** test the reversed username as the password.
		    **
		    ** reverse the word and check it.
		    **
		    */
		    reverse(&pwdaz);

		    pwd.dsc$a_pointer = &pwdaz;	    /* update the descrip   */

		    /*
		    ** Hash the new password and update 'found'
		    */
		    hash_pw();
		}

		break;

	    case 2:
		/*
		**
		** reset pwd with usrnam.
		**
		*/
		pwd.dsc$a_pointer = usrnam.dsc$a_pointer;
		pwd.dsc$w_length = usrnam.dsc$w_length;

		/*
		** reset pwdaz
		*/
		strncpy(pwdaz, pwd.dsc$a_pointer, pwd.dsc$w_length);

		loc_pt = strlen(pwdaz);

		/*
		** Add numbers 0 to 999 to the username.
		*/
		for (kntr = 0; kntr <= 1000 && !found; kntr++) {
		    itoa(kntr, temp);
		    kntr1 = strlen(temp);
		    memcpy (&pwdaz[loc_pt], &temp, kntr1);
		    pwdaz[loc_pt + kntr1] = 0;

		    pwd.dsc$a_pointer = &pwdaz;	    /* update descriptor    */
		    pwd.dsc$w_length = strlen(pwdaz);

		    /*
		    ** Hash the new password and update 'found'
		    */
		    hash_pw();
		}

		break;
	}
    }
}


void reverse(char *s)
{
    static int
	c,
	i,
	j;

    /*
    ** reverse the string.
    */
    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
	c = *(s + i);
	*(s + i) = *(s + j);
	*(s + j) = c;
    }
}


void itoa(int n, char s[])
{
    int
	i,
	sign;

    if ((sign = n) < 0)		    /* record sign			    */
	n = -n;			    /* make n positive if negative	    */

    i = 0;
    do {
	s[i++] = n % 10 + '0';	    /* get next digit			    */
    } while ((n /= 10) > 0);	    /* delete it			    */

    if (sign < 0)
	s[i++] = '-';

    s[i] = '\0';

    reverse(s);		    /* reverse the string		    */
}


int hash_pw(void)
{
    total_kntr++;

    /*
    ** Hash the password.
    */
    stat = sys$hash_password(&pwd, alg, salt, &usrnam, &hash);
    _STATUS(stat);

    /*
    ** Compare the password to the user supplied password.
    */
    if ((hash[0] == password[0]) && (hash[1] == password[1]))
	found = true;		    /* let's get out of here		    */
}
