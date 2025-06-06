/*
 * File:    demo.h
 * Purpose: main application module, generated by wxBuilder.
 * Author:  Alejandro Aguilar-Sierra
 * Date:    June 1995
 * Description:
 * A demo for the wxIma class library
 */

#ifndef _INC_DEMO_H
#define _INC_DEMO_H

/*
 * Forward declarations of all top-level window classes.
 */
class  FrameClass;

/*
 * Class representing the entire Application
 */
class AppClass: public wxApp
{
 public:
  FrameClass *frame;

  wxFrame *OnInit(void);
  Bool ParseCommandLine(void);
};

class CanvasClass;

/*
Main frame
*/
class FrameClass: public wxFrame
{
 private:
 protected:
   wxMenuBar *menuBar;
 public:
  // Subwindows for reference within the program.
  CanvasClass *canvas;

  // Constructor and destructor
  FrameClass(wxFrame *parent, char *title, int x, int y, int width, int height, long style, char *name);
  ~FrameClass(void);

 Bool OnClose(void);
 void OnSize(int w, int h);
 void OnMenuCommand(int commandId);
 void ComputeNewIma(void);
};

/* Menu identifiers
 */
enum {
	FRAMECLASS_FILE = 1,
	FRAMECLASS_OPEN,
	FRAMECLASS_SAVE,
	FRAMECLASS_SAVE_AS,
	FRAMECLASS_INFO,
	FRAMECLASS_EXIT,
	FRAMECLASS_OPTIONS,
	FRAMECLASS_DRAW,
	FRAMECLASS_STRETCH,
	FRAMECLASS_SPHERE,
	FRAMECLASS_HELP,
	FRAMECLASS_CONTENTS,
	FRAMECLASS_ABOUT
};

/* Toolbar identifiers
 */

class CanvasClass: public wxCanvas
{
 private:
 protected:
	wbIma *ima;
 public:
  // Constructor and destructor
  CanvasClass(wxFrame *parent, int x, int y, int width, int height, long style, char *name);
  ~CanvasClass(void);

 void OnSize(int w, int h);
 void OnPaint(void);
 void OnEvent(wxMouseEvent& event);
 void OnChar(wxKeyEvent& event);

 void SetIma(wbIma* imax)
 {
   ima = imax;
	if (ima->GetColourMap())
	  SetColourMap(ima->GetColourMap());
 }
 void AdjustScrollBars(void);
};

#endif /* mutual exclusion */
