$! file identifier  .......  HOLIDAYS.COM
$!
$  date = p1
$!
$! Make sure the procedure is not aborted on errors ...  :-)
$! (The subroutine call may return an "E" or "F" as severity code!)
$!
$  SET NOON         ! turn off error handling ...
$!
$  CALL -
      check_date -
      "''date'"
$!
$  status = $STATUS
$!
$  SET ON           ! turn on error handling again ...
$!
$  WRITE SYS$OUTPUT -
      F$MESSAGE(status)
$!
$! Take appropriate action depending on return value of subroutine call ...
$!
$  EXIT
$!
$! ============================================================================
$! S U B - R O U T I N E S
$! ============================================================================
$!
$check_date: SUBROUTINE
$!
$! author  ................  Michael Unger, Stuttgart, Germany
$!                           (unger@decus.de) for the DCL part,
$!                           others for the Gaussian easter formula
$!                           as mentioned below
$! copyright notice  ......  this subroutine may be used free of charge
$!                           for non-commercial purposes
$! creation  ..............  18-MAY-1999
$! last modification  .....  21-MAY-1999
$!
$! purpose  ...............  detection of legal holidays for a given date
$!                           (german holidays may vary from state to state)
$! operating system  ......  OpenVMS (VAX or Alpha)
$! language  ..............  DCL (default CLI)
$! tested platforms  ......  OpenVMS/VAX V 5.5-2
$!                           OpenVMS/VAX V 6.2
$!                           OpenVMS/Alpha V 6.2-1H3 (DECUSnet Alpha, in 1999)
$!                           OpenVMS/Alpha V 7.1 (DECUSnet Alpha, in 2000)
$! compatibility  .........  no known reason for this procedure to break
$!                           running on other/higher versions of OpenVMS
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! parameter:
$! ----------
$! date to be checked (OpenVMS absolute date format);
$! supported range is from 1-JAN-1859 to 31-DEC-9999
$! and explicitly excluding the year 1858
$! [implicitly assuming the Gregorian calender will be used
$! in the far future too]
$!
$! return code:   severity:     event:
$! ------------   -----------   --------------------------
$! %X1FFFFFF8     Warning       (not used)
$! %X1FFFFFF9     Success       date is no holiday
$! %X1FFFFFFA     Error         missing date specification
$! %X1FFFFFFB     Information   date is a legal holiday
$! %X1FFFFFFC     Fatal         date specification error
$!
$! (these message codes are not registered by Digital/Compaq;
$! all bits in facility number and message number are set to "1";
$! display of message is inhibited)
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$  SET NOON
$!
$  date            = F$EDIT(p1, "UPCASE, TRIM")
$  base_exit_code  = %X1FFFFFF8
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! check for valid date specification first ...
$!
$  IF date .EQS. ""
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("ERROR: no date parameter")
$     EXIT base_exit_code + %X00000002
$  ENDIF
$!
$  SET MESSAGE /NOFACILITY /NOSEVERITY /NOIDENTIFICATION /NOTEXT
$!
$  dummy           = F$CVTIME(date, , "DATE")
$  status          = $STATUS
$!
$  SET MESSAGE /FACILITY /SEVERITY /IDENTIFICATION /TEXT
$!
$  IF status .EQ. %X00038290
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("FATAL: invalid date parameter!/ \!AS\", -
               date)
$     EXIT base_exit_code + %X00000004
$  ENDIF
$!
$  IF F$INTEGER(F$CVTIME(date, , "YEAR")) .LT. 1859
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("FATAL: unsupported date parameter (prior to 1859)!/ \!AS\", -
               date)
