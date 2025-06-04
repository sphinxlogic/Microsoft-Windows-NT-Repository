/*
 * File:	bapp.cc
 * Purpose:	wxWindows GUI builder
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_help.h"
#include "wx_types.h"
#include <ctype.h>
#include <stdlib.h>
#ifdef wx_msw
#include <time.h>
#else
#include <sys/time.h>
#endif

#include "wxbuild.h"
#include "namegen.h"
#include "bapp.h"
#include "bframe.h"
#include "bmenu.h"
#include "bsubwin.h"
#include "bitem.h"
#include "btoolbar.h"
#include "bactions.h"
#include "bgdi.h"
#include "btree.h"

#define BUILDERSECTION "wxBuilder"

extern void ClearTree(void);
extern Bool isModified;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// This statement initialises the whole application
BuildApp     buildApp;

// Bitmaps for palette
wxBitmap *PaletteFrameBitmap = NULL;
wxBitmap *PaletteDialogBoxBitmap = NULL;
wxBitmap *PalettePanelBitmap = NULL;
wxBitmap *PaletteCanvasBitmap = NULL;
wxBitmap *PaletteTextWindowBitmap = NULL;
wxBitmap *PaletteMessageBitmap = NULL;
wxBitmap *PaletteButtonBitmap = NULL;
wxBitmap *PaletteCheckBoxBitmap = NULL;
wxBitmap *PaletteListBoxBitmap = NULL;
wxBitmap *PaletteRadioBoxBitmap = NULL;
wxBitmap *PaletteChoiceBitmap = NULL;
wxBitmap *PaletteTextBitmap = NULL;
wxBitmap *PaletteMultiTextBitmap = NULL;
wxBitmap *PaletteSliderBitmap = NULL;
wxBitmap *PaletteArrowBitmap = NULL;
wxBitmap *PaletteGroupBitmap = NULL;
wxBitmap *PaletteGaugeBitmap = NULL;

#ifdef wx_x
#include "bitmaps/frame.xbm"
#include "bitmaps/dialog.xbm"
#include "bitmaps/panel.xbm"
#include "bitmaps/canvas.xbm"
#include "bitmaps/textsw.xbm"
#include "bitmaps/message.xbm"
#include "bitmaps/button.xbm"
#include "bitmaps/check.xbm"
#include "bitmaps/listbox.xbm"
#include "bitmaps/radio.xbm"
#include "bitmaps/choice.xbm"
#include "bitmaps/text.xbm"
#include "bitmaps/mtext.xbm"
#include "bitmaps/slider.xbm"
#include "bitmaps/arrow.xbm"
#include "bitmaps/group.xbm"
#include "bitmaps/gauge.xbm"
#endif

// Bitmaps for toolbar
wxBitmap *ToolbarLoadBitmap = NULL;
wxBitmap *ToolbarSaveBitmap = NULL;
wxBitmap *ToolbarVertBitmap = NULL;
wxBitmap *ToolbarAlignTBitmap = NULL;
wxBitmap *ToolbarAlignBBitmap = NULL;
wxBitmap *ToolbarHorizBitmap = NULL;
wxBitmap *ToolbarAlignLBitmap = NULL;
wxBitmap *ToolbarAlignRBitmap = NULL;
wxBitmap *ToolbarCPPBitmap = NULL;
wxBitmap *ToolbarTreeBitmap = NULL;
wxBitmap *ToolbarHelpBitmap = NULL;

#ifdef wx_x
#include "bitmaps/load.xbm"
#include "bitmaps/save.xbm"
#include "bitmaps/vert.xbm"
#include "bitmaps/alignt.xbm"
#include "bitmaps/alignb.xbm"
#include "bitmaps/horiz.xbm"
#include "bitmaps/alignl.xbm"
#include "bitmaps/alignr.xbm"
#include "bitmaps/cpp.xbm"
#include "bitmaps/tree.xbm"
#include "bitmaps/help.xbm"
#endif

static void ObjectMenuProc(wxMenu& menu, wxCommandEvent& event);

// The `main program' equivalent, creating the windows and returning the
// main frame
wxFrame *BuildApp::OnInit(void)
{
#ifdef wx_msw
  resourceFile = NULL;
#endif
#ifdef wx_x
  resourceFile = new char[400];
  (void)wxGetHomeDir(resourceFile);
  strcat(resourceFile, "/.wxbuildrc");
#endif
  int i = 1;
  
  // Read input file if given without using -f
  if (argc > i && (argv[i][0] != '-'))
  {
    projectFilename = copystring(argv[i]);
    i ++;
  }

  while (i < argc)
  {
    if (strcmp(argv[i], "-f") == 0)
    {
      i ++;
      if (i == argc)
      {
        wxError("-f must take a filename!", "Warning");
      }
      else
      {
        projectFilename = copystring(argv[i]);
        i ++;
      }
    }
    else
    {
      char buf[200];
      sprintf(buf, "Unrecognised wxBuilder option %s", argv[i]);
      wxError(buf, "Warning");
      i++;
    }
  }

  handCursor = new wxCursor(wxCURSOR_HAND);
  crossCursor = new wxCursor(wxCURSOR_CROSS);
  // Create a small font
  SmallButtonFont = new wxFont(11, wxROMAN, wxNORMAL, wxNORMAL);
  SmallLabelFont = new wxFont(12, wxROMAN, wxITALIC, wxBOLD);
  TextWindowFont = new wxFont(12, wxSWISS, wxNORMAL, wxNORMAL);
  InitializeDefaults();
  
  // Create the main frame window
  MainFrame = new BuildFrame(NULL, "wxBuilder [unnamed]", mainX, mainY, mainWidth, mainHeight);

  // Load palette bitmaps
#ifdef wx_msw
  ToolbarLoadBitmap = new wxBitmap("LOADTOOL");
  ToolbarSaveBitmap = new wxBitmap("SAVETOOL");
  ToolbarVertBitmap = new wxBitmap("VERTTOOL");
  ToolbarAlignTBitmap = new wxBitmap("ALIGNTTOOL");
  ToolbarAlignBBitmap = new wxBitmap("ALIGNBTOOL");
  ToolbarHorizBitmap = new wxBitmap("HORIZTOOL");
  ToolbarAlignLBitmap = new wxBitmap("ALIGNLTOOL");
  ToolbarAlignRBitmap = new wxBitmap("ALIGNRTOOL");
  ToolbarCPPBitmap = new wxBitmap("CPPTOOL");
  ToolbarTreeBitmap = new wxBitmap("TREETOOL");
  ToolbarHelpBitmap = new wxBitmap("HELPTOOL");
#endif
#ifdef wx_x
  ToolbarLoadBitmap = new wxBitmap(load_bits, load_width, load_height);
  ToolbarSaveBitmap = new wxBitmap(save_bits, save_width, save_height);
  ToolbarVertBitmap = new wxBitmap(vert_bits, vert_width, vert_height);
  ToolbarAlignTBitmap = new wxBitmap(alignt_bits, alignt_width, alignt_height);
  ToolbarAlignBBitmap = new wxBitmap(alignb_bits, alignb_width, alignb_height);
  ToolbarHorizBitmap = new wxBitmap(horiz_bits, horiz_width, horiz_height);
  ToolbarAlignLBitmap = new wxBitmap(alignl_bits, alignl_width, alignl_height);
  ToolbarAlignRBitmap = new wxBitmap(alignr_bits, alignr_width, alignr_height);
  ToolbarCPPBitmap = new wxBitmap(cpp_bits, cpp_width, cpp_height);
  ToolbarTreeBitmap = new wxBitmap(tree_bits, tree_width, tree_height);
  ToolbarHelpBitmap = new wxBitmap(help_bits, help_width, help_height);
#endif

  // Create the toolbar
  MainFrame->toolbar = new EditorToolBar(MainFrame, 0, 0, -1, -1, 0,
                                        wxVERTICAL, 1);
  MainFrame->toolbar->SetMargins(2, 2);
  MainFrame->toolbar->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
  CreateObjectMenu();

#ifdef wx_msw
  int width = 24;
  int dx = 2;
  int gap = 6;
#else
  int width = ToolbarLoadBitmap->GetWidth();
  int dx = 2;
  int gap = 6;
#endif
  int currentX = gap;
  MainFrame->toolbar->AddSeparator();
  MainFrame->toolbar->AddTool(TOOLBAR_LOAD_FILE, ToolbarLoadBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Load");
  currentX += width + dx;
  MainFrame->toolbar->AddTool(TOOLBAR_SAVE_FILE, ToolbarSaveBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Save");
  currentX += width + dx + gap;
  MainFrame->toolbar->AddSeparator();
  MainFrame->toolbar->AddTool(TOOLBAR_FORMAT_HORIZ, ToolbarVertBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Horizontal align");
  currentX += width + dx;
  MainFrame->toolbar->AddTool(TOOLBAR_FORMAT_VERT_TOP_ALIGN, ToolbarAlignTBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Top align");
  currentX += width + dx;
  MainFrame->toolbar->AddTool(TOOLBAR_FORMAT_VERT_BOT_ALIGN, ToolbarAlignBBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Bottom align");
  currentX += width + dx;
  MainFrame->toolbar->AddTool(TOOLBAR_FORMAT_VERT, ToolbarHorizBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Vertical align");
  currentX += width + dx;
  MainFrame->toolbar->AddTool(TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN, ToolbarAlignLBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Left align");
  currentX += width + dx;
  MainFrame->toolbar->AddTool(TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN, ToolbarAlignRBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Right align");
  currentX += width + dx + gap;
  MainFrame->toolbar->AddSeparator();
  MainFrame->toolbar->AddTool(TOOLBAR_GEN_CPP, ToolbarCPPBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Generate C++");
  currentX += width + dx;
/*
  MainFrame->toolbar->AddTool(TOOLBAR_TREE, ToolbarTreeBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Tree view");
  currentX += width + dx;
*/
  MainFrame->toolbar->AddTool(TOOLBAR_HELP, ToolbarHelpBitmap, NULL,
                   FALSE, (float)currentX, -1, NULL, "Help");
  currentX += width + dx;
  
  MainFrame->toolbar->CreateTools();

  MainFrame->OnSize(-1, -1);

  // Load palette bitmaps
