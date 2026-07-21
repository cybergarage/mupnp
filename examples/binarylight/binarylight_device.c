/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2026
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "binarylight_device.h"

#include <stdio.h>
#include <string.h>

#define BINARYLIGHT_TARGET "Target"
#define BINARYLIGHT_STATUS "Status"

static const char binarylightDeviceDescription[] = "<?xml version=\"1.0\"?>\n"
                                                   "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
                                                   "  <specVersion>\n"
                                                   "    <major>1</major>\n"
                                                   "    <minor>0</minor>\n"
                                                   "  </specVersion>\n"
                                                   "  <device>\n"
                                                   "    <deviceType>" UPNP_BINARYLIGHT_DEVICE_TYPE "</deviceType>\n"
                                                   "    <friendlyName>mUPnP Binary Light</friendlyName>\n"
                                                   "    <manufacturer>CyberGarage</manufacturer>\n"
                                                   "    <manufacturerURL>https://www.cybergarage.org/</manufacturerURL>\n"
                                                   "    <modelDescription>mUPnP BinaryLight:1 sample device</modelDescription>\n"
                                                   "    <modelName>Binary Light</modelName>\n"
                                                   "    <modelNumber>1.0</modelNumber>\n"
                                                   "    <modelURL>https://www.cybergarage.org/</modelURL>\n"
                                                   "    <serialNumber>00000001</serialNumber>\n"
                                                   "    <UDN>uuid:2b9e8d7e-85ca-4a0f-9061-c3f1c034f7e2</UDN>\n"
                                                   "    <serviceList>\n"
                                                   "      <service>\n"
                                                   "        <serviceType>" UPNP_BINARYLIGHT_SERVICE_TYPE "</serviceType>\n"
                                                   "        <serviceId>urn:upnp-org:serviceId:SwitchPower:1</serviceId>\n"
                                                   "        <SCPDURL>/service/switchpower/description.xml</SCPDURL>\n"
                                                   "        <controlURL>/service/switchpower/control</controlURL>\n"
                                                   "        <eventSubURL>/service/switchpower/eventSub</eventSubURL>\n"
                                                   "      </service>\n"
                                                   "    </serviceList>\n"
                                                   "    <presentationURL>/presentation</presentationURL>\n"
                                                   "  </device>\n"
                                                   "</root>\n";

static const char binarylightServiceDescription[] = "<?xml version=\"1.0\"?>\n"
                                                    "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
                                                    "  <specVersion>\n"
                                                    "    <major>1</major>\n"
                                                    "    <minor>0</minor>\n"
                                                    "  </specVersion>\n"
                                                    "  <actionList>\n"
                                                    "    <action>\n"
                                                    "      <name>SetTarget</name>\n"
                                                    "      <argumentList>\n"
                                                    "        <argument>\n"
                                                    "          <name>newTargetValue</name>\n"
                                                    "          <direction>in</direction>\n"
                                                    "          <relatedStateVariable>Target</relatedStateVariable>\n"
                                                    "        </argument>\n"
                                                    "      </argumentList>\n"
                                                    "    </action>\n"
                                                    "    <action>\n"
                                                    "      <name>GetTarget</name>\n"
                                                    "      <argumentList>\n"
                                                    "        <argument>\n"
                                                    "          <name>RetTargetValue</name>\n"
                                                    "          <direction>out</direction>\n"
                                                    "          <relatedStateVariable>Target</relatedStateVariable>\n"
                                                    "        </argument>\n"
                                                    "      </argumentList>\n"
                                                    "    </action>\n"
                                                    "    <action>\n"
                                                    "      <name>GetStatus</name>\n"
                                                    "      <argumentList>\n"
                                                    "        <argument>\n"
                                                    "          <name>ResultStatus</name>\n"
                                                    "          <direction>out</direction>\n"
                                                    "          <relatedStateVariable>Status</relatedStateVariable>\n"
                                                    "        </argument>\n"
                                                    "      </argumentList>\n"
                                                    "    </action>\n"
                                                    "  </actionList>\n"
                                                    "  <serviceStateTable>\n"
                                                    "    <stateVariable sendEvents=\"no\">\n"
                                                    "      <name>Target</name>\n"
                                                    "      <dataType>boolean</dataType>\n"
                                                    "      <defaultValue>0</defaultValue>\n"
                                                    "    </stateVariable>\n"
                                                    "    <stateVariable sendEvents=\"yes\">\n"
                                                    "      <name>Status</name>\n"
                                                    "      <dataType>boolean</dataType>\n"
                                                    "      <defaultValue>0</defaultValue>\n"
                                                    "    </stateVariable>\n"
                                                    "  </serviceStateTable>\n"
                                                    "</scpd>\n";

