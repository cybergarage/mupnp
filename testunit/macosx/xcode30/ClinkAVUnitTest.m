//
//  ClinkAVUnitTest.m
//  ClinkUnitTest
//
//  Created by Satoshi Konno on 08/07/16.
//  Copyright 2008 CyberGarage. All rights reserved.
//

#import "ClinkAVUnitTest.h"

@implementation ClinkAVUnitTest

- (void) testController
{
	CGUpnpAvController *avCon = [[CGUpnpAvController alloc] init];
	STAssertNotNil(avCon, nil);
	
	[avCon search];
	
	NSArray *servers = [avCon servers];
	NSLog(@"[servers count] = %d", [servers count]);
	STAssertTrue(0 < [servers count], @"0 < [devices count]");
	NSLog(@"[servers objectAtIndex:0] = %@", [servers objectAtIndex:0]);
	int firstServerCnt = [servers count];
	CGUpnpAvServer *firstServer = [servers objectAtIndex:0];
	for (CGUpnpAvServer *server in servers) {
		NSLog(@"%@", [server friendlyName]);
		CGUpnpAvServer *foundServer; 
		foundServer = [avCon serverForFriendlyName:[server friendlyName]]; 
		STAssertNotNil(foundServer, nil);
		STAssertTrue(server == foundServer, @"server == foundServer");
		foundServer = [avCon serverForUDN:[server udn]]; 
		STAssertNotNil(foundServer, nil);
		STAssertTrue(server == foundServer, @"server == foundServer");
	}
	
	servers = [avCon servers];
	NSLog(@"[servers count] = %d", [servers count]);
	STAssertTrue([servers count] == firstServerCnt, @"0 < [devices count]");
	NSLog(@"[servers objectAtIndex:0] = %@", [servers objectAtIndex:0]);
	STAssertTrue([servers objectAtIndex:0] == firstServer, @"[servers objectAtIndex:0] == firstServer");
	
	
	[avCon release];
}

@end
