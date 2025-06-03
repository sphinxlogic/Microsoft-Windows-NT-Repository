
/*  @(#)rec.c 1.18 91/10/24
 *
 *  Record handling routines used by the faces program.
 * 
 *  Copyright (c) Rich Burridge - Sun Microsystems Australia.
 *                                All rights reserved.
 *
 *  Permission is given to distribute these sources, as long as the
 *  copyright messages are not removed, and no monies are exchanged. 
 * 
 *  No responsibility is taken for any errors on inaccuracies inherent
 *  either to the comments or the code of this program, but if reported
 *  to me, then an attempt will be made to fix them.
 */

#include <stdio.h>
#if defined(SYSV32) || defined(hpux)
#include <string.h>
#else
#include <strings.h>
#endif /* SYSV32 || hpux */
#include "faces.h"
#include "extern.h"


char *
Malloc(n)
int n ;
{
  char *val ;

  if ((val = malloc((unsigned) n)) == NULL)
    {
      FPRINTF(stderr, "%s: Out of memory.\n", progname) ;
      exit(1) ;
    }
  return val ;
}


/* Add new alias to hostnames' list. */

void
add_alias(crec, username, realcomm, alias)
struct comminfo *crec ;
char *username, *realcomm, *alias ;
{
  struct peopinfo *cptemp, *ptemp ;

  ptemp = (struct peopinfo *) LINT_CAST(Malloc(sizeof(struct peopinfo))) ;
  ptemp->alias = (char *) Malloc(strlen(alias)+1) ;
  STRCPY(ptemp->alias, alias) ;
  ptemp->username = (char *) Malloc(strlen(username)+1) ;
  STRCPY(ptemp->username, username) ;
  ptemp->community = (char *) Malloc(strlen(realcomm)+1) ;
  STRCPY(ptemp->community, realcomm) ;
  ptemp->next = NULL ;

  if (crec->people == NULL) crec->people = ptemp ;
  else
    {
      cptemp = crec->people ;
      while (cptemp != NULL)
        if (cptemp->next == NULL)
          {
            cptemp->next = ptemp ;
            return ;
          }
        else cptemp = cptemp->next ;
    }
}


void
add_machine(machine, community)    /* Add new machine to list. */
char *machine, *community ;
{
  struct machinfo *temp ;

  temp = (struct machinfo *) LINT_CAST(Malloc(sizeof(struct machinfo))) ;
  temp->machine = (char *) Malloc(strlen(machine)+1) ;
  STRCPY(temp->machine, machine) ;
  temp->community = (char *) Malloc(strlen(community)+1) ;
  STRCPY(temp->community, community) ;
  temp->next = NULL ;

  if (machines == NULL) machines = mlast = temp ;   /* Start chain. */
  else if (mlast != NULL)
    {
      mlast->next = temp ;     /* Add record to existing chain. */
      mlast = temp ;           /* Point to end of chain. */
    }

  IF_DEBUG( FPRINTF(stderr, "add_machine: Added %s ==> %s\n",
                    temp->machine, temp->community) ) ;
}


void
add_ps_rec(row, column, name)  /* Add record for later animation. */
int row, column ;
char *name ;
{
  struct psinfo *temp ;

  temp = (struct psinfo *) LINT_CAST(Malloc(sizeof(struct psinfo))) ;
  temp->name = (char *) Malloc(strlen(name)+1) ;
  STRCPY(temp->name, name) ;
  temp->row = row ;
  temp->column = column ;
  temp->next = NULL ;

  if (psrecs == NULL) psrecs = plast = temp ;    /* Start chain. */
  else if (plast != NULL)
    {
      plast->next = temp ;     /* Add record to existing chain. */
      plast = temp ;
    }
}


void
add_record(community, username, timestamp, size)
char *community, *username, *timestamp ;
int size ;
{
  struct recinfo *temp ;

  temp = (struct recinfo *) LINT_CAST(Malloc(sizeof(struct recinfo))) ;
  temp->community = (char *) Malloc(strlen(community)+1) ;
  STRCPY(temp->community, community) ;
  temp->username = (char *) Malloc(strlen(username)+1) ;
  STRCPY(temp->username, username) ;
  temp->iconname = (char *) Malloc(strlen(iconname)+1) ;
  STRCPY(temp->iconname, iconname) ;
  if (x_face)
    {
      temp->faceimage = (unsigned char *) Malloc(strlen(face_buf)+1) ;
      STRCPY((char *) temp->faceimage, face_buf) ;
    }
  else temp->faceimage = NULL ;
  STRCPY(temp->ts, timestamp) ;
  temp->size = size ;
  temp->total = 1 ;
  temp->update = 0 ;
  temp->next = NULL ;
  noicons++ ;
  if (recs == NULL) recs = last = temp ;        /* Start chain. */
  else
    {
      last->next = temp ;  /* Add record to existing chain. */
      last = temp ;        /* Point to the end of the chain. */
    }
}


/* Check community list. */