#ifdef wx_msw
  PaletteFrameBitmap = new wxBitmap("FRAMETOOL");
  PaletteDialogBoxBitmap = new wxBitmap("DIALOGBOXTOOL");
  PalettePanelBitmap = new wxBitmap("PANELTOOL");
  PaletteCanvasBitmap = new wxBitmap("CANVASTOOL");
  PaletteTextWindowBitmap = new wxBitmap("TEXTWINDOWTOOL");
  PaletteMessageBitmap = new wxBitmap("MESSAGETOOL");
  PaletteButtonBitmap = new wxBitmap("BUTTONTOOL");
  PaletteCheckBoxBitmap = new wxBitmap("CHECKBOXTOOL");
  PaletteListBoxBitmap = new wxBitmap("LISTBOXTOOL");
  PaletteRadioBoxBitmap = new wxBitmap("RADIOBOXTOOL");
  PaletteChoiceBitmap = new wxBitmap("CHOICETOOL");
  PaletteTextBitmap = new wxBitmap("TEXTTOOL");
  PaletteMultiTextBitmap = new wxBitmap("MULTITEXTTOOL");
  PaletteSliderBitmap = new wxBitmap("SLIDERTOOL");
  PaletteArrowBitmap = new wxBitmap("ARROWTOOL");
  PaletteGroupBitmap = new wxBitmap("GROUPTOOL");
  PaletteGaugeBitmap = new wxBitmap("GAUGETOOL");
#endif
#ifdef wx_x
  PaletteFrameBitmap = new wxBitmap(frame_bits, frame_width, frame_height);
  PaletteDialogBoxBitmap = new wxBitmap(dialog_bits, dialog_width, dialog_height);
  PalettePanelBitmap = new wxBitmap(panel_bits, panel_width, panel_height);
  PaletteCanvasBitmap = new wxBitmap(canvas_bits, canvas_width, canvas_height);
  PaletteTextWindowBitmap = new wxBitmap(textsw_bits, textsw_width, textsw_height);
  PaletteMessageBitmap = new wxBitmap(message_bits, message_width, message_height);
  PaletteButtonBitmap = new wxBitmap(button_bits, button_width, button_height);
  PaletteCheckBoxBitmap = new wxBitmap(check_bits, check_width, check_height);
  PaletteListBoxBitmap = new wxBitmap(listbox_bits, listbox_width, listbox_height);
  PaletteRadioBoxBitmap = new wxBitmap(radio_bits, radio_width, radio_height);
  PaletteChoiceBitmap = new wxBitmap(choice_bits, choice_width, choice_height);
  PaletteTextBitmap = new wxBitmap(text_bits, text_width, text_height);
  PaletteMultiTextBitmap = new wxBitmap(mtext_bits, mtext_width, mtext_height);
  PaletteSliderBitmap = new wxBitmap(slider_bits, slider_width, slider_height);
  PaletteArrowBitmap = new wxBitmap(arrow_bits, arrow_width, arrow_height);
  PaletteGroupBitmap = new wxBitmap(group_bits, group_width, group_height);
  PaletteGaugeBitmap = new wxBitmap(gauge_bits, gauge_width, gauge_height);
