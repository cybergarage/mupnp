//
//  ClinkAVUnitTest.m
//  ClinkUnitTest
//
//  Created by Satoshi Konno on 08/07/16.
//  Copyright 2008 CyberGarage. All rights reserved.
//

#import "ClinkAVUnitTest.h"

@implementation ClinkAVUnitTest

- (void) setUp
{
	dmc = [[CGUpnpdmctroller alloc] init];
	[dmc search];
}
 
- (void) tearDown
{
	[dmc release];
}

#define CLINKAVUNITTEST_TESTXMLNODE_STRINGVALUE @"testValue"
#define CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_NAME @"testAttrName"
#define CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_VALUE @"testAttrValue"

- (void) testXmlNode
{
	CGXmlNode *xmlNode = [[CGXmlNode alloc] init];
	
	/* String Value */
	NSString *nodeValue = [xmlNode stringValue];
	STAssertTrue(((nodeValue == nil) || ([nodeValue length] <= 0)), @"(nodeValue == nil) || ([nodeValue length] <= 0)");
	[xmlNode setStringValue:CLINKAVUNITTEST_TESTXMLNODE_STRINGVALUE];
	nodeValue = [xmlNode stringValue];
	STAssertTrue(0 < [nodeValue length], @"0 < [nodeValue length]");
	STAssertTrue([nodeValue isEqualToString:CLINKAVUNITTEST_TESTXMLNODE_STRINGVALUE], @"[nodeValue isEqualToString:CLINKAVUNITTEST_TESTXMLNODE_STRINGVALUE]");

	/* Attribute */
	NSString *attrValue = [xmlNode attributeValueForName:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_NAME];
	STAssertTrue(((attrValue == nil) || ([attrValue length] <= 0)), @"(attrValue == nil) || ([attrValue length] <= 0)");
	[xmlNode setAttributeWithName:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_NAME stringValue:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_VALUE];
	NSString *attrValue = [xmlNode attributeValueForName:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_NAME];
	STAssertTrue(0 < [attrValue length], @"0 < [attrValue length]");
	STAssertTrue([attrValue isEqualToString:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_VALUE], @"[attrValue isEqualToString:CLINKAVUNITTEST_TESTXMLNODE_ATTRIBUTE_VALUE]");

	[xmlNode release];
}

- (void) testObjectNode
{
	CGUpnpAvRoot *rootNode = [[CGUpnpAvRoot alloc] init];
	NSString *objId = [rootNode objectId];
	STAssertTrue([objId isEqualToString:@"0"], @"[objId isEqualToString:@"0"]");
	[rootNode release];
}

- (void) testServers
{
	NSArray *servers = [dmc servers];
	NSLog(@"[servers count] = %d", [servers count]);
	STAssertTrue(0 < [servers count], @"0 < [devices count]");
	NSLog(@"[servers objectAtIndex:0] = %@", [servers objectAtIndex:0]);
	int firstServerCnt = [servers count];
	CGUpnpAvServer *firstServer = [servers objectAtIndex:0];
	for (CGUpnpAvServer *server in servers) {
		NSLog(@"%@", [server friendlyName]);
		CGUpnpAvServer *foundServer; 
		foundServer = [dmc serverForFriendlyName:[server friendlyName]]; 
		STAssertNotNil(foundServer, nil);
		STAssertTrue(server == foundServer, @"server == foundServer");
		foundServer = [dmc serverForUDN:[server udn]]; 
		STAssertNotNil(foundServer, nil);
		STAssertTrue(server == foundServer, @"server == foundServer");
	}
	
	servers = [dmc servers];
	NSLog(@"[servers count] = %d", [servers count]);
	STAssertTrue([servers count] == firstServerCnt, @"0 < [devices count]");
	NSLog(@"[servers objectAtIndex:0] = %@", [servers objectAtIndex:0]);
	STAssertTrue([servers objectAtIndex:0] == firstServer, @"[servers objectAtIndex:0] == firstServer");
}

@end
