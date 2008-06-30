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
	return self;
}

- (void) finalize
{
	[super finalize];
}

@end
