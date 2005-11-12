/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cstring_function.c
*
*	Revision:
*
*	01/25/05
*		- first revision
*	11/11/053qeds
*		- Added cg_longlong2str() and cg_str2longlong().
*
******************************************************************/

#include <cybergarage/util/cstring.h>

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#if !defined(NORTiAPI)
#include <string.h>
#endif

/****************************************
* cg_strdup
****************************************/

char *cg_strdup(char *str)
{
#if defined(HAVE_STRDUP)
	return strdup(str);
#else
	char *cpStrBuf = (char *)malloc(strlen(str)+1);
	strcpy(cpStrBuf, str);
	return cpStrBuf;
#endif
}

/****************************************
* cg_strlen
****************************************/

int cg_strlen(char *str)
{
	return (str == NULL) ? 0 : strlen(str);
}

/****************************************
* cg_strcpy
****************************************/

char *cg_strcpy(char *dest, char *src)
{
	return strcpy(dest, src);
}

/****************************************
* cg_strcat
****************************************/

char *cg_strcat(char *dest, char *src)
{
	return strcat(dest, src);
}

/****************************************
* cg_strcmp
****************************************/

int cg_strcmp(char *str1, char *str2)
{
	if (str1 == NULL)
		return -1;
	if (str2 == NULL)
		return 1;
	return strcmp(str1, str2);
}

/****************************************
* cg_strcasecmp
****************************************/

int cg_strcasecmp(char *str1, char *str2)
{
#if !defined(WIN32)
	return strcasecmp(str1, str2);
#else
	for (; toupper(*str1) == toupper(*str2); str1++, str2++) {
		if (*str1 == '\0')
			return 0;
	}
	return *str1 - *str2;
#endif
}

/****************************************
* cg_streq
****************************************/

BOOL cg_streq(char *str1, char *str2)
{
	return ((cg_strcmp(str1, str2) == 0) ? TRUE : FALSE);
}

/****************************************
* cg_strcaseeq
****************************************/

BOOL cg_strcaseeq(char *str1, char *str2)
{
	return ((cg_strcasecmp(str1, str2) == 0) ? TRUE : FALSE);
}

/****************************************
* cg_strstr
****************************************/

int cg_strstr(char *haystack, char *needle)
{
	char *strPos;
	if (haystack == NULL || needle == NULL)
		return -1;
	strPos = strstr(haystack, needle);
	if (strPos == NULL)
		return -1;
	return (strPos - haystack);
}

/****************************************
* cg_strchr
****************************************/

int cg_strchr(char *str, char *chars, int nchars)
{
	int strLen;
	int i, j;
	
	if (str == NULL || chars == NULL)
		return -1;
		
	strLen = cg_strlen(str);
	for (i=0; i<strLen; i++) {
		for (j=0; j<nchars; j++) {
			if (str[i] == chars[j])
				return i;
		}		
	}
	
	return -1;
}

/****************************************
* cg_strrchr
****************************************/

int cg_strrchr(char *str, char *chars, int nchars)
{
	int strLen;
	int i, j;
	
	if (str == NULL || chars == NULL)
		return -1;
		
	strLen = cg_strlen(str);
	for (i=(strLen-1); 0<=i; i--) {
		for (j=0; j<nchars; j++) {
			if (str[i] == chars[j])
				return i;
		}		
	}
	
	return -1;
}

/****************************************
* cg_strtrim
****************************************/

char *cg_strtrim(char *str, char *delim, int ndelim)
{
	cg_strrtrim(str, delim, ndelim);
	return cg_strltrim(str, delim, ndelim);
}

/****************************************
* cg_strltrim
****************************************/

char *cg_strltrim(char *str, char *delim, int ndelim)
{
	int strLen, i, j;
	
	strLen = cg_strlen(str);
	for (i=0; i<strLen; i++) {
		BOOL hasDelim = FALSE;
		for (j=0; j<ndelim; j++) {
			if (str[i] == delim[j]) {
				hasDelim = TRUE;
				break;
			}
		}
		if (hasDelim == FALSE)
			return (str + i);
	}
	
	return (str + strLen);
}

/****************************************
* cg_strrtrim
****************************************/

char *cg_strrtrim(char *str, char *delim, int ndelim)
{
	int strLen, i, j;
	
	strLen = cg_strlen(str);
	for (i=(strLen-1); 0<=i; i--) {
		BOOL hasDelim = FALSE;
		for (j=0; j<ndelim; j++) {
			if (str[i] == delim[j]) {
				hasDelim = TRUE;
				str[i] = '\0';
				break;
			}
		}
		if (hasDelim == FALSE)
			break;
	}
	
	return str;
}

/****************************************
* cg_strncpy
****************************************/

char *cg_strncpy(char *str1, char *str2, size_t cnt)
{
	strncpy(str1, str2, cnt);
	str1[cnt-1] = '\0';
	return str1;
}

/****************************************
* cg_strncat
****************************************/

char *cg_strncat(char *str1, char *str2, size_t cnt)
{
	return cg_strncpy(&str1[cg_strlen(str1)], str2, cnt);
}

/****************************************
* cg_int2str
****************************************/

char *cg_int2str(int value, char *buf, int bufSize)
{
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%d", value);
#else
	sprintf(buf, "%d", value);
#endif
	return buf;
}

/****************************************
* cg_long2str
****************************************/

char *cg_long2str(long value, char *buf, int bufSize)
{
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%ld", value);
#else
	sprintf(buf, "%ld", value);
#endif
	return buf;
}

/****************************************
* cg_longlong2str
****************************************/

#if defined(__USE_ISOC99)
char *cg_longlong2str(long long value, char *buf, int bufSize)
{
#if defined(HAVE_SNPRINTF)
	snprintf(buf, bufSize, "%lld", value);
#else
	sprintf(buf, "%lld", value);
#endif
}
#elif defined(WIN32)
char *cg_longlong2str(__int64 value, char *buf, int bufSize)
{
	return _i64toa(value, buf, 10);
}
#endif
