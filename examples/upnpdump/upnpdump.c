/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: upnpdump.c
*
*	Revision:
*       05/11/05
*               - first release.
*
*	10/31/05
*		- Added some M-SEARCH sending actions
*
************************************************************/

#include <mupnp/upnp.h>

#include <ctype.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <conio.h>
#endif

#if defined(TENGINE)
#if defined(PROCESS_BASE) /* Process based */
#include <basic.h>
#define MBEG int main(W ac, TC* av[])
#define MEND 0
#define DIR "/SYS/bin/"
#else /* T-Kernel based */
#include <basic.h>
#include <tk/tkernel.h>
#include <sys/pinfo.h>
#define MBEG ER main(INT ac, UB* av[])
#define MEND E_SYS
#define DIR "/SYS/bin/"
#endif
#endif

#include "kb_util.h"

#if defined(WIN32) && !defined(__CYGWIN__)
#define kbgetkey getch
#else
#define kbgetkey getchar
#endif

/* Additional control commands */

#define CMD_NO_ALTERATIONS 1
#define CMD_LOOP_ACTION_CALLS 2

void ControlDeviceAlter(mUpnpControlPoint* ctrlPoint, int alteration_mask);

/////////////////////////////////////////////////////////////////////////////////
// PrintKeyMessage
/////////////////////////////////////////////////////////////////////////////////

void PrintKeyMessage()
{
  printf("'p' : Print\n");
#if !defined(MUPNP_NOUSE_ACTIONCTRL)
  printf("'c' : Control\n");
  printf("'o' : Control (multiple actions)\n");
#endif
#if !defined(MUPNP_NOUSE_QUERYCTRL)
  printf("'q' : Query\n");
#endif
#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
  printf("'s' : Subscribe\n");
  printf("'u' : Unsubscribe\n");
#endif
  printf("'m' : Change MX value for M-Search\n");
  printf("'r' : M-Search (rootdevice)\n");
  printf("'h' : M-Search (ssdp:all)\n");
  printf("'v' : M-Search (urn:schemas-upnp-org:device:clock:1)\n");
  printf("'t' : M-Search (urn:schemas-upnp-org:service:timer:1)\n");
  printf("'a' : Do all searches four times in a loop.\n");
  printf("'x' : eXit\n");
}

/////////////////////////////////////////////////////////////////////////////////
// Notify Listener
/////////////////////////////////////////////////////////////////////////////////

void SSDPNotifyListner(mUpnpSSDPPacket* ssdpPkt)
{
  if (mupnp_ssdp_packet_isdiscover(ssdpPkt) == true) {
    printf("ssdp:discover : ST = %s\n",
        mupnp_ssdp_packet_getst(ssdpPkt));
  }
  else if (mupnp_ssdp_packet_isalive(ssdpPkt) == true) {
    printf("ssdp:alive : uuid = %s, NT = %s, location = %s\n",
        mupnp_ssdp_packet_getusn(ssdpPkt),
        mupnp_ssdp_packet_getnt(ssdpPkt),
        mupnp_ssdp_packet_getlocation(ssdpPkt));
  }
  else if (mupnp_ssdp_packet_isbyebye(ssdpPkt) == true) {
    printf("ssdp:byebye : uuid = %s, NT = %s\n",
        mupnp_ssdp_packet_getusn(ssdpPkt),
        mupnp_ssdp_packet_getnt(ssdpPkt));
  }
  mupnp_ssdp_packet_print(ssdpPkt);
}

/////////////////////////////////////////////////////////////////////////////////
// Print Device
/////////////////////////////////////////////////////////////////////////////////

void PrintDeviceInfo(mUpnpDevice* dev, int indent)
{
  char indentStr[128];
  int n;
  mUpnpService* service;
  int serviceCnt;
  mUpnpAction* action;
  int actionCnt;
  mUpnpArgumentList* arg;
  int argCnt;
  mUpnpStateVariable* stateVar;
  int stateVarCnt;

  for (n = 0; n < indent && n < (sizeof(indentStr) - 1); n++)
    indentStr[n] = ' ';
  indentStr[n] = '\0';

  serviceCnt = 0;
  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    printf("%s service[%d] = %s\n", indentStr, ++serviceCnt, mupnp_service_getservicetype(service));
    actionCnt = 0;
    for (action = mupnp_service_getactions(service); action != NULL; action = mupnp_action_next(action)) {
      printf("%s  action[%d] = %s\n", indentStr, ++actionCnt, mupnp_action_getname(action));
      argCnt = 0;
      for (arg = mupnp_action_getarguments(action); arg != NULL; arg = mupnp_argument_next(arg)) {
        printf("%s   arg[%d] = %s\n", indentStr, ++argCnt, mupnp_argument_getname(arg));
      }
    }
    stateVarCnt = 0;
    for (stateVar = mupnp_service_getstatevariables(service); stateVar != NULL; stateVar = mupnp_statevariable_next(stateVar)) {
      printf("%s  stateVar[%d] = %s = %s\n",
          indentStr,
          ++stateVarCnt,
          mupnp_statevariable_getname(stateVar),
          mupnp_statevariable_getvalue(stateVar));
    }
  }
}

