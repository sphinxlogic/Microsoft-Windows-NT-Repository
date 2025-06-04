#define MAXDATAROWS 5
#define MAXDATASETS 10
#define MAXCOLS 100        
#define MAXROWS 100  

#define wxNone  1
#define wxFixed 2                 
#define wxBar 3
#define wxLine 4
#define wxArea 5 
#define wxCurve 6 
#define wxBarFloating 7 
#define wxPie 8              
#define wxXYPlot 12
#define wxPercentBar 13

#define wxTickNone 9
#define wxTickOut 10
#define wxTickIn 11

#ifndef wx_linebarcharth
#define wx_linebarcharth
 
#ifdef __GNUG__
#pragma interface
#endif		     
                
#include <math.h>
#include "dataset.h"

class wxChartLayout: public wxObject
{                             
	public:
//int yMax,xMax; // temp
		wxDC *chartDC;                  
		int 		colSpace; 
		float 	colWidth;    
		char 		*chartTitle;
		wxFont 	*chartTitleFont;

		int 		edgeTop;
		int 		edgeBottom;
		int 		edgeLeft;
		int 		edgeRight;   

		int 		*dataSetOrder;
			  
		int 		centerYChart, centerXChart;
		int 		maxWidth, maxHeight;

	 	double 	stepSize;    
	 	int			maxValue;               
	  	
		wxDataSet *dataSetArray[MAXDATASETS]; // array of datasets
		int 		numOfDataSets;           
		
		// Calculated x,y cordinates of outside edges of drawn chart
		// all relative to centerXChart,centerYChart * maxwidth,maxheight
		float			top;
		float			bottom;
		float			left;
		float			right;

		int				numCols;  // of columns
		int 			startCol, endCol;			
		int 			orientation; // wxHORIZONTAL, wxVERTICAL

		// For ticks	
		char 			*leadingText;
		char 			*trailingText;
		int 			minorInc,majorInc;
		int 			inOutNone; //wxTickNone, wxTickOut, wxTickIn 
		Bool			showValues; // weither the values are displayed in side object
		
  	Bool 			threeD;
  	int				depth; // of 3d bit.

		int 			clusterCount; // used for cluster wxBar's
		int 			clusterNo;                                                    
		//float			clusterWidth;
		
		void DrawAxis(void);
		void DrawMajorTicks(float x, float y, float x1, float y1);
		void DrawMinorTicks(float x, float y, float x1, float y1);
		void DrawXAxis(float x, float y, float x1, float y1);
		void DrawYAxis(float x, float y, float x1, float y1);
	//	void DrawColumnLabels(float x, float y)		;
		
		void DrawBar(int colNo,int row); // plus 
		void DrawBars(int a); 
		void DrawLine(int a);
		void DrawCurve(int a);
		void DrawArea(int a);		             
		void DrawBarFloating(int a);    
		void DrawPie(int row);
		void Draw3DBar(float x, float y, int h, int w, int col);		
		void DrawClusterBars(int *rows);          
		void DrawPercentBars(int a);  
		void DrawPercentBar(float h, float oldh, int row, int  value);
		void DrawXYPlot(int a);
		
		void SetChartBounds();
	public:         
		wxChartLayout(wxDC *dc=NULL,int noCols=20);
		~wxChartLayout();     
		void InitializeChart();
		void SetDC(wxDC *dc);	
    
    // Redefine these, is done in wxStoredChart
		virtual float GetData(int col, int row)=0;	
		virtual void SetData(int col, int row, float value)=0;
		virtual char *GetLabel(int col)=0;
		virtual void SetLabel(int col, char *label)=0;                         
		
		void SetTitle(char *title);		
		char *GetTitle(void);	
		void SetTitleFont(wxFont *font);
		wxFont *GetTitleFont(void);
		
		void SetMaxWidthHeight(int x,int y); 
		void SetEdgeTop(int t);
		void SetEdgeBottom(int b);
		void SetEdgeLeft(int l);
		void SetEdgeRight(int r);
		void SetChartCenter(int x, int y);
    //void SetChartBounds();
		
  	// DataSet specific Member functions
   	//int AddDataSet(int dataType, int dataRow1, int dataRow2=-1);
   	int AddDataSet(int dataType, wxList *rows);
   	int AddDataSet(int dataType, int row);
		void SetStartEndCol(int colStart, int colEnd);
		     
		void SetDataSetOrder(int *dataArray){ dataSetOrder = dataArray;}     
		void ShowDataSet(Bool show,int dataset) 
		{	if(dataSetArray[dataset])
				dataSetArray[dataset]->Show(show);
		}
		Bool GetShowDataSet(int dataset)
		{	if(dataSetArray[dataset])
				return dataSetArray[dataset]->GetShow();
			else 
				return 1;
		}
		
		// Tick accessors
		void SetMajorTickInc(int major);		          
		int GetMajorTickInc(int major);		          
		void SetMinorTickInc(int minor);
		int GetMinorTickInc(int minor);		
		void SetTailText(char *tail);
		char *GetTailText(void);
		void SetHeadText(char *head);
		char *GetHeadText(void);		   
		void SetTickInOutNone(int inoutnone) { inOutNone = inoutnone; }
		int	 GetTickInOutNone() { return inOutNone; }
		
		void ShowValues(Bool show){ showValues = show;}
		Bool GetShowValues() { return showValues;}
		
		// wxDataSet accessors more to be added
		void SetChartOrientation(int ori=wxVERTICAL);// wxHOR,wxVER
		int  GetChartOrientation() { return orientation;}
		void SetDataType(int dset,int dType);
		void SetLineStyle(int	dset, wxPen *lStyle);
		void SetDataStyle(int dset, wxBrush *dStyle);
		
    void SetColumnGap(int gap);
    int  GetColumnGap();
    
		// 3D Characteristics
		void SetDepth(int depth);
		Bool Get3D() { return threeD;}
		void Set3D(Bool ans=FALSE){ threeD = ans;}
		
		virtual void Draw(void);
};	

class Table 
{ public:
	float value;
	int empty;
};	
                                   
class wxStoredBarLineChart: public wxChartLayout
{ private:
    Table 			table[MAXROWS][MAXCOLS];
    // Use a range of start_col to end_col to specify the columns to use in chart
    char				*label[MAXCOLS];// was label[MAXCOLS][20]
    int 				numDataSet;
 	public:   
		wxStoredBarLineChart(wxDC *dc=NULL);
		~wxStoredBarLineChart();                   
		//Initialize();		

		// Data Functions & Data Headings
		virtual float GetData(int col, int row);	
		virtual void SetData(int col, int row, float value);
		virtual char *GetLabel(int col);
		virtual void SetLabel(int col, char *label);	
};
#endif // wx_linebarcharth
