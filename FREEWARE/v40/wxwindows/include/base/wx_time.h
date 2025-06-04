/*
 * File:	wx_time.h
 * Purpose:	wxTime class, from NIHCL
 * Author:	Julian Smart, after K. E. Gorlen
 * Created:	1995
 * Updated:	
 * Copyright:	Portions (c) 1995, AIAI, University of Edinburgh
 */

#ifndef wx_timeh
#define wx_timeh

#include "wx_obj.h"

#if USE_TIMEDATE

class wxDate;

typedef unsigned short hourTy;
typedef unsigned short minuteTy;
typedef unsigned short secondTy;
typedef unsigned long clockTy;

class WXDLLEXPORT wxTime: public wxObject
{
        DECLARE_DYNAMIC_CLASS(wxTime)
        
public:                 // type definitions
	enum tFormat { wx12h, wx24h };
	enum tPrecision { wxStdMinSec, wxStdMin };
private:
		  static tFormat		Format;
		  static tPrecision	Precision;

		  clockTy		sec;			/* seconds since 1/1/1901 */

        Bool IsDST() const;
        wxTime GetLocalTime() const;
private:                // static member functions
        static wxTime GetLocalTime(const wxDate& date, hourTy h=0, minuteTy m=0, secondTy s=0);
        static wxTime GetBeginDST(unsigned year);
        static wxTime GetEndDST(unsigned year);
public:
        wxTime();                         // current time 
        wxTime(clockTy s)                 { sec = s; }
        wxTime(const wxTime& t)                 { (*this) = t ; }
		  wxTime(hourTy h, minuteTy m, secondTy s =0, Bool dst =FALSE);
		  wxTime(const wxDate&, hourTy h =0, minuteTy m =0, secondTy s=0, Bool dst =FALSE);
		  // Convert to string
        operator char *   (void);
        operator wxDate() const;
        Bool operator<(const wxTime& t) const     { return sec < t.sec; }
        Bool operator<=(const wxTime& t) const    { return sec <= t.sec; }
        Bool operator>(const wxTime& t) const     { return sec > t.sec; }
        Bool operator>=(const wxTime& t) const    { return sec >= t.sec; }
        Bool operator==(const wxTime& t) const    { return sec == t.sec; }
        Bool operator!=(const wxTime& t) const    { return sec != t.sec; }
        void operator=(const wxTime& t)     { sec = t.sec; }
        friend wxTime operator+(const wxTime& t, long s)    { return wxTime(t.sec+s); }
		  friend wxTime operator+(long s, const wxTime& t)    { return wxTime(t.sec+s); }
		  long operator-(const wxTime& t) const     { return sec - t.sec; }
		  wxTime operator-(long s) const    { return wxTime(sec-s); }
        void operator+=(long s)         { sec += s; }
        void operator-=(long s)         { sec -= s; }
        Bool IsBetween(const wxTime& a, const wxTime& b) const;
        hourTy GetHour() const;            // hour in local time 
        hourTy GetHourGMT() const;         // hour in GMT 
        minuteTy GetMinute() const;        // minute in local time 
        minuteTy GetMinuteGMT() const;     // minute in GMT 
        secondTy GetSecond() const;        // second in local time or GMT 
        clockTy GetSeconds() const         { return sec; }
        wxTime Max(const wxTime&) const;
		  wxTime Min(const wxTime&) const;
		  static void SetFormat(const tFormat lFormat			= wx12h,
		  								const tPrecision lPrecision	= wxStdMinSec);
		  char *FormatTime() const;
/*
        virtual int compare(const Object&) const;
        virtual void deepenShallowCopy();       // {}
        virtual unsigned hash() const;
        virtual Bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual const Class* species() const;
*/
};

#endif
  // USE_TIMEDATE
#endif

