/*
 * File:	bgencpp.cc
 * Purpose:	wxWindows GUI builder: C++ generation
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
#include <ctype.h>
#include <stdlib.h>
#if !defined(USE_IOSTREAMH) || USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include "wxbuild.h"
#include "bapp.h"
#include "namegen.h"
#include "bframe.h"
#include "bsubwin.h"
#include "bgencpp.h"
#include "bgdi.h"

// Compiler types, as integers
enum MS_COMPILERS {
  COMPILER_MS_C7,
  COMPILER_MS_C8,
  COMPILER_MS_NT,
  COMPILER_BC_31,
  COMPILER_BC_4,
  COMPILER_WATCOM_95,
  COMPILER_WATCOM_10,
  COMPILER_SYMANTEC
};

Bool BuildApp::CheckCompleteCPP(void)
{
  ShowReportWindow();
  ClearReportWindow();
  
  if (!topLevelFrame)
  {
    Report("No top frame -- generation aborted.\n");
    wxMessageBox("Please create a top level frame.", "Error");
    return FALSE;
  }
  if (!(topLevelFrame->className && strlen(topLevelFrame->className) > 0))
  {
    char *s = wxGetTextFromUser("Enter a class name for the top level frame.");
    if (s)
    {
      topLevelFrame->className = copystring(s);
    }
    else
    {
      Report("No class name for top level frame -- generation aborted.\n");
      return FALSE;
    }
  }
  if (!(appClass && strlen(appClass) > 0))
  {
    char *s = wxGetTextFromUser("Enter a class name for the whole application.");
    if (s)
    {
      appClass = copystring(s);
    }
    else
    {
      Report("No class name for application -- generation aborted.\n");
      return FALSE;
    }
  }
  if (!(buildName && strlen(buildName) > 0))
  {
    char *s = wxGetTextFromUser("Enter a source code root name.");
    if (s)
    {
      buildName = copystring(s);
    }
    else
    {
      Report("No source code root name -- generation aborted.\n");
      return FALSE;
    }
  }
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif
  if (!DirExists(dir))
  {
    char buf[400];
    sprintf(buf, "Create directory %s?", dir);
    if (wxMessageBox(buf, "Message", wxYES_NO) == wxYES)
    {
      wxMkdir(dir);
      Report("Project directory "); Report(dir); Report(" created.\n");
    }
    else
    {
      Report("No project directory -- generation aborted.\n");
      return FALSE;
    }
  }
  return TRUE;
}

Bool BuildApp::GenerateCPP(void)
{
  wxBeginBusyCursor();

  if (!CheckCompleteCPP())
  {
    wxEndBusyCursor();
    return FALSE;
  }

  nameSpace.ClearNameSpace();
  currentMenuItemId = 1;
  
  MainFrame->SetStatusText("Generating C++...");

  // Set up files
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif
  char headerName[100];
  char headerPath[400];
  strcpy(headerName, buildName);
  strcat(headerName, ".h");

  strcpy(headerPath, dir);
#ifdef wx_msw
  strcat(headerPath, "\\");
#endif
#ifdef wx_x
  strcat(headerPath, "/");
#endif
  strcat(headerPath, buildName);
  strcat(headerPath, ".h");

  char appName[100];
  char appPath[400];

  // Filename
  strcpy(appName, buildName);
  strcat(appName, extensionCPP);

  // Path
  strcpy(appPath, dir);
#ifdef wx_msw
  strcat(appPath, "\\");
#endif
#ifdef wx_x
  strcat(appPath, "/");
#endif
  strcat(appPath, buildName);
  strcat(appPath, extensionCPP);
  
  ofstream headerFile(headerPath);
  if (headerFile.bad())
  {
    wxEndBusyCursor();
    Report("Could not create header file ");
    Report(headerPath);
    Report(" -- compilation aborted.\n");
    wxMessageBox("Cannot create header file.", "Error");
    return FALSE;
  }
  ofstream appFile(appPath);
  if (appFile.bad())
  {
    wxEndBusyCursor();
    Report("Could not create source file ");
    Report(appPath);
    Report(" -- compilation aborted.\n");
    wxMessageBox("Cannot create source file.", "Error");
    return FALSE;
  }

  WriteFileHeaderCPP(headerFile, headerName, buildDescription, buildAuthors, buildDate);
  WriteFileHeaderCPP(appFile, appName, buildDescription, buildAuthors, buildDate);

  WriteMutualExclusionCPP(headerFile, headerName);
  WriteExtraIncludesCPP(headerFile);
  WriteIncludesCPP(appFile, headerName);

  WriteAppClassCPP(headerFile, appClass);
  WriteAppInitCPP(appFile, appClass);

  WriteWindowClassDeclarationsCPP(headerFile);
  WriteWindowClassImplementationsCPP(appFile);

  // End mutual exclusion in header
  headerFile << "#endif /* mutual exclusion */\n";

  if (genMakefiles)
    GenerateMakefiles();
  if (genRCFile)
    GenerateRCFile();
  if (useResourceMethod)
  {
    GenerateWXResourceDefinesFile();
    GenerateWXResourceFile();
  }
  if (genDefFile)
    GenerateDefFile();

  // Set status line.
  char statusBuf[300];
  sprintf(statusBuf, "Wrote %s.", buildName);
  MainFrame->SetStatusText(statusBuf);
  Report("Generation successful.\n");

  wxEndBusyCursor();

  return TRUE;
}