#endif

  EditorPaletteFrame = new EditorToolPaletteFrame(MainFrame, "wxWin Objects", paletteX, paletteY, 100, 200,
  // Under Motif, looks better with a thick border than no border.
#ifdef wx_motif
         wxSDI | wxTHICK_FRAME | wxTINY_CAPTION_HORIZ);
#else
         wxSDI | wxTINY_CAPTION_HORIZ);
#endif
  EditorPaletteFrame->palette = new EditorToolPalette(EditorPaletteFrame, 0, 0, -1, -1, 0,
                                        wxHORIZONTAL, 6);
  EditorPaletteFrame->palette->SetMargins(2, 2);
  EditorPaletteFrame->palette->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
  EditorPaletteFrame->palette->AddTool(PALETTE_ARROW, PaletteArrowBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_FRAME, PaletteFrameBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_DIALOG_BOX, PaletteDialogBoxBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_PANEL, PalettePanelBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_CANVAS, PaletteCanvasBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_TEXT_WINDOW, PaletteTextWindowBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_MESSAGE, PaletteMessageBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_BUTTON, PaletteButtonBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_CHECKBOX, PaletteCheckBoxBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_RADIOBOX, PaletteRadioBoxBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_LISTBOX, PaletteListBoxBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_CHOICE, PaletteChoiceBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_TEXT, PaletteTextBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_MULTITEXT, PaletteMultiTextBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_SLIDER, PaletteSliderBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_GROUPBOX, PaletteGroupBitmap, NULL, TRUE);
  EditorPaletteFrame->palette->AddTool(PALETTE_GAUGE, PaletteGaugeBitmap, NULL, TRUE);

  EditorPaletteFrame->palette->Layout();
  float maxWidth, maxHeight;
  EditorPaletteFrame->palette->GetMaxSize(&maxWidth, &maxHeight);
  EditorPaletteFrame->SetClientSize((int)maxWidth, (int)maxHeight);
  EditorPaletteFrame->palette->ToggleTool(PALETTE_ARROW, TRUE);
  EditorPaletteFrame->palette->currentlySelected = PALETTE_ARROW;
  EditorPaletteFrame->Show(TRUE);
  EditorPaletteFrame->palette->OnPaint();

  HelpInstance = new wxHelpInstance(TRUE);
  HelpInstance->Initialize("wxbuild");

  if (projectFilename)
  {
    LoadProject(projectFilename, FALSE);
  }

  return MainFrame;
}

BuildApp::BuildApp(void)
{
  newObjectX = 0;
  newObjectY = 0;
  usePosition = FALSE;
  objectMenu = NULL;
  objectParent = NULL;
  
  revision = 0;
  topLevelFrame = NULL;
  projectFilename = NULL;
  buildName = copystring("noname");
  buildDescription = NULL;

#ifdef wx_x
  // Determine the user's full name
  {
    /* Fullname <email> */
    char answer[190+32+4+1];
    char name[127];
    char hostname[65];
    char id[32];

    if (wxGetUserName(name, 126)) {
      if (wxGetHostName(hostname, 64)) {
	if (wxGetUserId(id, 31) == FALSE) id[0] = '\0';
	sprintf(answer, "%s <%s@%s>"
	  ,name
	  ,id
	  ,hostname);
      } else
	strcpy(answer, name);
      buildAuthors = copystring(answer);
    } else buildAuthors = NULL;
  }
#else
  buildAuthors = NULL;
#endif
  buildDate = NULL;
  appClass = copystring("AppClass");
  buildMDIType = wxSDI;
  extensionCPP = copystring(".cc");
  currentMenuItemId = 1;
  testMode = FALSE;

  mainX = 250;
  mainY = 10;
#ifdef wx_x
  mainWidth = 500;
#else
  mainWidth = 400;
#endif
  mainHeight = 400;

  paletteX = 10;
  paletteY = 150;

  reportX = 50;
  reportY = 50;
  reportWidth = 350;
  reportHeight = 200;

  treeX = 50;
  treeY = 200;
  treeWidth = 250;
  treeHeight = 250;

  projectDirUNIX = copystring("project");
  projectDirDOS = copystring("c:\\project");
  commandLine = copystring("");
  genMakefiles = TRUE;
  genRCFile = TRUE;
  genDefFile = TRUE;
  separateFiles = FALSE;
  useResourceMethod = TRUE;

  XIncludes = copystring("-I/usr/include/X11R5 -I/usr/include/motif1.2");
  XLibs = copystring("-L/usr/lib/X11R5 -L/usr/lib/motif1.2");
  /* People compiling wxBuilder locally may prefer this 
   * more intelligent approach instead of the above.
   * In which case, uncomment.
   */
/*
#ifdef XINCLUDES
  XIncludes = copystring(XINCLUDES);
#elif defined(wx_motif)
  XIncludes = copystring("-I/usr/include/X11R5 -I/usr/include/motif1.2");
#else
  XIncludes = copystring("-I$(OPENWINHOME)/include");
#endif

#ifdef XLIBS
  XLibs = copystring(XLIBS);
#elif defined(wx_xview)
  Xlibs = copystring("-L$(OPENWINHOME)/lib");
#else 
  XLibs = copystring("-L/usr/lib/X11R5 -L/usr/lib/motif1.2");
#endif
*/

  windowsInclude = copystring("-Ic:\\c700\\include");
  wxDirUNIX = copystring("/usr/lib/wx");
  wxDirDOS = copystring("c:\\wx");

#ifdef hpux
  guiTarget = copystring("hp");
#elif defined(wx_xview)
  guiTarget = copystring("xview");
#else
  guiTarget = copystring("motif");
#endif

  extraIncludesX = copystring("");
  extraLibsX = copystring("");
  extraIncludesMSW = copystring("");
  extraLibsMSW = copystring("");

  autoCompile = FALSE;
  autoRun = FALSE;

  compilerDOS = copystring("Microsoft C/C++ Vsn 7");
  compilerUNIX = copystring("gcc");

  fileHistoryN = 0;
  for (int i = 0; i < MAX_FILE_HISTORY; i++)
    fileHistory[i] = NULL;
}

