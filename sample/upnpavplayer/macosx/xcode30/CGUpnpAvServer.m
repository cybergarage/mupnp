//
//  CGUpnpAvServer.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <CyberLink/UPnP.h>
#import <CGUpnpAvServer.h>
#import <CGUpnpAvItem.h>

@implementation CGUpnpAvServer

@synthesize contentDirectory;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;
	contentDirectory = [[CGUpnpAvContentDirectory alloc] init];
	return self;
}

- (id) initWithCObject:(CgUpnpDevice *)cobj
{
	if ((self = [super initWithCObject:cobj]) == nil)
		return nil;
	contentDirectory = [[CGUpnpAvContentDirectory alloc] init];
	return self;
}

- (void)dealloc
{
	[contentDirectory release];
	[super dealloc];
}

- (void)finalize
{
	[contentDirectory release];
	[super finalize];
}

- (NSArray *)browse:(NSString *)aObjectId;
{
	return [contentDirectory browse:aObjectId];
}

- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId
{
	return [contentDirectory objectForId:aObjectId];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath
{
	return [contentDirectory objectForTitlePath:aTitlePath];
}

@end
