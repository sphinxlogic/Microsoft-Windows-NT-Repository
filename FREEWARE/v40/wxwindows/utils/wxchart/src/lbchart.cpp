/*
 * File:	lbchart.cpp
 * Purpose:	Line Bar Chart class 
 * Author:	Neil Dudman
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */
 
static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation "lbchart.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include "lbchart.h"                        


// ******** wxBarGraph class code **********

//IMPLEMENT_ABSTRACT_CLASS(wxChartLayout, wxObject)

wxChartLayout::~wxChartLayout(){}

wxChartLayout::wxChartLayout(wxDC *dc,int noCols)
{ 
  numOfDataSets = 0; threeD = TRUE; colSpace = 0; minorInc = 1;	majorInc = 5;
  centerXChart = centerYChart = 0;  chartDC = dc; chartTitleFont = NULL;
  maxWidth = 300;  maxHeight = 200;  edgeBottom = 40;  edgeTop = 20;  edgeLeft = 40,
  edgeRight = 20; clusterCount = 0;	clusterNo = 1; showValues = FALSE;  
  orientation = wxVERTICAL;	trailingText = ""; leadingText = ""; chartTitle = NULL;
	inOutNone = wxTickOut; 	     
	
	dataSetOrder = new int[MAXDATASETS];	
	
	// initialize array of datasets 
	for(int i=0; i<MAXDATASETS ; i++)
	{
		dataSetArray[i]=NULL;
		dataSetOrder[i]=i;
	}	
	
	InitializeChart();		
} 

void wxChartLayout::InitializeChart()
{	
	SetChartBounds();
}                  

void wxChartLayout::SetChartCenter(int x, int y)
{ 
	centerXChart = x; centerYChart = y;
}

void wxChartLayout::SetMaxWidthHeight(int x,int y)
{                
	/*yMax = y;
	xMax = x-10;*/
	maxWidth = x;
	maxHeight = y;
}          


void wxChartLayout::SetChartBounds()
{
	top = centerYChart - (maxHeight/2) + edgeTop;
	bottom = centerYChart + (maxHeight/2) - edgeBottom;
	left = centerXChart - (maxWidth/2) + edgeLeft;
	right = centerXChart + (maxWidth/2) - edgeRight;
}

void wxChartLayout::SetEdgeTop(int t)
{
	edgeTop = t;
}

void wxChartLayout::SetEdgeBottom(int b)
{
	edgeBottom = b;
}

void wxChartLayout::SetEdgeLeft(int l)
{
	edgeLeft = l;
}

void wxChartLayout::SetEdgeRight(int r)
{
	edgeRight = r;
}

