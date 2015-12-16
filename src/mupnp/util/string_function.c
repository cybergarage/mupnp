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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mupnp/util/string.h>
#include <mupnp/util/log.h>

#include <ctype.h>

#if defined(WINCE)
//Theo Beisch - needed for _i64toa_s support -
// strangely just with the VS2005 ce environment
// evc4 is only happy without :-(
#include <altcecrt.h>
#endif

#if !defined(NORTiAPI)
#include <string.h>
#endif

/****************************************
* mupnp_strdup
****************************************/

char* mupnp_strdup(const char* str)
{
#if !defined(HAVE_STRDUP)
  char* cpStrBuf;
#endif

  /* mupnp_log_debug_l5("Entering...\n"); */

  if (str == NULL)
    return NULL;

#if defined(HAVE_STRDUP)
  return strdup(str);
#else
  cpStrBuf = (char*)malloc(strlen(str) + 1);
  if (NULL != cpStrBuf)
    strcpy(cpStrBuf, str);
  return cpStrBuf;
#endif

  /* mupnp_log_debug_l5("Leaving...\n"); */
}

/****************************************
* mupnp_strlen
****************************************/

size_t mupnp_strlen(const char* str)
{
  mupnp_log_debug_l5("Entering...\n");

  return (str == NULL) ? 0 : strlen(str);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strcpy
****************************************/

char* mupnp_strcpy(char* dest, const char* src)
{
  mupnp_log_debug_l5("Entering...\n");

  return strcpy(dest, src);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strcat
****************************************/

char* mupnp_strcat(char* dest, const char* src)
{
  mupnp_log_debug_l5("Entering...\n");

  return strcat(dest, src);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strcmp
****************************************/

int mupnp_strcmp(const char* str1, const char* str2)
{
  mupnp_log_debug_l5("Entering...\n");

  if (str1 == NULL)
    return -1;
  if (str2 == NULL)
    return 1;
  return strcmp(str1, str2);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strncmp
****************************************/

int mupnp_strncmp(const char* str1, const char* str2, int nchars)
{
  mupnp_log_debug_l5("Entering...\n");

  if (str1 == NULL)
    return -1;
  if (str2 == NULL)
    return 1;

  return strncmp(str1, str2, nchars);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strcasecmp
****************************************/

int mupnp_strcasecmp(const char* str1, const char* str2)
{
  mupnp_log_debug_l5("Entering...\n");

  if (str1 == NULL || str2 == NULL)
    return -1;
#if !defined(WIN32)
  return strcasecmp(str1, str2);
#else
  for (; toupper(*str1) == toupper(*str2); str1++, str2++) {
    if (*str1 == '\0')
      return 0;
  }
  return *str1 - *str2;
#endif

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_streq
****************************************/

bool mupnp_streq(const char* str1, const char* str2)
{
  mupnp_log_debug_l5("Entering...\n");

  if (str1 == NULL || str2 == NULL)
    return false;

  return ((mupnp_strcmp(str1, str2) == 0) ? true : false);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strcaseeq
****************************************/

bool mupnp_strcaseeq(const char* str1, const char* str2)
{
  mupnp_log_debug_l5("Entering...\n");

  if (str1 == NULL || str2 == NULL)
    return false;

  return ((mupnp_strcasecmp(str1, str2) == 0) ? true : false);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strstr
****************************************/

ssize_t mupnp_strstr(const char* haystack, const char* needle)
{
  char* strPos;

  mupnp_log_debug_l5("Entering...\n");

  if (haystack == NULL || needle == NULL)
    return -1;
  strPos = strstr(haystack, needle);
  if (strPos == NULL)
    return -1;
  return (strPos - haystack);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strchr
****************************************/

ssize_t mupnp_strchr(const char* str, const char* chars, size_t nchars)
{
  size_t strLen;
  ssize_t i, j;

  mupnp_log_debug_l5("Entering...\n");

  if (str == NULL || chars == NULL)
    return -1;

  strLen = mupnp_strlen(str);
  for (i = 0; i < strLen; i++) {
    for (j = 0; j < nchars; j++) {
      if (str[i] == chars[j])
        return i;
    }
  }

  return -1;

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strrchr
****************************************/

ssize_t mupnp_strrchr(const char* str, const char* chars, size_t nchars)
{
  size_t strLen;
  ssize_t i, j;

  mupnp_log_debug_l5("Entering...\n");

  if (str == NULL || chars == NULL)
    return -1;

  strLen = mupnp_strlen(str);
  for (i = (strLen - 1); 0 <= i; i--) {
    for (j = 0; j < nchars; j++) {
      if (str[i] == chars[j])
        return i;
    }
  }

  return -1;
}

/****************************************
* mupnp_strtrimwhite
****************************************/

char* mupnp_strtrimwhite(char* str)
{
  size_t strLen;
  ssize_t i;
  strLen = mupnp_strlen(str);
  if (strLen == 0)
    return str;
  for (i = (strLen - 1); 0 <= i; i--) {
    if (isspace(str[i])) {
      strLen--;
    }
  }
  for (i = 0; i < strLen; i++) {
    if (!isspace(str[i]))
      break;
  }

  if (i > 0)
    memmove(str, str + i, strLen - i);

  str[strLen] = 0;
  return str;
}

/****************************************
* mupnp_strtrim
****************************************/

char* mupnp_strtrim(char* str, char* delim, size_t ndelim)
{
  mupnp_log_debug_l5("Entering...\n");

  if (str == NULL || delim == NULL)
    return NULL;

  mupnp_strrtrim(str, delim, ndelim);
  return mupnp_strltrim(str, delim, ndelim);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strltrim
****************************************/

char* mupnp_strltrim(char* str, char* delim, size_t ndelim)
{
  size_t strLen;
  ssize_t i, j;

  mupnp_log_debug_l5("Entering...\n");

  strLen = mupnp_strlen(str);
  for (i = 0; i < strLen; i++) {
    bool hasDelim = false;
    for (j = 0; j < ndelim; j++) {
      if (str[i] == delim[j]) {
        hasDelim = true;
        break;
      }
    }
    if (hasDelim == false)
      return (str + i);
  }

  return (str + strLen);

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strrtrim
****************************************/

char* mupnp_strrtrim(char* str, char* delim, size_t ndelim)
{
  size_t strLen;
  ssize_t i, j;

  mupnp_log_debug_l5("Entering...\n");

  strLen = mupnp_strlen(str);
  for (i = (strLen - 1); 0 <= i; i--) {
    bool hasDelim = false;
    for (j = 0; j < ndelim; j++) {
      if (str[i] == delim[j]) {
        hasDelim = true;
        str[i] = '\0';
        break;
      }
    }
    if (hasDelim == false)
      break;
  }

  return str;

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_strncpy
****************************************/

char* mupnp_strncpy(char* str1, const char* str2, size_t cnt)
{
  mupnp_log_debug_l5("Entering...\n");

  strncpy(str1, str2, cnt);

  mupnp_log_debug_l5("Leaving...\n");

  return str1;
}

/****************************************
* mupnp_strncat
****************************************/

char* mupnp_strncat(char* str1, const char* str2, size_t cnt)
{
  size_t str1Len;
  mupnp_log_debug_l5("Entering...\n");

  str1Len = mupnp_strlen(str1);

  mupnp_log_debug_l5("Leaving...\n");

  return mupnp_strncpy((str1 + str1Len), str2, cnt);
}

/****************************************
* mupnp_int2str
****************************************/

const char* mupnp_int2str(int value, char* buf, size_t bufSize)
{
  mupnp_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%d", value);
#else
  sprintf(buf, "%d", value);
#endif

  mupnp_log_debug_l5("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_long2str
****************************************/

const char* mupnp_long2str(long value, char* buf, size_t bufSize)
{
  mupnp_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%ld", value);
#else
  sprintf(buf, "%ld", value);
#endif

  mupnp_log_debug_l5("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_float2str
****************************************/

const char* mupnp_float2str(float value, char* buf, size_t bufSize)
{
  mupnp_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%f", value);
#else
  sprintf(buf, "%f", value);
#endif

  mupnp_log_debug_l5("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_double2str
****************************************/

const char* mupnp_double2str(double value, char* buf, size_t bufSize)
{
  mupnp_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%lf", value);
#else
  sprintf(buf, "%lf", value);
#endif

  mupnp_log_debug_l5("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_sizet2str
****************************************/

const char* mupnp_sizet2str(size_t value, char* buf, size_t bufSize)
{
  mupnp_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%zd", value);
#else
  sprintf(buf, "%zd", value);
#endif

  mupnp_log_debug_l5("Leaving...\n");

  return buf;
}

/****************************************
* mupnp_ssizet2str
****************************************/

const char* mupnp_ssizet2str(ssize_t value, char* buf, size_t bufSize)
{
  mupnp_log_debug_l5("Entering...\n");

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize, "%zd", value);
#else
  sprintf(buf, "%zd", value);
#endif

  mupnp_log_debug_l5("Leaving...\n");

  return buf;
}
