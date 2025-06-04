#ifndef  _RCOBJECT_H_
#define _RCOBJECT_H_  1


// *******************************  rcObject ***********************************

class  rcObject: public wxObject
{
 DECLARE_CLASS(rcObject)
    public:
// No public data members

// Public methods follow
  int                left   ( void) const { return x; }
  int                top    ( void) const { return y; }
  int                width  ( void) const { return w; }
  int                height ( void) const { return h; }
  const char *       caption( void) const { return szCaption ;}
  const char *       label  ( void) const { return szCaption ;}
  unsigned long      Style  ( void) const { return lStyle ;}

  Bool               FlagSet( unsigned long lAStyle) const
  {
    return (lStyle & lAStyle) != 0;
  }

    protected:
  // Protected data members
  int       x,            // object's left corner
            y,            // object's top  corner
            w,            // object's width
            h;            // object's height
  unsigned long  lStyle;  // MS-Windows specific style flags
  char     *szCaption;    // objects' caption...

  // Protected member functions

  // the only allowed constructor
  rcObject( int Xleft, int Ytop, int Width, int Height, unsigned long style, const char *caption):
     x( Xleft), y( Ytop), w( Width), h( Height), lStyle( style)
  {
    szCaption = (caption != 0) ? (new char [strlen( caption) + 1]) : 0;
    if( szCaption)
      strcpy( szCaption, caption);
  }

  // destructor
  ~rcObject()
  {
    if(szCaption != 0)
      delete []szCaption;
  }

    private:
  // in this class used essentially only for forbidding certain things with the objects...
  rcObject(){};                            // def-ctr prohibited
  rcObject( const rcObject &rciAnother){};   // cpy-ctr prohibited
//  rcObject &operator=( const rcObject &){ };  // assignement operator prohibited!
};


// **************************  rcItem ***********************************

class rcItem: public rcObject
{
 DECLARE_CLASS(rcItem)
    public:
  // public enum typedef
  enum  RCITEMTYPE { rcCHECKBOX = 0, rcCOMBOBOX, rcCTEXT,      rcDEFPUSHBUTTON,
                     rcEDITTEXT,  rcGROUPBOX,   rcICON,        rcLISTBOX,
                     rcLTEXT,     rcPUSHBUTTON, rcRADIOBUTTON, rcRTEXT,
                     rcSCROLLBAR, rcSTATIC
                   };

  // public functions

  // ctr...
  rcItem( const char *szLabel, RCITEMTYPE rcType, unsigned Id, unsigned long style,
          int   x, int   y, int   w, int   h, char *szIDname) :
    rcObject( x, y, w, h, style, szLabel), type( rcType), id( Id)
  {
    if(szIDname != 0)
    {
      szNameID = new char[ strlen( szIDname) + 1];
      strcpy( szNameID, szIDname);
    }
    else
      szNameID = 0;
  }

 ~rcItem()         // dtr...
 {
   if(szNameID != 0)
     delete []szNameID;
 }

  unsigned     ID( void) const { return id; }
  RCITEMTYPE   ItemType( void) { return type; }
  const char  *szItemType( void);
  Bool         HasNamedID( void) { return (szNameID != 0); }
  const char  *szIdName( void) { return szNameID; }

    private:
  // data members: I do not expect somebody to derive from this simple class ;-)
  RCITEMTYPE  type;
  unsigned    id;
  char       *szNameID;

  // now disabled things. Possibly, this IS superfluous for ctrs, as the default ctr for
  // base class is already prohibited
  rcItem();
  rcItem( const rcItem &);
  rcItem &operator=( rcItem &);
};

// ************************** rcDialog ************************************


class  rcDialog: public rcObject
{
 DECLARE_CLASS(rcDialog)
    public:
  rcDialog( const char *name, char *caption, int x, int y, int w, int h, unsigned long style) :
    rcObject( x,y, w, h, style, caption), N(0)
  {
    szName = (name != 0) ? (new char [strlen( name) + 1]) : 0;
    if(szName != 0)
      strcpy( szName, name);
  }

  ~rcDialog();

  int  Add( rcItem *prcit);

