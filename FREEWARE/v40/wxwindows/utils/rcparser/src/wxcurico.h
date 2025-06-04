#ifndef wx_msw
  #error This code compiles only on the MS Windows platform!
#endif

#include   "wx_gdi.h"
#include   "curico.h"   


// Julian! The classes wxIconEx and wxCursorEx are just for demonstrative
// purposes, because I do not know, what you intend to do with the code.
// You could add there the usual constructors from wxIcon / wxCursor, i.e.
//  using resource name (or stock cursor id), but then you need to face
//  problem with selecting destructor (because mine use char * parameter, too).
// Or you might put this new code into original wxIcon / wxCursor classes,
//  but then you need to solve problem that HICON / HCURSOR resources got
//  from the CURICO library needs to be destroyed using DestroyIcon /
//  DestroyCursor, but not those got by LoadXXXX()! Perhaps a flag has to
//  be added in this setup.
// Or you might stay with these classes and use specifically them in your
//  wxBuilder code.
// Another nasty problem are the stupid base-class ctrs in ctrs for wxIconEx
//  and wxCursorEx. They are needed, because there is no-action default ctr!
//  Put in other words, the wxIcon and wxCursor classes are virtually non-
//  extensible! Possible solution is to add a do-nothing default ctrs to these
//  classes. These base-class-ctrs below rely on the non-functionality of
//  the ctrs used in the MS-W implementation!

class wxIconEx: public wxIcon
{
    public:
  wxIconEx( char *szIcoFileName)
  {
    ms_icon = ReadIconFile( szIcoFileName, wxhInstance, NULL, NULL);
  }

  wxIconEx( char *szCurFileName, int WXUNUSED(iDummy))
  {
    ms_icon = CursorToIcon( szCurFileName, wxhInstance);
  }

 ~wxIconEx()
 {
   if(ms_icon != 0)
     DestroyIcon( ms_icon);
 }
};

class wxCursorEx: public wxCursor
{
    public:
  wxCursorEx( char *szCurFileName) : wxCursor( szCurFileName, 0, 0)
  {
    ms_cursor = ReadCursorFile( szCurFileName, wxhInstance);
  }

  wxCursorEx( char *szIcoFileName, int XHot, int YHot) :
    wxCursor( szIcoFileName, 0, 0)
  {
    ms_cursor = IconToCursor( szIcoFileName, wxhInstance, XHot, YHot);
  }


 ~wxCursorEx()
 {
   if(ms_cursor != 0)
     DestroyCursor( ms_cursor);
 }
};

// this is a sort of gift to you: you may generate pixmap definitions
// or even pixmap files on the fly. But I never tried this too much!
// Parameter fIsIcon is TRUE for .ICO file and FALSE for .CUR file.
// cData must be pre-allocated in reasonable size (width * height / 8) bytes!
// returns TRUE on success.

BOOL fGetXPixmap( BOOL fIsIcon, char *szIcoFileName, HINSTANCE hInst,
                  char cData[], int &width, int &height);
