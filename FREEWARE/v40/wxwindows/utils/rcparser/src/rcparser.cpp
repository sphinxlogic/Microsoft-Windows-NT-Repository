/* ***********************************************************************
 * *******   RCPARSER.CC: Part of the rcParser library for wxWin toolkit *
 * *******   se at RCOBJECT.CC beginning for more information
 * ********************************************************************** */
static char RCS_id[] = "$Id: rcparser.cc,v 1.2 1994/08/22 21:20:02 edz Exp edz $";

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx_defs.h>
#include <wx_list.h>
#include <wx_utils.h>
#include <wb_main.h>
#endif

#include <wx_hash.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rcparser.h"

#ifdef wx_x
# define stricmp strcasecmp
extern "C" stricmp(const char *, const char *);
#endif

IMPLEMENT_CLASS(SymbolTable, wxHashTable)

// ********************* CLASS Condition **************************************

int Condition::Push( int fValue)
{
  if(N < sizeof(fLevels)/sizeof(fLevels[0]) ) {
    fLevels[N++] = (fValue != 0);
    return TRUE;
  }
  return FALSE;
}

int Condition::Pop( void)
{
  if(N > 0) {
    --N;
    return TRUE;
  }
  return FALSE;
}

int Condition::Invert( void)
{
  if(N > 0) {
    fLevels[N - 1] = ! fLevels[N - 1];
    return TRUE;
  }
  return FALSE;
}

int Condition::Set( void)
{ 
  for( int i = 0 ; i < N ; ++i)
    if(fLevels[i] == 0)
      return FALSE;

  return TRUE;
}

// **************** END OF CLASS Condition DEFINITIONS ****************************

// ***************** ParsedFile CLASS DEFINITION OF MEMBERS ***********************


PTR_MESSAGE_FUNC  pmfWarning = 0,
                  pmfError   = 0;


ParsedFile::ParsedFile( FILE *f, const char *szFileName, int nMaxLineLength)
{
  len  = nMaxLineLength;
  s    = new char [len + 1];
  F    = f;
  s[0] = '\0';

  name = copystring( szFileName);
  NL   = 0;
}

ParsedFile::~ParsedFile()
{
  if(s != 0)
    delete []s;
}


// Strip RC new line characters (Unix/Mac/Dos conventions)
static inline void stripRcNL(char *s)
{
  int scan_white = 1;

  // Strip new line characters (Unix/Mac/Dos conventions)
  for (char *tcp = s + strlen(s) - 1; tcp >= s; tcp--) {
    if (*tcp == '\r' || *tcp == '\n' ||
  // Zap also trailing spaces and tabs
       (scan_white && (*tcp == '\t' || *tcp == ' ')) ) *tcp = '\0';
    else scan_white = 0;
  }
}

// Readline code for RC files!
int  ParsedFile::ReadNextLine( void)
{
  // default values!
  if( pmfWarning == NULL) pmfWarning = wxError;
  if( pmfError == NULL)   pmfError   = wxFatalError;

  char *buffer = s;
  int   buffer_len = len;

  for(;;) {
    if( fgets( buffer, buffer_len, F) == NULL)
      return (ferror( F) ? -1 : 0);
    ++NL; // increase Number of Lines read

    stripRcNL( buffer );

    // Handle Continuation lines
    size_t tail = strlen(buffer) - 1;
    // MS AppStudio generates Multi-line RC files!
    // Lines ending in ',' or '|' are continued on next line
    // In dialogs also 'NOT' is used--- but don't support it yet! 
    if (buffer[0] == '\0' || (buffer[tail] != ',' && buffer[tail] != '|'))
       break;  // Line seems to have been complete!
    buffer += tail + 1;
    buffer_len -= tail + 1;
    if (buffer_len <= 0) {
       Warning("RC Parser", "Line too long, capacity exceeded");
       break;
    }
  } // for(;;)

  if (s[0] == '\0') {
    // Lines with length 0 signal EOF so...
    s[0] = ' ';
    s[1] = '\0';
  }
  return (int)strlen(s);
}


int ParsedFile::SkipUntilEND( int FirstBeginRead)
// skips to the end of the block(inclusive). Understands nested BEGIN - END pairs!
// PLAY RULES: we enter this with COMPLETE nested block, i.e. including the
//             starting BEGIN / "{"!
{ int   nLevel = 1;             // entering with the level of 1 ...

  if( !FirstBeginRead)
  {
    while( ReadNextLine() > 0)  // read lines until...
      if( IsBEGIN())            // ... we find the starting BEGIN mark
        break;
    if( !IsBEGIN())
      return 0;
  }
  do {
    if( ReadNextLine() <= 0)
      return 0;
    if( !StripOutComments())
      return 0;
    if( IsBEGIN())
      ++nLevel;
    else if( IsEND())
      --nLevel;
  } while(nLevel > 0);
  return 1;
}

