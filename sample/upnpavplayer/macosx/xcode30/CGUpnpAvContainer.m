//
//  CGUpnpAvContainer.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvContainer.h>

@implementation CGUpnpAvContainer

@synthesize objectId;
@synthesize title;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;
	childArray = [[NSMutableArray alloc] init];
	[childArray retain];
	return self;
}

- (void) finalize
{
	[childArray release];
	[super finalize];
}

- (void)addChildObject:(CGUpnpAvObject *)obj
{
	[childArray addObject:obj];
}

- (void)removeChildObject:(CGUpnpAvObject *)obj
{
	[childArray removeObject:obj];
}

- (NSArray *)children
{
	return childArray;
}

@end
