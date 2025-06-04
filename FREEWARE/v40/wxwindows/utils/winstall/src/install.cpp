/*
 * File:	install.cc
 * Purpose:     wxWindows installation utility.
 * Author:      Julian Smart
 * Date:        20th April 1995
 *              Written in native Windows for small size.
 */

#define USE_OWN_LZH     0

#ifndef GNUWIN32
#define LZH_AVAILABLE   1
#else
#define LZH_AVAILABLE   0
#endif

#ifdef WIN32
#define PROCCAST 
#else
#define PROCCAST (int (far pascal *)(void))
#endif

#include "windows.h"
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ddeml.h>
#include <direct.h>
#include <dos.h>

#ifdef GNUWIN32
#include <sys/stat.h>
#include <sys/unistd.h>
#endif

#include "utils.h"
#include "install.h"
#include "resource.h"

const char *VersionString = "1.1";

#if USE_OWN_LZH
#include "lzw.h"
#else
#if LZH_AVAILABLE
#include <lzexpand.h>
#endif
#endif

/*
 * Globals
 */

Application     theApplication;

int             theNumberOfDisks = 0;
char *          theDisks[30];

int             theNumberOfDestDirectories = 0;
char *          theDestDirectories[30];
int             theNumberOfSourceDirectories = 0;
char *          theSourceDirectories[30];

int             theNumberOfFileGroups = 0;
FileGroup *     theFileGroups[30];
FileGroup       theWindowsFileGroup;

int             theNumberOfExecutionEntries = 0;
ExecutionEntry *theExecutionEntries[30];

int             theNumberOfPrerequisiteEntries = 0;
PrerequisiteEntry *thePrerequisiteEntries[30];

int             theNumberOfCompressors = 0;
CompressionMethod *theCompressors[30];

int             theNumberOfPMEntries = 0;
PMGroupEntry *  thePMEntries[60];

BOOL            dialogOK = TRUE;
HINSTANCE       theInstance = 0;
DLGPROC         copyDlgProcInst = 0;
HWND            copyDlgWindow = 0;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//+++extensions by Stefan Hammes (steve)
// my extension are marked with '+++steve' and '---steve'
// 
// e-mail: stefan.hammes@urz.uni-heidelberg.de
//
// 20-Feb-1997
// 04-Mar-1997

// Language support
//
// In the INF file you can now specify the language, e.g.:
// Language=german
// or
// Language=english

#include <stdarg.h>

#define L_english 0 // they act as indizes
#define L_german 1

static int language=L_english; // act as an index to the parameterlist

// usage: LConv("Welcome","Willkommen");
// as many arguments as there are defined languages.
static const char *LConv(const char *first, ...)
{ 
  const char *act = first;
  va_list marker;

  va_start(marker,first); 
  int i;
  for(i=0; i<language; i++){
    act = va_arg(marker, char *); // get corresponding argument
  }
  va_end(marker);
  return(act);
}

// usage: RConv(DIALOG_ID_ENGLISH, DIALOG_ID_GERMAN);
// as many arguments as there are defined languages.
static int RConv(int first, ...)
{ 
  int act = first;
  va_list marker;

  va_start(marker,first); 
  int i;
  for(i=0; i<language; i++){
    act = va_arg(marker, int); // get corresponding argument
  }
  va_end(marker);
  return(act);
}

// check for free space
BOOL EnoughSpace(void)
{
#if defined(WIN32)
  return TRUE;
#else
  const int drive = toupper(theApplication.GetDirectory()[0])-'A'+1; // A=1, B=2,...
  struct _diskfree_t driveInfo;
  /* Get information on disk drive */
  if(_dos_getdiskfree( drive, &driveInfo )==0){
    const unsigned long bytesFree = 
      (unsigned long)driveInfo.avail_clusters *
      (unsigned long)driveInfo.sectors_per_cluster *
      (unsigned long)driveInfo.bytes_per_sector;
    return(((unsigned long)theApplication.MyGetFreeSpace())*1024 < bytesFree);
  }else{
    return(FALSE); // cannot get info
  }
#endif
}  
//---steve
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Execute a DDE command
BOOL ExecuteCommand(char *server_name, char *topic, char *data);

#ifdef WIN32
#define _EXPORT /**/
#else
#define _EXPORT _export
#endif

BOOL FAR PASCAL _EXPORT InstallDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL FAR PASCAL _EXPORT CopyDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

extern "C" HDDEDATA EXPENTRY _EXPORT wxDdeCallback(
WORD wType,
WORD wFmt,
HCONV hConv,
HSZ hsz1,
HSZ hsz2,
HDDEDATA hData,
DWORD lData1,
DWORD lData2);

DWORD wxIdInst = 0L;
HCONV wxhConv = 0L;

#define SERVERPROGRAM   "PROGMAN"

BOOL ExecuteCommand(char *server_name, char *topic, char *data)
{
  HSZ wxServiceName = DdeCreateStringHandle(wxIdInst, server_name, CP_WINANSI);
  HSZ topic_atom = DdeCreateStringHandle(wxIdInst, topic, CP_WINANSI);

  HCONV hConv = DdeConnect(wxIdInst, wxServiceName, topic_atom, (PCONVCONTEXT)NULL);
  if (hConv == NULL)
    return FALSE;
  else
  {
    wxhConv = hConv;
    DWORD result;
    int size = strlen(data) + 1;

    DdeClientTransaction((LPBYTE)data, size, hConv,
      NULL, CF_TEXT, XTYP_EXECUTE, 5000, &result);
    
    (void)DdeDisconnect(hConv);
    return TRUE;
  }
}

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: Calls initialization functions and processes message loop

****************************************************************************/

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  theInstance = hInstance;
  copyDlgProcInst = (DLGPROC)MakeProcInstance(PROCCAST CopyDlgProc, theInstance);
  
  // Should insert filter flags
  DdeInitialize(&wxIdInst, (PFNCALLBACK)MakeProcInstance(
               (FARPROC)wxDdeCallback, hInstance),
               APPCLASS_STANDARD,
               0L);
#if !USE_OWN_LZH
#if LZH_AVAILABLE
  LZStart();
#endif
#endif

/*
#if USE_OWN_LZH
  // If we have an argument, we want to compress a file.
  if (strlen(lpCmdLine) > 1)
  {
    char outBuf[256];
    strcpy(outBuf, lpCmdLine);
    outBuf[strlen(lpCmdLine) - 1] = '$';
    if (CompressFile(lpCmdLine, outBuf))
      MessageBox(NULL, LConv("Compressed file.","Komprimierte Datei."), LConv("Message","Meldung"), MB_APPLMODAL);
    else
      MessageBox(NULL, LConv("Could not compress file.","Datei nicht komprimierbar"), LConv("Message","Meldung"), MB_APPLMODAL);
    return Quit();
  }
#endif
*/
  if (strcmp(lpCmdLine, "/v") == 0)
  {
    char buf[256];
    sprintf(buf, "Julian Smart's free installation utility\r\nVersion %s (c) 1995", VersionString);
    MessageBox(NULL, buf, "About this program", MB_APPLMODAL|MB_ICONINFORMATION);
    return Quit();
  }

  // Initialize Windows directory
  char buf[256];
  ::GetWindowsDirectory(buf, 256);
  theApplication.SetWindowsDirectory(buf);

  getcwd(buf, 256);
  if (buf[strlen(buf)-1] == '\\')
    buf[strlen(buf)-1] = 0;
    
  theApplication.SetThisDirectory(buf);
  
