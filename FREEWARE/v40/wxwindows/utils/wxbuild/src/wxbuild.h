/*
 * File:	wxbuild.h
 * Purpose:	wxWindows GUI builder
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#include "read.h"
#include "wx_help.h"
#include "wx_form.h"
#include "wx_tbar.h"

#ifdef wx_msw
#include "wx_bbar.h"
#endif

#ifndef wxbuildh
#define wxbuildh

class BuildWindowData;
class BuildFrameData;
class ObjectEditorCanvas;
class EditorToolBar;

extern wxList BuildSelections;

void SelectWindow(BuildWindowData *win);
void DeselectWindow(BuildWindowData *win);
void DeselectAll(void);
BuildWindowData *GetFirstSelection(void);

// Define a new frame for the main project window
class BuildFrame: public wxFrame
{
 public:
  wxPanel *panel;
  EditorToolBar *toolbar;
  ObjectEditorCanvas *canvas;
  wxListBox *buildWindowsItem;

  BuildWindowData *currentWindow;
  int currentZoom;
  
  BuildFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
  Bool OnClose(void);
  void OnSize(int x, int y);
  void OnMenuCommand(int id);
  void ClearEditor(void);
};

// Define a frame for report window
class ReportFrame: public wxFrame
{
 public:
  wxTextWindow *textWindow;
  ReportFrame(wxFrame *frame, char *title, int x, int y, int w, int h, long style);
  Bool OnClose(void);
//  void OnSize(int x, int y);
//  void OnMenuCommand(int id);
};

void ShowReportWindow(void);
void ClearReportWindow(void);
void Report(char *s);
void MakeModified(Bool mod = TRUE);
Bool IsModified(void);

extern BuildFrame   *MainFrame;

class ObjectEditorCanvas: public wxCanvas
{
 public:
  ObjectEditorCanvas(wxFrame *frame, int x = -1, int y = -1, int width = -1, int height = -1,
               long style = wxRETAINED);
  ~ObjectEditorCanvas(void);
  BuildWindowData *FindSelectionOfType(WXTYPE type);
  BuildWindowData *FindSuitableItemParent(void);
  void OnPaint(void);
  void OnEvent(wxMouseEvent& event);
  void Redraw(void);
};

/*
 * Object editor tool palette
 *
 */
 
class EditorToolPalette: public wxToolBar
{
  public:
  int currentlySelected;

  EditorToolPalette(wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = 0, int direction = wxVERTICAL, int RowsOrColumns = 2);
  Bool OnLeftClick(int toolIndex, Bool toggled);
  void OnMouseEnter(int toolIndex);
};

#define PALETTE_FRAME           1
#define PALETTE_DIALOG_BOX      2
#define PALETTE_PANEL           3
#define PALETTE_CANVAS          4
#define PALETTE_TEXT_WINDOW     5
#define PALETTE_MESSAGE         6
#define PALETTE_BUTTON          7
#define PALETTE_CHECKBOX        8
#define PALETTE_LISTBOX         9
#define PALETTE_RADIOBOX        10
#define PALETTE_CHOICE          11
#define PALETTE_TEXT            12
#define PALETTE_MULTITEXT       13
#define PALETTE_SLIDER          14
#define PALETTE_ARROW           15
#define PALETTE_GAUGE           16
#define PALETTE_GROUPBOX        17

class EditorToolPaletteFrame: public wxFrame
{
  public:
  EditorToolPalette *palette;
  EditorToolPaletteFrame(wxFrame *parent, char *title, int x, int y, int w, int h, long style):
    wxFrame(parent, title, x, y, w, h, style)
  {
  }
  Bool OnClose(void);
};

extern EditorToolPaletteFrame *EditorPaletteFrame;

/*
 * Main toolbar
 *
 */
 
#ifdef wx_msw
class EditorToolBar: public wxButtonBar
#else
class EditorToolBar: public wxToolBar
#endif
{
  public:
  EditorToolBar(wxFrame *frame, int x = 0, int y = 0, int w = -1, int h = -1,
            long style = 0, int direction = wxVERTICAL, int RowsOrColumns = 2);
  Bool OnLeftClick(int toolIndex, Bool toggled);
  void OnMouseEnter(int toolIndex);
  void OnPaint(void);
};

// Toolbar ids
#define TOOLBAR_LOAD_FILE       1
#define TOOLBAR_SAVE_FILE       2
#define TOOLBAR_GEN_CPP         3
#define TOOLBAR_GEN_CLIPS       4
#define TOOLBAR_TREE            5
#define TOOLBAR_HELP            6