void PrintDevice(mUpnpDevice* dev, int indent)
{
  mUpnpDevice* childDev;

  PrintDeviceInfo(dev, indent);

  for (childDev = mupnp_device_getdevices(dev); childDev != NULL; childDev = mupnp_device_next(childDev))
    PrintDevice(childDev, indent + 1);
}

void PrintControlPointDevice(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* dev;
  int devCnt;

  printf("Device Num = %d\n", mupnp_controlpoint_getndevices(ctrlPoint));

  devCnt = 0;
  for (dev = mupnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = mupnp_device_next(dev)) {
    printf("[%d] = %s\n", ++devCnt, mupnp_device_getfriendlyname(dev));
    PrintDevice(dev, 1);
  }
}

/////////////////////////////////////////////////////////////////////////////////
// Select*
/////////////////////////////////////////////////////////////////////////////////

mUpnpDevice* SelectDevice(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* dev;
  int n;
  char key;
  int devNum;

  n = 0;
  for (dev = mupnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = mupnp_device_next(dev)) {
    key = 'a' + n;
    if ('z' < key)
      break;
    printf(" [%c] = %s\n", key, mupnp_device_getfriendlyname(dev));
    n++;
  }
  if (n == 0)
    return NULL;
  printf("Select Device : ");
  key = kbgetkey();
  key = tolower(key);
  printf("%c\n", key);

  if (!isalpha(key))
    return NULL;

  devNum = key - 'a';
  dev = mupnp_controlpoint_getdevices(ctrlPoint);
  for (n = 0; n < devNum; n++)
    dev = mupnp_device_next(dev);

  return dev;
}

mUpnpService* SelectService(mUpnpDevice* dev)
{
  mUpnpService* service;
  int n;
  char key;
  int serviceNum;

  n = 0;
  for (service = mupnp_device_getservices(dev); service != NULL; service = mupnp_service_next(service)) {
    key = 'a' + n;
    if ('z' < key)
      break;
    printf(" [%c] = %s\n", key, mupnp_service_getservicetype(service));
    n++;
  }
  printf("Select Service : ");
  key = kbgetkey();
  key = tolower(key);
  printf("%c\n", key);

  if (!isalpha(key))
    return NULL;

  serviceNum = key - 'a';
  service = mupnp_device_getservices(dev);
  for (n = 0; n < serviceNum; n++)
    service = mupnp_service_next(service);

  return service;
}

mUpnpAction* SelectAction(mUpnpService* service)
{
  mUpnpAction* action;
  int n;
  char key;
  int actionNum;

  n = 0;
  for (action = mupnp_service_getactions(service); action != NULL; action = mupnp_action_next(action)) {
    key = 'a' + n;
    if ('z' < key)
      break;
    printf(" [%c] = %s\n", key, mupnp_action_getname(action));
    n++;
  }
  printf("Select Action : ");
  key = kbgetkey();
  key = tolower(key);
  printf("%c\n", key);

  if (!isalpha(key))
    return NULL;

  actionNum = key - 'a';
  action = mupnp_service_getactions(service);
  for (n = 0; n < actionNum; n++)
    action = mupnp_action_next(action);

  return action;
}

mUpnpStateVariable* SelectStateVariable(mUpnpService* service)
{
  mUpnpStateVariable* stateVar;
  int n;
  char key;
  int serviceNum;

  n = 0;
  for (stateVar = mupnp_service_getstatevariables(service); stateVar != NULL; stateVar = mupnp_statevariable_next(stateVar)) {
    key = 'a' + n;
    if ('z' < key)
      break;
    printf(" [%c] = %s\n", key, mupnp_statevariable_getname(stateVar));
    n++;
  }
  printf("Select StateVariable : ");
  key = kbgetkey();
  key = tolower(key);
  printf("%c\n", key);

  if (!isalpha(key))
    return NULL;

  serviceNum = key - 'a';
  stateVar = mupnp_service_getstatevariables(service);
  for (n = 1; n < serviceNum; n++)
    stateVar = mupnp_statevariable_next(stateVar);

  return stateVar;
}