static char *Xstrchr( const char *s, int c)
{ char  *ptr;
  int    i,
         len = strlen( s),
         fIn = 0;

  for( i = 0, ptr = (char *)s ; i < len ; ++i, ++ptr)
    if(*ptr == '\"')  // start of string specification, where 'c' occurrence is ignored
      fIn = !fIn;     // check if unchecked and vice versa
    else if(*ptr == c)
      if(!fIn)
        return ptr;
  return 0;
}

int  ParsedFile::StripOutComments( void)
{ char *ptr;
  int   fOpened = 0;

  // the order is important: first we check for usual C -style comment...
  if( (ptr = Xstrchr( s, '/')) != 0)
  {
    if(ptr[1] == '*')  // we got it
    {
      if( strstr( ptr + 2, "*/") == 0)
        fOpened = 1;
      *ptr = 0;
    }
  }

  if( (ptr = Xstrchr( s, ';')) != 0)  // if anywhere except in label specification...
    *ptr = 0;
  if( (ptr = Xstrchr( s, '/')) != 0)
  {
    if(ptr[1] == '/')   // so we get the "//" sequence!
      *ptr = 0;
  }
  // so, now:
  if( fOpened)
    return 3;
  if(s[0] == 0)
    return 2;
 else
    return 1;
  // hmm, seems we cannot get error return value (0)!
}

int ParsedFile::HasClosingComment( void)
{
  return( strstr( s, "*/") != 0);
}

int ParsedFile::IsPreprocessor( void)
{ char *ptr = s;

  while( (*ptr == ' ') || (*ptr == '\t'))
    ++ptr;
  return (*ptr == '#');
}

int ParsedFile::IsBEGIN( void)
{
  char *szBuf = copystring( s );
  char *str = strtok( szBuf, " \t\n");
  int is_begin = (str && ((str[0] == '{' && str[1] == '\0') || stricmp(str, "BEGIN") == 0));
  delete[] szBuf;
  return is_begin;
}

int ParsedFile::IsEND( void)
{
  char *szBuf = copystring( s );
  char *str = strtok( szBuf, " \t\n");
  int is_end = (str && ((str[0] == '}' && str[1] == '\0') || stricmp(str, "END") == 0));
  delete[] szBuf;
  return is_end;
}

int ParsedFile::IsRCInclude( void)
{ char *szBuf = new char[len + 1],
       *ptr;

  strcpy( szBuf, s);
  if ((ptr = strtok( szBuf, " \t")) != NULL)
    if( stricmp( ptr, "rcinclude") == 0)
      if( strtok( 0, " \t\n") != NULL) {
        delete []szBuf;
        return 1;
      } else
        ;     // this is a syntax error, so what about reporting it?
  delete []szBuf;
  return 0;
}

char *ParsedFile::GetTheString( void)
{
  return s;
}


void ParsedFile::Warning( char *szWhere, char *szWhat)
{ char *szBuf = new char[ strlen( szWhere) + strlen( szWhat) + 128];

  if(szBuf == 0)
    return;
  sprintf( szBuf, "File '%s'%c Line %i\n%s:%c '%s'",
                  name, (strlen( name)>40) ? '\n' : ' ', NL,
                  szWhere, (strlen( szWhat)>40) ? '\n' : ' ', szWhat);
  pmfWarning( szBuf, "rcParser Library Warning");
  delete []szBuf;
  return;
}

void ParsedFile::Error( char *szWhere, char *szWhat)
{ char *szBuf = new char[ strlen( szWhere) + strlen( szWhat) + 128];

  if(szBuf == 0)
    wxExit();
  sprintf( szBuf, "File '%s'%c Line %i\n%s:%c '%s'",
                  name, (strlen( name)>40) ? '\n' : ' ', NL,
                  szWhere, (strlen( szWhat)>40) ? '\n' : ' ', szWhat);
  pmfError( szBuf, "rcParser Library Error");
  delete []szBuf;
  return;
}


// ************ END OF ParsedFile CLASS DEFINITIONS *************************


// *********************** CLASS ParsedString DEFINITIONS *********************

ParsedString::ParsedString( char *str, char *separators)
{
  szBuf = copystring( str );
  N     = 0;
  char *ptr = strtok( szBuf, separators);
  while(ptr != NULL) {
    wxlSegments.Append( (wxObject *)ptr);
    ++N;
    ptr = strtok( 0, separators);
  }
}

ParsedString::~ParsedString()
{
  wxlSegments.Clear();
  if(szBuf != 0)
    delete []szBuf;
}

char * ParsedString::GetSegment( int i)
{
  if((i < 0) || (i >= N) || (szBuf == 0))
    return 0;
  return (char *) ((wxlSegments.Nth( i))->Data());
}

// ***************************************************************************************

