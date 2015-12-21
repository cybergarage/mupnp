/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#ifndef _MUPNP_UTIL_STRING_H_
#define _MUPNP_UTIL_STRING_H_

#include <mupnp/typedef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************
* Define
****************************************/

#define MUPNP_STRING_MEMORY_ALLOCATION_UNIT 64

/* UINT_MAX : 4294967295U */
#define MUPNP_STRING_INTEGER_BUFLEN 16

/* ULONG_MAX : 4294967295UL */
#define MUPNP_STRING_LONG_BUFLEN 32

/*  ULLONG_MAX : 18446744073709551615ULL */
#define MUPNP_STRING_LONGLONG_BUFLEN 32

#define MUPNP_STRING_FLOAT_BUFLEN 64
#define MUPNP_STRING_DOUBLE_BUFLEN 64

/****************************************
* Function 
****************************************/

char* mupnp_strdup(const char* str);
size_t mupnp_strlen(const char* str);
char* mupnp_strcpy(char* dest, const char* src);
char* mupnp_strcat(char* dest, const char* src);
int mupnp_strcmp(const char* str1, const char* str2);
int mupnp_strncmp(const char* str1, const char* str2, int nchars);
int mupnp_strcasecmp(const char* str1, const char* str2);
bool mupnp_streq(const char* str1, const char* str2);
bool mupnp_strcaseeq(const char* str1, const char* str2);
ssize_t mupnp_strchr(const char* str, const char* chars, size_t nchars);
ssize_t mupnp_strrchr(const char* str, const char* chars, size_t nchars);
ssize_t mupnp_strstr(const char* haystack, const char* needle);
char* mupnp_strtrimwhite(char* str);
char* mupnp_strtrim(char* str, char* delim, size_t ndelim);
char* mupnp_strltrim(char* str, char* delim, size_t ndelim);
char* mupnp_strrtrim(char* str, char* delim, size_t ndelim);
char* mupnp_strncpy(char* str1, const char* str2, size_t cnt);
char* mupnp_strncat(char* str1, const char* str2, size_t cnt);

const char* mupnp_int2str(int value, char* buf, size_t bufSize);
const char* mupnp_long2str(long value, char* buf, size_t bufSize);
const char* mupnp_float2str(float value, char* buf, size_t bufSize);
const char* mupnp_double2str(double value, char* buf, size_t bufSize);
const char* mupnp_sizet2str(size_t value, char* buf, size_t bufSize);
const char* mupnp_ssizet2str(ssize_t value, char* buf, size_t bufSize);

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

typedef struct _mUpnpString {
  char* value;
  size_t memSize;
  size_t valueSize;
} mUpnpString;

typedef struct _mUpnpStringTokenizer {
  char* value;
  char* delim;
  size_t delimCnt;
  size_t nextStartPos;
  size_t lastPos;
  char* currToken;
  char* nextToken;
  char repToken;
  bool hasNextTokens;
} mUpnpStringTokenizer;

/****************************************
* Function (StringTokenizer)
****************************************/

mUpnpStringTokenizer* mupnp_string_tokenizer_new(const char* str, const char* delim);
void mupnp_string_tokenizer_delete(mUpnpStringTokenizer* strToken);
bool mupnp_string_tokenizer_hasmoretoken(mUpnpStringTokenizer* strToken);
char* mupnp_string_tokenizer_nexttoken(mUpnpStringTokenizer* strToken);
char* mupnp_string_tokenizer_nextalltoken(mUpnpStringTokenizer* strToken);

void mupnp_string_tokenizer_print(mUpnpStringTokenizer* strToken);

/****************************************
* Function (String)
****************************************/

mUpnpString* mupnp_string_new();
void mupnp_string_delete(mUpnpString* str);
void mupnp_string_clear(mUpnpString* str);

void mupnp_string_setvalue(mUpnpString* str, const char* value);
void mupnp_string_setintvalue(mUpnpString* str, int value);
void mupnp_string_setlongvalue(mUpnpString* str, long value);
void mupnp_string_setfloatvalue(mUpnpString* str, float value);
void mupnp_string_setdoublevalue(mUpnpString* str, double value);
void mupnp_string_setnvalue(mUpnpString* str, const char* value, size_t len);
void mupnp_string_setpointervalue(mUpnpString* str, char* value, size_t len);

char* mupnp_string_getvalue(mUpnpString* str);
#define mupnp_string_getintvalue(str) mupnp_str2int(mupnp_string_getvalue(str))
#define mupnp_string_getlongvalue(str) mupnp_str2long(mupnp_string_getvalue(str))
#define mupnp_string_getfloatvalue(str) mupnp_str2float(mupnp_string_getvalue(str))
#define mupnp_string_getdoublevalue(str) mupnp_str2double(mupnp_string_getvalue(str))

size_t mupnp_string_length(mUpnpString* str);

char* mupnp_string_addvalue(mUpnpString* str, const char* value);
char* mupnp_string_naddvalue(mUpnpString* str, const char* value, size_t valueLen);
char* mupnp_string_addrepvalue(mUpnpString* str, const char* value, size_t repeatCnt);
char* mupnp_string_naddrepvalue(mUpnpString* str, const char* value, size_t valueLen, size_t repeatCnt);

char* mupnp_string_replace(mUpnpString* str, char* fromStr[], char* toStr[], size_t fromStrCnt);

size_t mupnp_string_getmemorysize(mUpnpString* str);

#ifdef __cplusplus
}
#endif

#endif
