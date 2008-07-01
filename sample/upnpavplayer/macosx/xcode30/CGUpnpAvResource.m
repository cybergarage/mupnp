//
//  CGUpnpAvResource.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGUpnpAvResource.h>

@implementation CGUpnpAvResource

@synthesize url;

- (id) init
{
	if ((self = [super init]) == nil)
		return nil;
	return self;
}

- (void) finalize
{
	[super finalize];
}

@end
