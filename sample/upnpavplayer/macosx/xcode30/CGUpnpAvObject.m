//
//  CGUpnpAvObject.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvObject.h>

@implementation CGUpnpAvObject

@synthesize objectId;
@synthesize title;
@synthesize upnpClass;
@synthesize parentObject;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;
	[self setObjectId:nil];
	[self setTitle:nil];
	[self setUpnpClass:nil];
	[self setParentObject:nil];
	return self;
}

- (void) finalize
{
	[super finalize];
}

- (CGUpnpAvObject *)ancestorObject
{
	ancestorObject = self;
	while ([ancestorObject parentObject] != nil)
		ancestorObject = [ancestorObject parentObject];
	return ancestorObject;
}

@end
