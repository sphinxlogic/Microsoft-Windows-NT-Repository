/*
 * File:	wx_cmdlg.cc
 * Purpose:	Common dialogs: MS Windows implementation
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, Julian Smart
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation "wx_cmdlg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <stdio.h>
#include "wx_defs.h"
#include "wx_gdi.h"
#include "wx_utils.h"
#include "wx_types.h"
#include "wx_frame.h"
#include "wx_dialg.h"
#include "wx_cmdlg.h"
#endif

#include "wx_privt.h"
#include "math.h"
#include "stdlib.h"

#define wxDIALOG_DEFAULT_X 300
#define wxDIALOG_DEFAULT_Y 300

/*
 * wxColourDialog
 */

wxColourDialog::wxColourDialog(void)
{
  dialogParent = NULL;
}

wxColourDialog::wxColourDialog(wxWindow *parent, wxColourData *data)
{
  Create(parent, data);
}

Bool wxColourDialog::Create(wxWindow *parent, wxColourData *data)
{
  dialogParent = parent;
  
  if (data)
    colourData = *data;
  return TRUE;
}

Bool wxColourDialog::Show(Bool flag)
{
  if (flag)
  {
    CHOOSECOLOR chooseColorStruct;
    COLORREF custColours[16];
    memset(&chooseColorStruct, 0, sizeof(CHOOSECOLOR));

    int i;
    for (i = 0; i < 16; i++)
      custColours[i] = RGB(colourData.custColours[i].Red(), colourData.custColours[i].Green(), colourData.custColours[i].Blue());

    chooseColorStruct.lStructSize = sizeof(CHOOSECOLOR);
    chooseColorStruct.hwndOwner = (dialogParent ? dialogParent->GetHWND() : NULL);
    chooseColorStruct.rgbResult = RGB(colourData.dataColour.Red(), colourData.dataColour.Green(), colourData.dataColour.Blue());
    chooseColorStruct.lpCustColors = custColours;

    chooseColorStruct.Flags = CC_RGBINIT;

    if (!colourData.GetChooseFull())
      chooseColorStruct.Flags |= CC_PREVENTFULLOPEN;

    // Do the modal dialog
    Bool success = ChooseColor(&(chooseColorStruct));

    // Try to highlight the correct window (the parent)
    HWND hWndParent = 0;
    if (GetParent())
    {
      hWndParent = GetParent()->GetHWND();
      if (hWndParent)
        ::BringWindowToTop(hWndParent);
    }


    // Restore values
    for (i = 0; i < 16; i++)
    {
      colourData.custColours[i].Set(GetRValue(custColours[i]), GetGValue(custColours[i]),
         GetBValue(custColours[i]));
    }

    colourData.dataColour.Set(GetRValue(chooseColorStruct.rgbResult), GetGValue(chooseColorStruct.rgbResult),
     GetBValue(chooseColorStruct.rgbResult));

    return success;
  }
  return TRUE;
}

/*
 * wxFontDialog
 */

void wxFillLogFont(LOGFONT *logFont, wxFont *font);
wxFont *wxCreateFontFromLogFont(LOGFONT *logFont);

wxFontDialog::wxFontDialog(void)
{
  dialogParent = NULL;
}

wxFontDialog::wxFontDialog(wxWindow *parent, wxFontData *data)
{
  Create(parent, data);
}

Bool wxFontDialog::Create(wxWindow *parent, wxFontData *data)
{
  dialogParent = parent;
  
  if (data)
    fontData = *data;
  return TRUE;
}

