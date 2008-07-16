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
	rootObj = [[CGUpnpAvRoot alloc] init];
	return self;
}

- (void)dealloc
{
	[rootObj release];
	[super dealloc];
}

- (void)finalize
{
	[rootObj release];
	[super finalize];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)titlePath
{
	return [rootObj objectForTitlePath:titlePath];
}

@end

