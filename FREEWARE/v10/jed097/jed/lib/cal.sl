%
%  calender for JED
%
%  It was written to test a mixture of S-Lang RPN and infix notation.
%
%  It pops up a buffer like:

%     Jun 1993		      Jul 1993		       Aug 1993
% S  M Tu  W Th  F  S	  S  M Tu  W Th  F  S	   S  M Tu  W Th  F  S
%       1  2  3  4  5 		      1  *  3 	   1  2  3  4  5  6  7 
% 6  7  8  9 10 11 12 	  4  5  6  7  8  9 10 	   8  9 10 11 12 13 14 
%13 14 15 16 17 18 19 	 11 12 13 14 15 16 17 	  15 16 17 18 19 20 21 
%20 21 22 23 24 25 26 	 18 19 20 21 22 23 24 	  22 23 24 25 26 27 28 
%27 28 29 30 		 25 26 27 28 29 30 31 	  29 30 31 
%
%  The asterix denotes the current day.  
%  The actual computational part of the code presented here is a 
%  translation of cal.el included with the GNU Emacs distribution.
%  (suitably modified to work with 16 bit integers)
%----------------------------------------------------------------------



% parse date
define cal_convert_month(month, type)
{
  variable lis, m, mnth;
  lis =  "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec";
  if (type == 0) return (extract_element(lis, month - 1, ' '));
  for (m = 0; m < 12; ++m)
    { 
      mnth = extract_element(lis, m, ' ');
      if (0 == strcmp(month, mnth)) return (m + 1);
    }
}


define leap_year_p (year)
{
   if ( not(year mod 4) and (year mod 100)) return (1);
   not (year mod 400);
}

define cal_day_number(month, day, year)
{
   variable d;
   d = 31 * ( month - 1 ) + day;
   if (month > 2)
     {
	d = d - (month * 4 + 23) / 10;
	if (leap_year_p (year)) { ++d }
     } 
   d;
}


define cal_day_of_week(month, day, year)
{
   variable c, delta, n, a, b;
   
   n = cal_day_number(month, day, year);
   --year;
   
  a = n + year + year/4;
  c = year/100 * 3; b = 0;
  if (c mod 4) b = 1;

   return (a - (b + c/4)) mod 7;
}


define make_month(month year indent day)
{
   variable first, nm, ny, max, i, istr, m;

   m = cal_convert_month(month, 1);
   
   first = cal_day_of_week(m, 1, year);
   nm = m + 1; ny = year;
   if (nm == 13) max = 31;
   else max = cal_day_number(nm, 1, ny) - cal_day_number(m, 1, year);
   
   ++indent;
   
   bob; goto_column(indent);
   insert("     "); insert(month); insert_single_space; insert(string(year));
   if (1 != down(1)) newline();
   goto_column(indent);
   
   insert(" S  M Tu  W Th  F  S");
   if (1 != down(1)) newline;
   goto_column(first * 3 + indent);
   
   for (i = 1; i <= max; ++i)
     { 
	if (first == 7)
	  {
	     if (down(1) != 1) {eol; newline}
	     goto_column(indent); first = 0;
	  }
	
	istr = string(i);
	if (strlen(istr) == 1) insert_single_space();
	if (day == i) push_spot();
	insert(istr); insert_single_space();
	++first;
     } 
}

%%% strcaps-- returns capitalized string
define strcaps(str)
{
   str = strlow(str);
   strsub(str, 1, int (strup(char(int(str)))));
}

    
define calendar ()
{
   
  variable month, day, year, t, m, nlines, wlines, obuf, default, n;

  n = 0;
  obuf = whatbuf;
  t = time;
  month = extract_element(t, 1, ' ');
  day = extract_element(t, 2, ' ');

   % Some systems display the time as: Tue Jul 06 16:31:18 1993
   % while others use Tue Jul 06 16:31:18 1993
   % this silly bit is a result.
   
   if (strlen(day) == 0)
     { 
       day = extract_element(t, 3, ' ');
       n = 1
     } 
  day = integer(day);

  year = extract_element(t, 4 + n, ' ');

  default = strcat(month, strcat(" ", string(year)));
  t = read_mini("Month Year:", default, Null_String);
  t = strtrim(t);
  month = strcaps(substr(extract_element(t, 0, ' '), 1, 3));
  year = integer(extract_element(t, 1, ' '));
  m = cal_convert_month(month, 1);
   
  pop2buf("*calendar*"); set_readonly(0); erase_buffer();
  --m; if (0 == m) {m = 12; --year}
  cal_convert_month(m, 0); make_month(year, 0, 0);
  ++m; if (m == 13) {m = 1; ++year}
  cal_convert_month(m, 0);  make_month(year, 25, day);
  ++m;  if (m == 13) {m = 1; ++year} 
  cal_convert_month(m, 0);  make_month(year, 50, 0);
   %
   % fix window size
   %
   if (nwindows == 2)
     {
	eob();  bskip_chars("\n\t ");
	nlines = whatline - window_info('r');
	
	if (nlines > 0)
	  {
	     loop (nlines) {call("enlarge_window") }
	  }
	else
	  {
	     call("other_window");
	     loop(- nlines) {call("enlarge_window")}
	     call("other_window");
	  } 
	 bob();
     } 
     
   % find current day
   pop_spot();
   del(); insert_char('*');
   if (isdigit(what_char())) {del(); insert_char('*')}

   set_readonly(1); set_buffer_modified_flag(0);
   bob(); pop2buf(obuf);
   %
   %  what the heck, give current time
   %
   message(time)
}