void
check_comm(hostname, username, aliascomm, alias)
char *hostname, *username, *aliascomm, *alias ;
{
  struct comminfo *ctemp ;
 
  ctemp = communities ;      /* Try and find machine record for hostname. */
  while (ctemp != NULL)
    if (!strcmp(ctemp->community, aliascomm))
      {  
        add_alias(ctemp, username, hostname, alias) ;
        return ;
      }  
    else ctemp = ctemp->next ;
 
  ctemp = (struct comminfo *) LINT_CAST(Malloc(sizeof(struct comminfo))) ;
  ctemp->community = (char *) Malloc(strlen(aliascomm)+1) ;
  STRCPY(ctemp->community, aliascomm) ; 
  ctemp->people = NULL ; 
  ctemp->next = NULL ; 
 
  if (communities == NULL) communities = clast = ctemp ;  /* Start chain. */ 
  else 
    { 
      clast->next = ctemp ;   /* Add record to existing chain. */
      clast = ctemp ;         /* Point to end of chain. */
    }
  add_alias(ctemp, username, hostname, alias) ;
}


/* Remove records with zero count; zeroise count for others. */

void
garbage_collect()
{
  struct recinfo *this, *next ;

  last = NULL ;
  this = recs ;
  while (this != NULL)
    {
      next = this->next ;
      if (!this->total)
        {
          if (last != NULL) last->next = this->next ;
          if (this == recs) recs = this->next ;
          remove_record(this) ;
        }
      else
        {
          this->total = 0 ;
          last = this ;
        }
      this = next ;
    }
}


#define  MAXTOKENLEN  100
static char ptoken[MAXTOKENLEN] ;

char *
get_token(pntr)
char *pntr ;
{
  int i = 0 ;

  while (*pntr=='\t' || *pntr == ' ') pntr++ ;
  while ((i < MAXTOKENLEN) && (!index("/= \t#",*pntr)))
    ptoken[i++] = *(pntr++) ;
  while (*pntr=='\t' || *pntr == ' ') pntr++ ;
  ptoken[i] = '\0' ;
  return(pntr) ;
}


void
read_aliases()     /* Setup the hostname aliases subchains. */
{
  char alias[MAXLINE] ;      /* Alias for this community/username. */
  char aliascomm[MAXLINE] ;  /* Alias for this community/username. */
  char hostname[MAXLINE] ;   /* This records' hostname. */
  char username[MAXLINE] ;   /* This records real username. */
  char *ptr1 ;
  int i ;
  FILE *fd ;

  for (i = 0; facepath[i] != NULL; i++)
    {
      SPRINTF(peopfile, "%s/%s", facepath[i], PEOPLETAB) ;
      if ((fd = fopen(peopfile, "r")) == NULL) continue ;

      while (fgets(nextline, MAXLINE, fd) != NULL)
        {
          if (nextline[0] == '\n' || nextline[0] == '#') continue ;
          nextline[strlen(nextline)-1] = '\0' ;
          ptr1 = get_token(nextline) ;
          if (!strlen(ptoken) || *ptr1 != '/') continue ;
          STRCPY(aliascomm, ptoken) ;
          ptr1 = get_token(++ptr1) ;
          if (!strlen(ptoken) || *ptr1 != '=') continue ;
          STRCPY(alias, ptoken) ;
          ptr1 = get_token(++ptr1) ;
          if (!strlen(ptoken)) continue ;
          if (*ptr1 == '/')
            {
              STRCPY(hostname, ptoken) ;
              ptr1 = get_token(++ptr1) ;
              if (!strlen(ptoken)) continue ;
              STRCPY(username, ptoken) ;
            }
          else
            {
              STRCPY(hostname, aliascomm) ;
              STRCPY(username, ptoken) ;
            }
         check_comm(hostname, username, aliascomm, alias) ;
        }
      FCLOSE(fd) ;
    }
}


void
read_machines()       /* Setup the chain of machine/community records. */
{
  char community[MAXLINE] ;   /* This records' community. */
  char machine[MAXLINE] ;     /* This records' machine name. */
  char *ptr ;
  int i ;
  FILE *fd ;

  for (i = 0; facepath[i] != NULL; i++)
    {
      SPRINTF(machfile, "%s/%s", facepath[i], MACHINETAB) ;
      if ((fd = fopen(machfile, "r")) == NULL) continue ;

      while (fgets(nextline, MAXLINE, fd) != NULL)
        {
          if (nextline[0] == '\n' || nextline[0] == '#') continue ;
          nextline[strlen(nextline)-1] = '\0' ;
          ptr = get_token(nextline) ;
          if (!strlen(ptoken) || *ptr != '=') continue ;
          STRCPY(machine, ptoken) ;
          ptr = get_token(++ptr) ;
          if (!strlen(ptoken)) continue ;
          STRCPY(community, ptoken) ;
          add_machine(machine, community) ;
        }
      FCLOSE(fd) ;
    }
}


struct recinfo *
rec_exists(community,username)    /* Check if record exists for mail item. */
char *community, *username ;
{
  struct recinfo *temp ;     /* Pointer to mail records used for chaining. */

  temp = recs ;
  while (temp != NULL)
    {
      if (!strcmp(temp->username, username) &&
          !strcmp(temp->community, community))
        return(temp) ;       /* Record found. */
      temp = temp->next ;    /* Point to next record. */
    }
  return(NULL) ;
}


void
remove_record(thisrec)        /* Remove this record from the chain. */
struct recinfo *thisrec ;
{
  if (thisrec->community != NULL) FREE(thisrec->community) ;
  if (thisrec->faceimage != NULL) FREE(thisrec->faceimage) ;
  if (thisrec->username  != NULL) FREE(thisrec->username) ;
  if (thisrec->iconname  != NULL) FREE(thisrec->iconname) ;
  FREE((char *) thisrec) ;
}