Bool wxFontDialog::Show(Bool flag)
{
  if (flag)
  {
    CHOOSEFONT chooseFontStruct;
    LOGFONT logFont;

    DWORD flags = CF_TTONLY | CF_SCREENFONTS | CF_NOSIMULATIONS;

    memset(&chooseFontStruct, 0, sizeof(CHOOSEFONT));

    chooseFontStruct.lStructSize = sizeof(CHOOSEFONT);
    chooseFontStruct.hwndOwner = (dialogParent ? dialogParent->GetHWND() : NULL);
    chooseFontStruct.lpLogFont = &logFont;

    if (fontData.initialFont)
    {
      flags |= CF_INITTOLOGFONTSTRUCT;
      wxFillLogFont(&logFont, fontData.initialFont);
    }

    chooseFontStruct.iPointSize = 0;
    chooseFontStruct.rgbColors = RGB((BYTE)fontData.fontColour.Red(), (BYTE)fontData.fontColour.Green(), (BYTE)fontData.fontColour.Blue());

    if (!fontData.GetAllowSymbols())
      flags |= CF_ANSIONLY;
    if (fontData.GetEnableEffects())
      flags |= CF_EFFECTS;
    if (fontData.GetShowHelp())
      flags |= CF_SHOWHELP;
    if (!(fontData.minSize == 0 && fontData.maxSize == 0))
    {
      chooseFontStruct.nSizeMin = fontData.minSize;
      chooseFontStruct.nSizeMax = fontData.maxSize;
      flags |= CF_LIMITSIZE;
    }

    chooseFontStruct.Flags = flags;
    chooseFontStruct.nFontType = SCREEN_FONTTYPE;
    Bool success = ChooseFont(&(chooseFontStruct));

    // Restore values
    if (success)
    {
      fontData.fontColour.Set(GetRValue(chooseFontStruct.rgbColors), GetGValue(chooseFontStruct.rgbColors),
       GetBValue(chooseFontStruct.rgbColors));
      fontData.chosenFont = wxCreateFontFromLogFont(&logFont);
    }

    return success;
  }
  return TRUE;
}

void wxFillLogFont(LOGFONT *logFont, wxFont *font)
{
    BYTE ff_italic;
    int ff_weight = 0;
    int ff_family = 0;
    char *ff_face = NULL;

    switch (font->GetFamily())
    {
      case wxSCRIPT:     ff_family = FF_SCRIPT ;
                         ff_face = "Script" ;
                         break ;
      case wxDECORATIVE: ff_family = FF_DECORATIVE;
                         break;
      case wxROMAN:      ff_family = FF_ROMAN;
                         ff_face = "Times New Roman" ;
                         break;
      case wxTELETYPE:
      case wxMODERN:     ff_family = FF_MODERN;
                         ff_face = "Courier New" ;
                         break;
      case wxSWISS:      ff_family = FF_SWISS;
                         ff_face = "Arial";
                         break;
      case wxDEFAULT:
      default:           ff_family = FF_SWISS;
                         ff_face = "MS Sans Serif" ; 
    }

    if (font->GetStyle() == wxITALIC || font->GetStyle() == wxSLANT)
      ff_italic = 1;
    else
      ff_italic = 0;

    if (font->GetWeight() == wxNORMAL)
      ff_weight = FW_NORMAL;
    else if (font->GetWeight() == wxLIGHT)
      ff_weight = FW_LIGHT;
    else if (font->GetWeight() == wxBOLD)
      ff_weight = FW_BOLD;

    // Have to get screen DC Caps, because a metafile will return 0.
    HDC dc2 = ::GetDC(NULL);
    int ppInch = ::GetDeviceCaps(dc2, LOGPIXELSY);
    ::ReleaseDC(NULL, dc2);

    // New behaviour: apparently ppInch varies according to
    // Large/Small Fonts setting in Windows. This messes
    // up fonts. So, set ppInch to a constant 96 dpi.
    ppInch = 96;

#if FONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    int nHeight = (font->GetPointSize()*ppInch/72);
#else
    // Correct for Windows compatibility
    int nHeight = - (font->GetPointSize()*ppInch/72);
#endif

    Bool ff_underline = font->GetUnderlined();
    
    if (font->GetFaceName())
      ff_face = font->GetFaceName();

    logFont->lfHeight = nHeight;
    logFont->lfWidth = 0;
    logFont->lfEscapement = 0;
    logFont->lfOrientation = 0;
    logFont->lfWeight = ff_weight;
    logFont->lfItalic = ff_italic;
    logFont->lfUnderline = (BYTE)ff_underline;
    logFont->lfStrikeOut = 0;
    logFont->lfCharSet = ANSI_CHARSET;
    logFont->lfOutPrecision = OUT_DEFAULT_PRECIS;
    logFont->lfClipPrecision = CLIP_DEFAULT_PRECIS;
    logFont->lfQuality = PROOF_QUALITY;
    logFont->lfPitchAndFamily = DEFAULT_PITCH | ff_family;
    strcpy(logFont->lfFaceName, ff_face);
}

