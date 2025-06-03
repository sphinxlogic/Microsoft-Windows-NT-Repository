#if !defined (_MSC_VER) && !defined (__EMX__)
# include <dir.h>
#endif

#define INCL_BASE
#define INCL_NOPM
#define INCL_VIO
#define INCL_KBD
#define INCL_DOS
#if 0
# define INCL_DOSSEMAPHORES
#endif
#ifdef LONG
#undef LONG
#endif
#include <os2.h>

#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>
#include <dos.h>
#include <errno.h>

#include "config.h"
#include "sysdep.h"
#include "screen.h"
#include "buffer.h"
#include "misc.h"
#include "hooks.h"

int Abort_Char = 7;		       /* scan code for G (control) */

#include "dos_os2.c"

extern char *get_cwd (void);
void delay (int time);

void set_kbd (void);
void thread_getkey (void);

#define lowercase strlwr

KBDINFO	initialKbdInfo;	/* keyboard info		*/

/* Code to read keystrokes in a separate thread */

typedef struct kbdcodes {
  UCHAR ascii;
  UCHAR scan;
  USHORT shift;
} KBDCODES;

#define BUFFER_LEN 4096
static KBDCODES *threadKeys;
static int atEnd = 0;
static int startBuf;
static int endBuf;

/* Original code used semaphores to control access to threadKeys.
 * It is expected that the semaphore code will be deleted after 0.97.
*/
#if 0

#ifdef __os2_16__

typedef USHORT APIRET;
static HSEM Hmtx;

#define DosRequestMutexSem(hmtx,timeout) DosSemRequest(hmtx,timeout)
#define DosReleaseMutexSem(hmtx) DosSemClear(hmtx)
#define DosCloseMutexSem(hmtx) DosCloseSem(hmtx)

#else /* !defined(__os2_16__) */

static HMTX Hmtx;     /* Mutex Semaphore */

#endif


APIRET CreateSem(void)
{
#ifdef __os2_16__
  char SemName[32];
  sprintf(SemName, "\\SEM\\jed\\%u", getpid());
  return ( DosCreateSem (0, &Hmtx, SemName) );
#else
  return ( DosCreateMutexSem (NULL, &Hmtx, 0, 0) );
#endif
}

APIRET RequestSem(void)
{
  return ( DosRequestMutexSem (Hmtx, -1) );
}

APIRET ReleaseSem(void)
{
  return ( DosReleaseMutexSem (Hmtx) );
}

APIRET CloseSem(void)
{
  return( DosCloseMutexSem (Hmtx) );
}

#else

#define CreateSem()
#define RequestSem()
#define ReleaseSem()
#define CloseSem()

#endif


void thread_getkey ()
{
   KBDKEYINFO keyInfo;
   int n;

   while (!atEnd) {     /* at end is a flag */
      set_kbd();
      KbdCharIn(&keyInfo, IO_NOWAIT, 0);       /* get a character	*/
      if (keyInfo.fbStatus & 0x040) {          /* found a char process it */
	if (keyInfo.chChar == Abort_Char) {
	  SLang_Error = 2;
	  SLKeyBoard_Quit = 1;
	}
	n = (endBuf + 1) % BUFFER_LEN;
	if (n == startBuf) {
	  DosBeep (500, 20);
	  KbdFlushBuffer(0);
	  continue;
	}
	RequestSem();
	threadKeys [n].ascii = keyInfo.chChar;
	threadKeys [n].scan = keyInfo.chScan;
	threadKeys [n].shift = keyInfo.fsState;
	endBuf = n;
	ReleaseSem();
      } else                    /* no char available*/
	DosSleep (20);
   }
}

void thread_code (void *Args)
{
  (void) Args;
  startBuf = -1;      /* initialize the buffer pointers */
  endBuf = -1;
  thread_getkey ();
  atEnd = 0;          /* reset the flag */
  _endthread();
}


/* The code below is in the main thread */

