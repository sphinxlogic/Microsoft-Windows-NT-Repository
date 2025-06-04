/*
 * File:        install.h
 * Purpose:     Windows installation program for wxWindows
 * Author:      Julian Smart
 * Date:        20th April 1995
 */

#ifndef winstallh
#define winstallh

/*
 * Declare classes for holding file, directory etc. info
 */

class Application
{
  public:
   char *name;
   char *title;
   char *message; // Message to replace the default "This procedure installs...."
   char *pmGroup;
   char *directory;
   char *windowsDirectory;
   char *thisDirectory;
   int freeSpace;

   Application(void);
   ~Application(void);

   inline char *GetName(void) { return name; }
   inline char *GetTitle(void) { return title; }
   inline char *GetMessage(void) { return message; }
   inline char *GetPMGroup(void) { return pmGroup; }
   inline char *GetDirectory(void) { return directory; }
   inline char *MyGetWindowsDirectory(void) { return windowsDirectory; }
   inline char *GetThisDirectory(void) { return thisDirectory; }
   inline int MyGetFreeSpace(void) { return freeSpace; }
   inline void SetFreeSpace(int f) { freeSpace = f; }
   void SetName(char *n);
   void SetMessage(char *m);
   void SetDirectory(char *d);
   void SetWindowsDirectory(char *d);
   void SetThisDirectory(char *d);
   void SetTitle(char *t);
   void SetPMGroup(char *g);
};

class FileGroupEntry
{
 public:
   char *srcFilename;
   char *destFilename;
   char *title;
   BOOL compressed;
   int sourceDir;
   int destDir;
   int disk;
   char *compressionMethod;

   FileGroupEntry(void);
   ~FileGroupEntry(void);

   inline char *GetSrcFilename(void) { return srcFilename; }
   inline char *GetDestFilename(void) { return destFilename; }
   inline char *GetTitle(void) { return title; }
   inline char *GetMethod(void) { return compressionMethod; }
   void SetSrcFilename(char *s);
   void SetDestFilename(char *s);
   void SetTitle(char *s);

   void ReadEntry(char *value, Bool ignoreDestCheck);
};

class PMGroupEntry
{
 public:
   char *title;
   char *command;
   int iconNo;
   char *program;

   PMGroupEntry(void);
   ~PMGroupEntry(void);

   inline char *GetTitle(void) { return title; }
   inline char *GetCommand(void) { return command; }
   void SetTitle(char *s);
   void SetCommand(char *s);

   void ReadEntry(char *lhs, char *value);
};

class FileGroup
{
  public:
   char *groupName;
   int noEntries;
   FileGroupEntry *entries[500];
   Bool installThis;

   FileGroup(void);
   ~FileGroup(void);

   inline char *GetGroupName(void) { return groupName; }
   void SetGroupName(char *s);
   void ReadEntry(char *lhs, char *rhs);
};

class ExecutionEntry
{
  public:
   char *message;
   char *command;
   Bool waitForTerm;
   Bool askYN;

   ExecutionEntry(void);
   ~ExecutionEntry(void);

   inline char *GetMessage(void) { return message ; }
   inline char *GetCommand(void) { return command; }

   void SetCommand(char *s);
   
   void ReadEntry(char *lhs, char *rhs);
};

class CompressionMethod
{
  public:
   char *name;
   char *command;

   CompressionMethod(void);
   ~CompressionMethod(void);

   inline char *GetName(void) { return name; }
   char *GetInstantiatedCommand(char *srcDir, char *destDir, char *srcFile, char *destFile);
};

class PrerequisiteEntry
{
  public:
   char *file;
   char *message;
   Bool abort;
   Bool askForContinue;

   PrerequisiteEntry(void);
   ~PrerequisiteEntry(void);

   inline char *GetFile(void) { return file; }
   inline char *GetMessage(void) { return message; }

   void SetFile(char *s);
   void ReadEntry(char *lhs, char *rhs);
};

extern Application theApplication;

extern int theNumberOfDisks;
extern char *theDisks[];
extern void SetDiskName(int i, char *name);

extern int theNumberOfDestDirectories;
extern char *theDestDirectories[];
extern void SetDestDirectoryName(int i, char *name);

extern int theNumberOfSourceDirectories;
extern char *theSourceDirectories[];
extern void SetSourceDirectoryName(int i, char *name);

extern int theNumberOfFileGroups;
extern FileGroup *theFileGroups[];

extern int theNumberOfCompressors;
extern CompressionMethod *theCompressors[];

extern int theNumberOfPMEntries;
extern PMGroupEntry *thePMEntries[];

extern int theNumberOfExecutionEntries;
extern ExecutionEntry *theExecutionEntries[];

extern int theNumberOfPrerequisiteEntries;
extern PrerequisiteEntry *thePrerequisiteEntries[];

extern FileGroup theWindowsFileGroup;

extern Bool ReadInf(void);

extern Bool CompressFile(char *in, char *out);
extern Bool ExpandFile(char *in, char *out);

/*
 * Steps in installation
 */

Bool CreateDestDirectories(void);
Bool CopyAllFiles(void);
Bool InstallPMIcons(void);
Bool AskUserForFileGroups(void); // Which groups to install?
Bool QueryRunPrograms(void);
Bool CheckPrerequisites(void);
void SetCopyText(char *from, char *to, int percent);

int Quit(Bool ok = TRUE);
char *SubstituteDirectories(char *value, char *theSrcDir = NULL, char *theDestDir = NULL,
   char *srcFile = NULL, char *destFile = NULL);
// Turn %n into \r\n (suitable for newlines in static control)
char *ExpandNewLines(char *value);

// Substitute known directories AFTER the user has entered the
// actual directory.
void DoSubstituteDirectories(void);

Bool CheckDestinationOk(char *buf);
char *GetCompressionCommand(char *method, char *srcDir, char *destDir, char *srcFile, char *destFile);
Bool ExpandFileChooseMethod(char *fromPath, char *toPath, char *method, Bool compressed,
  char *srcDir, char *destDir, char *fromFile, char *toFile);

#endif