wxFont *wxCreateFontFromLogFont(LOGFONT *logFont)
{
  int fontFamily = wxSWISS;
  int fontStyle = wxNORMAL;
  int fontWeight = wxNORMAL;
  int fontPoints = 10;
  Bool fontUnderline = FALSE;
  char *fontFace = NULL;

//  int lfFamily = logFont->lfPitchAndFamily & 0xF0;
  int lfFamily = logFont->lfPitchAndFamily;
  if (lfFamily & FIXED_PITCH)
    lfFamily -= FIXED_PITCH;
  if (lfFamily & VARIABLE_PITCH)
    lfFamily -= VARIABLE_PITCH;
  
  switch (lfFamily)
  {
    case FF_ROMAN:
      fontFamily = wxROMAN;
      break;
    case FF_SWISS:
      fontFamily = wxSWISS;
      break;
    case FF_SCRIPT:
      fontFamily = wxSCRIPT;
      break;
    case FF_MODERN:
      fontFamily = wxMODERN;
      break;
    case FF_DECORATIVE:
      fontFamily = wxDECORATIVE;
      break;
    default:
      fontFamily = wxSWISS;
      break;
  }
  switch (logFont->lfWeight)
  {
    case FW_LIGHT:
      fontWeight = wxLIGHT;
      break;
    case FW_NORMAL:
      fontWeight = wxNORMAL;
      break;
    case FW_BOLD:
      fontWeight = wxBOLD;
      break;
    default:
      fontWeight = wxNORMAL;
      break;
  }
  if (logFont->lfItalic)
    fontStyle = wxITALIC;
  else
    fontStyle = wxNORMAL;

  if (logFont->lfUnderline)
    fontUnderline = TRUE;
    
  if  (logFont->lfFaceName)
    fontFace = logFont->lfFaceName;
    
  HDC dc2 = ::GetDC(NULL);
  fontPoints = abs(72*logFont->lfHeight/GetDeviceCaps(dc2, LOGPIXELSY));
  ::ReleaseDC(NULL, dc2);

  return new wxFont(fontPoints, fontFamily, fontStyle, fontWeight, fontUnderline, fontFace);
}

/*
 * The following code was formerly in wx_dialg.cc.
 */


/*
 * Common dialogs
 *
 */
 
// Pop up a message box
int wxMessageBox(Const char *message, Constdata char *caption, long type,
                 wxWindow *parent, int x, int y)
{
  if ((type & wxCENTRE) == wxCENTRE)
    return wxbMessageBox(message, caption, type, parent, x, y);
    
  HWND hWnd = 0;
  if (parent) hWnd = parent->GetHWND();
  unsigned int msStyle = MB_OK;
  if (type & wxYES_NO)
  {
    if (type & wxCANCEL)
      msStyle = MB_YESNOCANCEL;
    else
      msStyle = MB_YESNO;
  }
  if (type & wxOK)
  {
    if (type & wxCANCEL)
      msStyle = MB_OKCANCEL;
    else
      msStyle = MB_OK;
  }
  if (type & wxICON_EXCLAMATION)
    msStyle |= MB_ICONEXCLAMATION;
  else if (type & wxICON_HAND)
    msStyle |= MB_ICONHAND;
  else if (type & wxICON_INFORMATION)
    msStyle |= MB_ICONINFORMATION;
  else if (type & wxICON_QUESTION)
    msStyle |= MB_ICONQUESTION;

  if (hWnd)
    msStyle |= MB_APPLMODAL;
  else
    msStyle |= MB_TASKMODAL;
    
  int msAns = MessageBox(hWnd, (LPCSTR)message, (LPCSTR)caption, msStyle);
  int ans = wxOK;
  switch (msAns)
  {
    case IDCANCEL:
      ans = wxCANCEL;
      break;
    case IDOK:
      ans = wxOK;
      break;
    case IDYES:
      ans = wxYES;
      break;
    case IDNO:
      ans = wxNO;
      break;
  }
  return ans;
}

