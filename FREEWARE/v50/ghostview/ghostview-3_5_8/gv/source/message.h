/*
**
** message.h
** Version 2.6
** Mai 1996
**
** Copyright (C) 1995, 1996, 1997 Johannes Plass
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
** 
** Author:   Johannes Plass (plass@thep.physik.uni-mainz.de)
**           Department of Physics
**           Johannes Gutenberg-University
**           Mainz, Germany
**
*/


#ifndef _MESSAGE_LOADED_

#if defined(MESSAGES) || defined(MESSAGES1)


#ifdef MESSAGE_NO_ESC
#   define MESSAGE_B_ESC ""
#   define MESSAGE_E_ESC ""
#else
#   define MESSAGE_B_ESC "\033[1m"
#   define MESSAGE_E_ESC "\033[m"
#endif

#include<stdlib.h>
#include<stdio.h>

#if defined(__STDC__) || defined(__ALPHA)
#   define _MESSAGE_USE_STDC_
#endif

#ifdef _MESSAGE_USE_STDC_
#   define MESSAGE_STRING(x) #x
#else
#   define MESSAGE_STRING(x) "x"
#endif

/*#####################################################################*/
#ifdef VMS /*                    VMS                                   */
/*#####################################################################*/

#include<string.h>
#include<descrip.h>
#include<libclidef.h>
#include<lib$routines.h>
#include<unixlib.h>

#define MESSAGE_FILE (strrchr(__FILE__,']')+1)
   
#define MESSAGE_dsc(name,string,length)				\
	name.dsc$w_length  = (unsigned short) (length);		\
	name.dsc$b_dtype   = (unsigned char)  DSC$K_DTYPE_T;	\
	name.dsc$b_class   = (unsigned char)  DSC$K_CLASS_S;	\
	name.dsc$a_pointer = (char*) (string)

#define MESSAGE_SAVEINDENT(indent) {			\
	long table = LIB$K_CLI_LOCAL_SYM;		\
	struct dsc$descriptor symbol;			\
	struct dsc$descriptor value;			\
	MESSAGE_dsc(symbol,"MESSAGE_INDENT",14);	\
	MESSAGE_dsc(value,indent,strlen(indent));	\
	lib$set_symbol(&symbol,&value,&table);		\
	}

#define MESSAGE_GETINDENT(indent,len) {					\
	struct dsc$descriptor symbol;					\
	struct dsc$descriptor result;					\
	MESSAGE_dsc(symbol,"MESSAGE_INDENT",14);			\
	MESSAGE_dsc(result,indent,99);					\
	len = 0;							\
	if (lib$get_symbol(&symbol,&result,&len,0) != 1) len = 0;	\
	indent[len] = '\0';						\
	}

/*#####################################################################*/
#else /*            end of VMS, start of other systems                 */
/*#####################################################################*/

#define MESSAGE_FILE (__FILE__)

#define MESSAGE_SAVEINDENT(indent) {		\
	setenv("MESSAGE_INDENT",indent,1);	\
	}

#define MESSAGE_GETINDENT(indent,len) {					\
	char *__result=getenv("MESSAGE_INDENT");			\
	if (__result)	{ len=strlen(__result); strcpy(indent,__result); }\
	else		{ len=0; indent[0]='\0'; }			\
	}

/*#####################################################################*/
#endif /*                  end of other system                         */
/*#####################################################################*/

#define MESSAGE_PRINTF(format,value) {	\
   char __indent[100]; unsigned short __len;\
   MESSAGE_GETINDENT(__indent,__len);	\
   printf(__indent);			\
   printf(format,value);		\
}

#define MESSAGE2_PRINTF(format,value1,value2) {	\
   char __indent[100]; unsigned short __len;	\
   MESSAGE_GETINDENT(__indent,__len);		\
   printf(__indent);				\
   printf(format,value1,value2);		\
}

#define BEGIN_MESSAGE(txt)	{			\
   char __indent[100]; unsigned short __len;		\
   MESSAGE_GETINDENT(__indent,__len)			\
   printf("%s\273\273\273 %s%s%s <%s>\n",		\
	__indent,					\
	MESSAGE_B_ESC,MESSAGE_STRING(txt),MESSAGE_E_ESC,\
	MESSAGE_FILE					\
   );							\
   if (__len <= 56) strcat(__indent,"    ");		\
   MESSAGE_SAVEINDENT(__indent)				\
}

