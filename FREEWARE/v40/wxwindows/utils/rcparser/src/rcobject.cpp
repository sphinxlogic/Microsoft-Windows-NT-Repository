/* ***************************************************************************
 * RCOBJECT.CC, written by Petr Smilauer <petrsm@entu.cas.cz> as a
 * contribution to the wxWin library
 *
 * This module represents the core of the library. The other one (RCPARSER.CC)
 * is not dependent on things like format of RC files or MS Windows
 *
 * Purpose of the library is to parse Microsoft Windows resource files in the
 * source format (.RC files, as opposed to .RES files) and represent them as a
 * linked hierarchy of objects. At the top (or bottom - depends on your
 * perspective :-) is the rcFile instance, representing whole resource file
 * for a given project. This contains information about dialog boxes and
 * menus specifications included in the resource file. Menu-describing classes
 * are recursive in style using rcMenuPopup as a node and rcMenuItem as a
 * terminal leaf. rcMenuPopup inherits from rcMenuItem. For dialog boxes
 * (represented as instances of rcDialog class), the contained controls are
 * instances of rcItem class, with information about their caption, position,
 * size and style (with MS Windows coding). Note that the units are the dialog
 * units, not pixels.
 *************************************************************************** */

 /* *************************************************************************
    First release ("early alpha" ;-):   1 March 1994   P.S.

    Modified/Hacked by E. Zimmermann <edz@crg.net>


 *  ************************************************************************ */

static char RCS_id[] = "$Id: rcobject.cc,v 1.4 1994/08/22 21:20:10 edz Exp edz $";

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <wx_defs.h>
#include <wx_utils.h>
#include <wx_list.h>
#include <wx_hash.h>
#include  <wx_obj.h>

#include "rcobject.h"
#include "rcparser.h"

// Local global (:-) variable:1->does not warn,uses value -2, collects symbol
int  nSymbolFaultPolicy = 0;  

#ifdef wx_x
# ifndef BOOL
#  define BOOL Bool
# endif
# define stricmp strcasecmp
extern "C" int stricmp(const char *, const char *);

/* From <windows.h> */
#define BS_PUSHBUTTON	    0x00000000L
#define BS_DEFPUSHBUTTON    0x00000001L
#define BS_CHECKBOX	    0x00000002L
#define BS_AUTOCHECKBOX     0x00000003L
#define BS_RADIOBUTTON	    0x00000004L
#define BS_3STATE	    0x00000005L
#define BS_AUTO3STATE	    0x00000006L
#define BS_GROUPBOX	    0x00000007L
#define BS_USERBUTTON	    0x00000008L
#define BS_AUTORADIOBUTTON  0x00000009L
#define BS_OWNERDRAW	    0x0000000BL

#define SS_LEFT 	    0x00000000L
#define SS_CENTER	    0x00000001L
#define SS_RIGHT	    0x00000002L
#define SS_ICON 	    0x00000003L
#define SS_SIMPLE	    0x0000000BL

#define WS_OVERLAPPED	    0x00000000L
#define WS_POPUP	    0x80000000L
#define WS_CHILD	    0x40000000L

#define WS_BORDER	    0x00800000L
#define WS_SYSMENU	    0x00080000L

#define WS_GROUP	    0x00020000L
#define WS_TABSTOP	    0x00010000L

#define CBS_SIMPLE	    0x0001L
#define LBS_NOTIFY	    0x0001L
#define ES_LEFT 	    0x00000000L
#define SBS_HORZ            0x0000L

#endif

IMPLEMENT_CLASS(rcObject, wxObject)
IMPLEMENT_CLASS(rcItem, rcObject)
IMPLEMENT_CLASS(rcDialog, rcObject)
IMPLEMENT_CLASS(rcMenuItem, wxObject)
IMPLEMENT_CLASS(rcMenuPopup, rcMenuItem)
IMPLEMENT_CLASS(rcFile, wxObject)

// ...::szItemType() gives string representation of control type!

const char *rcItem::szItemType( void)
{
  static const char *szType[] = {
   // This must MATCH the enums in the header
   "CheckBox",             "ComboBox",
   "CenterText",           "DefPushButton",
   "EditText",             "GroupBox",
   "Icon",                 "ListBox",
   "LeftText",             "PushButton",
   "RadioButton",          "RightText",
   "ScrollBar",            "Static"
  };

  return (type <= rcSTATIC) ? szType[(int)type] : "Wild!";
}

// menu item ctr
rcMenuItem::rcMenuItem( const char *szName, int Id, RCMENUOPTION rcopt,
                        const char *szIDname)
{
  szLabel = copystring(szName ? szName : " ");
  ID      = Id;
  options = rcopt;
  szID    = szIDname ? copystring(szIDname) : NULL;
}

// dtr
rcMenuItem::~rcMenuItem()
{
  if(szLabel != 0) delete []szLabel;
  if(szID != 0)    delete []szID;
}

// retrieve specified child (either item or popup)
rcMenuItem * rcMenuPopup::rcGetIthChild( int i)
{
  return ( i < 0 || i >= N) ? NULL :
    (rcMenuItem *)((wxlChilds.Nth( i))->Data());
}

// append specified child
Bool rcMenuPopup::AddChild( rcMenuItem *child)
{
  if( wxlChilds.Append( (wxObject *)child) != 0) {
    ++N;
    return 1;
  }
  return 0;
}

// dtr
rcMenuPopup::~rcMenuPopup()
{
  if(N > 0) {
    // delete contained items
    for (wxNode *node = wxlChilds.First(); node; node = wxlChilds.First()) {
      delete (rcMenuItem *)node->Data(); // should call proper virt. dtr
      delete node;
    }	// for()
  }
}


// dtr
rcDialog::~rcDialog()
{
  if(szName)
    delete []szName;
  if(N > 0) {
    for (wxNode *node = wxlItems.First(); node; node= wxlItems.First()) {
      delete (rcItem *)node->Data();
      delete node;
    }	// for()
  }
}

// non/parsing context warning: no access to file name and line #
static void includeContextWarning(const char *filename)
{
  if(pmfWarning != NULL) {
    char szBuf[256]; // @@ MAXPATH ?
    sprintf( szBuf, "Cannot open '%s'", filename);
    pmfWarning( "RCOBJECT:pfTryOpen()", szBuf); // ParsedFile ctr!
  }
}

static char *top_dir; // Hack, Should rewrite the classes

