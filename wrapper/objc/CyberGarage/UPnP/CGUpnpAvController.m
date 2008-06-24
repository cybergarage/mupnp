//
//  CGUpnpAvController.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/06/25.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <CGUpnpControlPoint.h>
#import <CGUpnpAvController.h>

@implementation CGUpnpAvController

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	return self;
}

- (void)finalize
{
	[super finalize];
}

- (NSArray *)browse:(NSString *)objectId
{
	return nil;
}

@end
