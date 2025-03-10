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

#include "TestDevice.h"

#include <string.h>

/****************************************
 * Descriptions
 ****************************************/

char* TEST_DEVICE_DESCRIPTION = "<?xml version=\"1.0\" ?>\n"
                                "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
                                " 	<specVersion>\n"
                                " 		<major>1</major>\n"
                                " 		<minor>0</minor>\n"
                                " 	</specVersion>\n"
                                " 	<device>\n"
                                " 		<deviceType>" TEST_DEVICE_DEVICE_TYPE "</deviceType>\n"
                                " 		<friendlyName>" TEST_DEVICE_FREINDLYNAME "</friendlyName>\n"
                                " 		<manufacturer>CyberGarage</manufacturer>\n"
                                " 		<manufacturerURL>http://www.cybergarage.org</manufacturerURL>\n"
                                " 		<modelDescription>CyberUPnP Power Device</modelDescription>\n"
                                " 		<modelName>Power Device</modelName>\n"
                                " 		<modelNumber>1.0</modelNumber>\n"
                                " 		<modelURL>http://www.cybergarage.org</modelURL>\n"
                                " 		<serialNumber>1234567890</serialNumber>\n"
                                " 		<UDN>uuid:1234567890</UDN>\n"
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
                                " 				<serviceType>" TEST_DEVICE_SERVICE_TYPE "</serviceType>\n"
                                " 				<serviceId>urn:upnp-org:serviceId:SwitchPower.1</serviceId>\n"
                                " 				<SCPDURL>/service/power/description.xml</SCPDURL>\n"
                                " 				<controlURL>/service/power/control</controlURL>\n"
                                " 				<eventSubURL>/service/power/eventSub</eventSubURL>\n"
                                " 			</service>\n"
                                " 		</serviceList>\n"
                                " 		<presentationURL>/presentation</presentationURL>\n"
                                " 		<deviceList>\n"
                                "			<device>\n"
                                " 				<deviceType>" TEST_DEVICE_EMBEDDEDDEVICE_TYPE1 "</deviceType>\n"
                                "				<serviceList>\n"
                                " 					<service>\n"
                                " 						<serviceType>" TEST_DEVICE_SERVICE_TYPE "</serviceType>\n"
                                " 						<serviceId>urn:upnp-org:serviceId:SwitchPower.1</serviceId>\n"
                                " 						<SCPDURL>/service/power/description.xml</SCPDURL>\n"
                                "						<controlURL>/service/power/control</controlURL>\n"
                                " 						<eventSubURL>/service/power/eventSub</eventSubURL>\n"
                                " 					</service>\n"
                                " 				</serviceList>\n"
                                "			</device>\n"
                                "			<device>\n"
                                " 				<deviceType>" TEST_DEVICE_EMBEDDEDDEVICE_TYPE2 "</deviceType>\n"
                                "				<serviceList>\n"
                                " 					<service>\n"
                                " 						<serviceType>" TEST_DEVICE_SERVICE_TYPE "</serviceType>\n"
                                " 						<serviceId>urn:upnp-org:serviceId:SwitchPower.1</serviceId>\n"
                                " 						<SCPDURL>/service/power/description.xml</SCPDURL>\n"
                                "						<controlURL>/service/power/control</controlURL>\n"
                                " 						<eventSubURL>/service/power/eventSub</eventSubURL>\n"
                                " 					</service>\n"
                                " 				</serviceList>\n"
                                "			</device>\n"
                                " 		</deviceList>\n"
                                "	</device>\n"
                                "</root>\n";

/****************************************
 * Service Device Descriptions
 ****************************************/

