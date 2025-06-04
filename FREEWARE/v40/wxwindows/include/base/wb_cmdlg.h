/*
 * File:	wb_cmdlg.h
 * Purpose:	Common dialogs: generic declarations
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, Julian Smart
 */

/* sccsid[] = "%W% %G%" */

#ifndef wb_cmdlgh
#define wb_cmdlgh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_setup.h"
#include "wx_gdi.h"
#include "wx_dialg.h"

/*
 * COLOUR DIALOG
 */

class WXDLLEXPORT wxColourData: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxColourData)
 public:
  wxColour dataColour;
  wxColour custColours[16];
  Bool chooseFull;

  wxColourData(void);
  ~wxColourData(void);

  inline void SetChooseFull(Bool flag) { chooseFull = flag; }
  inline Bool GetChooseFull(void) { return chooseFull; }
  inline void SetColour(wxColour& colour) { dataColour = colour; }
  inline wxColour &GetColour(void) { return dataColour; } 

  // Array of 16 custom colours
  void SetCustomColour(int i, wxColour& colour);
  wxColour GetCustomColour(int i);

  void operator=(const wxColourData& data);
};

class wxSlider;
class WXDLLEXPORT wxGenericColourDialog: public wxDialogBox
{
 DECLARE_DYNAMIC_CLASS(wxGenericColourDialog)
 protected:
  wxColourData colourData;
  wxWindow *dialogParent;

  // Area reserved for grids of colours
  wxRectangle standardColoursRect;
  wxRectangle customColoursRect;
  wxRectangle singleCustomColourRect;

  // Size of each colour rectangle
  wxIntPoint smallRectangleSize;

  // For single customizable colour
  wxIntPoint customRectangleSize;

  // Grid spacing (between rectangles)
  int gridSpacing;

  // Section spacing (between left and right halves of dialog box)
  int sectionSpacing;

  // 48 'standard' colours
  wxColour standardColours[48];

  // 16 'custom' colours
  wxColour customColours[16];

  // One single custom colour (use sliders)
  wxColour singleCustomColour;

  // Which colour is selected? An index into one of the two areas.
  int colourSelection;
  int whichKind; // 1 for standard colours, 2 for custom colours,

  wxSlider *redSlider;
  wxSlider *greenSlider;
  wxSlider *blueSlider;

  int buttonY;

  int okButtonX;
  int customButtonX;

  static Bool colourDialogCancelled;
 public:
  wxGenericColourDialog(void);
  wxGenericColourDialog(wxWindow *parent, wxColourData *data = NULL);
  ~wxGenericColourDialog(void);

  Bool Create(wxWindow *parent, wxColourData *data = NULL);

  Bool Show(Bool show);
  wxColourData& GetColourData(void) { return colourData; }

  // Internal functions
  void OnEvent(wxMouseEvent& event);
  void OnPaint(void);
  Bool OnClose(void);
  Bool OnCharHook(wxKeyEvent& event);

  virtual void CalculateMeasurements(void);
  virtual void CreateWidgets(void);
  virtual void InitializeColours(void);
  
  virtual void PaintBasicColours(void);
  virtual void PaintCustomColours(void);
  virtual void PaintCustomColour(void);
  virtual void PaintHighlight(Bool draw);

  virtual void OnBasicColourClick(int which);
  virtual void OnCustomColourClick(int which);

  virtual void OnOk(void);
  virtual void OnCancel(void);
  virtual void OnAddCustom(void);

  virtual void OnRedSlider(wxCommandEvent& event);
  virtual void OnGreenSlider(wxCommandEvent& event);
  virtual void OnBlueSlider(wxCommandEvent& event);
};

/*
 * FONT DIALOG
 */
 
class WXDLLEXPORT wxFontData: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxFontData)
 public:
  wxColour fontColour;
  Bool showHelp;
  Bool allowSymbols;
  Bool enableEffects;
  wxFont *initialFont;
  wxFont *chosenFont;
  int minSize;
  int maxSize;

  wxFontData(void);
  ~wxFontData(void);

  inline void SetAllowSymbols(Bool flag) { allowSymbols = flag; }
  inline Bool GetAllowSymbols(void) { return allowSymbols; }
  inline void SetColour(wxColour& colour) { fontColour = colour; }
  inline wxColour &GetColour(void) { return fontColour; }
  inline void SetShowHelp(Bool flag) { showHelp = flag; }
  inline Bool GetShowHelp(void) { return showHelp; }
  inline void EnableEffects(Bool flag) { enableEffects = flag; }
  inline Bool GetEnableEffects(void) { return enableEffects; }
  inline void SetInitialFont(wxFont *font) { initialFont = font; }
  inline wxFont *GetInitialFont(void) { return initialFont; }
  inline void SetChosenFont(wxFont *font) { chosenFont = font; }
  inline wxFont *GetChosenFont(void) { return chosenFont; }
  inline void SetRange(int minRange, int maxRange) { minSize = minRange; maxSize = maxRange; }

  void operator=(const wxFontData& data);
};

class wxChoice;
class wxText;
class wxCheckBox;