void set_kbd()
{
  KBDINFO kbdInfo;

  kbdInfo = initialKbdInfo;
  kbdInfo.fsMask &= ~0x0001;		/* not echo on		*/
  kbdInfo.fsMask |= 0x0002;		/* echo off		*/
  kbdInfo.fsMask &= ~0x0008;		/* cooked mode off	*/
  kbdInfo.fsMask |= 0x0004;		/* raw mode		*/
  kbdInfo.fsMask &= ~0x0100;		/* shift report	off	*/
  KbdSetStatus(&kbdInfo, 0);
}

static int enhancedKeyboard;

void init_tty ()
{
  VIOCURSORINFO cursorInfo, OldcursorInfo;

  /*  set ^C off */
  signal (SIGINT, SIG_IGN);
  signal (SIGBREAK, SIG_IGN);

  /* set up the keyboard */

   if (threadKeys == NULL)
     {
	if (NULL == (threadKeys = SLCALLOC (sizeof(KBDCODES), BUFFER_LEN)))
	  {
	     exit_error ("init_tty: Malloc error.", 0);
	  }
     }

  initialKbdInfo.cb = sizeof(initialKbdInfo);
  KbdGetStatus(&initialKbdInfo, 0);
  set_kbd();
  enhancedKeyboard = 1;

  /* open a semaphore */
  CreateSem();

  /* start a separate to read the keyboard */
#if defined(__BORLANDC__)
  _beginthread (thread_code, 8096, NULL);
#else
  _beginthread (thread_code, NULL,  8096, NULL);
#endif

  VioGetCurType (&OldcursorInfo, 0);
  cursorInfo.yStart = 1;
  cursorInfo.cEnd = 15;
  cursorInfo.cx = 1;
  cursorInfo.attr = 1;
  if (VioSetCurType (&cursorInfo, 0))
    VioSetCurType (&OldcursorInfo, 0);   /* reset to previous value */
}


void reset_tty ()
{
  atEnd = 1;                      /* set flag and wait until thread ends */
  while (atEnd) {DosSleep (0);}

  CloseSem();

  /* close the keyboard */
  KbdSetStatus(&initialKbdInfo, 0); /* restore original state	*/
}

unsigned char sys_getkey ()
{
   char *keypad_scan =
      "\x4e\x53\x52\x4f\x50\x51\x4b\x4c\x4d\x47\x48\x49\x37\x4a";
   char *normal = "!@#$%^&*()-=\t*\0177QWERTYUIOP[]\r*ASDFGHJKL;'`*\\ZXCVBNM<>/";
   char *edt_chars = "lnpqrstuvwxyRS";
   int bra = 'O', P = 'P', keypad, weird;
   char *p;
   unsigned int c, c1, shft, shift, i;

   weird = 300;

   while (!sys_input_pending(&weird))
     {
	if (Display_Time)
	  {
	     JWindow->trashed = 1;
	     update((Line *) NULL, 0, 1);
	  }
     }

   /* read codes from buffer */
   RequestSem();
   startBuf = (startBuf + 1) % BUFFER_LEN;
   c = threadKeys [startBuf].ascii;
   c1 = threadKeys [startBuf].scan;
   shift = threadKeys [startBuf].shift;
   ReleaseSem();

   shft = shift & 0xF;

   keypad = (c1 > 0x36) && NumLock_Is_Gold && (enhancedKeyboard);
/*   keypad = (c == 0) && (c1 > 0x36); */
   /* allow Shift-keypad to give non edt keypad chars */
   keypad = keypad && !(shft & 0x3);

   /* This is for the damned enter and slash keys */
   weird = keypad && ((c1 == 0xE0) && ((c == 0xD) || (c == 0x2F)));

   keypad = keypad && (c != 0xE0);  /* excludes small keypad */

   if ((c == 8) && ((c1) == 0x0e)) c =127;
   else if ((c == 32) && (shft & 0x04))    /* ^ space = ^@ */
     {
	c = 3;
	ungetkey ((int*) &c);
	c = 0;
     }
   else if (weird || (keypad && (NULL != (p = strchr(keypad_scan, c1)))))
     {
	if (c1 == 0xE0) if (c == 0x2f) c = 'Q'; else c = 'M';
	else c = (unsigned int) edt_chars[(int) (p - keypad_scan)];
	ungetkey ((int*) &c);
	ungetkey ((int*) &bra);
	c = 27;
     }
   else if ((!c) || ((c == 0xe0) && (enhancedKeyboard) && (c1 > 0x43)))
     {
	if (NumLock_Is_Gold && (c1 == 0x3B))
	  {
	     ungetkey((int*)&P); ungetkey((int*) &bra); c = 27;
	  }
	/* if key is 5 of keypad, get another char */
	else if ((c1 == 'L') && enhancedKeyboard && (c == 0)) return(sys_getkey());
	else
	  {
	     if ((shft == 0x8) && (c == 0) && PC_Alt_Char)
	       {
		  if ((c1 >= 14) && (c1 <= 53))
		    {
		       c1 = (unsigned int) normal[c1];
		       c = PC_Alt_Char;
		    }
		  else if ((c1 >= 120) && (c1 <= 131))
		    {
		       c1 = (unsigned int) normal[c1 - 120];
		       c = PC_Alt_Char;
		    }
		  else if (c1 == 165) /* tab */
		    {
		       c1 = (unsigned int) normal[c1 - 165 + 12];
		       c = PC_Alt_Char;
		    }
	       }
	     else c = 0;
	     ungetkey((int*) &c1);
	  }
     }
   return(c);
}

