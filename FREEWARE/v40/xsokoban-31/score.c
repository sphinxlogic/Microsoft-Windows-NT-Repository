#include "config_local.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#ifndef VMS
#include <sys/types.h>
#else
#include "unix_types.h"
#endif
#include <assert.h>
#include <string.h>
#include <unistd.h>

#ifdef VMS
#include "in.h"
#else
#include <netinet/in.h>
#endif

#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#ifndef __DECC
#include <sys/param.h>
#endif
#include <fcntl.h>

#include "externs.h"
#include "globals.h"

#define SCORE_VERSION "xs01"

short scoreentries							;
struct st_entry scoretable[MAXSCOREENTRIES]				;
Boolean merging=_false_							;

static FILE *scorefile							;
static int sfdbn							;

#ifdef VMS
extern void unlink()							;
#endif


/* Acquire the lock on the score file. This is done by creating a new
   directory. If someone else holds the lock, the directory will exist.
   Since mkdir() should be done synchronously, even over NFS,  it will
   fail if someone else holds the lock.

   TIMEOUT is the number of seconds which you get to hold the lock on
   the score file for. Also, the number of seconds after which we give
   up on trying to acquire the lock nicely and start trying to break
   the lock. In theory, there are still some very unlikely ways to get
   hosed.  See comments in WriteScore about this. Portable locking over
   NFS is hard.

   After TIMEOUT seconds, we break the lock, assuming that the old
   lock-holder died in process. If the process that had the lock is
   alive but just very slow, it could end up deleting the changes we
   make, or we could delete its changes. However, the score file can't
   get trashed this way, since rename() is used to update the score file.

   See comments in WriteScore about how the score file can get trashed	,
   though it's extremely unlikely.
*/

static time_t lock_time						        ;
/* This timer is used to allow the writer to back out voluntarily if it
   notices that its time has expired. This is not a guarantee that no
   conflicts will occur, since the final rename() in WriteScore could
   take arbitrarily long, running the clock beyond TIMEOUT seconds.
*/

short LockScore(void)
{
     int i, result							;

#ifdef VMS
         lock_time = time(0)						;
#else
     for (i = 0; i < TIMEOUT; i++) {
	  result = mkdir(LOCKFILE, 0)					;
	  lock_time = time(0)						;
	  if (result < 0) {
	       if (errno == EEXIST) sleep(1)				;
	       else return E_WRITESCORE				        ;
	  } else {
	      break							;
	  }
     }

     if (result < 0) {
	 struct stat s							;
	 time_t t = time(0)						;
	 if (0 > stat(LOCKFILE, &s)) {
	     fprintf(stderr, "Warning: Can't mkdir or stat %s\n", LOCKFILE);
	     return E_WRITESCORE; 					 /*   disappeared? 			 */
	 }
	 /* Check to make sure that the lock is still the same one we
	    saw in the first place. This code assumes loosely synchronized
	    clocks. To do it right, we'd have to create another file on
	    the server machine, and compare timestamps. Not worth it.
	 */
	 if (t - s.st_ctime <= TIMEOUT) {
	     fprintf(stderr						,
	 "Warning: some other process is mucking with the lock file\n") ;
	     return E_WRITESCORE					;
	 }
	 /* Assume that the last process to muck with the score file
	    is dead.
	 */
	 fprintf(stderr, "Warning: breaking old lock\n")		;
	 if (0 > rmdir(LOCKFILE)) {
	     fprintf(stderr, "Warning: Couldn't remove old lock %s\n"	,
		     LOCKFILE)						;
	     return E_WRITESCORE					;
	 }
	 result = mkdir(LOCKFILE, 0)					;
	 if (result < 0) {
	     fprintf(stderr, "Warning: Couldn't create %s\n", LOCKFILE) ;
	     return E_WRITESCORE					;
	 }
     }

#endif
     return 0								;
}

void UnlockScore(void)
{
#ifndef VMS
     if (0 > rmdir(LOCKFILE)) {
	 fprintf(stderr, "Warning: Couldn't remove lock %s\n", LOCKFILE);
     }
#endif
}

/* print out the score list for level "level". If "level" == 0, show
   scores for all levels. */
