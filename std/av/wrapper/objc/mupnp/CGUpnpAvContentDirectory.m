//
//  CGUpnpAvContentDirectory.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGUpnpAvContentManager.h"
#import "CGUpnpAvContentDirectory.h"
#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvRoot.h"

@implementation CGUpnpAvContentDirectory

@synthesize contentMgr;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	self.contentMgr = [[[CGUpnpAvContentManager alloc] init] autorelease];
	return self;
}

- (void)dealloc
{
    self.contentMgr = nil;
	[super dealloc];
}

- (CGUpnpAvContainer *)rootObject
{
	return[[self contentMgr] rootObj];
}

- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId
{
	return [[self contentMgr] objectForId:aObjectId];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath
{
	return [[self contentMgr] objectForTitlePath:aTitlePath];
}

@end