void sys_flush_input()
{
  KbdFlushBuffer(0);
  endBuf = startBuf;
}

#define keyWaiting() (endBuf != startBuf)

/* sleep for *tsecs tenths of a sec waiting for input */
static int sys_input_pending(int *tsecs)
{
   int count = *tsecs * 5;

   if (Batch || Input_Buffer_Len) return(Input_Buffer_Len);

   if (count)
     {
	while(count > 0)
	  {
	     delay(20);		       /* 20 ms or 1/50 sec */
	     if (keyWaiting ()) break;
	     count--;
	  }
	return(count);
     }
   else return(keyWaiting ());
}


void get_term_dimensions(int *w, int *h)
{
  VIOMODEINFO vioModeInfo;

  vioModeInfo.cb = sizeof(vioModeInfo);
  VioGetMode (&vioModeInfo, 0);
  *w = vioModeInfo.col;
  *h = vioModeInfo.row;
}

#if defined(_MSC_VER) || defined (__EMX__)
int sys_chmod(char *file, int what, int *mode, short *uid, short *gid)
{
   struct stat buf;
   int m;

#ifdef _MSC_VER 
   /* MSC stat() is broken on directory names ending with a slash */

   char path[_MAX_PATH];
   strcpy(path, file); 
   deslash(file = path);
#endif

   if (what)
     {
	chmod(file, *mode);
	return(0);
     }

   if (stat(file, &buf) < 0) switch (errno)
     {
	case EACCES: return(-1); /* es = "Access denied."; break; */
	case ENOENT: return(0);  /* ms = "File does not exist."; */
	case ENOTDIR: return(-2); /* es = "Invalid Path."; */
	default: return(-3); /* "stat: unknown error."; break;*/
     }

   m = buf.st_mode;

   *mode = m & 0777;

   if (m & S_IFDIR) return (2);
   return(1);
}

#else