void BuildApp::WriteFileHeaderCPP(ostream& stream, char *fileName, char *description,
  char *authors, char *date)
{
  stream << "/*\n * File:    " << fileName << "\n";
  stream <<     " * Purpose: main application module, generated by wxBuilder.\n";
  if (authors && *authors)
      stream << " * Author:  " << authors << "\n";
  if (date && *date)
      stream << " * Date:    " << date << "\n";
  if (description && *description)
    stream << " * Description:\n * " << description << "\n";
  stream << " */\n\n";

}

void BuildApp::WriteMutualExclusionCPP(ostream& stream, char *name)
{
  char buffer[200];
  char *tcp = buffer;
  size_t len;
  char buffer1[200];

  strcpy(buffer1, FileNameFromPath(name));
  len = strlen(buffer1);

  *tcp++ = '_'; *tcp++ = 'I'; *tcp++ = 'N'; *tcp++ = 'C'; *tcp++ = '_';
  for (size_t i = 0; i < len; i++)
    *tcp++ = (isalnum(buffer1[i]) ? toupper(buffer1[i]) : '_');
  *tcp = '\0';

  stream << "#ifndef " << buffer << "\n#define " << buffer << "\n\n";
/*
  char buffer[200];
  char buffer1[200];
  strcpy(buffer1, FileNameFromPath(name));
  int j = 0;
  for (int i = 0; i < (int)strlen(buffer1); i++)
   if (isalpha(buffer1[i]))
   {
     buffer[j] = buffer1[i];
     j ++;
   }
  buffer[j] = 0;
  stream << "#ifndef " << buffer << "\n#define " << buffer << "\n\n";
*/
}

void BuildApp::WriteExtraIncludesCPP(ostream& stream)
{
  if (UsesToolBar())
  {
    stream << "#include \"wx_tbar.h\"\n";
  }
}

void BuildApp::WriteIncludesCPP(ostream& stream, char *headerName)
{
  stream << "#include \"wx.h\"\n#include \"wx_help.h\"\n";
  stream << "#include \"" << headerName << "\"\n\n";
  if (useResourceMethod)
  {
    stream << "// Includes the dialog, menu etc. resources\n";
    stream << "#include \"" << buildName << ".wxr\"\n\n";

    // Write any XBM or XPM data includes
    resourcesGenerated.Clear();
    wxNode *node = topLevelWindows.First();
    while (node)
    {
      BuildWindowData *winData = (BuildWindowData *)node->Data();
      winData->GenerateBitmapDataIncludes(stream);
      node = node->Next();
    }
  }
}

void BuildApp::WriteAppClassCPP(ostream& stream, char *className)
{
  // Forward declarations of all top-level window classes.
  stream << "/*\n * Forward declarations of all top-level window classes.\n */\n";
  wxNode *node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *winData = (BuildWindowData *)node->Data();
    stream << "class  " << winData->className << ";\n";
    node = node->Next();
  }
  stream << "\n";

  stream << "/*\n * Class representing the entire Application\n */\n";
  stream << "class " << className << ": public wxApp\n{\n";
  stream << " public:\n";

  // Have all top-level windows as members of the App, so we can reference
  // them.
  node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *winData = (BuildWindowData *)node->Data();
    stream << "  " << winData->className << " *" << winData->name << ";\n";
    node = node->Next();
  }
  // GDI objects are also members.
  node = TheFontManager.First();
  while (node)
  {
    BuildFontData *data = (BuildFontData *)node->Data();
    stream << "  wxFont *" << data->fontName << ";\n";
    node = node->Next();
  }
  stream << "\n";
  
  stream << "  wxFrame *OnInit(void);\n";
  stream << "  Bool ParseCommandLine(void);\n";
  stream << "};\n\n";

}

