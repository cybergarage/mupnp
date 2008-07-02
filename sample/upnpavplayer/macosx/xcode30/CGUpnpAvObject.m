//
//  CGUpnpAvObject.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvObject.h>
#import <CGUpnpAvContainer.h>
#import <CGUpnpAvItem.h>

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

- (BOOL)isContainer
{
	return [self isKindOfClass:[CGUpnpAvContainer class]];
}

- (BOOL)isItem
{
	return [self isKindOfClass:[CGUpnpAvItem class]];
}

- (CGUpnpAvObject *)ancestorObject
{
	CGUpnpAvObject *ancestorObj = self;
	while ([ancestorObj parentObject] != nil)
		ancestorObj = [ancestorObj parentObject];
	return ancestorObj;
}

@end