#if USE_COMMON_DIALOGS

// Crashes: removed for present - JACS Oct 94
// DISCOVERED WHY: according to the MS Knowledge Base,
// you must put it in the EXPORTS section of the .DEF file.
// Shouldn't _EXPORT do this? Anyway, it sounds rather
// like the reported error: not being able to access
// local variable because of wrong segment setting.
// Worth a try.
// OR -- more likely -- need to do MakeProcInstance in wxFileSelector
// to make it work properly.
#if 0    		 
unsigned int APIENTRY _EXPORT
  wxFileHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if FAFA_LIB
  switch(message)
  {
        case WM_ERASEBKGND:
        {
          RECT rect;
	  HDC pDC = (HDC)wParam ;
          GetClientRect(hWnd, &rect);
          int mode = SetMapMode(pDC, MM_TEXT);
          FillRect(pDC, &rect, brushFace);
          SetMapMode(pDC, mode);
          break;
        }
#ifdef WIN32
        case WM_CTLCOLORDLG:
	{
          return (unsigned int)brushFace ;
          break;
	}
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORSTATIC:
	{
          HDC pDC = (HDC)wParam;
          SetTextColor(pDC,colorLabel) ;
          SetBkMode(pDC,TRANSPARENT) ;
          return (unsigned int)brushFace ;
          break;
	}
        case WM_CTLCOLOREDIT:
	{
          HDC pDC = (HDC)wParam;
          SetTextColor(pDC,colorLabel) ;
          return NULL ;
          break;
	}
#else
        case WM_CTLCOLOR:
        {
          int nCtlColor = (int)HIWORD(lParam);
          HDC pDC = (HDC)wParam;
	  switch(nCtlColor)
          {
          case CTLCOLOR_DLG:
            return (unsigned int)brushFace ;
          break ;
	  case CTLCOLOR_BTN:
	  case CTLCOLOR_STATIC:
	    // BUGBUG For some reason it crashes at this point,
	    // so I'm commenting out this whole hook.
            SetTextColor(pDC,colorLabel) ;
            SetBkMode(pDC,TRANSPARENT) ;
            return (unsigned int)brushFace ;
          break ;
	  case CTLCOLOR_EDIT:
            SetTextColor(pDC,colorLabel) ;
            return NULL ;
          break ;
          }
          break;
        }
#endif
  }
#endif
  return(FALSE) ; // Pass non processed messages to standard dialog box fct.
}
#endif
#endif
  // end USE_COMMON_DIALOGS