  char *   dlgName( void){ return szName; }
  int      nItems( void) { return N; }
  rcItem * getIthItem ( int i)
  {
    if((i >= 0) && (i < N))
      return (rcItem *)((wxlItems.Nth( i))->Data());
    else
      return 0;
  }

    private:
  int     N;
  wxList  wxlItems;
  char   *szName;

};

// ************************* rcMenuItem *************************************

class rcMenuItem: public wxObject
{
 DECLARE_CLASS(rcMenuItem)
    public:
  enum RCMENUOPTION { rcMENUPOPUP = 1, rcCHECKED = 2, rcGRAYED = 4, rcHELP = 8,
                      rcINACTIVE  = 16, rcMENUBARBREAK = 32, rcMENUBREAK = 64,
                      rcSEPARATOR = 128};

  virtual Bool  IsPopup( void) { return FALSE;}
  virtual rcMenuItem *rcGetIthChild( int i) { return 0; }
  virtual Bool  AddChild( rcMenuItem *child) { return 0;}

  int   GetID( void)     { return ID; }
  char *GetName( void)   { return szLabel;}
  Bool  IsChecked( void) { return (options & rcCHECKED) != 0;}
  Bool  IsGrayed( void)  { return (options & rcGRAYED) != 0; }
  Bool  IsHelp( void)    { return (options & rcHELP) != 0;}
  Bool  IsInactive( void){ return (options & rcINACTIVE) != 0;}
  Bool  IsMenuBreak( void){ return (options & rcMENUBREAK) != 0;}
  Bool  IsMenuBarBreak( void) { return (options & rcMENUBARBREAK) != 0;}
  Bool  IsSeparator( void) { return (options & rcSEPARATOR) != 0; }
  Bool  HasNamedID( void) { return (szID != 0); }
  const char *szIdName( void) { return szID; }

  rcMenuItem( const char *szName, int Id = -1, RCMENUOPTION rcopt = (RCMENUOPTION )0,
              const char *szIDname = 0);
  virtual ~rcMenuItem();

    protected:
  int           ID;
  char         *szLabel;
  RCMENUOPTION  options;
  char         *szID;
};

// *********************** rcMenuPopup ***********************************

class rcMenuPopup: public rcMenuItem
{
 DECLARE_CLASS(rcMenuPopup)
    public:
  virtual Bool IsPopup( void) { return TRUE;}
  virtual rcMenuItem *rcGetIthChild( int i);
  virtual Bool AddChild( rcMenuItem *child);

  int     NChilds( void) { return N;}

  rcMenuPopup( char *szName, RCMENUOPTION rcopt = (RCMENUOPTION )0):
    rcMenuItem( szName, -1, (RCMENUOPTION)(rcMENUPOPUP | rcopt), 0)
  { N = 0; }
  ~rcMenuPopup();
    private:
  int     N;
  wxList  wxlChilds;
};

// ************************** rcFile ***************************************

class rcFile: public  wxObject
{
 DECLARE_CLASS(rcFile)
    public:
  // def.ctr.
  rcFile(): N(0), NMenus( 0) {}
  // def.dtr.
  ~rcFile();

  // read-in an .RC file ....
  int  Read     ( char *szFileName, char *szParserFlags = "");
  // number of dialogs parsed into...
  int  nDialogs ( void) const { return N; }
    // return ptr to one of them...
  rcDialog * GetIthDialog( int i)
  {
    if( (i >= 0) && (i < N))
      return (rcDialog *)((wxlDlgs.Nth( i))->Data());
    else
      return 0;
  }
  // add an dialog: used by the Read() member as well as outside, while EXPORTING dlgs...
  int  Add( rcDialog *prcd);

  int          nMenus( void) const { return NMenus; }
  rcMenuPopup *GetIthMenu( int i)
  {
    if( (i >= 0) && (i < NMenus))
      return (rcMenuPopup *)((wxlMenus.Nth( i))->Data());
    else
      return 0;
  }
  int  Add( rcMenuPopup *prcm);

  // write the current parsed RC file backed into an RC_FORM: no symbols are used...
  int  Write( char *szFileName) const; // not implemented yet :-(

    private:
  // data members...
  wxList wxlDlgs;
  wxList wxlMenus;
  int    N;
  int    NMenus;
};

#endif

