/*
 * File:     tex2rtf.cc
 * Purpose:  Converts Latex to linear/WinHelp RTF, HTML, wxHelp.
 *
 *                       wxWindows 1.50
 * Copyright (c) 1993 Artificial Intelligence Applications Institute,
 *                   The University of Edinburgh
 *
 *                     Author: Julian Smart
 *                        Date: 7-9-93
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice, author statement and this permission
 * notice appear in all copies of this software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE ARTIFICIAL INTELLIGENCE APPLICATIONS INSTITUTE OR THE
 * UNIVERSITY OF EDINBURGH BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF
 * DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
	Last change:  JS   17 Feb 97    8:22 pm
 */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#ifndef NO_GUI
#include <wx_help.h>
#include <wx_timer.h>
#endif
#ifdef NO_GUI
#include <iostream.h>
#endif

#include <ctype.h>
#include <stdlib.h>
#include "tex2any.h"
#include "tex2rtf.h"
#include "rtfutils.h"

#if (defined(wx_x) && !defined(NO_GUI))
#include "aiai.xbm"
#endif

const float versionNo = 1.61;

TexChunk *currentMember = NULL;
Bool startedSections = FALSE;
char *contentsString = NULL;
Bool suppressNameDecoration = FALSE;
Bool OkToClose = TRUE;
int passNumber = 1;

#ifndef NO_GUI
wxHelpInstance *HelpInstance = NULL;

#ifdef wx_msw
static char *ipc_buffer = NULL;
static char Tex2RTFLastStatus[100];
Tex2RTFServer *TheTex2RTFServer = NULL;
#endif
#endif

char *bulletFile = NULL;

FILE *Contents = NULL;   // Contents page
FILE *Chapters = NULL;   // Chapters (WinHelp RTF) or rest of file (linear RTF)
FILE *Sections = NULL;
FILE *Subsections = NULL;
FILE *Subsubsections = NULL;
FILE *Popups = NULL;
FILE *WinHelpContentsFile = NULL;

char *InputFile = NULL;
char *OutputFile = NULL;
char *MacroFile = copystring("tex2rtf.ini");

char *FileRoot = NULL;
char *ContentsName = NULL;    // Contents page from last time around
char *TmpContentsName = NULL; // Current contents page
char *TmpFrameContentsName = NULL; // Current frame contents page
char *WinHelpContentsFileName = NULL; // WinHelp .cnt file
char *RefName = NULL;         // Reference file name

char *RTFCharset = copystring("ansi");

#ifdef wx_msw
int BufSize = 60;             // Size of buffer in K
#else
int BufSize = 500;
#endif

Bool Go(void);
void ShowOptions(void);

