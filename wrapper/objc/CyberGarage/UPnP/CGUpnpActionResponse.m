//
//  CGUpnpActionResponse.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/13.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cservice.h>

#import <CyberGarage/UPnP/CGUpnpActionResponse.h>

@synthesize arguments;
@synthesize statusCode;

@implementation CGUpnpActionResponse

- (id) initWithResult:(NSDictionary *)arguments statusCode:(int) statusCode;
{
	if ((self = [super init]) == nil)
		return nil;

	self->arguments = arguments;
	self->statusCode = statusCode;

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
