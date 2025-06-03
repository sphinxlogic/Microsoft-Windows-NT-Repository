/*************************************************************************\
*   				Xpeg 1.0				  *
*									  *
* Copyright 1990, Kenneth C. Nelson			 	          *
*									  *
* Rights       : I, Ken Nelson own Xpeg.  I am donating the source	  *
*		 to the public domain with the restriction that nobody	  *
*		 sells it, or anything derived from it, for anything more *
*		 than media costs.  Xpeg uses a dynamic object library	  *
*		 that I don't own.  See the subdirectory dynobj for  	  *
*		 restrictions on its use.				  *
*								          *
*                Please feel free to modify Xpeg. See Todo for details.   *
*									  *
\*************************************************************************/

/*
 * pegbd.c	- routines to update the internal storage of the pegboard.
 *		  This is done with a Dynamic object, storing a list of
 *		  person records. A person record has a name, a date last updated,
 *		  and a location. Each of these fields also has an X Widget associated
 *		  with it.
 */
 

#include <stdio.h>

#include "pegbd.h"
#include "dyn.h"



#define VALID_XPEG_STR   "XPEG_FILE\n"
#define UNKNOWN_STR	"Unknown"



/*
 * Routine which removes leading and trailing characters in string1 from string2 and
 * returns copy of the trim string.
 *
 */

extern char *strtrim();


#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

extern char *strdup();


static char *group = NULL;		/* The name of the group this pegboard tracks. */
static char *pegfilename = NULL;	/* The name of the pegfile */
static char *username = NULL;		/* The name of the user running Xpeg. */



/*
 * dateString() - takes a number representing time and converts to string
 *		  that represents the date as the Xpeg user wants it.
 *		  TBD - for now the 
 *
 */

char *dateString(time)
  long time;
{
  return strdup(ctime(&time));
}



/*
 * setUserName() - sets the name of the pegfile, used by update, save, and load.
 *
*/
void setUserName(name)
{
  if (name != NULL)
    username = strdup(name);
}


/*
 * userName() - returns the name of the person running Xpeg.
 *
 */

char *userName()
{
  if (username == NULL)
    return UNKNOWN_STR;
  else
    return username;
}



/*
 * personEditable() - returns TRUE if the person running this Xpeg can edit the persons
 *		      location.
 */
 
int personEditable(personNum)
 int personNum;
{
  int validPerson();
  Person *getPerson();
  
  if (validPerson(personNum))
  {
    return getPerson(personNum)->okay_to_edit;
  }
  else
  {
    return FALSE;
  }
}




/*
 * setPegfileName() - sets the name of the pegfile, used by update, save, and load.
 *
*/
void setPegfileName(name)
{
  if (name != NULL)
    pegfilename = strdup(name);
}


/*
 * pegfileName() - returns the name of the current pegfile
 *
 */

char *pegfileName()
{
  return pegfilename;
}




/*
 * setGroupName()	- sets the name of the group this pegboard tracks.
 *
 */

void setGroupName(name)
 char	*name;
{
  if (group != NULL)
  {
    free(NULL);    
  }
  group = strdup(name);
}


/*
 * groupName()	- returns the name of the group or empty string if not set
 *
 */

char *groupName()
{
  if (group == NULL)
  {
    return "";
  }
  else
  {
    return group;
  }
}



/*
 * A dynamic array of people that is in the pegboard.
 *
 */

 
static DynObject	people = (DynObject)  NULL;




/*
 * newPerson() - allocates a new person, this is a local routine.
 *
 */
 
static Person *newPerson(name,date,location,okay_to_edit)
  char		*name;
  char		*date;
  char		*location;
  int		okay_to_edit;
  
{
   Person *new_person;
   char   tmp[256];
   
   new_person     = (Person *) calloc(1,sizeof(Person));

   /*
    * Trim carriage returns, and update the Person record.
    *
    */
    
   new_person->name = strtrim(name,"\n");
   new_person->date = strtrim(date,"\n");
   new_person->location = strtrim(location,"\n");
   new_person->wname = NULL;
   new_person->wdate = NULL;
   new_person->wlocation = NULL;
   new_person->okay_to_edit = okay_to_edit;
   
   return new_person;
}




/*
 * getPerson() - returns the numbered person record.
 *		 this is a local routine.
 *
 */
 
static Person *getPerson(number)
  int number;
{
  return (Person *) DynGet(people,number);
}




/*
 * firstPerson() - returns the number of the first person in the pegboard.
 *
 */

int firstPerson()
{
  if (people != NULL)
    return DynLow(people);
  else
    return 0;
}