void wxChartLayout::Draw(void)
{             
	clusterNo = 0; clusterCount = 0;
	chartDC->SetClippingRegion((float)centerXChart - (float)(maxWidth/2), 
	(float)centerYChart - (maxHeight/2), 
	(float)maxWidth-2.0, (float)maxHeight-2.0);
        int width,height;  
	SetChartBounds() 	;
	
	// Insure that no brushes are selected into dc, brush is not transparent
	chartDC->SetBrush(NULL);
	
 	if (chartDC)
 	{ 
 		chartDC->Clear(); 
 		
 		//set the title of Chart
		float x,y;
		if(chartTitleFont) chartDC->SetFont(chartTitleFont);	
	  chartDC->GetTextExtent(chartTitle,&x,&y);
	  chartDC->DrawText(chartTitle,centerXChart-(x/2),top - (y/2));  chartDC->SetFont(NULL);
		
		float height,width;       		  
		maxValue =1;
                int ii;
                int i;
		for(ii=0; ii<MAXDATASETS; ii++)
	  		for(i=startCol; i<endCol; i++) 
		  		if (GetData(i,ii) > maxValue) // always uses row 1 to scale data(fix)
	   				maxValue = GetData(i,ii);
    if(orientation==wxVERTICAL)
    {
	  	// Scaling values,        	   				
	  	//if (maxValue!=0)  
	  	stepSize = ( maxHeight - (edgeBottom + edgeTop) ) / maxValue; 	  	
	  	//	  
	  	// Scaling columns
			if (numCols!=0)
				colWidth = (maxWidth - (edgeLeft + edgeRight) - (numCols * colSpace) ) / numCols;
    } 
    else if (orientation==wxHORIZONTAL)
    {                               
		 	// Scaling values,         	  
	  	//if (maxValue!=0)  
	  	stepSize = ( maxWidth - (edgeLeft + edgeRight) ) / maxValue;
	  		  	
	  	// Scaling columns
			if (numCols!=0)                                                                  
				colWidth =  (maxHeight - (edgeTop + edgeBottom) - (numCols  * colSpace) ) / numCols;
		}                             
		
		// See how many of the dataSets chart types are wxBars for generating cluster bars
		int clusterRowArray[MAXDATASETS];
                for(i=0; i<MAXDATASETS; i++)	clusterRowArray[i] = 0;
		for(i=0; i<MAXDATASETS;	i++)
		{
			if(dataSetArray[i] && dataSetArray[i]->GetShow())                                     
				if(dataSetArray[i]->GetDataType()==wxBar)
				{
					clusterRowArray[clusterCount] = dataSetArray[i]->GetRowNo();
					clusterCount++;                    				
				}
		}		
				
  	// For each DataSet.	             
  	int num;
		//for (int a=0; a<MAXDATASETS ; a++)
		for(int a=0; a<MAXDATASETS; a++)
		{ //num = dataSetOrder[a];
			if(dataSetArray[a] && dataSetArray[a]->GetShow())
			{                                                        
				//int row = dataSetArray[a]->GetRowNo1();
			  //int row2 = dataSetArray[a]->GetRowNo2();
				                                
				wxBrush *dataBrush = dataSetArray[a]->GetDataStyle();
				if (dataBrush) chartDC->SetBrush(dataBrush);
				else chartDC->SetBrush(wxCYAN_BRUSH); // default colour
				
				wxPen *dataPen = dataSetArray[a]->GetLineStyle();
				if (dataPen)	chartDC->SetPen(dataPen);
				else  chartDC->SetPen(wxRED_PEN);		
			
				int datatype = dataSetArray[a]->GetDataType();			
				switch (datatype){
					case wxBar:
						if(clusterCount>1)
							DrawClusterBars(clusterRowArray);
						else
							DrawBars(a);
						break;	
					case wxLine:
						DrawLine(a);
						break;
					case wxCurve:
						DrawCurve(a);
						break;
					case wxArea:
						DrawArea(a);
							break;
					case wxBarFloating:
						DrawBarFloating(a);
						break;                             
					case wxPie:
						DrawPie(a);
						break;
					case wxPercentBar:
						DrawPercentBars(a);
						break;
					case wxXYPlot:
						DrawXYPlot(a);
						break;
				}
			}
		}                            
		// return to def brush and pen for chartDC
		chartDC->SetPen(NULL);
		chartDC->SetBrush(NULL);  
	  
	  DrawAxis();
	}
	chartDC->DestroyClippingRegion();
}     

void wxChartLayout::DrawClusterBars(int *rows)
{
	for(int i=startCol; i<=endCol; i++)
		for(int a = 0; a<clusterCount; a++)
		{ 
			wxBrush *dataBrush = dataSetArray[a]->GetDataStyle();
			if (dataBrush) chartDC->SetBrush(dataBrush);
			else chartDC->SetBrush(wxCYAN_BRUSH); // default colour
			
			clusterNo = a;
			DrawBar(i,rows[a]);			
		}
}
void wxChartLayout::DrawBars(int a)
{          	                      
	int row = dataSetArray[a]->GetRowNo();
	if(row<0) 
	{
		wxNode *node = dataSetArray[a]->rowsList->First();
		row = (int)node->Data();
	}		
	for(int i=startCol; i<=endCol; i++)
			DrawBar(i,row);	   
	clusterNo++;
}