BuildApp::~BuildApp(void)
{
  delete objectMenu;
}

void BuildApp::AddFileToHistory(char *file)
{
  int i;

  // Check we don't already have this file
  for (i = 0; i < fileHistoryN; i++)
  {
    if (fileHistory[i] && (strcmp(fileHistory[i], file) == 0))
      return;
  }
  
  // Add to the project file history:
  // Move existing files (if any) down so we can insert file at beginning.
  
  // First delete filename that has popped off the end of the array (if any)
  if (fileHistoryN == MAX_FILE_HISTORY)
  {
    delete[] fileHistory[MAX_FILE_HISTORY-1];
    fileHistory[MAX_FILE_HISTORY-1] = NULL;
  }
  if (fileHistoryN < MAX_FILE_HISTORY)
  {
    if (fileHistoryN == 0)
      fileMenu->AppendSeparator();
    fileMenu->Append(BUILD_FILE1+fileHistoryN, "[EMPTY]");
    fileHistoryN ++;
  }
  // Shuffle filenames down
  for (i = (fileHistoryN-1); i > 0; i--)
  {
    fileHistory[i] = fileHistory[i-1];
  }
  fileHistory[0] = copystring(file);
  
  for (i = 0; i < fileHistoryN; i++)
    if (fileHistory[i])
    {
      char buf[400];
      sprintf(buf, "&%d %s", i+1, buildApp.fileHistory[i]);
      fileMenu->SetLabel(BUILD_FILE1+i, buf);
    }
}

// Set up defaults from WIN.INI/.Xdefaults
void BuildApp::InitializeDefaults(void)
{
  wxGetResource(BUILDERSECTION, "target", &guiTarget, resourceFile);
  wxGetResource(BUILDERSECTION, "autoRun", &autoRun, resourceFile);
  wxGetResource(BUILDERSECTION, "autoCompile", &autoCompile, resourceFile);
  wxGetResource(BUILDERSECTION, "compilerDOS", &compilerDOS, resourceFile);
  wxGetResource(BUILDERSECTION, "compilerUNIX", &compilerUNIX, resourceFile);
  wxGetResource(BUILDERSECTION, "xIncludes", &XIncludes, resourceFile);
  wxGetResource(BUILDERSECTION, "xLib", &XLibs, resourceFile);
  wxGetResource(BUILDERSECTION, "windowsInclude", &windowsInclude, resourceFile);
  wxGetResource(BUILDERSECTION, "wxDirUNIX", &wxDirUNIX, resourceFile);
  wxGetResource(BUILDERSECTION, "wxDirDOS", &wxDirDOS, resourceFile);
  wxGetResource(BUILDERSECTION, "extraIncludesX", &extraIncludesX, resourceFile);
  wxGetResource(BUILDERSECTION, "extraLibsX", &extraLibsX, resourceFile);
  wxGetResource(BUILDERSECTION, "extraIncludesMSW", &extraIncludesMSW, resourceFile);
  wxGetResource(BUILDERSECTION, "extraLibsMSW", &extraLibsMSW, resourceFile);

  wxGetResource(BUILDERSECTION, "mainX", &mainX, resourceFile);
  wxGetResource(BUILDERSECTION, "mainY", &mainY, resourceFile);
  wxGetResource(BUILDERSECTION, "mainWidth", &mainWidth, resourceFile);
  wxGetResource(BUILDERSECTION, "mainHeight", &mainHeight, resourceFile);

  wxGetResource(BUILDERSECTION, "reportX", &reportX, resourceFile);
  wxGetResource(BUILDERSECTION, "reportY", &reportY, resourceFile);
  wxGetResource(BUILDERSECTION, "reportWidth", &reportWidth, resourceFile);
  wxGetResource(BUILDERSECTION, "reportHeight", &reportHeight, resourceFile);

  wxGetResource(BUILDERSECTION, "treeX", &treeX, resourceFile);
  wxGetResource(BUILDERSECTION, "treeY", &treeY, resourceFile);
  wxGetResource(BUILDERSECTION, "treeWidth", &treeWidth, resourceFile);
  wxGetResource(BUILDERSECTION, "treeHeight", &treeHeight, resourceFile);

  wxGetResource(BUILDERSECTION, "paletteX", &paletteX, resourceFile);
  wxGetResource(BUILDERSECTION, "paletteY", &paletteY, resourceFile);

  fileHistoryN = 0;
  char buf[100];
  sprintf(buf, "file%d", fileHistoryN+1);
  char *historyFile = NULL;
  while ((fileHistoryN <= MAX_FILE_HISTORY) && wxGetResource(BUILDERSECTION, buf, &historyFile, resourceFile) && historyFile)
  {
    // wxGetResource allocates memory so this is o.k.
    fileHistory[fileHistoryN] = historyFile;
    fileHistoryN ++;
    sprintf(buf, "file%d", fileHistoryN+1);
    historyFile = NULL;
  }
}