short OutputScore(int level)
{
  short ret								;

  if ((ret = LockScore()))
       return ret							;

  if ((ret = ReadScore(0)) == 0)
    ShowScore(level)							;
  UnlockScore()							        ;
  return ((ret == 0) ? E_ENDGAME : ret)				        ;
}

/* create a new score file */
short MakeNewScore(void)
{
  short ret = 0							        ;

  if ((ret = LockScore()))
       return ret							;

  scoreentries = 0							;

  if ((scorefile = fopen(SCOREFILE, "w")) == NULL)
    ret = E_FOPENSCORE							;
  else {
    sfdbn = fileno(scorefile)						;
    if (write(sfdbn, SCORE_VERSION, 4) != 4)
      ret = E_WRITESCORE						;
    else if (write(sfdbn, &scoreentries, 2) != 2)
      ret = E_WRITESCORE						;
    fclose(scorefile)							;
  }
  UnlockScore()							        ;
  return ((ret == 0) ? E_ENDGAME : ret)				        ;
}

/* get the players current level based on the level they last scored on */
short GetUserLevel(short *lv)
{
  short ret = 0, pos							;

  if ((ret = LockScore()))
       return ret							;

  if ((scorefile = fopen(SCOREFILE, "r")) == NULL)
    ret = E_FOPENSCORE							;
  else {
    if ((ret = ReadScore(0)) == 0)
      *lv = ((pos = FindUser()) > -1) ? scoretable[pos].lv + 1 : 1	;
  }
  UnlockScore()							        ;
  return (ret)								;
}

/* Add a new score to the score file. Show the current scores if "show". */
short Score(Boolean show)
{
  short ret								;

  if ((ret = LockScore()))
       return ret							;
  if ((ret = ReadScore(0)) == 0)
    if ((ret = MakeScore()) == 0)
      if ((ret = WriteScore()) == 0)
	if (show) ShowScore(0)						;
  UnlockScore()							        ;
  return ((ret == 0) ? E_ENDGAME : ret)				        ;
}

void ntohs_entry(struct st_entry *entry)
{
    entry->lv = ntohs(entry->lv)					;
    entry->mv = ntohs(entry->mv)					;
    entry->ps = ntohs(entry->ps)					;
}




/* read in an existing score file.  Uses the ntoh() and hton() functions
 * so that the score files transfer across systems.
 */
short ReadScore(char *filename)
{
  short ret = 0							        ;
  long tmp								;
  char LSCOREFILE[132]							;

  if ( filename == 0)	strcpy(LSCOREFILE,SCOREFILE)			;
  else 			strcpy(LSCOREFILE,filename)			;


  sfdbn = open(LSCOREFILE, O_RDONLY)					;
  if (0 > sfdbn){
    ret = E_FOPENSCORE							;
  } else {
    char magic[5]							;
    if (read(sfdbn, &magic[0], 4) != 4) ret = E_READSCORE		;
    magic[4] = 0							;
    if (0 == strcmp(magic, SCORE_VERSION)) {
    									/*   we have the right version 		 */
    } else {
	fprintf(stderr, "Warning: old-style score file\n")		;
#ifndef VMS
	lseek(sfdbn, 0, SEEK_SET)					;
#endif
    }
    if (read(sfdbn, &scoreentries, 2) != 2)
      ret = E_READSCORE						        ;
    else {
      scoreentries = ntohs(scoreentries)				;
      tmp = scoreentries * sizeof(scoretable[0])			;
      if (read(sfdbn, &(scoretable[0]), tmp) != tmp)
	ret = E_READSCORE						;

      /*   swap up for little-endian machines 	 */
      for (tmp = 0; tmp < scoreentries; tmp++) ntohs_entry(&scoretable[tmp]);
    }
    (void)close(sfdbn)							;
  }
  return ret								;
}