void wxChartLayout::DrawBar(int no,int row)
{                                              
	if (chartDC)
  { chartDC->BeginDrawing();   		                            
  	float clusterWidth = colWidth/clusterCount;
  	float widthx,heighty;
  	char label[50];
    float h,w,x,y;
  	if(orientation==wxVERTICAL)
		{
   		h = GetData(no,row) * stepSize;
   		w = clusterWidth;
   		y = bottom - h;
   		if(clusterCount>1)
   			x = left +  (clusterCount * no * clusterWidth) +
   								(clusterNo * clusterWidth) + 
   								(colSpace * no);
   		else
   			x = left + ((colWidth+colSpace) * no);
			chartDC->DrawRectangle(x,y,w,h);      
   	} else if (orientation==wxHORIZONTAL)
  	{
  		w = GetData(no,row) * stepSize;
   		h = colWidth;
   		x = left; 
   		y = top + ((colWidth+colSpace) * no);
   		chartDC->DrawRectangle(x,y,w,h);
		}              
		if(showValues)                     
		{
			wxFont *font2 = 
					wxTheFontList->FindOrCreateFont(clusterWidth/15,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE);
			
			chartDC->SetFont(font2);
			sprintf(label,"%.1f",GetData(no,row));
			chartDC->GetTextExtent(label,&widthx,&heighty);
			chartDC->DrawText(label,x + (clusterWidth/2) - (widthx/2), y + (h/2));					
		}
 		if(threeD)
 		  Draw3DBar(x,y,h,w,clusterWidth);
 		chartDC->EndDrawing();   		                            
	}
}         
                  
void wxChartLayout::DrawBarFloating(int a)
{ int row2=-1,row=-1;
	wxNode *node = dataSetArray[a]->rowsList->First();
	row = (int)node->Data();	
	node = node->Next();
	row2 = (int)node->Data();

	chartDC->BeginDrawing();   		                            	
	if (chartDC && row!=-1 && row!=-1)
  { 
  	for(int i=startCol; i<=endCol; i++)
		{
			float h,w,x,y;
  		if(orientation==wxVERTICAL)
			{	
	   		h = ( GetData(i,row2) * stepSize) - ( GetData(i,row) * stepSize );
   			w = colWidth;
   			y = bottom - h - ( GetData(i,row) * stepSize );
   			x = left + ((colWidth+colSpace) * i);
   			chartDC->DrawRectangle(x,y,w,h);
  		} else if (orientation==wxHORIZONTAL)
  		{
	  		w = ( GetData(i,row2) * stepSize) - ( GetData(i,row) * stepSize );
   			h = colWidth;
   			x = left + ( GetData(i,row) * stepSize);
   			y = top + ((colWidth+colSpace) * i);
   			chartDC->DrawRectangle(x,y,w,h);
			}                                 
			if(threeD)
   				Draw3DBar(x,y,h,w,colWidth);                   		
   	}
	} 
	chartDC->EndDrawing();   		                            
}

void wxChartLayout::DrawLine(int a)
{	float x,y;                      
	int row = dataSetArray[a]->GetRowNo();
	if(row<0) 
	{
		wxNode *node = dataSetArray[a]->rowsList->First();
		row = (int)node->Data();
	}		
	if(chartDC)
	{
		wxPoint points[MAXCOLS];;
		if(orientation==wxVERTICAL)
		{
			for(int i=startCol; i<=endCol; i++)                                         
			{
				points[i].x = left + ( (colWidth + colSpace) * i) + (colWidth/2);
				points[i].y =  bottom - ( GetData(i,row) * stepSize);
			}         
			chartDC->DrawLines(i,points);
		}
		else if (orientation==wxHORIZONTAL)
		{                                 
			for(int i=startCol; i<=endCol; i++)                                         
			{
				points[i].y = top + ( (colWidth + colSpace) * i) + (colWidth/2);
				points[i].x = left + ( GetData(i,row) * stepSize);	
			}         
			chartDC->DrawLines(i,points);
		}
	}                   
}