/////////////////////////////////////////////////////////////////////////////////
// Action
/////////////////////////////////////////////////////////////////////////////////

#if !defined(MUPNP_NOUSE_ACTIONCTRL)

void ControlDevice(mUpnpControlPoint* ctrlPoint)
{
  ControlDeviceAlter(ctrlPoint, CMD_NO_ALTERATIONS);
}

void ControlDeviceAlter(mUpnpControlPoint* ctrlPoint, int alteration_mask)
{
  mUpnpDevice* selDev;
  mUpnpService* selService;
  mUpnpAction* selAction;
  bool actionSuccess;
  mUpnpArgument* arg;
  char argValue[2048];

  printf("Control Device\n");

  selDev = SelectDevice(ctrlPoint);
  if (selDev == NULL)
    return;
  selService = SelectService(selDev);
  if (selService == NULL)
    return;
  selAction = SelectAction(selService);
  if (selAction == NULL)
    return;

  for (arg = mupnp_action_getarguments(selAction); arg; arg = mupnp_argument_next(arg)) {
    if (mupnp_argument_isindirection(arg) == true) {
      printf("%s : ", mupnp_argument_getname(arg));
      if (scanf("%s", argValue) == 1)
        mupnp_argument_setvalue(arg, argValue);
    }
  }

  /* NOTE: Go through selAction memory management... */
  if (alteration_mask & CMD_LOOP_ACTION_CALLS) {
    int loop_count, i;

    printf("\nHow many times action should be sent?");

    if (1 == scanf("%d", &loop_count)) {
      printf("\n");
      for (i = 0; i < loop_count; i++) {
        actionSuccess = mupnp_action_post(selAction);
        printf("Control Result(%d)\n", (int)actionSuccess);
      }

      mupnp_sleep(3000);

      for (arg = mupnp_action_getarguments(selAction);
           arg;
           arg = mupnp_argument_next(arg)) {
        if (mupnp_argument_isoutdirection(arg) == true)
          printf(" %s = %s\n",
              mupnp_argument_getname(arg),
              mupnp_argument_getvalue(arg));
      }

      mupnp_sleep(2000);
    }
  }

  if ((alteration_mask & CMD_NO_ALTERATIONS) == CMD_NO_ALTERATIONS) {
    actionSuccess = mupnp_action_post(selAction);

    printf("Control Result(%d)\n", (int)actionSuccess);
    for (arg = mupnp_action_getarguments(selAction); arg; arg = mupnp_argument_next(arg)) {
      if (mupnp_argument_isoutdirection(arg) == true)
        printf(" %s = %s\n", mupnp_argument_getname(arg), mupnp_argument_getvalue(arg));
    }
  }
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Query
/////////////////////////////////////////////////////////////////////////////////

#if !defined(MUPNP_NOUSE_QUERYCTRL)

void QueryDevice(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* selDev;
  mUpnpService* selService;
  mUpnpStateVariable* selStateVar;
  bool querySuccess;
  char* stateValue;

  printf("Query Device\n");

  selDev = SelectDevice(ctrlPoint);
  if (selDev == NULL)
    return;
  selService = SelectService(selDev);
  if (selService == NULL)
    return;
  selStateVar = SelectStateVariable(selService);
  if (selStateVar == NULL)
    return;

  querySuccess = mupnp_statevariable_post(selStateVar);

  stateValue = mupnp_statevariable_getvalue(selStateVar);

  printf("Query Result(%d) = %s\n",
      (int)querySuccess,
      (stateValue != NULL) ? stateValue : "");
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Subscribe
/////////////////////////////////////////////////////////////////////////////////

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

void SubscribeService(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* selDev;
  mUpnpService* selService;
  bool subSuccess;

  printf("Subscribe Device\n");

  mupnp_controlpoint_lock(ctrlPoint);
  selDev = SelectDevice(ctrlPoint);
  if (selDev == NULL) {
    mupnp_controlpoint_unlock(ctrlPoint);
    return;
  }
  selService = SelectService(selDev);
  if (selService == NULL) {
    mupnp_controlpoint_unlock(ctrlPoint);
    return;
  }
  subSuccess = mupnp_controlpoint_subscribe(ctrlPoint, selService, 300);

  printf("Subscribe Result(%d) = %s\n",
      (int)subSuccess,
      (subSuccess == true) ? mupnp_service_getsubscriptionsid(selService) : "");
  mupnp_controlpoint_unlock(ctrlPoint);
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Unsubscribe
/////////////////////////////////////////////////////////////////////////////////

#if !defined(MUPNP_NOUSE_SUBSCRIPTION)

void UnsubscribeService(mUpnpControlPoint* ctrlPoint)
{
  mUpnpDevice* selDev;
  mUpnpService* selService;
  bool subSuccess;

  printf("Query Device\n");

  selDev = SelectDevice(ctrlPoint);
  if (selDev == NULL)
    return;
  selService = SelectService(selDev);
  if (selService == NULL)
    return;

  subSuccess = mupnp_controlpoint_unsubscribe(ctrlPoint, selService);

  printf("Unsubscribe Result(%d)\n",
      (int)subSuccess);
}

#endif

/////////////////////////////////////////////////////////////////////////////////
// Set MX value
/////////////////////////////////////////////////////////////////////////////////

void SetMXValue(mUpnpControlPoint* ctrlPoint)
{
  unsigned int mxValue;

  printf("Give new MX value: ");
  if (scanf("%u", &mxValue) == 1) {
    printf("%d", mxValue);
    mupnp_controlpoint_setssdpsearchmx(ctrlPoint, mxValue);
  }
  printf("\n");
}

/////////////////////////////////////////////////////////////////////////////////
// Event
/////////////////////////////////////////////////////////////////////////////////

void EventListener(mUpnpProperty* prop)
{
  printf("Property Changed (%s) = %s\n",
      mupnp_property_getname(prop),
      mupnp_property_getvalue(prop));
}

/////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////

#if defined(TENGINE)
MBEG
#else
int main(int argc, char* argv[])
#endif
{
  mUpnpControlPoint* ctrlPoint;
  int key;

  ctrlPoint = mupnp_controlpoint_new();
  mupnp_controlpoint_setssdplistener(ctrlPoint, SSDPNotifyListner);
  mupnp_controlpoint_seteventlistener(ctrlPoint, EventListener);
  if (mupnp_controlpoint_start(ctrlPoint) == false) {
    printf("Couldn't start this control point !!");
    exit(1);
  }

  PrintKeyMessage();

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(TENGINE)
  kbinit();
#endif

  key = 0;
  do {
#if !defined(TENGINE)
    key = kbgetkey();
    key = toupper(key);
    switch (key) {
    case 'P':
      PrintControlPointDevice(ctrlPoint);
      break;
#if !defined(MUPNP_NOUSE_ACTIONCTRL)
    case 'C':
      ControlDeviceAlter(ctrlPoint, CMD_NO_ALTERATIONS);
      break;

    case 'O':
      ControlDeviceAlter(ctrlPoint, CMD_LOOP_ACTION_CALLS);
      break;
#endif
#if !defined(MUPNP_NOUSE_QUERYCTRL)
    case 'Q':
      QueryDevice(ctrlPoint);
      break;
#endif
#if !defined(MUPNP_NOUSE_SUBSCRIPTION)
    case 'S':
      SubscribeService(ctrlPoint);
      break;
    case 'U':
      UnsubscribeService(ctrlPoint);
      break;
#endif
    case 'M':
      SetMXValue(ctrlPoint);
      break;
    case 'R':
      printf("M-Search upnp::rootdevice\n");
      mupnp_controlpoint_search(ctrlPoint, MUPNP_ST_ROOT_DEVICE);
      break;
    case 'H':
      printf("M-Search ssdp:all\n");
      mupnp_controlpoint_search(ctrlPoint, "ssdp:all");
      break;
    case 'V':
      printf("M-Search device type\n");
      mupnp_controlpoint_search(ctrlPoint, "urn:schemas-upnp-org:device:clock:1");
      break;
    case 'T':
      printf("M-Search service type\n");
      mupnp_controlpoint_search(ctrlPoint, "urn:schemas-upnp-org:service:timer:1");
      break;
    case 'A': {
      int i;

      printf("Doing all searches four times...\n");

      for (i = 0; i < 4; i++) {
        mupnp_controlpoint_search(ctrlPoint, MUPNP_ST_ROOT_DEVICE);
        mupnp_controlpoint_search(ctrlPoint, "ssdp:all");
        mupnp_controlpoint_search(ctrlPoint, "urn:schemas-upnp-org:device:clock:1");
        mupnp_controlpoint_search(ctrlPoint, "urn:schemas-upnp-org:service:timer:1");
      }

      printf("Done (doing all searches four times).\n");
    } break;
    case 'X':
      break;
    default:
      PrintKeyMessage();
    }
#else
    mupnp_wait(1000);
#endif
  } while (key != 'X');

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(TENGINE)
  kbexit();
#endif

  mupnp_controlpoint_stop(ctrlPoint);
  mupnp_controlpoint_delete(ctrlPoint);

  return (0);
}
