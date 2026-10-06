//
//  mUPnP4ObjCTests.m
//  mUPnP4ObjCTests
//
//  Created by Satoshi Konno on 2014/06/24.
//  Copyright (c) 2014年 Satoshi Konno. All rights reserved.
//
//  Regression tests for the Objective-C wrapper API. They use only local
//  descriptions and never contact a device on the network. Native allocation
//  failure, HTTP fixture and lifetime regressions that need test hooks live in
//  test/security/objc-api-regression.m and test/security/objc-lifetime.m.
//

#import <XCTest/XCTest.h>

#import "CGUpnpAction.h"
#import "CGUpnpDevice.h"
#import "CGUpnpService.h"
#import "CGUpnpStateVariable.h"

static NSString* const kServiceType = @"urn:schemas-upnp-org:service:SwitchPower:1";

static NSString* DeviceDescription(NSString* friendlyName)
{
  return [NSString stringWithFormat:
                       @"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                        "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
                        "<specVersion><major>1</major><minor>0</minor></specVersion>\n"
                        "<URLBase>http://127.0.0.1:9/</URLBase>\n"
                        "<device>\n"
                        "<deviceType>urn:schemas-upnp-org:device:BinaryLight:1</deviceType>\n"
                        "<friendlyName>%@</friendlyName>\n"
                        "<UDN>uuid:mupnp4objc-tests</UDN>\n"
                        "<serviceList><service>\n"
                        "<serviceType>urn:schemas-upnp-org:service:SwitchPower:1</serviceType>\n"
                        "<serviceId>urn:upnp-org:serviceId:SwitchPower.1</serviceId>\n"
                        "<SCPDURL>/scpd.xml</SCPDURL>\n"
                        "<controlURL>/control</controlURL>\n"
                        "<eventSubURL>/event</eventSubURL>\n"
                        "</service></serviceList>\n"
                        "</device>\n"
                        "</root>\n",
                   friendlyName];
}

static NSString* ServiceDescription(NSString* allowedValue)
{
  return [NSString stringWithFormat:
                       @"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                        "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
                        "<specVersion><major>1</major><minor>0</minor></specVersion>\n"
                        "<actionList><action>\n"
                        "<name>SetTarget</name>\n"
                        "<argumentList>\n"
                        "<argument><name>NewTargetValue</name><direction>in</direction>"
                        "<relatedStateVariable>Target</relatedStateVariable></argument>\n"
                        "</argumentList>\n"
                        "</action></actionList>\n"
                        "<serviceStateTable>\n"
                        "<stateVariable sendEvents=\"no\"><name>Target</name><dataType>string</dataType>"
                        "<allowedValueList><allowedValue>%@</allowedValue></allowedValueList>"
                        "</stateVariable>\n"
                        "</serviceStateTable>\n"
                        "</scpd>\n",
                   allowedValue];
}

@interface mUPnP4ObjCTests : XCTestCase
@end

@implementation mUPnP4ObjCTests

- (CGUpnpService*)serviceWithAllowedValue:(NSString*)allowedValue device:(CGUpnpDevice**)deviceOut
{
  CGUpnpDevice* device = [[CGUpnpDevice alloc] initWithXMLDescription:DeviceDescription(@"Light")];
  XCTAssertNotNil(device);
  CGUpnpService* service = [device getServiceForType:kServiceType];
  XCTAssertNotNil(service);
  XCTAssertTrue([service parseXMLDescription:ServiceDescription(allowedValue)]);
  if (deviceOut)
    *deviceOut = device;
  return service;
}

- (void)testStateVariableWithoutNativeObjectFailsSafely
{
  NSArray* variables = @[ [[CGUpnpStateVariable alloc] init], [[CGUpnpStateVariable alloc] initWithCObject:NULL] ];
  for (CGUpnpStateVariable* variable in variables) {
    XCTAssertTrue(variable.cObject == NULL);
    XCTAssertNil([variable name]);
    XCTAssertNil([variable value]);
    XCTAssertEqual([[variable allowedValues] count], (NSUInteger)0);
    XCTAssertFalse([variable isAllowedValue:@"anything"]);
    XCTAssertFalse([variable query]);
    XCTAssertEqual([variable statusCode], (NSInteger)0);
  }
}