// Attempts to open specified file, event. looking for it in the specified
// include paths....
static FILE * pfTryOpen( const char *szFileName, char *szParserFlags)
{ ParsedString  psOptions( szParserFlags, " \t\n"); // split options
  int           N = psOptions.Segments(),     // how many are there?
                i;
  FILE         *fpTry;

  // First try absolute path
  if( (fpTry = fopen( szFileName, "rt")) != 0)  // OK!
    return fpTry;

  if (top_dir) {
    char *sz = new char [strlen(top_dir) + strlen(szFileName) + 2];
    strcpy(sz, top_dir);
    strcat(sz, "\\");
    strcat(sz, szFileName);
#ifndef wx_msw
    Dos2UnixFilename(sz);
#endif
    fpTry = fopen(sz, "rt");
    delete []sz;
    if (fpTry) return fpTry; // Found it
  }

  for( i = 0 ; i < N ; ++i) {
    char *s = psOptions.GetSegment( i); // try each option
    if(s[0] == '-' && (s[1] == 'i' || s[1] == 'I')) {
      s += 2; // Skip -I
      // this is an include directory option
      int   iLen = strlen( s );
      char *sz = new char [iLen + strlen( szFileName) + 2];

      strcpy( sz, s );
      if( (sz[iLen - 1] != '\\') && (sz[iLen - 1] != '/')
#ifdef wx_msw
          && (! ((iLen == 2) && (sz[1] == ':')))
#endif
          )
      //this saves us appending backslash to -Id: or so
        strcat( sz, "\\");
      strcat( sz, szFileName);  // try to look in that directory
#ifndef wx_msw
     Dos2UnixFilename(sz); 
#endif
      fpTry = fopen( sz, "rt");
      delete []sz;
      if(fpTry != 0)  // we succeeded...
        return fpTry;
    }
  }
  includeContextWarning(szFileName);
  return 0;
}

static char *szIncludeName( char *str)
// we could parse in-place, using the str parameter
{
  char  *s;
  int    iLen;

  strtok( str, " \t");

  s = strtok( NULL, " \t\n");
  if(*s == '<') {
    // this is the #include <XXX.H> form
    iLen = strlen( ++s) - 1;
    // now, I suppose that s[iLen] is equal to '>', so I suppress this
    s[iLen] = '\0';
  } else if(*s == '"') {
   // this is the #include "YYY.H" form
    iLen = strlen( ++s) - 1;
    s[iLen] = '\0';
  }
  return s;         // if none of the above, return original string
}

// are parentheses in the string?
static inline int  fHasParentheses( const char *s)
{
  return (strchr( s, '(') != 0);
}

// Is the string a long representation (event.hexadecimaly represented)
static int  fIsLong( const char *s)
{
  static const char szOK[] = "0123456789ABCDEFabcdefXxL+-";  // only these admissible

  if( !isdigit( *s) && *s != '-' && *s != '+') // must be digit or sign
    return 0;
  while( *s ) {
    if( strchr( szOK, *s++) == NULL)  // we found disallowed character...
      return 0;
  }
  return 1;
}

// smart convertor: translates constants as well as symbolid values from
// symbol table. Returns 1 on success.
static int  fGetLong( char *s, unsigned long &lVal, SymbolTable &Symbols)
{
  if( s == NULL)
    return 0;

  if( fIsLong( s)) {
#ifdef wx_x 
    // @@@ Don't have strtoul
    lVal = (unsigned long)strtol(s, 0, 0);
#else
    lVal = strtoul( s, 0, 0);
#endif
    return 1;
  } else if( Symbols.IsDefined( s)) {
    lVal = Symbols.SymbolValue( s);
    return 1;
  }
  return 0;
}

//Attempts to dissect simple logical expressions where no separators are present
static int fDissect( const char *s, char *&s2, char *&s3)
{
  if( (s2 = (char *)strchr( (char *)s, '!')) != 0) {
    s3 = s2 + 2;
    return 1;
  } else if( (s2 = (char *)strchr( (char *)s, '<')) != 0) {
    s3 = (s2[1] == '=') ? (s2 + 2) : (s2 + 1);
    return 1;
  } else if( (s2 = (char *)strchr( (char *)s, '>')) != 0) {
    s3 = (s2[1] == '=') ? (s2 + 2) : (s2 + 1);
    return 1;
  } else if( (s2 = (char *)strchr( (char *)s, '=')) != 0) {
    // I hope it is apparent why I look for this at end!
    s3 = s2 + 2;
    return 1;
  }
  return 0;
}

// Attempts to dissect even simpler expressions with either + or - or |
static int fDissectAdd( const char *s, char *&s2, char *&s3)
{
  if( (s2 = (char *)strchr( (char *)s, '+')) != 0) {
    s3 = s2 + 1;
  } else if( (s2 = (char *)strchr( (char *)s, '-')) != 0) {
    s3 = s2 + 1;
  } else if( (s2 = (char *)strchr( (char *)s, '|')) != 0) {
    s3 = s2 + 1;
  } else
    return 0;
  return 1;
}


// Parses preprocessor command recoginized to be '#if ...'
// return value here means the value of the logical expression after "#if" part
// Hence, we cannot return error. But this approach seems to be more robust!
static int  fProcessIF( int nParts, ParsedString &ps, SymbolTable &Symbols,
                 ParsedFile &pf)
{ register int i,
               len;

  for( len = 2, i = 1 ; i < nParts ; ++i)
    len += (strlen( ps.GetSegment( i)) + 2);  // the +1 is for separating tokens by space...

  char  *szBuf = new char[len + 1];
  if(szBuf == NULL) {
    pf.Warning( "RCPARSER:fProcessIF()", "Cannot parse-no memory");
    return 1;  // Generally conforms to strategy "If I do not understand that,
               // it is probably true" :-)
  }
  szBuf[0] = '\0';  // init
  for( i = 1 ; i < nParts ; ++i) {
    strcat( szBuf, ps.GetSegment( i));  // add segment
    strcat( szBuf, " ");                // and insert a separator
  }
  // we have done all these exercises to be able to strip down seriously including
  // parentheses from the IF expression. We cannot do that in the first place, because
  // e.g. in #define expression their presence is decisive for recognizing a macro...

  ParsedString  ps2( szBuf, " \t()");   // a new version of the parsed string...
  int           N = ps2.Segments();
  char         *s1,
               *s2,
               *s3;

  delete []szBuf;                       // no longer needed ...

  switch( N) {
    case 0:
         LSyntaxErr:
#ifdef STRICT_REPORT
      pf.Warning( "RCOBJECT:fProcessIF()", "'#if' too complicated");
#endif
      return 1;                    // being an error! "#if" staying alone
    case 2:
      s1 = ps2.GetSegment( 0);
      s2 = ps2.GetSegment( 1);
      if(strcmp( s1, "defined") == 0) // for #if defined(__WINDOWS_H)    etc
        return (Symbols.IsDefined( s2) != 0);
      else if(strcmp( s1, "!defined") == 0) // for #if !defined( BWCC_H)   etc
        return (Symbols.IsDefined( s2) == 0);
      else
        goto LSyntaxErr;
    case 1:
      s1 = ps2.GetSegment( 0);
      s2 = s3 = 0;
      if( !fDissect( s1, s2, s3)) // attempt to dissect things like [#if] (XXX<2)
      {
        if(Symbols.IsDefined( s1)) // this symbol exists ...
          return (Symbols.SymbolValue( s1) != 0L); //for things like "#if IN_SOMEWHERE"
        else
          goto LSyntaxErr;
      }
      break;
    default:    // simplistic, cause e.g. #if (X > 3) && (X < 1) is true if X==4
      s1 = ps2.GetSegment( 0);
      s2 = ps2.GetSegment( 1);
      s3 = ps2.GetSegment( 2);
  }
  // being here means that we have 3 string initialized...

  unsigned long   lLeft,
                  lRight;

  if( !fGetLong( s1, lLeft,  Symbols))
  {
      LSymbolErr:
#ifdef STRICT_REPORT
    pf.Warning( "RCOBJECT:fProcessIF()", "syntax complicated");
#endif
    return 1;    // error of course
  }
  if( !fGetLong( s3, lRight, Symbols))
    goto LSymbolErr;

  switch( *s2)
  {
    case '=':
      if(s2[1] != '=')   // that's error....
        goto LSyntaxErr;
      else
        return(lLeft == lRight);
    case '!':
      if(s2[1] != '=')
        goto LSyntaxErr;
      else
        return(lLeft != lRight);
    case '>':
      if(s2[1] == '=')    // supposed to be >=
        return (lLeft >= lRight);
      else
        return (lLeft >  lRight);
    case '<':
      if(s2[1] == '=')
        return (lLeft <= lRight);
      else
        return (lLeft <  lRight);
    default:
      goto LSyntaxErr;
  }
}

