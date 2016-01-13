//
//  CGUpnpAvObject.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <mupnp/xml/xml.h>
#include <mupnp/std/av/cupnpav.h>

#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvContainer.h"
#import "CGUpnpAvItem.h"
#import "CGUpnpAvConstants.h"
#import "CGUpnpAvResource.h"

@implementation CGUpnpAvObject

@synthesize parent;
@synthesize userObject;

+ (NSArray *)arrayWithXMLString:(NSString *)aXmlString
{
#if  !defined(TARGET_OS_IPHONE)
	NSError *xmlErr;
	NSXMLDocument *xmlDoc = [[NSXMLDocument alloc] initWithXMLString:aXmlString options:0 error:&xmlErr];
	if (!xmlDoc)
		return nil;

	NSMutableArray *avObjArray = [[[NSMutableArray alloc] init] autorelease];
	
	NSArray *contentArray = [xmlDoc nodesForXPath:@"/DIDL-Lite/*" error:&xmlErr];
	for (NSXMLElement *contentNode in contentArray) {
		CGUpnpAvObject *avObj = nil;
		if ([[contentNode name] isEqualToString:@"container"]) {
			CGUpnpAvContainer *avCon = [[CGUpnpAvContainer alloc] initWithXMLNode:contentNode];
			avObj = avCon;
		}
		else if ([[contentNode name] isEqualToString:@"item"]) {
			CGUpnpAvItem *avItem = [[CGUpnpAvItem alloc] initWithXMLNode:contentNode];
			avObj = avItem;
		}
		if (avObj == nil)
			continue;
		[avObjArray addObject:avObj];
		[avObj release];
	}
	[xmlDoc release];
#else
	char *resultXml;
	mUpnpXmlParser *xmlParser;
	mUpnpXmlNodeList *rootNode;
	mUpnpXmlNode *didlNode;
	mUpnpXmlNode *cnode;
	
	resultXml = (char *)[aXmlString UTF8String];
	if (mupnp_strlen(resultXml) <= 0)
		return nil;
	
	NSMutableArray *avObjArray = [NSMutableArray array];
	
	rootNode = mupnp_xml_nodelist_new();
	xmlParser = mupnp_xml_parser_new();
	if (mupnp_xml_parse(xmlParser, rootNode, resultXml, mupnp_strlen(resultXml))) {
		didlNode = mupnp_xml_nodelist_getbyname(rootNode, "DIDL-Lite");
		if (didlNode) {
			for (cnode=mupnp_xml_node_getchildnodes(didlNode); cnode; cnode=mupnp_xml_node_next(cnode)) {
				CGUpnpAvObject *avObj = nil;
				if (mupnp_xml_node_isname(cnode, "container")) {
					CGUpnpAvContainer *avCon = [[CGUpnpAvContainer alloc] initWithXMLNode:cnode];
					avObj = avCon;
				}
				else if (mupnp_xml_node_isname(cnode, "item")) {
					CGUpnpAvItem *avItem = [[CGUpnpAvItem alloc] initWithXMLNode:cnode];
					avObj = avItem;
				}
				if (avObj == nil)
					continue;
				[avObjArray addObject:avObj];
			}
		}
	}
	mupnp_xml_nodelist_delete(rootNode);
	mupnp_xml_parser_delete(xmlParser);
#endif
	
	return avObjArray;
}

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	return self;
}

#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
#else
- (id)initWithXMLNode:(mUpnpXmlNode *)aXmlNode
#endif
{
	if ((self = [super initWithXMLNode:aXmlNode]) == nil)
		return nil;
	return self;
}

#if  defined(TARGET_OS_IPHONE)
- (id)initWithMediaContent:(mUpnpAvContent *)aMediaContent
{
	if ((self = [super initWithXMLNode:aMediaContent]) == nil)
		return nil;
	return self;
}
#endif

- (void)dealloc
{
    self.parent = nil;
    self.userInfo = nil;
}

