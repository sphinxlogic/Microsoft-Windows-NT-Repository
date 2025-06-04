/*
 * File:	bactions.h
 * Purpose:	wxWindows GUI builder: actions
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#include "wxbuild.h"
#include "bapp.h"
#include "bwin.h"
#include "bframe.h"
#include "bsubwin.h"
#include "btoolbar.h"
#include "bitem.h"

#ifndef bactionsh
#define bactionsh

class BuildWindowData;

/*
 * An action is a connection between a user-initiated event
 * (such as a button press or menu command) and another
 * user-interface action (such as creating and showing a frame.)
 * Obviously in a real application, events may cause non-interface
 * things to happen too, but we restrict ourselves to the user
 * interface.
 */

class BuildAction: public wxObject
{
 public:
  int actionType;       // The type of action.
  int commandId;        // Command id if relevant (e.g. menubar, toolbar command)
  WXTYPE eventType;     // Type of event
  WXTYPE windowType1;   // The sort of window the action can apply to (if any)
  WXTYPE windowType2;   // The sort of window the action can apply to (if any)
  BuildWindowData *actionWindow; // Window the action applies to
  long actionWindowId;           // Id of the actionWindow (for when reading in)
  char *filename;       // Filename if relevant to the action
  char *stringArg1;     // String arguments if relevant
  char *stringArg2;
  long integerArg1;     // Integer arguments if relevant
  long integerArg2;

  BuildAction(int typ = 0, WXTYPE evType = 0, WXTYPE winType1 = -1, WXTYPE winType2 = -1);
  ~BuildAction(void);

  inline void SetFilename(char *s) { if (filename) delete[] filename; filename = copystring(s); }
  inline char *GetFilename(void) { return filename; }

  inline void SetStringArg1(char *s) { if (stringArg1) delete[] stringArg1; stringArg1 = copystring(s); }
  inline char *GetStringArg1(void) { return stringArg1; }

  inline void SetStringArg2(char *s) { if (stringArg2) delete[] stringArg2; stringArg2 = copystring(s); }
  inline char *GetStringArg2(void) { return stringArg2; }

  void ReadPrologAttributes(PrologExpr *listExpr, PrologDatabase *database);
  void WritePrologAttributes(PrologExpr *listExpr, PrologDatabase *database);

  Bool EditAction(void);
  Bool DoAction(void);

  // Make list of windows, filtering out those incompatible with
  // action type.
  void MakeWindowList(wxList& list);

  // Recursively add windows to list
  void FindWindows(BuildWindowData *win, wxList& list);

  // Find window for member name
  BuildWindowData *FindWindowForName(char *name, wxList& list);

  // win is the window from which the action originates.
  void GenerateActionCPP(ostream& stream, char *indentation, BuildWindowData *win);
};

/*
 * Prompt for an action type, edit the action, and return the action.
 *
 */

/*
To prompt for window: collect arrays of window names and pointers to
window data. Filter through winType1, winType2, e.g. showing a window can
be for a frame or dialog. Setting a label can be for wxItem only.
 */
 
BuildAction *CreateNewAction(void);

/*
 * Action types
 *
 */

/**** File operations ****/
// Load an arbitrary file, then do nothing.
#define WXBUILD_ACTION_LOAD_FILE        1
// Save an arbitrary file, then do nothing
#define WXBUILD_ACTION_SAVE_FILE        2
// Load a text file into a wxTextWindow
#define WXBUILD_ACTION_LOAD_TEXT_FILE   3
// Save a text file from a wxTextWindow
#define WXBUILD_ACTION_SAVE_TEXT_FILE   4
// Load a bitmap into a wxCanvas (implies an appropriate OnPaint to draw it)
#define WXBUILD_ACTION_LOAD_BITMAP      5

/**** General user interface operations ****/
#define WXBUILD_ACTION_ABOUT_BOX        10
#define WXBUILD_ACTION_HELP             11
#define WXBUILD_ACTION_MESSAGE_BOX      12
#define WXBUILD_ACTION_CHOICE_BOX       13

#define WXBUILD_ACTION_SHOW_WINDOW      15
#define WXBUILD_ACTION_CLOSE_WINDOW     16
                                   // Might exit if the main window.

/**** Widget set operations ****/
#define WXBUILD_ACTION_SET_LABEL        20
                                   // Set label with literal or prompted string
#define WXBUILD_ACTION_ADD_STRING       21
                                   // Add string to listbox

/**** More involved actions -- low priority ****/
#define WXBUILD_ACTION_PRINT            30
#define WXBUILD_ACTION_COPY_METAFILE_TO_CLIPBOARD 31

typedef struct {
  int actionId;
  char *actionString;
} ActionTypeRecord;


#endif // bactionsh

