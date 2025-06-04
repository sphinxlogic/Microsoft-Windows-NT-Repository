/*
 * Global Data
 *
 * RCS_ID:      $Id: wb_data.cc,v 1.3 1994/08/14 21:34:01 edz Exp $
	Last change:  JS   25 May 97   11:02 am
 */

/* static const char sccsid[] = "@(#)wb_data.cc	1.2 5/9/94"; */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if USE_TOOLBAR
#include "wx_tbar.h"
#endif

#if USE_BUTTONBAR
#include "wx_bbar.h"
#endif

#include "wx_scrol.h"

#define _MAXPATHLEN 500

// Useful buffer, initialized in wxCommonInit
char *wxBuffer = NULL;

// Windows List
wxList wxTopLevelWindows;

// List of windows pending deletion
wxList wxPendingDelete;

// Message Strings for Internationalization
char **wx_msg_str = (char**)NULL;

// Custom OS version, as optionally placed in wx.ini/.wxrc
// Currently this can be Win95, Windows, Win32s, WinNT.
// For some systems, you can't tell until run-time what services you
// have. See wxGetOsVersion, which uses this string if present.
char *wxOsVersion = NULL;

int wxPageNumber;

// GDI Object Lists
wxBrushList *wxTheBrushList = NULL;
wxPenList   *wxThePenList = NULL;
wxFontList   *wxTheFontList = NULL;
wxGDIList   *wxTheBitmapList = NULL;

wxColourDatabase *wxTheColourDatabase = NULL;

#if defined(wx_x) || (defined(wx_msw) && USE_PORTABLE_FONTS_IN_MSW)
wxFontNameDirectory wxTheFontNameDirectory;
#endif

// Stock objects
wxFont *wxNORMAL_FONT;
wxFont *wxSMALL_FONT;
wxFont *wxITALIC_FONT;
wxFont *wxSWISS_FONT;
wxPen *wxRED_PEN;

wxPen *wxCYAN_PEN;
wxPen *wxGREEN_PEN;
wxPen *wxBLACK_PEN;
wxPen *wxWHITE_PEN;
wxPen *wxTRANSPARENT_PEN;
wxPen *wxBLACK_DASHED_PEN;
wxPen *wxGREY_PEN;
wxPen *wxMEDIUM_GREY_PEN;
wxPen *wxLIGHT_GREY_PEN;

wxBrush *wxBLUE_BRUSH;
wxBrush *wxGREEN_BRUSH;
wxBrush *wxWHITE_BRUSH;
wxBrush *wxBLACK_BRUSH;
wxBrush *wxTRANSPARENT_BRUSH;
wxBrush *wxCYAN_BRUSH;
wxBrush *wxRED_BRUSH;
wxBrush *wxGREY_BRUSH;
wxBrush *wxMEDIUM_GREY_BRUSH;
wxBrush *wxLIGHT_GREY_BRUSH;

wxColour *wxBLACK;
wxColour *wxWHITE;
wxColour *wxRED;
wxColour *wxBLUE;
wxColour *wxGREEN;
wxColour *wxCYAN;
wxColour *wxLIGHT_GREY;

wxCursor *wxSTANDARD_CURSOR = NULL;
wxCursor *wxHOURGLASS_CURSOR = NULL;
wxCursor *wxCROSS_CURSOR = NULL;