- (void)testDeviceDescriptionPreservesUTF8Text
{
  for (NSString* name in @[ @"Living Room Light", @"リビングの照明", @"照明💡ランプ🏠" ]) {
    CGUpnpDevice* device = [[CGUpnpDevice alloc] initWithXMLDescription:DeviceDescription(name)];
    XCTAssertNotNil(device, @"%@", name);
    XCTAssertEqualObjects([device friendlyName], name);
    XCTAssertEqualObjects([device udn], @"uuid:mupnp4objc-tests");

    CGUpnpDevice* parsed = [[CGUpnpDevice alloc] init];
    XCTAssertTrue([parsed parseXMLDescription:DeviceDescription(name)]);
    XCTAssertEqualObjects([parsed friendlyName], name);
  }
}

- (void)testDeviceDescriptionRejectsInvalidInput
{
  CGUpnpDevice* device = [[CGUpnpDevice alloc] initWithXMLDescription:DeviceDescription(@"照明")];
  XCTAssertNotNil(device);
  XCTAssertFalse([device parseXMLDescription:nil]);
  XCTAssertFalse([device parseXMLDescription:@""]);
  XCTAssertEqualObjects([device friendlyName], @"照明");
  XCTAssertFalse([device parseXMLDescription:@"<root><device><friendlyName>壊れた"]);

  XCTAssertNil([[CGUpnpDevice alloc] initWithXMLDescription:nil]);
  XCTAssertNil([[CGUpnpDevice alloc] initWithXMLDescription:@""]);
  XCTAssertNil([[CGUpnpDevice alloc] initWithXMLDescription:@"not xml"]);
}

- (void)testServiceDescriptionPreservesUTF8Text
{
  CGUpnpDevice* device = nil;
  CGUpnpService* service = [self serviceWithAllowedValue:@"日本語の値🎌" device:&device];
  CGUpnpStateVariable* target = [service getStateVariableForName:@"Target"];
  XCTAssertNotNil(target);
  XCTAssertTrue([[target allowedValues] containsObject:@"日本語の値🎌"]);
  XCTAssertNotNil([service getActionForName:@"SetTarget"]);
  XCTAssertFalse([service parseXMLDescription:nil]);
}

- (void)testActionRejectsInvalidArgumentsWithoutChangingValues
{
  CGUpnpDevice* device = nil;
  CGUpnpService* service = [self serviceWithAllowedValue:@"ASCII" device:&device];
  CGUpnpAction* action = [service getActionForName:@"SetTarget"];
  XCTAssertNotNil(action);
  XCTAssertTrue([action setArgumentValue:@"initial" forName:@"NewTargetValue"]);

  XCTAssertFalse([action postWithArguments:@{ @"Unknown" : @"1" }]);
  XCTAssertFalse(([action postWithArguments:@{ @"NewTargetValue" : @"changed", @"Unknown" : @"1" }]));
  XCTAssertFalse([action postWithArguments:@{ @"NewTargetValue" : @1 }]);
  XCTAssertFalse([action postWithArguments:@{ @1 : @"value" }]);
  XCTAssertEqualObjects([action argumentValueForName:@"NewTargetValue"], @"initial");
}

- (void)testChildWrappersOutliveTheirDevice
{
  CGUpnpAction* action = nil;
  CGUpnpStateVariable* variable = nil;
  @autoreleasepool {
    CGUpnpDevice* device = nil;
    CGUpnpService* service = [self serviceWithAllowedValue:@"ASCII" device:&device];
    action = [service getActionForName:@"SetTarget"];
    variable = [service getStateVariableForName:@"Target"];
  }
  XCTAssertEqualObjects([action name], @"SetTarget");
  XCTAssertEqualObjects([variable name], @"Target");
}

@end
