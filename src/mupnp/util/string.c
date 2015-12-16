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

#include <mupnp/util/string.h>
#include <mupnp/util/log.h>

#include <string.h>

/* Define amount of extra characters allocated on each realloc, with this we
   can avoid many small subsequent reallocs, which takes lots of time */
#define MUPNP_STRING_REALLOC_EXTRA 16

/****************************************
* mupnp_string_new
****************************************/

mUpnpString* mupnp_string_new()
{
  mUpnpString* str;

  mupnp_log_debug_l5("Entering...\n");

  str = (mUpnpString*)malloc(sizeof(mUpnpString));

  if (NULL != str) {
    str->value = NULL;
    str->memSize = 0;
    str->valueSize = 0;
  }

  mupnp_log_debug_l5("Leaving...\n");

  return str;
}

/****************************************
* mupnp_string_delete
****************************************/

void mupnp_string_delete(mUpnpString* str)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL != str) {
    mupnp_string_clear(str);
    free(str);
  }

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_string_delete
****************************************/

void mupnp_string_clear(mUpnpString* str)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL != str) {
    if (str->value != NULL) {
      free(str->value);
      str->value = NULL;
      str->memSize = 0;
      str->valueSize = 0;
    }
  }

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_string_setvalue
****************************************/

void mupnp_string_setvalue(mUpnpString* str, const char* value)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL != str) {
    if (value != NULL)
      mupnp_string_setnvalue(str, value, mupnp_strlen(value));
  }

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_string_setintvalue
****************************************/

void mupnp_string_setintvalue(mUpnpString* str, int value)
{
  char buf[MUPNP_STRING_INTEGER_BUFLEN];

  mupnp_log_debug_l5("Entering...\n");

  mupnp_string_setvalue(str, mupnp_int2str(value, buf, sizeof(buf)));

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_string_setlongvalue
****************************************/

void mupnp_string_setlongvalue(mUpnpString* str, long value)
{
  char buf[MUPNP_STRING_LONG_BUFLEN];

  mupnp_log_debug_l5("Entering...\n");

  mupnp_string_setvalue(str, mupnp_long2str(value, buf, sizeof(buf)));

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_string_setnvalue
****************************************/

void mupnp_string_setnvalue(mUpnpString* str, const char* value, size_t len)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL != str) {
    mupnp_string_clear(str);
    if (value != NULL) {
      str->valueSize = len;
      str->memSize = str->valueSize + 1;
      str->value = (char*)malloc(str->memSize * sizeof(char));

      if (NULL == str->value) {
        mupnp_log_debug_s("Memory allocation failure!\n");
        return;
      }

      /* memcpy works better with non-zero-terminated data than strncpy */
      memcpy(str->value, value, len);
      str->value[len] = '\0';
    }
  }

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_string_setpointervalue
****************************************/

void mupnp_string_setpointervalue(mUpnpString* str, char* value, size_t len)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL != str) {
    mupnp_string_clear(str);
    str->value = value;
    str->valueSize = len;
    str->memSize = str->valueSize + 1;
  }

  mupnp_log_debug_l5("Leaving...\n");
}

/****************************************
* mupnp_string_getvalue
****************************************/

char* mupnp_string_getvalue(mUpnpString* str)
{
  mupnp_log_debug_l5("Entering...\n");

  mupnp_log_debug_l5("Leaving...\n");

  return (NULL != str) ? str->value : NULL;
}

/****************************************
* mupnp_string_getmemorysize
****************************************/

size_t mupnp_string_getmemorysize(mUpnpString* str)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == str)
    return 0;

  mupnp_log_debug_l5("Leaving...\n");

  return str->memSize;
}

/****************************************
* mupnp_string_length
****************************************/

size_t mupnp_string_length(mUpnpString* str)
{
  mupnp_log_debug_l5("Entering...\n");

  if (NULL == str)
    return 0;

  if (str->value == NULL)
    return 0;

  mupnp_log_debug_l5("Leaving...\n");

  return str->valueSize;
}

/****************************************
* mupnp_string_add
****************************************/

char* mupnp_string_addvalue(mUpnpString* str, const char* value)
{
  mupnp_log_debug_l5("Entering...\n");

  mupnp_log_debug_l5("Leaving...\n");

  return mupnp_string_naddvalue(str, value, mupnp_strlen(value));
}

