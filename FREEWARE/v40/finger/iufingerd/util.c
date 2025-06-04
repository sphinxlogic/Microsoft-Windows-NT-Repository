/***********************************************************************
 *
 *  (C) Copyright 1992-1993 The Trustees of Indiana University
 *
 *   Permission to use, copy, modify, and distribute this program for
 *   non-commercial use and without fee is hereby granted, provided that
 *   this copyright and permission notice appear on all copies and
 *   supporting documentation, the name of Indiana University not be used
 *   in advertising or publicity pertaining to distribution of the program
 *   without specific prior permission, and notice be given in supporting
 *   documentation that copying and distribution is by permission of
 *   Indiana University.
 *
 *   Indiana University makes no representations about the suitability of
 *   this software for any purpose. It is provided "as is" without express
 *   or implied warranty.
 *
 ***********************************************************************
 *
 * Module:
 *     util.c - utility routines for iufingerd.
 *
 * Description:
 *     This module contains generic utility routines
 *     used by iufingerd.
 *
 * Routines:
 *     NumericTime - convert quadword time to cool time.
 *     SytemLog    - logginf function.
 *     Tokenize    - tokenize input.
 *     TrimImage   - "trim" the image name.
 *     UpCase      - upcase a string.
 *     VmsTime     - convert quadword time to VMS time.
 *     WayCoolTime - convert quadword time to way cool text format.
 *
 ************************************************************************/

/************************************************************************
 * #include's
 ************************************************************************/
#include "iufingerd.h"
#include <varargs.h>


/************************************************************************
 * Function:
 *     NumericTime - convert quadword time to cool time.
 *
 * Description:
 *     This routine converts quadword time to cool time.
 *
 * Arguments:
 *     sysTime -    the input system time.
 *     timeString - the resulting time string.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void NumericTime(int *sysTime, char *timeString)
{
  int status;
  struct numTime
  {
    short year;
    short month;
    short day;
    short hour;
    short minute;
    short second;
    short centiseconds;
  } myTime;

  status = sys$numtim(&myTime, sysTime);
  if (VmsError(status))
  {
    SystemLog("sys$numtim: %s", VmsMessage(status));
    sprintf(timeString, "???");
  }
  else
    sprintf(timeString, "%02.2u/%02.2u %02.2u:%02.2u",
        myTime.month, myTime.day, myTime.hour, myTime.minute);
}


/************************************************************************
 * Function:
 *     SystemLog - log a timestamped string to stdout
 *
 * Description:
 *
 * Arguments:
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void SystemLog(va_alist)
va_dcl
{
  char    *format;
  va_list ap;
  char    *mp;
  char    message[256];
  time_t  calendar_time;
  struct  tm *local_time;

  calendar_time = time(0);
  local_time = localtime(&calendar_time);
  ++local_time->tm_mon;
  sprintf(message, "19%02d-%02d-%02d %02d:%02d:%02d ",
    local_time->tm_year,local_time->tm_mon,local_time->tm_mday,
    local_time->tm_hour,local_time->tm_min,local_time->tm_sec);

  va_start(ap);
  format = va_arg(ap, char *);
  mp = message + strlen(message);
  vsprintf(mp, format, ap);
  va_end(ap);

  printf("%s\n",message);
}


/************************************************************************
 * Function:
 *     Tokenize - tokenize input.
 *
 * Description:
 *     Never mind.
 *
 * Arguments:
 *     inBuffer -    the input buffer.
 *     outBuffer -   the output buffer.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void Tokenize(char *inBuffer, char *outBuffer)
{
  int        escape = FALSE;
  char      *tokenPtr;

  tokenPtr = inBuffer;
  while (*tokenPtr)
  {
    switch (*tokenPtr)
    {
      case '\r': /*** whitespace ***/
      case '\n':
      case ' ' :
        escape = FALSE;
        break;

      case '/': /*** escape character ***/
        escape = TRUE;
        break;

      default: /*** regular characters ***/
        if (!escape) *outBuffer++ = *tokenPtr;
        escape = FALSE;
        break;
    }
    tokenPtr++;
  }
  *outBuffer = '\0';
}


/************************************************************************
 * Function:
 *     TrimImage - this routine "spruces up" the image name.
 *
 * Description:
 *     This routine takes a pointer to an image string and
 *     removes the unneccessary parts.
 *
 * Arguments:
 *     image -   the image string.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void TrimImage(char *image)
{
  char *lastBracket;
  char tmpBuffer[512];
  char *tmpPtr;

  strcpy(tmpBuffer, image);

  /*** strip leading stuff ***/
  lastBracket = tmpPtr = tmpBuffer;
  while (*tmpPtr)
  {
    if (*tmpPtr == ']') lastBracket = tmpPtr;
    tmpPtr++;
  }

  if (*lastBracket) ++lastBracket;

  strcpy(image, lastBracket);

  /*** strip file extension ***/
  tmpPtr = image;
  while (*tmpPtr && (*tmpPtr != '.')) tmpPtr++;

  if (*tmpPtr) *tmpPtr = '\0';
  if (strlen(image) == 0) sprintf(image, "DCL");
}


/************************************************************************
 * Function:
 *     UpCase - upcase a string.
 *
 * Description:
 *     This routine converts a string to upper case letters.
 *
 * Arguments:
 *     string -     the string to upcase.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void UpCase(char *string)
{
  while (*string)
  {
    *string = (char)toupper(*string);
    string++;
  }
}


/************************************************************************
 * Function:
 *     VmsMessage - return a vms error message.
 *
 * Description:
 *     This routine returns the address of a static buffer which
 *     contains the error message associated with a return value.
 *
 * Arguments:
 *     code -   the return value of a vms routine.
 *
 * Returns:
 *     A pointer to a static buffer containing the associated
 *     error message.
 ************************************************************************/