int sys_chmod(char *file, int what, int *mode, short *dum1, short *dum2)
{
  FILESTATUS3 fileInfo;
  USHORT Result;
  (void) dum1; (void) dum2;
  Result = DosQueryPathInfo (file, FIL_STANDARD, &fileInfo, sizeof (fileInfo));
  if (Result == ERROR_FILE_NOT_FOUND) return 0;
  else if (Result == ERROR_ACCESS_DENIED) return -1;
  else if (Result == ERROR_INVALID_PATH) return -2;
  else if (Result != NO_ERROR) return -3;

  *mode = fileInfo.attrFile;

  if (what) {
    fileInfo.attrFile = *mode;
    DosSetPathInfo (file, FIL_STANDARD, &fileInfo, sizeof (fileInfo), 0);
  }

  if (*mode & 0x10) return(2);
  else return (1);
}

#endif

static char Found_Dir[256];

#ifdef __os2_16__
#define DosFindFirst(FileName, DirHandle, Attribute, ResultBuf, \
		     ResultBufLen, SearchCount, Reserved) \
	DosFindFirst(FileName, DirHandle, Attribute, ResultBuf, \
		     ResultBufLen, SearchCount, 0)
#endif

int sys_findfirst(char *theFile)
{
   char *f, the_path[CCHMAXPATH], *file, *f1;
   char *pat;

#if defined(__os2_16__)
  FILEFINDBUF FindBuffer;
  USHORT FindCount;
  USHORT File_Attr;
#else
  FILEFINDBUF3 FindBuffer;
  ULONG FindCount;
  ULONG File_Attr;
#endif

  HDIR FindHandle;
  File_Attr = FILE_READONLY | FILE_DIRECTORY;

  file = expand_filename(theFile);
  f1 = f = extract_file(file);
  strcpy (Found_Dir, file);

   Found_Dir[(int) (f - file)] = 0;

  strcpy(the_path, file);

  while (*f1 && (*f1 != '*')) f1++;
  if (! *f1)
    {
	while (*f && (*f != '.')) f++;
	if (*f) strcat(the_path, "*"); else strcat(the_path, "*.*");
    }
  pat = the_path;

  FindHandle = 1;
  FindCount = 1;
  if (DosFindFirst(pat, &FindHandle, File_Attr, &FindBuffer,
    sizeof (FindBuffer), &FindCount, FIL_STANDARD) != NO_ERROR)
    return 0;
  strcpy (theFile, Found_Dir);
  strcat (theFile, FindBuffer.achName);
  if (FindBuffer.attrFile & FILE_DIRECTORY) fixup_dir (theFile);
  return 1;
}

int sys_findnext(char *file)
{
#ifdef __os2_16__
  FILEFINDBUF FindBuffer;
  USHORT FileCount;
#else
  FILEFINDBUF3 FindBuffer;
  ULONG FileCount;
#endif

  FileCount = 1;

  if (DosFindNext(1, &FindBuffer, sizeof (FindBuffer), &FileCount))
    return 0;
  else {
    strcpy (file, Found_Dir);
    strcat (file, FindBuffer.achName);
    if (FindBuffer.attrFile & FILE_DIRECTORY) fixup_dir (file);
    return 1;
  }
}



/* Here we do a find first followed by calling routine to conver time */
#if defined(_MSC_VER) || defined(__EMX__)

unsigned long sys_file_mod_time(char *file)
{
   struct stat buf;

   if (stat(file, &buf) < 0) return(0);
   return((unsigned long) buf.st_mtime);
}

#else

unsigned long sys_file_mod_time(char *file)
{
   FILESTATUS3 fileInfo;
   struct time t;
   struct date d;

   if (DosQueryPathInfo (file, FIL_STANDARD, &fileInfo, sizeof (fileInfo))
       != NO_ERROR)
       return 0;
   t.ti_min = fileInfo.ftimeLastWrite.minutes;
   t.ti_hour = fileInfo.ftimeLastWrite.hours;
   t.ti_hund = 0;
   t.ti_sec = fileInfo.ftimeLastWrite.twosecs;
   d.da_day = fileInfo.fdateLastWrite.day;
   d.da_mon = fileInfo.fdateLastWrite.month;
   d.da_year = fileInfo.fdateLastWrite.year;
   return dostounix(&d, &t);
}