// Default window names
Constdata char *wxButtonNameStr = "button";
Constdata char *wxCanvasNameStr = "canvas";
Constdata char *wxCheckBoxNameStr = "check";
Constdata char *wxChoiceNameStr = "choice";
Constdata char *wxComboBoxNameStr = "comboBox";
Constdata char *wxDialogNameStr = "dialog";
Constdata char *wxFrameNameStr = "frame";
Constdata char *wxGaugeNameStr = "gauge";
Constdata char *wxGroupBoxNameStr = "groupBox";
Constdata char *wxListBoxNameStr = "listBox";
Constdata char *wxMessageNameStr = "message";
Constdata char *wxMultiTextNameStr = "multitext";
Constdata char *wxPanelNameStr = "panel";
Constdata char *wxRadioBoxNameStr = "radioBox";
Constdata char *wxRadioButtonNameStr = "radioButton";
Constdata char *wxScrollBarNameStr = "scrollBar";
Constdata char *wxSliderNameStr = "slider";
Constdata char *wxStaticNameStr = "static";
Constdata char *wxTextWindowNameStr = "textWindow";
Constdata char *wxTextNameStr = "text";
Constdata char *wxVirtListBoxNameStr = "virtListBox";
Constdata char *wxButtonBarNameStr = "buttonbar";
Constdata char *wxEnhDialogNameStr = "Shell";
Constdata char *wxToolBarNameStr = "toolbar";
Constdata char *wxStatusLineNameStr = "status_line";
Constdata char *wxEmptyString = "";
Constdata char *wxGetTextFromUserPromptStr = "Input Text";
Constdata char *wxMessageBoxCaptionStr = "Message";
Constdata char *wxFileSelectorPromptStr = "Select a file";
Constdata char *wxFileSelectorDefaultWildcardStr = "*.*";
const char *wxInternalErrorStr = "wxWindows Internal Error";
const char *wxFatalErrorStr = "wxWindows Fatal Error";

// See wx_utils.h
const char *wxFloatToStringStr = "%.2f";
const char *wxDoubleToStringStr = "%.2f";

#ifdef wx_msw
const char *wxUserResourceStr = "TEXT";
#endif

/*
 * For wxWindows to be made into a dynamic library (e.g. Sun),
 * all IMPLEMENT_... macros must be in one place.
 */

// Hand-coded IMPLEMENT... macro for wxObject (define static data)
wxClassInfo wxObject::classwxObject("wxObject", NULL, NULL, sizeof(wxObject), NULL);
wxClassInfo *wxClassInfo::first = NULL;

#include "wx_buttn.h"
IMPLEMENT_DYNAMIC_CLASS(wxButton, wxItem)

#include "wx_panel.h"
IMPLEMENT_DYNAMIC_CLASS(wxCanvas, wxWindow)

#include "wx_check.h"
IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxItem)

#include "wx_choic.h"
IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxItem)

#if USE_CLIPBOARD
#include "wx_clipb.h"
IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxClipboardClient, wxObject)
#endif

#if USE_COMBOBOX
#include "wx_combo.h"
IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxItem)
#endif

#include "wx_dc.h"
#include "wx_dcmem.h"
IMPLEMENT_ABSTRACT_CLASS(wxDC, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxCanvasDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxMemoryDC, wxCanvasDC)
IMPLEMENT_DYNAMIC_CLASS(wxPanelDC, wxCanvasDC)
IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxCanvasDC)

#if defined(wx_msw)
IMPLEMENT_CLASS(wxPrinterDC, wxDC)
#endif

#include "wx_dialg.h"
IMPLEMENT_DYNAMIC_CLASS(wxDialogBox, wxPanel)

#include "wx_frame.h"
IMPLEMENT_DYNAMIC_CLASS(wxFrame, wxWindow)

#include "wx_cmdlg.h"
IMPLEMENT_DYNAMIC_CLASS(wxColourData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxGenericColourDialog, wxDialogBox)
IMPLEMENT_DYNAMIC_CLASS(wxFontData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxGenericFontDialog, wxDialogBox)

// X defines wxColourDialog to be wxGenericColourDialog
#ifndef wx_x
IMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialogBox)
IMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialogBox)
#endif

#if USE_FORM
#include "wx_form.h"
IMPLEMENT_DYNAMIC_CLASS(wxFormItem, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxForm, wxObject)
IMPLEMENT_CLASS(wxRealRange, wxObject)
IMPLEMENT_CLASS(wxFormItemConstraint, wxObject)
#endif

#include "wx_gdi.h"
IMPLEMENT_DYNAMIC_CLASS(wxColour, wxObject)
IMPLEMENT_CLASS(wxColourDatabase, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxFontList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxPenList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxBrushList, wxList)
IMPLEMENT_DYNAMIC_CLASS(wxGDIList, wxList)

#if (!USE_TYPEDEFS)
IMPLEMENT_DYNAMIC_CLASS(wxPoint, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxIntPoint, wxObject)
#endif

#if defined(wx_x) || (defined(wx_msw) && USE_PORTABLE_FONTS_IN_MSW)
IMPLEMENT_DYNAMIC_CLASS(wxFontNameDirectory, wxObject)
#endif