void BuildApp::WriteAppInitCPP(ostream& stream, char *className)
{
  // Declare an instance of the application class
  stream << "// Declare an instance of the application: allows the program to start\n";
  stream << className << " theApp;\n\n";

  nameSpace.BeginBlock();

  // Create a new top level frame.
  // All the interesting frame stuff is done in the frame constructor.

  stream << "// Called to initialize the program\n";
  stream << "wxFrame *" << className << "::OnInit(void)\n{\n";

  // Load wxWindows resources, if any
  if (useResourceMethod)
  {
    // First register any included bitmaps with the resource
    // system
    resourcesGenerated.Clear();
    wxNode *node = buildApp.topLevelWindows.First();
    while (node)
    {
      BuildWindowData *win = (BuildWindowData *)node->Data();
      win->GenerateWXResourceBitmapRegistration(stream);
      node = node->Next();
    }

    resourcesGenerated.Clear();
    node = topLevelWindows.First();
    while (node)
    {
      BuildWindowData *winData = (BuildWindowData *)node->Data();
      winData->GenerateResourceLoading(stream);
      node = node->Next();
    }
  }

  // Initialize fonts
  wxNode *node = TheFontManager.First();
  while (node)
  {
    BuildFontData *data = (BuildFontData *)node->Data();
    stream << "  " << data->fontName << " = new wxFont(";
    stream << data->fontSize << ", ";
    if (strcmp(data->fontFamily, "Roman") == 0)
      stream << "wxROMAN, ";
    else if (strcmp(data->fontFamily, "Decorative") == 0)
      stream << "wxDECORATIVE, ";
    else if (strcmp(data->fontFamily, "Script") == 0)
      stream << "wxSCRIPT, ";
    else if (strcmp(data->fontFamily, "Modern") == 0)
      stream << "wxMODERN, ";
    else if (strcmp(data->fontFamily, "Teletype") == 0)
      stream << "wxTELETYPE, ";
    else
      stream << "wxSWISS, ";

    if (strcmp(data->fontStyle, "Italic") == 0)
      stream << "wxITALIC, ";
    else if (strcmp(data->fontStyle, "Slant") == 0)
      stream << "wxSLANT, ";
    else
      stream << "wxNORMAL, ";
    if (strcmp(data->fontWeight, "Light") == 0)
      stream << "wxLIGHT";
    else if (strcmp(data->fontWeight, "Bold") == 0)
      stream << "wxBOLD";
    else
      stream << "wxNORMAL";
    stream << ");\n";

    node = node->Next();
  }
  stream << "\n";


  // Initialize all the top-level window members to NULL.
  stream << "  // Initialize all the top-level window members to NULL.\n";
  node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *winData = (BuildWindowData *)node->Data();
    stream << "  " << winData->name << " = NULL;\n";
    node = node->Next();
  }

  stream << "  if (!ParseCommandLine())\n    return NULL;\n\n";

  stream << "  " << topLevelFrame->name << " = \n    ";

  topLevelFrame->GenerateConstructorCall(stream, "NULL");
  stream << ";\n\n";

  // Return the top level frame
  stream << "  return " << topLevelFrame->name << ";\n}\n\n";

  nameSpace.EndBlock();

  WriteCommandParsingCPP(stream);
}

// Write class declarations and callback function prototypes
void BuildApp::WriteWindowClassDeclarationsCPP(ostream& stream)
{
  wxNode *node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *winData = (BuildWindowData *)node->Data();
    winData->WriteClassDeclaration(stream);
    node = node->Next();
  }
}

void BuildApp::WriteWindowClassImplementationsCPP(ostream& stream)
{
  wxNode *node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *winData = (BuildWindowData *)node->Data();
    winData->WriteClassImplementation(stream);
    node = node->Next();
  }
}

void BuildApp::WriteCommandParsingCPP(ostream& stream)
{
  stream << "// Parses the command line\n";
  stream << "Bool " << appClass << "::ParseCommandLine(void)\n{\n";
  stream << "  return TRUE;\n";
  stream << "}\n\n";
}

Bool BuildApp::GenerateMakefiles(void)
{
  if (strcmp(compilerDOS, "Microsoft C/C++ Vsn 7") == 0)
    GenerateMakefileMS(COMPILER_MS_C7);
  else if (strcmp(compilerDOS, "Microsoft Visual C++") == 0)
    GenerateMakefileMS(COMPILER_MS_C8);
  else if (strcmp(compilerDOS, "Microsoft C++ for NT") == 0)
    GenerateMakefileMS(COMPILER_MS_NT);
  else if (strcmp(compilerDOS, "Borland C++ 3.1") == 0)
    GenerateMakefileBorland(COMPILER_BC_31);
  else if (strcmp(compilerDOS, "Borland C++ 4.x") == 0)
    GenerateMakefileBorland(COMPILER_BC_4);
  else if (strcmp(compilerDOS, "Symantec C++") == 0)
    GenerateMakefileSymantec();
  else if (strcmp(compilerDOS, "Watcom C++") == 0)
    GenerateMakefileWatcom(COMPILER_WATCOM_95);
  GenerateMakefileUNIX();
  return TRUE;
}

