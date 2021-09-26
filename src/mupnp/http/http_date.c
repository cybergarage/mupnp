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

#include <time.h>

#include <mupnp/http/http.h>
#include <mupnp/util/log.h>
#include <mupnp/util/time.h>

/****************************************
* to_month_string()
****************************************/

static char MONTH_STRING[][4] = {
  "Jan",
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec",
};

static char* to_month_string(int value)
{
  mupnp_log_debug_l4("Entering...\n");

  if (0 <= value && value < 12)
    return MONTH_STRING[value];
  return "";
}

/****************************************
* to_week_string()
****************************************/

static char WEEK_STRING[][4] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
};

static char* to_week_string(int value)
{
  mupnp_log_debug_l4("Entering...\n");

  if (0 <= value && value < 7)
    return WEEK_STRING[value];
  return "";

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
* mupnp_http_getservername()
****************************************/

#if !defined(ITRON)

const char* mupnp_http_getdate(mUpnpTime sysTime, char* buf, size_t bufSize)
{
#if defined(HAVE_GMTIME_R)
  struct tm gmTimeBuf;
  struct tm* gmTime = &gmTimeBuf;
#elif defined(WINCE)
  SYSTEMTIME systemTime;
#else
  struct tm* gmTime;
#endif

  mupnp_log_debug_l4("Entering...\n");

#if defined(HAVE_GMTIME_R)
  gmtime_r(&sysTime, &gmTimeBuf);
#elif defined(WINCE)
  GetSystemTime(&systemTime);
#else
  gmTime = gmtime(&sysTime);
#endif

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize,
#else
  sprintf(buf,
#endif
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
#if defined(HAVE_GMTIME_R)
      to_week_string(gmTime->tm_wday),
      gmTime->tm_mday,
      to_month_string(gmTime->tm_mon),
      gmTime->tm_year + 1900,
      gmTime->tm_hour,
      gmTime->tm_min,
      gmTime->tm_sec);
#elif defined(WINCE)
      to_week_string(systemTime.wDayOfWeek),
      systemTime.wDay,
      to_month_string(systemTime.wMonth - 1),
      systemTime.wYear,
      systemTime.wHour,
      systemTime.wMinute,
      systemTime.wSecond);
#else
      to_week_string(gmTime->tm_wday),
      gmTime->tm_mday,
      to_month_string(gmTime->tm_mon),
      gmTime->tm_year + 1900,
      gmTime->tm_hour,
      gmTime->tm_min,
      gmTime->tm_sec);
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return buf;
}

#endif

/****************************************
* mupnp_http_getservername() (ITRON)
****************************************/

#if defined(ITRON)

static const int dayYear[] = {
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const int dayLearYear[] = {
  31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static bool IsLeapYear(int year)
{
  mupnp_log_debug_l4("Entering...\n");

  if (!(year % 4) && ((year % 100) || !(year % 400)))
    return true;
  return false;

  mupnp_log_debug_l4("Leaving...\n");
}

char* mupnp_http_getdate(mUpnpTime sysTime, char* buf, int bufSize)
{
  SYSTIM systim;
  long uxtime;
  int year, month, day, hour, min, sec, week;

  mupnp_log_debug_l4("Entering...\n");

  get_tim(&systim);

  uxtime = (systim.ltime / 1000) + ((systim.utime / 1000) << 32);

  sec = uxtime % 60;
  uxtime -= sec;
  uxtime /= 60;

  min = uxtime % 60;
  uxtime -= min;
  uxtime /= 60;

  hour = uxtime % 24;
  uxtime -= hour;
  uxtime /= 24;

  week = uxtime % 7;

  year = 1970;
  day = uxtime;
  while (day > 366) {
    if (IsLeapYear(year))
      day -= 366;
    else
      day -= 365;
    year++;
  }
  day++;

  month = 0;
  while (1) {
    if (IsLeapYear(year)) {
      if (day <= dayLearYear[month])
        break;
      day -= dayLearYear[month];
      month++;
    }
    else {
      if (day <= dayYear[month])
        break;
      day -= dayYear[month];
      month++;
    }
  }

#if defined(HAVE_SNPRINTF)
  snprintf(buf, bufSize,
#else
  sprintf(buf,
#endif
      "%s, %02d %s %04d %02d:%02d:%02d GMT",
      to_week_string(week),
      day,
      to_month_string(month),
      year,
      hour,
      min,
      sec);

  return buf;

  mupnp_log_debug_l4("Leaving...\n");
}

#endif
