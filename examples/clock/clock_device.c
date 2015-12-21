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
*	File: clock_device.c
*
*	Revision:
*       05/11/05
*               - first release.
*
*	10/31/05
*		- Removed :schemas: from clock service description
*
*	11-Jan-06 Heikki Junnila
*		- Renamed *_getservicebyname's to *_getservicebyexacttype
*
************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clock_device.h"
#include "clock.h"

#include <string.h>

/****************************************
* Descriptions
****************************************/

char* CLOCK_DEVICE_DESCRIPTION = "<?xml version=\"1.0\" ?>\n"
                                 "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
                                 " 	<specVersion>\n"
                                 " 		<major>1</major>\n"
                                 " 		<minor>0</minor>\n"
                                 " 	</specVersion>\n"
                                 " 	<device>\n"
                                 " 		<deviceType>urn:schemas-upnp-org:device:clock:1</deviceType>\n"
                                 " 		<friendlyName>CyberGarage Clock Device</friendlyName>\n"
                                 " 		<manufacturer>CyberGarage</manufacturer>\n"
                                 " 		<manufacturerURL>http://www.cybergarage.org</manufacturerURL>\n"
                                 " 		<modelDescription>CyberUPnP Clock Device</modelDescription>\n"
                                 " 		<modelName>Clock</modelName>\n"
                                 " 		<modelNumber>1.0</modelNumber>\n"
                                 " 		<modelURL>http://www.cybergarage.org</modelURL>\n"
                                 " 		<serialNumber>1234567890</serialNumber>\n"
                                 " 		<UDN>uuid:cybergarageClockDevice</UDN>\n"
                                 " 		<UPC>123456789012</UPC>\n"
                                 " 		<iconList>\n"
                                 " 			<icon>\n"
                                 " 				<mimetype>image/gif</mimetype>\n"
                                 " 				<width>48</width>\n"
                                 " 				<height>32</height>\n"
                                 " 				<depth>8</depth>\n"
                                 " 				<url>icon.gif</url>\n"
                                 " 			</icon>\n"
                                 " 		</iconList>\n"
                                 " 		<serviceList>\n"
                                 " 			<service>\n"
                                 " 				<serviceType>urn:schemas-upnp-org:service:timer:1</serviceType>\n"
                                 " 				<serviceId>urn:upnp-org:serviceId:timer:1</serviceId>\n"
                                 " 				<SCPDURL>/service/timer/description.xml</SCPDURL>\n"
                                 " 				<controlURL>/service/timer/control</controlURL>\n"
                                 " 				<eventSubURL>/service/timer/eventSub</eventSubURL>\n"
                                 " 			</service>\n"
                                 " 		</serviceList>\n"
                                 " 		<presentationURL>/presentation</presentationURL>\n"
                                 "	</device>\n"
                                 "</root>\n";

char* CLOCK_SERVICE_DESCRIPTION = "<?xml version=\"1.0\"?>\n"
                                  "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\" >\n"
                                  " 	<specVersion>\n"
                                  " 		<major>1</major>\n"
                                  " 		<minor>0</minor>\n"
                                  " 	</specVersion>\n"
                                  " 	<actionList>\n"
                                  " 		<action>\n"
                                  " 			<name>SetTime</name>\n"
                                  " 			<argumentList>\n"
                                  " 				<argument>\n"
                                  " 					<name>NewTime</name>\n"
                                  " 					<relatedStateVariable>Time</relatedStateVariable>\n"
                                  " 					<direction>in</direction>\n"
                                  " 				</argument>\n"
                                  " 				<argument>\n"
                                  " 					<name>Result</name>\n"
                                  " 					<relatedStateVariable>Result</relatedStateVariable>\n"
                                  " 					<direction>out</direction>\n"
                                  " 				</argument>\n"
                                  " 			</argumentList>\n"
                                  " 		</action>\n"
                                  " 		<action>\n"
                                  " 			<name>GetTime</name>\n"
                                  " 			<argumentList>\n"
                                  " 				<argument>\n"
                                  " 					<name>CurrentTime</name>\n"
                                  " 					<relatedStateVariable>Time</relatedStateVariable>\n"
                                  " 					<direction>out</direction>\n"
                                  " 				</argument>\n"
                                  " 			</argumentList>\n"
                                  " 		</action>\n"
                                  " 	</actionList>\n"
                                  " 	<serviceStateTable>\n"
                                  " 		<stateVariable sendEvents=\"yes\">\n"
                                  " 			<name>Time</name>\n"
                                  " 			<dataType>string</dataType>\n"
                                  " 		</stateVariable>\n"
                                  " 		<stateVariable sendEvents=\"no\">\n"
                                  " 			<name>Result</name>\n"
                                  " 			<dataType>string</dataType>\n"
                                  " 		</stateVariable>\n"
                                  " 	</serviceStateTable>\n"
                                  "</scpd>\n";

