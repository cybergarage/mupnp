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

/**
 * @brief Duplicate a string (NULL-safe wrapper for strdup)
 *
 * @details
 * Allocates memory and creates a copy of the input string.
 * The caller is responsible for freeing the returned string with free().
 *
 * @param str The string to duplicate. May be NULL.
 *
 * @return A newly-allocated copy of str, or NULL if str is NULL or
 *         memory allocation fails.
 *
 * @note Thread-safe: Can be called from any thread.
 * @note The returned string must be freed with free() to avoid memory leaks.
 *
 * @see free()
 */
char* mupnp_strdup(const char* str);

/**
 * @brief Get the length of a string (NULL-safe wrapper for strlen)
 *
 * @param str The string to measure. May be NULL.
 *
 * @return The length of str (excluding null terminator), or 0 if str is NULL.
 *
 * @note Thread-safe: Can be called from any thread.
 */
size_t mupnp_strlen(const char* str);

/**
 * @brief Copy a string (NULL-safe wrapper for strcpy)
 *
 * @param dest Destination buffer. Must not be NULL and must be large enough.
 * @param src Source string. May be NULL (dest will be set to empty string).
 *
 * @return dest pointer.
 *
 * @note Thread-safe: Can be called from any thread.
 * @warning No bounds checking is performed. Ensure dest is large enough
 *          to hold src plus null terminator.
 */
char* mupnp_strcpy(char* dest, const char* src);

/**
 * @brief Concatenate strings (NULL-safe wrapper for strcat)
 *
 * @param dest Destination buffer. Must not be NULL and must have sufficient space.
 * @param src Source string to append. May be NULL (no-op if NULL).
 *
 * @return dest pointer.
 *
 * @note Thread-safe: Can be called from any thread.
 * @warning No bounds checking is performed. Ensure dest has enough space
 *          for the concatenated result plus null terminator.
 */
char* mupnp_strcat(char* dest, const char* src);

/**
 * @brief Compare two strings (NULL-safe wrapper for strcmp)
 *
 * @param str1 First string. May be NULL.
 * @param str2 Second string. May be NULL.
 *
 * @return Negative if str1 < str2, 0 if equal, positive if str1 > str2.
 *         NULL is considered less than any non-NULL string.
 *
 * @note Thread-safe: Can be called from any thread.
 */
int mupnp_strcmp(const char* str1, const char* str2);

/**
 * @brief Compare first n characters of two strings (NULL-safe)
 *
 * @param str1 First string. May be NULL.
 * @param str2 Second string. May be NULL.
 * @param nchars Maximum number of characters to compare.
 *
 * @return Negative if str1 < str2, 0 if equal, positive if str1 > str2.
 *
 * @note Thread-safe: Can be called from any thread.
 */
int mupnp_strncmp(const char* str1, const char* str2, int nchars);

/**
 * @brief Compare two strings case-insensitively (NULL-safe)
 *
 * @param str1 First string. May be NULL.
 * @param str2 Second string. May be NULL.
 *
 * @return Negative if str1 < str2, 0 if equal, positive if str1 > str2
 *         (case-insensitive comparison).
 *
 * @note Thread-safe: Can be called from any thread.
 */
int mupnp_strcasecmp(const char* str1, const char* str2);

/**
 * @brief Test if two strings are equal (NULL-safe)
 *
 * @param str1 First string. May be NULL.
 * @param str2 Second string. May be NULL.
 *
 * @return true if strings are equal, false otherwise.
 *         Two NULL strings are considered equal.
 *
 * @note Thread-safe: Can be called from any thread.
 */
bool mupnp_streq(const char* str1, const char* str2);

/**
 * @brief Test if two strings are equal case-insensitively (NULL-safe)
 *
 * @param str1 First string. May be NULL.
 * @param str2 Second string. May be NULL.
 *
 * @return true if strings are equal (case-insensitive), false otherwise.
 *
 * @note Thread-safe: Can be called from any thread.
 */
bool mupnp_strcaseeq(const char* str1, const char* str2);

/**
 * @brief Find first occurrence of any character from a set (strcspn variant)
 *
 * @param str The string to search. Must not be NULL.
 * @param chars Array of characters to search for. Must not be NULL.
 * @param nchars Number of characters in chars array.
 *
 * @return Index of first matching character, or -1 if not found.
 *
 * @note Thread-safe: Can be called from any thread.
 */
ssize_t mupnp_strchr(const char* str, const char* chars, size_t nchars);

/**
 * @brief Find last occurrence of any character from a set
 *
 * @param str The string to search. Must not be NULL.
 * @param chars Array of characters to search for. Must not be NULL.
 * @param nchars Number of characters in chars array.
 *
 * @return Index of last matching character, or -1 if not found.
 *
 * @note Thread-safe: Can be called from any thread.
 */