Bool BuildApp::GenerateMakefileMS(int compType)
{
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif

  char makeName[400];
  strcpy(makeName, dir);
#ifdef wx_msw
  strcat(makeName, "\\");
#endif
#ifdef wx_x
  strcat(makeName, "/");
#endif
  if (compType == COMPILER_MS_NT)
    strcat(makeName, "makefile.nt");
  else
    strcat(makeName, "makefile.dos");

  ofstream stream(makeName);
  if (stream.bad())
  {
    wxMessageBox("Cannot create Microsoft C++ makefile.", "Error");
    return FALSE;
  }
  stream << "#!nmake\n";
  stream << "# " << compilerDOS << " makefile for " << buildName << "\n";
  stream << "# Generated by wxBuilder.\n\n";

  if (compType == COMPILER_MS_NT)
  {
    stream << "!include <ntwin32.mak>\n";
    stream << "OS_SPEC=/nologo -DWINNT -DMS_WINDOWS -Dwx_msw\n";
    stream << "ALLFLAGS= $(OS_SPEC) $(cflags) $(cvars) $(cdebug)\n";
    stream << "MAKEFILE=makefile.nt\n";
  } else {
    stream << "CC=cl\n";
    stream << "MODEL=L\n";
    stream << "MAKEFILE=makefile.dos\n";
  }
  stream << "# Change WXDIR to wherever wxWindows is found.\n";
  
  stream << "WXDIR = " << wxDirDOS << "\n";
//  stream << "WXLIB = $(WXDIR)\\lib\\wx$(MODEL).lib\n";
  stream << "WXLIB = $(WXDIR)\\lib\\wx.lib\n";
  stream << "WXINC = $(WXDIR)\\include\n";

  Bool usesToolBar = UsesToolBar();
  Bool usesLoadBitmap = UsesLoadBitmap();

  stream << "THISDIR = " << projectDirDOS << "\n\n";

  stream << "OBJECTS = " << buildName << ".obj\n\n";
  stream << "LIBS=$(WXLIB) ";

  if (extraLibsMSW && (strlen(extraLibsMSW) > 0))
    stream << extraLibsMSW << " ";

  stream << "oldnames.lib libw llibcew commdlg ddeml shell ctl3d\n\n";
  stream << "!ifndef FINAL\n";
  stream << "FINAL=0\n";
  stream << "!endif\n\n";

  stream << "INC=/I$(WXDIR)\\include\\base /I$(WXDIR)\\include\\msw";

  if (extraIncludesMSW && (strlen(extraIncludesMSW) > 0))
    stream << extraIncludesMSW << " ";

  stream << "\n";

  if (compType != COMPILER_MS_NT)
  {
    stream << "WARN=/W3\n";
    stream << "#PRECOMP=/Yuwx.h\n";
    stream << "#PCHFILE=/Fp$(WXDIR)\\src\\msw\\wx.pch\n\n";

    stream << "!if \"$(FINAL)\" == \"0\"\n";
    stream << "OPT=/Od\n";

    if (compType == COMPILER_MS_C8)
      stream << "CPPFLAGS= /AL /W3 /Zi /Z7 /G2sw $(OPT) $(PRECOMP) /Dwx_msw $(INC) /Fp$(WXDIR)\\src\\msw\\wx.pch\n";
    else
      stream << "CPPFLAGS= /AL /W3 /Zi /G2sw $(OPT) $(PRECOMP) /Dwx_msw $(INC) $(PCHFILE)\n";
    stream << "LINKFLAGS=/NOD /SEG:512 /CO /ONERROR:NOEXE\n";
    stream << "!else\n";
    stream << "OPT=/Ox\n";
    if (compType == COMPILER_MS_C8)
      stream << "CPPFLAGS= /AL /W3 /Z7 /G2sw $(OPT) $(PRECOMP) /Dwx_msw $(INC) /Fp$(WXDIR)\\src\\msw\\wx.pch\n";
    else
      stream << "CPPFLAGS= /AL /W3 /G2sw $(OPT) $(PRECOMP) /Dwx_msw $(INC) $(PCHFILE)\n";
    stream << "LINKFLAGS=/NOD /SEG:512 /ONERROR:NOEXE\n";
    stream << "!endif\n\n";
  }
  
  stream << "all:    " << buildName << ".exe\n\n";

  stream << "wx:\n";
  stream << "\tcd $(WXDIR)\\src\\msw\n";
  if (compType == COMPILER_MS_NT)
    stream << "\tnmake -f makefile.nt FINAL=$(FINAL)\n";
  else
    stream << "\tnmake -f makefile.dos FINAL=$(FINAL) OPT=$(OPT) PRECOMP=$(PRECOMP)\n";
  stream << "\tcd $(THISDIR)\n\n";

  if (compType == COMPILER_MS_NT)
  {
    stream << buildName << ".obj: " << buildName << ".h " << buildName << extensionCPP << "\n";
    stream << "\t$(cc) @<<\n";
    stream << "$(ALLFLAGS) $(INC) /c /Tp $*" << extensionCPP << "\n";
    stream << "<<\n\n";
  }
  else
  {
    stream << buildName << ".obj: " << buildName << ".h " << buildName << extensionCPP << "\n";
    stream << "\tcl @<<\n";
    stream << "$(CPPFLAGS) /c /Tp $*" << extensionCPP << "\n";
    stream << "<<\n\n";
  }
  
  if (compType == COMPILER_MS_NT)
  {
    stream << buildName << ".res :      " << buildName << ".rc $(WXDIR)\\include\\msw\\wx.rc\n";
    stream << "\t$(rc) -r /i$(WXDIR)\\include\\msw /i$(WXDIR)\\contrib\\fafa -fotmp.tmp " << buildName << ".rc\n";
    stream << "\t$(cvtres) -$(CPU) -o " << buildName << ".res tmp.tmp\n";
    stream << "\tdel tmp.tmp\n\n";
  }
  else
  {
    stream << buildName << ".res :      " << buildName << ".rc $(WXDIR)\\include\\msw\\wx.rc\n";
    stream << "\trc /i$(WXDIR)\\include\\msw /i$(WXDIR)\\contrib\\fafa -r " << buildName << "\n\n";
  }
  
  if (compType == COMPILER_MS_NT)
  {
    stream << buildName << ".exe:      $(WXDIR)\\src\\msw\\dummy.obj $(WXLIB) $(OBJECTS) " << buildName << ".def " << buildName << ".res\n";
    stream << "\t$(link) @<<\n";
    stream << "-out:" << buildName << ".exe\n";
    stream << "-machine:$(CPU)\n";
    stream << "-subsystem:windows\n";
    stream << "$(LINKFLAGS)\n";
    stream << "$(WXDIR)\\src\\msw\\dummy.obj " << buildName << ".obj " << buildName << ".res\n";
    stream << "$(WXLIB)\n";
    stream << "shell32.lib $(guilibs)\n";
    stream << "<<\n";
  }
  else
  {
    stream << buildName << ".exe:      $(WXDIR)\\src\\msw\\dummy.obj $(WXLIB) $(OBJECTS) " << buildName << ".def " << buildName << ".res\n";
    stream << "\tlink $(LINKFLAGS) @<<\n";
    stream << "$(WXDIR)\\src\\msw\\dummy.obj $(OBJECTS)\n";
    stream << buildName << ".exe,\n";
    stream << "nul,\n";
    stream << "$(LIBS),\n";
    stream << buildName << ".def\n";
    stream << "<<\n";
    stream << "\trc -K " << buildName << ".res\n\n";
  }
  
  stream << "clean:\n\t-erase *.obj *.exe *.res\n";

  return TRUE;
}