// Write defaults into WIN.INI (not .Xdefaults!)
void BuildApp::WriteDefaults(void)
{
  wxWriteResource(BUILDERSECTION, "target", guiTarget, resourceFile);
  wxWriteResource(BUILDERSECTION, "autoRun", autoRun, resourceFile);
  wxWriteResource(BUILDERSECTION, "autoCompile", autoCompile, resourceFile);
  wxWriteResource(BUILDERSECTION, "compilerDOS", compilerDOS, resourceFile);
  wxWriteResource(BUILDERSECTION, "compilerUNIX", compilerUNIX, resourceFile);
  wxWriteResource(BUILDERSECTION, "xIncludes", XIncludes, resourceFile);
  wxWriteResource(BUILDERSECTION, "xLib", XLibs, resourceFile);
  wxWriteResource(BUILDERSECTION, "windowsInclude", windowsInclude, resourceFile);
  wxWriteResource(BUILDERSECTION, "wxDirUNIX", wxDirUNIX, resourceFile);
  wxWriteResource(BUILDERSECTION, "wxDirDOS", wxDirDOS, resourceFile);
  wxWriteResource(BUILDERSECTION, "extraIncludesX", extraIncludesX, resourceFile);
  wxWriteResource(BUILDERSECTION, "extraLibsX", extraLibsX, resourceFile);
  wxWriteResource(BUILDERSECTION, "extraIncludesMSW", extraIncludesMSW, resourceFile);
  wxWriteResource(BUILDERSECTION, "extraLibsMSW", extraLibsMSW, resourceFile);

  wxWriteResource(BUILDERSECTION, "mainX", mainX, resourceFile);
  wxWriteResource(BUILDERSECTION, "mainY", mainY, resourceFile);
  wxWriteResource(BUILDERSECTION, "mainWidth", mainWidth, resourceFile);
  wxWriteResource(BUILDERSECTION, "mainHeight", mainHeight, resourceFile);
  wxWriteResource(BUILDERSECTION, "reportX", reportX, resourceFile);
  wxWriteResource(BUILDERSECTION, "reportY", reportY, resourceFile);
  wxWriteResource(BUILDERSECTION, "reportWidth", reportWidth, resourceFile);
  wxWriteResource(BUILDERSECTION, "reportHeight", reportHeight, resourceFile);
  wxWriteResource(BUILDERSECTION, "treeX", treeX, resourceFile);
  wxWriteResource(BUILDERSECTION, "treeY", treeY, resourceFile);
  wxWriteResource(BUILDERSECTION, "treeWidth", treeWidth, resourceFile);
  wxWriteResource(BUILDERSECTION, "treeHeight", treeHeight, resourceFile);
  wxWriteResource(BUILDERSECTION, "paletteX", paletteX, resourceFile);
  wxWriteResource(BUILDERSECTION, "paletteY", paletteY, resourceFile);

  char buf[100];
  for (int i = 0; i < fileHistoryN; i++)
  {
    sprintf(buf, "file%d", i+1);
    wxWriteResource(BUILDERSECTION, buf, fileHistory[i], resourceFile);
  }
}

void BuildApp::EditGlobalSettings(void)
{
  wxBeginBusyCursor();
  wxDialogBox *dialog = new wxDialogBox(NULL, "Global Settings", TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Global settings");
  
  form->Add(wxMakeFormString("Windows compiler", &compilerDOS, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings(
    "Borland C++ 4.x",
    "Borland C++ 3.1",
    "Microsoft C/C++ Vsn 7",
    "Microsoft Visual C++",
    "Microsoft C++ for NT",
    "Symantec C++",
    "Watcom C++",
    NULL), NULL), NULL, wxVERTICAL, 200));
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("UNIX compiler", &compilerUNIX, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               200));
  form->Add(wxMakeFormString("X GUI target", &guiTarget, wxFORM_CHOICE,
              new wxList(wxMakeConstraintStrings("xview", "motif", "hp", NULL), NULL),
            NULL, wxVERTICAL, 150));
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("X includes", &XIncludes, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("X libs", &XLibs, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Extra includes (UNIX)", &extraIncludesX, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               150));
//  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormString("Extra libs (UNIX)", &extraLibsX, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               150));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Extra includes (Windows)", &extraIncludesMSW, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               150));
//  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Extra libs (Windows)", &extraLibsMSW, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               150));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("wxWindows home (Windows)", &wxDirDOS, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("wxWindows home (UNIX)", &wxDirUNIX, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
/* OPTIONS TO BE IMPLEMENTED
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormBool("Auto compile", &autoCompile));
  form->Add(wxMakeFormBool("Auto run", &autoRun));
*/
  form->AssociatePanel(dialog);
  form->dialog = dialog;

  dialog->Fit();
  dialog->Centre(wxBOTH);

  wxEndBusyCursor();
  dialog->Show(TRUE);
}

void BuildApp::EditProjectSettings(void)
{
  wxBeginBusyCursor();
  wxDialogBox *dialog = new wxDialogBox(NULL, "Project Settings", TRUE, 10, 10);
  dialog->SetLabelFont(SmallLabelFont);
  dialog->SetButtonFont(SmallButtonFont);

  BuildForm *form = new BuildForm("Project settings");
  
  form->Add(wxMakeFormString("Root name", &buildName, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));

  form->Add(wxMakeFormString("C++ Extension", &extensionCPP, wxFORM_CHOICE,
	new wxList(wxMakeConstraintStrings(
    ".cc",
    ".cpp",
    ".ccx",
#ifdef wx_x
    ".C",
#endif
    NULL), NULL), 
    NULL, wxVERTICAL, 120));

  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Directory under Windows", &projectDirDOS, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormString("Directory under UNIX", &projectDirUNIX, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
/*
  form->Add(wxMakeFormString("Command line", &commandLine, wxFORM_DEFAULT, NULL, NULL, wxVERTICAL,
               300));
  form->Add(wxMakeFormNewLine());
*/
  form->Add(wxMakeFormBool("Generate makefiles", &genMakefiles));
  form->Add(wxMakeFormBool("Generate RC file", &genRCFile));
  form->Add(wxMakeFormBool("Generate DEF file", &genDefFile));
  form->Add(wxMakeFormNewLine());
  form->Add(wxMakeFormBool("Generate wxWindows resources", &useResourceMethod));

/*** NOT YET IMPLEMENTED
  form->Add(wxMakeFormNewLine());

  form->Add(wxMakeFormBool("Generate separate file per window", &separateFiles));
*/

  form->AssociatePanel(dialog);
  form->dialog = dialog;

  dialog->Fit();
  dialog->Centre(wxBOTH);

  wxEndBusyCursor();
  dialog->Show(TRUE);
}

Bool BuildApp::WritePrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  expr->AddAttributeValue("wxbuilder_version", buildVersion);
#if !defined(AIXV3)
  expr->AddAttributeValue("revision", (long)revision);
  {
    time_t now = time(NULL);
    char *date = ctime(&now); 
    date[24] = '\0';
    expr->AddAttributeValueString("updated", date);
  }
#endif
  if (topLevelFrame)
    expr->AddAttributeValue("top_level_frame", topLevelFrame->id);
  if (buildName)
    expr->AddAttributeValueString("name", buildName);
  if (buildDescription)
    expr->AddAttributeValueString("description", buildDescription);
  if (buildAuthors)
    expr->AddAttributeValueString("authors", buildAuthors);
  if (buildDate)
    expr->AddAttributeValueString("date", buildDate);
  if (projectDirDOS)
    expr->AddAttributeValueString("project_dir_dos", projectDirDOS);
  if (projectDirUNIX)
    expr->AddAttributeValueString("project_dir_unix", projectDirUNIX);
  if (commandLine)
    expr->AddAttributeValueString("command_line", commandLine);

  expr->AddAttributeValue("generate_makefiles", (long)genMakefiles);
  expr->AddAttributeValue("generate_rc_file", (long)genRCFile);
  expr->AddAttributeValue("generate_def_file", (long)genDefFile);
  expr->AddAttributeValue("generate_separate_files", (long)separateFiles);
  expr->AddAttributeValue("use_resources", (long)useResourceMethod);
  
  expr->AddAttributeValue("mdi", (long)buildMDIType);
  if (extensionCPP)
    expr->AddAttributeValueString("extension", extensionCPP);
  return TRUE;
}

