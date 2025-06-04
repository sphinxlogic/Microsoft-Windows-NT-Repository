/*
 * table.cc
 * Utilities for manipulating tables
 *
 */
 
// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include <wx_hash.h>
#if !defined(USE_IOSTREAMH) || USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif
#include <fstream.h>
#include <ctype.h>
#include "tex2any.h"
#include "table.h"

ColumnData TableData[40];
Bool inTabular = FALSE;

Bool startRows = FALSE;
Bool tableVerticalLineLeft = FALSE;
Bool tableVerticalLineRight = FALSE;
int noColumns = 0;   // Current number of columns in table
int ruleTop = 0;
int ruleBottom = 0;
int currentRowNumber = 0;

/*
 * Parse table argument
 *
 */

Bool ParseTableArgument(char *value)
{
  noColumns = 0;
  int i = 0;
  int len = strlen(value);
  Bool isBorder = FALSE;
  while (i < len)
  {
    int ch = value[i];
    if (ch == '|')
    {
      i ++;
      isBorder = TRUE;
    }
    else if (ch == 'l')
    {
      TableData[noColumns].leftBorder = isBorder;
      TableData[noColumns].rightBorder = FALSE;
      TableData[noColumns].justification = 'l';
      TableData[noColumns].width = 2000; // Estimate
      TableData[noColumns].absWidth = FALSE;
//      TableData[noColumns].spacing = ??
      noColumns ++;
      i ++;
      isBorder = FALSE;
    }
    else if (ch == 'c')
    {
      TableData[noColumns].leftBorder = isBorder;
      TableData[noColumns].rightBorder = FALSE;
      TableData[noColumns].justification = 'c';
      TableData[noColumns].width = defaultTableColumnWidth; // Estimate
      TableData[noColumns].absWidth = FALSE;
//      TableData[noColumns].spacing = ??
      noColumns ++;
      i ++;
      isBorder = FALSE;
    }
    else if (ch == 'r')
    {
      TableData[noColumns].leftBorder = isBorder;
      TableData[noColumns].rightBorder = FALSE;
      TableData[noColumns].justification = 'r';
      TableData[noColumns].width = 2000; // Estimate
      TableData[noColumns].absWidth = FALSE;
//      TableData[noColumns].spacing = ??
      noColumns ++;
      i ++;
      isBorder = FALSE;
    }
    else if (ch == 'p')
    {
      i ++;
      int j = 0;
      char numberBuf[50];
      ch = value[i];
      if (ch == '{')
      {
        i++;
        ch = value[i];
      }
        
      while ((i < len) && (isdigit(ch) || ch == '.'))
      {
        numberBuf[j] = ch;
        j ++;
        i ++;
        ch = value[i];
      }
      // Assume we have 2 characters for units
      numberBuf[j] = value[i];
      j ++; i++;
      numberBuf[j] = value[i];
      j ++; i++;
      numberBuf[j] = 0;
      if (value[i] == '}') i++;
      
      TableData[noColumns].leftBorder = isBorder;
      TableData[noColumns].rightBorder = FALSE;
      TableData[noColumns].justification = 'l';
      TableData[noColumns].width = 20*ParseUnitArgument(numberBuf);
      TableData[noColumns].absWidth = TRUE;
//      TableData[noColumns].spacing = ??
      noColumns ++;
      isBorder = FALSE;
    }
    else
    {
      char *buf = new char[strlen(value) + 80];
      sprintf(buf, "Tabular first argument \"%s\" too complex!", value);
      OnError(buf);
      delete[] buf;
      return FALSE;
    }
  }
  if (isBorder)
    TableData[noColumns-1].rightBorder = TRUE;
  return TRUE;
}