Bool BuildApp::GenerateMakefileBorland(int compType)
{
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif

  char makeName[400];
  strcpy(makeName, dir);
#ifdef wx_msw
  strcat(makeName, "\\");
#endif
#ifdef wx_x
  strcat(makeName, "/");
#endif
  strcat(makeName, "makefile.bcc");

  ofstream stream(makeName);
  if (stream.bad()) {
    wxMessageBox("Couldn't create Borland C++ makefile.", "Error");
      return FALSE;
    }
  stream << "#!make\n";
  stream << "# "<< compilerDOS << " makefile for " << buildName << "\n";
  stream << "# Generated by wxBuilder.\n\n";
  stream << "CCC=bcc\n";
  stream << "MODEL=l\n";
  stream << "CFG = wxwin.cfg\n";
  stream << "PCH = wxwin.pch\n";
  stream << "\n# Change BCCDIR to wherever your Borland Compiler is found.\n";
  if (compType == COMPILER_BC_4)
    stream << "BCCDIR = C:\\BC4\n";
  else
    stream << "BCCDIR = C:\\BORLANDC\n";
  stream << "\n# Change WXDIR to wherever wxWindows is found.\n";

  if (compType == COMPILER_BC_4)
    stream << "BCCDIR = C:\\BC4\n";
  else
    stream << "BCCDIR = C:\\BORLANDC\n";
  stream << "WXDIR = " << wxDirDOS << "\n";
  stream << "WXLIB = $(WXDIR)\\lib\\$(MODEL)wx.lib\n";
  stream << "WXINC = $(WXDIR)\\include\n\n";

  stream << "THISDIR = " << projectDirDOS << "\n\n";

  stream << "OBJECTS = " << buildName << ".obj\n\n";

  stream << "LIBS=$(WXLIB) mathwl cwl import commdlg ddeml shell $(WXDIR)\\contrib\\ctl3d\\ctl3d\n";

  stream << "!ifndef FINAL\n";
  stream << "FINAL=0\n";
  stream << "!endif\n\n";

  stream << "!if \"$(FINAL)\" == \"0\"\n";
  stream << "OPT=-Od\n";
  stream << "DEBUG_FLAGS=-v\n";
  stream << "LINKFLAGS=/v/Vt /Twe /L$(WXDIR)\\lib;$(BCCDIR)\\lib\n";
  stream << "!else\n";
  stream << "OPT=-O2\n";
  stream << "DEBUG_FLAGS=\n";
  stream << "LINKFLAGS=/Twe /L$(WXDIR)\\lib;$(BCCDIR)\\lib\n";
  stream << "!endif\n\n";

  stream << "CPPFLAGS=$(DEBUG_FLAGS) $(OPT) @$(CFG)\n\n";

  stream << extensionCPP << ".obj:\n";
  stream << "\t$(CCC) $(CPPFLAGS) -c {$< }\n\n";
  
  stream << "all:    $(CFG) " << buildName << ".exe\n\n";

  stream << "$(CFG): makefile.bcc\n";
  stream << "\tcopy &&!\n";
  stream << "-H=$(PCH)\n";
  stream << "-3\n";
  stream << "-P\n";
  if (compType == COMPILER_BC_4) {
    stream << "-Ff=512\n";
    stream << "-dc\n";
  } else
    stream << "-Ff=4\n";
  stream << "-Fc\n";
  stream << "-d\n";
  stream << "-N\n";
  stream << "-w-hid\n";
  stream << "-w-par\n";
  stream << "-w-pia\n";
  stream << "-w-aus\n";
  stream << "-w-rch\n";
  stream << "-m$(MODEL)\n";
  stream << "-WSE\n";
  stream << "-I$(WXINC)\\msw;$(WXINC)\\base;$(BCCDIR)\\include\n";
  stream << "-Dwx_msw\n";
  stream << "! $(CFG)\n\n";

  stream << "wx:\n";
  stream << "\tcd $(WXDIR)\\src\\msw\n";
  stream << "\tmake -f makefile.bcc FINAL=$(FINAL) OPT=$(OPT)\n";
  stream << "\tcd $(THISDIR)\n\n";

  stream << buildName << ".obj: " << buildName << ".h " << buildName << extensionCPP << "\n";
  
  stream << buildName << ".res :      " << buildName << ".rc $(WXDIR)\\include\\msw\\wx.rc\n";
  stream << "\trc /i$(BCCDIR)\\include /i$(WXDIR)\\include\\msw /i$(WXDIR)\\include\\base /i$(WXDIR)\\contrib\\fafa -r " << buildName << "\n\n";
  
  stream << buildName << ".exe:      $(WXLIB) $(OBJECTS) " << buildName << ".def " << buildName << ".res\n";
  stream << "\ttlink $(LINKFLAGS) @&&!\n";
  stream << "c0wl.obj $(OBJECTS)\n";
  stream << buildName << "\n";
  stream << "nul\n";
  stream << "$(LIBS)\n";
  stream << buildName << ".def\n";
  stream << "!\n";
  stream << "\trc -K " << buildName << ".res\n\n";
  
  stream << "clean:\n";
  stream << "\t-erase *.obj\n";
  stream << "\t-erase *.exe\n";
  stream << "\t-erase *.res\n";
  stream << "\t-erase $(PCH)\n";

  return TRUE;
}

