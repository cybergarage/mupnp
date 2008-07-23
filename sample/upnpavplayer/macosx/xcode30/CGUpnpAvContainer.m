//
//  CGUpnpAvContainer.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvContainer.h>

@implementation CGUpnpAvContainer

@synthesize xmlNode;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	childArray = [[NSMutableArray alloc] init];
	return self;
}

- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
{
	if ((self = [super initWithXMLNode:aXmlNode]) == nil)
		return nil;
	childArray = [[NSMutableArray alloc] init];
	return self;
}

- (void)dealloc
{
	[childArray release];
	[super dealloc];
}

- (void) finalize
{
	[childArray release];
	[super finalize];
}

- (void)addChild:(CGUpnpAvObject *)obj
{
	[childArray addObject:obj];
	[obj setParent:self];
}

- (void)addChildren:(NSArray *)objArray;
{
	for (CGUpnpAvObject *obj in objArray) {
		[childArray addObject:obj];
		[obj setParent:self];
	}
}

- (void)removeChild:(CGUpnpAvObject *)obj
{
	[childArray removeObject:obj];
	[obj setParent:nil];
}

- (void)removeAllChildren
{
	for (CGUpnpAvObject *obj in [self children])
		[obj setParent:nil];
	[childArray removeAllObjects];
}

- (NSArray *)children
{
	return childArray;
}

- (CGUpnpAvObject *)childforId:(NSString *)aObjectId;
{
	for (CGUpnpAvObject *avObj in [self children]) {
		if ([avObj isObjectId:aObjectId])
			return avObj;
	}
	return nil;
}

- (CGUpnpAvObject *)childforTitle:(NSString *)aTitle
{
	for (CGUpnpAvObject *avObj in [self children]) {
		if ([avObj isTitle:aTitle])
			return avObj;
	}
	return nil;
}

- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId
{
	if ([self isObjectId:aObjectId])
		return self;

	CGUpnpAvObject *foundAvObj = [self childforId:aObjectId];
	if (foundAvObj != nil)
		return foundAvObj;

	for (CGUpnpAvObject *avObj in [self children]) {
		if (![avObj isContainer])
			continue;
		CGUpnpAvContainer *avCon = (CGUpnpAvContainer *)avObj;
		foundAvObj = [avCon childforId:aObjectId];
		if (foundAvObj != nil)
			return foundAvObj;
	}
	return nil;
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath
{
	NSArray *titleArray = [aTitlePath pathComponents];
	if ([titleArray count] <= 0)
		return self;
	BOOL isAbsolutePath = [aTitlePath isAbsolutePath];
	CGUpnpAvObject *lastObject = isAbsolutePath ? [self ancestor] : self;
	int pathIndex = 0;
	for (NSString *title in titleArray) {
		pathIndex++;
		if (pathIndex == 1) {
			if (isAbsolutePath)
				continue;
		}
		CGUpnpAvObject *foundObj = nil;
		if ([lastObject isContainer]) {
			CGUpnpAvContainer *avCon = (CGUpnpAvContainer *)lastObject;
			for (CGUpnpAvObject *childObj in [avCon children]) {
				if ([childObj isTitle:title]) {
					foundObj = childObj;
					break;
				}
			}
		}
		if (foundObj == nil)
			return nil;
		lastObject = foundObj;
	}
	[[lastObject retain] autorelease];
	return lastObject;
}

@end