class WXDLLEXPORT wxGenericFontDialog: public wxDialogBox
{
 DECLARE_DYNAMIC_CLASS(wxGenericFontDialog)
 protected:
  wxFontData fontData;
  wxFont *dialogFont;
  wxWindow *dialogParent;

  // Area reserved for font display
  wxRectangle fontRect;

  wxChoice *familyChoice;
  wxChoice *styleChoice;
  wxChoice *weightChoice;
  wxChoice *colourChoice;
  wxCheckBox *underLineCheckBox;
  wxText   *pointSizeText;

  static Bool fontDialogCancelled;
 public:
 
  wxGenericFontDialog(void);
  wxGenericFontDialog(wxWindow *parent, wxFontData *data = NULL);
  ~wxGenericFontDialog(void);

  Bool Create(wxWindow *parent, wxFontData *data = NULL);

  Bool Show(Bool show);
  inline wxFontData& GetFontData(void) { return fontData; }

  // Internal functions
  void OnEvent(wxMouseEvent& event);
  void OnPaint(void);
  Bool OnClose(void);
  Bool OnCharHook(wxKeyEvent& event);

  virtual void CreateWidgets(void);
  virtual void InitializeFont(void);
  
  virtual void PaintFontBackground(void);
  virtual void PaintFont(void);

  virtual void OnOk(void);
  virtual void OnCancel(void);
  virtual void OnChangeFont(void);
};

WXDLLEXPORT char *wxFontFamilyIntToString(int family);
WXDLLEXPORT char *wxFontWeightIntToString(int weight);
WXDLLEXPORT char *wxFontStyleIntToString(int style);
WXDLLEXPORT int wxFontFamilyStringToInt(char *family);
WXDLLEXPORT int wxFontWeightStringToInt(char *weight);
WXDLLEXPORT int wxFontStyleStringToInt(char *style);

// Handy dialog functions (will be converted into classes at some point)
WXDLLEXPORT extern Constdata char *wxGetTextFromUserPromptStr;
WXDLLEXPORT extern Constdata char *wxEmptyString;
WXDLLEXPORT char *wxGetTextFromUser(Const char *message, Constdata char *caption = wxGetTextFromUserPromptStr,
                        Constdata char *default_value = wxEmptyString, wxWindow *parent = NULL,
                        int x = -1, int y = -1, Bool centre = TRUE);

#define wxCHOICE_HEIGHT 150
#define wxCHOICE_WIDTH 200

WXDLLEXPORT char *wxGetSingleChoice(Const char *message, Const char *caption,
                        int n, char *choices[], wxWindow *parent = NULL,
                        int x = -1, int y = -1, Bool centre = TRUE,
                        int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

// Same as above but gets position in list of strings, instead of string,
// or -1 if no selection
WXDLLEXPORT int wxGetSingleChoiceIndex(Const char *message, Const char *caption,
                           int n, char *choices[], wxWindow *parent = NULL,
                           int x = -1, int y = -1, Bool centre = TRUE,
                           int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

// Return client data instead
WXDLLEXPORT char *wxGetSingleChoiceData(Const char *message, Const char *caption,
                            int n, char *choices[], char *client_data[],
                            wxWindow *parent = NULL, int x = -1, int y = -1,
                            Bool centre = TRUE,
                            int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);
                           
WXDLLEXPORT int wxGetMultipleChoice(Const char *message, Const char *caption,
			  int n, char *choices[], 
			  int nsel, int * selection,
			  wxWindow *parent = NULL, int x = -1 , int y = -1, Bool centre = TRUE,
			  int width = wxCHOICE_WIDTH, int height = wxCHOICE_HEIGHT);

// type is an 'or' (|) of wxOK, wxCANCEL, wxYES_NO
// Returns wxYES/NO/OK/CANCEL

WXDLLEXPORT extern Constdata char *wxMessageBoxCaptionStr;
WXDLLEXPORT int wxbMessageBox(Const char *message, Constdata char *caption = wxMessageBoxCaptionStr, long style = wxOK|wxCENTRE,
  wxWindow *parent = NULL, int x = -1, int y = -1);

#define wxOPEN 1
#define wxSAVE 2
#define wxOVERWRITE_PROMPT 4
#define wxHIDE_READONLY 8

// Generic file load dialog
WXDLLEXPORT char * wxLoadFileSelector(Const char *what, Const char *extension, Const char *default_name = NULL, wxWindow *parent = NULL);

// Generic file save dialog
WXDLLEXPORT char * wxSaveFileSelector(Const char *what, Const char *extension, Const char *default_name = NULL, wxWindow *parent = NULL);

// File selector
WXDLLEXPORT extern Constdata char *wxFileSelectorPromptStr;
WXDLLEXPORT extern Constdata char *wxFileSelectorDefaultWildcardStr;
WXDLLEXPORT char *wxFileSelector(Constdata char *message = wxFileSelectorPromptStr, Const char *default_path = NULL,
                     Const char *default_filename = NULL, Const char *default_extension = NULL,
                     Constdata char *wildcard = wxFileSelectorDefaultWildcardStr, int flags = 0,
                     wxWindow *parent = NULL, int x = -1, int y = -1);

#endif