#ifdef NO_GUI
int main(int argc, char **argv)
#else
wxMenuBar *menuBar = NULL;
MyFrame *frame = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initializes the whole application and calls OnInit
MyApp myApp;

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
#endif
{
  // Use default list of macros defined in tex2any.cc
  DefineDefaultMacros();
  AddMacroDef(ltHARDY, "hardy", 0);

  FileRoot = new char[300];
  ContentsName = new char[300];
  TmpContentsName = new char[300];
  TmpFrameContentsName = new char[300];
  WinHelpContentsFileName = new char[300];
  RefName = new char[300];

  int n = 1;
  
  // Read input/output files
  if (argc > 1)
  {
    if (argv[1][0] != '-')
    {
      InputFile = argv[1];
      n ++;

      if (argc > 2)
      {
        if (argv[2][0] != '-')
        {
          OutputFile = argv[2];
          n ++;
        }
      }
    }
  }

#ifdef NO_GUI
  if (!InputFile || !OutputFile)
  {
    cout << "Tex2RTF: input or output file is missing.\n";
    ShowOptions();
    exit(1);
  }
#endif
  if (InputFile)
  {
    TexPathList.EnsureFileAccessible(InputFile);
  }
  if (!InputFile || !OutputFile)
    isInteractive = TRUE;

  for (int i = n; i < argc;)
  {
    if (strcmp(argv[i], "-winhelp") == 0)
    {
      i ++;
      convertMode = TEX_RTF;
      winHelp = TRUE;
    }
#ifndef NO_GUI
    else if (strcmp(argv[i], "-interactive") == 0)
    {
      i ++;
      isInteractive = TRUE;
    }
#endif
    else if (strcmp(argv[i], "-sync") == 0)  // Don't yield
    {
      i ++;
      isSync = TRUE;
    }
    else if (strcmp(argv[i], "-rtf") == 0)
    {
      i ++;
      convertMode = TEX_RTF;
    }
    else if (strcmp(argv[i], "-html") == 0)
    {
      i ++;
      convertMode = TEX_HTML;
    }
    else if (strcmp(argv[i], "-xlp") == 0)
    {
      i ++;
      convertMode = TEX_XLP;
    }
    else if (strcmp(argv[i], "-twice") == 0)
    {
      i ++;
      runTwice = TRUE;
    }
    else if (strcmp(argv[i], "-macros") == 0)
    {
      i ++;
      if (i < argc)
      {
        MacroFile = copystring(argv[i]);
        i ++;
      }
    }
    else if (strcmp(argv[i], "-bufsize") == 0)
    {
      i ++;
      if (i < argc)
      {
        BufSize = atoi(argv[i]);
        i ++;
      }
    }
    else if (strcmp(argv[i], "-charset") == 0)
    {
      i ++;
      if (i < argc)
      {
        char *s = argv[i];
        i ++;
        if (strcmp(s, "ansi") == 0 || strcmp(s, "pc") == 0 || strcmp(s, "mac") == 0 ||
            strcmp(s, "pca") == 0)
          RTFCharset = copystring(s);
        else
        {
          OnError("Incorrect argument for -charset");
        }
      }
    }
    else
    {
      char buf[100];
      sprintf(buf, "Invalid switch %s.\n", argv[i]);
      OnError(buf);
      i++;
#ifdef NO_GUI
      ShowOptions();
      exit(1);
#endif
    }
  }

#if defined(wx_msw) && !defined(NO_GUI)
  wxIPCInitialize();
  Tex2RTFLastStatus[0] = 0; // DDE connection return value
  TheTex2RTFServer = new Tex2RTFServer;
  TheTex2RTFServer->Create("TEX2RTF");
#endif

#if defined(wx_msw) && !defined(WIN32)
  // Limit to max Windows array size
  if (BufSize > 64) BufSize = 64;
#endif

  TexInitialize(BufSize);
  ResetContentsLevels(0);

#ifndef NO_GUI

  if (isInteractive)
  {
    char buf[100];

    // Create the main frame window
    frame = new MyFrame(NULL, "Tex2RTF", 0, 0, 400, 300);
    frame->CreateStatusLine(2);
    // Give it an icon
#ifdef wx_msw
    wxIcon *icon = new wxIcon("tex2rtf");
#endif
#ifdef wx_x
    wxIcon *icon = new wxIcon(aiai_bits, aiai_width, aiai_height);
#endif

    frame->SetIcon(icon);

    if (InputFile)
    {
      sprintf(buf, "Tex2RTF [%s]", FileNameFromPath(InputFile));
      frame->SetTitle(buf);
    }

    // Make a menubar
    wxMenu *file_menu = new wxMenu;
    file_menu->Append(TEX_GO, "&Go",                        "Run converter");
    file_menu->Append(TEX_SET_INPUT, "Set &Input File",     "Set the LaTeX input file");
    file_menu->Append(TEX_SET_OUTPUT, "Set &Output File",   "Set the output file");
    file_menu->AppendSeparator();
    // Only 32K in MSW, so forget it.
#ifndef wx_msw
    file_menu->Append(TEX_VIEW_LATEX, "View &LaTeX File",   "View the LaTeX input file");
    file_menu->Append(TEX_VIEW_OUTPUT, "View Output &File", "View output file");
#endif
    file_menu->Append(TEX_SAVE_FILE, "&Save log file", "Save displayed text into file");
    file_menu->AppendSeparator();
    file_menu->Append(TEX_QUIT, "E&xit",                    "Exit Tex2RTF");

    wxMenu *macro_menu = new wxMenu;

    macro_menu->Append(TEX_LOAD_CUSTOM_MACROS, "&Load Custom Macros", "Load custom LaTeX macro file");
    macro_menu->Append(TEX_VIEW_CUSTOM_MACROS, "View &Custom Macros", "View custom LaTeX macros");

    wxMenu *mode_menu = new wxMenu;

    mode_menu->Append(TEX_MODE_RTF, "Output linear &RTF",   "Wordprocessor-compatible RTF");
    mode_menu->Append(TEX_MODE_WINHELP, "Output &WinHelp RTF", "WinHelp-compatible RTF");
    mode_menu->Append(TEX_MODE_HTML, "Output &HTML",        "HTML World Wide Web hypertext file");
    mode_menu->Append(TEX_MODE_XLP, "Output &XLP",          "wxHelp hypertext help file");

    wxMenu *help_menu = new wxMenu;

    help_menu->Append(TEX_HELP, "&Help", "Tex2RTF Contents Page");
    help_menu->Append(TEX_ABOUT, "&About Tex2RTF", "About Tex2RTF");

    menuBar = new wxMenuBar;
    menuBar->Append(file_menu, "&File");
    menuBar->Append(macro_menu, "&Macros");
    menuBar->Append(mode_menu, "&Conversion Mode");
    menuBar->Append(help_menu, "&Help");

    frame->SetMenuBar(menuBar);
    frame->textWindow = new wxTextWindow(frame, -1, -1, -1, -1, wxNATIVE_IMPL|wxREADONLY);

    (*frame->textWindow) << "Welcome to Julian Smart's LaTeX to RTF converter.\n";
//    ShowOptions();    

    HelpInstance = new wxHelpInstance(TRUE);
    HelpInstance->Initialize("tex2rtf");

    /*
     * Read macro/initialisation file
     *
     */
   
    char *path;
    if (path = TexPathList.FindValidPath(MacroFile))
      ReadCustomMacros(path);

    strcpy(buf, "In ");

    if (winHelp && (convertMode == TEX_RTF))
      strcat(buf, "WinHelp RTF");
    else if (!winHelp && (convertMode == TEX_RTF))
      strcat(buf, "linear RTF");
    else if (convertMode == TEX_HTML) strcat(buf, "HTML");
    else if (convertMode == TEX_XLP) strcat(buf, "XLP");
      strcat(buf, " mode.");
    frame->SetStatusText(buf, 1);
  
    frame->Show(TRUE);
    return frame;
  }
  else
#endif // NO_GUI
  {
    /*
     * Read macro/initialisation file
     *
     */
   
    char *path;
    if (path = TexPathList.FindValidPath(MacroFile))
      ReadCustomMacros(path);

    Go();
    if (runTwice) Go();
#ifdef NO_GUI
    return 0;
#else
    return NULL;
#endif
  }

#ifndef NO_GUI
  // Return the main frame window
  return frame;
#else
  return 0;
#endif
}

