/*
 * File:	wx_prop.h
 * Purpose:	Property sheet classes
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifndef wx_proph
#define wx_proph

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wxstring.h"
#include "wx_hash.h"
#include "wx_dialg.h"
#include "wx_frame.h"
#include "wx_buttn.h"
#include "wx_lbox.h"
#include "wx_txt.h"
#include "wx_gdi.h"
#include "wx_lay.h"

class wxWindow;
class wxProperty;
class wxPropertyValue;
class wxPropertySheet;
class wxPropertyView;
class wxPropertyValidator;
class wxPropertyValidatorRegistry;

#define wxPROPERTY_VERSION 1.1

// A storable sheet of values
class wxPropertySheet: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxPropertySheet)
 protected:
  wxObject *viewedObject;
  wxList properties;
  wxPropertyView *propertyView;

 public:
  wxPropertySheet(void);
  ~wxPropertySheet(void);

  // Add a property
  virtual void AddProperty(wxProperty *property);

  // Get property by name
  virtual wxProperty *GetProperty(wxString name);

  // Clear all properties
  virtual void Clear(void);

  virtual Bool Save(ostream& str);
  virtual Bool Load(ostream& str);

  virtual void UpdateAllViews(wxPropertyView *thisView = NULL);
  inline virtual wxList& GetProperties(void) { return properties; }
  
  // Sets/clears the modified flag for each property value
  virtual void SetAllModified(Bool flag = TRUE);
};


// Base class for property sheet views. There are currently two directly derived
// classes: wxPropertyListView, and wxPropertyFormView.
class wxPropertyView: public wxEvtHandler
{
 DECLARE_DYNAMIC_CLASS(wxPropertyView)
 protected:
  long buttonFlags;
  wxPropertySheet *propertySheet;
  wxProperty *currentProperty;
  wxList validatorRegistryList;
  wxPropertyValidator *currentValidator;
 public:
  wxPropertyView(long flags = 0);
  ~wxPropertyView(void);

  // Associates and shows the view
  virtual void ShowView(wxPropertySheet *propertySheet, wxPanel *panel) {};

  // Update this view of the viewed object, called e.g. by
  // the object itself.
  virtual Bool OnUpdateView(void) {return FALSE;};

  // Override this to do something as soon as the property changed,
  // if the view and validators support it.
  virtual void OnPropertyChanged(wxProperty *WXUNUSED(property)) {}

  virtual void AddRegistry(wxPropertyValidatorRegistry *registry);
  inline virtual wxList& GetRegistryList(void)
   { return validatorRegistryList; }

  virtual wxPropertyValidator *FindPropertyValidator(wxProperty *property);
  inline virtual void SetPropertySheet(wxPropertySheet *sheet) { propertySheet = sheet; }
  inline virtual wxPropertySheet *GetPropertySheet(void) { return propertySheet; }

  virtual void OnOk(void) {};
  virtual void OnCancel(void) {};
  virtual void OnHelp(void) {};

  inline virtual Bool OnClose(void) { return FALSE; }
  inline long GetFlags(void) { return buttonFlags; }
};


class wxPropertyValidator: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValidator)
 protected:
  long validatorFlags;
  wxProperty *validatorProperty;
 public:
  wxPropertyValidator(long flags = 0);
  ~wxPropertyValidator(void);

  inline long GetFlags(void) { return validatorFlags; }
  inline void SetValidatorProperty(wxProperty *prop) { validatorProperty = prop; }
  inline wxProperty *GetValidatorProperty(void) { return validatorProperty; }

  virtual Bool StringToFloat (char *s, float *number);
  virtual Bool StringToDouble (char *s, double *number);
  virtual Bool StringToInt (char *s, int *number);
  virtual Bool StringToLong (char *s, long *number);
  virtual char *FloatToString (float number);
  virtual char *DoubleToString (double number);
  virtual char *IntToString (int number);
  virtual char *LongToString (long number);
};


// extern wxPropertyValidator *wxDefaultPropertyValidator;

class wxPropertyValidatorRegistry: public wxHashTable
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValidatorRegistry)
 public:
  wxPropertyValidatorRegistry(void);
  ~wxPropertyValidatorRegistry(void);

  virtual void RegisterValidator(wxString& roleName, wxPropertyValidator *validator);
  virtual wxPropertyValidator *GetValidator(wxString& roleName);
  void ClearRegistry(void);
};

/*
 * Property value class
 */

typedef enum {
    wxPropertyValueNull,
    wxPropertyValueInteger,
    wxPropertyValueReal,
    wxPropertyValueBool,
    wxPropertyValueString,
    wxPropertyValueList,
    wxPropertyValueIntegerPtr,
    wxPropertyValueRealPtr,
    wxPropertyValueBoolPtr,
    wxPropertyValueStringPtr
} wxPropertyValueType;

