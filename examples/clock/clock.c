/************************************************************
 *
 *	mUPnP for C
 *
 *	Copyright (C) Satoshi Konno 2005
 *
 *	File: clock.c
 *
 *	Revision:
 *       06/07/05
 *               - first release.
 *
 ************************************************************/

#include "clock.h"

#include <stdio.h>
#include <time.h>

/****************************************
 * toMonthString
 ****************************************/

static char* monthString[] = {
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
  if (0 <= value && value < 12)
    return monthString[value];
  return "";
}

/****************************************
 * toWeekString
 ****************************************/

static char* weekString[] = {
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
  if (0 <= value && value < 7)
    return weekString[value];
  return "";
}

/****************************************
 * GetSystemTimeString
 ****************************************/

char* get_system_time_string(mUpnpTime currTime, char* buf)
{
  struct tm* localTime;

  localTime = localtime(&currTime);
  sprintf(buf, "%s, %s %d, %02d, %02d:%02d:%02d", to_week_string(localTime->tm_wday), to_month_string(localTime->tm_mon), localTime->tm_mday, localTime->tm_year + 1900, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);

  return buf;
}

/****************************************
 * GetDateString
 ****************************************/

char* get_date_string(mUpnpTime currTime, char* buf)
{
  struct tm* localTime;

  localTime = localtime(&currTime);

  sprintf(buf, "%s, %s %d, %02d", to_week_string(localTime->tm_wday), to_month_string(localTime->tm_mon), localTime->tm_mday, localTime->tm_year + 1900);

  return buf;
}

/****************************************
 * GetTimeString
 ****************************************/

char* get_time_string(mUpnpTime currTime, char* buf)
{
  struct tm* localTime;

  localTime = localtime(&currTime);

  sprintf(buf, "%02d%s%02d", localTime->tm_hour, ((localTime->tm_sec % 2) == 0) ? ":" : " ", localTime->tm_min);

  return buf;
}

/****************************************
 * GetSecond
 ****************************************/

int get_second(mUpnpTime currTime)
{
  struct tm* localTime;

  localTime = localtime(&currTime);

  return localTime->tm_sec;
}