/*
  MSG msg;

  while (GetMessage(&msg, NULL, NULL, NULL))
  {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
  }
*/

  if (!ReadInf())
    return Quit(FALSE);

  if (!AskUserForFileGroups())
    return Quit(FALSE);

  if (!CheckPrerequisites())
    return Quit(FALSE);

  if (!CreateDestDirectories())
    return Quit(FALSE);

  if (!CopyAllFiles())
    return Quit(FALSE);

  if (!InstallPMIcons())
    return Quit(FALSE);

  if (!QueryRunPrograms())
    return Quit();

  sprintf(buf, LConv("%s was installed succesfully.","%s erfolgreich installiert."), theApplication.GetName());
  MessageBox(NULL, buf, theApplication.GetTitle(), MB_APPLMODAL);

  return Quit();
}

int Quit(Bool ok)
{
  if (!ok)
  {
    char buf[256];
    sprintf(buf, LConv("Sorry, %s was not installed succesfully.","%s konnte leider nicht erfolgreich installiert werden."), theApplication.GetName());
    MessageBox(NULL, buf, LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
  }
  
  FreeProcInstance (copyDlgProcInst);
  DdeUninitialize(wxIdInst);
#if !USE_OWN_LZH
#if LZH_AVAILABLE
  LZDone();
#endif
#endif
  return 0;
}

HDDEDATA EXPENTRY _EXPORT wxDdeCallback(
WORD wType,
WORD wFmt,
HCONV hConv,
HSZ hsz1,
HSZ hsz2,
HDDEDATA hData,
DWORD lData1,
DWORD lData2)
{
  switch (wType)
  {
    case XTYP_CONNECT:
    {
      return 0;
      break;
    }

    case XTYP_CONNECT_CONFIRM:
    {
      return 0;
      break;
    }

    case XTYP_DISCONNECT:
    {
      return 0;
      break;
    }

    case XTYP_EXECUTE:
    {
      return DDE_FNOTPROCESSED;
      break;
    }

    case XTYP_REQUEST:
    {
      return 0;
      break;
    }

    case XTYP_POKE:
    {
      return 0;
      break;
    }

    case XTYP_ADVSTART:
    {
      return 0;
      break;
    }

    case XTYP_ADVSTOP:
    {
      return 0;
      break;
    }

    case XTYP_ADVREQ:
    {
      return 0;
      break;
    }

    case XTYP_ADVDATA:
    {
      return DDE_FNOTPROCESSED;
      break;
    }
  }
  return 0;
}

/*
 * Class implementations
 */
 
Application::Application(void)
{
  title = NULL;
  name = NULL;
  message = NULL;
  directory = NULL;
  windowsDirectory = NULL;
  pmGroup = NULL;
  freeSpace = 0;
}

Application::~Application(void)
{
  if (name) delete[] name;
  if (title) delete[] title;
  if (directory) delete[] directory;
  if (windowsDirectory) delete[] windowsDirectory;
  if (thisDirectory) delete[] thisDirectory;
  if (pmGroup) delete[] pmGroup;
}

void Application::SetName(char *n)
{
  if (name) delete[] name;
  name = copystring(n);
}

void Application::SetMessage(char *m)
{
  if (message) delete[] m;
  message = copystring(m);
}

void Application::SetTitle(char *t)
{
  if (title) delete[] title;
  title = copystring(t);
}

void Application::SetPMGroup(char *g)
{
  if (pmGroup) delete[] pmGroup;
  pmGroup = copystring(g);
}

void Application::SetDirectory(char *d)
{
  if (directory) delete[] directory;
  directory = copystring(d);
}

void Application::SetWindowsDirectory(char *d)
{
  if (windowsDirectory) delete[] windowsDirectory;
  windowsDirectory = copystring(d);
}

void Application::SetThisDirectory(char *d)
{
  if (thisDirectory) delete[] thisDirectory;
  thisDirectory = copystring(d);
}


/*
 * File group
 */

FileGroup::FileGroup(void)
{
  groupName = NULL;
  noEntries = 0;
  installThis = TRUE;
}

FileGroup::~FileGroup(void)
{
  if (groupName) delete[] groupName;
}

void FileGroup::SetGroupName(char *n)
{
  if (groupName) delete[] groupName;
  groupName = copystring(n);
}

void FileGroup::ReadEntry(char *lhs, char *rhs)
{
  SetGroupName(rhs);
  
  char *token;
  const char *IFS = " ,\t";
  int i = 0;
  if ((token = strtok(lhs, IFS)) != NULL) {

    do {
        if (*token != '\0' && strchr(IFS, *token) == NULL)
        {
          switch (i)
          {
            case 0:
            {
              // The file group number
              int num = atoi(token);
              if (num > theNumberOfFileGroups)
                theNumberOfFileGroups = num;
              theFileGroups[num-1] = this;
              break;
            }
            case 1:
            {
              // The optional Y/N for whether we show this by default
              if (strcmp(token, "N") == 0)
                installThis = FALSE;
              break;
            }
            default:
              break;
          }
        }
        i ++;
      } while ((token = strtok(NULL, IFS)) != NULL);
    }
}

/*
 * File group entry
 */
 
FileGroupEntry::FileGroupEntry(void)
{
  srcFilename = NULL;
  destFilename = NULL;
  title = NULL;
  destDir = 1;
  sourceDir = 1;
  disk = 1;
  compressed = FALSE;
  compressionMethod = NULL;
}

FileGroupEntry::~FileGroupEntry(void)
{
  if (srcFilename) delete[] srcFilename;
  if (destFilename) delete[] destFilename;
  if (title) delete[] title;
  if (compressionMethod) delete[] compressionMethod;
}

void FileGroupEntry::SetSrcFilename(char *f)
{
  if (srcFilename) delete[] srcFilename;
  srcFilename = copystring(f);
}

void FileGroupEntry::SetDestFilename(char *f)
{
  if (destFilename) delete[] destFilename;
  destFilename = copystring(f);
}

void FileGroupEntry::SetTitle(char *f)
{
  if (title) delete[] title;
  title = copystring(f);
}

void FileGroupEntry::ReadEntry(char *value, Bool ignoreDestCheck)
{
  char *token;
  const char *IFS = " \t\r\n";
  int i = 0;
  if ((token = strtok(value, IFS)) != NULL) {

    do {
        if (*token != '\0' && strchr(IFS, *token) == NULL)
        {
          switch (i)
          {
            case 0:
            {
              SetSrcFilename(token);
              break;
            }
            case 1:
            {
              SetDestFilename(token);
              break;
            }
            case 2:
            {
              sourceDir = atoi(token);
              if ((sourceDir < 1) || (sourceDir > theNumberOfSourceDirectories))
              {
                char buf[256];
                sprintf(buf, LConv("Source directory of %s should be between 1 and %d","Quellverzeichnis von %s sollte zwischen 1 und %d liegen"), srcFilename, theNumberOfSourceDirectories);
                MessageBox(NULL, buf, LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
                return;
              }
              break;
            }
            case 3:
            {
              destDir = atoi(token);
              if (!ignoreDestCheck && ((destDir < 1) || (destDir > theNumberOfDestDirectories)))
              {
                char buf[256];
                sprintf(buf, LConv("Destination directory of %s should be between 1 and %d","Zielverzeichnis von %s sollte zwischen 1 und %d liegen"), srcFilename, theNumberOfDestDirectories);
                MessageBox(NULL, buf, LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
                return;
              }
              break;
            }
            case 4:
            {
              disk = atoi(token);
              if ((disk < 1) || (disk > theNumberOfDisks))
              {
                char buf[256];
                sprintf(buf, LConv("Disk number of %s should be between 1 and %d","Die Nummer der Diskette %s sollte zwischen 1 und %d liegen"), srcFilename, theNumberOfDisks);
                MessageBox(NULL, buf, LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
                return;
              }
              break;
            }
            case 5:
            {
              compressed = (strcmp(token, "N") != 0);
              compressionMethod = copystring(token);
              break;
            }
            default:
              break;
          }
        }
        i ++;
      } while ((token = strtok(NULL, IFS)) != NULL);
    }
}

/*
 * Execution entry
 */
 
ExecutionEntry::ExecutionEntry(void)
{
  command = NULL;
  message = NULL;
  waitForTerm = FALSE;
  askYN = FALSE;
}

ExecutionEntry::~ExecutionEntry(void)
{
  if (command) delete[] command;
  if (message) delete[] message;
}

void ExecutionEntry::SetCommand(char *s)
{
  if (command) delete[] command;
  command = copystring(s);
}

void ExecutionEntry::ReadEntry(char *lhs, char *rhs)
{
  char *ptr = lhs;
  if (ptr[0] == 'W')
  {
    waitForTerm = TRUE;
    ptr = ptr + 1;
  }
  if (ptr[0] == '?')
  {
    askYN = TRUE;
    ptr = ptr + 1;
  }
  if (ptr[0] == 39)
  {
    ptr = ptr + 1;
    int i = 0;
    int j = 0;
    char msgBuf[1000];
    while (ptr[i] != 0 && ptr[i] != 39)
    {
      if (ptr[i] == '\\')
      {
         if (ptr[i+1] == 'n')       {i++; msgBuf[j++] = '\n';}
         else if (ptr[i+1] == 'r')  {i++; msgBuf[j++] = '\r';}
         else if (ptr[i+1] == 't')  {i++; msgBuf[j++] = '\t';}
         else if (ptr[i+1] == '-')  {i++; msgBuf[j++] = '=' ;}
         else if (ptr[i+1] == '\\') {i++; msgBuf[j++] = '\\';}
      }
      else
      {
         msgBuf[j++] = ptr[i];
      }
      i ++;
    }
    msgBuf[j] = 0;
    message = copystring(msgBuf);
  }
  if (rhs)
    command = copystring(rhs);
  else
    command = NULL;    
}

/*
 * Prerequisite entry
 */

PrerequisiteEntry::PrerequisiteEntry(void)
{
  file = NULL;
  message = NULL;
  abort = FALSE;
  askForContinue = FALSE;
}

PrerequisiteEntry::~PrerequisiteEntry(void)
{
  if (file) delete[] file;
  if (message) delete[] message;
}

void PrerequisiteEntry::SetFile(char *s)
{
  if (file) delete[] file;
  file = copystring(s);
}

void PrerequisiteEntry::ReadEntry(char *lhs, char *rhs)
{
  char *ptr = lhs;
  if (ptr[0] == 'A')
  {
	 abort = TRUE;
	 ptr = ptr + 1;
  }
  if (ptr[0] == '?')
  {
	 askForContinue = TRUE;
	 ptr = ptr + 1;
  }
  if (ptr[0] == 39)
  {
	 ptr = ptr + 1;
	 int i = 0;
	 int j = 0;
	 char msgBuf[1000];
	 while (ptr[i] != 0 && ptr[i] != 39)
	 {
		if (ptr[i] == '\\')
		{
			if (ptr[i+1] == 'n')       {i++; msgBuf[j++] = '\n';}
			else if (ptr[i+1] == 'r')  {i++; msgBuf[j++] = '\r';}
			else if (ptr[i+1] == 't')  {i++; msgBuf[j++] = '\t';}
			else if (ptr[i+1] == '-')  {i++; msgBuf[j++] = '=' ;}
			else if (ptr[i+1] == '\\') {i++; msgBuf[j++] = '\\';}
		}
		else
		{
			msgBuf[j++] = ptr[i];
		}
		i ++;
	 }
	 msgBuf[j] = 0;
	 message = copystring(msgBuf);
  }
  if (rhs)
	 file = copystring(rhs);
  else
	 file = NULL;	//useless, but ...
}

/*
 * Program manager group entry
 */

PMGroupEntry::PMGroupEntry(void)
{
  title = NULL;
  command = NULL;
  program = NULL;
  iconNo = 0;
}

PMGroupEntry::~PMGroupEntry(void)
{
  if (title) delete[] title;
  if (command) delete[] command;
  if (program) delete[] program;
}

void PMGroupEntry::SetTitle(char *t)
{
  if (title) delete[] title;
  title = copystring(t);
}

void PMGroupEntry::SetCommand(char *s)
{
  if (command) delete[] command;
  command = copystring(s);
}

void PMGroupEntry::ReadEntry(char *lhs, char *value)
{
  char *token;
  const char *IFS = ",";
  int i = 0;
  if ((token = strtok(lhs, IFS)) != NULL) {

    do {
        if (*token != '\0' && strchr(IFS, *token) == NULL)
        {
          switch (i)
          {
            case 0:
            {
              SetTitle(token);
              break;
            }
            case 1:
            {
              program = copystring(token);
              break;
            }
            case 2:
            {
              iconNo = atoi(token);
              break;
            }
            default:
              break;
          }
        }
        i ++;
      } while ((token = strtok(NULL, IFS)) != NULL);
    }
  
  if (value)
    SetCommand(value);
  else
    command = NULL;
}

/*
 * Compression method
 */

CompressionMethod::CompressionMethod(void)
{
  name = NULL;
  command = NULL;
}

CompressionMethod::~CompressionMethod(void)
{
  if (name) delete[] name;
  if (command) delete[] command;
}

char *CompressionMethod::GetInstantiatedCommand(char *srcDir, char *destDir, char *srcFile, char *destFile)
{
  if (!command)
    return NULL;

  return SubstituteDirectories(command, srcDir, destDir, srcFile, destFile);
}

char *GetCompressionCommand(char *method, char *srcDir, char *destDir, char *srcFile, char *destFile)
{
  for (int i = 0; i < theNumberOfCompressors; i++)
  {
    if (strcmp(theCompressors[i]->name, method) == 0)
      return theCompressors[i]->GetInstantiatedCommand(srcDir, destDir, srcFile, destFile);
  }
  return NULL;
}

/*
 * Disks, directories
 */
 
void SetDiskName(int i, char *name)
{
  theDisks[i] = copystring(name);
}

void SetDestDirectoryName(int i, char *name)
{
  theDestDirectories[i] = copystring(name);
}

void SetSourceDirectoryName(int i, char *name)
{
  theSourceDirectories[i] = copystring(name);
}


/*
 * Read the install.inf file
 */

Bool ReadInf(void)
{
  // Read application information
  char *appName = NULL;
  char *message = NULL;
  char *title = NULL;
  char *dirName = NULL;
  char *pmGroup = NULL;
  char *languageString = NULL; // steve
  int freeSpace = 1000;
  char entryBuf[2000];

  if (!wxGetResource("Application", "Name", &appName, ".\\install.inf"))
  {
    MessageBox(NULL, "Could not find install.inf file.", LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
    return FALSE;
  }

  //++steve
  language = L_english; // by default
  if(wxGetResource("Application", "Language", &languageString, ".\\install.inf")){
    if(!strcmp(languageString,"english")){
      language = L_english;
    }else if(!strcmp(languageString,"german")){
      language = L_german;
    }else{
      MessageBox(NULL, LConv("Undefined language in INF file.","Undefinierte Sprache in der INF Datei"), 
                       LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
      return FALSE;
    }
  }
  //--steve
  wxGetResource("Application", "Title", &title, ".\\install.inf");
  wxGetResource("Application", "Directory", &dirName, ".\\install.inf");
  wxGetResource("Application", "PM Group", &pmGroup, ".\\install.inf");
  wxGetResource("Application", "Free Space", &freeSpace, ".\\install.inf");
  wxGetResource("Application", "Message", &message, ".\\install.inf");

  if (!appName || !title)
  {
    MessageBox(NULL, LConv("Missing application name or title in INF file.","Fehlender Anwendungsname oder Titel in der INF Datei."), LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
    return FALSE;
  }

  theApplication.SetName(appName);
  theApplication.SetTitle(title);
  if (message && strlen(message) > 0)
    theApplication.SetMessage(message);
  theApplication.SetPMGroup(pmGroup);
  theApplication.SetDirectory(dirName);
  theApplication.SetFreeSpace(freeSpace);

  // Read disk information
  int i = 0;
  for (i = 0; i < 30; i++)
  {
    char buf[10];
    char *diskName = NULL;
    sprintf(buf, "%d", i+1);
    if (!wxGetResource("Disks", buf, &diskName, ".\\install.inf"))
    {
      break;
    }
    else
    {
      theDisks[i] = diskName;
    }
  }
  theNumberOfDisks = i;
  
  // Read source directory names
  for (i = 0; i < 30; i++)
  {
    char buf[10];
    char *dirName = NULL;
    sprintf(buf, "%d", i+1);
    if (!wxGetResource("SourceDirs", buf, &dirName, ".\\install.inf"))
    {
      break;
    }
    else
    {
      SetSourceDirectoryName(i, dirName);
      delete[] dirName;
    }
  }
  theNumberOfSourceDirectories = i;

  // Read destination directory names
  for (i = 0; i < 30; i++)
  {
    char buf[10];
    char *dirName = NULL;
    sprintf(buf, "%d", i+1);
    if (!wxGetResource("Directories", buf, &dirName, ".\\install.inf"))
    {
      break;
    }
    else
    {
      char *s = SubstituteDirectories(dirName);
      SetDestDirectoryName(i, s);
      delete[] dirName;
    }
  }
  theNumberOfDestDirectories = i;

  // Read file group names
  int n = GetPrivateProfileString("FileGroups", NULL, "", entryBuf, 1000, ".\\install.inf");
  if (n > 0)
  {
    char *key;
    for (key = entryBuf; *key != '\0'; key += strlen(key) + 1)
    {
      char *entryValue = NULL;
      wxGetResource("FileGroups", key, &entryValue, ".\\install.inf");
      if (entryValue)
      {
        FileGroup *group = new FileGroup;
        group->ReadEntry(key, entryValue);
      }
    }
  }

/*
  for (i = 0; i < 30; i++)
  {
    char buf[10];
    char *groupName = NULL;
    sprintf(buf, "%d", i+1);
    if (!wxGetResource("FileGroups", buf, &groupName, ".\\install.inf"))
    {
      break;
    }
    else
    {
      FileGroup *group = new FileGroup;
      theFileGroups[i] = group;
      group->SetGroupName(groupName);
      delete[] groupName;
    }
  }
  theNumberOfFileGroups = i;
*/

  // Read Windows file group

  n = GetPrivateProfileString("Windows Files", NULL, "", entryBuf, 1000, ".\\install.inf");
  if (n > 0)
  {
    char *key;
    for (key = entryBuf; *key != '\0'; key += strlen(key) + 1)
    {
      char *entryValue = NULL;
      wxGetResource("Windows Files", key, &entryValue, ".\\install.inf");
      if (entryValue)
      {
        theWindowsFileGroup.entries[theWindowsFileGroup.noEntries] = new FileGroupEntry;
        theWindowsFileGroup.entries[theWindowsFileGroup.noEntries]->SetTitle(key);
        theWindowsFileGroup.entries[theWindowsFileGroup.noEntries]->ReadEntry(entryValue, TRUE);

        theWindowsFileGroup.noEntries ++;
      }
    }
  }

  // Read each FilesN group
  for (i = 0; i < 30; i++)
  {
    char buf[10];
    char *groupName = NULL;
    sprintf(buf, "Files%d", i+1);

    entryBuf[0] = 0;
    int n = GetPrivateProfileString(buf, NULL, "", entryBuf, 1000, ".\\install.inf");
    if (n == 0 || (strlen(entryBuf) == 0) || (i >= theNumberOfFileGroups))
    {
      break;
    }
    else
    {
      if (theNumberOfFileGroups < (i+1))
      {
        MessageBox(NULL, LConv("There are more file groups than specified in FileGroups!","Mehr Dateigruppen als im Abschnitt FileGroups angegeben"),
                         LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
        return FALSE;
      }
      FileGroup *group = theFileGroups[i];
      
      char *key;
      for (key = entryBuf; *key != '\0'; key += strlen(key) + 1)
      {
        char *entryValue = NULL;
        wxGetResource(buf, key, &entryValue, ".\\install.inf");
        if (entryValue)
        {
//          wxDebugMsg("%s\n", entryValue);
          group->entries[group->noEntries] = new FileGroupEntry;
          group->entries[group->noEntries]->SetTitle(key);
          group->entries[group->noEntries]->ReadEntry(entryValue, FALSE);
//          wxDebugMsg("Entry no. %d, %s: source directory = %d, disk = %d, dest dir = %d\n", group->noEntries, group->entries[group->noEntries]->srcFilename, group->entries[group->noEntries]->sourceDir,
//             group->entries[group->noEntries]->disk, group->entries[group->noEntries]->destDir);

          group->noEntries ++;
        }
      }
    }
  }
  // Read PM Group entries

  n = GetPrivateProfileString("PM Group", NULL, "", entryBuf, 2000, ".\\install.inf");
  if (n > 0)
  {
    char *key;
    theNumberOfPMEntries = 0;
    for (key = entryBuf; *key != '\0'; key += strlen(key) + 1)
    {
      char *entryValue = NULL;
      wxGetResource("PM Group", key, &entryValue, ".\\install.inf");
      if (entryValue)
      {
        thePMEntries[theNumberOfPMEntries] = new PMGroupEntry;
        thePMEntries[theNumberOfPMEntries]->ReadEntry(key, entryValue);
        theNumberOfPMEntries ++;
      }
    }
  }

  // Read execution section
  n = GetPrivateProfileString("Execution", NULL, "", entryBuf, 1000, ".\\install.inf");
  if (n > 0)
  {
    i = 0;
    char *key;
    for (key = entryBuf; *key != '\0'; key += strlen(key) + 1)
    {
      char *entryValue = NULL;
      wxGetResource("Execution", key, &entryValue, ".\\install.inf");
      if (entryValue)
      {
        theExecutionEntries[theNumberOfExecutionEntries] = new ExecutionEntry;
        theExecutionEntries[theNumberOfExecutionEntries]->ReadEntry(key, entryValue);
        theNumberOfExecutionEntries ++;
      }
    }
  }

  // Read prerequisites section
  n = GetPrivateProfileString("Prerequisites", NULL, "", entryBuf, 1000, ".\\install.inf");
  if (n > 0)
  {
	 i = 0;
	 char *key;
	 for (key = entryBuf; *key != '\0'; key += strlen(key) + 1)
	 {
		char *entryValue = NULL;
		wxGetResource("Prerequisites", key, &entryValue, ".\\install.inf");
		if (entryValue)
		{
  		  thePrerequisiteEntries[theNumberOfPrerequisiteEntries] = new PrerequisiteEntry;
		  thePrerequisiteEntries[theNumberOfPrerequisiteEntries]->ReadEntry(key, entryValue);
		  theNumberOfPrerequisiteEntries ++;
		}
	 }
  }

  // Read compression method entries

  n = GetPrivateProfileString("Decompressors", NULL, "", entryBuf, 1000, ".\\install.inf");
  if (n > 0)
  {
    char *key;
    theNumberOfCompressors = 0;
    for (key = entryBuf; *key != '\0'; key += strlen(key) + 1)
    {
      theCompressors[theNumberOfCompressors] = new CompressionMethod;
      char *entryValue = NULL;
      
      wxGetResource("Decompressors", key, &entryValue, ".\\install.inf");
      if (entryValue)
      {
        theCompressors[theNumberOfCompressors]->name = copystring(key);
        theCompressors[theNumberOfCompressors]->command = entryValue;
        theNumberOfCompressors ++;
      }
    }
  }

  return TRUE;
}

/*
 * Compress/uncompress
 */

Bool CompressFile(char *in, char *out)
{
  FILE *fd = fopen(in, "r");
  if (!fd)
  {
    char buf[300];
    sprintf(buf, LConv("Could not find file %s","Die Datei %s existiert nicht"), in);
    MessageBox(NULL, buf, LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
    return FALSE;
  }
  fclose(fd);

#if USE_OWN_LZH
  ifstream istr(in);
  ofstream ostr(out);
  int success = lzw_compress(istr, ostr);
  return success;
#else
  return FALSE;
#endif
}

Bool ExpandFile(char *in, char *out, Bool isCompressed)
{
  FILE *fd = fopen(in, "r");
  if (!fd)
  {
    char buf[300];
    sprintf(buf, LConv("Could not find file on source media: %s","Die Datei %s existiert nicht auf dem Quellmedium"), in);
    MessageBox(NULL, buf, LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
    return FALSE;
  }
  fclose(fd);

#if USE_OWN_LZH
  if (!isCompressed)
    return wxCopyFile(in, out);
  else
  {
    ifstream istr(in);
    ofstream ostr(out);
    int success = lzw_expand(istr, ostr);
    return success;
  }
#else
#ifdef GNUWIN32
  MessageBox(NULL, "LZW compression not available in this configuration of install.exe", "Installation Error", MB_APPLMODAL|MB_ICONEXCLAMATION);
  return FALSE;
#else
  OFSTRUCT ofStrSrc;
  OFSTRUCT ofStrDest;
  HFILE hfSrcFile = LZOpenFile(in, &ofStrSrc, OF_READ);
  HFILE hfDestFile = LZOpenFile(out, &ofStrDest, OF_CREATE);

  CopyLZFile(hfSrcFile, hfDestFile);

  LZClose(hfSrcFile);
  LZClose(hfDestFile);
  return TRUE;
#endif
#endif
}


/*
 * Steps in installation
 */
 
Bool CreateDestDirectories(void)
{
  if (!theApplication.GetDirectory())
    return FALSE;
    
  if (!wxDirExists(theApplication.GetDirectory()))
  {
    if (!wxMkdir(theApplication.GetDirectory()))
    {
      MessageBox(NULL, LConv("Could not create destination directory.","Zielverzeichnis kann nicht angelegt werden."),
                       LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
      return FALSE;
    }
  }

  for (int i = 0; i < theNumberOfDestDirectories; i++)
  {
    if (strcmp(theDestDirectories[i], ".") != 0)
    {
      char buf[256];
      char *d = theDestDirectories[i];
      if (d[0] == '.' && d[1] == '\\')
        d = d + 2;
        
      strcpy(buf, theApplication.GetDirectory());
      strcat(buf, "\\");
      strcat(buf, d);
      if (!wxDirExists(buf))
        wxMkdir(buf);
    }
  }
  
  return TRUE;
}

Bool DoCopyFiles(void)
{
  //+++steve
  if(!EnoughSpace()){
    MessageBox(NULL,
      LConv("Not enough free space on destination drive!","Zuwenig Platz auf dem Ziellaufwerk!"),
      LConv("Error","Fehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
    return FALSE;
  }
  //---steve
  // First work out how many files will be copied, for purposes of
  // calculating percentage done.
  int noFiles = 0;

  // Windows files
  int i;
  for (i = 0; i < theWindowsFileGroup.noEntries; i++)
  {
    FileGroupEntry *entry = theWindowsFileGroup.entries[i];
    if (entry && entry->GetTitle() && entry->GetSrcFilename() && entry->GetDestFilename())
    {
      noFiles ++;
    }
  }

  // File groups
  for (i = 0; i < theNumberOfFileGroups; i++)
  {
    FileGroup *group = theFileGroups[i];
    if (group->installThis)
    {
      for (int j = 0; j < group->noEntries; j++)
      {
        FileGroupEntry *entry = group->entries[j];
        if (entry && entry->GetTitle() && entry->GetSrcFilename() && entry->GetDestFilename())
        {
          noFiles ++;
        }
      }
    }
  }

  int whichFile = 1;

  // Do each disk one at a time.
  int diskNo;
  for (diskNo = 1; diskNo <= theNumberOfDisks; diskNo++)
  {
    // Now do actual copying
    // Do Windows files
    for (i = 0; i < theWindowsFileGroup.noEntries; i++)
    {
      FileGroupEntry *entry = theWindowsFileGroup.entries[i];
      if (entry && entry->GetTitle() && entry->GetSrcFilename() && entry->GetDestFilename() && (entry->disk == diskNo))
      {
        char buf1[256];
        char buf2[256];
        char theSrcDir[256];
        char theDestDir[256];

//        sprintf(buf1, ".\\%s", entry->GetSrcFilename());
        if (strcmp(theSourceDirectories[entry->sourceDir-1], ".") == 0)
          sprintf(buf1, "%s\\%s", theApplication.GetThisDirectory(), entry->GetSrcFilename());
        else
          sprintf(buf1, "%s\\%s", theSourceDirectories[entry->sourceDir-1], entry->GetSrcFilename());
        sprintf(buf2, "%s\\system\\%s", theApplication.MyGetWindowsDirectory(), entry->GetDestFilename());

        if (strcmp(theSourceDirectories[entry->sourceDir-1], ".") == 0)
          sprintf(theSrcDir, "%s", theApplication.GetThisDirectory());
        else
          sprintf(theSrcDir, "%s", theSourceDirectories[entry->sourceDir-1]);
        sprintf(theDestDir, "%s\\system", theApplication.MyGetWindowsDirectory());

        SetCopyText(entry->GetSrcFilename(), buf2, (int)(100.0*(whichFile/(float)noFiles)));

        while (!wxFileExists(buf1))
        {
          char buf3[256];
          sprintf(buf3, LConv("Please insert disk %d: %s","Bitte Diskette %d einlegen: %s"), diskNo, theDisks[diskNo-1]);
          int ans = MessageBox(NULL, buf3, theApplication.GetTitle(), MB_APPLMODAL|MB_ICONSTOP|MB_OKCANCEL);
          if (ans == IDCANCEL)
            return FALSE;
          wxYield();
        }

        //+++steve: patch picked up from mailing list (Marcel Hendrickx)
        if(wxFileExists(buf2)){
          char buf3[256];
          sprintf(buf3,LConv("The file %s already exists, overwrite?","Die Datei %s existiert bereits. Soll sie ersetzt werden?"),
                       entry->GetDestFilename());
          int ans = MessageBox(NULL,buf3,theApplication.GetTitle(),
                               MB_APPLMODAL|MB_ICONQUESTION|MB_OKCANCEL);
          if(ans == IDOK){
            // do normal copying
            if (!ExpandFileChooseMethod(buf1, buf2, entry->compressionMethod, entry->compressed,
                  theSrcDir, theDestDir, entry->GetSrcFilename(), entry->GetDestFilename()))
              return FALSE;
          }
          wxYield(); // necessary?
        }else{
          // this was the original code
        if (!ExpandFileChooseMethod(buf1, buf2, entry->compressionMethod, entry->compressed,
              theSrcDir, theDestDir, entry->GetSrcFilename(), entry->GetDestFilename()))
          return FALSE;
        }
        //---steve

        whichFile ++;
      }
    }

    // Do all file groups
    for (i = 0; i < theNumberOfFileGroups; i++)
    {
      FileGroup *group = theFileGroups[i];
      if (group->installThis)
      {
        for (int j = 0; j < group->noEntries; j++)
        {
          FileGroupEntry *entry = group->entries[j];
          if (entry && entry->GetTitle() && entry->GetSrcFilename() && entry->GetDestFilename() && (entry->disk == diskNo))
          {
            char buf1[256];
            char buf2[256];

            char theSrcDir[256];
            char theDestDir[256];

//            sprintf(buf1, "%s\\%s", theSourceDirectories[entry->sourceDir-1], entry->GetSrcFilename());
            if (strcmp(theSourceDirectories[entry->sourceDir-1], ".") == 0)
              sprintf(buf1, "%s\\%s", theApplication.GetThisDirectory(), entry->GetSrcFilename());
            else
              sprintf(buf1, "%s\\%s", theSourceDirectories[entry->sourceDir-1], entry->GetSrcFilename());

            if (strcmp(theDestDirectories[entry->destDir-1], ".") == 0)
              sprintf(buf2, "%s\\%s", theApplication.GetDirectory(), entry->GetDestFilename());
            else if (theDestDirectories[entry->destDir-1][0] == '.' && theDestDirectories[entry->destDir-1][1] == '\\')
              // it's relative to the main installation directory: cut off the dot and backslash
              sprintf(buf2, "%s\\%s\\%s", theApplication.GetDirectory(), (theDestDirectories[entry->destDir-1] + 2), entry->GetDestFilename());
            else
              // it's an absolute pathname
              sprintf(buf2, "%s\\%s", theDestDirectories[entry->destDir-1], entry->GetDestFilename());

            if (strcmp(theSourceDirectories[entry->sourceDir-1], ".") == 0)
              sprintf(theSrcDir, "%s", theApplication.GetThisDirectory());
            else
              sprintf(theSrcDir, "%s", theSourceDirectories[entry->sourceDir-1]);

            if (strcmp(theDestDirectories[entry->destDir-1], ".") == 0)
              sprintf(theDestDir, "%s", theApplication.GetDirectory());
            else if (theDestDirectories[entry->destDir-1][0] == '.' && theDestDirectories[entry->destDir-1][1] == '\\')
              // it's relative to the main installation directory: cut off the dot and backslash
              sprintf(theDestDir, "%s\\%s", theApplication.GetDirectory(), (theDestDirectories[entry->destDir-1] + 2));
            else
              // it's an absolute pathname
              sprintf(theDestDir, "%s", theDestDirectories[entry->destDir-1]);

            SetCopyText(entry->GetSrcFilename(), buf2, (int)(100.0*(whichFile/(float)noFiles)));
          
            while (!wxFileExists(buf1))
            {
              char buf3[256];
              sprintf(buf3, LConv("Please insert disk %d: %s","Bitte Diskette %d einlegen: %s"), diskNo, theDisks[diskNo-1]);
              int ans = MessageBox(NULL, buf3, theApplication.GetTitle(), MB_APPLMODAL|MB_ICONSTOP|MB_OKCANCEL);
              if (ans == IDCANCEL)
                return FALSE;
              wxYield();
            }
        
            if (!ExpandFileChooseMethod(buf1, buf2, entry->compressionMethod, entry->compressed,
                  theSrcDir, theDestDir, entry->GetSrcFilename(), entry->GetDestFilename()))
              return FALSE;
            whichFile ++;
          }
        }
      }
    }
  }
  return TRUE;
}

Bool CopyAllFiles(void)
{
  copyDlgWindow = ::CreateDialog(theInstance, MAKEINTRESOURCE(RConv(IDD_E_DIALOG2,IDD_G_DIALOG2)), NULL,
                            (DLGPROC)copyDlgProcInst);
  ShowWindow(copyDlgWindow, SW_SHOW);
  wxYield();

  Bool success = DoCopyFiles();

  ShowWindow(copyDlgWindow, SW_HIDE);
  DestroyWindow(copyDlgWindow);
  copyDlgWindow = 0;

  wxYield();

  return success;
}

Bool InstallPMIcons(void)
{
  if (theApplication.GetPMGroup())
  {
    sprintf(wxBuffer, "[CreateGroup(%s)]", theApplication.GetPMGroup());
    ExecuteCommand("PROGMAN", "PROGMAN", wxBuffer);

    for (int i = 0; i < theNumberOfPMEntries; i++)
    {
      PMGroupEntry *entry = thePMEntries[i];
      if (entry->GetTitle() && entry->GetCommand())
      {
        sprintf(wxBuffer, "[DeleteItem(%s)]", entry->GetTitle());
        ExecuteCommand("PROGMAN", "PROGMAN", wxBuffer);
        if (entry->program)
          sprintf(wxBuffer, "[AddItem(%s, %s, %s, %d)]", entry->GetCommand(), entry->GetTitle(),
            entry->program, entry->iconNo);
        else
          sprintf(wxBuffer, "[AddItem(%s, %s)]", entry->GetCommand(), entry->GetTitle());
        ExecuteCommand("PROGMAN", "PROGMAN", wxBuffer);
      }
    }
  }
  return TRUE;
}

// Which groups to install, and destination directory.
Bool AskUserForFileGroups(void)
{
  DLGPROC lpfn = (DLGPROC)MakeProcInstance(PROCCAST InstallDlgProc, theInstance);
  DialogBox (theInstance, MAKEINTRESOURCE(RConv(IDD_E_DIALOG1,IDD_G_DIALOG1)), NULL, lpfn);
  FreeProcInstance (lpfn);
  
  return dialogOK;
}

Bool QueryRunPrograms(void)
{
  chdir(theApplication.GetDirectory());
  for (int i = 0; i < theNumberOfExecutionEntries; i++)
  {
    ExecutionEntry *entry = theExecutionEntries[i];
    if (entry)
    {
      Bool doIt = TRUE;
      if (entry->askYN)
      {
        int ans = MessageBox(NULL, entry->message, theApplication.GetTitle(), MB_APPLMODAL|MB_ICONQUESTION|MB_YESNO);
        if (ans == IDNO)
          doIt = FALSE;
      }
      else if (entry->message)
      {
        MessageBox(NULL, entry->message, theApplication.GetTitle(), MB_APPLMODAL|MB_ICONINFORMATION);
      }
      if (doIt && entry->command)
      {
        char *d = theApplication.GetDirectory();
        wxSetWorkingDirectory(d);
//+++steve165(10.01.96): added built in command
        // Check for built in commands
        char *cmd = entry->command;
        if(!strncmp(cmd,"del",3)){ // del doesn't work as an external command
          // delete a file
          cmd+=3; // point beyond 'del'
          while(*cmd && *cmd==' ') cmd++; // go over spaces
          remove(cmd); // do the deletion
        }else{
        wxExecute(entry->command, entry->waitForTerm);
        }
//---steve165        
      }
    }
  }
  return TRUE;
}

Bool CheckPrerequisites(void)
{
  wxSetWorkingDirectory(theApplication.GetDirectory());
  for (int i = 0; i < theNumberOfPrerequisiteEntries; i++)
  {
	 PrerequisiteEntry *entry = thePrerequisiteEntries[i];
	 if (entry)
	 {
		Bool abort = FALSE;

		//check for file
		char* file = entry->file;
		if (file && !wxFileExists(file))
		{
			abort = TRUE;
			if (entry->askForContinue)
			{
			  int ans = MessageBox(NULL, entry->message, theApplication.GetTitle(), MB_APPLMODAL|MB_ICONQUESTION|MB_YESNO);
			  if (ans == IDYES)
			  abort = FALSE;
			}
			else if (entry->message)
			{
				MessageBox(NULL, entry->message, theApplication.GetTitle(), MB_APPLMODAL|MB_ICONINFORMATION);
			}
		}
		if (abort)
			return FALSE;
	 }
  }
  return TRUE;
}

void SetCopyText(char *from, char *to, int percent)
{
  sprintf(wxBuffer, LConv("Source File:\r\n%s\r\n\r\nDestination File:\r\n%s","Quelldatei:\r\n%s\r\n\r\nZieldatei:\r\n%s"), from, to);
  SendDlgItemMessage(copyDlgWindow, RConv(IDC_E_STATICOPY,IDC_G_STATICOPY), WM_SETTEXT, 0, (LPARAM)wxBuffer);

  char buf[50];
  sprintf(buf, "%d %%", percent);
  SendDlgItemMessage(copyDlgWindow, RConv(IDC_E_STATICPERCENT,IDC_G_STATICPERCENT), WM_SETTEXT, 0, (LPARAM)buf);
  wxYield();
}

/****************************************************************************
 *									    *
 *  FUNCTION   : InstallDlgProc ( hwnd, msg, wParam, lParam )		    *
 *									    *
 *  PURPOSE    : Dialog function for the main installation dialog.   	    *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL _EXPORT InstallDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg){
	case WM_INITDIALOG:
	{
	    // Set statics, checkboxes etc.
	    if (theApplication.GetTitle())
              SetWindowText(hwnd, (LPCSTR)theApplication.GetTitle());

            char buf[400];
            if (theApplication.GetMessage())
              strcpy(buf, ExpandNewLines(theApplication.GetMessage()));
            else
          sprintf(buf, LConv("This procedure installs %s onto your hard disk.\r\n\r\nIt will take up about %d KB.",
                          "Dieses Programm installiert %s auf Ihre Harddisk.\r\n\r\nEs sind ca. %d KB freier Speicher notwendig."),
                  theApplication.GetName(), theApplication.MyGetFreeSpace());

            SendDlgItemMessage(hwnd, RConv(IDC_E_STATIC,IDC_G_STATIC), WM_SETTEXT, 0, (LPARAM)buf);

            SendDlgItemMessage(hwnd, RConv(IDC_E_EDIT1,IDC_G_EDIT1), WM_SETTEXT, 0, (LPARAM)theApplication.GetDirectory());
            for (int i = 0; i < 5; i++)
            {
              int id = RConv(IDC_E_CHECK1,IDC_G_CHECK1) + i;
              if ((i < theNumberOfFileGroups) && (theNumberOfFileGroups > 1))
              {
                SendDlgItemMessage(hwnd, id, WM_SETTEXT, 0, (LPARAM)theFileGroups[i]->GetGroupName());
                if (theFileGroups[i]->installThis)
                  CheckDlgButton(hwnd, id, 1);
                else
                  CheckDlgButton(hwnd, id, 0);
              }
              else
              {
                HWND dlgWnd = GetDlgItem(hwnd, id);
                ShowWindow(dlgWnd, SW_HIDE);
              }
            }
            // Centre the dialog
            HDC dc = ::GetDC(NULL);
            int screenWidth = GetDeviceCaps(dc, HORZRES);
            int screenHeight = GetDeviceCaps(dc, VERTRES);
            ReleaseDC(NULL, dc);

            RECT rect;
            GetWindowRect(hwnd, &rect);
            int dialogWidth = rect.right - rect.left;
            int dialogHeight = rect.bottom - rect.top;

            int x = (int)((screenWidth - dialogWidth)/2.0);
            int y = (int)((screenHeight - dialogHeight)/2.0);

            MoveWindow(hwnd, x, y, dialogWidth, dialogHeight, FALSE);

	    break;
	}
	case WM_COMMAND:
	    switch (wParam){
		case IDOK:
		{
		    char buf[256];
        HWND dlgItem = GetDlgItem(hwnd, RConv(IDC_E_EDIT1,IDC_G_EDIT1));
		    GetWindowText(dlgItem, buf, 256);

		    if (!CheckDestinationOk(buf))
		    {
                      MessageBox(NULL,
         LConv("Destination directory must not be the same as the media source directory!\r\nPlease choose a different directory.",
            "Das Zielverzeichnis darf nicht das Quellverzeichnis sein!\r\nBitte selektieren Sie ein anderes Zielverzeichnis."),
                      LConv("Error","Fehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
                      return TRUE;
		    }

		    theApplication.SetDirectory(buf);

		    DoSubstituteDirectories();
		    
		    dialogOK = TRUE;
		    EndDialog(hwnd, 0);
		    if (theNumberOfFileGroups > 1)
		    {
  		      for (int i = 0; i < theNumberOfFileGroups; i++)
		      {
                        int id = RConv(IDC_E_CHECK1,IDC_G_CHECK1) + i;
		        char buf[256];
		        theFileGroups[i]->installThis = IsDlgButtonChecked(hwnd, id);
		      }
		    }
		    break;
		}
		case IDCANCEL:
		    dialogOK = FALSE;
		    EndDialog(hwnd, 0);
		    break;

		default:
		    return FALSE;
	    }
	    break;

	default:
	    return FALSE;
    }

    return TRUE;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : CopyDlgProc ( hwnd, msg, wParam, lParam )		    *
 *									    *
 *  PURPOSE    : Dialog function for the copying dialog.       	            *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL _EXPORT CopyDlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch (msg){
	case WM_INITDIALOG:
	{
            // Centre the dialog
            HDC dc = ::GetDC(NULL);
            int screenWidth = GetDeviceCaps(dc, HORZRES);
            int screenHeight = GetDeviceCaps(dc, VERTRES);
            ReleaseDC(NULL, dc);

            RECT rect;
            GetWindowRect(hwnd, &rect);
            int dialogWidth = rect.right - rect.left;
            int dialogHeight = rect.bottom - rect.top;

            int x = (int)((screenWidth - dialogWidth)/2.0);
            int y = (int)((screenHeight - dialogHeight)/2.0);

            MoveWindow(hwnd, x, y, dialogWidth, dialogHeight, FALSE);

	    break;
	}
	case WM_COMMAND:
	    switch (wParam){
		case IDOK:
		{
		    break;
		}
		case IDCANCEL:
		{
		    break;
		}

		default:
		    return FALSE;
	    }
	    break;

	default:
	    return FALSE;
    }

    return TRUE;
}

char *SubstituteDirectories(char *value, char *theSrcDir, char *theDestDir, char *srcFile, char *destFile)
{
  // Change e.g. {1}file.txt into c:\test\thing\file.txt
  int srcI = 0;
  int destI = 0;
  wxBuffer[0] = 0;
  int valLen = strlen(value);  
  while (value[srcI] != 0)
  {
    // Substitute a destination directory
    if ((srcI + 2 < valLen) && (value[srcI] == '{') && (value[srcI+2] == '}'))
    {
      int which = value[srcI+1] - 48;
      if ((which > 0) && (which <= theNumberOfDestDirectories))
      {
        char buf[300];
        char *destDir = theDestDirectories[which-1];
        if (destDir && (strcmp(destDir, ".") == 0))
          sprintf(buf, "%s\\", theApplication.GetDirectory());
        else if (destDir && destDir[0] == '.' && destDir[1] == '\\')
          sprintf(buf, "%s\\%s\\", theApplication.GetDirectory(), destDir + 2);
        // Check if destDir is absolute - in which case, don't prepend GetDirectory()
        else if (destDir && (destDir[1] == ':' || destDir[0] == '\\'))
          sprintf(buf, "%s\\", destDir);
        else
          sprintf(buf, "%s\\%s\\", theApplication.GetDirectory(), destDir);

        int len = strlen(buf);
        for (int j = 0; j < len; j++)
          wxBuffer[destI + j] = buf[j];

        destI += len;
      }
      srcI += 3;
    }
    // Substitute a source directory
    else if ((srcI + 2 < valLen) && (value[srcI] == '(') && (value[srcI+2] == ')'))
    {
      int which = value[srcI+1] - 48;
      if ((which > 0) && (which <= theNumberOfSourceDirectories))
      {
        char buf[300];
        char *srcDir = theSourceDirectories[which-1];
        if (srcDir && (strcmp(srcDir, ".") != 0))
          sprintf(buf, "%s\\%s\\", theApplication.GetThisDirectory(), srcDir);
        else
          sprintf(buf, "%s\\", theApplication.GetThisDirectory());

        int len = strlen(buf);
        for (int j = 0; j < len; j++)
          wxBuffer[destI + j] = buf[j];

        destI += len;
      }
      srcI += 3;
    }
    // Substitute the specified destination directory
    else if (theDestDir && (srcI + 8 < valLen) && (strncmp((value + srcI), "{DESTDIR}", 9) == 0))
    {
      int len = strlen(theDestDir);
      for (int j = 0; j < len; j++)
        wxBuffer[destI + j] = theDestDir[j];

      destI += len;

      srcI += 9;
    }
    // Substitute the specified source directory
    else if (theSrcDir && (srcI + 7 < valLen) && (strncmp((value + srcI), "{SRCDIR}", 8) == 0))
    {
      int len = strlen(theSrcDir);
      for (int j = 0; j < len; j++)
        wxBuffer[destI + j] = theSrcDir[j];

      destI += len;
      srcI += 8;
    }
    // Substitute the windows directory
    else if ( (srcI + 11 < valLen) && (strncmp((value + srcI), "{WINDOWSDIR}", 12) == 0))
    {
      char* winDir = theApplication.MyGetWindowsDirectory();
      int len = strlen(winDir);
      for (int j = 0; j < len; j++)
        wxBuffer[destI + j] = winDir[j];

      destI += len;
      if(wxBuffer[destI-1] != '\\')
      {
         wxBuffer[destI] = '\\';
         destI++;
      }
      srcI += 12;
    }
    // Substitute the specified destination file
    else if (destFile && (srcI + 9 < valLen) && (strncmp((value + srcI), "{DESTFILE}", 10) == 0))
    {
      int len = strlen(destFile);
      for (int j = 0; j < len; j++)
        wxBuffer[destI + j] = destFile[j];

      destI += len;
      srcI += 10;
    }
    // Substitute the specified source file
    else if (srcFile && (srcI + 8 < valLen) && (strncmp((value + srcI), "{SRCFILE}", 9) == 0))
    {
      int len = strlen(srcFile);
      for (int j = 0; j < len; j++)
        wxBuffer[destI + j] = srcFile[j];

      destI += len;
      srcI += 9;
    }
    else
    {
      wxBuffer[destI] = value[srcI];
      destI ++;
      srcI ++;
    }
  }
  wxBuffer[destI] = 0;
  return wxBuffer;
}

Bool CheckDestinationOk(char *buf)
{
  int i;
  for (i = 0; i < strlen(buf); i++)
    buf[i] = tolower(buf[i]);
    
  char buf2[256];
  
  getcwd(buf2, 256);

  for (i = 0; i < strlen(buf2); i++)
    buf2[i] = tolower(buf2[i]);

  return (strcmp(buf, buf2) != 0);
}

Bool ExpandFileChooseMethod(char *fromPath, char *toPath, char *method, Bool compressed,
  char *srcDir, char *destDir, char *fromFile, char *toFile)
{
  if (!compressed || (compressed && (strcmp(method, "Y") == 0)))
    return ExpandFile(fromPath, toPath, compressed);

  char *command = GetCompressionCommand(method, srcDir, destDir, fromFile, toFile);
  if (!command)
  {
    char buf[256];
    sprintf(buf, LConv("Don't know about compression method %s!","Kompressionsmethode %s unbekannt"), method);
    MessageBox(NULL, buf, LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
    return FALSE;
  }

  // Now we have a command. Execute it.
  int success = wxExecute(command, TRUE);
  if (!success)
  {
    char buf[400];
    sprintf(buf, LConv("Sorry, could not execute command:\r\n%s","Kommando nicht ausführbar:\r\n%s"), command);
    MessageBox(NULL, buf, LConv("Installation Error","Installationsfehler"), MB_APPLMODAL|MB_ICONEXCLAMATION);
    return FALSE;
  }
  return TRUE;
}

// Substitute known directories AFTER the user has entered the
// actual directory.
void DoSubstituteDirectories(void)
{
  char theSrcDir[256];
  sprintf(theSrcDir, "%s", theApplication.GetThisDirectory());

  int i;
  for (i = 0; i < theNumberOfPMEntries; i++)
    if (thePMEntries[i]->command)
      thePMEntries[i]->SetCommand(SubstituteDirectories(thePMEntries[i]->command, theSrcDir));
  for (i = 0; i < theNumberOfExecutionEntries; i++)
    if (theExecutionEntries[i]->command)
      theExecutionEntries[i]->SetCommand(SubstituteDirectories(theExecutionEntries[i]->command, theSrcDir));
  for (i = 0; i < theNumberOfPrerequisiteEntries; i++)
    if (thePrerequisiteEntries[i]->file)
      thePrerequisiteEntries[i]->SetFile(SubstituteDirectories(thePrerequisiteEntries[i]->file, theSrcDir));
}

// Turn %n into \r\n
char *ExpandNewLines(char *value)
{
  int srcI = 0;
  int destI = 0;
  wxBuffer[0] = 0;
  int valLen = strlen(value);  
  while (value[srcI] != 0)
  {
    // Substitute a destination directory
    if ((value[srcI] == '%') && (value[srcI+1] == 'n'))
    {
      wxBuffer[destI] = '\r';
      wxBuffer[destI+1] = '\n';
      destI += 2;
      srcI += 2;
    }
    else
    {
      wxBuffer[destI] = value[srcI];
      destI ++;
      srcI ++;
    }
  }
  wxBuffer[destI] = 0;
  return wxBuffer;
}
