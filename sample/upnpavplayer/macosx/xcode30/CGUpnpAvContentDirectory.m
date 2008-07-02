//
//  CGUpnpAvContentDirectory.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvContentDirectory.h>

@implementation CGUpnpAvContentDirectory

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	rootContainer = [[CGUpnpAvContainer alloc] init];
	return self;
}

- (void)finalize
{
	[rootContainer release];
	[super finalize];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)titlePath
{
	CGUpnpAvObject *lastObject = nil;
	NSArray *titleArray = [titlePath pathComponents];
	for (NSString *title in titleArray) {
		if (title == nil)
			continue;
	}
	[lastObject retain];
	return lastObject;
}

@end

