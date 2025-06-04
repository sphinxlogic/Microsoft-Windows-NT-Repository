/* global info for the preferences facility.
 * N.B. many of these enums are used as indexes -- don't change without
 *   checking where they are used!
 */
#ifndef _PREFERENCES_H
#define _PREFERENCES_H

typedef enum {
    PREF_ALGO, PREF_UNITS, PREF_DATE_FORMAT, PREF_ZONE,
    PREF_DPYPREC, PREF_MSG_BELL
} Preferences;

#define	NPREFS	6	/* number of entries in Preferences */

typedef enum {PREF_ACCURATE, PREF_FAST} PrefAlgo;
typedef enum {PREF_ENGLISH, PREF_METRIC} PrefUnits;
typedef enum {PREF_MDY, PREF_YMD, PREF_DMY} PrefDateFormat;
typedef enum {PREF_LOCALTZ, PREF_UTCTZ} PrefStampZone;
typedef enum {PREF_LOPREC, PREF_HIPREC} PrefDpyPrec;
typedef enum {PREF_NOMSGBELL, PREF_MSGBELL} PrefMsgBell;

int pref_get();

#endif /* _PREFERENCES_H */
