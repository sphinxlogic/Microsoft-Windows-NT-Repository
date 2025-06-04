/*
 * File:	namegen.h
 * Purpose:	wxWindows GUI builder -- name generation
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef namegenh
#define namegenh

/*
 * Simulates block scope and generates sensible variable names according
 * to scope. I.e. we don't want variables like var13864, we want
 * say button1, button2, button3, listbox1, listbox2 within a given scope.
 *
 * NameSpace is a stack of scopes, pushed and popped with BeginBlock
 * and EndBlock. Each element contains a list of objects, one for each variable
 * name 'root', and with an ID count.
 *
 */

class NameSpaceVar: public wxObject
{
 public:
  char *name;
  long id;
  inline NameSpaceVar(char *theName)
  {
    if (theName) name = copystring(theName);
    else
      theName = NULL;
    id = 1;
  }
  inline ~NameSpaceVar(void)
  {
    if (name) delete[] name;
  }
};

class NameSpaceBlock: public wxList
{
 public:
  inline NameSpaceBlock(void):wxList(wxKEY_STRING) {}
  ~NameSpaceBlock(void);
};

class NameSpace: public wxList
{
  int indentLevel;
  long globalId;
 public:
  NameSpace(void);
  ~NameSpace(void);

  void BeginBlock(void);
  void EndBlock(void);

  char *MakeVariable(char *root);

  void ClearNameSpace(void);

  inline int GetIndentLevel(void) { return indentLevel; }
};

extern NameSpace nameSpace;

#endif // namegenh