#endif

void delay (int time)
{
  DosSleep (time);
}

int IsHPFSFileSystem (char *directory)
{
    ULONG		lMap;
    BYTE		bData[128];
    BYTE		bName[3];
    int			i;
    char		*FName;
#if defined (__os2_16__)
    USHORT		cbData;
    USHORT		nDrive;
#define DosQueryCurrentDisk DosQCurDisk
#else
    ULONG		cbData;
    ULONG		nDrive;
    PFSQBUFFER2		pFSQ = (PFSQBUFFER2)bData;
#endif

    if ( _osmode == DOS_MODE )
	return FALSE;

    if (isalpha (directory[0]) && (directory[1] == ':'))
	nDrive = toupper (directory[0]) - '@';
    else
	DosQueryCurrentDisk (&nDrive, &lMap);

/* Set up the drive name */

    bName[0] = (char) (nDrive + '@');
    bName[1] = ':';
    bName[2] = 0;

    cbData = sizeof (bData);

#if defined(__os2_16__)
    if (DosQFSAttach (bName, 0, FSAIL_QUERYNAME, bData, &cbData, 0L))
#else
    if (DosQueryFSAttach (bName, 0, FSAIL_QUERYNAME, pFSQ, &cbData))
#endif
      return FALSE;

#if defined(__os2_16__)
    FName = bData + (*((USHORT *) (bData + 2)) + 7);
#else
    FName = pFSQ->szName + pFSQ->cbName + 1;
#endif

    if (strcmp(FName, "HPFS"))
      return FALSE;
    return(TRUE);
}


/* Code for EAs */

#if defined(__os2_16__)
/* Variables and data structures used for handling EA's, these are
   defined in the 32 BIT API, so these will be included in os2.h */

typedef USHORT APIRET;
typedef struct _FEA2         /* fea2 */
		{
#if 0			/* This field is in the 32 bit structure */
		ULONG oNextEntryOffset;		/* New field */
#endif
		BYTE fEA;
		BYTE cbName;
		USHORT cbValue;
		CHAR szName[1];				/* New field */
		} FEA2;
typedef FEA2 *PFEA2;

typedef struct _FEA2LIST		/* fea2l */
		{
		ULONG cbList;
		FEA2 list[ 1 ];
		} FEA2LIST;

typedef FEA2LIST *PFEA2LIST;

typedef struct _GEA2			/* gea2 */
		{
#if 0
		/* New field - in the 32 bit structure */
		ULONG oNextEntryOffset;
#endif /* OS2_32 */
		BYTE cbName;
		CHAR szName[ 1 ];		/* New field */
		} GEA2;

typedef GEA2 *PGEA2;

typedef struct _GEA2LIST		/* gea2l */
		{
		ULONG cbList;
		GEA2 list[ 1 ];
		} GEA2LIST;

typedef GEA2LIST *PGEA2LIST;

typedef struct _EAOP2			/* eaop2 */
		{
		PGEA2LIST fpGEA2List;	/* GEA set */
		PFEA2LIST fpFEA2List;	/* FEA set */
		ULONG oError;			/* Offset of FEA error */
		} EAOP2;

/* typedef EAOP2 *PEAOP2; */

#define DosSetPathInfo(PathName, PathInfoLevel, PathInfoBuf, \
		       PathInfoBufSize, PathInfoFlags) \
        DosSetPathInfo(PathName, PathInfoLevel, PathInfoBuf, \
		       PathInfoBufSize, PathInfoFlags, 0)
#define DosQueryPathInfo(PathName, PathInfoLevel, PathInfoBuf, \
			 PathInfoBufSize) \
        DosQPathInfo(PathName, PathInfoLevel, PathInfoBuf, PathInfoBufSize, 0)
