/*
 * File:	wx_stdev.h
 * Purpose:	Standard wxWindows event classes
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_stdev.h	1.2 5/9/94" */


#ifndef wxb_stdevh
#define wxb_stdevh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_obj.h"
#include "wx_sysev.h"


/*
 * Event types
 *
 */
enum {
 EVENT_TYPES_FIRST = 10000,

/* Command event types */
 wxEVENT_TYPE_BUTTON_COMMAND,
 wxEVENT_TYPE_CHECKBOX_COMMAND,
 wxEVENT_TYPE_RESERVED1,
 wxEVENT_TYPE_CHOICE_COMMAND,
 wxEVENT_TYPE_LISTBOX_COMMAND,
 wxEVENT_TYPE_LISTBOX_DCLICK_COMMAND,
 wxEVENT_TYPE_TEXT_COMMAND,
 wxEVENT_TYPE_MULTITEXT_COMMAND,
 wxEVENT_TYPE_MENU_COMMAND,
 wxEVENT_TYPE_SLIDER_COMMAND,
 wxEVENT_TYPE_RADIOBOX_COMMAND,
 wxEVENT_TYPE_RADIOBUTTON_COMMAND,
 wxEVENT_TYPE_TEXT_ENTER_COMMAND,
 wxEVENT_TYPE_SET_FOCUS,
 wxEVENT_TYPE_KILL_FOCUS,
 wxEVENT_TYPE_SCROLLBAR_COMMAND,
 wxEVENT_TYPE_VIRT_LISTBOX_COMMAND,
 wxEVENT_TYPE_COMBOBOX_COMMAND,

/* Mouse event types */
 wxEVENT_TYPE_LEFT_DOWN =  EVENT_TYPES_FIRST + 30,
 wxEVENT_TYPE_LEFT_UP,
 wxEVENT_TYPE_MIDDLE_DOWN,
 wxEVENT_TYPE_MIDDLE_UP,
 wxEVENT_TYPE_RIGHT_DOWN,
 wxEVENT_TYPE_RIGHT_UP,
 wxEVENT_TYPE_MOTION,
 wxEVENT_TYPE_ENTER_WINDOW,
 wxEVENT_TYPE_LEAVE_WINDOW,
 wxEVENT_TYPE_LEFT_DCLICK,
 wxEVENT_TYPE_MIDDLE_DCLICK,
 wxEVENT_TYPE_RIGHT_DCLICK, /* +41 */

/* Character input event type  */
 wxEVENT_TYPE_CHAR = EVENT_TYPES_FIRST + 50,

 /*
  * Scrollbar event identifiers
  */
 wxEVENT_TYPE_SCROLL_TOP,
 wxEVENT_TYPE_SCROLL_BOTTOM,
 wxEVENT_TYPE_SCROLL_LINEUP,
 wxEVENT_TYPE_SCROLL_LINEDOWN,
 wxEVENT_TYPE_SCROLL_PAGEUP,
 wxEVENT_TYPE_SCROLL_PAGEDOWN,
 wxEVENT_TYPE_SCROLL_THUMBTRACK
};

#ifdef IN_CPROTO
typedef       void    *wxCommandEvent ;
typedef       void    *wxMouseEvent;
typedef       void    *wxKeyEvent;
#else

// Item or menu event class
class WXDLLEXPORT wxCommandEvent: public wxEvent
{
  DECLARE_DYNAMIC_CLASS(wxCommandEvent)

 public:
  char *commandString; // String event argument
  int commandInt;      // Integer event argument
  long extraLong;      // Additional information (e.g. select/deselect)
  char *labelString;   // The label of the item
  char *clientData;    // Arbitrary client data
  wxCommandEvent(WXTYPE commandType = 0);
  inline ~wxCommandEvent(void) {}

  /*
   * Accessors dependent on context
   *
   */

  // Get listbox/choice client data  
  inline virtual char *GetClientData() { return clientData; }

  // Get listbox selection if single-choice
  inline virtual int GetSelection() { return commandInt; }

  // Get listbox/choice selection string
  inline virtual char *GetString() { return commandString; }

  // Get checkbox value
  inline virtual Bool Checked() { return (Bool)commandInt; }

  // TRUE if the listbox event was a selection.
  inline virtual Bool IsSelection() { return (Bool)extraLong; }

  Bool ReadEvent(istream&);
  Bool WriteEvent(ostream&);
};

// Mouse event class
class WXDLLEXPORT wxMouseEvent: public wxEvent
{
  DECLARE_DYNAMIC_CLASS(wxMouseEvent)

 public:
  float x;
  float y;
  Bool leftDown;
  Bool middleDown;
  Bool rightDown;

  Bool controlDown;
  Bool shiftDown;
  Bool altDown;
  Bool metaDown;
  
  wxMouseEvent(WXTYPE mouseType = 0);

  // Was it a button event?
  virtual Bool IsButton(void);

  // Was it a down event from button 1, 2 or 3 or any?
  virtual Bool ButtonDown(int but = -1);

  // Was it a dclick event from button 1, 2 or 3 or any?
  virtual Bool ButtonDClick(int but = -1);

  // Was it a up event from button 1, 2 or 3 or any?
  virtual Bool ButtonUp(int but = -1);

  // Was the given button 1,2,3 or any changing state?
  virtual Bool Button(int but);

  // Was the given button 1,2,3 or any in Down state?
  virtual Bool ButtonIsDown(int but);

  // Find state of shift/control keys
  virtual Bool ControlDown(void);
  virtual Bool MetaDown(void);
  virtual Bool AltDown(void);
  virtual Bool ShiftDown(void);

  // Find which event was just generated
  virtual Bool LeftDown(void);
  virtual Bool MiddleDown(void);
  virtual Bool RightDown(void);

  virtual Bool LeftUp(void);
  virtual Bool MiddleUp(void);
  virtual Bool RightUp(void);

  virtual Bool LeftDClick(void);
  virtual Bool MiddleDClick(void);
  virtual Bool RightDClick(void);

  // Find the current state of the mouse buttons (regardless
  // of current event type)
  virtual Bool LeftIsDown(void);
  virtual Bool MiddleIsDown(void);
  virtual Bool RightIsDown(void);

  // True if a button is down and the mouse is moving
  virtual Bool Dragging(void);

  // True if the mouse is moving, and no button is down
  virtual Bool Moving(void);

  // True if the mouse is just entering the window
  virtual Bool Entering(void);

  // True if the mouse is just leaving the window
  virtual Bool Leaving(void);

  // Find the position of the event
  virtual void Position(float *x, float *y);

  Bool ReadEvent(istream&);
  Bool WriteEvent(ostream&);
};

// Keyboard input event class
class WXDLLEXPORT wxKeyEvent: public wxEvent
{
  DECLARE_DYNAMIC_CLASS(wxKeyEvent)

 public:
  float x ;
  float y ;
  long keyCode;
  Bool controlDown;
  Bool shiftDown;
  Bool altDown;
  Bool metaDown;
  
  wxKeyEvent(WXTYPE keyType = 0);

  virtual Bool ControlDown(void);
  virtual Bool AltDown(void);
  virtual Bool MetaDown(void);
  virtual Bool ShiftDown(void);
  virtual long KeyCode(void);

  virtual void Position(float *x,float *y) ;

  Bool ReadEvent(istream&);
  Bool WriteEvent(ostream&);
};

#endif // IN_CPROTO
#endif // wxb_stdevh

