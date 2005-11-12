/******************************************************************
*
*	CyberUtil for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: cstring.h
*
*	Revision:
*
*	01/25/05
*		- first revision
*
******************************************************************/

#ifndef _CG_UTIL_CSTRING_H_
#define _CG_UTIL_CSTRING_H_

#include <cybergarage/typedef.h>
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

/****************************************
* Data Type
****************************************/

typedef struct _CgString {
	char *value;
	BOOL freeFlag;
	int memSize;
	int valueSize;
} CgString;

typedef struct _CgStringTokenizer {
	char *value;
	char *delim;
	int delimCnt;
	int nextStartPos;
	int lastPos;
	char *currToken;
	char *nextToken;
	char repToken;
	BOOL hasNextTokens;
} CgStringTokenizer;

/****************************************
* Function (String)
****************************************/

CgString *cg_string_new();
void cg_string_delete(CgString *str);
void cg_string_clear(CgString *str);
void cg_string_setvalue(CgString *str, char *value);
void cg_string_setintvalue(CgString *str, int value);
void cg_string_setlongvalue(CgString *str, long value);
void cg_string_setnvalue(CgString *str, char *value, int len);
void cg_string_setpointervalue(CgString *str, char *value);
char *cg_string_getvalue(CgString *str);
int cg_string_length(CgString *str);

#define cg_string_setfreeflag(str, flag) (str->freeFlag = flag)
#define cg_string_getfreeflag(str) (str->freeFlag)

char *cg_string_addvalue(CgString *str, char *value);
char *cg_string_naddvalue(CgString *str, char *value, int valueLen);
char *cg_string_addrepvalue(CgString *str, char *value, int repeatCnt);
char *cg_string_naddrepvalue(CgString *str, char *value, int valueLen, int repeatCnt);

char *cg_string_replace(CgString *str, char *fromStr[], char *toStr[], int fromStrCnt);

int cg_string_getmemorysize(CgString *str);

/****************************************
* Function 
****************************************/

char *cg_strdup(char *str);
int cg_strlen(char *str);
char *cg_strcpy(char *dest, char *src);
char *cg_strcat(char *dest, char *src);
int cg_strcmp(char *str1, char *str2);
int cg_strcasecmp(char *str1, char *str2);
BOOL cg_streq(char *str1, char *str2);
BOOL cg_strcaseeq(char *str1, char *str2);
int cg_strchr(char *str, char *chars, int nchars);
int cg_strrchr(char *str, char *chars, int nchars);
int cg_strstr(char *haystack, char *needle);
char *cg_strtrim(char *str, char *delim, int ndelim);
char *cg_strltrim(char *str, char *delim, int ndelim);
char *cg_strrtrim(char *str, char *delim, int ndelim);
char *cg_int2str(int value, char *buf, int bufSize);
char *cg_long2str(long value, char *buf, int bufSize);
char *cg_strncpy(char *str1, char *str2, size_t cnt);
char *cg_strncat(char *str1, char *str2, size_t cnt);

#if defined(__USE_ISOC99)
char *cg_longlong2str(long long value, char *buf, int bufSize);
#elif defined(WIN32)
char *cg_longlong2str(__int64 value, char *buf, int bufSize);
#endif

#define cg_str2int(value) atoi(value)
#define cg_str2long(value) atol(value)

#if defined(__USE_ISOC99)
#define cg_str2longlong(value) atoll(value)
#elif defined(WIN32)
#define cg_str2longlong(value) _atoi64(value)
#endif

/****************************************
* Function (StringTokenizer)
****************************************/

CgStringTokenizer *cg_string_tokenizer_new(char *str, char *delim);
void cg_string_tokenizer_delete(CgStringTokenizer *strToken);
BOOL cg_string_tokenizer_hasmoretoken(CgStringTokenizer *strToken);
char *cg_string_tokenizer_nexttoken(CgStringTokenizer *strToken);
char *cg_string_tokenizer_nextalltoken(CgStringTokenizer *strToken);

void cg_string_tokenizer_print(CgStringTokenizer *strToken);

#ifdef  __cplusplus
}
#endif

#endif