char *wxFileSelector(Constdata char *message,
                     Const char *default_path, Const char *default_filename, 
                     Const char *default_extension, Constdata char *wildcard, int flags,
                     wxWindow *parent, int x, int y)
{
#if !USE_COMMON_DIALOGS
  // Great file selector folks !!!
  return wxGetTextFromUser(message, "Enter filename", NULL, parent, x, y);
#else
  // Use extended version (below) NO!! This function is very buggy. (WAS very buggy.)
//  return wxFileSelectorEx(message, default_path, default_filename,
//        NULL, wildcard, flags, parent, x, y);

  if (x < 0) x = wxDIALOG_DEFAULT_X;
  if (y < 0) y = wxDIALOG_DEFAULT_Y;

  wxWnd *wnd = NULL;
  HWND hwnd = NULL;
  if (parent)
  {
    wnd = (wxWnd *)parent->handle;
    hwnd = wnd->handle;
  }
  static char file_buffer[400];

  if (default_filename)
    strcpy(file_buffer, default_filename);
  else file_buffer[0] = 0;

  char title_buffer[80];
  title_buffer[0] = 0;

  char filter_buffer[200];

  char *theWildCard = (char *)wildcard;
  if (!theWildCard)
    theWildCard = "*.*";

/* Alejandro Sierra's wildcard modification

In wxFileSelector you can put, instead of a single wild_card, pairs of
strings separated by '|'. The first string is a description, and the
second is the wild card. You can put any number of pairs.

eg.  "description1 (*.ex1)|*.ex1|description2 (*.ex2)|*.ex2"

If you put a single wild card, it works as before my modification.
*/

// Here begin my changes (Alex)              ******************************
  if (theWildCard)                               
  {
         if (!strchr(theWildCard, '|'))         // No '|'s, I leave it as it was
                sprintf(filter_buffer, "Files (%s)|%s",theWildCard, theWildCard);
         else
                strcpy(filter_buffer, theWildCard);

         int len = strlen(filter_buffer);

         int i;
         for (i = 0; i < len; i++)
                if (filter_buffer[i]=='|')
                  filter_buffer[i] = '\0';

         filter_buffer[len+1] = '\0';

  }
  OPENFILENAME of;
  memset(&of, 0, sizeof(OPENFILENAME));

  of.lStructSize = sizeof(OPENFILENAME);
  of.hwndOwner = hwnd;

  if (wildcard)
  {
    of.lpstrFilter = (LPSTR)filter_buffer;
    of.nFilterIndex = 1L;
  }
  else
  {
    of.lpstrFilter = NULL;
    of.nFilterIndex = 0L;
  }
  of.lpstrCustomFilter = NULL;
  of.nMaxCustFilter = 0L;
  of.lpstrFile = file_buffer;
  of.nMaxFile = 400;
  of.lpstrFileTitle = title_buffer;
  of.nMaxFileTitle = 50;
  of.lpstrInitialDir = default_path;
  of.lpstrTitle = message;
  of.nFileOffset = 0;
  of.nFileExtension = 0;
  of.lpstrDefExt = (char *)default_extension;

  long msw_flags = 0;

  if (flags & wxOVERWRITE_PROMPT)
    msw_flags |= OFN_OVERWRITEPROMPT;
  if (flags & wxHIDE_READONLY)
    msw_flags |= OFN_HIDEREADONLY;
  of.Flags = msw_flags;
//  of.Flags = msw_flags|OFN_ENABLEHOOK;
//  of.lpfnHook = wxFileHook ;

  Bool success;
  if (flags & wxSAVE)
    success = GetSaveFileName(&of);
  else
    success = GetOpenFileName(&of);

  DWORD error = CommDlgExtendedError();
  if (success)
  {
    return file_buffer;
  }
  else
    return NULL;
#endif
  // End USE_COMMON_DIALOGS
}

