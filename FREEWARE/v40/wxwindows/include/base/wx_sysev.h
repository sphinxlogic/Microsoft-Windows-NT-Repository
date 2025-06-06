/*
 * File:	wx_sysev.h
 * Purpose:	System event base declaration
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "@(#)wx_sysev.h	1.2 5/9/94" */


#ifndef wxb_sysevh
#define wxb_sysevh

#include "wx_defs.h"

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_obj.h"
#include "wx_types.h"
#ifndef IN_CPROTO

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#endif

/*
 * wxWindows events, covering all interesting things that might happen
 * (button clicking, resizing, setting text in widgets, etc.).
 *
 * For each completely new event type, derive a new event class.
 * An event CLASS represents a C++ class defining a range of similar event TYPES;
 * examples are canvas events, panel item command events.
 * An event TYPE is a unique identifier for a particular system event,
 * such as a button press or a listbox deselection.
 *
 */

#ifdef IN_CPROTO
typedef       void    *wxEvent ;
typedef       void    *wxSystemEventClassStruc;
typedef       void    *wxSystemEventNameStruc;
typedef void  *wxEventHandler;
typedef void  *wxEventConstructor;
#else

class WXDLLEXPORT wxEvent: public wxObject
{
  DECLARE_ABSTRACT_CLASS(wxEvent)

 public:
  char *eventHandle;         // Handle of an underlying windowing system event
  WXTYPE   eventType;
  WXTYPE   eventClass;
  WXTYPE   objectType;
  wxObject *eventObject;
  long timeStamp;

  wxEvent(void);
  ~wxEvent(void);

  // Read event's arguments from any input stream
  virtual Bool ReadEvent(istream& str) = 0;
  // Write event's arguments to any output stream
  virtual Bool WriteEvent(ostream& str) = 0;

  inline WXTYPE GetEventType(void) { return eventType; }
  inline WXTYPE GetEventClass(void) { return eventClass; }
  inline WXTYPE GetObjectType(void) { return objectType; }
  inline wxObject *GetEventObject(void) { return eventObject; }
  inline virtual long GetTimestamp(void) { return timeStamp; }
  virtual void SetTimestamp(long ts = 0);
};

// Event handler function type
typedef Bool (*wxEventHandler) (wxEvent *event, Bool external);

// Event constructor function type
typedef wxEvent * (* wxEventConstructor) (WXTYPE eventType);

class WXDLLEXPORT wxSystemEventClassStruc: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxSystemEventClassStruc)

  public:
    WXTYPE eventClass;     // Event class, e.g. command event
    wxEventConstructor eventConstructor;
    char *eventDescription;
    inline wxSystemEventClassStruc(void) {}
    inline ~wxSystemEventClassStruc(void) { if (eventDescription) delete [] eventDescription ; }
};

class WXDLLEXPORT wxSystemEventNameStruc: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxSystemEventNameStruc)

  public:
    WXTYPE eventClass;     // Event class, e.g. command event
    WXTYPE eventType;      // Individual event type, e.g. button command
    char *eventName;
    inline wxSystemEventNameStruc(void) {}
    inline ~wxSystemEventNameStruc(void) { if (eventName) delete [] eventName ; }
};

// Send an event to the system, usually will be external, but specify
// external = FALSE if calling from within the main application in response
// to other events.
WXDLLEXPORT Bool wxSendEvent(wxEvent& event, Bool external = TRUE);

// Add a primary event handler - the normal event handler for this
// event. For built-in events, these would include moving and resizing
// windows. User-defined primary events might include the code to
// select an image in a diagram (which could of course be achieved by a series
// of external events for mouse-clicking, but would be more difficult to specify
// and less robust).
// Returns TRUE iff it succeeds.
WXDLLEXPORT Bool wxAddPrimaryEventHandler(wxEventHandler handlerFunc);

// Add a secondary (non-essential) event handler (e.g. for intercepting events).
// pre = TRUE iff it should be called before the
// event is executed. override = TRUE iff the handler is allowed to override
// all subsequent events by returning TRUE.
// Returns TRUE iff it succeeds.
WXDLLEXPORT Bool wxAddSecondaryEventHandler(wxEventHandler handlerFunc,
                       Bool pre = FALSE, Bool override = FALSE, Bool append = TRUE);

// Remove event handler. Returns TRUE iff it succeeds.
WXDLLEXPORT Bool wxRemoveSecondaryEventHandler(wxEventHandler handlerFunc, Bool pre = FALSE);

// Register a new event class (derived from wxEvent),
// giving the new event class type, its superclass, a function for creating
// a new event object of this class, and an optional description.
WXDLLEXPORT void wxRegisterEventClass(WXTYPE eventClassId, WXTYPE superClassId,
                          wxEventConstructor constructor, char *description = NULL);

// Register the name of the event. This will allow a simple command language
// where giving the event type name and some arguments will cause
// a new event of class eventClassId to be created, with given event type,
// and some arguments, allows an event to be dynamically constructed and sent.
WXDLLEXPORT void wxRegisterEventName(WXTYPE eventTypeId, WXTYPE eventClassId, char *eventName);

// Notify the system of the event you are about to execute/have just executed.
// If TRUE is returned and pre = TRUE, calling code should not execute the event
// (i.e. it's been intercepted by a handler and vetoed).
WXDLLEXPORT Bool wxNotifyEvent(wxEvent& event, Bool pre = FALSE);

// Define this and link before wxWindows library to allow
// registering events from 'outside' the main application
WXDLLEXPORT void wxRegisterExternalEventHandlers(void);

WXDLLEXPORT void wxInitStandardEvents(void);

WXDLLEXPORT void wxDeleteEventLists(void) ;

/*
 * Event reading/writing helper functions
 *
 */

void wxWriteString(ostream& out, char *s);

void wxWriteInteger(ostream& out, int i);

void wxWriteLong(ostream& out, long i);

void wxWriteFloat(ostream& out, float f);

void wxWriteSpace(ostream& out);

Bool wxReadWhiteSpace(istream& in);

Bool wxReadString(istream& in, char **s);

Bool wxReadInteger(istream& in, int *theInt);

Bool wxReadLong(istream& in, long *theLong);

Bool wxReadFloat(istream& in, float *theFloat);

#endif // IN_CPROTO
#endif // wxb_sysevh