Bool BuildApp::ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database)
{
  expr->AssignAttributeValue("top_level_frame", &topLevelFrameId);
  expr->AssignAttributeValue("name", &buildName);
  expr->AssignAttributeValue("description", &buildDescription);
  expr->AssignAttributeValue("authors", &buildAuthors);
  expr->AssignAttributeValue("date", &buildDate);
  expr->AssignAttributeValue("project_dir_dos", &projectDirDOS);
  expr->AssignAttributeValue("project_dir_unix", &projectDirUNIX);
  expr->AssignAttributeValue("command_line", &commandLine);
  expr->AssignAttributeValue("generate_makefiles", &genMakefiles);
  expr->AssignAttributeValue("generate_rc_file", &genRCFile);
  expr->AssignAttributeValue("generate_def_file", &genDefFile);
  expr->AssignAttributeValue("generate_separate_files", &separateFiles);
  expr->AssignAttributeValue("use_resources", &useResourceMethod);
  expr->AssignAttributeValue("mdi", &buildMDIType);
  expr->AssignAttributeValue("extension", &extensionCPP);
  expr->AssignAttributeValue("revision", &revision);
  revision++; /* increment */
  
  float version = 0.0;
  expr->AssignAttributeValue("wxbuilder_version", &version);
  if (version < 3.0)
  {
    wxMessageBox("You are using a new version of wxBuilder on an old .wxp file.\n\
Unfortunately, style flags were changed in wxWindows 1.66, wxBuilder version 3.0,\n\
and .wxp files are sensitive to this change. You will need to check all\n\
your windows, modify and save them. Many apologies for this incompatibility.", "wxBuilder", wxOK);
  }
  
  return TRUE;
}

Bool BuildApp::SaveProject(char *filename)
{
  if (filename && !projectFilename)
    projectFilename = copystring(filename);
    
  if (!filename || !projectFilename)
  {
    char *file = wxFileSelector("Select a project file to save to", PathOnly(projectFilename),  FileNameFromPath(projectFilename), "wxp", "*.wxp",
      wxSAVE | wxOVERWRITE_PROMPT);
    if (file && *file)
    {
      projectFilename = copystring(file);
    }
    else return FALSE;
  }
  if (projectFilename)
  {
    wxBeginBusyCursor();

    char buf[300];
    sprintf(buf, "wxBuilder [%s]", FileNameFromPath(projectFilename));
    MainFrame->SetTitle(buf);

    PrologDatabase database;

    // First write the project header
    PrologExpr *clause = new PrologExpr("project");
    WritePrologAttributes(clause, &database);
    database.Append(clause);

    // Now all the top-level windows
    wxNode *node = topLevelWindows.First();
    while (node)
    {
      BuildWindowData *win = (BuildWindowData *)node->Data();

      win->WriteRecursively(&database);
      node = node->Next();
    }
    TheFontManager.WriteFonts(&database);
    
    ofstream stream(projectFilename);
    database.WriteProlog(stream);
    MakeModified(FALSE);
    AddFileToHistory(projectFilename);

    wxEndBusyCursor();
  }
  else return FALSE;

  return TRUE;
}