/****************************************
* mupnp_string_add
****************************************/

char* mupnp_string_naddvalue(mUpnpString* str, const char* value, size_t valueLen)
{
  char* newValue = NULL;
  size_t newMemSize = 0;

  mupnp_log_debug_l5("Entering...\n");

  if (NULL == str)
    return NULL;

  if (value == NULL || valueLen <= 0) {
    /* Empty string, nothing to add */
    return mupnp_string_getvalue(str);
  }

  /* Check, if we need to allocate memory for the new data */
  newMemSize = str->valueSize + valueLen + 1;
  if (newMemSize > str->memSize || str->value == NULL) {
    /* realloc also some extra in order to avoid multiple reallocs */
    newMemSize += MUPNP_STRING_REALLOC_EXTRA;
    newValue = realloc(str->value, newMemSize * sizeof(char));

    if (newValue == NULL) {
      /* Memory allocation failed, bail out */
      return NULL;
    }

    str->memSize = newMemSize;
    str->value = newValue;
  }

  /* memcpy works better with non-zero-terminated data
	   than strncpy */
  memcpy(str->value + str->valueSize, value, valueLen);

  str->valueSize += valueLen;

  /* In case this is a string, append a termination character */
  str->value[str->valueSize] = '\0';

  mupnp_log_debug_l5("Leaving...\n");

  return mupnp_string_getvalue(str);
}

/****************************************
* mupnp_string_addrep
****************************************/

char* mupnp_string_addrepvalue(mUpnpString* str, const char* value, size_t repeatCnt)
{
  int n;
  mupnp_log_debug_l5("Entering...\n");

  for (n = 0; n < repeatCnt; n++)
    mupnp_string_addvalue(str, value);

  mupnp_log_debug_l5("Leaving...\n");

  return mupnp_string_getvalue(str);
}

/****************************************
* mupnp_string_naddrep
****************************************/

char* mupnp_string_naddrepvalue(mUpnpString* str, const char* value, size_t valueLen, size_t repeatCnt)
{
  int n;

  mupnp_log_debug_l5("Entering...\n");

  for (n = 0; n < repeatCnt; n++)
    mupnp_string_naddvalue(str, value, valueLen);

  mupnp_log_debug_l5("Leaving...\n");

  return mupnp_string_getvalue(str);
}

/****************************************
* mupnp_string_replace
****************************************/

char* mupnp_string_replace(mUpnpString* str, char* fromStr[], char* toStr[], size_t fromStrCnt)
{
  char* orgValue = NULL;
  size_t orgValueLen = 0;
  int n = 0;
  int copyPos = 0;
  size_t* fromStrLen = NULL;
  mUpnpString* repValue = NULL;
  bool isReplaced = false;

  mupnp_log_debug_l5("Entering...\n");

  if (NULL == str)
    return NULL;

  repValue = mupnp_string_new();

  fromStrLen = (size_t*)malloc(sizeof(size_t) * fromStrCnt);

  if (NULL == fromStrLen) {
    mupnp_string_delete(repValue);
    mupnp_log_debug_s("Memory allocation failure!\n");
    return NULL;
  }

  for (n = 0; n < fromStrCnt; n++)
    fromStrLen[n] = mupnp_strlen(fromStr[n]);

  orgValue = mupnp_string_getvalue(str);
  orgValueLen = mupnp_string_length(str);

  copyPos = 0;
  while (copyPos < orgValueLen) {
    isReplaced = false;
    for (n = 0; n < fromStrCnt; n++) {
      if (strncmp(fromStr[n], orgValue + copyPos, fromStrLen[n]) == 0) {
        mupnp_string_addvalue(repValue, toStr[n]);
        copyPos += fromStrLen[n];
        isReplaced = true;
        continue;
      }
    }
    if (isReplaced == true)
      continue;
    mupnp_string_naddvalue(repValue, orgValue + copyPos, 1);
    copyPos++;
  }

  free(fromStrLen);

  mupnp_string_setvalue(str, mupnp_string_getvalue(repValue));

  mupnp_string_delete(repValue);

  mupnp_log_debug_l5("Leaving...\n");

  return mupnp_string_getvalue(str);
}