static mUpnpService* upnp_binarylight_getservice(mUpnpDevice* dev)
{
  if (dev == NULL)
    return NULL;

  return mupnp_device_getservicebyexacttype(dev, UPNP_BINARYLIGHT_SERVICE_TYPE);
}

static bool upnp_binarylight_parseboolean(const char* value, bool* result)
{
  if (value == NULL || result == NULL)
    return false;

  if (strcmp(value, "1") == 0 || mupnp_strcasecmp(value, "true") == 0 || mupnp_strcasecmp(value, "yes") == 0) {
    *result = true;
    return true;
  }

  if (strcmp(value, "0") == 0 || mupnp_strcasecmp(value, "false") == 0 || mupnp_strcasecmp(value, "no") == 0) {
    *result = false;
    return true;
  }

  return false;
}

static bool upnp_binarylight_getstate(mUpnpService* service, const char* name, bool* value)
{
  mUpnpStateVariable* stateVar;

  if (service == NULL || value == NULL)
    return false;

  stateVar = mupnp_service_getstatevariablebyname(service, name);
  if (stateVar == NULL)
    return false;

  return upnp_binarylight_parseboolean(mupnp_statevariable_getvalue(stateVar), value);
}

static bool upnp_binarylight_settarget(mUpnpService* service, bool target)
{
  const char* currentStatus;
  const char* value;
  mUpnpStateVariable* targetState;
  mUpnpStateVariable* statusState;

  if (service == NULL)
    return false;

  targetState = mupnp_service_getstatevariablebyname(service, BINARYLIGHT_TARGET);
  statusState = mupnp_service_getstatevariablebyname(service, BINARYLIGHT_STATUS);
  if (targetState == NULL || statusState == NULL)
    return false;

  value = target ? "1" : "0";
  mupnp_statevariable_setvaluewithoutnotify(targetState, value);
  currentStatus = mupnp_statevariable_getvalue(statusState);
  if (currentStatus == NULL || strcmp(currentStatus, value) != 0)
    mupnp_statevariable_setvalue(statusState, value);

  return true;
}

bool upnp_binarylight_device_settarget(mUpnpDevice* dev, bool target)
{
  return upnp_binarylight_settarget(upnp_binarylight_getservice(dev), target);
}

bool upnp_binarylight_device_gettarget(mUpnpDevice* dev, bool* target)
{
  return upnp_binarylight_getstate(upnp_binarylight_getservice(dev), BINARYLIGHT_TARGET, target);
}

bool upnp_binarylight_device_getstatus(mUpnpDevice* dev, bool* status)
{
  return upnp_binarylight_getstate(upnp_binarylight_getservice(dev), BINARYLIGHT_STATUS, status);
}

static bool upnp_binarylight_actionfailed(mUpnpAction* action)
{
  mupnp_action_setstatuscode(action, MUPNP_STATUS_ACTION_FAILED);
  mupnp_action_setstatusdescription(action, mupnp_status_code2string(MUPNP_STATUS_ACTION_FAILED));
  return false;
}

static bool upnp_binarylight_actionreceived(mUpnpAction* action)
{
  const char* actionName;
  const char* argumentValue;
  const char* stateValue;
  mUpnpService* service;
  mUpnpStateVariable* stateVar;
  bool target;

  actionName = mupnp_action_getname(action);
  service = mupnp_action_getservice(action);

  if (strcmp(actionName, "SetTarget") == 0) {
    argumentValue = mupnp_action_getargumentvaluebyname(action, "newTargetValue");
    if (!upnp_binarylight_parseboolean(argumentValue, &target)) {
      mupnp_action_setstatuscode(action, MUPNP_STATUS_INVALID_ARGS);
      mupnp_action_setstatusdescription(action, mupnp_status_code2string(MUPNP_STATUS_INVALID_ARGS));
      return false;
    }
    if (!upnp_binarylight_settarget(service, target))
      return upnp_binarylight_actionfailed(action);
    return true;
  }

  if (strcmp(actionName, "GetTarget") == 0) {
    stateVar = mupnp_service_getstatevariablebyname(service, BINARYLIGHT_TARGET);
    stateValue = stateVar != NULL ? mupnp_statevariable_getvalue(stateVar) : NULL;
    if (stateValue == NULL || !mupnp_action_setargumentvaluebyname(action, "RetTargetValue", stateValue))
      return upnp_binarylight_actionfailed(action);
    return true;
  }

  if (strcmp(actionName, "GetStatus") == 0) {
    stateVar = mupnp_service_getstatevariablebyname(service, BINARYLIGHT_STATUS);
    stateValue = stateVar != NULL ? mupnp_statevariable_getvalue(stateVar) : NULL;
    if (stateValue == NULL || !mupnp_action_setargumentvaluebyname(action, "ResultStatus", stateValue))
      return upnp_binarylight_actionfailed(action);
    return true;
  }

  return false;
}

