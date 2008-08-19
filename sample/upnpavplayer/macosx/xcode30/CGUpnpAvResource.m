//
//  CGUpnpAvResource.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CGXmlNode.h>
#import <CGUpnpAvResource.h>
#import <CGUpnpAvConstants.h>

@implementation CGUpnpAvResource

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	return self;
}

- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
{
	if ((self = [super initWithXMLNode:aXmlNode]) == nil)
		return nil;
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void) finalize
{
	[super finalize];
}

- (NSString *)url
{
	return [self stringValue];
}

- (long long)size
{
	return [[self attributeValueForName:CG_UPNPAV_RESOURCE_PROTOCOLINFO_SIZE] longLongValue];
}

- (NSString *)mimeType
{
	return [self attributeValueForName:CG_UPNPAV_RESOURCE_DLNAPN];
}

- (NSString *)dlnaOrgPn
{
	return [self attributeValueForName:CG_UPNPAV_RESOURCE_DLNAPN];
}

- (NSString *)dlnaOrgOp
{
	return [self attributeValueForName:CG_UPNPAV_RESOURCE_DLNAOP];
}

- (NSString *)dlnaOrgFlags
{
	return [self attributeValueForName:CG_UPNPAV_RESOURCE_DLNAOP];
}

@end