Bool BuildApp::GenerateMakefileWatcom(int )
{
#ifdef wx_msw
  char *dir = projectDirDOS;
#else
  char *dir = projectDirUNIX;
#endif

  char makeName[400];
  strcpy(makeName, dir);
#ifdef wx_msw
  strcat(makeName, "\\");
#else
  strcat(makeName, "/");
#endif
  strcat(makeName, "makefile.wat");

  ofstream stream(makeName);
  if (stream.bad()) {
    wxMessageBox("Couldn't create Watcom C++ makefile.", "Error");
    return FALSE;
  }
  stream << "#!wmake\n";
  stream << "# " << compilerDOS << " makefile for " << buildName << "\n";
  stream << "# Generated by wxBuilder.\n";
  stream << "CCC = wpp386\n";
  stream << "CFLAGS = -zW -oaxt -d1 -w2\n\n";

  stream << "NAME = " << buildName << "\n";
  stream << "OBJS = $(name).obj\n";
  stream << "LNK = $(name).lnk\n\n";

  stream << "# Change WXDIR to wherever wxWindows is found.\n";
  stream << "WXDIR = " << wxDirDOS << "\n";
  stream << "WXLIB = $(WXDIR)\\lib\\wx386.lib\n";
  stream << "WXINC = $(WXDIR)\\include\n";
  stream << "INC = -i$(WXDIR)\\include\\base -i$(WXDIR)\\include\\msw\n\n";

  stream << "$(name).exe : $(OBJS) $(name).res $(LNK)\n";
  stream << "\twlink @$(LNK)\n";
  stream << "\twbind $(name) -R $(inc) -30 $(name).res\n\n";

  stream << "$(name).res : $(name).rc\n";
  stream << "\twrc $(inc) -r -30 $(name).rc\n\n";

  stream << "$(LNK) : makefile.w32\n";
  stream << "\t%create $(LNK)\n";
  stream << "\t@%append $(LNK) debug all\n";
  stream << "\t@%append $(LNK) sys win386\n";
  stream << "\t@%append $(LNK) option mindata=100K\n";
  stream << "\t@%append $(LNK) option maxdata=100K\n";
  stream << "\t@%append $(LNK) option stack=8k\n";
  stream << "\t@%append $(LNK) name $(name)\n";
  stream << "\t@for %i in ($(LIBS)) do @%append $(LNK) library %i\n";
  stream << "\t@for %i in ($(OBJS)) do @%append $(LNK) file %i\n\n";

  stream << extensionCPP << ".obj :\n";
  stream << "\t$(CCC) $(CFLAGS) $(INC) $[* \n\n";

  return TRUE;
}

Bool BuildApp::GenerateMakefileSymantec(void)
{
  return FALSE;
}

