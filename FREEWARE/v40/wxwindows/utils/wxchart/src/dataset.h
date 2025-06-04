#ifndef wx_dataseth
#define wx_dataseth
 
#ifdef __GNUG__
#pragma interface
#endif		       

#include <wx.h>
                               
class wxDataSet: public wxObject
{
	protected:           
		int 		dataType; 
		wxBrush	*dataStyle;
		wxPen		*lineStyle;
		int			dataWidth; // Scaled|Fixed
		int			lineWidth;
		int			dataGap;		// between bars .etc.
		int 		pointMark;
		int			displayValues; // def=NONE |IN | OUT 
		
		int 		dataRow;
				
		Bool		display;
	
	public:       
		wxList	*rowsList; 
		wxDataSet(int dType,wxPen *lStype, wxBrush *dStyle, 
							int dWidth, int lWidth, int dGap, int pMark, 
							int disValues);
		wxDataSet(int dType);
		~wxDataSet();
		      		 
		//void SetRowNos(int rows1,int rows2=-1);
		void SetRowNos(int row){ dataRow = row; }		
		void SetRowNos(wxList *rows){ rowsList = rows;}		
		int GetRowNo(void) { return dataRow;}
	//int NoRows();

		void SetDataType(int type);
		void SetDataStyle(wxBrush *brush);
		void SetDataWidth(int width);
		void SetDataGap(int gap);
		void SetLineStyle(wxPen *style);
		void SetPointMark(int mark);
	//	void SetDisplayValues(int disValues);
		
		Bool GetShow(){return display;}
		void Show(Bool show){display=show;}
		
		int GetDataType();
		wxBrush *GetDataStyle();
		int GetDataWidth();
		int GetDataGap();
		wxPen *GetLineStyle();
		int GetPointMark();
//		void GetDisplayValues();		
};
#endif
 // dataseth
