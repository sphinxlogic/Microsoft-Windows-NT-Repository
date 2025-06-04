/*
 * File:	test.cc
 * Purpose:	Chart demo
 * Author:	Neil Dudman
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */
static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation "dataset.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include "dataset.h"
// *********** wxDataSet class ***********************
wxDataSet::wxDataSet(int dType, wxPen *lStyle, wxBrush *dStyle, 
		 int dWidth, int lWidth, int dGap, int pMark, int disValues)
{
	dataType = dType; lineStyle = lStyle;	dataStyle = dStyle;	dataWidth = dWidth;
	lineWidth = lWidth;	dataGap = dGap;	pointMark = pMark;	displayValues = disValues; 
	dataRow = -1;	display = TRUE; rowsList = NULL;
}

wxDataSet::wxDataSet(int dType)
{                                               
	dataType = dType; 
	lineStyle = NULL;	dataStyle = NULL;	dataWidth = NULL;
	lineWidth = NULL;	dataGap = NULL;	pointMark = NULL;	
	displayValues = NULL; dataRow = -1;	display = TRUE; rowsList = NULL;
}
wxDataSet::~wxDataSet(){}   
     /*
int wxDataSet::NoRows(void)
{
	if(dataRow2=-1)
		return 1;
	else
		return 2;
}  */
void wxDataSet::SetDataType(int type){dataType = type;}

void wxDataSet::SetDataStyle(wxBrush *brush){dataStyle = brush;}
void wxDataSet::SetDataWidth(int width){dataWidth = width;}
void wxDataSet::SetDataGap(int gap){dataGap = gap;}

void wxDataSet::SetLineStyle(wxPen *style){lineStyle = style;}
void wxDataSet::SetPointMark(int mark){pointMark = mark;}
                        
int 			wxDataSet::GetDataType(){return dataType;}
wxBrush 	*wxDataSet::GetDataStyle(){return dataStyle;}
int 			wxDataSet::GetDataWidth(){return dataWidth;}
int 			wxDataSet::GetDataGap(){return dataGap;}
wxPen 		*wxDataSet::GetLineStyle(){return lineStyle; }
int 			wxDataSet::GetPointMark(){return pointMark;}