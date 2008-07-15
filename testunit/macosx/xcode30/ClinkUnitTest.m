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
	[cp release];
}

@end