void wxChartLayout::DrawCurve(int a)
{ 
	int row = dataSetArray[a]->GetRowNo();
	if(row<0) 
	{
		wxNode *node = dataSetArray[a]->rowsList->First();
		row = (int)node->Data();
	}		
	if(chartDC)
	{ float x,y;
		wxPoint *points;
		wxList pointList;
		if(orientation==wxVERTICAL)
		{
			for(int i=startCol; i<=endCol; i++)                                         
			{
				x = left + ( (colWidth + colSpace) * i) + (colWidth/2);
				y = bottom - (GetData(i,row) * stepSize) ;	
				points =  new wxPoint(x,y);
				pointList.Append(points);
			}         
			chartDC->DrawSpline(&pointList);
		}
		else if (orientation==wxHORIZONTAL)
		{                                 
			for(int i=startCol; i<=endCol; i++)                                         
			{
				y = top + ( (colWidth + colSpace) * i) + (colWidth/2);
				x = left + ( GetData(i,row) * stepSize);	
				points =  new wxPoint(x,y);
				pointList.Append(points);
			}         
			chartDC->DrawSpline(&pointList);
		}
	}                              
		
} 

void wxChartLayout::DrawXYPlot(int a)
{                                   
	int row = dataSetArray[a]->GetRowNo();
	if(row<0) 
	{
		wxNode *node = dataSetArray[a]->rowsList->First();
		row = (int)node->Data();
	}		
	if(chartDC)
	{ 
		float x,y;                     
		int radius = 10;
		if(orientation==wxVERTICAL)
		{
			for(int i=startCol; i<=endCol; i++)                                         
			{
				x = left + ( (colWidth + colSpace) * i) + (colWidth/2);
				y =  bottom - ( GetData(i,row) * stepSize);
				chartDC->BeginDrawing();   		                            
				chartDC->DrawEllipse(x+radius, y+radius, radius, radius);
				chartDC->EndDrawing();   		                            
			}
		}
		else if (orientation==wxHORIZONTAL)
		{                                 
			for(int i=startCol; i<=endCol; i++)                                         
			{
				y = top + ( (colWidth + colSpace) * i) + (colWidth/2);
				x = left + ( GetData(i,row) * stepSize);	
				chartDC->BeginDrawing();   		                            
				chartDC->DrawEllipse(x+radius, y+radius, radius, radius);
				chartDC->EndDrawing();   		                            
			}
		}
	}                   
}