/****************************************
* upnp_clock_actionreceived
****************************************/

bool upnp_clock_actionreceived(mUpnpAction* action)
{
  mUpnpTime currTime;
  const char* actionName;
  mUpnpArgument* currTimeArg;
  char sysTimeStr[SYSTEM_TIME_BUF_LEN];
  mUpnpArgument *newTimeArg, *resultArg;

  currTime = mupnp_getcurrentsystemtime();

  actionName = mupnp_action_getname(action);
  if (strcmp("GetTime", actionName) == 0) {
    GetSystemTimeString(currTime, sysTimeStr);
    currTimeArg = mupnp_action_getargumentbyname(action, "CurrentTime");
    mupnp_argument_setvalue(currTimeArg, sysTimeStr);
    return true;
  }
  if (strcmp(actionName, "SetTime") == 0) {
    newTimeArg = mupnp_action_getargumentbyname(action, "NewTime");
    resultArg = mupnp_action_getargumentbyname(action, "Result");
    mupnp_argument_setvalue(resultArg, "Not implemented");
    return true;
  }

  return false;
}

/****************************************
* upnp_clock_queryreceived
****************************************/

bool upnp_clock_queryreceived(mUpnpStateVariable* statVar)
{
  const char* varName;
  mUpnpTime currTime;
  char sysTimeStr[SYSTEM_TIME_BUF_LEN];

  varName = mupnp_statevariable_getname(statVar);
  if (strcmp("Time", varName) == 0) {
    currTime = mupnp_getcurrentsystemtime();
    GetSystemTimeString(currTime, sysTimeStr);
    mupnp_statevariable_setvalue(statVar, sysTimeStr);
    return true;
  }

  return false;
}

/****************************************
* upnp_clock_device_httprequestrecieved
****************************************/

