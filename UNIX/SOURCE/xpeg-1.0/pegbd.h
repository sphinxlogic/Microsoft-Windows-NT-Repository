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
 * pegbd.h  - provides definition of a person and external definitions of global
 *	      routines for the pegboard.
 *
 */


#ifndef PEGBOARD_DEFINED

#  define PEGBOARD_DEFINED

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>



/*
 * A person on the pegboard is really two label wigdets and a
 * text widget that you can edit.  The char * fields are updated
 * when the pegboard file is read, the widgets are then created using
 * that information.  When an update is done the text from the location
 * widget is used because it may have been edited.
 * The okay_to_edit field tells if the person running the xpeg application
 * has authorization to edit location information for this person.
 *
 */


typedef struct person
{
  Widget	wname;
  char		*name;
  Widget	wdate;
  char		*date;
  Widget	wlocation;
  char		*location;
  int		okay_to_edit;
} Person;


/*
 * person routines
 *
 */
 
extern  char   *personName();
extern  char   *personDate();
extern  char   *personLocation();
extern  Widget personNameWidget();
extern  Widget personDateWidget();
extern  Widget personLocationWidget();

extern  int    personEditable();

extern  void setPersonName();
extern  void setPersonDate();
extern  void setPersonLocation();
extern  void setPersonNameWidget();
extern  void setPersonDateWidget();
extern  void setPersonLocationWidget();

extern  int firstPerson();
extern  int lastPerson();
extern  int numPeople();
extern  int nextPerson();


extern  void addPerson();


/*
 * pegboard routines.
 *
 */

extern void setPegfileName();
extern char *pegfileName();

extern void setGroupName();
extern char *groupName();

extern char *dateString();

extern void buildXpegBoard();
extern void saveXpegBoard();
extern void updateXpegBoard();




#endif PEGBOARD_DEFINED