#define DosEnumAttribute(RefType, FileRef, EntryNum, EnumBuf, \
			 EnumBufSize, EnumCnt, InfoLevel) \
        DosEnumAttribute(RefType, FileRef, EntryNum, EnumBuf, \
			 EnumBufSize, EnumCnt, InfoLevel, 0)
#endif

/* The HoldFEA is used to hold individual EAs.  The member names correspond
   directly to those of the FEA structure.  Note however, that both szName
   and aValue are pointers to the values.  An additional field, next, is
   used to link the HoldFEA's together to form a linked list. */

struct _HoldFEA
	{
	BYTE fEA;					/* Flag byte */
	BYTE cbName;
	USHORT cbValue;
	CHAR *szName;
	BYTE *aValue;
	struct _HoldFEA *next;
	};

typedef struct _HoldFEA HOLDFEA;

#define MAX_GEA				500L /* Max size for a GEA List */
#define REF_ASCIIZ			1	/* Reference type for DosEnumAttribute */

#define GET_INFO_LEVEL1		1	/* Get info from SFT */
#define GET_INFO_LEVEL2		2	/* Get size of FEAlist */
#define GET_INFO_LEVEL3		3	/* Get FEAlist given the GEAlist */
#define GET_INFO_LEVEL4		4	/* Get whole FEAlist */
#define GET_INFO_LEVEL5		5	/* Get FSDname */

#define SET_INFO_LEVEL1		1	/* Set info in SFT */
#define SET_INFO_LEVEL2		2	/* Set FEAlist */


/* #define BufferSize 1024
static char FileBuffer [BufferSize]; */
static HOLDFEA *pHoldFEA;			/* EA linked-list pointer */

/* Free_FEAList (pFEA)
   Frees the memory used by the linked list of HOLDFEA's pointed to by pFEA */

void Free_FEAList( HOLDFEA *pFEA )
	{
	HOLDFEA *next;	/* Holds the next field since we free the structure
					   before reading the current next field */

	/* Step through the list freeing all the fields */
	while( pFEA )
		{
		/* Free the fields of the struct */
		next = pFEA->next;
		if( pFEA->szName != NULL )
			/* Free if non-NULL name */
			free(pFEA->szName);
		if( pFEA->aValue != NULL )
			/* Free if non-NULL value */
			free(pFEA->aValue);

		/* Free the pFEA struct itself and move on to the next field */
		free(pFEA);
		pFEA = next;
		}
	}

#if 0
/* Read the EA type, this is stored at the start of the EA value */

ULONG getEAType( const CHAR *Value )
	{
	USHORT Type = *( USHORT * ) Value;

	return( Type );
	}

/* Return the EA length, stored in pFEA, this done so that it is calculated
   in only one place */

ULONG getEALength( const HOLDFEA *pFEA )
	{
	return( sizeof( FEA2 )
			- sizeof( CHAR )	/* Don't include the first element of aValue */
			+ pFEA->cbName + 1	/* Length of ASCIIZ name */
			+ pFEA->cbValue );	/* The value length */
	}

/* Set the first two words of the EA value, this is usually the
   EA type and EA size in pFEA, from the values Type and Size */

void setEATypeSize( const HOLDFEA *pFEA, const USHORT Type, const USHORT Size )
	{
	USHORT *valPtr = ( USHORT * ) pFEA->aValue;
	valPtr[ 0 ] = Type;
	valPtr[ 1 ] = Size;
	}

/* Read the first two words of the EA value, this is usually the
   EA type and EA size in pFEA, into the Type and Size */

void getEATypeSize( const HOLDFEA *pFEA, USHORT *Type, USHORT *Size )
	{
	USHORT *valPtr = ( USHORT * ) pFEA->aValue;
	*Type = valPtr[ 0 ];
	*Size = valPtr[ 1 ];
	}

/* Get the address of the EA value in pFEA, ie skip over the Type and Size */

void* getEADataVal (const HOLDFEA *pFEA)
	{
	/* Skip over the type and size */
	return pFEA->aValue + 2 * sizeof ( USHORT );
	}