// parses preprocessor command recoginized to be #define. If OK, inserts symbol
// and its (long )value into symbol table. Needs to cope with uninteresting
// info occurring in include files: (a) parametric macros  (b) textual defines
// return value here means the validity of the expression after the "#define"
static int fProcessDEFINE( int nParts, ParsedString &ps, SymbolTable &Symbols,
                    ParsedFile &pf)
{ char  *symbol,
        *szExpression;
  int    i,
         len;

  symbol = copystring( ps.GetSegment( 1) ); // make a private copy that eventually stays in hashtable

  if( fHasParentheses( symbol))       // i.e. things like #define max( a, b) (...
  {
    delete []symbol;
    return 1;                        // we return, but this is NOT an error
  }
  for( i = 2, len = 2 ; i < nParts ; ++i)
    len += (strlen( ps.GetSegment( i)) + 2);

  if( (szExpression = new char [len + 1]) == 0) {
    delete []symbol;
    pf.Warning( "RCOBJECT:fProcessDEFINE()", "Cannot parse - no memory");
    return 0;                        // handle this fault more elegantly ??
  }
  szExpression[0] = '\0';
  for( i = 2 ; i < nParts ; ++i) {
    strcat( szExpression, ps.GetSegment( i));
    strcat( szExpression, " ");
  }

  // reparse the expression...
  ParsedString psExp( szExpression, " \t()");

  delete []szExpression;
  len = psExp.Segments();

  char         *s1,
               *s2,
               *s3;
  unsigned long lLeft,
                lRight;

  switch( len)
  {
    case 0:
      Symbols.Add( symbol, 1L);    // things like "#define RC_INVOKED"
      // since now, the hash table Symbols cares about the string allocated in symbols[]!
      return 1;
    case 2:
      goto LSyntaxError;
    case 1:
      s1 = psExp.GetSegment( 0);
      if( fGetLong( s1, lLeft, Symbols))
      {
        Symbols.Add( symbol, lLeft);
        return 1;
      }
      if( !fDissectAdd( s1, s2, s3)) // attempt to dissect things like #define X (Y+1)
      {
        goto LSyntaxError;
      }
      else
        break;
    default:    // simplistic, cause e.g. #define X (Y + 1 - Z) ... leads to X=Y+1  !
      s1 = psExp.GetSegment( 0);
      s2 = psExp.GetSegment( 1);
      s3 = psExp.GetSegment( 2);
  }
  // being here means that we have 3 string initialized...

  if( !fGetLong( s1, lLeft,  Symbols))
  {
    goto LSyntaxError;
  }
  if( !fGetLong( s3, lRight, Symbols))
  {
    goto LSyntaxError;
  }

  switch( *s2)
  {
    case '+':
      Symbols.Add( symbol, lLeft + lRight);
      return 1;
    case '-':
      Symbols.Add( symbol, lLeft - lRight);
      return 1;
    case '|':
      if( (len & 1) == 1)  // attempt to solve things like #define X (A | B | C | D)
      {
        lLeft |= lRight;
        for( i = 3 ; i < (len - 1) ; i += 2)
        {
          if( *(psExp.GetSegment( i)) != '|')
            return 0;    // that's wrong!
          if( !fGetLong( psExp.GetSegment( i + 1), lRight, Symbols))
            return 0;    // that's wrong!
          else
            lLeft |= lRight;
        }
        Symbols.Add( symbol, lLeft);
        return 1;
      }
      else
      {
        goto LSyntaxError;
      }
  }
    LSyntaxError:
#ifdef STRICT_REPORT
      pf.Warning( "RCOBJECT:fProcessDEFINE()", "'#define' syntax");
#endif
      delete[]symbol;
      return 0;   // error...

}

// forward declaration ....
static int  ProcessInclude( const char *szFileName, SymbolTable &Symbols,
                     char *szParserFlags, ParsedFile &pf);


// This is the routine dispatch for processing preprocessor commands in .RC / .H
static int  ProcessCommand( Condition   &fEnabled, char *szCommand, SymbolTable &Symbols,
                     char *szParserFlags, ParsedFile &pf)
{ ParsedString  psCommand( szCommand, " \t\n");
  int           nSS = psCommand.Segments();     // number of substrings
  char         *s1  = psCommand.GetSegment( 0); // should be always at least 1

  if( strcmp( s1, "#include") == 0) {
    // it is an include file...
    if( !fEnabled.Set())       // do not include if in excluded block
      return 1;
    return ProcessInclude( szIncludeName( szCommand), Symbols, szParserFlags, pf);
  } else if( strcmp( s1, "#define") == 0) {
    // defining a new symbol
    if( !fEnabled.Set())
      return 1;                 // do not define if in excluded block
    return fProcessDEFINE( nSS, psCommand, Symbols, pf);
  } else if( strcmp( s1, "#ifdef") == 0) {
    // conditional block: symbol must be defined
    if(nSS != 2) {
      pf.Warning( "RCOBJECT:ProcessCommand()", "'#ifdef' statement");
      return 0;
    }
    fEnabled.Push( Symbols.IsDefined( psCommand.GetSegment( 1)));
    return 1;
  } else if( strcmp( s1, "#ifndef") == 0) {
    if(nSS != 2) {
      pf.Warning( "RCOBJECT:ProcessCommand()", "'#ifndef' statement");
      return 0;
    }
    fEnabled.Push( ! Symbols.IsDefined( psCommand.GetSegment( 1)));
    return 1;
  } else if( strcmp( s1, "#undef") == 0) {
    if( ! fEnabled.Set())
      return 1;                 // do not undef if in an excluded block!
    if(nSS != 2)
    {
      pf.Warning( "RCOBJECT:ProcessCommand()", "'#undef' statement");
      return 0;
    }
    if( Symbols.IsDefined( psCommand.GetSegment( 1)))
      Symbols.Remove( psCommand.GetSegment( 1));
    return 1;
  } else if( strcmp( s1, "#else") == 0) {
    fEnabled.Invert();
    return 1;
  } else if( strcmp( s1, "#endif") == 0) {
    if(fEnabled.Pop())
      return 1;
    else {
      pf.Error( "RCOBJECT:ProcessCommand()", "Unbalanced #endif");
      return 0;
    }
  } else if( strcmp( s1, "#if") == 0) {
    if(nSS < 2)
    {
      pf.Warning( "RCOBJECT:ProcessCommand()", "'#if' statement");
      return 0;
    }
    fEnabled.Push( fProcessIF( nSS, psCommand, Symbols, pf));
    return 1;
  }
#ifdef STRICT_REPORT
  pf.Warning( "RCOBJECT:ProcessCommand()", "Unknown directive");
#endif
  return 0;
}