#include "wx_hash.h"
IMPLEMENT_DYNAMIC_CLASS(wxHashTable, wxObject)

#if USE_HELP
#include "wx_help.h"
IMPLEMENT_DYNAMIC_CLASS(wxHelpInstance, wxClient)
IMPLEMENT_CLASS(wxHelpConnection, wxConnection)
#endif

#include "wx_list.h"
IMPLEMENT_DYNAMIC_CLASS(wxNode, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxList, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxStringList, wxList)

#if USE_PRINTING_ARCHITECTURE
#include "wx_print.h"
IMPLEMENT_DYNAMIC_CLASS(wxPrintDialog, wxDialogBox)
IMPLEMENT_DYNAMIC_CLASS(wxPrintData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrinter, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxPrintout, wxObject)
IMPLEMENT_CLASS(wxPreviewCanvas, wxCanvas)
IMPLEMENT_CLASS(wxPreviewControlBar, wxPanel)
IMPLEMENT_CLASS(wxPreviewFrame, wxFrame)
IMPLEMENT_CLASS(wxPrintPreview, wxObject)
#if !(defined(wx_msw) && !USE_POSTSCRIPT_ARCHITECTURE_IN_MSW)
IMPLEMENT_CLASS(wxGenericPrintDialog, wxDialogBox)
IMPLEMENT_CLASS(wxGenericPrintSetupDialog, wxDialogBox)
#endif
#endif

#if USE_POSTSCRIPT
#include "wx_dcps.h"
IMPLEMENT_DYNAMIC_CLASS(wxPostScriptDC, wxDC)
IMPLEMENT_DYNAMIC_CLASS(wxPrintSetupData, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperType, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperDatabase, wxList)
#endif

#if USE_WX_RESOURCES
#include "wx_res.h"
IMPLEMENT_DYNAMIC_CLASS(wxItemResource, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxResourceTable, wxHashTable)
#endif

#include "wx_stdev.h"
IMPLEMENT_DYNAMIC_CLASS(wxCommandEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxMouseEvent, wxEvent)
IMPLEMENT_DYNAMIC_CLASS(wxKeyEvent, wxEvent)

#include "wx_sysev.h"
IMPLEMENT_ABSTRACT_CLASS(wxEvent, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxSystemEventClassStruc, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxSystemEventNameStruc, wxObject)

#include "wx_types.h"
#if USE_OLD_TYPE_SYSTEM
IMPLEMENT_DYNAMIC_CLASS(wxTypeDef, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxTypeTree, wxHashTable)
#endif

#include "wx_utils.h"
IMPLEMENT_DYNAMIC_CLASS(wxPathList, wxList)

IMPLEMENT_DYNAMIC_CLASS(wxRectangle, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxEvtHandler, wxObject)

#if USE_BUTTONBAR && USE_TOOLBAR
#include "wx_bbar.h"
IMPLEMENT_DYNAMIC_CLASS(wxButtonBar, wxToolBar)
#endif

#include "wx_split.h"
IMPLEMENT_DYNAMIC_CLASS(wxSplitterWindow, wxCanvas)

#if USE_TIMEDATE
#include "wx_date.h"
IMPLEMENT_DYNAMIC_CLASS(wxDate, wxObject)
#endif

#if USE_DOC_VIEW_ARCHITECTURE
#include "wx_doc.h"
//IMPLEMENT_ABSTRACT_CLASS(wxDocItem, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxDocument, wxEvtHandler)
IMPLEMENT_ABSTRACT_CLASS(wxView, wxEvtHandler)
IMPLEMENT_ABSTRACT_CLASS(wxDocTemplate, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxDocManager, wxEvtHandler)
IMPLEMENT_CLASS(wxDocChildFrame, wxFrame)
IMPLEMENT_CLASS(wxDocParentFrame, wxFrame)
#if USE_PRINTING_ARCHITECTURE
IMPLEMENT_DYNAMIC_CLASS(wxDocPrintout, wxPrintout)
#endif
IMPLEMENT_CLASS(wxCommand, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxCommandProcessor, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxFileHistory, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintInfo, wxObject)
#endif

#if USE_ENHANCED_DIALOG
#include "wx_enhdg.h"
IMPLEMENT_DYNAMIC_CLASS(wxEnhDialogBox, wxDialogBox)
#endif