Bool BuildApp::GenerateMakefileUNIX(void)
{
  char makeName[400];

  // Determine C++ lib for compiler
/*
  int have_gcc = (strlen(compilerUNIX) > 2 &&
    (strncmp(compilerUNIX, "gcc", 3) == 0 ||
     strncmp(compilerUNIX, "g++", 3) == 0) );
*/
  int have_gcc = (strlen(compilerUNIX) > 2 &&
    (strncmp(compilerUNIX+(strlen(compilerUNIX)-3), "gcc", 3) == 0 ||
     strncmp(compilerUNIX+(strlen(compilerUNIX)-3), "g++", 3) == 0) );
     
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif
  strcpy(makeName, dir);
#ifdef wx_msw
  strcat(makeName, "\\");
#endif
#ifdef wx_x
  strcat(makeName, "/");
#endif
  strcat(makeName, "makefile.unx");

  ofstream stream(makeName);
  if (stream.bad())
  {
    Report("Could not create UNIX makefile.\n");
    wxMessageBox("Cannot create UNIX makefile.", "Error");
    return FALSE;
  }
  stream << "#!/bin/make\n";
  stream << "# UNIX makefile for " << buildName << "\n";
  stream << "# Generated by wxBuilder.\n\n";
  stream << "# Change WXDIR to wherever wxWindows is found.\n\n";
  stream << "CC = " << compilerUNIX << "\n";
  stream << "GUISUFFIX=_ol\n\n";

  stream << "# Replace X include/lib directories with your own.\n";
  stream << "XINCLUDE=" << XIncludes << "\n";
  stream << "XLIB=" << XLibs << "\n\n";

  stream << "WXDIR = " << wxDirUNIX << "\n";
  stream << "WXLIB = $(WXDIR)/lib/libwx$(GUISUFFIX).a\n";
  stream << "WXINC = $(WXDIR)/include\n";
  stream << "INC = -I$(WXINC)/base -I$(WXINC)/x";

  if (extraIncludesX && (strlen(extraIncludesX) > 0))
    stream << " " << extraIncludesX;

  stream << "\n";
  stream << "THISDIR = " << projectDirUNIX << "\n";
  stream << "OBJDIR = objects$(GUISUFFIX)\n\n";
  stream << "OBJECTS = $(OBJDIR)/" << buildName << ".o\n";

  stream << "\nDEBUG = -ggdb\n";
  stream << "WARN = -Wall\n";
  stream << "OPT = -O\n";

  stream << "GUI = -Dwx_xview\n";
  stream << "XVIEW_LINK = $(WXDIR)/src/x/objects_ol/sb_scrol.o\n\n";

  stream << "CPPFLAGS = $(XINCLUDE) $(INC) $(GUI) $(DEBUG) $(WARN)";

  stream << "\n";
  stream << "LDFLAGS = $(XLIB) -L$(WXDIR)/lib";

  if (extraLibsX && (strlen(extraLibsX) > 0))
    stream << " " << extraLibsX;

  stream << "\n";

  stream << "LDLIBS =";

  stream << " -lwx_ol -lxview -lolgx -lX11 -lm -lg++";
  if (have_gcc)
    stream << " -lg++";
  else
    stream << " -lC";
  stream << "\n\n";

  stream << "all: $(OBJDIR) " << buildName << "$(GUISUFFIX)\n\n";
  stream << ".SUFFIXES:\n\n";

  stream << "motif:\n";
  stream << "\tmake -f makefile.unx GUISUFFIX=_motif GUI=-Dwx_motif OPT=$(OPT) LDLIBS='-lwx_motif";

  stream << " -lXm -lXt -lX11 -lm";
  if (have_gcc)
    stream << " -lg++";
  else
    stream << " -lC";
  stream << "' XVIEW_LINK=\n";

  stream << "\nhp:\n";
  stream << "\tmake -f makefile.unx GUISUFFIX=_hp GUI=-Dwx_hp OPT=$(OPT) LDLIBS='-lwx_hp";

  stream << " -lXm -lXt -lX11 -lm";
  if (have_gcc)
    stream << " -lg++";
  else
    stream << " -lC";
  stream << "' XVIEW_LINK='' \\\n";
  stream << "\tCC=CC DEBUG='-g' WARN='-w' OPT='' \\\n";
  stream << "\tXINCLUDE='-I/usr/include/Motif1.2 -I/usr/include/X11R5' \\\n";
  stream << "\tXLIB='-L/usr/lib/Motif1.2 -L/usr/lib/X11R5'\n\n";

  stream << "xview:\n";
  stream << "\tmake -f makefile.unx GUISUFFIX=_ol GUI=-Dwx_xview OPT=$(OPT) LDLIBS='-lwx_ol";

  stream << " -lxview -lolgx -lX11 -lm";
  if (have_gcc)
    stream << " -lg++";
  else
    stream << " -lC";
  stream << "'\n\n";
  
  stream << "wx_motif:\n";
  stream << "\tcd $(WXDIR)/src/x; make -f makefile.unx motif OPT=$(OPT)\n\n";
  stream << "wx_ol:\n";
  stream << "\tcd $(WXDIR)/src/x; make -f makefile.unx xview OPT=$(OPT)\n\n";
  stream << "wx_hp:\n";
  stream << "\tcd $(WXDIR)/src/x; make -f makefile.unx hp OPT=$(OPT)\n\n";

  stream << "$(OBJDIR):\n\tmkdir $(OBJDIR)\n\n";

  stream << "$(OBJDIR)/" << buildName << ".o: " << buildName << ".h " << buildName << extensionCPP << "\n";
  stream << "\t$(CC) -c $(CPPFLAGS) -o $@ " << buildName << extensionCPP << "\n\n";

  stream << buildName << "$(GUISUFFIX):\t" << "$(OBJECTS) $(WXLIB)";

  stream << "\n";
  stream << "\t$(CC) $(CPPFLAGS) $(LDFLAGS) -o " << buildName << "$(GUISUFFIX)" << " $(OBJDIR)/" << buildName << ".o $(XVIEW_LINK) $(LDLIBS)\n\n";

  stream << "cleaneach:\n";
  stream << "\trm -f $(OBJDIR)/*.o " << buildName << "$(GUISUFFIX) core\n\n";

  stream << "clean_ol:\n\tmake -f makefile.unx GUISUFFIX=_ol cleaneach\n\n";
  stream << "clean_hp:\n\tmake -f makefile.unx GUISUFFIX=_hp cleaneach\n\n";
  stream << "clean_motif:\n\tmake -f makefile.unx GUISUFFIX=_motif cleaneach\n\n";

  return TRUE;
}

