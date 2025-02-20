# mUPnP for MacOSX Programming Guide Document Version 2.2

## 1 Introduction

UPnP™\*[^1] architecture is based on open networking to enable discovery and control of networked devices and services, such as media servers and players at home.

UPnP™ architecture is based on many standard protocols, such as GENA, SSDP, SOAP, HTTPU and HTTP. Therefore you have to understand and implement these protocols to create your devices of UPnP™.

mUPnP for MacOSX is a development package for UPnP™ developers. The mUPnP controls these protocols automatically, and supports to create your control points quickly.

Please see the following site and documents to know about UPnP™ in more detail.

| Document                                               | URL                                                                       |
|----------------------------------------------------------|---------------------------------------------------------------------------|
| UPnP™ Forum                                            | http://www.upnp.org/                                                      |
| Universal Plug and Play Device Architecture            | http://www.upnp.org/download/UPnPDA10_20000613.htm                        |
| Universal Plug and Play Vendor\'s Implementation Guide | http://www.upnp.org/download/UPnP_Vendor_Implementation_Guide_Jan2001.htm |

## 2 Setup

### 2.1 System Requirement

Currently, the framework is based on Objective-C 2.0. Thus, the current framework requires MacOSX v10.5, Leopard, later. In the future, I will support other lower MacOSX and iPhone v2.0.

### 2.2 Installer

The framework is distributed as a install package as the following. Using the installer, the framework is installed into \'/Library/Framework/mUPnP.framework\' as default.

![](img/mupnpc-objc-macosx-installer.jpg)

## 3 Control Point

### 3.1 Class Overview

The following static structure diagram is related classes of mUPnP to create your control point of UPnP™. The control point has some root devices in the UPnP™ network.

```plantuml
@startuml
class CGUpnpControlPoint {
+start()
+stop()
+search()
+searchWithST()
+setSsdpSearchMX()
+ssdpSearchMX()
+devices()    
}
class CGUpnpDevice {
+friendlyName()
+deviceType()
+isDeviceType()
+services()
+getServiceForID()
+getServiceForType()
}
class CGUpnpService {
+serviceId()
+serviceType()
+actions()
+stateVariables()
+getActionByName()
+getStateVariableByName()
}
class CGUpnpAction {
+name()
+arguments()
+setArgumentValue:forName()
+argumentValueForName()
+post()
+postWithArguments()
+statusCode()
}
class CGUpnpStateVariable {
+name()
+value()
+query()
+statusCode()
}
CGUpnpControlPoint *-- CGUpnpDevice
CGUpnpDevice *-- CGUpnpService
CGUpnpService *-- CGUpnpAction
CGUpnpService *-- CGUpnpStateVariable
@enduml
```

### 3.2 Initiating

To create a UPnP™ control point, create a instance of CGUpnpControlPoint class. The new instance is activated automatically using CGUpnpControlPoint::start. Use CGUpnpControlPoint::search or searchWithST to find the devices in the local network.

```
#import <mUPnP/UPnP.h>
......
CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];
......
[ctrolPoint search];
```

### 3.3 Root Devices

Use CGUpnpControlPoint:devices to get the all root devices which the control point found. The method returns a NSArray object which has the devices as instances of CGUpnpDevice.

```
#import <mUPnP/UPnP.h>
......
CGUpnpControlPoint \*ctrlPoint = [[CGUpnpControlPoint alloc] init];
......
[ctrolPoint search];

NSArray *devArray = [ctrlPoint devices];
for (CGUpnpDevice *dev in devArray)
    NSLog(@"%@", [dev friendlyName]);
```

### 3.4 Control

The control point can send action or query control messages to the discovered devices. To send the action control message, use CGUpnpAction:setArgumentValue:forName and CGUpnpAction:post. You should set the action values to the all input arguments, and the output argument values is ignored if you set. The following sample posts a action control request that sets a new time, and output the response result.

```
CGUpnpDevice *clockDev = .....
CGUpnpService *timeService = [clockDev getServiceForType:@"urn:schemas-upnp-org:service:xxxxx:1"];
CGUpnpAction *setTimeAct = [timeService getActionForName:@"SetTime"];

NSString *currTime = ......
[setTimeAct setArgumentValue: currTime forName:@"NewTime"];
if ([setTimeAct post]) {
    NSArray \*argArray = [ setTimeAct arguments]; for (CGUpnpArgument \*arg in argArray)
    NSLog(@"%@ = %@", [arg name], [arg value]]);
}
```

Similarly, to send the query control message, use CGUpnpStateVariable::query. The following sample posts a query control request, and output the return value.

```
CGUpnpDevice *clockDev = ......
CGUpnpService *timeService = [clockDev getServiceForType:@"urn:schemas-upnp-org:service:xxxxx:1"];
CGUpnpStateVariable *timeStateVar = [timeService "time"]; 
if ([timeStateVar query])
    NSLog(@"%@ = %@", [timeStateVar name], [timeStateVar value]]);
```

# 4 License

Copyright (c) 2004-2006, Satoshi Konno
Copyright (c) 2005-2006, Nokia Corporation
Copyright (c) 2005-2006, Theo Beisch Collectively the Copyright Owners All rights reserved.

Subject to the below, redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

-   Redistributions of source code must retain the above copyright notice, this list of conditions and thefollowing disclaimer.
-   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and thefollowing disclaimer in the documentation and/or other materials provided with the distribution.
-   Neither the names of the Copyright Owners nor the names of its contributors may be used to endorse orpromote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT OWNERS AND CONTRIBUTORS \"AS IS\" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
While any of the respective Copyright Owners licenses and/or distributes this software and any files contained therein under its or third party copyrights, for clarification it is hereby stated that no patent license explicit or
implied is granted by any of the Copyright Owners in connection to this license or distribution of this software.

In addition to the disclaimer above and not limiting its generality, no assurances are provided by the Copyright Owners that the software does not infringe patents of either third parties or any of the Copyright Owners in any particular jurisdiction. As a condition to exercising the rights and licenses granted hereunder in any particular jurisdiction, each recipient hereby assumes sole responsibility to procure licenses under any relevant patents in that jurisdiction. For example, if a third party patent license is required to allow recipient to distribute the program in a particular country, it is the recipient\'s responsibility to acquire that license before distributing the program.

This software has been supported by IPA, INFORMATION-TECHNOLOGY PROMOTION AGENCY, JAPAN, as a project of Exploratory Software Project.

[^1]: UPnP™ is a certification mark of the UPnP™ Implementers Corporation.　