/* Return the solution rank for table index "j". The solution rank for
   an entry is one greater than the number of entries that are better
   than it, unless there is a better or equal solution that is by the
   same person, in which case the solution rank is at least "BADSOLN".
   If two solutions are equal, the one that was arrived at first, and
   thus has a lower table index, is considered to be better.
   One solution is at least as good as another solution if it is at
   least as good in numbers of moves and pushes. Note that
   non-comparable solutions may exist.

   The array "ignore" indicates that some scoretable entries should
   be ignored for the purpose of computing rank.
*/
#define BADSOLN 100
int SolnRank(int j, Boolean *ignore)
{
    int i, rank = 1							;
    unsigned short level = scoretable[j].lv				;
    for (i = 0; i < j; i++) {
	if ((!ignore || !ignore[i]) && scoretable[i].lv == level) {
	    if (scoretable[i].mv <= scoretable[j].mv &&
		scoretable[i].ps <= scoretable[j].ps)
	    {
		if (0 == strcmp(scoretable[i].user			,
				scoretable[j].user))
		    rank = BADSOLN					;
		else
		    rank++						;
	    }
	}
    }
    return rank							        ;
}

/* Removes all score entries for a user who has multiple entries	,
 * that are for a level below the user's top level, and that are not "best
 * solutions" as defined by "SolnRank". Also removes duplicate entries
 * for a level that is equal to the user's top level, but which are not
 * the user's best solution as defined by table position.
 *
 * The current implementation is O(n^2) in the number of actual score entries.
 * A hash table would fix this.
 */

void CleanupScoreTable()
{
    int i								;
    Boolean deletable[MAXSCOREENTRIES]					;
    for (i = 0; i < scoreentries; i++) {
	deletable[i] = _false_						;
	if (SolnRank(i, deletable) > MAXSOLNRANK) {
	    char *user = scoretable[i].user				;
	    int j							;
	    for (j = 0; j < i; j++) {
		if (0 == strcmp(scoretable[j].user, user))
		  deletable[i] = _true_				        ;
	    }
	}
    }
    FlushDeletedScores(deletable)					;
}

/* Deletes entries from the score table for which the boolean array
   contains true.
*/
void FlushDeletedScores(Boolean delete[])
{
    int i, k = 0							;
    for (i = 0; i < scoreentries; i++) {
	if (i != k) CopyEntry(k, i)					;
	if (!delete[i]) k++						;
    }
    scoreentries = k							;
}

/* Adds a new user score to the score table, if appropriate. Users' top
 * level scores, and the best scores for a particular level (in moves and
 * pushes, separately considered), are always preserved.
 */
short MakeScore(void)
{
  short pos, i								;

  pos = FindPos();							 /*   find the new score position 	 */
  if (pos > -1) {							 /*   score table not empty 		 */
      for (i = scoreentries; i > pos; i--)
	CopyEntry(i, i - 1)						;
    } else {
      pos = scoreentries						;
    }

  strcpy(scoretable[pos].user, username)				;
  scoretable[pos].lv = scorelevel					;
  scoretable[pos].mv = scoremoves					;
  scoretable[pos].ps = scorepushes					;
  scoreentries++							;

  CleanupScoreTable()							;
  if (scoreentries == MAXSCOREENTRIES)
    return E_TOMUCHSE							;
  else
    return 0								;
}


/* searches the score table to find a specific player. */
short FindUser(void)
{
  short i								;
  Boolean found = _false_						;

  for (i = 0; (i < scoreentries) && (!found); i++)
    found = (strcmp(scoretable[i].user, username) == 0)		        ;
  return ((found) ? i - 1 : -1)					        ;
}

/* finds the position for a new score in the score table */
short FindPos(void)
{
  short i								;
  Boolean found = _false_						;

  for (i = 0; (i < scoreentries) && (!found); i++)
    found = ((scorelevel > scoretable[i].lv) ||
	     ((scorelevel == scoretable[i].lv) &&
	      (scoremoves < scoretable[i].mv)) ||
	     ((scorelevel == scoretable[i].lv) &&
	      (scoremoves == scoretable[i].mv) &&
	      (scorepushes < scoretable[i].ps)))			;
  return ((found) ? i - 1 : -1)					        ;
}