- (BOOL)isEqual:(id)anObject
{
	if (![anObject isKindOfClass:[CGUpnpAvContainer class]])
		return NO;
	CGUpnpAvObject *avObj = (CGUpnpAvObject *)anObject;
	NSString *selfObjectId = [self objectId];
	if (selfObjectId == nil)
		return NO;
	if (!([selfObjectId isEqualToString:[avObj objectId]]))
		return NO;
	return YES;
}

- (BOOL)isContainer
{
	return [self isKindOfClass:[CGUpnpAvContainer class]];
}

- (BOOL)isItem
{
	return [self isKindOfClass:[CGUpnpAvItem class]];
}

- (CGUpnpAvObject *)ancestor
{
	CGUpnpAvObject *ancestorObj = self;
	while ([ancestorObj parent] != nil)
		ancestorObj = [ancestorObj parent];
	return ancestorObj;
}

- (NSString *)objectId
{
	return [self attributeValueForName:@CG_UPNPAV_OBJECT_ID];
}

- (NSUInteger)childCount;
{
	return mupnp_str2int([[self attributeValueForName:@CG_UPNPAV_OBJECT_CHILDCOUNT] UTF8String]);
}

- (NSString *)title;
{
	return [self elementValueForName:@CG_UPNPAV_OBJECT_TITLE];
}

- (NSString *)creator
{
    return [self elementValueForName:@CG_UPNPAV_OBJECT_CREATOR];
}

- (NSString *)upnpClass;
{
	return [self elementValueForName:@CG_UPNPAV_OBJECT_UPNPCLASS];
}

- (NSString *)date;
{
	return [self elementValueForName:@CG_UPNPAV_OBJECT_DATE];
}

- (NSString *)albumArtURI;
{
	return [self elementValueForName:@CG_UPNPAV_OBJECT_ALBUMARTURI];
}

- (void)setObjectId:(NSString *)aValue
{
	[self setAttributeWithName:@CG_UPNPAV_OBJECT_ID stringValue:aValue];
}

- (void)setTitle:(NSString *)aValue;
{
	[self setElementValueWithName:@CG_UPNPAV_OBJECT_TITLE stringValue:aValue];
}

- (void)setCreator:(NSString *)aValue
{
    [self setElementValueWithName:@CG_UPNPAV_OBJECT_CREATOR stringValue:aValue];
}

- (void)setUpnpClass:(NSString *)aValue;
{
	[self setElementValueWithName:@CG_UPNPAV_OBJECT_UPNPCLASS stringValue:aValue];
}

- (void)setAlbumArtURI:(NSString *)aValue
{
    [self setElementValueWithName:@CG_UPNPAV_OBJECT_ALBUMARTURI stringValue:aValue];
}

- (BOOL)isObjectId:(NSString *)aObjectId
{
	return [aObjectId isEqualToString:[self objectId]];
}

- (BOOL)isTitle:(NSString *)aTitle
{
	return [aTitle isEqualToString:[self title]];
}

- (BOOL)isUpnpClass:(NSString *)aUpnpClass
{
	return [aUpnpClass isEqualToString:[self upnpClass]];
}

- (NSString *)toFileNameString
{
	CGUpnpAvObject *avObj = self;
	
	NSString *avObjTitle = [avObj title];
	if ([avObj isContainer])
		return avObjTitle;
	
	CGUpnpAvItem *avItem = (CGUpnpAvItem *)avObj;
	
	CGUpnpAvResource *avResource = [avItem resource];
	if (avResource != nil) {
		NSString *extention = [avResource extention];
		if (0 < [extention length])
			return [NSString stringWithFormat:@"%@.%@", avObjTitle, extention];
	}
	
	NSString *fileExtName = @"none";
	if ([avItem isMovieClass]) {
		fileExtName = CGUPNPAV_OBJECT_MOVIE_FILEEXT;
	} else if ([avItem isAudioClass]) {
		fileExtName = CGUPNPAV_OBJECT_SOUND_FILEEXT;
	} else if ([avItem isImageClass]) {
		fileExtName = CGUPNPAV_OBJECT_IMAGE_FILEEXT;
	}
	
	return [NSString stringWithFormat:@"%@.%@", avObjTitle, fileExtName];
}

@end