// this function is called only for TRUE includes (i.e. *.h, *.hh or whatever..
//  - they are included with the #include directive. They cannot contain proper
// RC statements. So, only lines being preprocessor commands are used...
// In a way, this is a stripped-down version of ...::Read() below, being too recursive etc.
#ifdef __BORLANDC__
#pragma argsused
#endif
static int ProcessInclude( const char *szFileName, SymbolTable &Symbols,
                     char *szParserFlags, ParsedFile &pf)
{ FILE      *f = pfTryOpen( szFileName, szParserFlags);
  Condition  fEnabled;  // this condit.-compil.condition has a file scope

  if(f == NULL)
    return 0;

  ParsedFile  Pfile( f, (char *)szFileName); // create local context for parsing

  while( Pfile.ReadNextLine() > 0) {
    switch( Pfile.StripOutComments()) {
      case 0:
        return 0;
      case 2:
        continue;
      case 3:   // comment block starts in this line...
        while( Pfile.ReadNextLine()) {
          switch( Pfile.StripOutComments()) {
            case 0:
              return 0;
            case 1:
              if( Pfile.HasClosingComment())
                goto LEOComment;
          }
        }
        return 0;  // because no end-of-comment-block
          LEOComment:
        continue;

      case 1:
        if( ! Pfile.IsPreprocessor())  // this is NOT preprocessor line..
          continue;                    // so no interest in this context...
        else
          ProcessCommand( fEnabled, Pfile.GetTheString(), Symbols,
                          szParserFlags, Pfile);
    }
  }
  fclose( f);
  return 1;
}

// is the string a signed (long)int?
BOOL fIsSignedNum( char *sz)
{ ParsedString test( sz, " \t\n");

  if(test.Segments() == 1)  // only one
  { char *s = test.GetSegment( 0);

    if( *s == '+' || *s == '-') //starts with sign
      return TRUE;
  }
  return FALSE;
}


static long  lGetID( char *szToken, SymbolTable &Symbols, ParsedFile &pf,
              char *& idName)
// for IDs, there could be the following expressions:
// ID                :=  { symbolInt | simple-expression }
// simple-expression := symbolInt %op% symbolInt
// symbolInt         := { int-const | symbol }
// %op%              := { '+' | '-' }
{
  idName = 0;   // just for safety...
  if(fIsSignedNum( szToken))
    return atol( szToken);

  if( (strchr( szToken, '+') != 0) ||
      (strchr( szToken, '-') != 0))   // cannot be symbol, try to parse
  { BOOL          fPlus = (strchr( szToken, '+') != 0);
    unsigned long lLeft,
                  lRight;
    ParsedString expr( szToken, " \t+-");
    if(expr.Segments() != 2) {
      pf.Warning( "RCOBJECT:lGetID()", "Complicated expression");
      return -1L;
    }
    if( (!fGetLong( expr.GetSegment( 0), lLeft,  Symbols)) ||
        (!fGetLong( expr.GetSegment( 1), lRight, Symbols))) {
      pf.Warning( "RCOBJECT:lGetID()", "Cannot locate symbol");
      return -1L;
    } else
      return( fPlus ? (lLeft + lRight) : (lLeft - lRight));
  }
  else
  { unsigned long lLeft;
    ParsedString  expr( szToken, " \t()");

    if(expr.Segments() != 1)
    {
      pf.Warning( "RCOBJECT:lGetID()", "Complicated expression!");
      return -1L;
    }
    else if( fGetLong( expr.GetSegment( 0), lLeft, Symbols))
    { char  *s = expr.GetSegment( 0);  // safe accesses

      if( !fIsLong( s)) // means: must be a symbol
      {
        idName = copystring( s);
      }
      return lLeft;
    }
    else    // Symbol not found situation:
    {
      if( nSymbolFaultPolicy == 0) // usual one
      {
        pf.Warning( "RCOBJECT:lGetID()", "Cannot locate symbol");
        return -1L;
      }
      else                 // quietly ignore fault and record unknown symbol
      { char *s = expr.GetSegment( 0);

        if(s != 0)
          if( strlen( s) > 0) {
            idName = copystring( s);
          }
        return -2L;
      }
    }
  }
}

// we return <XXXX> based on input of <ZZZZ "XXXX">
// Parsing In-Place!
static char *szGetCaption( char *s)
{
  strtok( s, "\"");   //a very special parsing!
  return strtok( NULL, "\"");
}

static char *szGetStrip( char *s)
{
  char *sptr = s;

  while( (*sptr != 0) && (*sptr != '"'))
    ++sptr;
  if(*sptr == 0)
    return s;
  return strtok( sptr, "\"");
}

static char szCNBuf[64];

// similar to the previous, but non-destructive!
static char *szControlName( char *szToken)
{ char  *szT = copystring( szToken);

  strncpy( szCNBuf, strtok( szT, " \t\""), 63);
  szCNBuf[63] = '\0';
  delete []szT;
  return szCNBuf;
}


// parsing list of Microsoft' style flags
static unsigned long  lGetStyle( char *szToken, SymbolTable &Symbols)
// here we assume restricted form X1[ ]|[ ]X2[ ]|[ ]X3 ... etc
{ ParsedString  ors( szToken, " |\t\n");
  int           N = ors.Segments(),
                i;
  unsigned long lTStyle = 0L,
                lStyleBit;

  for( i = 0 ; i < N ; ++i)
    if( fGetLong( ors.GetSegment( i), lStyleBit, Symbols))
      lTStyle |= lStyleBit;
  return lTStyle;
}

// The following function is to patch the terrible problem of parsing
// tokens separated by commas: If I get a comma in the text like "A,B"
// it is considered as separator, too. So, the trick is to run through the
// string, and change all the "non-valid" commans to something else.
// After parsing, we should run the parsed tokens through this function
// again (with the fEncode set to 0) if they are suspect of having this
// stuff in (basically this is caption only!), i.e. the first token!
static void  RecodeCommas( char *szString, int fEncode = 1)
{ int   fInString = 0;
  char *s         = szString;

  if( fEncode) // this is the more difficult part
    while(*s != 0) {
      switch( *s) {
        case '\"':
          if(*(s - 1) != '\\') //this trick assumes that '"' cannot be first char!
            fInString = !fInString;
          break;
        case ',':           // the TARGET character:
          if(fInString)     // we are in a string specification!
            *s = 1;         // stupid, but should work
      }
      ++s;
    }
  else                      // much more simple!
    while(*s != 0) {
      if(*s == 1)
        *s = ',';
      ++s;
    }
  return;
}

