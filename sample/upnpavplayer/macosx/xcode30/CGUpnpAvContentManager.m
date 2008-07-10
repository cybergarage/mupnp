//
//  CGUpnpAvContentManager.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvContentManager.h>

@implementation CGUpnpAvContentManager

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	rootContainer = [[CGUpnpAvContainer alloc] init];
	return self;
}

- (void)dealloc
{
	[rootContainer release];
	[super dealloc];
}

- (void)finalize
{
	[rootContainer release];
	[super finalize];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)titlePath
{
	return [rootContainer objectForTitlePath:titlePath];
}

@end

