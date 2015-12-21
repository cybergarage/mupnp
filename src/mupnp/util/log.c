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

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include <mupnp/util/log.h>
#include <mupnp/util/string.h>
#include <mupnp/util/mutex.h>

#if defined(WIN32)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

static const char* sev_error_s = SEV_ERROR_S;
static const char* sev_warning_s = SEV_WARNING_S;
static const char* sev_info_s = SEV_INFO_S;
static const char* sev_debug_l1_s = SEV_DEBUG_L1_S;
static const char* sev_debug_l2_s = SEV_DEBUG_L2_S;
static const char* sev_debug_l3_s = SEV_DEBUG_L3_S;
static const char* sev_debug_l4_s = SEV_DEBUG_L4_S;
static const char* sev_debug_l5_s = SEV_DEBUG_L5_S;

static const char* sev_unknown_s = SEV_UNKNOWN_S;

static void log_init_with_defaults();
static const char* map_severity(int severity);

struct fd_list {
  struct fd_list* next;
  FILE* fd;
  char* name;
  int apply_mask;
};

static struct fd_list* descriptor_list = NULL; /* Contains logging targets (single linked list) */
static int initialized = 0;
static char* separator = NULL; /* Log item separator */

static mUpnpMutex*(print_mutex) = NULL;

/* Local helper functions */

static void log_init_with_defaults()
{
  mupnp_log_add_target("stdout", SEV_INFO | SEV_DEBUG_L1 | SEV_DEBUG_L2 | SEV_DEBUG_L3);
  mupnp_log_add_target("stderr", SEV_ERROR | SEV_WARNING);
}

static const char* map_severity(int severity)
{
  switch (severity) {
  case SEV_ERROR:
    return sev_error_s;
    break;

  case SEV_WARNING:
    return sev_warning_s;
    break;

  case SEV_INFO:
    return sev_info_s;
    break;

  case SEV_DEBUG_L1:
    return sev_debug_l1_s;
    break;

  case SEV_DEBUG_L2:
    return sev_debug_l2_s;
    break;

  case SEV_DEBUG_L3:
    return sev_debug_l3_s;
    break;

  case SEV_DEBUG_L4:
    return sev_debug_l4_s;
    break;

  case SEV_DEBUG_L5:
    return sev_debug_l5_s;
    break;

  default:
    return sev_unknown_s;
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
  struct fd_list* temp = NULL;
  FILE* r_target = NULL;

  initialized = 1;

  /* Checking if target is standard stream or should we create
	 * new stream for file output.
	 */
  if (!strcmp(target, "stdout"))
    r_target = stdout;
  else if (!strcmp(target, "stderr"))
    r_target = stderr;
  else {
    /* Try to use existing fd */
    for (temp = descriptor_list; temp; temp = temp->next) {
      if (!strcmp(target, temp->name))
        r_target = temp->fd;
    }

    /* User is adding new file for output, note that file is cleared if
		 * it is not already open. */
    if (NULL == r_target)
      r_target = fopen(target, "w");
  }

  if (NULL == (temp = (struct fd_list*)malloc(sizeof(struct fd_list)))) {
    return -1;
  }

  /* Adding new target into single linked list */
  temp->next = descriptor_list;
  temp->apply_mask = mask;
  temp->name = mupnp_strdup(target);
  temp->fd = r_target;
  descriptor_list = temp;

  return 1;
}

/**
 * Clear target list
 *
 */
int mupnp_log_clear_targets()
{
  struct fd_list* temp = descriptor_list;

  while (temp) {
    /* We won't close standard streams... */
    if (strcmp(temp->name, "stdout") && strcmp(temp->name, "stdin"))
      fclose(temp->fd);

    free(temp->name);
    descriptor_list = temp->next;
    free(temp);
    temp = descriptor_list;
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
void mupnp_log_print(int severity, const char* file, int line_n, const char* function, const char* format, ...)
{
  va_list list;

  char log_line[MAX_LOG_STRING], *l_ptr, t_ptr[MAX_LOG_STRING];
  int prefix_length = -1;
  struct fd_list* temp = NULL;
  size_t timestamp;
  struct tm* timestamp_human_readable;

  /* If output targets are empty, do return */
  if (!descriptor_list)
    return;

  /* If logger is not initialized, do it now */
  if (!initialized)
    log_init_with_defaults();

  /* If separator is not set, do it now */
  if (NULL == separator)
    mupnp_log_set_separator(" : ");

  /* Create a mutex */
  if (!print_mutex)
    print_mutex = mupnp_mutex_new();
  mupnp_mutex_lock(print_mutex);

  /* Create timestamp for the log prefix */
  timestamp = time(NULL);
  timestamp_human_readable = localtime(&timestamp);

#if !defined(WIN32)
  strftime(t_ptr, MAX_LOG_STRING, "%c", timestamp_human_readable);
#else
  snprintf(log_line, MAX_LOG_STRING, "%d-%d-%d %d:%d %d", timestamp_human_readable->tm_year + 1900, timestamp_human_readable->tm_mon + 1, timestamp_human_readable->tm_mday, timestamp_human_readable->tm_hour, timestamp_human_readable->tm_min, timestamp_human_readable->tm_sec);
#endif
  /* Creating the full log prefix */
  prefix_length = snprintf(log_line, MAX_LOG_STRING, "%s%s%s%s%s%s%d%s%s%s ", t_ptr, separator, map_severity(severity), separator, file, separator, line_n, separator, function, separator);

  /* Setting pointer where the actual message should start */
  l_ptr = &log_line[prefix_length];

  /* Filling out rest of the log message */
  va_start(list, format);
  vsnprintf(log_line + prefix_length, MAX_LOG_STRING - prefix_length, format, list);
  va_end(list);

  /* Multiplexing the created message into targets */
  for (temp = descriptor_list; temp; temp = temp->next) {
    if (severity == (severity & temp->apply_mask)) {
      fputs(log_line, temp->fd);
    }
  }

  mupnp_mutex_unlock(print_mutex);
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