void ShowOptions(void)
{
    char buf[100];
    sprintf(buf, "Tex2RTF version %.2f", versionNo);
    OnInform(buf);
    OnInform("Usage: tex2rtf [input] [output] [switches]\n");
    OnInform("where valid switches are");
    OnInform("    -interactive");
    OnInform("    -bufsize <size in K>");
    OnInform("    -charset <pc | pca | ansi | mac> (default ansi)");
    OnInform("    -twice");
    OnInform("    -sync");
    OnInform("    -macros <filename>");
    OnInform("    -winhelp");
    OnInform("    -rtf");
    OnInform("    -html");
    OnInform("    -xlp\n");
}

#ifndef NO_GUI
// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{}

Bool MyFrame::OnClose(void)
{
  if (!stopRunning && !OkToClose)
  {
    stopRunning = TRUE;
    runTwice = FALSE;
    return FALSE;
  }
  else if (OkToClose)
  {
#ifdef wx_msw
    delete TheTex2RTFServer;
    wxIPCCleanUp();
#endif
    return TRUE;
  }
  return FALSE;
}
 
// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id) {
    case TEX_QUIT:
      delete this;
      break;
    case TEX_GO:
    {
      menuBar->EnableTop(0, FALSE);
      menuBar->EnableTop(1, FALSE);
      menuBar->EnableTop(2, FALSE);
      menuBar->EnableTop(3, FALSE);
      textWindow->Clear();
      Tex2RTFYield(TRUE);
      Go();

      if (runTwice)
      {
        Tex2RTFYield(TRUE);
        Go();
      }
      menuBar->EnableTop(0, TRUE);
      menuBar->EnableTop(1, TRUE);
      menuBar->EnableTop(2, TRUE);
      menuBar->EnableTop(3, TRUE);
      break;
    }
    case TEX_SET_INPUT:
    {
      ChooseInputFile(TRUE);
      break;
    }
    case TEX_SET_OUTPUT:
    {
      ChooseOutputFile(TRUE);
      break;
    }
    case TEX_SAVE_FILE:
    {
      char *s = wxFileSelector("Save text to file", NULL, NULL, "txt", "*.txt");
      if (s)
      {
        textWindow->SaveFile(s);
        char buf[350];
        sprintf(buf, "Saved text to %s", s);
        frame->SetStatusText(buf, 0);
      }
      break;
    }
    case TEX_VIEW_OUTPUT:
    {
      ChooseOutputFile();
      if (OutputFile && FileExists(OutputFile))
      {
        textWindow->LoadFile(OutputFile);
        char buf[300];
        sprintf(buf, "Tex2RTF [%s]", FileNameFromPath(OutputFile));
        frame->SetTitle(buf);
      }
      break;
    }
    case TEX_VIEW_LATEX:
    {
      ChooseInputFile();
      if (InputFile && FileExists(InputFile))
      {
        textWindow->LoadFile(InputFile);
        char buf[300];
        sprintf(buf, "Tex2RTF [%s]", FileNameFromPath(InputFile));
        frame->SetTitle(buf);
      }
      break;
    }
    case TEX_LOAD_CUSTOM_MACROS:
    {
      textWindow->Clear();
      char *s = wxFileSelector("Choose custom macro file", PathOnly(MacroFile), FileNameFromPath(MacroFile), "ini", "*.ini");
      if (s)
      {
        MacroFile = copystring(s);
        ReadCustomMacros(s);
        ShowCustomMacros();
      }
      break;
    }
    case TEX_VIEW_CUSTOM_MACROS:
    {
      textWindow->Clear();
      Tex2RTFYield(TRUE);
      ShowCustomMacros();
      break;
    }
    case TEX_MODE_RTF:
    {
      convertMode = TEX_RTF;
      winHelp = FALSE;
      InputFile = NULL;
      OutputFile = NULL;
      SetStatusText("In linear RTF mode.", 1);
      break;
    }
    case TEX_MODE_WINHELP:
    {
      convertMode = TEX_RTF;
      winHelp = TRUE;
      InputFile = NULL;
      OutputFile = NULL;
      SetStatusText("In WinHelp RTF mode.", 1);
      break;
    }
    case TEX_MODE_XLP:
    {
      convertMode = TEX_XLP;
      InputFile = NULL;
      OutputFile = NULL;
      SetStatusText("In XLP mode.", 1);
      break;
    }
    case TEX_MODE_HTML:
    {
      convertMode = TEX_HTML;
      InputFile = NULL;
      OutputFile = NULL;
      SetStatusText("In HTML mode.", 1);
      break;
    }
    case TEX_HELP:
    {
      HelpInstance->LoadFile();
      HelpInstance->DisplayContents();
      break;
    }
    case TEX_ABOUT:
    {
      char buf[300];
#ifdef WIN32
      char *platform = " (32-bit)";
#else
#ifdef wx_msw
      char *platform = " (16-bit)";
#else
      char *platform = "";
#endif
#endif
      sprintf(buf, "Tex2RTF Version %.2f%s\nLaTeX to RTF, WinHelp, HTML and wxHelp Conversion\n\n(c) Julian Smart 1997", versionNo, platform);
      wxMessageBox(buf, "About Tex2RTF");
      break;
    }
  }
}