$     EXIT base_exit_code + %X00000004
$  ENDIF
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$  year            = F$CVTIME(date, , "YEAR")
$  integer_year    = F$INTEGER(year)
$  weekday         = F$CVTIME(date, , "WEEKDAY")
$  comparison_date = F$CVTIME(date, "COMPARISON", "DATE")
$  current_date    = F$TIME()
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$  IF comparison_date .LTS. F$CVTIME(current_date, "COMPARISON", "DATE")
$  THEN
$     verb = "was"
$  ENDIF
$!
$  IF comparison_date .EQS. F$CVTIME(current_date, "COMPARISON", "DATE")
$  THEN
$     verb = "is"
$  ENDIF
$!
$  IF comparison_date .GTS. F$CVTIME(current_date, "COMPARISON", "DATE")
$  THEN
$     verb = "will be"
$  ENDIF
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! check for fixed date holidays ...
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "01-01"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "New Year's Day (Neujahr)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "01-06"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Epiphany (Dreikoenige)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "05-01"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Labour Day (Tag der Arbeit)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "10-03"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "German Unity Day (Tag der Deutschen Einheit)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "11-01"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "All Saints' Day (Allerheiligen)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "12-24"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Christmas Eve (Heiliger Abend)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "12-25"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "First Christmas Day (Erster Weihnachtstag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "12-26"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Second Christmas Day (Zweiter Weihnachtstag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF F$EXTRACT(5, 5, comparison_date) .EQS. "12-31"
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "New Year's Eve (Silvester)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! calculate date of easter sunday ...
$!
$! NOTE: the following formulae are taken from an article of
$! "Physikalisch-Technische Bundesanstalt", Braunschweig, Germany
$! (the national metrology institute providing scientific
$! and technical services); location of the article:
$! http://www.ptb.de/deutsch/org/4/43/432/oste.htm (in German),
$! http://www.ptb.de/english/org/4/43/432/east.htm (in English)
$!
$! quoted reference:
$!
$! Lichtenberg, Dr. Heiner:
$! "Zur Interpretation der Gaussschen Osterformel und ihrer Ausnahmeregeln"
$! [Historia Mathematica 24 (1997), pages 441 to 444]
$! (translated: "On the interpretation of the Gaussian Easter Formula
$! and its exception rules")
$!
$! WARNING: please be aware of _integer_ arithmetics within DCL !!!
$! REMARK:  "x modulo y" is equal to "x - ((x / y) * y)" in DCL
$!
$  x  = integer_year
$!
$  k  = x / 100
$  m  = 15 + ((3 * k + 3) / 4) - ((8 * k + 13) / 25)
$  s  = 2 - ((3 * k + 3) / 4)
$  a  = x - ((x / 19) * 19)
$  d  = (19 * a + m) - (((19 * a + m) / 30) * 30)
$  r  = (d / 29) + ((d / 28) - (d / 29)) * (a / 11)
$  og = 21 + d - r
$  sz = 7 - ((x + (x / 4) + s) - ((x + (x / 4) + s) / 7) * 7)
$  oe = 7 - ((og - sz) - ((og - sz) / 7) * 7)
$  os = og + oe
$!
$! NOTE: please do not ask _me_ what the meaning of the letters is ;-)
$!
$  IF os .GT. 31
$  THEN
$     easter_month = "APR"
$     easter_day   = F$STRING(os - 31)
$     easter_year  = F$STRING(x)
$  ELSE
$     easter_month = "MAR"
$     easter_day   = F$STRING(os)
$     easter_year  = F$STRING(x)
$  ENDIF
$!
$  easter_sunday = easter_day + "-" + easter_month + "-" + easter_year
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! calculate date of holidays depending on easter date ...
$!
$  monday_before_lent = F$CVTIME("''easter_sunday'-48-0", "COMPARISON", "DATE")
$  carnival_tuesday   = F$CVTIME("''easter_sunday'-47-0", "COMPARISON", "DATE")
$  ash_wednesday      = F$CVTIME("''easter_sunday'-46-0", "COMPARISON", "DATE")
$  good_friday        = F$CVTIME("''easter_sunday'-02-0", "COMPARISON", "DATE")
$  easter_monday      = F$CVTIME("''easter_sunday'+01-0", "COMPARISON", "DATE")
$  ascension_day      = F$CVTIME("''easter_sunday'+39-0", "COMPARISON", "DATE")
$  whit_sunday        = F$CVTIME("''easter_sunday'+49-0", "COMPARISON", "DATE")
$  whit_monday        = F$CVTIME("''easter_sunday'+50-0", "COMPARISON", "DATE")
$  corpus_christi_day = F$CVTIME("''easter_sunday'+60-0", "COMPARISON", "DATE")
$!
$  easter_sunday      = F$CVTIME(easter_sunday, "COMPARISON", "DATE")
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! calculate date of holidays depending on christmas date ...
$!
$  first_xmas_day     = "25-DEC-" + year
$  first_xmas_weekday = F$CVTIME(first_xmas_day, , "WEEKDAY")
$!
$  IF first_xmas_weekday .EQS. "Monday"
$  THEN
$     repentence_day = F$CVTIME("''first_xmas_day'-33-0", "COMPARISON", "DATE")
$  ENDIF
$!
$  IF first_xmas_weekday .EQS. "Tuesday"
$  THEN
$     repentence_day = F$CVTIME("''first_xmas_day'-34-0", "COMPARISON", "DATE")
$  ENDIF
$!
$  IF first_xmas_weekday .EQS. "Wednesday"
$  THEN
$     repentence_day = F$CVTIME("''first_xmas_day'-35-0", "COMPARISON", "DATE")
$  ENDIF
$!
$  IF first_xmas_weekday .EQS. "Thursday"
$  THEN
$     repentence_day = F$CVTIME("''first_xmas_day'-36-0", "COMPARISON", "DATE")
$  ENDIF
$!
$  IF first_xmas_weekday .EQS. "Friday"
$  THEN
$     repentence_day = F$CVTIME("''first_xmas_day'-37-0", "COMPARISON", "DATE")
$  ENDIF
$!
$  IF first_xmas_weekday .EQS. "Saturday"
$  THEN
$     repentence_day = F$CVTIME("''first_xmas_day'-38-0", "COMPARISON", "DATE")
$  ENDIF
$!
$  IF first_xmas_weekday .EQS. "Sunday"
$  THEN
$     repentence_day = F$CVTIME("''first_xmas_day'-39-0", "COMPARISON", "DATE")
$  ENDIF
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! check for variable date holidays ...
$!
$  IF comparison_date .EQS. good_friday
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Good Friday (Karfreitag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. easter_sunday
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Easter Sunday (Ostersonntag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. easter_monday
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Easter Monday (Ostermontag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. ascension_day
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Ascension Day (Christi Himmelfahrt)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. whit_sunday
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Whit Sunday (Pfingstsonntag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. whit_monday
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Whit Monday (Pfingstmontag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. corpus_christi_day
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Corpus Christi Day (Fronleichnam)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$  IF comparison_date .EQS. monday_before_lent
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Monday before Lent (Rosenmontag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. carnival_tuesday
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Carnival Tuesday (Karnevalsdienstag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. ash_wednesday
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Ash Wednesday (Aschermittwoch)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$  IF comparison_date .EQS. repentence_day
$  THEN
$     WRITE SYS$OUTPUT -
         F$FAO("!AS !AS a !AS and !AS", -
               date, verb, weekday, -
               "Repentence Day (Buss- und Bettag)")
$     EXIT base_exit_code + %X00000003
$  ENDIF
$!
$! - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
$!
$! no holiday ...
$!
$  WRITE SYS$OUTPUT -
      F$FAO("!AS !AS a !AS", -
            date, verb, weekday)
$!
$  EXIT base_exit_code + %X00000001
$!
$  ENDSUBROUTINE
$!
$! ============================================================================
$! E N D   O F   C O M M A N D   P R O C E D U R E
$! ============================================================================
[end of mail]