static int IsA( unsigned long lStyle, unsigned long lType)
{
  lStyle = lStyle & 0x0000000FL; // mask everything except lower 4 bits!
  return (lStyle == lType);
}


// parses all dialog controls in a dialog block in RC file
static void  ParseControl( rcDialog *prcd, char *szLine, SymbolTable &Symbols, ParsedFile &pf)
{ char               *szControl,
                     *szCaption,
// next row defines variable optionally holding symbol for the control ID. The
// policy is that if found, the variable poitns to dynamically allocated area
// that should be release at the end of function. If not found, value is 0.
                     *szIDname    = 0;
  long                lID;
  unsigned long       lStyle;
  int                 x,
                      y,
                      w,
                      h;
  int                 fWasGeneric = 0;
  rcItem::RCITEMTYPE  rciType = rcItem::rcSTATIC;

  RecodeCommas( szLine, 1);
  // now we can apply parsing without fears  (really ?!! ;-)
  ParsedString   statement( szLine, ",\n\t");
  int            N = statement.Segments();

  if(N < 4)  // which is minimum number of tokens, for the ICON control!
  {
      LSyntaxError:
    pf.Warning( "RCOBJECT:ParseControl()", "Syntax");
    return;
  } else
    RecodeCommas( statement.GetSegment( 0), 0); // recode back!
  szControl = szControlName( statement.GetSegment( 0));

  // first, we will establish the identity of the control type...
  if(stricmp( szControl, "CHECKBOX") == 0)
    rciType = rcItem::rcCHECKBOX;
  else if(stricmp( szControl, "CTEXT") == 0)
    rciType = rcItem::rcCTEXT;
  else if(stricmp( szControl, "LTEXT") == 0)
    rciType = rcItem::rcLTEXT;
  else if(stricmp( szControl, "RTEXT") == 0)
    rciType = rcItem::rcRTEXT;
  else if(stricmp( szControl, "PUSHBUTTON") == 0)
    rciType = rcItem::rcPUSHBUTTON;
  else if(stricmp( szControl, "DEFPUSHBUTTON") == 0)
    rciType = rcItem::rcDEFPUSHBUTTON;
  else if(stricmp( szControl, "GROUPBOX") == 0)
    rciType = rcItem::rcGROUPBOX;
  else if(stricmp( szControl, "RADIOBUTTON") == 0)
    rciType = rcItem::rcRADIOBUTTON;

  // those above this line share common way of parsing, as they have the general pattern:
  // CtrlType "Caption" , id , x , y , w , h [, style]

  else if(stricmp( szControl, "COMBOBOX") == 0)
    rciType = rcItem::rcCOMBOBOX;
  else if(stricmp( szControl, "EDITTEXT") == 0)
    rciType = rcItem::rcEDITTEXT;
  else if(stricmp( szControl, "LISTBOX") == 0)
    rciType = rcItem::rcLISTBOX;
  else if(stricmp( szControl, "SCROLLBAR") == 0)
    rciType = rcItem::rcSCROLLBAR;

  // those (four) items above share common way of parsing, as they have general pattern:
  // CtrType id , x , y , w , h [, style]

  else if(stricmp( szControl, "ICON") == 0)
    rciType = rcItem::rcICON; // this is somewhat peculiar: ICON "text", id, x ,y ...ignore rest!
  else if(stricmp( szControl, "CONTROL") == 0) { 
    char *szClassName = szGetStrip( statement.GetSegment( 2));

    // here, as an exception we get the style earlier, as we need to make the ctrl.classification
    // finer than corresponds to control classes - eg. to know what is LTEXT and what RTEXT!

    lStyle      = lGetStyle( statement.GetSegment( 3), Symbols);
    fWasGeneric = 1;         // we need to know this so that we can parse properly the rest

    if(stricmp( szClassName, "BUTTON") == 0) {
      if( IsA( lStyle, BS_RADIOBUTTON) || IsA( lStyle, BS_AUTORADIOBUTTON))
        rciType = rcItem::rcRADIOBUTTON;
      else if( IsA( lStyle, BS_CHECKBOX) || IsA( lStyle, BS_AUTOCHECKBOX) ||
               IsA( lStyle, BS_3STATE) || IsA( lStyle, BS_AUTO3STATE))
        rciType = rcItem::rcCHECKBOX;
      else if( IsA( lStyle, BS_DEFPUSHBUTTON))
        rciType = rcItem::rcDEFPUSHBUTTON;
      else if( IsA( lStyle, BS_GROUPBOX))
        rciType = rcItem::rcGROUPBOX;
      else if( IsA( lStyle, BS_PUSHBUTTON))
        rciType = rcItem::rcPUSHBUTTON;
      else
        rciType = rcItem::rcPUSHBUTTON; // a BS_USERBUTTON or BS_OWNERDRAW ?
    } else if(stricmp( szClassName, "STATIC") == 0) {
      if( IsA( lStyle, SS_LEFT))
        rciType = rcItem::rcLTEXT;
      else if( IsA( lStyle, SS_RIGHT))
        rciType = rcItem::rcRTEXT;
      else if( IsA( lStyle, SS_CENTER))
        rciType = rcItem::rcCTEXT;
      else if( IsA( lStyle, SS_ICON))
        rciType = rcItem::rcICON;
      else if( IsA( lStyle, SS_SIMPLE))
        rciType = rcItem::rcLTEXT;
      else      // this is a sort of rectangle or frame, most probably
        rciType = rcItem::rcCTEXT;
    } else if(stricmp( szClassName, "COMBOBOX") == 0)
      rciType = rcItem::rcCOMBOBOX;
    else if(stricmp( szClassName, "EDIT") == 0)
      rciType = rcItem::rcEDITTEXT;
    else if(stricmp( szClassName, "LISTBOX") == 0)
      rciType = rcItem::rcLISTBOX;
    else if(stricmp( szClassName, "SCROLLBAR") == 0)
      rciType = rcItem::rcSCROLLBAR;
    else if(stricmp( szClassName, "BorShade") == 0)   // here start BWCC classes
    { long lMask = lStyle & 7;

      if( (lMask == 1) || (lMask == 6))
        rciType = rcItem::rcGROUPBOX;
      else
        rciType = rcItem::rcSTATIC;
    }
    else if(stricmp( szClassName, "BorBtn") == 0)
      rciType = IsA( lStyle, BS_DEFPUSHBUTTON) ? rcItem::rcDEFPUSHBUTTON :
                                                 rcItem::rcPUSHBUTTON;
    else if(stricmp( szClassName, "BorCheck") == 0)
      rciType = rcItem::rcCHECKBOX;
    else if(stricmp( szClassName, "BorRadio") == 0)
      rciType = rcItem::rcRADIOBUTTON;
    else if(stricmp( szClassName, "BorStatic") == 0) {
      if( IsA( lStyle, SS_CENTER))
        rciType = rcItem::rcCTEXT;
      else if( IsA( lStyle, SS_RIGHT))
        rciType = rcItem::rcRTEXT;
      else
        rciType = rcItem::rcLTEXT;
    } else {
      pf.Warning( "RCOBJECT:ParseControl()", "Unknown control class");
      return;     // means an unknown class for the control...
    }
  }
  // so, now we have rciType initialized. We will go to parse the other information:
  if( fWasGeneric) {
    // lStyle is already retrieved..
    if(N != 8)
      goto LSyntaxError;           // an error, obviously!
    lID = lGetID( statement.GetSegment( 1), Symbols, pf, szIDname);
    x   = atoi( statement.GetSegment( 4));
    y   = atoi( statement.GetSegment( 5));
    w   = atoi( statement.GetSegment( 6));
    h   = atoi( statement.GetSegment( 7));
    szCaption = szGetCaption( statement.GetSegment( 0));
  } else
    switch( rciType)
    {
      case rcItem::rcGROUPBOX:
      case rcItem::rcCHECKBOX:
      case rcItem::rcRADIOBUTTON:
      case rcItem::rcLTEXT:
      case rcItem::rcCTEXT:
      case rcItem::rcRTEXT:
      case rcItem::rcPUSHBUTTON:
      case rcItem::rcDEFPUSHBUTTON:
      case rcItem::rcICON:

        if( (N != 6) && (N != 7))
          goto LSyntaxError;
        szCaption = szGetCaption( statement.GetSegment( 0));
        lID       = lGetID( statement.GetSegment( 1), Symbols, pf, szIDname);
        x         = atoi( statement.GetSegment( 2));
        y         = atoi( statement.GetSegment( 3));
        if(rciType != rcItem::rcICON) {
          w       = atoi( statement.GetSegment( 4));
          h       = atoi( statement.GetSegment( 5));
        } else
          w = h = 0;
        if(N == 7)
          lStyle = lGetStyle( statement.GetSegment( 6), Symbols);
        else {
          // default, set implied styles!
          switch( rciType) {
            case rcItem::rcCHECKBOX:
              lStyle = BS_CHECKBOX | WS_TABSTOP;    break;
            case rcItem::rcRADIOBUTTON:
              lStyle = BS_RADIOBUTTON | WS_TABSTOP; break;
            case rcItem::rcLTEXT:
              lStyle = SS_LEFT | WS_GROUP;          break;
            case rcItem::rcCTEXT:
              lStyle = SS_CENTER | WS_GROUP;        break;
            case rcItem::rcRTEXT:
              lStyle = SS_RIGHT | WS_GROUP;         break;
            case rcItem::rcPUSHBUTTON:
              lStyle = BS_PUSHBUTTON | WS_TABSTOP;  break;
            case rcItem::rcDEFPUSHBUTTON:
              lStyle = BS_DEFPUSHBUTTON|WS_TABSTOP; break;
            case rcItem::rcGROUPBOX:
              lStyle = BS_GROUPBOX;                 break;
            case rcItem::rcICON:
              lStyle = SS_ICON;
          }
        }
        break;
      case rcItem::rcCOMBOBOX:
      case rcItem::rcEDITTEXT:
      case rcItem::rcLISTBOX:
      case rcItem::rcSCROLLBAR:
        if( (N != 6) && (N != 5))
          goto LSyntaxError;
        szCaption = "";
        strtok( statement.GetSegment( 0), " ");  // prepare for next statement...
        lID       = lGetID( strtok( NULL, " "), Symbols, pf, szIDname);
        x         = atoi( statement.GetSegment( 1));
        y         = atoi( statement.GetSegment( 2));
        w         = atoi( statement.GetSegment( 3));
        h         = atoi( statement.GetSegment( 4));
        if(N == 6)
          lStyle = lGetStyle( statement.GetSegment( 5), Symbols);
        else        // default, implied styles!
        {
          switch( rciType) {
            case rcItem::rcCOMBOBOX:
              lStyle = CBS_SIMPLE | WS_TABSTOP;     break;
            case rcItem::rcEDITTEXT:
              lStyle = ES_LEFT | WS_BORDER | WS_TABSTOP; break;
            case rcItem::rcLISTBOX:
              lStyle = LBS_NOTIFY | WS_BORDER;      break;
            case rcItem::rcSCROLLBAR:
              lStyle = SBS_HORZ;                    break;
          }
        }
    }
  // we are done with this control, so record the info (creating a ctrl instance)
  prcd->Add( new rcItem( szCaption, rciType, (unsigned int )lID, lStyle,
                         x, y, w, h, szIDname));
  if(szIDname != 0) // see function begin...
    delete []szIDname;
  return;
}

