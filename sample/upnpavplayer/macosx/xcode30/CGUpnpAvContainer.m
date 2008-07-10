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

- (id)init
{
	if ((self = [super init]) == nil)
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
	CGUpnpAvObject *lastObject = isAbsolutePath ? [self ancestorObject] : self;
	int pathIndex = 0;
	for (NSString *title in titleArray) {
		pathIndex++;
		if (pathIndex == 1) {
			if (isAbsolutePath)
				continue;
		}
		if (title == nil)
			continue;
	}
	[lastObject retain];
	return lastObject;
}

@end
