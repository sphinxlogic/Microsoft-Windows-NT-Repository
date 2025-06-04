/*
 * tex2rtf.h
 *
 * Main include for all converters
 *
 */

#ifndef NO_GUI
// Define a new application type
class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    wxTextWindow *textWindow;
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnMenuCommand(int id);
    Bool OnClose(void);
};

#ifdef wx_msw
#include "wx_ipc.h"

class Tex2RTFConnection: public wxConnection
{
 public:
  Tex2RTFConnection(char *buf, int size);
  ~Tex2RTFConnection(void);
  Bool OnExecute(char *topic, char *data, int size, int format);
  char *OnRequest(char *topic, char *item, int *size, int format);
//  Bool OnPoke(char *topic, char *item, char *data, int size, int format);
};

class Tex2RTFServer: public wxServer
{
 public:
  wxConnection *OnAcceptConnection(char *topic);
};

#endif // wx_msw

#endif // NO_GUI

/*
 * Itemize/enumerate structure: put on a stack for
 * getting the indentation right
 *
 */

#define LATEX_ENUMERATE   1
#define LATEX_ITEMIZE     2
#define LATEX_DESCRIPTION 3
#define LATEX_TWOCOL      5
#define LATEX_INDENT      6

class ItemizeStruc: public wxObject
{
  public:
    int listType;
    int currentItem;
    int indentation;
    int labelIndentation;
    inline ItemizeStruc(int lType, int indent = 0, int labIndent = 0)
    { listType = lType; currentItem = 0;
      indentation = indent; labelIndentation = labIndent; }
};

// ID for the menu quit command
#define TEX_QUIT 1
#define TEX_GO   2

#define TEX_SET_INPUT   3
#define TEX_SET_OUTPUT  4

#define TEX_VIEW_LATEX  5
#define TEX_VIEW_OUTPUT 6

#define TEX_VIEW_CUSTOM_MACROS 7
#define TEX_LOAD_CUSTOM_MACROS 8

#define TEX_MODE_RTF    9
#define TEX_MODE_WINHELP 10
#define TEX_MODE_HTML   11
#define TEX_MODE_XLP    12

#define TEX_HELP        13
#define TEX_ABOUT       14
#define TEX_SAVE_FILE   15

extern TexChunk *currentMember;
extern Bool startedSections;
extern char *contentsString;
extern Bool suppressNameDecoration;
extern wxList itemizeStack;

extern FILE *Contents;
extern FILE *Chapters;
extern FILE *Sections;
extern FILE *Subsections;
extern FILE *Subsubsections;

extern char *InputFile;
extern char *OutputFile;
extern char *MacroFile;

extern char *FileRoot;
extern char *ContentsName;    // Contents page from last time around
extern char *TmpContentsName; // Current contents page
extern char *TmpFrameContentsName; // Current frame contents page
extern char *WinHelpContentsFileName; // WinHelp .cnt file
extern char *RefName;         // Reference file name
extern char *bulletFile;

#ifndef NO_GUI
void ChooseOutputFile(Bool force = FALSE);
void ChooseInputFile(Bool force = FALSE);
#endif

void RTFOnMacro(int macroId, int no_args, Bool start);
Bool RTFOnArgument(int macroId, int arg_no, Bool start);

void HTMLOnMacro(int macroId, int no_args, Bool start);
Bool HTMLOnArgument(int macroId, int arg_no, Bool start);

void XLPOnMacro(int macroId, int no_args, Bool start);
Bool XLPOnArgument(int macroId, int arg_no, Bool start);

Bool RTFGo(void);
Bool HTMLGo(void);
Bool XLPGo(void);

#define ltHARDY         10000