void ChooseInputFile(Bool force)
{
  if (force || !InputFile)
  {
    char *s = wxFileSelector("Choose LaTeX input file", PathOnly(InputFile), FileNameFromPath(InputFile), "tex", "*.tex");
    if (s)
    {
      // Different file, so clear index entries.
      ClearKeyWordTable();
      ResetContentsLevels(0);
      passNumber = 1;
      char buf[300];
      InputFile = copystring(s);
      sprintf(buf, "Tex2RTF [%s]", FileNameFromPath(InputFile));
      frame->SetTitle(buf);
      OutputFile = NULL;
    }
  }
}

void ChooseOutputFile(Bool force)
{
  char extensionBuf[10];
  char wildBuf[10];
  strcpy(wildBuf, "*.");
  char *path = NULL;
  if (OutputFile)
    path = PathOnly(OutputFile);
  else if (InputFile)
    path = PathOnly(InputFile);
    
  switch (convertMode)
  {
    case TEX_RTF:
    {
      strcpy(extensionBuf, "rtf");
      strcat(wildBuf, "rtf");
      break;
    }
    case TEX_XLP:
    {
      strcpy(extensionBuf, "xlp");
      strcat(wildBuf, "xlp");
      break;
    }
    case TEX_HTML:
    {
#if defined(wx_msw) && !defined(WIN32)
      strcpy(extensionBuf, "htm");
      strcat(wildBuf, "htm");
#else
      strcpy(extensionBuf, "html");
      strcat(wildBuf, "html");
#endif
      break;
    }
  }
  if (force || !OutputFile)
  {
    char *s = wxFileSelector("Choose output file", path, FileNameFromPath(OutputFile),
                   extensionBuf, wildBuf);
    if (s)
      OutputFile = copystring(s);
  }
}
#endif