// Parsing the dialog block proper (i.e. parts after the XXName DIALOG x y ...)
// I suppose that no .RC generator does insert things like comment blocks or #defines
// into DIALOG blocks! If does, it is a pig and I will not parse that 8>}
static void ParseDialog( char *szName, int x, int y, int w, int h,
                  SymbolTable &Symbols, ParsedFile &pf, rcFile &rcfile)
{
   unsigned long lStyle = WS_POPUP | WS_BORDER | WS_SYSMENU; // default style
  char *szCaption      = 0;

  do {
    if( pf.ReadNextLine() == 0)
      return;
    switch( pf.StripOutComments()) {
      case 0:
        pf.Error( "RCOBJECT:ParseDialog()", "Unexpected EOF");
        return;          // unexpected error....
      case 1:            // something to do!
        if( pf.IsBEGIN())
          continue;
        else {
        // at this point, TWO possibilities are of interest:
        // (a) STYLE.  I assume it cannot contain '"'. Then we could use blanks + () etc.
        // (b) CAPTION.I assume it must contain '"'. We can split only along these!
          ParsedString  command( pf.GetTheString(),
                                 (strchr( pf.GetTheString(), '"') != 0) ? "\"\n" : " ()\n");
          char         *s = command.GetSegment( 0);

          if( stricmp( s, "STYLE") == 0)
          { int            N = command.Segments() - 1;
            unsigned long  lT;

            if((N & 1) != 1)  // must be odd!
              break;          // wrong for parsing...
            lStyle = 0L;
            for( int i = 2 ; i < N ; i += 2)
            {
              if( strcmp( command.GetSegment( i), "|") != 0)
                break;
              if( !fGetLong( command.GetSegment( i + 1), lT, Symbols))
                break;
              else
                lStyle |= lT;
            }
          }
          else if( stricmp( strtok( s, " \t"), "CAPTION") == 0)
          // the strtok is needed because this was, possible parsed along the "" markers only!
          {
            if(szCaption != 0)
              delete []szCaption;
            szCaption = new char[strlen( command.GetSegment( 1)) + 1];
            strcpy( szCaption, strtok( command.GetSegment( 1), "\""));
          }
        }
      default:
        continue;
    }
  } while( !(pf.IsBEGIN()));

  rcDialog *rcdialog = new rcDialog( szName, (szCaption != 0) ? szCaption : "",
                                     x, y, w, h, lStyle);
  rcfile.Add( rcdialog );

  // now the BEGIN - END block ...
  while( pf.ReadNextLine() != 0) {
    switch( pf.StripOutComments()) // I ignore possibility of multiline comment block here!
    {
      case 0:
        pf.Error( "RCOBJECT:ParseDialog()", "Unexpected EOF");
        return;
      case 2:
        continue;
      case 1:
        if(pf.IsEND())
          return;                    // OK, end of specification...
        else
          ParseControl( rcdialog, pf.GetTheString(), Symbols, pf);
    }
  }
}