#endif

/* QueryEAs (szFileName)
      find all EAs that file szFileName has
      return these in the linked list of HOLDFEAs
      if no EAs exist or the linked list cannot be created then
      return NULL
  This function is modelled after code from IBM's Toolkit */

HOLDFEA *QueryEAs( const CHAR *szFileName )
	{
	HOLDFEA *pHoldFEA;		/* The start of the linked list */

	CHAR *pAllocc = NULL;	/* Temp buffer used with DosEnumAttribute */
	CHAR *pBigAlloc = NULL;	/* Temp buffer to hold each EA as it is read in */
	USHORT cbBigAlloc = 0;

	ULONG ulEntryNum = 1;	/* Count of current EA to read (1-relative) */
	ULONG ulEnumCnt;		/* Number of EAs for Enum to return, always 1 */

	HOLDFEA *pLastIn = 0;	/* Points to last EA added, so new EA can link    */
	HOLDFEA *pNewFEA = NULL; /* Struct to build the new EA in                  */

	FEA2 *pFEA;				/* Used to read from Enum's return buffer */
	GEA2LIST *pGEAList;		/* Ptr used to set up buffer for DosQueryPathInfo() */
	EAOP2 eaopGet;			/* Used to call DosQueryPathInfo() */

	pAllocc = malloc( MAX_GEA );	/* Allocate room for a GEA list */
	pFEA = ( FEA2 * ) pAllocc;
	pHoldFEA = NULL;		/* Initialize the linked list */

	/* Loop through all the EA's adding them to the list */
	while( TRUE )
		{
		ulEnumCnt = 1;		/* No of EAs to retrieve */
		if( DosEnumAttribute( REF_ASCIIZ,	/* Read into EA name into */
							  szFileName,	/* pAlloc Buffer */
							  ulEntryNum, pAllocc, MAX_GEA, &ulEnumCnt,
							  ( LONG ) GET_INFO_LEVEL1 ) )
			break;	/* An error */

		/* Exit if all the EA's have been read */
		if( ulEnumCnt != 1 )
			break;

		/* Move on to next EA */
		ulEntryNum++;

		/* Try and allocate the HoldFEA structure */
		if( ( pNewFEA = malloc( sizeof( HOLDFEA ) ) ) == NULL )
			{
			free( pAllocc );
			Free_FEAList( pHoldFEA );
			return( NULL );
			}

		/* Fill in the HoldFEA structure */
		pNewFEA->cbName = pFEA->cbName;
		pNewFEA->cbValue= pFEA->cbValue;
		pNewFEA->fEA = pFEA->fEA;
		pNewFEA->next = '\0'; 
/*		pNewFEA->next = NULL; */

		/* Allocate the two arrays */
		if( ( pNewFEA->szName = malloc( pFEA->cbName + 1 ) ) == NULL || \
			( pNewFEA->aValue = malloc( pFEA->cbValue ) ) == NULL )
			{
			/* Out of memory, clean up and exit */
			if( pNewFEA->szName )
				free( pNewFEA->szName );
			if( pNewFEA->aValue )
				free( pNewFEA->aValue );

			free( pAllocc );
			free( pNewFEA );

			Free_FEAList( pHoldFEA );
			return( NULL );
			}

		/* Copy EA name across */
		strcpy( pNewFEA->szName, pFEA->szName );
		cbBigAlloc = sizeof( FEA2LIST ) + pNewFEA->cbName + 1 + pNewFEA->cbValue;
							/* +1 is for '\0' */
		if( ( pBigAlloc = malloc( cbBigAlloc ) ) == NULL )
			{
			free( pNewFEA->szName );
			free( pNewFEA->aValue );
			free( pAllocc );
			free( pNewFEA );
			Free_FEAList( pHoldFEA );
			return( NULL );
			}

		/* Set up GEAlist structure */
		pGEAList = ( GEA2LIST * ) pAllocc;
		pGEAList->cbList = sizeof( GEA2LIST ) + pNewFEA->cbName;	/* + 1 for NULL */
#ifndef __os2_16__
		pGEAList->list[ 0 ].oNextEntryOffset = 0L;
#endif
		pGEAList->list[ 0 ].cbName = pNewFEA->cbName;
		strcpy( pGEAList->list[ 0 ].szName, pNewFEA->szName );

		eaopGet.fpGEA2List = ( GEA2LIST FAR * ) pAllocc;
		eaopGet.fpFEA2List = ( FEA2LIST FAR * ) pBigAlloc;

		eaopGet.fpFEA2List->cbList = cbBigAlloc;

		/* Get the complete EA info and copy the EA value */
		DosQueryPathInfo( szFileName, FIL_QUERYEASFROMLIST, ( PVOID ) &eaopGet, \
						  sizeof( EAOP2 ) );
		memcpy( pNewFEA->aValue, \
				pBigAlloc + sizeof( FEA2LIST ) + pNewFEA->cbName, \
				pNewFEA->cbValue );

		/* Release the temp. Enum buffer */
		free( pBigAlloc );

		/* Add to the list */
		if( pHoldFEA == NULL )
			pHoldFEA = pNewFEA;
		else
			pLastIn->next = pNewFEA;
		pLastIn = pNewFEA;
		}

	/* Free up the GEA buffer for DosEnum() */
	free( pAllocc );

	return pHoldFEA;
	}