Bool BuildApp::LoadProject(char *f, Bool interactive)
{
  if (interactive)
    if (Modified())
      if (wxNO == wxMessageBox("Discard modified project?", "Please confirm", wxYES_NO))
        return FALSE;

  char filename[256];
  filename[0] = 0;
  if (f) strcpy(filename, f);
  if (interactive)
  {
    char *file = wxFileSelector("Select a project file to load from", PathOnly(filename),  FileNameFromPath(filename), "wxp", "*.wxp");
    if (file)
    {
      strcpy(filename, file);
    }
    else return FALSE;
  }

  if (strlen(filename) > 0)
  {
    // Create a database hashing on the "id" attribute,
    // so we can quickly find structures we've built up so far.
    PrologDatabase database(PrologInteger, "id");

    wxBeginBusyCursor();

    if (!database.ReadProlog(filename))
    {
      wxEndBusyCursor();
      wxMessageBox("Could not load file.", "Error");
      return FALSE;
    }
    TheFontManager.Show(FALSE);
    database.BeginFind();
    PrologExpr *header = database.FindClauseByFunctor("project");
    if (!header)
    {
      wxEndBusyCursor();
      wxMessageBox("Not a valid project file.", "Error");
      return FALSE;
    }
    ReadPrologAttributes(header, &database);

    // Only now clear the current project, when we're more
    // confident of progressing.
    ClearProject();
    if (projectFilename) delete[] projectFilename;
    projectFilename = copystring(filename);

    // Find all the menus
    database.BeginFind();
    PrologExpr *expr = database.FindClauseByFunctor("menu_item");
    while (expr)
    {
      long parentId = 0;
      long id = 0;

      BuildMenuItem *item = new BuildMenuItem;
      item->ReadPrologAttributes(expr, &database);
      expr->AssignAttributeValue("parent", &parentId);
      expr->AssignAttributeValue("id", &id);
      RegisterId(id);
      expr->SetClientData(item);

      PrologExpr *parentExpr = database.HashFind("menu_item", parentId);
      if (parentExpr)
      {
        item->parent = (BuildMenuItem *)parentExpr->GetClientData();
        if (item->parent)
          item->parent->menus.Append(item);
      }

      expr = database.FindClauseByFunctor("menu_item");
    }

    // Find all the windows
    database.BeginFind();
    expr = database.FindClauseByFunctor("window");
    while (expr)
    {
      int windowType = 0;
      long parentId = 0;
      int id = 0;
      expr->AssignAttributeValue("type", &windowType);
      expr->AssignAttributeValue("parent", &parentId);
      expr->AssignAttributeValue("id", &id);
      RegisterId(id);

      switch (windowType)
      {
        case wxTYPE_FRAME:
        {
          BuildFrameData *data = new BuildFrameData(NULL);
          expr->SetClientData(data);
          
          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);

          if (topLevelFrameId == data->id)
            topLevelFrame = data;
          topLevelWindows.Append(data);

          // Add menu bar
          int menuBarId = 0;
          expr->AssignAttributeValue("menu_bar", &menuBarId);
          PrologExpr *menuExpr = database.HashFind("menu_item", menuBarId);
          if (menuExpr)
          {
            BuildMenuItem *menuBar = (BuildMenuItem *)menuExpr->GetClientData();
            if (menuBar)
            {
              delete data->buildMenuBar;
              data->buildMenuBar = menuBar;
            }
          }

          // Add toolbar
          int toolBarId = 0;
          expr->AssignAttributeValue("tool_bar", &toolBarId);
          PrologExpr *toolbarExpr = database.HashFind("window", toolBarId);
          if (toolbarExpr)
          {
            BuildToolbarData *toolBar = (BuildToolbarData *)toolbarExpr->GetClientData();
            if (toolBar)
            {
              data->toolbar = toolBar;
              toolBar->buildParent = data;
            }
          }
          break;
        }
        case wxTYPE_DIALOG_BOX:
        {
          BuildDialogBoxData *data = new BuildDialogBoxData(NULL);
          expr->SetClientData(data);
          
          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);

          topLevelWindows.Append(data);

          break;
        }
        case wxTYPE_PANEL:
        {
          BuildFrameData *parentFrame = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentFrame = (BuildFrameData *)parentExpr->GetClientData();

          if (!parentFrame)
          {
            wxMessageBox("Subwindow has no parent frame!", "Error");
            break;
          }

          BuildPanelData *data = new BuildPanelData(parentFrame);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);

          break;
        }
        case wxTYPE_CANVAS:
        {
          BuildFrameData *parentFrame = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentFrame = (BuildFrameData *)parentExpr->GetClientData();

          if (!parentFrame)
          {
            wxMessageBox("Subwindow has no parent frame!", "Error");
            break;
          }

          BuildCanvasData *data = new BuildCanvasData(parentFrame);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);

          break;
        }
        case wxTYPE_TEXT_WINDOW:
        {
          BuildFrameData *parentFrame = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentFrame = (BuildFrameData *)parentExpr->GetClientData();

          if (!parentFrame)
          {
            wxMessageBox("Subwindow has no parent frame!", "Error");
            break;
          }

          BuildTextWindowData *data = new BuildTextWindowData(parentFrame);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);

          break;
        }
        case wxTYPE_BUTTON:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildButtonData *data = new BuildButtonData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_CHECK_BOX:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildCheckBoxData *data = new BuildCheckBoxData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_MESSAGE:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildMessageData *data = new BuildMessageData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_TEXT:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildTextData *data = new BuildTextData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_MULTI_TEXT:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildMultiTextData *data = new BuildMultiTextData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_LIST_BOX:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildListBoxData *data = new BuildListBoxData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_RADIO_BOX:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildRadioBoxData *data = new BuildRadioBoxData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_SLIDER:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildSliderData *data = new BuildSliderData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_GAUGE:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildGaugeData *data = new BuildGaugeData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_GROUP_BOX:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildGroupBoxData *data = new BuildGroupBoxData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_TOOLBAR:
        {
          // Parent will be assigned later
          BuildToolbarData *data = new BuildToolbarData(NULL, FALSE);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        case wxTYPE_CHOICE:
        {
          BuildPanelData *parentPanel = NULL;
          PrologExpr *parentExpr = database.HashFind("window", parentId);
          if (parentExpr)
            parentPanel = (BuildPanelData *)parentExpr->GetClientData();

          if (!parentPanel)
          {
            wxMessageBox("Panel item has no parent panel!", "Error");
            break;
          }

          BuildChoiceData *data = new BuildChoiceData(parentPanel);
          expr->SetClientData(data);

          data->ReadPrologAttributes(expr, &database);
          data->ReadActions(expr, &database);
          break;
        }
        default:
        {
          wxMessageBox("Unrecognised window type.", "Error");
          break;
        }
      }
      expr = database.FindClauseByFunctor("window");
    }
    ResolveActionReferences(&database);
    TheFontManager.ReadFonts(&database);
    char buf[300];
    sprintf(buf, "wxBuilder [%s]", FileNameFromPath(filename));
    MainFrame->SetTitle(buf);
    UpdateWindowList(topLevelFrame);

    // Add to the project file history
    AddFileToHistory(filename);

    buildApp.ShowObjectEditor(topLevelFrame);

    wxEndBusyCursor();
  }
  else return FALSE;
  return TRUE;
}

void BuildApp::ResolveActionReferences(PrologDatabase *database)
{
  // Find all the windows
  database->BeginFind();
  PrologExpr *expr = database->FindClauseByFunctor("window");
  while (expr)
  {
    BuildWindowData *win = (BuildWindowData *)expr->GetClientData();
    if (win)
    {
      wxNode *node = win->buildActions.First();
      while (node)
      {
        BuildAction *action = (BuildAction *)node->Data();
        if (action->actionWindowId > -1)
        {
          PrologExpr *winExpr = database->HashFind("window", action->actionWindowId);
          if (winExpr)
            action->actionWindow = (BuildWindowData *)winExpr->GetClientData();
        }
        node = node->Next();
      }
    }
    expr = database->FindClauseByFunctor("window");
  }
}

Bool BuildApp::ClearProject(void)
{
  wxNode *node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *data = (BuildWindowData *)node->Data();
    DeleteWindow(data);
    node = topLevelWindows.First();
  }
  ClearTree();
  if (projectFilename) delete[] projectFilename;
  projectFilename = NULL;
  MainFrame->SetTitle("wxBuilder [unnamed]");
  MakeModified(FALSE);
  return TRUE;
}

Bool BuildApp::Modified(void)
{
/*
  if (topLevelFrame || (topLevelWindows.Number() > 0))
    return TRUE;
  else
    return FALSE;
 */
 return isModified;
}

