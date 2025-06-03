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

#define SUNDAY    0
#define MONDAY    1
#define TUESDAY   2
#define WEDNESDAY 3
#define THURSDAY  4
#define FRIDAY    5
#define SATURDAY  6

#define JANUARY     1
#define FEBRUARY    2
#define MARCH       3
#define APRIL       4
#define MAY         5
#define JUNE        6
#define JULY        7
#define AUGUST      8
#define SEPTEMBER   9
#define OCTOBER     10
#define NOVEMBER    11
#define DECEMBER    12

#define ENGLISH 0
#define GERMAN  1
#define SPANISH 2
#define FRENCH  3
#define ITALIAN 4


static char * month_names[][13] = {
   {
      /* ENGLISH */
      "January", "February", "March", "April", "May", "June", "July",
      "August", "September", "October", "November", "December", NULL
   },
   {
      /* GERMAN */
      "Januar", "Februar", "M\344rz", "April", "Mai", "Juni", "Juli",
      "August", "September", "Oktober", "November", "Dezember", NULL
   },
   {
      /* SPANISH */
      "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio",
      "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre", NULL
   },
   {
      /* FRENCH */
      "Janvier", "F\351vrier", "Mars", "Avril", "Mai", "Juin", "Juillet",
      "Auguste", "Septembre", "Octobre", "Novembre", "D\351cembre", NULL
   }
};


static char * weekday_names[][8] = {
   {
      /* ENGLISH */
      "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
      "Friday", "Saturday", NULL
   },
   {
      /* GERMAN */
      "Sontag", "Montag", "Dienstag", "Mittwoch", "Donnerstag",
      "Freitag", "Samstag", NULL
   },
   {
      /* SPANISH */
      "Domingo", "Lunes", "Martes", "Mi\351rcoles", "Jueves", "Viernes",
      "S\341bado",  NULL
   },
   {
      /* FRENCH */
      "Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi",
      "Samedi", NULL
   }
};

static char * quit_label[] = {
   /* ENGLISH */ "Quit",
   /* GERMAN  */ "Aufgeben",
   /* SPANISH */ "Descargar",
   /* FRENCH  */ "Quitte"
};

static char * day_label[] = {
   /* ENGLISH */ "Day",
   /* GERMAN  */ "Tag",
   /* SPANISH */ "Dia",
   /* FRENCH  */ "Jour"
};

static char * entry_label[] = {
   /* ENGLISH */ "Entry",
   /* GERMAN  */ "Eintritt",
   /* SPANISH */ "Entrada",
   /* FRENCH  */ "Entre\351"  /* e' */
};

static char * month_label[] = {
   /* ENGLISH */ "Month",
   /* GERMAN  */ "Monat",
   /* SPANISH */ "Mes",
   /* FRENCH  */ "Mois"
};

static char * year_label[] = {
   /* ENGLISH */ "Year",
   /* GERMAN  */ "Jahr",
   /* SPANISH */ "A\361o",    /* n~ */
   /* FRENCH  */ "Ann\351e"   /* e' */
};

static char * warp_label[] = {
   /* ENGLISH */ "Warp",
   /* GERMAN  */ "Reisen",
   /* SPANISH */ "Viajar",
   /* FRENCH  */ "Voyage"
};

static char * today_label[] = {
   /* ENGLISH */ "Today",
   /* GERMAN  */ "Heute",
   /* SPANISH */ "Hoy",
   /* FRENCH  */ "Aujourd'hui"
};

static char * dialog_label[] = {
   /* ENGLISH */ "Destination:",
   /* GERMAN  */ "Bestimmung:",
   /* SPANISH */ "Destino:",
   /* FRENCH  */ "Destination:"  /* Same as English */
};

static char * diary_label[] = {
   /* ENGLISH */ "Diary",
   /* GERMAN  */ "Tagebuch",
   /* SPANISH */ "Diario",
   /* FRENCH  */ "Agenda"
};

static char * option_label[] = {
   /* ENGLISH */ "Option",
   /* GERMAN  */ "Option",    /* Same as English */
   /* SPANISH */ "Opci\363n", /* o' */
   /* FRENCH  */ "Option"     /* Same as English */
};

static char * single_label[] = {
   /* ENGLISH */ "Single",
   /* GERMAN  */ "Einzig",
   /* SPANISH */ "Solo",
   /* FRENCH  */ "Unique"
};

static char * full_label[] = {
   /* ENGLISH */ "Full",
   /* GERMAN  */ "Voll",
   /* SPANISH */ "Completo",
   /* FRENCH  */ "Complet"
};

static char * save_label[] = {
   /* ENGLISH */ "Save",
   /* GERMAN  */ "Speichern",
   /* SPANISH */ "Salve",
   /* FRENCH  */ "Sauver"
};

#define MAX_WIDGETS 25

/* in milliseconds */
#define ONE_SECOND          (1000) 
#define ONE_MINUTE          (60 * ONE_SECOND)
#define FIVE_MINUTES        (5  * ONE_MINUTE)
#define ONE_HOUR            (60 * ONE_MINUTE)
#define TWENTY_FOUR_HOURS   (24 * ONE_HOUR)

#define DIARY_FILE "/.xdiary"
#define TEMPLATE_FILE "/.xdiary-template"

#define DAY   (1L<<1)
#define MONTH (1L<<2)
#define YEAR  (1L<<3)
#define ENTRY (1L<<4)

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