class wxPropertyValue: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxPropertyValue)
 public:
  wxObject *client_data;
  wxPropertyValueType type;
  Bool modifiedFlag;

  union {
    long integer; // Also doubles as Bool
    char *string;
    float real;
    long *integerPtr;
    Bool *BoolPtr;
    char **stringPtr;
    float *realPtr;
    wxPropertyValue *first;  // If is a list expr, points to the first node
    } value;

  wxPropertyValue *next;     // If this is a node in a list, points to the next node
  wxPropertyValue *last;     // If is a list expr, points to the last node

  wxPropertyValue(void);                       // Unknown type
  wxPropertyValue(const wxPropertyValue& copyFrom);  // Copy constructor
  wxPropertyValue(char *val);
  wxPropertyValue(long val);
  wxPropertyValue(Bool val);
  wxPropertyValue(float val);
  wxPropertyValue(double the_real);
  wxPropertyValue(wxList *val);
  wxPropertyValue(wxStringList *val);
  // Pointer versions
  wxPropertyValue(char **val);
  wxPropertyValue(long *val);
  wxPropertyValue(Bool *val);
  wxPropertyValue(float *val);

  ~wxPropertyValue(void);

  virtual inline wxPropertyValueType Type(void) { return type; }
  virtual inline void SetType(wxPropertyValueType typ) { type = typ; }
  virtual long IntegerValue(void);
  virtual float RealValue(void);
  virtual Bool BoolValue(void);
  virtual char *StringValue(void);
  virtual long *IntegerValuePtr(void);
  virtual float *RealValuePtr(void);
  virtual Bool *BoolValuePtr(void);
  virtual char **StringValuePtr(void);

  // Get nth arg of clause (starting from 1)
  virtual wxPropertyValue *Arg(wxPropertyValueType type, int arg);

  // Return nth argument of a list expression (starting from zero)
  virtual wxPropertyValue *Nth(int arg);
  // Returns the number of elements in a list expression
  virtual int Number(void);

  virtual wxPropertyValue *NewCopy(void);
  virtual void Copy(wxPropertyValue& copyFrom);

  virtual void WritePropertyClause(ostream& stream);  // Write this expression as a top-level clause
  virtual void WritePropertyType(ostream& stream);    // Write as any other subexpression

  // Append an expression to a list
  virtual void Append(wxPropertyValue *expr);
  // Insert at beginning of list
  virtual void Insert(wxPropertyValue *expr);

  // Get first expr in list
  virtual inline wxPropertyValue *GetFirst(void) { return ((type == wxPropertyValueList) ? value.first : NULL); }

  // Get next expr if this is a node in a list
  virtual inline wxPropertyValue *GetNext(void) { return next; }

  // Get last expr in list
  virtual inline wxPropertyValue *GetLast(void) { return ((type == wxPropertyValueList) ? last : NULL); }
  
  // Delete this node from the list
  virtual void Delete(wxPropertyValue *node);

  // Clear list
  virtual void ClearList(void);

  virtual inline void SetClientData(wxObject *data) { client_data = data; }
  virtual inline wxObject *GetClientData(void) { return client_data; }

  virtual wxString GetStringRepresentation(void);
  
  inline void SetModified(Bool flag = TRUE) { modifiedFlag = flag; }
  inline Bool GetModified(void) { return modifiedFlag; }

  // Operators
  void operator=(const wxPropertyValue& val);
  void operator=(const char *val);
  void operator=(const long val);
  void operator=(const Bool val);
  void operator=(const float val);
  void operator=(const char **val);
  void operator=(const long *val);
  void operator=(const Bool *val);
  void operator=(const float *val);
};

/*
 * Property class: contains a name and a value.
 */

class wxProperty: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxProperty)
 protected:
  Bool enabled;
 public:
  wxPropertyValue value;
  wxString name;
  wxString propertyRole;
  wxPropertyValidator *propertyValidator;
  wxWindow *propertyWindow; // Usually a panel item, if anything

  wxProperty(void);
  wxProperty(wxProperty& copyFrom);
  wxProperty(wxString name, wxString role, wxPropertyValidator *ed = NULL);
  wxProperty(wxString name, const wxPropertyValue& val, wxString role, wxPropertyValidator *ed = NULL);
  ~wxProperty(void);

  virtual wxPropertyValue& GetValue(void);
  virtual wxPropertyValidator *GetValidator(void);
  virtual wxString& GetName(void);
  virtual wxString& GetRole(void);
  virtual void SetValue(wxPropertyValue val);
  virtual void SetValidator(wxPropertyValidator *v);
  virtual void SetName(wxString& nm);
  virtual void SetRole(wxString& role);
  void operator=(const wxPropertyValue& val);
  virtual inline void SetWindow(wxWindow *win) { propertyWindow = win; }
  virtual inline wxWindow *GetWindow(void) { return propertyWindow; }
  
  inline void Enable(Bool en) { enabled = en; }
  inline Bool IsEnabled(void) { return enabled; }
};

#endif
