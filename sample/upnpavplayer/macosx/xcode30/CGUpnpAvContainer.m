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

- (void)addChildObject:(CGUpnpAvObject *)obj
{
	[childArray addObject:obj];
	[obj setParent:self];
}

- (void)removeChildObject:(CGUpnpAvObject *)obj
{
	[childArray removeObject:obj];
	[obj setParent:nil];
}

- (NSArray *)children
{
	return childArray;
}

- (CGUpnpAvObject *)childforTitle:(NSString *)title
{
	for (CGUpnpAvObject *avObj in [self children]) {
		if ([avObj isTitle:title])
			return avObj;
	}
	return nil;
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)titlePath
{
	NSArray *titleArray = [titlePath pathComponents];
	if ([titleArray count] <= 0)
		return nil;
	NSString *rootPath = [titleArray objectAtIndex:0];
	BOOL isAbsolutePath = [rootPath isEqualToString:@"/"];
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