void wxChartLayout::DrawArea(int a)
{ int s = 0;                      
	int row = dataSetArray[a]->GetRowNo();
	if(row<0) 
	{
		wxNode *node = dataSetArray[a]->rowsList->First();
		row = (int)node->Data();
	}		    
	if(chartDC)
	{
		wxPoint points[MAXCOLS];
		if(orientation==wxVERTICAL)
		{
			points[s].x = left + (colWidth/2);
			points[s].y = bottom;			
			for(int i=startCol, s=1 ; i<=endCol+1; s++, i++)                                         
			{
				points[s].x = left + ( (colWidth + colSpace) * i) + (colWidth/2);
				points[s].y = bottom - (GetData(i,row) * stepSize);	
			}                                     
			points[i].x = points[i-1].x;
			points[i].y = bottom;			                              
			chartDC->BeginDrawing();   		                            
			chartDC->DrawPolygon(s,points);                         
			chartDC->EndDrawing();   		                            
		}
		else if (orientation==wxHORIZONTAL)
		{  
			points[s].x = left;
			points[s].y = top;
			for(int i=startCol, s=1; i<=endCol+1; i++, s++)                                         
			{
				points[s].y = top + ( (colWidth + colSpace) * i) + (colWidth/2);
				points[s].x = left + ( GetData(i,row) * stepSize);	
			}         
			points[i].x = left;
			points[i].y = bottom;
			chartDC->BeginDrawing();   		                            
			chartDC->DrawPolygon(s,points);
			chartDC->EndDrawing();   		                            
		}
	}                   
}
void wxChartLayout::DrawPercentBars(int a)
{ char colourArray[20][30]={"BLUE","RED","YELLOW",
	 "DARK OLIVE","ORANGE","LIGHT BLUE",
	 "GREEN YELLOW","LIME GREEN","RED","THISTLE","TURQUOISE",
	 "VIOLET","YELLOW","WHEAT","SLATE BLUE"};
	wxBrush *brush;												 
  int  sum = 0, row;  
	int  relrow=0;	
	float oldh, x, val;
	wxNode *node = dataSetArray[a]->rowsList->First();
	while(node)
	{	
		sum = 0; val = 0.0; x = 0; 
		if(orientation==wxVERTICAL) oldh = bottom;
		else if(orientation==wxHORIZONTAL) oldh = left;
		row = (int)node->Data();
		
		// Find the sum of all values in this row ( to be bar).	
		for (int i = startCol; i<=endCol; i++){	val = GetData(i,row);	sum = sum + val;}		                    
	
		for (i = startCol; i<=endCol; i++)
		{	
			val = GetData(i,row) + val;                    
			int bb = maxValue*stepSize;
			if(orientation==wxVERTICAL)
				x = bottom - (((float)val / sum) * bb);
			else if(orientation==wxHORIZONTAL)
				x = left + (((float)val / sum) * bb);
			wxDebugMsg("bott=%d,X==%d value=%d,sum=%d, bb=%d\n",(int)bottom,(int)x,(int)val,sum,bb);
			
			brush = wxTheBrushList->FindOrCreateBrush(colourArray[i],wxSOLID);
			chartDC->SetBrush(brush);
		
			chartDC->SetBrush(brush);
			DrawPercentBar(x, oldh, relrow, val );		
			oldh = x;
		}        
		  
		relrow++;                             
		node = node->Next();
	}
}
		
// a floating bar (will convert clusterDraw)
void wxChartLayout::DrawPercentBar(float hi, float oldh, int no, int value)
{                                              
	if (chartDC)
  { float widthx,heighty;
  	char label[50];
    float h,w,x,y;                                          
    chartDC->BeginDrawing();   		                            
  	if(orientation==wxVERTICAL)
		{       
			h = oldh-hi;
			y = hi;
   		w = colWidth;
   		x = left + ((colWidth+colSpace) * no);
			chartDC->DrawRectangle(x,y,w,h);      
   	} else if (orientation==wxHORIZONTAL)
  	{              
  		w = hi-oldh;
			y = hi;
   		h = colWidth;
   		y = top + ((colWidth+colSpace) * no);
   		chartDC->DrawRectangle(x,y,w,h);
		} 
		 		
		if(showValues)                     
		{ wxFont *font2 = 
					wxTheFontList->FindOrCreateFont(colWidth/15,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE);
			chartDC->SetFont(font2);
			sprintf(label,"%.1f",value);                        
			chartDC->GetTextExtent(label,&widthx,&heighty);
			chartDC->DrawText(label,x + (colWidth/2) - (widthx/2), y + (h/2));					
		}
		chartDC->EndDrawing();   		                            
 		if(threeD)
 		  Draw3DBar(x,y,h,w,colWidth);
	}
}   

