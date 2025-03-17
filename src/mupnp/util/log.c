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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <mupnp/util/log.h>
#include <mupnp/util/mutex.h>
#include <mupnp/util/string.h>

#if defined(WIN32)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

static const char* sevErrorS = SEV_ERROR_S;
static const char* sevWarningS = SEV_WARNING_S;
static const char* sevInfoS = SEV_INFO_S;
static const char* sevDebugL1S = SEV_DEBUG_L1_S;
static const char* sevDebugL2S = SEV_DEBUG_L2_S;
static const char* sevDebugL3S = SEV_DEBUG_L3_S;
static const char* sevDebugL4S = SEV_DEBUG_L4_S;
static const char* sevDebugL5S = SEV_DEBUG_L5_S;

static const char* sevUnknownS = SEV_UNKNOWN_S;

static void log_init_with_defaults(void);
static const char* map_severity(int severity);

struct FdList {
  struct FdList* next;
  FILE* fd;
  char* name;
  int applyMask;
};

static struct FdList* descriptorList = NULL; /* Contains logging targets (single linked list) */
static int initialized = 0;
static char* separator = NULL; /* Log item separator */

static mUpnpMutex*(printMutex) = NULL;

/* Local helper functions */

static void log_init_with_defaults(void)
{
  mupnp_log_add_target("stdout", SEV_INFO | SEV_DEBUG_L1 | SEV_DEBUG_L2 | SEV_DEBUG_L3);
  mupnp_log_add_target("stderr", SEV_ERROR | SEV_WARNING);
}

static const char* map_severity(int severity)
{
  switch (severity) {
  case SEV_ERROR:
    return sevErrorS;
    break;

  case SEV_WARNING:
    return sevWarningS;
    break;

  case SEV_INFO:
    return sevInfoS;
    break;

  case SEV_DEBUG_L1:
    return sevDebugL1S;
    break;

  case SEV_DEBUG_L2:
    return sevDebugL2S;
    break;

  case SEV_DEBUG_L3:
    return sevDebugL3S;
    break;

  case SEV_DEBUG_L4:
    return sevDebugL4S;
    break;

  case SEV_DEBUG_L5:
    return sevDebugL5S;
    break;

  default:
    return sevUnknownS;
  }
}

/* Public interface functions */

/**
 * Add new logging target
 * @param target String identifying the logging target (Currently stdout|stderr|FILENAME)
 * @param mask Bitmask defining what messages are to be printed into this target
 */
int mupnp_log_add_target(char* target, int mask)
{
  struct FdList* temp = NULL;
  FILE* rTarget = NULL;

  initialized = 1;

  /* Checking if target is standard stream or should we create
   * new stream for file output.
   */
  if (!strcmp(target, "stdout"))
    rTarget = stdout;
  else if (!strcmp(target, "stderr"))
    rTarget = stderr;
  else {
    /* Try to use existing fd */
    for (temp = descriptorList; temp; temp = temp->next) {
      if (!strcmp(target, temp->name))
        rTarget = temp->fd;
    }

    /* User is adding new file for output, note that file is cleared if
     * it is not already open. */
    if (NULL == rTarget)
      rTarget = fopen(target, "w");
  }

  if (NULL == (temp = (struct FdList*)malloc(sizeof(struct FdList)))) {
    return -1;
  }

  /* Adding new target into single linked list */
  temp->next = descriptorList;
  temp->applyMask = mask;
  temp->name = mupnp_strdup(target);
  temp->fd = rTarget;
  descriptorList = temp;

  return 1;
}

/**
 * Clear target list
 *
 */
int mupnp_log_clear_targets(void)
{
  struct FdList* temp = descriptorList;

  while (temp) {
    /* We won't close standard streams... */
    if (strcmp(temp->name, "stdout") && strcmp(temp->name, "stdin"))
      fclose(temp->fd);

    free(temp->name);
    descriptorList = temp->next;
    free(temp);
    temp = descriptorList;
  }

  return 1;
}

