//
//  CGUpnpActionResponse.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/13.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <cybergarage/upnp/cservice.h>
#import <CyberGarage/UPnP/CGUpnpActionResponse.h>

@implementation CGUpnpActionResponse

@synthesize arguments;
@synthesize statusCode;

- (id) initWithResult:(NSDictionary *)args statusCode:(int) statCode;
{
	if ((self = [super init]) == nil)
		return nil;

	arguments = args;
	statusCode = statCode;

	return self;
}

- (id) init
{
	return [self initWithResult:nil statusCode:0];
}

- (void) finalize
{
	[super finalize];
}

@end
