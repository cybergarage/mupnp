/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2026
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <mupnp/upnp.h>

#include "binarylight_device.h"
#include "kb_util.h"

#include <stdio.h>
#include <stdlib.h>

static void print_status(mUpnpDevice* lightDev)
{
  bool status;

  if (upnp_binarylight_device_getstatus(lightDev, &status))
    printf("Light: %s\n", status ? "ON" : "OFF");
}

int main(int argc, char* argv[])
{
  int cacheControl;
  int ch;
  mUpnpDevice* lightDev;
  bool status;

  cacheControl = 1800;
  if (argc > 2 || (argc == 2 && sscanf(argv[1], "%i", &cacheControl) != 1) || cacheControl <= 0) {
    fprintf(stderr, "usage: binarylight [cache_control_in_sec]\n");
    return EXIT_FAILURE;
  }

  lightDev = upnp_binarylight_device_new();
  if (lightDev == NULL) {
    fprintf(stderr, "Couldn't create the BinaryLight device.\n");
    return EXIT_FAILURE;
  }

  mupnp_device_setleasetime(lightDev, cacheControl);
  if (!mupnp_device_start(lightDev)) {
    fprintf(stderr, "Couldn't start the BinaryLight device.\n");
    mupnp_device_delete(lightDev);
    return EXIT_FAILURE;
  }

  printf("BinaryLight:1 started (CACHE-CONTROL: %d sec)\n", cacheControl);
  printf("Press 1 to turn on, 0 to turn off, space to toggle, or q to quit.\n");
  print_status(lightDev);

  kbinit();
  ch = 0;
  while (ch != 'q' && ch != 'Q') {
    if (0 < kbhit()) {
      ch = getchar();
      if (ch == '1') {
        upnp_binarylight_device_settarget(lightDev, true);
        print_status(lightDev);
      }
      else if (ch == '0') {
        upnp_binarylight_device_settarget(lightDev, false);
        print_status(lightDev);
      }
      else if (ch == ' ' && upnp_binarylight_device_getstatus(lightDev, &status)) {
        upnp_binarylight_device_settarget(lightDev, !status);
        print_status(lightDev);
      }
    }
    else {
      mupnp_wait(100);
    }
  }
  kbexit();

  mupnp_device_stop(lightDev);
  mupnp_device_delete(lightDev);

  return EXIT_SUCCESS;
}
