//
//  CGUpnpAvServer.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

//#import <CGUpnpControlPoint.h>
#import <CyberLink/UPnP.h>
#import <CGUpnpAvServer.h>

@implementation CGUpnpAvServer

@synthesize contentDirectory;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	contentDirectory = [[CGUpnpAvContentDirectory alloc] init];
	return self;
}

- (void)finalize
{
	[contentDirectory release];
	[super finalize];
}

@end