/*  WriteScore() writes out the score table.  It uses ntoh() and hton()
    functions to make the scorefile compatible across systems. It and
    LockScore() try to avoid trashing the score file, even across NFS.
    However, they are not perfect.

     The vulnerability here is that if we take more than 10 seconds to
     finish Score(), AND someone else decides to break the lock, AND
     they pick the same temporary name, they may write on top of the
     same file. Then we could scramble the score file by moving it with
     alacrity to SCOREFILE before they finish their update. This is
     quite unlikely, but possible.

     We could limit the damage by writing just the one score we're
     adding to a temporary file *when we can't acquire the lock*. Then	,
     the next time someone comes by and gets the lock, they integrate
     all the temporary files. Since the score change would be smaller
     than one block, duplicate temporary file names means only that a
     score change can be lost. This approach would not require a TIMEOUT.

     The problem with that scheme is that if someone dies holding the
     lock, the temporary files just pile up without getting applied.
     Also, user intervention is required to blow away the lock; and
     blowing away the lock can get us in the same trouble that happens
     here.
*/

char const *tempnm = SCOREFILE "XXXXXX"				        ;

short WriteScore(void)
{
  short ret = 0							        ;
  int tmp								;

  char tempfile[MAXPATHLEN]						;
  strcpy(tempfile, tempnm)						;

#ifdef DEBUG
  printf("TempFile :: %s\n",tempfile)					;
#endif


  (void)mktemp(tempfile)						;

#ifdef DEBUG
  printf("Mktemp() :: %s\n",tempfile)					;
#endif

  scorefile = fopen(tempfile, "w")					;

#ifdef DEBUG
  printf("Opening tempfile :: %s status=%d\n",tempfile,scorefile)	;
#endif

  if (!scorefile) return E_FOPENSCORE					;
  sfdbn = fileno(scorefile)						;

  scoreentries = htons(scoreentries)					;
  if (fwrite(SCORE_VERSION, 4, 1, scorefile) != 1) {
      ret = E_WRITESCORE						;
  } else if (fwrite(&scoreentries, 2, 1, scorefile) != 1) {
      ret = E_WRITESCORE						;
  } else {
      scoreentries = ntohs(scoreentries)				;

      									 /*   swap around for little-endian machines   */
      for (tmp = 0; tmp < scoreentries; tmp++) {
	scoretable[tmp].lv = htons(scoretable[tmp].lv)			;
	scoretable[tmp].mv = htons(scoretable[tmp].mv)			;
	scoretable[tmp].ps = htons(scoretable[tmp].ps)			;
      }
      tmp = scoreentries						;
      while (tmp > 0) {
	int n = fwrite(&(scoretable[scoreentries - tmp])		,
			sizeof(struct st_entry)			        ,
			tmp						,
			scorefile)					;
	if (n <= 0 && errno) {
	    perror(tempfile)						;
	    ret = E_WRITESCORE						;
	    break							;
	}
	tmp -= n							;
      }

      									 /*   and swap back for the rest of the run ...   */
      for (tmp = 0; tmp < scoreentries; tmp++) {
	scoretable[tmp].lv = ntohs(scoretable[tmp].lv)			;
	scoretable[tmp].mv = ntohs(scoretable[tmp].mv)			;
	scoretable[tmp].ps = ntohs(scoretable[tmp].ps)			;
      }
    }

#ifdef DEBUG
  printf("Now will flush score file\n")				        ;
#endif

    if (EOF == fflush(scorefile)) {
	ret = E_WRITESCORE						;
	perror(tempfile)						;
    } else
    if (0 > fsync(sfdbn)) {
	ret = E_WRITESCORE						;
	perror(tempfile)						;
    }
    if (EOF == fclose(scorefile)) {
	ret = E_WRITESCORE						;
	perror(tempfile)						;
    }
    if (ret == 0) {
      time_t t = time(0)						;
      if ((t - lock_time >= TIMEOUT) && (!merging) ) {
	  fprintf(stderr						,
  "Took more than %d seconds trying to write score file; lock expired.\n",
		  TIMEOUT)						;
	  ret = E_WRITESCORE						;
      } else if (0 > rename(tempfile, SCOREFILE)) {
	  ret = E_WRITESCORE						;
      }
    }
    if (ret != 0) unlink(tempfile)					;
    return ret								;
}


/* displays the score table to the user. If level == 0, show all
   levels. */
void ShowScore(int level)
{
  register i								;

  fprintf(stdout, "Rank      User     Level     Moves    Pushes\n")	;
  fprintf(stdout, "============================================\n")	;
  for (i = 0; i < scoreentries; i++) {
    if (level == 0 || scoretable[i].lv == level) {
	int rank = SolnRank(i, 0)					;
	if (rank <= MAXSOLNRANK) fprintf(stdout, "%4.4d ", rank)	;
	else fprintf(stdout, "    ")					;
	fprintf(stdout, "%15s  %8d  %8d  %8d\n", scoretable[i].user	,
		scoretable[i].lv, scoretable[i].mv, scoretable[i].ps)	;
    }
  }
}

