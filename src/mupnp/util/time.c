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

#include <limits.h>

#include <mupnp/util/log.h>
#include <mupnp/util/time.h>

#if defined(WIN32) && !defined(ITRON) && !defined(WINCE)
#include <windows.h>
#elif defined(WIN32) && defined(WINCE)
#include <windows.h>
#elif defined(BTRON)
#include <btron/clk.h>
#include <btron/proctask.h>
#elif defined(ITRON)
#include <kernel.h>
#elif defined(TENGINE) && !defined(PROCESS_BASE)
#include <tk/tkernel.h>
#elif defined(TENGINE) && defined(PROCESS_BASE)
#include <btron/proctask.h>
#include <tk/tkernel.h>
#elif defined(ESP32) || defined(ESP_PLATFORM)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sys/time.h>
#else
#include <unistd.h>
#endif

/****************************************
 * mupnp_time_wait
 ****************************************/

void mupnp_wait(mUpnpTime mtime)
{
  mupnp_log_debug_l4("Entering...\n");

#if defined(WIN32) && !defined(ITRON)
  Sleep(mtime);
#elif defined(BTRON)
  slp_tsk(mtime);
#elif defined(ITRON)
  tslp_tsk(mtime);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
  tk_slp_tsk(mtime);
#elif defined(TENGINE) && defined(PROCESS_BASE)
  b_slp_tsk(mtime);
#elif defined(ESP32) || defined(ESP_PLATFORM)
  vTaskDelay(mtime / portTICK_PERIOD_MS);
#else
  usleep(((useconds_t)(mtime * 1000)));
#endif

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_waitrandom
 ****************************************/

void mupnp_waitrandom(mUpnpTime mtime)
{
  double factor;
  long waitTime;

  mupnp_log_debug_l4("Entering...\n");

  factor = (double)rand() / (double)RAND_MAX;
  waitTime = (long)((double)mtime * factor);
  mupnp_wait(waitTime);

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_getcurrentsystemtime
 ****************************************/

mUpnpTime mupnp_getcurrentsystemtime(void)
{
#if defined(BTRON)
  STIME mUpnpTime;
  TIMEZONE tz;
  STIME localtime;
  if (get_tim(&mUpnpTime, &tz) != 0)
    return 0;
  localtime = mUpnpTime - tz.adjust + (tz.dst_flg ? (tz.dst_adj * 60) : 0);
#elif defined(ITRON)
  static bool initialized = false;
  SYSTIM sysTim;
  if (initialized == false) {
    sysTim.utime = 0;
    sysTim.ltime = 0;
    set_tim(&sysTim);
  }
  get_tim(&sysTim);
#endif

  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_l4("Leaving...\n");

#if defined(BTRON)
  return localtime;
#elif defined(ITRON)
  return ((sysTim.utime / 1000) << 32) + (sysTim.ltime / 1000);
#else
  return time((time_t*)NULL);
#endif
}

/****************************************
 * mupnp_random
 ****************************************/

float mupnp_random(void)
{
  static bool seedDone = false;

  mupnp_log_debug_l4("Entering...\n");

  if (seedDone == false) {
    srand((int)(mupnp_getcurrentsystemtime() % INT_MAX));
    seedDone = true;
  }

  mupnp_log_debug_l4("Leaving...\n");

  return (float)rand() / (float)RAND_MAX;
}
