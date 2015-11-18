//
//  CGUpnpAvContentManager.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvContainer.h"
#import "CGUpnpAvRoot.h"
#import "CGUpnpAvContentManager.h"

@implementation CGUpnpAvContentManager

@synthesize rootObj;

- (id)init
{ 
	if ((self = [super init]) == nil)
		return nil;
	self.rootObj = [[[CGUpnpAvRoot alloc] init] autorelease];
	return self;
}

- (void)dealloc
{
    self.rootObj = nil;
	[super dealloc];
}

- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId
{
	return [rootObj objectForId:aObjectId];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath
{
	return [rootObj objectForTitlePath:aTitlePath];
}

@end