/*
 * lastPerson() - returns the number of the last person in the pegboard.
 *
 */

int lastPerson()
{
  if (people != NULL)
  {
    return DynHigh(people);
  }
  else
  {
    return 0;
  }
}



/*
 * numPeople() - returns the number of people in the pegboard.
 *
 */

int numPeople()
{
  if (people != NULL)
    return DynSize(people);
  else
   return 0;
}



/*
 * nextPerson() - returns the number of the next person in the pegboard.
 *		  kind of a kludge in that it only increments a number,
 *		  but it reads nicer than i++.
 */

int nextPerson(personNum)
{
  return personNum+1;
}



/*
 * validPerson() - returns TRUE if personNum identifies a valid person, FALSE otherwise.
 *
 */

static int validPerson(personNum)
{
  return (personNum >= firstPerson() && personNum <= lastPerson());
}





/*
 * setPersonName() - sets the name of the numbered person, does nothing if
 *		     the person number is not valid.
 *
 */

void setPersonName(personNum,name)
  int personNum;
  char *name;
{
  Person *person;
  
  if (validPerson(personNum))
  {
    person = getPerson(personNum);
    if (person->name != NULL)
    {
      cfree(person->name);
    }
    person->name = strtrim(name,"\n");	/* add name without leading of trailing \n's */
  }
}


/*
 * setPersonNameWidget() - sets the name widget of the numbered person, does nothing if
 *		           the person number is not valid.
 *
 */

void setPersonNameWidget(personNum,w)
  int personNum;
  Widget w;
{
  Person *person;
  
  if (validPerson(personNum))
  {
    person = getPerson(personNum);
    person->wname = w;
  }
}



/*
 * personName() - returns the name of the numbered person, or Unknown string.
 *
 */
 

char *personName(personNum)
   int personNum;
{

  if (validPerson(personNum))
  {
    return getPerson(personNum)->name;
  }
  else
  {
    return UNKNOWN_STR;
  }
}



/*
 * personNameWidget() - returns the name of the numbered person, or NULL.
 *
 */
 

Widget personNameWidget(personNum)
   int personNum;
{

  if (validPerson(personNum))
  {
    return getPerson(personNum)->wname;
  }
  else
  {
    return NULL;
  }
}




/*
 * personDate() - returns the date that this person was last updated into
 *		  the pegboard, or uknown string.
 *
 */
 

char *personDate(personNum)
   int personNum;
{

  if (validPerson(personNum))
  {
    return getPerson(personNum)->date;
  }
  else
  {
    return UNKNOWN_STR;
  }
}



/*
 * personDateWidget() - returns the date widget of the numbered person, or NULL.
 *
 */
 

Widget personDateWidget(personNum)
   int personNum;
{

  if (validPerson(personNum))
  {
    return getPerson(personNum)->wdate;
  }
  else
  {
    return NULL;
  }
}



/*
 * setPersonDate() - sets the date of the numbered person, does nothing if
 *		     the person number is not valid.
 *
 */

void setPersonDate(personNum,date)
  int personNum;
  char *date;
{
  Person *person;
  
  if (validPerson(personNum))
  {
    person = getPerson(personNum);
    if (person->date != NULL)
    {
      cfree(person->date);
    }
    person->date = strtrim(date,"\n");	/* add name without leading of trailing \n's */
  }
}



/*
 * setPersonDateWidget() - sets the date widget of the numbered person, does nothing if
 *		           the person number is not valid.
 *
 */

void setPersonDateWidget(personNum,w)
  int personNum;
  Widget w;
{
  Person *person;
  
  if (validPerson(personNum))
  {
    person = getPerson(personNum);
    person->wdate = w;
  }
}





/*
 * personLocation() - returns the location of this person, or the unknown string.
 *
 */


char *personLocation(personNum)
   int personNum;
{

  if (validPerson(personNum))
  {
    return getPerson(personNum)->location;
  }
  else
  {
    return UNKNOWN_STR;
  }
}



/*
 * personLocationWidget() - returns the location of the numbered person, or NULL.
 *
 */
 

Widget personLocationWidget(personNum)
   int personNum;
{

  if (validPerson(personNum))
  {
    return getPerson(personNum)->wlocation;
  }
  else
  {
    return NULL;
  }
}




/*
 * setPersonLocation() - sets the location of the numbered person, does nothing if
 *		         the person number is not valid.
 *
 */

void setPersonLocation(personNum,loc)
  int personNum;
  char *loc;
{
  Person *person;
  
  if (validPerson(personNum))
  {
    person = getPerson(personNum);
    if (person->location != NULL)
    {
      cfree(person->location);
    }
    person->location = strtrim(loc,"\n");	/* add loc without leading of trailing \n's */
  }
}