/* WriteEAs(szFileName, pHoldFEA)

   Write the EAs contained in the linked list pointed to by pHoldFEA
   to the file szFileName.

   Returns TRUE if the write was successful, FALSE otherwise */

int WriteEAs( const char *szFileName, HOLDFEA *pHoldFEA )
{
  HOLDFEA *pHFEA = pHoldFEA;
  EAOP2 eaopWrite;
  CHAR *aPtr = NULL;
  USHORT usMemNeeded;
  APIRET rc;

  eaopWrite.fpGEA2List = NULL;
  while( pHFEA )                                  /* Go through each HoldFEA */
    {
      usMemNeeded = sizeof( FEA2LIST ) + pHFEA->cbName + 1 + pHFEA->cbValue;

      if( ( aPtr = malloc( usMemNeeded ) ) == NULL )
	return FALSE;
      
      /* Fill in eaop structure */
      eaopWrite.fpFEA2List = ( FEA2LIST FAR * ) aPtr;
      eaopWrite.fpFEA2List->cbList = usMemNeeded;
#ifdef __os2_16__  /* ugly hack for difference in 16-bit FEA struct */
      eaopWrite.fpFEA2List->cbList -= ( sizeof(FEA2LIST) - sizeof(FEALIST) );
#else
      eaopWrite.fpFEA2List->list[ 0 ].oNextEntryOffset = 0L;
#endif
      eaopWrite.fpFEA2List->list[ 0 ].fEA = pHFEA->fEA;
      eaopWrite.fpFEA2List->list[ 0 ].cbName = pHFEA->cbName;
      eaopWrite.fpFEA2List->list[ 0 ].cbValue = pHFEA->cbValue;
      strcpy( eaopWrite.fpFEA2List->list[ 0 ].szName, pHFEA->szName );
      memcpy( eaopWrite.fpFEA2List->list[ 0 ].szName + pHFEA->cbName + 1, \
	     pHFEA->aValue, pHFEA->cbValue );

      /* Write out the EA */
      rc = DosSetPathInfo( szFileName, FIL_QUERYEASIZE,
			     ( PVOID ) &eaopWrite, sizeof( EAOP2 ),
			     DSPI_WRTTHRU );

      /* Free up the FEALIST struct */
      free( aPtr );
      
      /* If the write failed, leave now */
      if( rc )
	return FALSE;
      
      pHFEA = pHFEA->next;
    }
  
  return( TRUE );
}