// Format panel items
Bool BuildApp::FormatItems(int formatTool)
{
  if (BuildSelections.Number() == 0)
  {
    wxMessageBox("Please select panel items to format", "Warning");
    return FALSE;
  }
  // Ensure all panel items have the same parent
  BuildWindowData *parent = NULL;
  wxNode *node = BuildSelections.First();
  while (node)
  {
    BuildWindowData *data = (BuildWindowData *)node->Data();
    if (!parent)
      parent = data->buildParent;
    if ((!data->buildParent) ||
        (!data->buildParent->IsKindOf(CLASSINFO(wxPanel))) ||
        (data->buildParent != parent))
    {
      wxMessageBox("Selections must be panel items and have same parent.", "Error");
      return FALSE;
    }
    node = node->Next();
  }

  BuildWindowData *first = GetFirstSelection();
  int firstX, firstY;
  int firstW, firstH;
  first->userWindow->GetPosition(&firstX, &firstY);
  first->userWindow->GetSize(&firstW, &firstH);
  int centreX = (int)(firstX + (firstW / 2));
  int centreY = (int)(firstY + (firstH / 2));

  node = BuildSelections.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    wxNode *next = node->Next();
    if (win->userWindow && (win != first))
    {
      int x, y, w, h;
      win->userWindow->GetPosition(&x, &y);
      win->userWindow->GetSize(&w, &h);

      int newX, newY;

      switch (formatTool)
      {
        case TOOLBAR_FORMAT_HORIZ:
        {
          newX = x;
          newY = (int)(centreY - (h/2.0));
          break;
        }
        case TOOLBAR_FORMAT_VERT:
        {
          newX = (int)(centreX - (w/2.0));
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_HORIZ_LEFT_ALIGN:
        {
          newX = firstX;
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_VERT_TOP_ALIGN:
        {
          newX = x;
          newY = firstY;
          break;
        }
        case TOOLBAR_FORMAT_HORIZ_RIGHT_ALIGN:
        {
          newX = firstX + firstW - w;
          newY = y;
          break;
        }
        case TOOLBAR_FORMAT_VERT_BOT_ALIGN:
        {
          newX = x;
          newY = firstY + firstH - h;
          break;
        }
        default:
          newX = x; newY = y;
          break;
      }

      win->userWindow->SetSize(newX, newY, w, h);
    }
    node = next;
  }
  if (parent && parent->userWindow && parent->userWindow->IsKindOf(CLASSINFO(wxPanel)))
  {
    wxPanel *panel = (wxPanel *)parent->userWindow;
    panel->GetPanelDC()->Clear();
    panel->Refresh();
  }

  return TRUE;
}

void BuildApp::AssociateObjectWithEditor(BuildWindowData *bwin)
{
  if (MainFrame->currentWindow == bwin)
    return;
  MainFrame->ClearEditor();
  MainFrame->currentWindow = bwin;
}

void BuildApp::DisassociateObjectWithEditor(BuildWindowData *bwin)
{
  if (!MainFrame)
    return;
    
  if (MainFrame->currentWindow != bwin)
    return;

  DeselectAll();
  
  MainFrame->ClearEditor();
  MainFrame->currentWindow = NULL;
}

Bool BuildApp::ShowObjectEditor(BuildWindowData *bwin)
{
  if (!bwin->userWindow)
    bwin->MakeRealWindow();
  if (!bwin->userWindow)
    return FALSE;
  
  if (MainFrame)
  {
    AssociateObjectWithEditor(bwin);
    MainFrame->Show(TRUE);
    EditorPaletteFrame->Show(TRUE);
    bwin->userWindow->Show(TRUE);
    DisplayTree(TRUE, bwin);
  }
  return TRUE;
}

void BuildApp::UpdateWindowList(BuildWindowData *bwin)
{
  MainFrame->buildWindowsItem->Clear();
  wxNode *node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    MainFrame->buildWindowsItem->Append(win->name, (char *)win);
    node = node->Next();
  }

  if (bwin)
  {
    int sel = MainFrame->buildWindowsItem->FindString(bwin->name);
    if (sel > -1)
      MainFrame->buildWindowsItem->SetSelection(sel);
  }
#ifdef wx_x
/*
  // wxListBox needs to be resized or it doesn't appear
  // under Motif! Maybe SetSelection contains the bug?
  // See fix in wxListBox::Append which may need to be
  // repeated elsewhere.
  int toolbarHeight = 28;
    
  int listW, listH, listX, listY;
  MainFrame->buildWindowsItem->GetSize(&listW, &listH);
  MainFrame->buildWindowsItem->GetPosition(&listX, &listY);
  int panelW = listW + 5;
  int panelH = listH + listY + 5;

  int frameW, frameH;
  MainFrame->GetClientSize(&frameW, &frameH);
  int canvasH = frameH - panelH - toolbarHeight;

  MainFrame->buildWindowsItem->SetSize(listX, listY, frameW - listX - 5, listH);
//  panel->SetSize(0, toolbarHeight+canvasH, frameW, panelH);
*/
#endif
}

BuildWindowData *BuildApp::FindCurrentWindow(void)
{
  int sel = MainFrame->buildWindowsItem->GetSelection();
  if (sel > -1)
  {
    BuildWindowData *win = (BuildWindowData *)MainFrame->buildWindowsItem->GetClientData(sel);
    return win;
  }
  else return NULL;
}

// Delete an arbitrary window object
Bool BuildApp::DeleteWindow(BuildWindowData *window)
{
  if (window == topLevelFrame)
    topLevelFrame = NULL;

  if (MainFrame && (MainFrame->currentWindow == window))
  {
    MainFrame->canvas->Clear();
  }

  window->DestroyRealWindow();
  topLevelWindows.DeleteObject(window);
  delete window;

  UpdateWindowList();
  MakeModified();

  return TRUE;
}


void BuildApp::CreateObjectMenu(void)
{
  objectMenu = new wxMenu(NULL, (wxFunction)ObjectMenuProc);
  objectMenu->Append(WXBUILD_OBJECT_MENU_EDIT, "Edit attributes");
  objectMenu->Append(WXBUILD_OBJECT_MENU_DELETE, "Delete object");
}

void ObjectMenuProc(wxMenu& menu, wxCommandEvent& event)
{
  BuildWindowData *data = (BuildWindowData *)menu.GetClientData();
  if (!data)
    return;

  switch (event.commandInt)
  {
    case WXBUILD_OBJECT_MENU_EDIT:
    {
      data->EditAttributes();
      data->DestroyRealWindow();
      data->MakeRealWindow();
      break;
    }
    case WXBUILD_OBJECT_MENU_DELETE:
    {
      if (data == MainFrame->currentWindow)
      {
        buildApp.DisassociateObjectWithEditor(MainFrame->currentWindow);
      }
      buildApp.DeleteWindow(data);
      if (MainFrame->currentWindow)
        DisplayTree(TRUE, MainFrame->currentWindow);
      else
        DisplayTree(FALSE, NULL);
      break;
    }
  }
}

