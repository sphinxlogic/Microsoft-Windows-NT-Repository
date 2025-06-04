/*
 * File:	bwin.h
 * Purpose:	wxWindows GUI builder -- base window data
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef bwinh
#define bwinh

class BuildAction;

// Data for making a general window (abstract class)
class BuildWindowData: public wxObject
{
 public:
  wxWindow *userWindow; // Pointer to real wxWindow object
  BuildWindowData *buildParent; // Pointer to parent
  wxList buildActions;  // Actions for this window, if any
  wxList actionWindows; // Windows that have an action involving this
                        // this window, so if we delete this window, we
                        // don't have dangling pointers.
  char *name;
  char *description;
  char *className;
  char *functionName;

  char *memberName;   // Name if member of a containing window (when generating code)

  char *title;       // Title or label
  char *helpString;  // Help string (if any)
  char *stringValue; // String value, if any
  long intValue;     // Bool or integer value, if any
  long windowStyle;   // Style
  int x;
  int y;
  int width;
  int height;
  float treeX;
  float treeY;
  wxFont *windowFont;
  wxColour *windowColour;

  wxList children;
  WXTYPE windowType;
  long id;

  Bool dontResize; // Tells real window OnSize not to try resizing
                   // the CanvasObject or we'll go recursive!

  BuildWindowData(BuildWindowData *parent);
  ~BuildWindowData(void);

  // Must be defined by derived class from here on

  // Find the screen position of the window, possibly by recursing
  // up the hierarchy of windows.
  virtual void FindScreenPosition(int *sx, int *sy) { *sx = x; *sy = y; };
  // Ditto for real client position for a screen position
  virtual void FindClientPosition(int sx, int sy, int *cx, int *cy)
  { *cx = sx; *cy = sy; };
  
  Bool WriteRecursively(PrologDatabase *database);
  virtual Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  virtual Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);

  virtual Bool WriteActions(PrologExpr *expr, PrologDatabase *database);
  virtual Bool ReadActions(PrologExpr *expr, PrologDatabase *database);
  
  // Make the wxWindows object
  virtual Bool MakeRealWindow(void) = 0;
  void MakeRealWindowChildren(void);

  // Destroy real wxWindows object
  virtual Bool DestroyRealWindow(void) = 0;

  // Set userWindow to NULL in all children, since
  // we're deleting this real window.
  void NullUserWindows(void);

  // Popup a window for editing properties
  virtual Bool EditAttributes(void) = 0;

  // Add class-specific items to form
  virtual void AddFormItems(wxForm *form) {};

  // Add class-specific items to dialog, since forms can't
  // cope with everything.
  virtual void AddDialogItems(wxDialogBox *dialog) {};

  // Generation
  virtual void WriteClassImplementation(ostream& stream);
  virtual void WriteClassDeclaration(ostream& stream);
  virtual void GenerateWindowStyleString(char *buf);
  Bool GenerateStyle(char *buf, long flag, char *strStyle);
  virtual void GenerateConstructorCall(ostream& stream, char *parentName) {};

  // Generate bitmap, icon etc. resource entries for RC file.
  virtual void GenerateResourceEntries(ostream &stream) {};
  
  // Before construction, to initialise the window
  virtual void GenerateWindowInitialisationPre(ostream& stream, char *instanceName, char *indentation) {};
  // After construction, to initialise the window (e.g. filling out listbox)
  virtual void GenerateWindowInitialisationPost(ostream& stream, char *instanceName, char *indentation) {};

  // Write .wxr resource information for this window
  virtual void GenerateWXResourceData(ostream& stream);
  virtual void GenerateWXResourceBitmaps(ostream& stream);
  // Generate 'wxResourceLoadBitmapData' calls if necessary
  virtual void GenerateWXResourceBitmapRegistration(ostream& stream);
  virtual void GenerateDefines(ostream& stream);
  // Generates code to load wxWindows resources from static data,
  // in OnInit.
  virtual void GenerateResourceLoading(ostream& stream);
  // Generate #includes for XBM/XPM bitmap data
  virtual void GenerateBitmapDataIncludes(ostream& stream);

  // Some windows (e.g. radiobox) don't need (or like) explicit sizing.
  virtual Bool CanSizeWidth(void) { return TRUE; }
  virtual Bool CanSizeHeight(void) { return TRUE; }

  // Actions
  void AddAction(BuildAction *action);
  void DeleteAction(BuildAction *action);

  // Find an action for this window, for a specific event.
  // There may be zero or more actions for each type of event relevant
  // to this window. This function is only relevant when there
  // is only one action per event (e.g. a button, as opposed to a frame
  // where there may be many command actions for the menu command event).
  BuildAction *FindAction(WXTYPE eventType);

  // Set test mode for actual windows, recursively
  void SetTestMode(Bool testMode);

  // Find a BuildWindowData child given the real window.
  BuildWindowData *FindChildWindow(wxWindow *win);
};

#endif // bwinh