#if USE_COMMON_DIALOGS
/*
    wxFileSelector  modification (only for MS-Windows (yet) ) :


    char* wxFileSelectorEx( char* 	  title,               // = "Select a file"
						    char* 	  defaultDir,          // = NULL
						    char* 	  defaultFileName,     // = NULL
						    int*      defaultFilterIndex,  // = NULL
						    char*     filter,              // = "*.*"
                            int       flags,               // = 0
                            wxWindow* parent,              // = NULL
                            int       x,                   // = -1
                            int       y )                  // = -1

    wxFileSelectorEx is as wxFileSelector with little changes:

    The defaultFilterIndex is changed to int*,
    so You can get the selected filter.

    To the filename is automaticely added the selected
    extension if You type only the filename in the dialogbox.

    eg.  if filter is "Data (*.dat)|*.dat|Text (*.txt)|*.txt|All (*.*)|*.*"

    and you have selected: *.txt
    and you type 'hello' into the Filedialog,
    the filename is expanded to hello.txt.
    If you want explicitely save or load 'hello' as
    'hello' and not as 'hello.txt', You have to select *.*,
    or type 'hello.'  .

    So I use it in my Application:

void Open( void )
{
# ifdef wx_msw
    char tempList[] = "Daten (*.dat)|*.dat|Texte (*.txt)|*.txt|Alle (*.*)|*.*";
# else
    char tempList[] = "*.dat";  // <<== This also works under wx_msw  
# endif                                 

    static int fileDialogIndex = 1;

    char* tempName = wxFileSelector( "Open file",
                                     NULL,
                                     NULL,
                                     &fileDialogIndex,
                                     tempList,
                                     wxOPEN | wxHIDE_READONLY,
                                     NULL,
                                     20,
                                     20 );
}

Regards,
         Alfred

*/

# if __BORLANDC__
#   include <dir.h>  // for MAXPATH etc. ( Borland 3.1 ) 
# endif

# ifndef MAXPATH
# define MAXPATH   400   
# endif

# ifndef MAXDRIVE
# define MAXDRIVE  3
# endif

# ifndef MAXFILE 
# define MAXFILE   9
# endif

# ifndef MAXEXT
# define MAXEXT    5
# endif


char* wxFileSelectorEx( Constdata char* 	  title,               // = "Select a file"
			Const char* 	  defaultDir,          // = NULL
			Const char* 	  defaultFileName,     // = NULL
			int*      defaultFilterIndex,  // = NULL
			Constdata char*     filter,              // = "*.*"
                        int       flags,               // = 0
                        wxWindow* parent,              // = NULL
                        int       x,                   // = -1
                        int       y )                  // = -1