char* TEST_SERVICE_DESCRIPTION = "<?xml version=\"1.0\"?>\n"
                                 "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\" >\n"
                                 " 	<specVersion>\n"
                                 " 		<major>1</major>\n"
                                 " 		<minor>0</minor>\n"
                                 " 	</specVersion>\n"
                                 "  <serviceStateTable>\n"
                                 "    <stateVariable>\n"
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
                                 "  <actionList>\n"
                                 "    <action>\n"
                                 "    <name>SetTarget</name>\n"
                                 "      <argumentList>\n"
                                 "        <argument>\n"
                                 "          <name>newTargetValue</name>\n"
                                 "          <direction>in</direction>\n"
                                 "          <relatedStateVariable>Target</relatedStateVariable>\n"
                                 "        </argument>\n"
                                 "      </argumentList>\n"
                                 "    </action>\n"
                                 "    <action>\n"
                                 "    <name>GetTarget</name>\n"
                                 "      <argumentList>\n"
                                 "        <argument>\n"
                                 "          <name>RetTargetValue</name>\n"
                                 "          <direction>out</direction>\n"
                                 "          <relatedStateVariable>Target</relatedStateVariable>\n"
                                 "        </argument>\n"
                                 "      </argumentList>\n"
                                 "    </action>\n"
                                 "    <action>\n"
                                 "    <name>GetStatus</name>\n"
                                 "      <argumentList>\n"
                                 "        <argument>\n"
                                 "          <name>ResultStatus</name>\n"
                                 "          <direction>out</direction>\n"
                                 "          <relatedStateVariable>Status</relatedStateVariable>\n"
                                 "      </argument>\n"
                                 "     </argumentList>\n"
                                 "    </action>\n"
                                 "	  </actionList>\n"
                                 "</scpd>\n";

/****************************************
 * upnp_test_actionreceived
 ****************************************/

bool upnp_test_actionreceived(mUpnpAction* action)
{
  /*
        mUpnpTime currTime;
        char *actionName;
        mUpnpArgument *currTimeArg;

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
*/

  return false;
}

/****************************************
 * upnp_test_queryreceived
 ****************************************/

bool upnp_test_queryreceived(mUpnpStateVariable* statVar)
{
  /*
        char *varName;
        mUpnpTime currTime;

        char sysTimeStr[SYSTEM_TIME_BUF_LEN];

        varName = mupnp_statevariable_getname(statVar);
        if (strcmp("Time", varName) == 0) {
                currTime = mupnp_getcurrentsystemtime();
                GetSystemTimeString(currTime, sysTimeStr);
                mupnp_statevariable_setvalue(statVar, sysTimeStr);
                return true;
        }
*/

  return false;
}

/****************************************
 * upnp_test_device_httprequestrecieved
 ****************************************/

void upnp_test_device_httprequestrecieved(mUpnpHttpRequest* httpReq)
{
  // mUpnpTime currTime;
  // mUpnpDevice* dev;
  char* uri;
  char content[2048];
  // char sysTimeStr[SYSTEM_TIME_BUF_LEN];
  // char serverName[MUPNP_SEVERNAME_MAXLEN];
  mUpnpHttpResponse* httpRes;

  // dev = (mUpnpDevice*)mupnp_http_request_getuserdata(httpReq);

  uri = mupnp_http_request_geturi(httpReq);
  if (strcmp(uri, "/presentation") != 0) {
    mupnp_device_httprequestrecieved(httpReq);
    return;
  }

  // currTime = mupnp_getcurrentsystemtime();

  /*
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
*/

  httpRes = mupnp_http_response_new();
  mupnp_http_response_setstatuscode(httpRes, MUPNP_HTTP_STATUS_OK);
  mupnp_http_response_setcontent(httpRes, content);
  mupnp_http_response_setcontenttype(httpRes, "text/html");
  mupnp_http_response_setcontentlength(httpRes, strlen(content));
  mupnp_http_request_postresponse(httpReq, httpRes);
  mupnp_http_response_delete(httpRes);
}

/****************************************
 * upnp_test_device_new
 ****************************************/

mUpnpDevice* upnp_test_device_new()
{
  mUpnpDevice* testDev;
  mUpnpService* testService;

  testDev = mupnp_device_new();

  if (mupnp_device_parsedescription(testDev, TEST_DEVICE_DESCRIPTION, strlen(TEST_DEVICE_DESCRIPTION)) == false) {
    mupnp_device_delete(testDev);
    return NULL;
  }

  testService = mupnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
  if (testService == NULL) {
    mupnp_device_delete(testDev);
    return NULL;
  }

  if (mupnp_service_parsedescription(testService, TEST_SERVICE_DESCRIPTION, strlen(TEST_SERVICE_DESCRIPTION)) == false) {
    mupnp_device_delete(testDev);
    return NULL;
  }

  mupnp_device_setactionlistener(testDev, upnp_test_actionreceived);
  mupnp_device_setquerylistener(testDev, upnp_test_queryreceived);
  mupnp_device_sethttplistener(testDev, upnp_test_device_httprequestrecieved);

  return testDev;
}
