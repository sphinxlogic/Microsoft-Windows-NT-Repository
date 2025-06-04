/*
 * File:	wx_dcps.h
 * Purpose:	PostScript device context
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
	Last change:  JS   28 May 97   11:42 am
 */

/* sccsid[] = "@(#)wx_dcps.h	1.2 5/9/94" */


#ifndef wx_dcpsh
#define wx_dcpsh

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_dc.h"
#if USE_POSTSCRIPT

#ifdef IN_CPROTO
typedef       void    *wxPostScriptDC ;
#else

class WXDLLEXPORT wxPostScriptDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxPostScriptDC)

 public:
  int yorigin;          // For EPS
  ofstream *pstream;    // PostScript output stream
  char *filename;
  unsigned char currentRed;
  unsigned char currentGreen;
  unsigned char currentBlue;

  // Create a printer DC
  wxPostScriptDC(void);
  wxPostScriptDC(char *output, Bool interactive = TRUE, wxWindow *parent = NULL);

  ~wxPostScriptDC(void);

  Bool Create(char *output, Bool interactive = TRUE, wxWindow *parent = NULL);

  virtual Bool PrinterDialog(wxWindow *parent = NULL);

  inline virtual void BeginDrawing(void) {} ;
  inline virtual void EndDrawing(void) {} ;

  void FloodFill(float x1, float y1, wxColour *col, int style=wxFLOOD_SURFACE) ;
  Bool GetPixel(float x1, float y1, wxColour *col) ;

  void DrawLine(float x1, float y1, float x2, float y2);
  void IntDrawLine(int x1, int y1, int x2, int y2);
  void CrossHair(int x, int y) ;
  void DrawArc(float x1,float y1,float x2,float y2,float xc,float yc);
  void DrawEllipticArc(float x,float y,float w,float h,float sa,float ea);
  void DrawPoint(float x, float y);
  // Avoid compiler warning
  void DrawPoint(wxPoint& point) { wxbDC::DrawPoint(point); }
  void DrawLines(int n, wxPoint points[], float xoffset = 0, float yoffset = 0);
  void DrawLines(int n, wxIntPoint points[], int xoffset = 0, int yoffset = 0);
  // Avoid compiler warning
  void DrawLines(wxList *lines, float xoffset = 0, float yoffset = 0)
  { wxbDC::DrawLines(lines, xoffset, yoffset); }
  void DrawPolygon(int n, wxPoint points[], float xoffset = 0, float yoffset = 0, int fillStyle=wxODDEVEN_RULE);
  // Avoid compiler warning
  void DrawPolygon(wxList *lines, float xoffset = 0, float yoffset = 0, int fillStyle=wxODDEVEN_RULE)
  { wxbDC::DrawPolygon(lines, xoffset, yoffset, fillStyle); }
  void DrawRectangle(float x, float y, float width, float height);
  void DrawRoundedRectangle(float x, float y, float width, float height, float radius = 20);
  void DrawEllipse(float x, float y, float width, float height);
  // Splines
  // 3-point spline
  void DrawSpline(float x1, float y1, float x2, float y2, float x3, float y3);
  // Any number of control points - a list of pointers to wxPoints
  void DrawSpline(wxList *points);
  void DrawSpline(int n, wxPoint points[]);
  void DrawIcon(wxIcon *icon, float x, float y);
  void DrawText(const char *text, float x, float y, Bool use16 = FALSE);

  void Clear(void);
  void SetFont(wxFont *font);
  void SetPen(wxPen *pen);
  void SetBrush(wxBrush *brush);
  void SetLogicalFunction(int function);
  void SetBackground(wxBrush *brush);
  void SetClippingRegion(float x, float y, float width, float height);
  void DestroyClippingRegion(void);

  Bool StartDoc(char *message);
  void EndDoc(void);
  void StartPage(void);
  void EndPage(void);

  float GetCharHeight(void);
  float GetCharWidth(void);
  void GetTextExtent(const char *string, float *x, float *y,
                     float *descent = NULL, float *externalLeading = NULL,
                     wxFont *theFont = NULL, Bool use16 = FALSE);
  virtual void SetLogicalOrigin(float x, float y);
//  virtual void SetLogicalScale(float x, float y);
  virtual void CalcBoundingBox(float x, float y);

  void SetMapMode(int mode);
  void SetUserScale(float x, float y);
  float DeviceToLogicalX(int x);
  float DeviceToLogicalY(int y);
  float DeviceToLogicalXRel(int x);
  float DeviceToLogicalYRel(int y);
  int LogicalToDeviceX(float x);
  int LogicalToDeviceY(float y);
  int LogicalToDeviceXRel(float x);
  int LogicalToDeviceYRel(float y);
  Bool Blit(float xdest, float ydest, float width, float height,
            wxCanvasDC *source, float xsrc, float ysrc, int rop = wxCOPY, Bool useMask = FALSE);
  inline Bool CanGetTextExtent(void) { return FALSE; }
  inline Bool CanDrawBitmap(void) { return FALSE; }

  void GetSize(float *width, float *height);
  void GetSizeMM(float *width, float *height);

  inline void SetBackgroundMode(int WXUNUSED(mode)) {};
  inline void SetColourMap(wxColourMap *WXUNUSED(cmap)) {}
  
  inline ostream *GetStream(void) { return pstream; }

  void SetupCTM();
  float scale_factor;
};