#define END_MESSAGE(txt)	{			\
   char __indent[100]; unsigned short __len;		\
   MESSAGE_GETINDENT(__indent,__len)			\
   if (__len>= 4) __indent[__len-4] = '\0';		\
   printf("%s\253\253\253 %s%s%s <%s>\n",		\
	__indent,					\
	MESSAGE_B_ESC,MESSAGE_STRING(txt),MESSAGE_E_ESC,\
	MESSAGE_FILE					\
   );							\
   MESSAGE_SAVEINDENT(__indent)				\
}

#define MAIN_BEGIN_MESSAGE(txt) {	\
   char *__indent = "";			\
   MESSAGE_SAVEINDENT(__indent)		\
   BEGINMESSAGE(txt)			\
}

#define MAIN_END_MESSAGE(txt) {		\
   char *__indent = "";			\
   MESSAGE_SAVEINDENT(__indent)		\
   ENDMESSAGE(txt)			\
}

#endif /* MESSAGES || MESSAGES1 */

#ifdef MESSAGES

#define MESSAGE_NULL_PTR		"<NULL pointer>"

#ifdef _MESSAGE_USE_STDC_ 
#   define MAINBEGINMESSAGE(txt)	MAIN_BEGIN_MESSAGE(txt)
#   define MAINENDMESSAGE(txt)		MAIN_END_MESSAGE(txt)
#   define BEGINMESSAGE(txt)		BEGIN_MESSAGE(txt)
#   define ENDMESSAGE(txt)		END_MESSAGE(txt)
#   define INFMESSAGE(txt)		MESSAGE_PRINTF(#txt "%s\n","")
#   define CMESSAGE(cr)			MESSAGE_PRINTF(#cr "=%c\n",cr)
#   define INFCMESSAGE(txt,cr)		MESSAGE_PRINTF(#txt " " #cr "=%c\n",cr)
#   define IMESSAGE(it)			MESSAGE_PRINTF(#it "=%d\n",it)
#   define INFIMESSAGE(txt,it)		MESSAGE_PRINTF(#txt " " #it "=%d\n",it)
#   define IIMESSAGE(it1,it2)		MESSAGE2_PRINTF(#it1 "=%d, " #it2 "=%d\n",it1,it2)
#   define INFIIMESSAGE(txt,it1,it2)	MESSAGE2_PRINTF(#txt " " #it1 "=%d, " #it2 "=%d\n",it1,it2)
#   define FMESSAGE(ft)			MESSAGE_PRINTF(#ft "=%f\n",ft)
#   define INFFMESSAGE(txt,ft)		MESSAGE_PRINTF(#txt " " #ft "=%f\n",ft)
#   define SMESSAGE(st)			MESSAGE_PRINTF(#st "=%s\n",st ? st : MESSAGE_NULL_PTR)
#   define INFSMESSAGE(txt,st)		MESSAGE_PRINTF(#txt " " #st "=%s\n",st ? st : MESSAGE_NULL_PTR)
#else
#   define MAINBEGINMESSAGE(txt)	MAIN_BEGIN_MESSAGE(txt)
#   define MAINENDMESSAGE(txt)		MAIN_END_MESSAGE(txt)
#   define BEGINMESSAGE(txt)		BEGIN_MESSAGE(txt)
#   define ENDMESSAGE(txt)		END_MESSAGE(txt)
#   define INFMESSAGE(txt)		MESSAGE_PRINTF("txt %s\n","")
#   define CMESSAGE(cr)			MESSAGE_PRINTF("cr=%c\n",cr)
#   define INFCMESSAGE(txt,cr)		MESSAGE_PRINTF("txt cr=%c\n",cr)
#   define IMESSAGE(it)			MESSAGE_PRINTF("it=%d\n",it)
#   define INFIMESSAGE(txt,it)		MESSAGE_PRINTF("txt it=%d\n",it)
#   define IIMESSAGE(it1,it2)		MESSAGE2_PRINTF("it1=%d, it2=%d\n",it1,it2)
#   define INFIIMESSAGE(txt,it1,it2)	MESSAGE2_PRINTF("txt it1=%d, it2=%d\n",it1,it2)
#   define FMESSAGE(ft)			MESSAGE_PRINTF("ft=%f\n",ft)
#   define INFFMESSAGE(txt,ft)		MESSAGE_PRINTF("txt ft=%f\n",ft)
#   define SMESSAGE(st)			MESSAGE_PRINTF("st=%s\n",st ? st : MESSAGE_NULL_PTR)
#   define INFSMESSAGE(txt,st)		MESSAGE_PRINTF("txt st=%s\n",st ? st : MESSAGE_NULL_PTR)
#endif

