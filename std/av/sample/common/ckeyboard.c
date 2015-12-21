#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: kb_util.h
*
*	Revision:
*
*	05/11/05
*		- first revision
*
******************************************************************/

#if !defined(WIN32) && !defined(__MINGW32__)
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#endif

#if defined(__CYGWIN__) && !defined(FIONREAD)
#include <winsock2.h>
#include <windows.h>
#endif

#if defined(HAVE_SYS_IOCTL_H)
#define BSD_COMP /* Get FIONREAD on Solaris2 */
#include <sys/ioctl.h>
#endif
#if defined(HAVE_SYS_FILIO_H) /* Get FIONREAD on Solaris 2.5 */
#include <sys/filio.h>
#endif

#if !defined(WIN32) && !defined(__MINGW32__)
static struct termios orgTermAttr;
#endif
#include "ckeyboard.h"

void mupnp_kb_init()
{
#if !defined(WIN32) && !defined(__MINGW32__)
  struct termios termAttr;
  tcgetattr(0, &orgTermAttr);
  termAttr = orgTermAttr;
  termAttr.c_lflag &= ~(ICANON | ECHO);
  termAttr.c_cc[VTIME] = 0;
  termAttr.c_cc[VMIN] = 1;
  tcsetattr(0, TCSANOW, &termAttr);
#endif
}

int mupnp_kb_hit()
{
#if !defined(WIN32) && !defined(__MINGW32__)
  int ret, n;
  ret = ioctl(0, FIONREAD, &n);
  if (ret != -1)
    return n;
#endif
  return 0;
}

int mupnp_kb_key()
{
  return 0;
}

void mupnp_kb_exit()
{
#if !defined(WIN32) && !defined(__MINGW32__)
  tcsetattr(0, TCSANOW, &orgTermAttr);
#endif
}