void wxChartLayout::DrawPie(int row)
{ char colourArray[20][30]={"BLUE","RED","YELLOW",
			 "DARK OLIVE","ORANGE","LIGHT BLUE",
			 "GREEN YELLOW","LIME GREEN","RED","THISTLE","TURQUOISE",
			 "VIOLET","YELLOW","WHEAT","SLATE BLUE"};
	wxBrush *brush;												 
  float radius = (maxWidth)/4;         
	int val, fromx=0,fromy=0, tox, toy, sum = 0;  
	float toAngle=0.0;
	for (int i = startCol; i<=endCol; i++)
	{	val = GetData(i,row);		
		sum = sum + val;
	}	
	for (i = startCol; i<=endCol; i++)
	{
		val = GetData(i,row);
		toAngle = toAngle + (360 * ((float)val / sum));
		tox = cos( toAngle/57.29577951) * radius;
		toy = sin( toAngle/57.29577951) * radius;
		brush = wxTheBrushList->FindOrCreateBrush(colourArray[i],wxSOLID);
		chartDC->SetBrush(brush);
		
		wxPoint points[4];
		//if(threeD)		stuff for 3D pie (any sugestions)
/*			if(toAngle > 0 && toAngle < 180)
			{ 
				chartDC->DrawArc(cx + tox, cy + toy + 20, cx + fromx, cy + fromy + 20, cx,cy+20);							
				chartDC->SetBrush(wxWHITE_BRUSH);             	                                    
				chartDC->DrawArc(cx + tox, cy + toy, cx + fromx, cy + fromy, cx,cy);
			}	             */
		chartDC->SetBrush(brush);		
		chartDC->BeginDrawing();                                           		
		chartDC->DrawArc(centerXChart + tox, centerYChart + toy, 
										 centerXChart + fromx, centerYChart + fromy, 
										 centerXChart, centerYChart);			
		chartDC->EndDrawing();   		                            										 
		fromx = tox;
		fromy = toy;		
	}	                
	/*if(showValues)                     
	{
		wxFont *font2 = 
				wxTheFontList->FindOrCreateFont(clusterWidth/15,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE);
			
		chartDC->SetFont(font2);
		sprintf(label,"%.1f",GetData(no,row));
		chartDC->GetTextExtent(label,&widthx,&heighty);
		chartDC->DrawText(label,x + (clusterWidth/2) - (widthx/2), y + (h/2));					
	} */
	
	chartDC->SetBrush(NULL);	
}

void wxChartLayout::Draw3DBar(float x, float y, int h, int w, int dep)
{
 wxPoint ptop[4], pside[4];                                          
 int depx = dep;
 int depy = dep;
	if(orientation==wxVERTICAL)
	{
		ptop[0].x = x; 								ptop[0].y = y;
  	ptop[1].x = x + depx;		 			ptop[1].y = y; 
  	ptop[2].x = x + (depx*2); 		ptop[2].y = y - depy;
  	ptop[3].x = x + depx; 				ptop[3].y = y - depy;
  	pside[0].x = ptop[1].x;				pside[0].y = ptop[1].y;
  	pside[1].x = ptop[2].x;				pside[1].y = ptop[2].y;
	  pside[2].x = ptop[2].x;				pside[2].y = y + h - depy;
	  pside[3].x = x + w;				pside[3].y = y + h;
  }else if (orientation==wxHORIZONTAL)
  {
  	ptop[0].x = x; 								ptop[0].y = y;
  	ptop[1].x = x + w;		 				ptop[1].y = y; 
  	ptop[2].x = x + w + depx; 		ptop[2].y = y - depy;
  	ptop[3].x = x + depx; 				ptop[3].y = y - depy;
  	pside[0].x = ptop[1].x;				pside[0].y = ptop[1].y;
  	pside[1].x = ptop[2].x;				pside[1].y = ptop[2].y;
	  pside[2].x = ptop[2].x;				pside[2].y = y + h - depy;
	  pside[3].x = x + w;						pside[3].y = y + h;
	}
  chartDC->BeginDrawing();
  chartDC->DrawPolygon(4,ptop);
  chartDC->DrawPolygon(4,pside);   			
  chartDC->EndDrawing();
}          