char *VmsMessage(unsigned int code)
{
  static char             message[256];
  unsigned short length = 0;
  struct dsc$descriptor   message_dx;

  memset(message, 0, sizeof(message));
  message_dx.dsc$w_length =  sizeof(message);
  message_dx.dsc$a_pointer = &message;
  sys$getmsg(code, &length, &message_dx, 0xf, 0);
  message[length] = '\0';

  return(&message);
}


/************************************************************************
 * Function:
 *     VmsTime
 *
 * Description:
 *     This routine converts a quadword time to a VMS-style
 *     character string.
 *
 * Arguments:
 *     time -        the time to convert.
 *     buffer -      the buffer in which to place the string.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void VmsTime(int *time, char *buffer)
{
  long                status;
  short               timeLength;
  char               *tmp;
  $DESCRIPTOR(timeDesc, buffer);

  if ((*time == 0) && (*(time + 1) == 0))
    sprintf(buffer, "never.");
  else
  {
    timeDesc.dsc$w_length = 30;
    status = lib$sys_asctim(&timeLength, &timeDesc, time, 0);
    if (VmsError(status))
        SystemLog("lib$sys_asctim: %s", VmsMessage(status));
    buffer[timeLength] = '\0';
    tmp = buffer;

    /*** if null string, stop ***/
    if (!*tmp) return;

    /*** if string is "never", stop ***/
    if (isalpha(*tmp)) return;

    /*** skip over everything till you find a period ***/
    while (*tmp && (*tmp != '.')) tmp++;

    /*** if current char is the '.', put a null-byte there ***/
    if (*tmp) *tmp = '\0';
  }
}


/************************************************************************
 * Function:
 *     WayCoolTime - convert time from quadword to string.
 *
 * Description:
 *     This function converts a quadword time to a character string.
 *     This string is in a date/time format judged Way Most Cool by James
 *     Harvey because it fits in only nine bytes and looks like the one
 *     used to display login time in the BSD finger, and it's even easier
 *     to do on VMS using LIB$FORMAT_DATE_TIME.
 *
 *     This function is not reentrant.
 *
 * Arguments:
 *     sysTime -    the input system time.
 *     timeString - the resulting time string.
 *
 * Returns:
 *     Nothing.
 ************************************************************************/
void WayCoolTime(int *sysTime, char *timeString)
{
  unsigned long status, timeNow[2];
  int daysThen, daysNow, timeStringLength;
  struct dsc$descriptor_s timeDesc;
  static unsigned long recentContext = 0;
  static unsigned long weekoldContext = 0;
  static unsigned long yearoldContext = 0;
  static const $DESCRIPTOR(recentFormat,"|!WAC !H04:!M0|");
  static const $DESCRIPTOR(weekoldFormat,"|!MAAC !DB|");
  static const $DESCRIPTOR(yearoldFormat,"|!D0-!MAAC-!Y2|");

  /* We only initialize the context variables once. */
  if (!recentContext)
  {
    status = LIB$INIT_DATE_TIME_CONTEXT(
    		&recentContext,
    		&LIB$K_OUTPUT_FORMAT,
    		&recentFormat);
    if (VmsError(status))
      exit(status);
  }

  if (!weekoldContext)
  {
    status = LIB$INIT_DATE_TIME_CONTEXT(
    		&weekoldContext,
    		&LIB$K_OUTPUT_FORMAT,
    		&weekoldFormat);
    if (VmsError(status))
        exit(status);
  }

  if (!yearoldContext)
  {
    status = LIB$INIT_DATE_TIME_CONTEXT(
    		&yearoldContext,
    		&LIB$K_OUTPUT_FORMAT,
    		&yearoldFormat);
    if (VmsError(status))
      exit(status);
  }

  timeDesc.dsc$w_length = 9;
  timeDesc.dsc$b_dtype = DSC$K_DTYPE_T;
  timeDesc.dsc$b_class = DSC$K_CLASS_S;
  timeDesc.dsc$a_pointer = timeString;

  /* Get current time */
  status = SYS$GETTIM(timeNow);
  if (VmsError(status))
  {
    strcpy(timeString,"???");
    return;
  }

  /* Get total days from Nov 17, 1858 to now */
  status = LIB$DAY(&daysNow,timeNow,0);
  if (VmsError(status))
  {
      strcpy(timeString,"???");
      return;
  }

  /* Get total days from Nov 17, 1858 to then */
  status = LIB$DAY(&daysThen,sysTime,0);
  if (VmsError(status))
  {
      strcpy(timeString,"???");
      return;
  }

  /* If less than a week, use something like "Mon 11:20", else mmm dd */
  if (daysNow - daysThen < 7)
    status = LIB$FORMAT_DATE_TIME(&timeDesc,
                                    sysTime,
                                    &recentContext,
                                    &timeStringLength,
                                    0);
  else if (daysNow - daysThen < 6 * 30)	/* More than 6 months ago? */
    status = LIB$FORMAT_DATE_TIME(&timeDesc,
                                    sysTime,
                                    &weekoldContext,
                                    &timeStringLength,
                                    0);
  else					/* Yes, so just show date */
    status = LIB$FORMAT_DATE_TIME(&timeDesc,
                                    sysTime,
                                    &yearoldContext,
                                    &timeStringLength,
                                    0);

  if (VmsError(status))
    strcpy(timeString,"???");
  else
    timeString[timeStringLength] = '\0';
}