void upnp_clock_device_httprequestrecieved(mUpnpHttpRequest* httpReq)
{
  mUpnpTime currTime;
  mUpnpDevice* dev;
  char* uri;
  char content[2048];
  char sysTimeStr[SYSTEM_TIME_BUF_LEN];
  char serverName[MUPNP_SEVERNAME_MAXLEN];
  mUpnpHttpResponse* httpRes;
  bool postRet;

  dev = (mUpnpDevice*)mupnp_http_request_getuserdata(httpReq);

  uri = mupnp_http_request_geturi(httpReq);
  if (strcmp(uri, "/presentation") != 0) {
    mupnp_device_httprequestrecieved(httpReq);
    return;
  }

  currTime = mupnp_getcurrentsystemtime();

#if defined(HAVE_SNPRINTF)
  snprintf(content, sizeof(content),
#else
  sprintf(content,
#endif
      "<HTML>"
      "<HEAD>"
      "<TITLE>UPnP Clock Sample</TITLE>"
      "</HEAD>"
      "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"1; URL=/presentation\">"
      "<BODY><CENTER>"
      "<H1>UPnP Clock Sample</H1>"
      "<TABLE border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"
      "<TR>"
      "<TD style=\"width: 50px; height: 50px; background-color: rgb(176, 176, 176);\"></TD>"
      "<TD style=\"background-color: rgb(176, 176, 176);\"></TD>"
      "<TD style=\"width: 50px; height: 50px; background-color: rgb(176, 176, 176);\"></TD>"
      "</TR>"
      "<TR>"
      "<TD style=\"height: 50px; background-color: rgb(176, 176, 176);\"></TD>"
      "<TD style=\"height: 50px; background-color: rgb(221, 236, 245);\" align=\"center\"><H1>"
      "%s"
      "</H1></TD>"
      "<TD style=\"height: 50px; background-color: rgb(176, 176, 176);\"></TD>"
      "</TR>"
      "<TR>"
      "<TD style=\"height: 50px; background-color: rgb(176, 176, 176);\"></TD>"
      "<TD style=\"height: 50px; background-color: rgb(221, 236, 245);\" align=\"center\"><H3>"
      "Server : %s"
      "</H3></TD>"
      "<TD style=\"height: 30px; background-color: rgb(176, 176, 176);\"></TD>"
      "</TR>"
      "<TR>"
      "<TD style=\"width: 30px; height: 50px; background-color: rgb(176, 176, 176);\"></TD>"
      "<TD style=\"background-color: rgb(176, 176, 176);\"></TD>"
      "<TD style=\"width: 30px; height: 50px; background-color: rgb(176, 176, 176);\"></TD>"
      "</TR>"
      "</TABLE>"
      "<CENTER></BODY>"
      "</HTML>",
      GetSystemTimeString(currTime, sysTimeStr),
      mupnp_getservername(serverName, sizeof(serverName)));

  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_http_response_setcontent(httpRes, content);
  mupnp_http_response_setcontenttype(httpRes, "text/html");
  mupnp_http_response_setcontentlength(httpRes, strlen(content));
  postRet = mupnp_http_request_postresponse(httpReq, httpRes);
  mupnp_http_response_delete(httpRes);
}

/****************************************
* upnp_clock_device_new
****************************************/

mUpnpDevice* upnp_clock_device_new()
{
  mUpnpDevice* clockDev;
  mUpnpService* timeService;

  clockDev = mupnp_device_new();

  if (mupnp_device_parsedescription(clockDev, CLOCK_DEVICE_DESCRIPTION, strlen(CLOCK_DEVICE_DESCRIPTION)) == false) {
    mupnp_device_delete(clockDev);
    return NULL;
  }

  timeService = mupnp_device_getservicebyexacttype(clockDev, "urn:schemas-upnp-org:service:timer:1");
  if (timeService == NULL) {
    mupnp_device_delete(clockDev);
    return NULL;
  }

  if (mupnp_service_parsedescription(timeService, CLOCK_SERVICE_DESCRIPTION, strlen(CLOCK_SERVICE_DESCRIPTION)) == false) {
    mupnp_device_delete(clockDev);
    return NULL;
  }

  mupnp_device_setactionlistener(clockDev, upnp_clock_actionreceived);
  mupnp_device_setquerylistener(clockDev, upnp_clock_queryreceived);
  mupnp_device_sethttplistener(clockDev, upnp_clock_device_httprequestrecieved);

  return clockDev;
}

/****************************************
* upnp_clock_device_update
****************************************/

void upnp_clock_device_update(mUpnpDevice* clockDev)
{
  mUpnpTime currTime;
  mUpnpService* timeService;
  mUpnpStateVariable* timeState;
  char sysTimeStr[SYSTEM_TIME_BUF_LEN];

  timeService = mupnp_device_getservicebyexacttype(clockDev, "urn:schemas-upnp-org:service:timer:1");
  if (timeService == NULL)
    return;

  timeState = mupnp_service_getstatevariablebyname(timeService, "Time");
  if (timeState == NULL)
    return;

  currTime = mupnp_getcurrentsystemtime();
  GetSystemTimeString(currTime, sysTimeStr);
  mupnp_statevariable_setvalue(timeState, sysTimeStr);

  printf("%s\n", sysTimeStr);
}
