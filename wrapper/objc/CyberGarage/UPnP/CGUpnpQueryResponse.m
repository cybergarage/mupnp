//
//  CGUpnpQueryResponse.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/13.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <cybergarage/upnp/cservice.h>
#import <CyberGarage/UPnP/CGUpnpQueryResponse.h>

@implementation CGUpnpQueryResponse

@synthesize value;
@synthesize statusCode;

- (id) initWithResult:(NSString *)value statusCode:(int) statusCode
{
	if ((self = [super init]) == nil)
		return nil;

	self->value = value;
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