ssize_t mupnp_strrchr(const char* str, const char* chars, size_t nchars);

/**
 * @brief Find substring (NULL-safe wrapper for strstr)
 *
 * @param haystack The string to search in. May be NULL.
 * @param needle The substring to search for. May be NULL.
 *
 * @return Index of first occurrence of needle in haystack, or -1 if not found
 *         or if either parameter is NULL.
 *
 * @note Thread-safe: Can be called from any thread.
 */
ssize_t mupnp_strstr(const char* haystack, const char* needle);

/**
 * @brief Trim whitespace from both ends of a string (in-place)
 *
 * @details
 * Removes leading and trailing whitespace characters (space, tab, newline,
 * carriage return) from the string by modifying it in place.
 *
 * @param str The string to trim. Must not be NULL. Will be modified.
 *
 * @return Pointer to the trimmed string (same as str, but offset to start
 *         of non-whitespace).
 *
 * @note Thread-safe: Can be called from any thread.
 * @note Side effect: Modifies the input string in place.
 */
char* mupnp_strtrimwhite(char* str);

/**
 * @brief Trim specified delimiter characters from both ends (in-place)
 *
 * @param str The string to trim. Must not be NULL. Will be modified.
 * @param delim Array of delimiter characters to remove. Must not be NULL.
 * @param ndelim Number of characters in delim array.
 *
 * @return Pointer to the trimmed string.
 *
 * @note Thread-safe: Can be called from any thread.
 * @note Side effect: Modifies the input string in place.
 */
char* mupnp_strtrim(char* str, char* delim, size_t ndelim);

/**
 * @brief Trim delimiter characters from the left (start) of string (in-place)
 *
 * @param str The string to trim. Must not be NULL. Will be modified.
 * @param delim Array of delimiter characters to remove. Must not be NULL.
 * @param ndelim Number of characters in delim array.
 *
 * @return Pointer to the trimmed string.
 *
 * @note Thread-safe: Can be called from any thread.
 * @note Side effect: Modifies the input string in place.
 */
char* mupnp_strltrim(char* str, char* delim, size_t ndelim);

/**
 * @brief Trim delimiter characters from the right (end) of string (in-place)
 *
 * @param str The string to trim. Must not be NULL. Will be modified.
 * @param delim Array of delimiter characters to remove. Must not be NULL.
 * @param ndelim Number of characters in delim array.
 *
 * @return Pointer to the trimmed string (same as str).
 *
 * @note Thread-safe: Can be called from any thread.
 * @note Side effect: Modifies the input string in place.
 */
char* mupnp_strrtrim(char* str, char* delim, size_t ndelim);

/**
 * @brief Copy at most n characters from one string to another (NULL-safe)
 *
 * @param str1 Destination buffer. Must not be NULL and must have space for cnt+1 bytes.
 * @param str2 Source string. May be NULL (str1 will be set to empty string).
 * @param cnt Maximum number of characters to copy (excluding null terminator).
 *
 * @return str1 pointer.
 *
 * @note Thread-safe: Can be called from any thread.
 * @note Always null-terminates the destination string.
 */
char* mupnp_strncpy(char* str1, const char* str2, size_t cnt);

/**
 * @brief Append at most n characters from one string to another (NULL-safe)
 *
 * @param str1 Destination buffer. Must not be NULL and must have sufficient space.
 * @param str2 Source string to append. May be NULL (no-op if NULL).
 * @param cnt Maximum number of characters to append (excluding null terminator).
 *
 * @return str1 pointer.
 *
 * @note Thread-safe: Can be called from any thread.
 */
char* mupnp_strncat(char* str1, const char* str2, size_t cnt);

/**
 * @brief Convert integer to string
 *
 * @param value The integer value to convert.
 * @param buf Buffer to store the result. Must not be NULL.
 * @param bufSize Size of buf in bytes. Should be at least
 *                MUPNP_STRING_INTEGER_BUFLEN (16) to safely hold any int.
 *
 * @return Pointer to buf containing the converted string.
 *
 * @note Thread-safe: Can be called from any thread.
 * @note The buffer is always null-terminated if bufSize > 0.
 */
const char* mupnp_int2str(int value, char* buf, size_t bufSize);

/**
 * @brief Convert long integer to string
 *
 * @param value The long value to convert.
 * @param buf Buffer to store the result. Must not be NULL.
 * @param bufSize Size of buf in bytes. Should be at least
 *                MUPNP_STRING_LONG_BUFLEN (32) to safely hold any long.
 *
 * @return Pointer to buf containing the converted string.
 *
 * @note Thread-safe: Can be called from any thread.
 */