Bool BuildApp::GenerateRCFile(void)
{
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif

  char rcName[400];
  strcpy(rcName, dir);
#ifdef wx_msw
  strcat(rcName, "\\");
#endif
#ifdef wx_x
  strcat(rcName, "/");
#endif
  strcat(rcName, buildName);
  strcat(rcName, ".rc");

  ofstream stream(rcName);
  if (stream.bad())
  {
    Report("Could not create RC file.\n");
    wxMessageBox("Cannot create RC file.", "Error");
    return FALSE;
  }
  resourcesGenerated.Clear();
  wxNode *node = buildApp.topLevelWindows.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    win->GenerateResourceEntries(stream);
    node = node->Next();
  }
  stream << "\nrcinclude wx.rc\n";
  return TRUE;
}

Bool BuildApp::GenerateWXResourceFile(void)
{
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif

  char rcName[400];
  strcpy(rcName, dir);
#ifdef wx_msw
  strcat(rcName, "\\");
#endif
#ifdef wx_x
  strcat(rcName, "/");
#endif
  strcat(rcName, buildName);
  strcat(rcName, ".wxr");

  ofstream stream(rcName);
  if (stream.bad())
  {
    Report("Could not create wxWindows resource file.\n");
    wxMessageBox("Cannot create wxWindows resource file.", "Error");
    return FALSE;
  }
  stream << "/*\n * wxWindows Resource File\n * Written by wxBuilder\n *\n */\n\n";
  stream << "#include \"" << buildName << ".ids\"\n\n";
  
  resourcesGenerated.Clear();
  wxNode *node = buildApp.topLevelWindows.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    win->GenerateWXResourceBitmaps(stream);
    node = node->Next();
  }

  resourcesGenerated.Clear();
  node = buildApp.topLevelWindows.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    win->GenerateWXResourceData(stream);
    node = node->Next();
  }
  return TRUE;
}

Bool BuildApp::GenerateWXResourceDefinesFile(void)
{
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif

  char rcName[400];
  strcpy(rcName, dir);
#ifdef wx_msw
  strcat(rcName, "\\");
#endif
#ifdef wx_x
  strcat(rcName, "/");
#endif
  strcat(rcName, buildName);
  strcat(rcName, ".ids");

  ofstream stream(rcName);
  if (stream.bad())
  {
    Report("Could not create wxWindows id include file.\n");
    wxMessageBox("Cannot create wxWindows id include file.", "Error");
    return FALSE;
  }
  stream << "/*\n * wxWindows Identifier Include File\n * Written by wxBuilder\n *\n */\n\n";
  
  wxNode *node = buildApp.topLevelWindows.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    win->GenerateDefines(stream);
    node = node->Next();
  }
  return TRUE;
}

Bool BuildApp::GenerateDefFile(void)
{
#ifdef wx_msw
  char *dir = projectDirDOS;
#endif
#ifdef wx_x
  char *dir = projectDirUNIX;
#endif
  char defName[400];
  strcpy(defName, dir);
#ifdef wx_msw
  strcat(defName, "\\");
#endif
#ifdef wx_x
  strcat(defName, "/");
#endif
  strcat(defName, buildName);
  strcat(defName, ".def");

  ofstream stream(defName);
  if (stream.bad())
  {
    Report("Could not create DEF file.\n");
    wxMessageBox("Cannot create DEF file.", "Error");
    return FALSE;
  }
  stream << "; " << buildName << "\n";
  stream << "; Generated by wxBuilder\n";
  stream << ";\n";
  stream << "NAME          " << buildName << "app\n";
  stream << "DESCRIPTION   'A wxWindows application'\n";
  stream << ";\n";
  stream << "EXETYPE      WINDOWS\n";
  stream << "STUB         'WINSTUB.EXE'\n";
  stream << ";\n";
  stream << "CODE         PRELOAD MOVEABLE DISCARDABLE\n";
  stream << "DATA         PRELOAD MOVEABLE MULTIPLE\n";
  stream << ";\n";
  stream << "HEAPSIZE     1024\n";
  stream << "STACKSIZE    8192\n";
  return TRUE;
}

Bool BuildApp::UsesToolBar(void)
{
  // Check for toolbars used
  wxNode *node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    if (win->windowType == wxTYPE_FRAME)
    {
      if (((BuildFrameData *)win)->toolbar)
      {
        return TRUE;
      }
    }
    node = node->Next();
  }
  return FALSE;
}

Bool BuildApp::UsesLoadBitmap(void)
{
  // Check for wxLoadBitmap used
  wxNode *node = topLevelWindows.First();
  while (node)
  {
    BuildWindowData *win = (BuildWindowData *)node->Data();
    if (win->windowType == wxTYPE_FRAME)
    {
      wxNode *node2 = win->children.First();
      while (node2)
      {
        BuildWindowData *win2 = (BuildWindowData *)node2->Data();
        if (win2->windowType == wxTYPE_CANVAS)
	{
          BuildCanvasData *canvasData = (BuildCanvasData *)win2;
          if (!buildApp.useResourceMethod && canvasData->bitmapData)
            return TRUE;
	}
        node2 = node2->Next();
      }
    }
    node = node->Next();
  }
  return FALSE;
}