{
    if ( x < 0 ) { x = wxDIALOG_DEFAULT_X; }
    if ( y < 0 ) { y = wxDIALOG_DEFAULT_Y; }

    HWND  HWindow = ( parent ) ? ((wxWnd *)parent->handle)->handle : NULL;

	static char  fileNameBuffer [ MAXPATH ];		   // the file-name
	char         titleBuffer    [ MAXFILE+1+MAXEXT ];  // the file-name, without path

	*fileNameBuffer = '\0';
    *titleBuffer    = '\0';

	char* filterBuffer = NULL;
	char* extension    = NULL;
	char* theFilter    = (char *)filter;

    long msw_flags = 0;
    if ( flags & wxHIDE_READONLY ) { msw_flags |= OFN_HIDEREADONLY; }


	OPENFILENAME of;
	memset(&of, 0, sizeof(OPENFILENAME));

    of.lpstrCustomFilter = NULL;   // system should not save custom filter
	of.nMaxCustFilter    = 0L;

	of.nFileOffset       = 0;      // 0-based pointer to filname in lpstFile
	of.nFileExtension    = 0;      // 0-based pointer to extension in lpstrFile
	of.lpstrDefExt       = NULL;   // no default extension

	of.lStructSize 		 = sizeof(OPENFILENAME);
	of.hwndOwner 		 = HWindow;
	of.lpstrTitle        = title;


	of.lpstrFileTitle    = titleBuffer;
	of.nMaxFileTitle     = MAXFILE + 1 + MAXEXT;	// Windows 3.0 and 3.1

	of.lpstrInitialDir   = defaultDir;

	of.Flags 			 = msw_flags;



    //=== Like Alejandro Sierra's wildcard modification >>===================
    /*
        In wxFileSelector you can put, instead of a single wild_card,
        pairs of strings separated by '|'.
        The first string is a description, and the
        second is the wild card. You can put any number of pairs.

        eg.  "description1 (*.ex1)|*.ex1|description2 (*.ex2)|*.ex2"

        If you put a single wild card, it works as before the modification.
    */
    //=======================================================================

	if ( !theFilter ) theFilter = "*.*";

	int filterBufferLen = 0;

	if ( !strchr( theFilter, '|' ) ) { 			// only one filter ==> default text:
		char buffText[] = "Files (%s)|%s";
		filterBufferLen = strlen( theFilter )*2 + strlen( buffText ) -4;
		filterBuffer    = new char[ filterBufferLen +2 ];

		if ( filterBuffer ) {
			sprintf( filterBuffer, buffText, theFilter, theFilter );
		}
	}
	else {                  					// more then one filter
		filterBufferLen = strlen( theFilter );
		filterBuffer    = new char[ filterBufferLen +2 ];

		if ( filterBuffer ) {
			strcpy( filterBuffer, theFilter );
	    }
	}

    if ( filterBuffer ) {  					    // Substituting '|' with '\0'
		for ( int i = 0; i < filterBufferLen; i++ ) {
			if ( filterBuffer[i] == '|' ) { filterBuffer[i] = '\0'; }
		}
	}

	filterBuffer[filterBufferLen+1] = '\0';

	of.lpstrFilter  = (LPSTR)filterBuffer;
	of.nFilterIndex = (defaultFilterIndex) ? *defaultFilterIndex : 1;



    //=== Setting defaultFileName >>=========================================

	if ( defaultFileName ) {
        strncpy( fileNameBuffer, defaultFileName, MAXPATH-1 );
	    fileNameBuffer[ MAXPATH-1 ] = '\0';
    }

	of.lpstrFile = fileNameBuffer;  // holds returned filename
	of.nMaxFile  = MAXPATH;




    //== Execute FileDialog >>=================================================

    Bool success = (flags & wxSAVE) ? GetSaveFileName(&of) : GetOpenFileName(&of);

    char* returnFileName = NULL;

    if ( success )
    {
        //=== Adding the correct extension >>=================================

        if ( defaultFilterIndex ) {
            *defaultFilterIndex = (int)of.nFilterIndex;
        }

	    if ( of.nFileExtension && fileNameBuffer[ of.nFileExtension-1] != '.' )
        {                                    // user has typed an filename
                                             // without an extension:

	        int   maxFilter = (int)(of.nFilterIndex*2L-1L);
	        extension       = filterBuffer;

	        for( int i = 0; i < maxFilter; i++ ) {		    // get extension
		        extension = extension + strlen( extension ) +1;
	        }

	        if (  (extension = strrchr( extension, '.' ))   // != "blabla" 
			      && !strrchr( extension, '*' )             // != "blabla.*"
			      && !strrchr( extension, '?' )             // != "blabla.?"
			      && extension[1]                           // != "blabla."
			      && extension[1] != ' ' )                  // != "blabla. "
	        {
                              // now concat extension to the fileName:

                int len = strlen( fileNameBuffer );
	            strncpy( fileNameBuffer + len, extension, MAXPATH - len );
                fileNameBuffer[ MAXPATH -1 ] = '\0';
            }
	    }

        returnFileName = fileNameBuffer;


	    //=== Simulating the wxOVERWRITE_PROMPT >>============================

	    if ( (flags & wxOVERWRITE_PROMPT) && ::FileExists( fileNameBuffer ) )
        {
		    char  questionText[] = "Replace file\n%s\n?";
		    char* messageText    = new char[strlen(questionText)+strlen(fileNameBuffer)-1];
		    sprintf( messageText, questionText, fileNameBuffer );

		    if ( messageText && ( wxMessageBox( (Const char *)messageText, title, wxYES_NO ) != wxYES ) )
            {
                returnFileName = NULL;
		    }

		    delete[] messageText;
	    }

    } // END: if ( success )


    delete[] filterBuffer;

    return returnFileName;
}
#endif