const char* mupnp_long2str(long value, char* buf, size_t bufSize);

/**
 * @brief Convert float to string
 *
 * @param value The float value to convert.
 * @param buf Buffer to store the result. Must not be NULL.
 * @param bufSize Size of buf in bytes. Should be at least
 *                MUPNP_STRING_FLOAT_BUFLEN (64) to safely hold any float.
 *
 * @return Pointer to buf containing the converted string.
 *
 * @note Thread-safe: Can be called from any thread.
 */
const char* mupnp_float2str(float value, char* buf, size_t bufSize);

/**
 * @brief Convert double to string
 *
 * @param value The double value to convert.
 * @param buf Buffer to store the result. Must not be NULL.
 * @param bufSize Size of buf in bytes. Should be at least
 *                MUPNP_STRING_DOUBLE_BUFLEN (64) to safely hold any double.
 *
 * @return Pointer to buf containing the converted string.
 *
 * @note Thread-safe: Can be called from any thread.
 */
const char* mupnp_double2str(double value, char* buf, size_t bufSize);

/**
 * @brief Convert size_t to string
 *
 * @param value The size_t value to convert.
 * @param buf Buffer to store the result. Must not be NULL.
 * @param bufSize Size of buf in bytes. Should be at least
 *                MUPNP_STRING_LONG_BUFLEN (32) bytes.
 *
 * @return Pointer to buf containing the converted string.
 *
 * @note Thread-safe: Can be called from any thread.
 */
const char* mupnp_sizet2str(size_t value, char* buf, size_t bufSize);

/**
 * @brief Convert ssize_t to string
 *
 * @param value The ssize_t value to convert.
 * @param buf Buffer to store the result. Must not be NULL.
 * @param bufSize Size of buf in bytes. Should be at least
 *                MUPNP_STRING_LONG_BUFLEN (32) bytes.
 *
 * @return Pointer to buf containing the converted string.
 *
 * @note Thread-safe: Can be called from any thread.
 */
const char* mupnp_ssizet2str(ssize_t value, char* buf, size_t bufSize);

/**
 * @brief Convert string to integer (NULL-safe)
 *
 * @param value String to convert. May be NULL.
 *
 * @return Converted integer value, or 0 if value is NULL or invalid.
 *
 * @note Thread-safe macro.
 */
#define mupnp_str2int(value) (value ? atoi(value) : 0)

/**
 * @brief Convert string to long (NULL-safe)
 *
 * @param value String to convert. May be NULL.
 *
 * @return Converted long value, or 0 if value is NULL or invalid.
 *
 * @note Thread-safe macro.
 */
#define mupnp_str2long(value) (value ? atol(value) : 0)

/**
 * @brief Convert hexadecimal string to long (NULL-safe)
 *
 * @param value Hexadecimal string to convert (e.g., "1A3F"). May be NULL.
 *
 * @return Converted long value, or 0 if value is NULL or invalid.
 *
 * @note Thread-safe macro.
 */
#define mupnp_strhex2long(value) (value ? strtol(value, NULL, 16) : 0)

/**
 * @brief Convert hexadecimal string to unsigned long (NULL-safe)
 *
 * @param value Hexadecimal string to convert. May be NULL.
 *
 * @return Converted unsigned long value, or 0 if value is NULL or invalid.
 *
 * @note Thread-safe macro.
 */
#define mupnp_strhex2ulong(value) (value ? strtoul(value, NULL, 16) : 0)

/**
 * @brief Convert string to float (NULL-safe)
 *
 * @param value String to convert. May be NULL.
 *
 * @return Converted float value, or 0.0 if value is NULL or invalid.
 *
 * @note Thread-safe macro.
 */
#define mupnp_str2float(value) ((float)(value ? atof(value) : 0.0))

/**
 * @brief Convert string to double (NULL-safe)
 *
 * @param value String to convert. May be NULL.
 *
 * @return Converted double value, or 0.0 if value is NULL or invalid.
 *
 * @note Thread-safe macro.
 */
#define mupnp_str2double(value) (value ? atof(value) : 0.0)

/**
 * @brief Convert string to size_t (NULL-safe)
 *
 * @param value String to convert. May be NULL.
 *
 * @return Converted size_t value, or 0 if value is NULL or invalid.
 *
 * @note Thread-safe macro.
 */
#define mupnp_str2sizet(value) ((size_t)(value ? atol(value) : 0))

/**
 * @brief Convert string to ssize_t (NULL-safe)
 *
 * @param value String to convert. May be NULL.
 *
 * @return Converted ssize_t value, or 0 if value is NULL or invalid.
 *
 * @note Thread-safe macro.
 */
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

mUpnpString* mupnp_string_new(void);
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
