#ifndef _RCPARSER_H_
#define _RCPARSER_H_  1

typedef void MESSAGE_FUNC( const char *what, const char *title);
typedef MESSAGE_FUNC  *PTR_MESSAGE_FUNC; //ptr to message function!
// these variables should be set to functions for reporting warnings / errors
// resulting from parsing process.
extern PTR_MESSAGE_FUNC  pmfWarning,
                         pmfError;

class Condition
{
    public:
  Condition(): N( 0) {}
  int Level ( void) { return N;}

  int Push  ( int fValue);
  int Pop   ( void);
  int Invert( void);
  int Set   ( void);

    private:
  int  N;
  int  fLevels[10];
} ;

class ParsedFile
// treats the (.RC) file as an ordinary text file consisting of individual lines.
// Well, it could be used as a stupid sort of parser, but it still contains few
// special methods.
{
    public:
  ParsedFile( FILE *file, const char *szFileName, int nMaxLineLength = 256);
 ~ParsedFile();

  int ReadNextLine     ( void);  // reads in ANY next line. Returns line length or 0 on EOF
                                 // or -1 on error...
  int SkipUntilEND     ( int FirstBeginRead = 0);
                // for skipping RC statement block: reads until "END" or "}" line
  int StripOutComments ( void);  // strips trailling comments (";" or "//" or "/*"
  // If anything left on line returns 1 (or 3, see further), if nothing left returns either 2
  // or 3(the latter is for the case of not matched "/*"). Returns 0 on failure..
  int HasClosingComment( void);  // the line contains (possibly after striping the "*/" sequence
  int IsPreprocessor   ( void);  // is this preprocessor command ?
  int IsBEGIN          ( void);  // is this beginning line of RC-block? ("BEGIN" or "{") ?
  int IsEND            ( void);  // is the end line of RC-block ("END" or "}") ?
  int IsRCInclude      ( void);
  char *GetTheString   ( void);  // returns current state of the line (after stripping, possibly)
  void  Warning( char *szWhere, char *szWhat);
  void  Error(   char *szWhere, char *szWhat);

    private:

  char  *s,
        *name;
  int    len;
  int    NL;
  FILE  *F;
};

class ParsedString
{
    public:
  ParsedString( char *str, char *separators);
 ~ParsedString();

  int   Segments  ( void) { return N; } 
  char *GetSegment( int i); 

    private:
  int     N;
  wxList  wxlSegments;
  char   *szBuf;
};

class SymbolTable : public wxHashTable
{
 DECLARE_CLASS(SymbolTable)
    public:
  SymbolTable(): wxHashTable( wxKEY_STRING, 1000){}
 ~SymbolTable() { Clear(); }  // this is not done in wxHashTable, I think...

  void  Add( char *symbol, unsigned long lValue)
  {
    Put( symbol,(wxObject *)lValue);
    delete []symbol;  // we base this on knowledge of 'symbol' being dynamically
    // allocated for this call, but being copied in the Put() method!
  }
  void  Remove( char *symbol) { Delete( symbol); }
  int   IsDefined( char *symbol) { return( Get( symbol) != 0) ; }
  unsigned long  SymbolValue( char *symbol)
  { wxObject *wxo = Get( symbol);

    if(wxo != 0)
      return (unsigned long )(wxo);
    else
      return 0L;    // we probably need to throw an exception here..
  } 
};



#endif