#else	/* MESSAGES */
#   define MAINBEGINMESSAGE(txt)
#   define MAINENDMESSAGE(txt)
#   define BEGINMESSAGE(txt)
#   define ENDMESSAGE(txt)
#   define INFMESSAGE(txt)
#   define CMESSAGE(cr)
#   define INFCMESSAGE(txt,cr)
#   define IMESSAGE(it)
#   define INFIMESSAGE(txt,it)
#   define IIMESSAGE(it1,it2)
#   define INFIIMESSAGE(txt,it1,it2)
#   define FMESSAGE(ft)
#   define INFFMESSAGE(txt,ft)
#   define SMESSAGE(st)
#   define INFSMESSAGE(txt,st)
#endif /* MESSAGES */

#ifdef MESSAGES1

#ifdef _MESSAGE_USE_STDC_
#   define BEGINMESSAGE1(txt)		BEGIN_MESSAGE(txt)
#   define ENDMESSAGE1(txt)		END_MESSAGE(txt)
#   define INFMESSAGE1(txt)		MESSAGE_PRINTF(#txt "%s\n","")
#   define IMESSAGE1(it)		MESSAGE_PRINTF(#it "=%d\n",it)
#   define INFIMESSAGE1(txt,it)		MESSAGE_PRINTF(#txt " " #it "=%d\n",it)
#   define FMESSAGE1(ft)		MESSAGE_PRINTF(#ft "=%f\n",ft)
#   define INFFMESSAGE1(txt,ft)		MESSAGE_PRINTF(#txt " " #ft "=%f\n",ft)
#   define SMESSAGE1(st)		MESSAGE_PRINTF(#st "=%s\n",st ? st : MESSAGE_NULL_PTR)
#   define INFSMESSAGE1(txt,st)		MESSAGE_PRINTF(#txt " " #st "=%s\n",st ? st : MESSAGE_NULL_PTR)
#   define IIMESSAGE1(it1,it2)		MESSAGE2_PRINTF(#it1 "=%d, " #it2 "=%d\n",it1,it2)
#   define INFIIMESSAGE1(txt,it1,it2)	MESSAGE2_PRINTF(#txt " " #it1 "=%d, " #it2 "=%d\n",it1,it2)
#else
#   define BEGINMESSAGE1(txt)		BEGIN_MESSAGE(txt)
#   define ENDMESSAGE1(txt)		END_MESSAGE(txt)
#   define INFMESSAGE1(txt)		MESSAGE_PRINTF("txt %s\n","")
#   define IMESSAGE1(it)		MESSAGE_PRINTF("it=%d\n",it)
#   define INFIMESSAGE1(txt,it)		MESSAGE_PRINTF("txt it=%d\n",it)
#   define FMESSAGE1(ft)		MESSAGE_PRINTF("ft=%f\n",ft)
#   define INFFMESSAGE1(txt,ft)		MESSAGE_PRINTF("txt ft=%f\n",ft)
#   define SMESSAGE1(st)		MESSAGE_PRINTF("st=%s\n",st ? st : MESSAGE_NULL_PTR)
#   define INFSMESSAGE1(txt,st)		MESSAGE_PRINTF("txt st=%s\n",st ? st : MESSAGE_NULL_PTR)
#   define IIMESSAGE1(it1,it2)		MESSAGE2_PRINTF("it1=%d, it2=%d\n",it1,it2)
#   define INFIIMESSAGE1(txt,it1,it2)	MESSAGE2_PRINTF("txt it1=%d, it2=%d\n",it1,it2)
#endif

#else	/* MESSAGES1 */

#   define BEGINMESSAGE1(txt)
#   define ENDMESSAGE1(txt)
#   define INFMESSAGE1(txt)
#   define IMESSAGE1(it)
#   define INFIMESSAGE1(txt,it)
#   define FMESSAGE1(ft)
#   define INFFMESSAGE1(txt,ft)
#   define SMESSAGE1(st)
#   define INFSMESSAGE1(txt,st)
#   define IIMESSAGE1(it1,it2)
#   define INFIIMESSAGE1(txt,it1,it2)

#endif	/* MESSAGES1 */


#define _MESSAGE_LOADED_
#endif /* _MESSAGE_LOADED_ */