static bool upnp_binarylight_queryreceived(mUpnpStateVariable* stateVar)
{
  const char* name;

  if (stateVar == NULL)
    return false;

  name = mupnp_statevariable_getname(stateVar);
  if (name == NULL)
    return false;

  return strcmp(name, BINARYLIGHT_TARGET) == 0 || strcmp(name, BINARYLIGHT_STATUS) == 0;
}

static void upnp_binarylight_httprequestreceived(mUpnpHttpRequest* httpReq)
{
  const char* uri;
  const char* statusText;
  char content[2048];
  char serverName[MUPNP_SEVERNAME_MAXLEN];
  mUpnpDevice* dev;
  mUpnpHttpResponse* httpRes;
  bool status;

  uri = mupnp_http_request_geturi(httpReq);
  dev = (mUpnpDevice*)mupnp_http_request_getuserdata(httpReq);

  if (mupnp_http_request_isgetrequest(httpReq) && strcmp(uri, "/presentation/on") == 0)
    upnp_binarylight_device_settarget(dev, true);
  else if (mupnp_http_request_isgetrequest(httpReq) && strcmp(uri, "/presentation/off") == 0)
    upnp_binarylight_device_settarget(dev, false);
  else if (!mupnp_http_request_isgetrequest(httpReq) || strcmp(uri, "/presentation") != 0) {
    mupnp_device_httprequestrecieved(httpReq);
    return;
  }

  if (!upnp_binarylight_device_getstatus(dev, &status))
    status = false;
  statusText = status ? "ON" : "OFF";

#if defined(HAVE_SNPRINTF)
  snprintf(content, sizeof(content),
#else
  sprintf(content,
#endif
      "<!doctype html>"
      "<html><head><title>mUPnP Binary Light</title>"
      "<style>body{font-family:sans-serif;text-align:center;margin:4em}"
      ".light{display:inline-block;padding:2em 4em;border-radius:1em;background:%s;color:%s}"
      "a{display:inline-block;margin:1.5em .5em;padding:.7em 1.5em;border:1px solid #555;border-radius:.4em;text-decoration:none}</style>"
      "</head><body><h1>mUPnP Binary Light</h1>"
      "<div class=\"light\"><strong>%s</strong></div><div>"
      "<a href=\"/presentation/on\">Turn on</a>"
      "<a href=\"/presentation/off\">Turn off</a></div>"
      "<p>Server: %s</p></body></html>",
      status ? "#ffd54f" : "#444",
      status ? "#111" : "#fff",
      statusText,
      mupnp_getservername(serverName, sizeof(serverName)));

  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_http_response_setcontent(httpRes, content);
  mupnp_http_response_setcontenttype(httpRes, "text/html; charset=utf-8");
  mupnp_http_response_setcontentlength(httpRes, strlen(content));
  mupnp_http_request_postresponse(httpReq, httpRes);
  mupnp_http_response_delete(httpRes);
}

mUpnpDevice* upnp_binarylight_device_new(void)
{
  mUpnpDevice* lightDev;
  mUpnpService* switchPowerService;

  lightDev = mupnp_device_new();
  if (lightDev == NULL)
    return NULL;

  if (!mupnp_device_parsedescription(lightDev, binarylightDeviceDescription, strlen(binarylightDeviceDescription))) {
    mupnp_device_delete(lightDev);
    return NULL;
  }

  switchPowerService = upnp_binarylight_getservice(lightDev);
  if (switchPowerService == NULL || !mupnp_service_parsedescription(switchPowerService, binarylightServiceDescription, strlen(binarylightServiceDescription))) {
    mupnp_device_delete(lightDev);
    return NULL;
  }

  if (!upnp_binarylight_settarget(switchPowerService, false)) {
    mupnp_device_delete(lightDev);
    return NULL;
  }

  mupnp_device_setactionlistener(lightDev, upnp_binarylight_actionreceived);
  mupnp_device_setquerylistener(lightDev, upnp_binarylight_queryreceived);
  mupnp_device_sethttplistener(lightDev, upnp_binarylight_httprequestreceived);

  return lightDev;
}
