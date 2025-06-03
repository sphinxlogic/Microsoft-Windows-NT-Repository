/*
 * Author: Jason Baietto, jason@ssd.csd.harris.com
 * xdiary Copyright 1990 Harris Corporation
 *
 * Permission to use, copy, modify, and distribute, this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holder be used in
 * advertising or publicity pertaining to distribution of the software with
 * specific, written prior permission, and that no fee is charged for further
 * distribution of this software, or any modifications thereof.  The copyright
 * holder makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, AND IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM ITS USE,
 * LOSS OF DATA, PROFITS, QPA OR GPA, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* Application Resources */

typedef struct {
   String  language;
   String  diaryfile;
   String  templatefile;
   Boolean nocalendar;
   Boolean nodiary;
   Boolean long_opt;
   int     abbrev;
   Boolean fullyear;
   Boolean singlemonth;
} app_data_type, *app_data_ptr_type;

#define XtNlanguage     "language"
#define XtNdiaryFile    "diaryFile"
#define XtNtemplateFile "templateFile"
#define XtNnoCalendar   "noCalendar"
#define XtNnoDiary      "noDiary"
#define XtNlong         "long"
#define XtNabbrev       "abbrev"
#define XtNfullYear     "fullYear"
#define XtNsingleMonth  "singleMonth"

#define XtCLanguage     "Language"
#define XtCDiaryFile    "DiaryFile"
#define XtCTemplateFile "TemplateFile"
#define XtCNoCalendar   "NoCalendar"
#define XtCNoDiary      "NoDiary"
#define XtCLong         "Long"
#define XtCAbbrev       "Abbrev"
#define XtCFullYear     "FullYear"
#define XtCSingleMonth  "SingleMonth"

#define offset(field) XtOffset(app_data_ptr_type, field)
XtResource application_resources[] = {
   {
      XtNlanguage,
      XtCLanguage,
      XtRString,
      sizeof(String),
      offset(language),
      XtRImmediate,
      (XtPointer)NULL
   },
   {
      XtNdiaryFile,
      XtCDiaryFile,
      XtRString,
      sizeof(String),
      offset(diaryfile),
      XtRImmediate,
      (XtPointer)NULL
   },
   {
      XtNtemplateFile,
      XtCTemplateFile,
      XtRString,
      sizeof(String),
      offset(templatefile),
      XtRImmediate,
      (XtPointer)NULL
   },
   {
      XtNnoCalendar,
      XtCNoCalendar,
      XtRBoolean,
      sizeof(Boolean),
      offset(nocalendar),
      XtRImmediate,
      (XtPointer)False
   },
   {
      XtNnoDiary,
      XtCNoDiary,
      XtRBoolean,
      sizeof(Boolean),
      offset(nodiary),
      XtRImmediate,
      (XtPointer)False
   },
   {
      XtNlong,
      XtCLong,
      XtRBoolean,
      sizeof(Boolean),
      offset(long_opt),
      XtRImmediate,
      (XtPointer)False
   },
   {
      XtNabbrev,
      XtCAbbrev,
      XtRInt,
      sizeof(int),
      offset(abbrev),
      XtRImmediate,
      (XtPointer)0
   },
   {
      XtNfullYear,
      XtCFullYear,
      XtRBoolean,
      sizeof(Boolean),
      offset(fullyear),
      XtRImmediate,
      (XtPointer)False
   },
   {
      XtNsingleMonth,
      XtCSingleMonth,
      XtRBoolean,
      sizeof(Boolean),
      offset(singlemonth),
      XtRImmediate,
      (XtPointer)False
   }
};
