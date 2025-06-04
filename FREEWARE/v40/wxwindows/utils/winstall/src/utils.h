/*
 * File:        utils.h
 * Purpose:     Windows installation program for wxWindows: utilities
 * Author:      Julian Smart
 * Date:        20th April 1995
 */

#ifndef utilsh
#define utilsh

#define Bool BOOL

extern char *copystring(char *);
extern char wxBuffer[];

extern Bool wxGetResource(const char *section, const char *entry, char **value, const char *file);
extern Bool wxGetResource(const char *section, const char *entry, int *value, const char *file);

void wxStripExtension(char *buffer);
Bool wxDirExists(const char *dir);
Bool wxFileExists(const char *f);
Bool wxMkdir(const char *dir);
Bool wxYield(void);
Bool wxCopyFile (const char *file1, const char *file2);
Bool wxExecute(const char *command, Bool sync);
void wxDebugMsg(const char *fmt ...);
Bool wxSetWorkingDirectory(char *d);

#endif

