//
//  main.m
//  upnpdump
//
//  Created by Satoshi Konno on 08/04/16.
//  Copyright Satoshi Konno 2008. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <CyberGarage/UPnP/CGUpnpControlPoint.h>

int main(int argc, char *argv[])
{
	CGUpnpControlPoint *ctrlPoint = [CGUpnpControlPoint init];
	[ctrlPoint search];
	NSArray devArray = [ctrlPoint getDeviceArray];
	return NSApplicationMain(argc,  (const char **) argv);
}