Bool Go(void)
{
#ifndef NO_GUI
  ChooseInputFile();
  ChooseOutputFile();
#endif

  if (!InputFile || !OutputFile)
    return FALSE;

#ifndef NO_GUI
  if (isInteractive)
  {
    char buf[300];
    sprintf(buf, "Tex2RTF [%s]", FileNameFromPath(InputFile));
    frame->SetTitle(buf);
  }

  wxStartTimer();
#endif

  // Find extension-less filename
  strcpy(FileRoot, OutputFile);
  StripExtension(FileRoot);

  if (truncateFilenames && convertMode == TEX_HTML)
  {
    // Truncate to five characters. This ensures that
    // we can generate DOS filenames such as thing999. But 1000 files
    // may not be enough, of course...
    char* sName = wxFileNameFromPath( FileRoot);  // this Julian's method is non-destructive reference

    if(sName)
      if(strlen( sName) > 5)
        sName[5] = '\0';  // that should do!
  }
  
  sprintf(ContentsName, "%s.con", FileRoot);
  sprintf(TmpContentsName, "%s.cn1", FileRoot);
  sprintf(TmpFrameContentsName, "%s.frc", FileRoot);
  sprintf(WinHelpContentsFileName, "%s.cnt", FileRoot);
  sprintf(RefName, "%s.ref", FileRoot);

  TexPathList.EnsureFileAccessible(InputFile);
  if (!bulletFile)
  {
    char *s = TexPathList.FindValidPath("bullet.bmp");
    if (s)
      bulletFile = copystring(FileNameFromPath(s));
  }

  if (FileExists(RefName))
    ReadTexReferences(RefName);
    
  Bool success = FALSE;

  if (InputFile && OutputFile)
  {
    if (!FileExists(InputFile))
    {
      OnError("Cannot open input file!");
      TexCleanUp();
      return FALSE;
    }
#ifndef NO_GUI
    if (isInteractive)
    {
      char buf[50];
      sprintf(buf, "Working, pass %d...", passNumber);
      frame->SetStatusText(buf);
    }
#endif
    OkToClose = FALSE;
    OnInform("Reading LaTeX file...");
    TexLoadFile(InputFile);

    switch (convertMode)
    {
      case TEX_RTF:
      {
        success = RTFGo();
        break;
      }
      case TEX_XLP:
      {
        success = XLPGo();
        break;
      }
      case TEX_HTML:
      {
        success = HTMLGo();
        break;
      }
    }
  }
  if (stopRunning)
  {
    OnInform("*** Aborted by user.");
    success = FALSE;
    stopRunning = FALSE;
  }

  if (success)
  {
    WriteTexReferences(RefName);
    TexCleanUp();
    startedSections = FALSE;

    char buf[100];
#ifndef NO_GUI
    long tim = wxGetElapsedTime();
    sprintf(buf, "Finished in %ld seconds.", (long)(tim/1000.0));
    OnInform(buf);
    if (isInteractive)
    {
      sprintf(buf, "Done, %d %s.", passNumber, (passNumber > 1) ? "passes" : "pass");
      frame->SetStatusText(buf);
    }
#else
    sprintf(buf, "Done, %d %s.", passNumber, (passNumber > 1) ? "passes" : "pass");
    OnInform(buf);
#endif
    passNumber ++;
    OkToClose = TRUE;
    return TRUE;
  }

  TexCleanUp();
  startedSections = FALSE;

  OnInform("Sorry, unsuccessful.");
  OkToClose = TRUE;
  return FALSE;
}