// Print Orientation (Should also add Left, Right)
enum {
  PS_PORTRAIT,
  PS_LANDSCAPE
};// ps_orientation = PS_PORTRAIT;

// Print Actions
enum {
  PS_PRINTER,
  PS_FILE,
  PS_PREVIEW
};// ps_action = PS_PREVIEW;

// PostScript printer settings
void wxSetPrinterCommand(char *cmd);
void wxSetPrintPreviewCommand(char *cmd);
void wxSetPrinterOptions(char *flags);
void wxSetPrinterOrientation(int orientation);
void wxSetPrinterScaling(float x, float y);
void wxSetPrinterTranslation(float x, float y);
void wxSetPrinterMode(int mode);
void wxSetPrinterFile(char *f);
void wxSetAFMPath(char *f);

// Get current values
char *wxGetPrinterCommand(void);
char *wxGetPrintPreviewCommand(void);
char *wxGetPrinterOptions(void);
Bool wxGetPrinterOrientation(void);
void wxGetPrinterScaling(float *x, float *y);
void wxGetPrinterTranslation(float *x, float *y);
int wxGetPrinterMode(void);
char *wxGetPrinterFile(void);
char *wxGetAFMPath(void);

/*
 * PostScript print setup information
 */

class WXDLLEXPORT wxPrintSetupData: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPrintSetupData)

 public:
  char *printerCommand;
  char *previewCommand;
  char *printerFlags;
  char *printerFile;
  int printerOrient;
  float printerScaleX;
  float printerScaleY;
  float printerTranslateX;
  float printerTranslateY;
  // 1 = Preview, 2 = print to file, 3 = send to printer
  int printerMode;
  char *afmPath;
  // A name in the paper database (see wx_print.h: the printing framework)
  char *paperName;
  Bool printColour;
 public:
  wxPrintSetupData(void);
  ~wxPrintSetupData(void);

  void SetPrinterCommand(char *cmd);
  void SetPaperName(char *paper);
  void SetPrintPreviewCommand(char *cmd);
  void SetPrinterOptions(char *flags);
  void SetPrinterFile(char *f);
  void SetPrinterOrientation(int orient);
  void SetPrinterScaling(float x, float y);
  void SetPrinterTranslation(float x, float y);
  // 1 = Preview, 2 = print to file, 3 = send to printer
  void SetPrinterMode(int mode);
  void SetAFMPath(char *f);
  void SetColour(Bool col);

  // Get current values
  char *GetPrinterCommand(void);
  char *GetPrintPreviewCommand(void);
  char *GetPrinterOptions(void);
  char *GetPrinterFile(void);
  char *GetPaperName(void);
  int GetPrinterOrientation(void);
  void GetPrinterScaling(float *x, float *y);
  void GetPrinterTranslation(float *x, float *y);
  int GetPrinterMode(void);
  char *GetAFMPath(void);
  Bool GetColour(void);

  void operator=(wxPrintSetupData& data);
};

extern wxPrintSetupData *wxThePrintSetupData;
WXDLLEXPORT extern void wxInitializePrintSetupData(Bool init = TRUE);


/*
 * Again, this only really needed for non-Windows platforms
 * or if you want to test the PostScript printing under Windows.
 */

class WXDLLEXPORT wxPrintPaperType: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPrintPaperType)

 public:
  int widthMM;
  int heightMM;
  int widthPixels;
  int heightPixels;
  char *pageName;

  wxPrintPaperType(char *name = NULL, int wmm = 0, int hmm = 0, int wp = 0, int hp = 0);
  ~wxPrintPaperType(void);
};

class WXDLLEXPORT wxPrintPaperDatabase: public wxList
{
  DECLARE_DYNAMIC_CLASS(wxPrintPaperDatabase)

 public:
  wxPrintPaperDatabase(void);
  ~wxPrintPaperDatabase(void);

  void CreateDatabase(void);
  void ClearDatabase(void);

  void AddPaperType(char *name, int wmm, int hmm, int wp, int hp);
  wxPrintPaperType *FindPaperType(char *name);
};

extern wxPrintPaperDatabase *wxThePrintPaperDatabase;

#endif // IN_CPROTO
#endif // USE_POSTSCRIPT
#endif // wx_dcpsh