void wxChartLayout::DrawAxis(void)
{                      
 if(chartDC)
 {            
 	float widthx,heighty;                         
 	char text[20] = "";                           
 	int val=0;
 	float x,y;
 	float minorIncStep = minorInc * stepSize;
	float majorIncStep = majorInc * stepSize;
	char buff[50];		
	if (orientation==wxVERTICAL)       
	{		
		DrawXAxis(left, bottom, right, bottom);
		
		DrawYAxis(left,bottom,left,top);		
		
		// Draw Labels
		for(int i=startCol; i<=endCol; i++)
		{ 
			sprintf(buff,"%f",colWidth/8);
			wxDebugMsg("Colwidth=%s\n",buff);			
						
			wxFont *font2 = 
				wxTheFontList->FindOrCreateFont(colWidth/8,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE);
			chartDC->BeginDrawing();   		                          
			chartDC->SetFont(font2);
			char *label = GetLabel(i);
			sprintf(buff,"%s %s %s",leadingText,label,trailingText);
			chartDC->GetTextExtent(label,&widthx,&heighty);
			if((i%2)==0)
				chartDC->DrawText(label,left + ((colWidth+colSpace) * i) + ( (colWidth/2) - (widthx/2) ),
										        	bottom + (heighty/2));					
			else                                                    
				chartDC->DrawText(label,left + ((colWidth+colSpace) * i) + ( (colWidth/2) - (widthx/2) ),
										        	bottom + heighty);					
		}
		// Major Tick Marks with values
  	for(x=left,y=bottom; y >= top; y-=majorIncStep, val+=majorInc)
  	{
			
			chartDC->DrawLine(x-7,y,x,y); // x was edgeLeft
			sprintf(text,"%i",val);                     
			chartDC->GetTextExtent(text,&widthx,&heighty);          			  
			chartDC->DrawText(text,x-10-widthx,y-(heighty/2));			// x was edgeLeft
		}
		// Minor Tick Marks
		for(x=left,y=bottom; y >= top; y-=minorIncStep, val+=minorInc)
  	{ 
  		if(inOutNone==wxTickOut)
				chartDC->DrawLine(left-3,y,left,y);
			else if(inOutNone==wxTickIn)        
				chartDC->DrawLine(left+3,y,left,y);
		}        
	} else if (orientation == wxHORIZONTAL)
	{	
		DrawXAxis(left, top, left, bottom);
		DrawYAxis(left, bottom, right, bottom);	
		// Draw Labels
		for(int i=startCol; i<=endCol; i++)
		{                         
		/*	wxFont *font2 = 
				wxTheFontList->FindOrCreateFont(colWidth/8,wxDEFAULT,wxNORMAL,wxNORMAL,FALSE);
			chartDC->SetFont(font2);			
			
			char *label = GetLabel(i);                              
			sprintf(buff,"%s %s %s",leadingText,label,trailingText);
			chartDC->GetTextExtent(label,&widthx,&heighty);
			chartDC->DrawText(label,left - widthx - 4, 
					top + ( (colWidth+colSpace) * i) + ( (colWidth/2) - (widthx/2) ) );					*/
		}
		// Major Tick Marks with values
  	for(x=left,y=bottom; x <= right; x+=majorIncStep, val+=majorInc)
  	{
			chartDC->DrawLine(x,y,x,y+7);
			sprintf(text,"%i",val);                     
			chartDC->GetTextExtent(text,&widthx,&heighty);
			chartDC->DrawText(text,x-widthx, y + (heighty/2) );			
		}
		// Minor Tick Marks
		for(x=left,y=bottom; x <= right; x+=minorIncStep)
  	{ 
  		if(inOutNone==wxTickOut)
				chartDC->DrawLine(x,y,x,y+3);
			else if(inOutNone==wxTickIn)   
				chartDC->DrawLine(x,y,x,y-3);
		}        
	}
	chartDC->EndDrawing();   		                            
 }
}   

void wxChartLayout::DrawXAxis(float x, float y, float x1, float y1) 
{                                                         
	chartDC->BeginDrawing();   		                            
	chartDC->DrawLine(x,y,x1,y1);                           
	chartDC->EndDrawing();   		                            
}               
void wxChartLayout::DrawYAxis(float x, float y, float x1, float y1) 
{               
	chartDC->BeginDrawing();   		                            
	chartDC->DrawLine(x,y,x1,y1);
	chartDC->EndDrawing();   		                            
}            
                 
