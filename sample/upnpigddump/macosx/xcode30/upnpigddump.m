//
//  upnpigddump.c
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CyberLink/UPnP.h>

void PrintIGDInfo(CGUpnpDevice *dev, int igdNum) 
{
	NSLog(@"[%d] %@", igdNum, [dev friendlyName]);

	CGUpnpService *ipConService = [dev getServiceByType:@"urn:schemas-upnp-org:service:WANIPConnection:1"];
	if (ipConService) {
		CGUpnpAction *extIpAddrAction = [ipConService getActionByName:@"GetExternalIPAddress"];
		if (extIpAddrAction) {
			if ([extIpAddrAction post])
				NSLog(@"  GetExternalIPAddress = %@", [extIpAddrAction argumentValueforName:@"NewExternalIPAddress"]);
		}
	}

	CGUpnpService *wanComIfCfgService = [dev getServiceByType:@"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1"];
	if (wanComIfCfgService) {
		CGUpnpAction *totalBytesSentAction = [wanComIfCfgService getActionByName:@"GetTotalBytesSent"];
		if (totalBytesSentAction) {
			if ([totalBytesSentAction post])
				NSLog(@"  GetTotalBytesSent = %@", [totalBytesSentAction argumentValueforName:@"NewTotalBytesSent"]);
		}
		CGUpnpAction *totalBytesRecvAction = [wanComIfCfgService getActionByName:@"GetTotalBytesReceived"];
		if (totalBytesRecvAction) {
			if ([totalBytesRecvAction post])
				NSLog(@"  GetTotalBytesReceived = %@", [totalBytesRecvAction argumentValueforName:@"NewTotalBytesReceived"]);
		}
	}
	
}

int main (int argc, const char * argv[])
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];

	[ctrlPoint search];

	int igdNum = 0;
	NSArray *devArray = [ctrlPoint devices];
	for (CGUpnpDevice *dev in devArray) {
		if ([dev isDeviceType:@"urn:schemas-upnp-org:device:InternetGatewayDevice:1"])
			PrintIGDInfo(dev, ++igdNum);
	}

	if (igdNum <= 0)
		NSLog(@"IGD is not found !!\n");

	[pool drain];
	return 0;
}
