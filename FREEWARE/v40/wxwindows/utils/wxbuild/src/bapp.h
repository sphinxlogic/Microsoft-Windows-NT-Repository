/*
 * File:	bapp.h
 * Purpose:	wxWindows GUI builder -- application stuff
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

#ifndef bapph
#define bapph

#include "bwin.h"

class BuildWindowData;
class BuildFrameData;

#define MAX_FILE_HISTORY    5

// Define a new application
class BuildApp: public wxApp
{
 public:
  wxMenu *objectMenu;
  int newObjectX, newObjectY; // Temporary variables recording position of new object
  Bool usePosition; // Should we take note of the new position?
  BuildWindowData *objectParent; // Use this as the new object parent
  char *projectFilename;
  char *resourceFile; // E.g. WIN.INI, ~/.wxbuildrc
  wxList topLevelWindows;    // List of top-level frames and dialog boxes.
  wxStringList resourcesGenerated; // List of resource names generated,
                                   // so we don't repeat any
  BuildFrameData *topLevelFrame; // The top frame
  int topLevelFrameId; // top level frame id
  Bool testMode;
  char *buildDescription;
  char *buildAuthors;
  char *buildDate;
  int   buildMDIType;
  int   revision;
  char *extensionCPP;
  char *appClass;
  int  currentMenuItemId;

  // Makefile/compilation settings
  // (1) Per Project
  char *buildName;
  char *projectDirUNIX;
  char *projectDirDOS;
  char *commandLine;
  Bool genMakefiles;   // Generate UNIX and DOS makefiles
  Bool genRCFile;     // Generate an RC file
  Bool genDefFile;    // Generate a module definition file
  Bool separateFiles; // Separate file for each window class
  Bool useResourceMethod; // Use wxWindows resource system

  // (2) Per User (i.e. in WIN.INI, .Xdefaults)
  // UNIX
  char *XIncludes;
  char *XLibs;
  char *windowsInclude;
  char *guiTarget;    // One of xview, motif, and hp: for running program.
  char *wxDirUNIX;    // Directory of wxWindows under UNIX
  char *wxDirDOS;     // Directory of wxWindows under DOS
  char *extraIncludesX; // Extra include directories
  char *extraLibsX; // Extra library directories
  char *extraIncludesMSW; // Extra include directories
  char *extraLibsMSW; // Extra library directories

  // General
  Bool autoCompile;
  Bool autoRun;
  char *compilerDOS;    // Which compiler
  char *compilerUNIX;    // Which compiler

  int mainX;          // Main window position/size
  int mainY;
  int mainWidth;
  int mainHeight;

  int reportX;          // Report window position/size
  int reportY;
  int reportWidth;
  int reportHeight;

  int treeX;          // Tree window position/size
  int treeY;
  int treeWidth;
  int treeHeight;

  int paletteX;          // Palette position
  int paletteY;

  // Last 5 projects
  char *fileHistory[MAX_FILE_HISTORY];
  // Number of files saved
  int fileHistoryN;
  
  BuildApp(void);
  ~BuildApp(void);

  wxFrame *OnInit(void);

  void InitializeDefaults(void); // Set up defaults from WIN.INI/.Xdefaults
  void WriteDefaults(void); // Write defaults into WIN.INI (not .Xdefaults!)
  void EditProjectSettings(void);
  void EditGlobalSettings(void);

  void AddFileToHistory(char *file);

  Bool WritePrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool ReadPrologAttributes(PrologExpr *expr, PrologDatabase *database);
  Bool SaveProject(char *filename = NULL);
  Bool LoadProject(char *filename = NULL, Bool interactive = TRUE);
  // A phase after main reading-in -- connect actions to windows, since all
  // we initially have are ids.
  void ResolveActionReferences(PrologDatabase *database);
  Bool ClearProject(void);
  Bool Modified(void);
  inline Bool TestMode(void) { return testMode; }

  // Format panel items
  Bool FormatItems(int formatTool);

  Bool ShowObjectEditor(BuildWindowData *bwin);
  void AssociateObjectWithEditor(BuildWindowData *bwin);
  void DisassociateObjectWithEditor(BuildWindowData *bwin);
  BuildWindowData *FindCurrentWindow(void);
  void UpdateWindowList(BuildWindowData *bwin = NULL);

  Bool DeleteWindow(BuildWindowData *window); // Delete an arbitrary window object

  // The generation game...
  Bool CheckCompleteCPP(void);
  Bool GenerateCPP(void);

  Bool GenerateMakefiles(void);
  Bool GenerateMakefileMS(int compilerType);
  Bool GenerateMakefileBorland(int);
  Bool GenerateMakefileSymantec(void);
  Bool GenerateMakefileWatcom(int);
  Bool GenerateMakefileUNIX(void);

  Bool GenerateRCFile(void);
  Bool GenerateWXResourceFile(void);
  Bool GenerateWXResourceDefinesFile(void);
  Bool GenerateDefFile(void);

  void WriteFileHeaderCPP(ostream& stream, char *fileName, char *description,
                          char *authors, char *date);
  void WriteMutualExclusionCPP(ostream& stream, char *name);
  void WriteExtraIncludesCPP(ostream& stream);
  void WriteIncludesCPP(ostream& stream, char *headerName);
  void WriteAppClassCPP(ostream& stream, char *className);
  void WriteAppInitCPP(ostream& stream, char *className);
  void WriteWindowClassDeclarationsCPP(ostream& stream);
  void WriteWindowClassImplementationsCPP(ostream& stream);

  void WriteCommandParsingCPP(ostream& stream);

  // Does the application use toolbars?
  Bool UsesToolBar(void);
  // Does the application use wxLoadBitmap?
  Bool UsesLoadBitmap(void);

  inline void UsePosition(Bool usePos, BuildWindowData *parent = NULL, int x = 0, int y = 0)
  { usePosition = usePos; objectParent = parent; newObjectX = x; newObjectY = y; }
  inline Bool GetUsePosition(void) { return usePosition; }
  inline int GetObjectX(void) { return newObjectX; }
  inline int GetObjectY(void) { return newObjectY; }
  inline BuildWindowData *GetObjectParent(void) { return objectParent; }

  void CreateObjectMenu(void);
};

extern BuildApp buildApp;

extern float buildVersion;

#define WXBUILD_OBJECT_MENU_EDIT      1
#define WXBUILD_OBJECT_MENU_DELETE    2

#endif // wxbuildh