void OnError(char *msg)
{
#ifdef NO_GUI
  cerr << "Error: " << msg << "\n";
  cerr.flush();
#else
  if (isInteractive)
    (*frame->textWindow) << "Error: " << msg << "\n";
  else
#ifdef wx_x
  {
    cerr << "Error: " << msg << "\n";
    cerr.flush();
  }
#endif
#ifdef wx_msw
    wxError(msg);
#endif
  Tex2RTFYield(TRUE);
#endif // NO_GUI
}

void OnInform(char *msg)
{
#ifdef NO_GUI
  cout << msg << "\n";
  cout.flush();
#else
  if (isInteractive)
    (*frame->textWindow) << msg << "\n";
  else
#ifdef wx_x
  {
    cout << msg << "\n";
    cout.flush();
  }
#endif
#ifdef wx_msw
    {}
#endif
  if (isInteractive)
  {
    Tex2RTFYield(TRUE);
  }
#endif // NO_GUI
}

void OnMacro(int macroId, int no_args, Bool start)
{
  switch (convertMode)
  {
    case TEX_RTF:
    {
      RTFOnMacro(macroId, no_args, start);
      break;
    }
    case TEX_XLP:
    {
      XLPOnMacro(macroId, no_args, start);
      break;
    }
    case TEX_HTML:
    {
      HTMLOnMacro(macroId, no_args, start);
      break;
    }
  }
}

Bool OnArgument(int macroId, int arg_no, Bool start)
{
  switch (convertMode)
  {
    case TEX_RTF:
    {
      return RTFOnArgument(macroId, arg_no, start);
      break;
    }
    case TEX_XLP:
    {
      return XLPOnArgument(macroId, arg_no, start);
      break;
    }
    case TEX_HTML:
    {
      return HTMLOnArgument(macroId, arg_no, start);
      break;
    }
  }
  return TRUE;
}

/*
 * DDE Stuff
 */
#if defined(wx_msw) && !defined(NO_GUI)

/*
 * Server
 */