/*
 * setPersonLocationWidget() - sets the location widget of the numbered person, does nothing if
 *			       the person number is not valid.
 *
 */

void setPersonLocationWidget(personNum,w)
  int personNum;
  Widget w;
{
  Person *person;
  
  if (validPerson(personNum))
  {
    person = getPerson(personNum);
    person->wlocation = w;
  }
}






/*
 * addPerson() - adds the person into the pegboard, no checking to see if the
 *		 name of the person matches one already there,etc...
 */

void addPerson(name,date,location,changeable)
  char	*name;
  char	*date;
  char	*location;
  int   changeable;
{
  Person	new_person;

  if (people == NULL)
  {
    /*
     * Create a dynmaic array realloced every 100 inserts
     *
     */

    people = DynCreate(sizeof(Person),100);
  }
  DynAdd(people,newPerson(name,date,location,changeable));
}




/*
 * buildXpegBoard()  - reads the Xpeg file and builds up the board.
 *
*/


void buildXpegBoard(filename)
  char	*filename;
{
  FILE	*file = NULL;
  char  line[256];
  char  name[256],date[256],location[256];
  int   okay_to_edit = FALSE;

  
  file = fopen(filename,"r");
  if (file == NULL)
  {
     printf("xpeg: Could not open %s.\n",filename);
  }
  else
  {
    fgets(line,256,file);
    if (strcmp(line,VALID_XPEG_STR) != 0)
    {
      printf("xpeg: Invalid Xpeg file: %s\n",filename);
      exit(1);
    }
    else
    {
      fgets(line,256,file);
      setGroupName(line);

      while (!feof(file))
      {
	  fgets(name,256,file);

	  if (strcmp(userName(),strtrim(name,"\n")) == 0)
	  {
	    okay_to_edit = TRUE;
	  }
	  else
	  {
	    okay_to_edit = FALSE;
	  }

	  fgets(date,256,file);
	  fgets(location,256,file);
	  if (!feof(file))
	  {
  	    addPerson(name,date,location,okay_to_edit);
	  }
      }
      fclose(file);
    }
  }

}




/*
 * updateXpegBoard() - re-reads the file and updates any date and location
 *		       information that has changed.
 *
 */

void updateXpegBoard(filename)
  char *filename;
{
  FILE	*file = NULL;
  char  line[256];
  char  name[256],date[256],location[256];
  int personNum;
  int	num_people;

  num_people = numPeople(people);
  
  file = fopen(filename,"r");
  if (file == NULL)
  {
     printf("Xpeg: Could not open %s.\n",filename);
  }
  else
  {
    fgets(line,256,file);
    if (strcmp(line,VALID_XPEG_STR) != 0)
    {
      printf("Xpeg: Invalid Xpeg file: %s\n",filename);
      exit(1);
    }
    else
    {
      fgets(line,256,file);
      setGroupName(line);

      personNum = firstPerson();
      while (!feof(file))
      {
	  fgets(name,256,file);
	  fgets(date,256,file);
	  fgets(location,256,file);

	  /*
	   * If more people have been added then add them.
	   * else just update their information.
	   *
	   */
	   
	  if (!feof(file))
	  {
	     if (personNum > num_people)
  	     {
  	       addPerson(name,date,location,FALSE);
  	     }
   	     else
	    {	      
	      setPersonName(personNum,strtrim(name,"\n"));
	      setPersonDate(personNum,strtrim(date,"\n"));
	      setPersonLocation(personNum,strtrim(location,"\n"));
	    }
            personNum = nextPerson(personNum);
	  }
      }
      fclose(file);
    }
  }

}





/*
 * saveXpegBoard() - saves the current board into the xpeg file.
 *
 */

void saveXpegBoard(filename)
  char *filename;
{
  FILE *file = NULL;
  int  personNum;
  char *groupName();
  char *strtrim();
  
  file = fopen(filename,"w");
  if (file != NULL)
  {
     fprintf(file,"%s",VALID_XPEG_STR);
     fprintf(file,"%s\n",strtrim(groupName(),"\n"));
     if (numPeople() > 0)
     {
       for (personNum = firstPerson(); personNum <= lastPerson(); personNum = nextPerson(personNum))
       {
         fprintf(file,"%s\n%s\n%s\n",personName(personNum),
	  			     personDate(personNum),
				     personLocation(personNum)
 	        );
       }
     }
     fclose(file);
  }
  else
  {
    printf("Xpeg: Error saving pegboard file: %s\n",filename);
  }
}



