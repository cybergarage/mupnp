/******************************************************************
*
* CyberUtil for C
*
* Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
* File: cstring.h
*
* Revision:
*
* 01/25/05
*  - first revision
* 03/01/06 Theo Beisch
*  - introduced LONGLONG macro
*  - added mupnp_strtrimwhite() to trim all 
*    leading and trailing non-printable chars
*
******************************************************************/

#ifndef _CG_UTIL_CSTRING_H_
#define _CG_UTIL_CSTRING_H_

#include <mupnp/typedef.h>
#include <stdlib.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define CG_STRING_MEMORY_ALLOCATION_UNIT 64
 
/* UINT_MAX : 4294967295U */
#define CG_STRING_INTEGER_BUFLEN 16 
 
 /* ULONG_MAX : 4294967295UL */
#define CG_STRING_LONG_BUFLEN 32

 /*  ULLONG_MAX : 18446744073709551615ULL */
#define CG_STRING_LONGLONG_BUFLEN 32

#define CG_STRING_FLOAT_BUFLEN 64
#define CG_STRING_DOUBLE_BUFLEN 64
	
/****************************************
* Function 
****************************************/
	
char *mupnp_strdup(const char *str);
size_t mupnp_strlen(const char *str);
char *mupnp_strcpy(char *dest, const char *src);
char *mupnp_strcat(char *dest, const char *src);
int mupnp_strcmp(const char *str1, const char *str2);
int mupnp_strncmp(const char *str1, const char *str2, int nchars);
int mupnp_strcasecmp(const char *str1, const char *str2);
BOOL mupnp_streq(const char *str1, const char *str2);
BOOL mupnp_strcaseeq(const char *str1, const char *str2);
ssize_t mupnp_strchr(const char *str, const char *chars, size_t nchars);
ssize_t mupnp_strrchr(const char *str, const char *chars, size_t nchars);
ssize_t mupnp_strstr(const char *haystack, const char *needle);
char *mupnp_strtrimwhite(char *str);
char *mupnp_strtrim(char *str, char *delim, size_t ndelim);
char *mupnp_strltrim(char *str, char *delim, size_t ndelim);
char *mupnp_strrtrim(char *str, char *delim, size_t ndelim);
char *mupnp_strncpy(char *str1, const char *str2, size_t cnt);
char *mupnp_strncat(char *str1, const char *str2, size_t cnt);
	
const char *mupnp_int2str(int value, char *buf, size_t bufSize);
const char *mupnp_long2str(long value, char *buf, size_t bufSize);
const char *mupnp_float2str(float value, char *buf, size_t bufSize);
const char *mupnp_double2str(double value, char *buf, size_t bufSize);
const char *mupnp_sizet2str(size_t value, char *buf, size_t bufSize);
const char *mupnp_ssizet2str(ssize_t value, char *buf, size_t bufSize);

#define mupnp_str2int(value) (value ? atoi(value) : 0)
#define mupnp_str2long(value) (value ? atol(value) : 0)
#define mupnp_strhex2long(value) (value ? strtol(value, NULL, 16) : 0)
#define mupnp_strhex2ulong(value) (value ? strtoul(value, NULL, 16) : 0)
#define mupnp_str2float(value) ((float)(value ? atof(value) : 0.0))
#define mupnp_str2double(value) (value ? atof(value) : 0.0)
#define mupnp_str2sizet(value) ((size_t)(value ? atol(value) : 0))
#define mupnp_str2ssizet(value) ((ssize_t)(value ? atol(value) : 0))

/****************************************
* Data Type
****************************************/
	
typedef struct _CgString {
	char *value;
	size_t memSize;
	size_t valueSize;
} CgString;
	
typedef struct _CgStringTokenizer {
	char *value;
	char *delim;
	size_t delimCnt;
	size_t nextStartPos;
	size_t lastPos;
	char *currToken;
	char *nextToken;
	char repToken;
	BOOL hasNextTokens;
} CgStringTokenizer;
	
/****************************************
* Function (StringTokenizer)
****************************************/
	
CgStringTokenizer *mupnp_string_tokenizer_new(const char *str, const char *delim);
void mupnp_string_tokenizer_delete(CgStringTokenizer *strToken);
BOOL mupnp_string_tokenizer_hasmoretoken(CgStringTokenizer *strToken);
char *mupnp_string_tokenizer_nexttoken(CgStringTokenizer *strToken);
char *mupnp_string_tokenizer_nextalltoken(CgStringTokenizer *strToken);
	
void mupnp_string_tokenizer_print(CgStringTokenizer *strToken);
	
/****************************************
* Function (String)
****************************************/

CgString *mupnp_string_new();
void mupnp_string_delete(CgString *str);
void mupnp_string_clear(CgString *str);

void mupnp_string_setvalue(CgString *str, const char *value);
void mupnp_string_setintvalue(CgString *str, int value);
void mupnp_string_setlongvalue(CgString *str, long value);
void mupnp_string_setfloatvalue(CgString *str, float value);
void mupnp_string_setdoublevalue(CgString *str, double value);
void mupnp_string_setnvalue(CgString *str, const char *value, size_t len);
void mupnp_string_setpointervalue(CgString *str, char *value, size_t len);

char *mupnp_string_getvalue(CgString *str);
#define mupnp_string_getintvalue(str) mupnp_str2int(mupnp_string_getvalue(str))
#define mupnp_string_getlongvalue(str) mupnp_str2long(mupnp_string_getvalue(str))
#define mupnp_string_getfloatvalue(str) mupnp_str2float(mupnp_string_getvalue(str))
#define mupnp_string_getdoublevalue(str) mupnp_str2double(mupnp_string_getvalue(str))
	
size_t mupnp_string_length(CgString *str);

char *mupnp_string_addvalue(CgString *str, const char *value);
char *mupnp_string_naddvalue(CgString *str, const char *value, size_t valueLen);
char *mupnp_string_addrepvalue(CgString *str, const char *value, size_t repeatCnt);
char *mupnp_string_naddrepvalue(CgString *str, const char *value, size_t valueLen, size_t repeatCnt);

char *mupnp_string_replace(CgString *str, char *fromStr[], char *toStr[], size_t fromStrCnt);

size_t mupnp_string_getmemorysize(CgString *str);

#ifdef  __cplusplus
}
#endif

#endif