/* duplicates a score entry */
void CopyEntry(short i1, short i2)
{
  strcpy(scoretable[i1].user, scoretable[i2].user)			;
  scoretable[i1].lv = scoretable[i2].lv				        ;
  scoretable[i1].mv = scoretable[i2].mv				        ;
  scoretable[i1].ps = scoretable[i2].ps				        ;
}





/* Attempt to merge score file */
short MergeScore(void)
{
char	filename[132]							;
char	luser[MAXUSERNAME]						;
int	offs,j								;
unsigned short mv,ps,lv,swap						;
struct  st_entry lscoretable[MAXSCOREENTRIES]				;


 printf("Enter all file names you want to merge ; CTRL/Z to terminate\n");
 offs = 0								;
 NEXT:
  strcpy(filename,"\0")						        ;
  printf("File name : ")						;
  scanf("%s",filename)							;
  if( strcmp(filename,"\0") == 0){
	if(offs != 0){
	   printf("Transfering scores for merging ; %d entries\n",offs)	;
	   for( j = 0 ; j < offs ; j++){
		strcpy(scoretable[j].user,lscoretable[j].user)		;
	        scoretable[j].lv   = lscoretable[j].lv			;
	        scoretable[j].mv   = lscoretable[j].mv			;
        	scoretable[j].ps   = lscoretable[j].ps			;
	   }

	   printf("Sorting now ...\n")					;
	   do{
	    swap = 0							;
	    for (j = 0; j < offs-1; j++) {
		if (	 (scoretable[j].lv < scoretable[j+1].lv)	||
			((scoretable[j].lv == scoretable[j+1].lv)	&&
			 ( scoretable[j].mv+scoretable[j].ps > 	\
			   scoretable[j+1].mv+scoretable[j+1].ps))	){

			/* Swap elements */
			strcpy(luser,scoretable[j].user)		;
			lv = scoretable[j].lv				;
			ps = scoretable[j].ps				;
			mv = scoretable[j].mv				;
			strcpy(scoretable[j].user,scoretable[j+1].user)	;
			scoretable[j].lv = scoretable[j+1].lv		;
			scoretable[j].ps = scoretable[j+1].ps		;
			scoretable[j].mv = scoretable[j+1].mv		;
			strcpy(scoretable[j+1].user,luser)		;
			scoretable[j+1].lv = lv				;
			scoretable[j+1].ps = ps				;
			scoretable[j+1].mv = mv				;
			swap = 1					;
		}
	    }
	   } while(swap == 1)						;


/* #if DEBUG */
/*            for( j = 0 ; j < offs ; j++){ */
/*                 printf("%s %d %d %d\n", */
/*                         scoretable[j].user      , */
/*                         scoretable[j].lv        , */
/*                         scoretable[j].mv        , */
/*                         scoretable[j].ps        ); */
/*            } */
/* #endif */

	   merging = _true_						;
/* 	   CleanupScoreTable()						; */
	   scoreentries =    offs					;
	   WriteScore()							;
	} else printf("Nothing to merge\n")				;
	exit(0)							        ;
  } else {
	if (ReadScore(filename)	== 0){
 	 UnlockScore()							;
	 for( j = 0 ; (j < scoreentries) && (j+offs < MAXSCOREENTRIES) ; j++){
		strcpy(lscoretable[j+offs].user,scoretable[j].user)	;
                lscoretable[j+offs].lv   = scoretable[j].lv		;
                lscoretable[j+offs].mv   = scoretable[j].mv		;
                lscoretable[j+offs].ps   = scoretable[j].ps		;
	 }
	 if( j+offs >= MAXSCOREENTRIES){
		printf("Cannot accept more files for merging ...\n")	;
	 } else {
		offs += scoreentries					;
	 }
	} else	printf("Could not read %s\n",filename)			;
	goto NEXT							;
  }
/*  printf("Option not implemented\n")					; */
/*  abort; */
}