// PARSE MENU ITEM OPTIONS
rcMenuItem::RCMENUOPTION GetMenuOpt( ParsedString &ps, int from, int to)
{ register int  i;
  int           opt = 0;

  for( i = from ; i <= to ; ++i)
  { char *s = ps.GetSegment( i);

    if(s == 0)
      return (rcMenuItem::RCMENUOPTION )opt;
    else if(stricmp( s, "CHECKED") == 0)
      opt |= (int )rcMenuItem::rcCHECKED;
    else if(stricmp( s, "GRAYED") == 0)
      opt |= (int )rcMenuItem::rcGRAYED;
    else if(stricmp( s, "HELP") == 0)
      opt |= (int )rcMenuItem::rcHELP;
    else if(stricmp( s, "INACTIVE") == 0)
      opt |= (int )rcMenuItem::rcINACTIVE;
    else if(stricmp( s, "MENUBARBREAK") == 0)
      opt |= (int )rcMenuItem::rcMENUBARBREAK;
    else if(stricmp( s, "MENUBREAK") == 0)
      opt |= (int )rcMenuItem::rcMENUBREAK;
    else
      ;
  }
  return (rcMenuItem::RCMENUOPTION )opt;
}


// Parse Menu hierarchy
static rcMenuPopup *ParseMenu( char *szName, rcMenuItem::RCMENUOPTION opt,
                        ParsedFile &pf,  SymbolTable &Symbols)
{ rcMenuPopup *rcm = new rcMenuPopup( szName, opt);
  char        *szType,
              *szRest;

  pf.ReadNextLine();      // this is supposed to be the line with BEGIN
  while( pf.ReadNextLine() != 0)
    switch( pf.StripOutComments())
    {
      case 0:
        pf.Error( "RCOBJECT:ParseMenu()", "Unexpected EOF");
        return rcm;
      case 2:
        continue;  // nothing on this line
      case 1:
        if(pf.IsEND())
          return rcm;
        // now, we are where we are:
        szType = strtok( pf.GetTheString(), " \t\n");
        // this destroys pf's buffer content
        szRest = strtok( NULL, "\n"); //should return all the rest
        if( (szType == 0) || (szRest == 0)) // empty line..
          continue;
        if(stricmp( szType, "POPUP") == 0) // a pop/up definition starts here
        { char *s;

          if( (s = strchr( szRest, '"')) != 0) // there is some "XXX" string...
          {
            if( (szRest = strchr( s + 1, '"')) != 0)
            {
              *szRest = '\0';
              ++szRest;
              ++s;
              // now, s points to beginning of menu item text, szRest to options

              ParsedString options( szRest, " \t,\n");
              int          NN = options.Segments() - 1;

              rcm->AddChild( ParseMenu( s, GetMenuOpt( options, 0, NN), pf, Symbols));
            }
            else
            {
              pf.Error( "RCOBJECT:ParseMenu()", "POPUP statement");
              return rcm;
            }
          }
          else
            break;
        }
        else if( stricmp( szType, "MENUITEM") == 0)
        { char *s;

          if( (s = strchr( szRest, '"')) != 0)
          {
            if( (szRest = strchr( s + 1, '"')) != 0)
            {
              *szRest = '\0';
              ++szRest;
              ++s;

              ParsedString  IdAndOptions( szRest, " \t,\n");
              int           NN = IdAndOptions.Segments() - 1;
              unsigned long lID;
              char          *szIDname = 0,
                            *s1;

              if(NN < 0)
                goto LSyntaxError;
              if(!fGetLong( IdAndOptions.GetSegment( 0), lID, Symbols))
              {
                if(nSymbolFaultPolicy == 0)  //the normal one
                  goto LSyntaxError;
                else
                {
                  s1 = IdAndOptions.GetSegment( 0);
                  if(s1 != 0)
                    if(strlen( s1) > 0) {
                      szIDname = copystring( s1);
                    }
                  lID = 0x0FFFFFFFEL;  // -2L ?
                }
              }
              else
              {
                // parsed. Now check for symbol being present...
                s1 = IdAndOptions.GetSegment( 0);
                if( !fIsLong( s1)) //is not a constant
                {
                  szIDname = copystring( s1);
                }
              }
              rcm->AddChild( new rcMenuItem( s, (int )lID,
                                        GetMenuOpt( IdAndOptions, 1, NN),
                                        szIDname));
              if(szIDname != 0)
                delete []szIDname;
            }
            else
            {
                LSyntaxError:
              pf.Error( "RCOBJECT:ParseMenu()","MENUITEM statement");
              return rcm;  // error
            }
          }
          else if( strstr( szRest, "SEPARATOR") != 0)
            rcm->AddChild( new rcMenuItem( "", -1, rcMenuItem::rcSEPARATOR, 0));
          else
            goto LSyntaxError;
        }
        else
        {
          // report warning!
          pf.Warning( "RCOBJECT:ParseMenu()", "Don't know how to parse");
          break;
        }
    }
    return rcm;
}

// Core function for parsing the RC statements proper ...
static void  ProcessStatement( rcFile &rcfile, ParsedFile &pf, SymbolTable &Symbols, char *szLine)
{ ParsedString  command( szLine, " ,\t\n");
  int           N = command.Segments();

  if(N == 0)
    return;    // nothing to do, obviously...
  if(N == 1)
  { char *s = command.GetSegment( 0);

    if( (s[0] == '{' && s[1] == '\0') ||  stricmp( s, "BEGIN") == 0) {
      // THIS could happen with user-defined resources where the BEGIN/END block is optional!
      pf.SkipUntilEND( 1);
      return;
    }
    if( stricmp( s, "STRINGTABLE") == 0) {
     // this one is exception it doesnt start with NameID
      // plug string-table parsing here: shouldn't be difficult
      pf.SkipUntilEND();
      return;
    }
    // being here means something went wrong with my quick&dirty code or with .RC file syntax!
    pf.Warning( "RCOBJECT:ProcessStatement()", "Unknown RC statement");
    return;       // giving up...
  }
  // the first-level possibilities should now have something recognizable in the second field

  char *szType = command.GetSegment( 1),
       *szName = command.GetSegment( 0);

  // first the stuff we do not pay attention to  - at least in this version!
  // blocked things ....
  if( (stricmp( szType, "ACCELERATORS") == 0)  ||
      (stricmp( szType, "RCDATA")       == 0)  ||
      (stricmp( szType, "VERSIONINFO")  == 0))
  {
    // this triumvirate above is generally a BIG BAG OF WORMS. I am not much
    // willing to progress further with this
    pf.SkipUntilEND();
    return;
  }
  // now the one-liners :-)
  if( (stricmp( szType, "BITMAP")   == 0) ||
      (stricmp( szType, "CURSOR")   == 0) ||
      (stricmp( szType, "FONT")     == 0) ||
      (stricmp( szType, "ICON")     == 0))
  // this might be a snap ; sometimes in future (near?)
    return;
  if( stricmp( szType, "MENU") == 0) {
    // parse menu
    rcfile.Add( ParseMenu( szName, rcMenuItem::rcMENUPOPUP, pf, Symbols));
    return;
  }
  // OK: now seems that only other possibilities are user-defined resource and DIALOG . Yupee!
  if( stricmp( szType, "DIALOG") != 0)
  // this IS actually a catch-all: we hope that only reason to get here is user-defined resource
    return;    // I could do hardly anything more. U.D.R. MAY have BEGIN - END block following.
               // if we were to parse these (but, sadly Borland RC extensions to readable
               // icons and bitmaps etc, we would need to implement a sort of "ungetline" so
               // that we were able to look at following line and eventually to return it back!

  // it may be only a DIALOG!
  unsigned long x,
                y,
                w,
                h;
  // from the dialog statement we should parse the X, Y, W, H ... skipping eventually
  // the load- and memory-options. We do that in a quite dirty way: assume that LAST four
  // fields are what we need!
  if(N < 6)
  {
      LSyntaxError:
    pf.Error( "RCOBJECT:ProcessStatement()", "DIALOG statement");
    return;
  }
  if( !fGetLong( command.GetSegment( N - 4), x, Symbols))
    x = 0L;
  if( !fGetLong( command.GetSegment( N - 3), y, Symbols))
    y = 0L;
  if( !fGetLong( command.GetSegment( N - 2), w, Symbols))
    w = 0L;
  if( !fGetLong( command.GetSegment( N - 1), h, Symbols))
    h = 0L;
  // parse it (added to rcfile in the call)
  ParseDialog( szName, (int )x, (int )y, (int )w, (int )h, Symbols, pf, rcfile);
  return;
}