wxConnection *Tex2RTFServer::OnAcceptConnection(char *topic)
{
  if (strcmp(topic, "TEX2RTF") == 0)
  {
    if (!ipc_buffer)
      ipc_buffer = new char[1000];
      
    return new Tex2RTFConnection(ipc_buffer, 4000);
  }
  else
    return NULL;
}

 /*
  * Connection
  */
  
Tex2RTFConnection::Tex2RTFConnection(char *buf, int size):wxConnection(buf, size)
{
}

Tex2RTFConnection::~Tex2RTFConnection(void)
{
}

Bool SplitCommand(char *data, char *firstArg, char *secondArg)
{
  firstArg[0] = 0;
  secondArg[0] = 0;
  int i = 0;
  int len = strlen(data);
  Bool stop = FALSE;
  // Find first argument (command name)
  while (!stop)
  {
    if (data[i] == ' ' || data[i] == 0)
      stop = TRUE;
    else
    {
      firstArg[i] = data[i];
      i ++;
    }
  }
  firstArg[i] = 0;
  if (data[i] == ' ')
  {
    // Find second argument
    i ++;
    int j = 0;
    while (data[i] != 0)
    {
      secondArg[j] = data[i];
      i ++;
      j ++;
    }
    secondArg[j] = 0;
  }
  return TRUE;
}

Bool Tex2RTFConnection::OnExecute(char *topic, char *data, int size, int format)
{
  strcpy(Tex2RTFLastStatus, "OK");

  char firstArg[50];
  char secondArg[300];
  if (SplitCommand(data, firstArg, secondArg))
  {
    Bool hasArg = (strlen(secondArg) > 0);
    if (strcmp(firstArg, "INPUT") == 0 && hasArg)
    {
      if (InputFile) delete[] InputFile;
      InputFile = copystring(secondArg);
      if (frame)
      {
        char buf[100];
        sprintf(buf, "Tex2RTF [%s]", FileNameFromPath(InputFile));
        frame->SetTitle(buf);
      }
    }
    else if (strcmp(firstArg, "OUTPUT") == 0 && hasArg)
    {
      if (OutputFile) delete[] OutputFile;
      OutputFile = copystring(secondArg);
    }
    else if (strcmp(firstArg, "GO") == 0)
    {
      strcpy(Tex2RTFLastStatus, "WORKING");
      if (!Go())
        strcpy(Tex2RTFLastStatus, "CONVERSION ERROR");
      else
        strcpy(Tex2RTFLastStatus, "OK");
    }
    else if (strcmp(firstArg, "EXIT") == 0)
    {
      if (frame && frame->OnClose())
        delete frame;
    }
    else if (strcmp(firstArg, "MINIMIZE") == 0 || strcmp(firstArg, "ICONIZE") == 0)
    {
      if (frame)
        frame->Iconize(TRUE);
    }
    else if (strcmp(firstArg, "SHOW") == 0 || strcmp(firstArg, "RESTORE") == 0)
    {
      if (frame)
      {
        frame->Iconize(FALSE);
        frame->Show(TRUE);
      }
    }
    else
    {
      // Try for a setting
      strcpy(Tex2RTFLastStatus, RegisterSetting(firstArg, secondArg, FALSE));
#ifndef NO_GUI
      if (frame && strcmp(firstArg, "conversionMode") == 0)
      {
        char buf[100];
        strcpy(buf, "In ");

        if (winHelp && (convertMode == TEX_RTF))
          strcat(buf, "WinHelp RTF");
        else if (!winHelp && (convertMode == TEX_RTF))
          strcat(buf, "linear RTF");
        else if (convertMode == TEX_HTML) strcat(buf, "HTML");
        else if (convertMode == TEX_XLP) strcat(buf, "XLP");
          strcat(buf, " mode.");
        frame->SetStatusText(buf, 1);
      }
#endif
    }
  }
  return TRUE;
}

char *Tex2RTFConnection::OnRequest(char *topic, char *item, int *size, int format)
{
  return Tex2RTFLastStatus;
}

#endif