// Formatting tools
#define TOOLBAR_FORMAT_HORIZ    10
#define TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN    11
#define TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN    12
#define TOOLBAR_FORMAT_VERT     13
#define TOOLBAR_FORMAT_VERT_TOP_ALIGN     14
#define TOOLBAR_FORMAT_VERT_BOT_ALIGN     15

extern wxCursor *handCursor;
extern wxCursor *crossCursor;

/*
 * Form class which can be used for all property dialogs
 *
 */

extern Bool formCancelled;
 
class BuildForm: public wxForm
{
 public:
  wxDialogBox *dialog;
  BuildForm(char *helpTopic = NULL);
  ~BuildForm(void);
  void OnOk(void);
  void OnCancel(void);
  void OnHelp(void);
};

// Called before a form is popped up
void SetHelpTopic(char *topic);

extern wxFont *SmallButtonFont;
extern wxFont *SmallLabelFont;
extern wxFont *TextWindowFont;

/*
 * Utilities
 *
 */

char *GetNewObjectName(char *root);
void ShowAppEditor(void);

/*
 * Callbacks
 *
 */
void TopLevelWindowsProc(wxListBox& list, wxCommandEvent& event);

void StripExtension(char *buffer);

extern wxHelpInstance *HelpInstance;
extern wxMenu *fileMenu;

// Returns double quoted string if non-null, NULL if null.
char *SafeString(char *s);

// Returns quoted string if non-null, '' if null
char *SafeWord(char *s);

/*
 * Menu items for main project window
 *
 */

// File menu 
#define BUILD_OPEN                  100
#define BUILD_EXIT                  101
#define BUILD_SAVE                  102
#define BUILD_SET_TITLE             103
#define BUILD_NEW_PROJECT           104
#define BUILD_SAVE_AS               105
#define BUILD_SHOW_RCLOADER         106

#define BUILD_DELETE_WINDOW         110
#define BUILD_SHOW_TOPLEVEL_FRAME   111
#define BUILD_APPLICATION_SETTINGS  112

// Edit menu
#define OBJECT_EDITOR_NEW_FRAME       220
#define OBJECT_EDITOR_NEW_DIALOG      221
#define OBJECT_EDITOR_NEW_PANEL       222
#define OBJECT_EDITOR_NEW_CANVAS      223
#define OBJECT_EDITOR_NEW_TEXT_WINDOW 224
#define OBJECT_EDITOR_NEW_BUTTON      225
#define OBJECT_EDITOR_NEW_CHECKBOX    226
#define OBJECT_EDITOR_NEW_MESSAGE     227
#define OBJECT_EDITOR_NEW_CHOICE      228
#define OBJECT_EDITOR_NEW_LISTBOX     229
#define OBJECT_EDITOR_NEW_RADIOBOX    230
#define OBJECT_EDITOR_NEW_SLIDER      231
#define OBJECT_EDITOR_NEW_TEXT        232
#define OBJECT_EDITOR_NEW_MULTITEXT   233
#define OBJECT_EDITOR_NEW_GAUGE       234
#define OBJECT_EDITOR_NEW_GROUPBOX    235

#define OBJECT_EDITOR_NEW_ITEM        240
#define OBJECT_EDITOR_NEW_SUBWINDOW   241

#define OBJECT_EDITOR_EDIT_MENU       250
#define OBJECT_EDITOR_EDIT_ATTRIBUTES 251
#define OBJECT_EDITOR_CLOSE_OBJECT    252
#define OBJECT_EDITOR_DELETE_OBJECT   253
#define OBJECT_EDITOR_EDIT_TOOLBAR    254

#define OBJECT_EDITOR_TOGGLE_TEST_MODE 255

#define OBJECT_EDITOR_RC_CONVERT      260
#define OBJECT_EDITOR_RC_CONVERT_MENU 261
#define OBJECT_EDITOR_RC_CONVERT_DIALOG 262

#define OBJECT_EDITOR_GRID            263

// Zoom menu
#define ZOOM_30                       330
#define ZOOM_40                       331
#define ZOOM_50                       332
#define ZOOM_60                       333
#define ZOOM_70                       334
#define ZOOM_80                       335
#define ZOOM_90                       336
#define ZOOM_100                      337

// Generate menu
#define BUILD_GENERATE_CPP          400
#define BUILD_GENERATE_CLIPS        401
#define BUILD_GENERATE_RC           402
#define BUILD_PROJECT_SETTINGS      403
#define BUILD_GLOBAL_SETTINGS       404

// GDI objects
#define BUILD_GDI_FONTS             405

// Help menu
#define BUILD_HELP_CONTENTS         410
#define BUILD_ABOUT                 411

#define BUILD_FILE1                 500
#define BUILD_FILE2                 501
#define BUILD_FILE3                 502
#define BUILD_FILE4                 503
#define BUILD_FILE5                 504

#endif // wxbuildh