// add dialog control instance to dialog instance
int rcDialog::Add( rcItem * prcit)
{
  if( wxlItems.Append( (wxObject *)prcit) != 0)
  {
    ++N;
    return 1;
  }
  else
    return 0;
}


// dtr
rcFile::~rcFile()
{
  if(N > 0)
  { wxNode *node = wxlDlgs.First();
    while( node)
    {
      delete (rcDialog *)node->Data();
      delete node;
      node = wxlDlgs.First();
    }
  }
}


// add a dialog instance to resource file
int rcFile::Add( rcDialog * prcd)
{
  if( wxlDlgs.Append( (wxObject *)prcd) != 0)
  {
    ++N;
    return 1;
  }
  else
    return 0;
}

// add menu instance to resource file instance
int rcFile::Add( rcMenuPopup * prcm)
{
  if( wxlMenus.Append( (wxObject *)prcm) != 0)
  {
    ++NMenus;
    return 1;
  }
  else
    return 0;
}

// this allows to use -Dsymbol options to be used by parser
static void ProcessDefines( SymbolTable &symbols, char *szOptions)
{ ParsedString options( szOptions, " ;\t\n");
  int          N = options.Segments();
  char        *s,
              *sOption;

  for( int i = 0 ; i < N ; ++i) {
    sOption = options.GetSegment( i);
    if(sOption[0] == '-' && sOption[1] ==  'D') {
      // -D means define
      sOption += 2; // Skip -D
      if(*sOption) {
        s = copystring(sOption);
        symbols.Add( s, 1L);   // define it with value 1L
        // note that the above is NOT memory leak: symbols.Add() deletes 's'!
      }
    }
  }
}

static int FindSymbolsPolicy( char *szOptions)
{
  ParsedString options( szOptions, " ;\t\n");
  int          N = options.Segments();

  for( int i = 0 ; i < N ; ++i)
    if( stricmp( options.GetSegment( i), "-S") == 0)
      return 1;
  return 0;
}



// this is the top-most parsing function.
// this function is supposed to be recursively called for nested rcinclude's
int rcFile::Read( char *szFileName, char *szParserFlags)
{ static int           iLevel     = 0; // keeps records of recursion level because of next item:
  static SymbolTable  *pSymbols = 0;
  // now the non-static stuff ...
  FILE                *pf;
  int                  success    = 1;  // generally, I am optimistic :-)
  Condition            fEnabled;        // used for layered #ifdef #ifndef etc.

  if (szFileName == NULL) return 0;

  if(iLevel == 0)  // we are entering the function first time!
  {
    N = NMenus = 0;

    top_dir = copystring( PathOnly(szFileName)  );

    if( (pSymbols = new SymbolTable) == 0)
      return 0;
    else {
     char *szRCInvoked = new char[16];

      strcpy( szRCInvoked, "RC_INVOKED");
      pSymbols->Add( szRCInvoked, 1L); // THIS should be defined by RC compiler
      ProcessDefines( *pSymbols, szParserFlags); // use up -Dxxx flags
      nSymbolFaultPolicy = FindSymbolsPolicy( szParserFlags);
    }
  }
  ++iLevel;        // we are in the "next" level now: the first one is '1'

  if( (pf = pfTryOpen( szFileName, szParserFlags)) == 0) { 
    success = 0;
    goto LEnd;
  } else {
    ParsedFile  Pfile( pf, szFileName);

    while( Pfile.ReadNextLine() > 0) {
      switch( Pfile.StripOutComments()) {
        case 0:    // some failure...
            LError:
          Pfile.Error( "rcFile.Read()", "Unexpected EOF");
          success = 0;
          goto LEnd;
        case 2:    // nothing left, so we could move on...
          continue;
        case 3:    // an comment block starts on this line (i.e. with "/*")
          while( Pfile.ReadNextLine())  // so we read lines until ...
          {
            switch( Pfile.StripOutComments())
            {
              case 0:                   // we get an error 8-(
                goto LError;
              case 1:                   // we get a valid, non-empty line ...
                if( Pfile.HasClosingComment()) // ... where comment block ends
                  goto LEOComment;
              // anything other forces the loop to continue
            }
          }
          // being here means we have read until end of file without end of
          // comment block. Which is an error!
          Pfile.Error( "rcFile.Read()", "Unexpected EOF before end of comment");
          success = 0;
          goto LEnd;

            LEOComment:
          continue;

        case 1:   // some real text remained after stripping comments!
          if(Pfile.IsPreprocessor())  //this is preprocessor line
            ProcessCommand( fEnabled, Pfile.GetTheString(), *pSymbols,
                            szParserFlags, Pfile);
          else                        // this must have something to do with resources ;-)
          {
            if( fEnabled.Set())       // only then we would bother to parser the file...
            {
              if( Pfile.IsRCInclude()) // this is rcinclude directive....
                if( !Read( szIncludeName( Pfile.GetTheString()), //parse recursively
                           szParserFlags))
                {
                  success = 0;
                  goto LEnd;
                }
                else
                  ;    // all is OK ; we have parsed the RCINCLUDEd file...
              else
              {
                ProcessStatement( *this, Pfile, *pSymbols, Pfile.GetTheString());
              }
            }
            else
              ;   // we do not parse the line...
          }
      }
    }
    fclose( pf);
  }
    LEnd:
  --iLevel;
  if(iLevel == 0) {
    delete pSymbols;
    delete []top_dir;
  }
  if(fEnabled.Level() > 0)
    return 0;
  else
    return success;
}

