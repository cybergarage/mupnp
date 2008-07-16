//
//  ClinkUnitTest.m
//  ClinkUnitTest
//
//  Created by Satoshi Konno on 08/07/15.
//  Copyright 2008 CyberGarage. All rights reserved.
//

#import "ClinkUnitTest.h"
#import <CyberLink/UPnP.h>

@implementation ClinkUnitTest

- (void) setUp
{
}
 
- (void) tearDown
{
}

- (void) testControlPoint
{
	CGUpnpControlPoint *cp = [[CGUpnpControlPoint alloc] init];
	STAssertNotNil(cp, nil);
	[cp search];
	NSArray *devices = [cp devices];
	STAssertTrue((0 < [devices count]), @"0 < [devices count]");
	[cp release];
}

@end