/**
 * Set log item separator
 * @param s String to use as a log item separator
 */
void mupnp_log_set_separator(char* s)
{
  if (separator != NULL)
    free(separator);

  separator = mupnp_strdup(s);
}

/**
 * Multiplex log messages into different targets (streams), should be used via convenience macros
 * @param severity Message severity
 * @param file File name where the function is called
 * @param line_n Line number where the function is called
 * @param function Function name where this function is called
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */
void mupnp_log_print(int severity, const char* file, int lineN, const char* function, const char* format, ...)
{
  va_list list;

  char logLine[MAX_LOG_STRING], *lPtr, tPtr[MAX_LOG_STRING];
  int prefixLength = -1;
  struct FdList* temp = NULL;
  long timestamp;
  struct tm* timestampHumanReadable;

  /* If output targets are empty, do return */
  if (!descriptorList)
    return;

  /* If logger is not initialized, do it now */
  if (!initialized)
    log_init_with_defaults();

  /* If separator is not set, do it now */
  if (NULL == separator)
    mupnp_log_set_separator(" : ");

  /* Create a mutex */
  if (!printMutex)
    printMutex = mupnp_mutex_new();
  mupnp_mutex_lock(printMutex);

  /* Create timestamp for the log prefix */
  timestamp = time(NULL);
  timestampHumanReadable = localtime(&timestamp);

#if !defined(WIN32)
  strftime(tPtr, MAX_LOG_STRING, "%c", timestampHumanReadable);
#else
  snprintf(log_line, MAX_LOG_STRING, "%d-%d-%d %d:%d %d", timestamp_human_readable->tm_year + 1900, timestamp_human_readable->tm_mon + 1, timestamp_human_readable->tm_mday, timestamp_human_readable->tm_hour, timestamp_human_readable->tm_min, timestamp_human_readable->tm_sec);
#endif
  /* Creating the full log prefix */
  prefixLength = snprintf(logLine, MAX_LOG_STRING, "%s%s%s%s%s%s%d%s%s%s ", tPtr, separator, map_severity(severity), separator, file, separator, lineN, separator, function, separator);

  /* Setting pointer where the actual message should start */
  lPtr = &logLine[prefixLength];

  /* Filling out rest of the log message */
  va_start(list, format);
  vsnprintf(lPtr, MAX_LOG_STRING - prefixLength, format, list);
  va_end(list);

  /* Multiplexing the created message into targets */
  for (temp = descriptorList; temp; temp = temp->next) {
    if (severity == (severity & temp->applyMask)) {
      fputs(logLine, temp->fd);
    }
  }

  mupnp_mutex_unlock(printMutex);
}

#if defined(WIN32)

/**
 * Outpu error messages
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */
void mupnp_log_error(const char* format, ...)
{
  /*
        va_list args;
        char *fmt;
        char buf[256];

        va_start(args);
        fmt=va_arg(args,char *);
        //vsprintf(buf,fmt,args);
        va_end(args);
*/
}

/**
 * Output warning messages
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */
void mupnp_log_warning(const char* format, ...)
{
}

/**
 * Output log messages
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */
void mupnp_log_info(const char* format, ...)
{
}

/**
 * Output debug messages
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */
void mupnp_log_debug(const char* format, ...)
{
}

/**
 * Output debug l4  messages
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */
void mupnp_log_debug_l4(const char* format, ...)
{
}

/**
 * Output debug l5 messages
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */
void mupnp_log_debug_l5(const char* format, ...)
{
}

/**
 * Output debug service messages
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */

void mupnp_log_debug_s(const char* format, ...)
{
}

/**
 * Output debug application messages
 * @param format Format string for the actual log message
 * @param ... Possible parameters for the format string
 */
void mupnp_log_debug_a(const char* format, ...)
{
}

#endif