#if USE_FRACTION
#include "wx_frac.h"
IMPLEMENT_DYNAMIC_CLASS(wxFraction, wxObject)
#endif

#if USE_CONSTRAINTS
#include "wx_lay.h"
IMPLEMENT_DYNAMIC_CLASS(wxIndividualLayoutConstraint, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxLayoutConstraints, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxSizer, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxRowColSizer, wxSizer)
#endif

#if USE_TOOLBAR
#include "wx_tbar.h"
IMPLEMENT_DYNAMIC_CLASS(wxToolBarTool, wxObject)
#ifdef wx_xview
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxCanvas)
#else
IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxPanel)
#endif
#endif

#if USE_TIMEDATE
#include "wx_time.h"
IMPLEMENT_DYNAMIC_CLASS(wxTime, wxObject)
#endif

#if !USE_GNU_WXSTRING
#include "wxstring.h"
IMPLEMENT_DYNAMIC_CLASS(wxString, wxObject)
#endif

#ifdef wx_motif
IMPLEMENT_DYNAMIC_CLASS(wxXColormap, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxXFont, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxXCursor, wxObject)
#endif
IMPLEMENT_DYNAMIC_CLASS(wxFont, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxColourMap, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPen, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxBrush, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxIcon, wxBitmap)
IMPLEMENT_DYNAMIC_CLASS(wxCursor, wxBitmap)
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)

#include "wx_group.h"
IMPLEMENT_DYNAMIC_CLASS(wxGroupBox, wxItem)

#if USE_IPC
#include "wx_ipc.h"
IMPLEMENT_ABSTRACT_CLASS(wxIPCObject, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxServer, wxIPCObject)
IMPLEMENT_DYNAMIC_CLASS(wxClient, wxIPCObject)
IMPLEMENT_CLASS(wxConnection, wxObject)
#endif

IMPLEMENT_ABSTRACT_CLASS(wxItem, wxWindow)

#include "wx_lbox.h"
IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxItem)

IMPLEMENT_DYNAMIC_CLASS(wxApp, wxObject)

#include "wx_menu.h"
IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxWindow)
IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxWindow)

#include "wx_messg.h"
IMPLEMENT_DYNAMIC_CLASS(wxMessage, wxItem)

#if USE_METAFILE
#include "wx_mf.h"
IMPLEMENT_DYNAMIC_CLASS(wxMetaFile, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxMetaFileDC, wxDC)
#endif

#include "wx_mtxt.h"
IMPLEMENT_DYNAMIC_CLASS(wxMultiText, wxText)

IMPLEMENT_DYNAMIC_CLASS(wxPanel, wxCanvas)

#include "wx_rbox.h"
IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxItem)

#if USE_RADIOBUTTON
IMPLEMENT_DYNAMIC_CLASS(wxRadioButton, wxItem)
#endif

#if USE_SCROLLBAR
IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxItem)
#endif

#include "wx_slidr.h"
IMPLEMENT_DYNAMIC_CLASS(wxSlider, wxItem)

#if USE_EXTENDED_STATICS
#include <wx_stat.h>
IMPLEMENT_CLASS(wxBasicColors, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxStaticItem, wxItem)
IMPLEMENT_CLASS(wxStaticBox, wxItem)
IMPLEMENT_CLASS(wxStaticSeparator, wxItem)
IMPLEMENT_CLASS(wxStaticBitmap, wxItem)
#endif

#include "wx_text.h"
IMPLEMENT_DYNAMIC_CLASS(wxTextWindow, wxWindow)

#include "wx_timer.h"
IMPLEMENT_DYNAMIC_CLASS(wxTimer, wxObject)

#include "wx_txt.h"
IMPLEMENT_DYNAMIC_CLASS(wxText, wxItem)

#if USE_VLBOX
#include "wx_vlbox.h"
IMPLEMENT_DYNAMIC_CLASS(wxVirtListBox, wxItem)
#endif

#include "wx_win.h"
IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxEvtHandler)

#if USE_GAUGE
#ifdef wx_motif
#include "../../contrib/xmgauge/gauge.h"
#endif
#include "wx_gauge.h"
IMPLEMENT_DYNAMIC_CLASS(wxGauge, wxItem)
#endif