void wxChartLayout::DrawMinorTicks(float x, float y, float x1, float y1){}
void wxChartLayout::DrawMajorTicks(float x, float y, float x1, float y1) {}

void wxChartLayout::SetDepth(int dep)
{
	depth = dep;
}

void wxChartLayout::SetDataStyle(int dset, wxBrush *brush)
{                   
	if(dataSetArray[dset])
		dataSetArray[dset]->SetDataStyle(brush);
}

void wxChartLayout::SetLineStyle(int dset, wxPen *pen)
{                   
	if(dataSetArray[dset])
		dataSetArray[dset]->SetLineStyle(pen);
}

void wxChartLayout::SetChartOrientation(int ori)
{
	  orientation = ori;
	  //wxDebugMsg("Orientation = %d",orientation);
}                   
          
void wxChartLayout::SetColumnGap(int gap)
{
	colSpace = gap;
}
                 
int wxChartLayout::GetColumnGap()
{
	return colSpace;
}
                 
void wxChartLayout::SetStartEndCol(int colStart,int colEnd)
{
         startCol = colStart;
         endCol = colEnd;
         numCols = endCol - startCol;
         if(colStart==0)	numCols ++;
}                                    

void wxChartLayout::SetDC(wxDC *dc)
{
 	  chartDC = dc;         
}

void wxChartLayout::SetTitle(char *title)
{
	chartTitle = title;
}
void wxChartLayout::SetTitleFont(wxFont *font)
{
	chartTitleFont = font;
}                        
char *wxChartLayout::GetTitle(void)
{
	return chartTitle;
}
wxFont *wxChartLayout::GetTitleFont(void)
{                   
	return chartTitleFont;
}                 

void 	wxChartLayout::SetMajorTickInc(int major){	 majorInc = major; }
void 	wxChartLayout::SetMinorTickInc(int minor){  minorInc = minor; }
void 	wxChartLayout::SetTailText(char *tail){	trailingText = tail; }
void 	wxChartLayout::SetHeadText(char *head){  leadingText = head;  }
		
char 	*wxChartLayout::GetTailText(void){	return trailingText; }
char 	*wxChartLayout::GetHeadText(void){	return leadingText; }
int 	wxChartLayout::GetMajorTickInc(int major){ return majorInc; }
int 	wxChartLayout::GetMinorTickInc(int minor){ return minorInc; }

int wxChartLayout::AddDataSet(int dataType,int row)
{                   
	dataSetArray[numOfDataSets] =	new wxDataSet(dataType);
	dataSetArray[numOfDataSets]->SetRowNos(row);
	return numOfDataSets++;
} 
int wxChartLayout::AddDataSet(int dataType, wxList *rows)
{                   
	dataSetArray[numOfDataSets] =	new wxDataSet(dataType);
	dataSetArray[numOfDataSets]->SetRowNos(rows);
	return numOfDataSets++;
} 


// ************* StoredBarLineChart *****************
        
wxStoredBarLineChart::wxStoredBarLineChart(wxDC *dc):
	wxChartLayout(dc,2)
{
	numDataSet = 0;
	for(int i = 0; i<MAXROWS; i++)
 		for(int a = 0; a<MAXCOLS; a++)
 		{	table[i][a].value = 0;
	 		table[i][a].empty = 0;
	 	}	    
	startCol = 0; endCol = 0;
		 	                         
	numCols = endCol-startCol;
}                           

wxStoredBarLineChart::~wxStoredBarLineChart(){}

//GetData
float wxStoredBarLineChart::GetData(int col, int row)
{
    return  table[col][row].value;
}
// SetData
void wxStoredBarLineChart::SetData(int col, int row, float value)
{
  table[col][row].value = value;
 	table[col][row].empty = 0;
}
// SetLabel
void wxStoredBarLineChart::SetLabel(int col, char *la)
{
 	label[col]=la;
}
//GetLabel
char *wxStoredBarLineChart::GetLabel(int col)
{     
	return label[col];
}        
  
		
   
