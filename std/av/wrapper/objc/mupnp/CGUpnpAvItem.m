//
//  CGUpnpAvItem.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <mupnp/std/av/cupnpav.h>
#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvResource.h"
#import "CGUpnpAvItem.h"

@interface CGUpnpAvItem()
+ (NSArray *)dlnaStandardImageMimeTypes;
- (NSArray *)resourcesWithMimeTypes:(NSArray *)mimeTypes;
- (NSArray *)sortedImageResouces;
- (NSArray *)sortedImageResoucesWithMimeTypes:(NSArray *)mimeTypes;
@end

@implementation CGUpnpAvItem

@synthesize resourceArray;

#if defined(TARGET_OS_IPHONE)
@synthesize thumbnailImage;
#endif

+ (NSArray *)dlnaStandardImageMimeTypes
{
    return [NSArray arrayWithObjects:@"image/jpeg", @"image/png", @"image/gif", nil];
}

+ (CGUpnpAvItem *)avItemWithXMLString:(NSString *)aXmlString
{
    char *resultXml;
    mUpnpXmlParser *xmlParser;
    mUpnpXmlNodeList *rootNode;
    mUpnpXmlNode *didlNode;
    mUpnpXmlNode *cnode;
    
    resultXml = (char *)[aXmlString UTF8String];
    if (mupnp_strlen(resultXml) <= 0)
        return nil;
    
    CGUpnpAvItem *avObj = nil;
    rootNode = mupnp_xml_nodelist_new();
    xmlParser = mupnp_xml_parser_new();
    if (mupnp_xml_parse(xmlParser, rootNode, resultXml, mupnp_strlen(resultXml)))
    {
        didlNode = mupnp_xml_nodelist_getbyname(rootNode, "DIDL-Lite");
        if (NULL != didlNode) {
            for (cnode=mupnp_xml_node_getchildnodes(didlNode); cnode; cnode=mupnp_xml_node_next(cnode)) {
                if (mupnp_xml_node_isname(cnode, "item")) {
                    CGUpnpAvItem *avItem = [[CGUpnpAvItem alloc] initWithXMLNode:cnode];
                    avObj = avItem;
                    break;
                }
            }
        } else {
            cnode = mupnp_xml_nodelist_getbyname(rootNode, "item");
            CGUpnpAvItem *avItem = [[CGUpnpAvItem alloc] initWithXMLNode:cnode];
            avObj = avItem;
        }
    }
    
    return avObj;
}

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
    [self setNodeName:@CG_UPNPAV_OBJECT_ITEM];
    [self setAttributeWithName:@CG_UPNPAV_OBJECT_ID stringValue:@"0"];
    [self setAttributeWithName:@CG_UPNPAV_OBJECT_PARENTID stringValue:@CG_UPNPAV_ROOT_CONTENT_PARENTID];
    [self setAttributeWithName:@CG_UPNPAV_OBJECT_RESTRICTED stringValue:@"0"];
    
	self.resourceArray = [NSMutableArray array];
	return self;
}

#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
{
    if ((self = [super initWithXMLNode:aXmlNode]) == nil)
        return nil;
    
    self.resourceArray = [NSMutableArray array];
    NSArray *resArray = [aXmlNode elementsForName:@"res"];
    for (NSXMLElement *resNode in resArray) {
        CGUpnpAvResource *avRes = [[CGUpnpAvResource alloc] initWithXMLNode:resNode];
        [self.resourceArray addObject:avRes];
        [avRes release];
    }
    
    return self;
}
#else
- (id)initWithXMLNode:(mUpnpXmlNode *)aXmlNode
{
    mUpnpXmlNode *rnode;
	if ((self = [super initWithXMLNode:aXmlNode]) == nil)
		return nil;
    
	self.resourceArray = [NSMutableArray array];
    for (rnode=mupnp_xml_node_getchildnodes(aXmlNode); rnode; rnode=mupnp_xml_node_next(rnode)) {
        if (mupnp_xml_node_isname(rnode, "res")) {
            CGUpnpAvResource *avRes = [[CGUpnpAvResource alloc] initWithXMLNode:rnode];
            [self.resourceArray addObject:avRes];
        }
    }
    
	return self;
}
#endif

- (void)dealloc
{
	self.resourceArray = nil;
#if defined(TARGET_OS_IPHONE)
    self.thumbnailImage = nil;
#endif
}

- (void)addResource:(CGUpnpAvResource *)res
{
    [self addChildNode:res];
	[[self resourceArray] addObject:res];
}

- (void)removeResource:(CGUpnpAvResource *)res
{
	[[self resourceArray] removeObject:res];
}

- (NSArray *)resources
{
	return [self resourceArray];
}

- (NSString *)metaData
{
    NSString *metaData = [NSString stringWithFormat:@"<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:sec=\"http://www.sec.co.kr/\">%@</DIDL-Lite>", [self xmlNodeToString]];
    return metaData;
}

- (NSArray *)resourcesWithMimeTypes:(NSArray *)mimeTypes
{
	NSMutableArray *imageResouces = [NSMutableArray array];
	for (CGUpnpAvResource *resource in [self resources]) {
        NSString *resourceMimeType = [resource mimeType];
        if (resourceMimeType == nil || [resourceMimeType length] <= 0)
        	continue;
        for (NSString *mimeType in mimeTypes) {
            if ([mimeType isEqualToString:resourceMimeType]) {
            	[imageResouces addObject:resource];
                break;
            }
        }
	}
    return imageResouces;
}

- (CGUpnpAvResource *)resource
{
	if ([self isImageClass])
		return [self imageResource];
	if ([self isAudioClass])
		return [self audioResource];
	if ([self isMovieClass])
		return [self movieResource];	
	return nil;
}

- (BOOL)hasRendererResource;
{
	return ([self rendererResource] != nil) ? YES : NO;
}

- (CGUpnpAvResource *)rendererResource;
{
	if ([self isImageClass])
		return [self highestImageResource];
	if ([self isAudioClass])
		return [self audioResource];
	if ([self isMovieClass])
		return [self movieResource];	
	return nil;
}

- (NSURL *)resourceUrl
{
	CGUpnpAvResource *resource = [self resource];
	if (resource == nil)
		return nil;
	return [NSURL URLWithString:[resource url]];
}

- (CGUpnpAvResource *)smallImageResourceWithMimeTypes:(NSArray *)mimeTypes
{
	for (CGUpnpAvResource *res in [self resourcesWithMimeTypes:mimeTypes]) {
		if ([res isSmallImage])
			return res;
	}
	return nil;
}

- (CGUpnpAvResource *)smallImageResource
{
    return [self smallImageResourceWithMimeTypes:[CGUpnpAvItem dlnaStandardImageMimeTypes]];
}

- (CGUpnpAvResource *)mediumImageResourceWithMimeTypes:(NSArray *)mimeTypes
{
	for (CGUpnpAvResource *res in [self resourcesWithMimeTypes:mimeTypes]) {
		if ([res isMediumImage])
			return res;
	}
	return nil;
}

- (CGUpnpAvResource *)mediumImageResource
{
    return [self mediumImageResourceWithMimeTypes:[CGUpnpAvItem dlnaStandardImageMimeTypes]];
}

- (CGUpnpAvResource *)largeImageResourceWithMimeTypes:(NSArray *)mimeTypes
{
	for (CGUpnpAvResource *res in [self resourcesWithMimeTypes:mimeTypes]) {
		if ([res isLargeImage])
			return res;
	}
	return nil;
}

- (CGUpnpAvResource *)largeImageResource
{
    return [self largeImageResourceWithMimeTypes:[CGUpnpAvItem dlnaStandardImageMimeTypes]];
}

- (NSArray *)sortedImageResouces
{
	NSMutableArray *imageResouces = [NSMutableArray array];
    
	for (CGUpnpAvResource *resource in [self resources]) {
		if ([resource isImage] == NO)
			continue;
        [imageResouces addObject:resource];
	}
	
    return [imageResouces sortedArrayUsingSelector:@selector(imageSizeCompare:)];
}

- (NSArray *)sortedImageResoucesWithMimeTypes:(NSArray *)mimeTypes
{
	NSMutableArray *imageResouces = [NSMutableArray array];
	for (CGUpnpAvResource *resource in [self sortedImageResouces]) {
        NSString *resourceMimeType = [resource mimeType];
        if (resourceMimeType == nil || [resourceMimeType length] <= 0)
        	continue;
        for (NSString *mimeType in mimeTypes) {
            if ([mimeType isEqualToString:resourceMimeType]) {
            	[imageResouces addObject:resource];
                break;
            }
        }
	}
    return imageResouces;
}

- (CGUpnpAvResource *)lowestImageResourceWithMimeTypes:(NSArray *)mimeTypes
{
    NSArray *sortedImageResouces = [self sortedImageResoucesWithMimeTypes:mimeTypes];
    
    if ([sortedImageResouces count] <= 0)
        return nil;
    
    return [sortedImageResouces objectAtIndex:0];
}


- (CGUpnpAvResource *)lowestImageResource
{
    return [self lowestImageResourceWithMimeTypes:[CGUpnpAvItem dlnaStandardImageMimeTypes]];
}

- (CGUpnpAvResource *)highestImageResourceWithMimeTypes:(NSArray *)mimeTypes
{
    NSArray *sortedImageResouces = [self sortedImageResoucesWithMimeTypes:mimeTypes];
    
    if ([sortedImageResouces count] <= 0)
        return nil;
    
    return [sortedImageResouces objectAtIndex:([sortedImageResouces count] - 1)];
}

- (CGUpnpAvResource *)highestImageResource
{
    return [self highestImageResourceWithMimeTypes:[CGUpnpAvItem dlnaStandardImageMimeTypes]];
}

- (CGUpnpAvResource *)applicableImageResourceBySize:(CGSize)wantedSize mimeTypes:(NSArray *)mimeTypes
{
	NSArray *imageResouces = [self sortedImageResoucesWithMimeTypes:mimeTypes];
	
	CGUpnpAvResource *applicableResource = nil;
	for (CGUpnpAvResource *resource in imageResouces) {
		if (applicableResource == nil) {
			applicableResource = resource;
			continue;
		}
		CGSize resourceSize = [resource resolution];
        if (wantedSize.width < resourceSize.width)
        	break;
        applicableResource = resource;
	}
	return applicableResource;
}

- (CGUpnpAvResource *)applicableImageResourceBySize:(CGSize)wantedSize
{
	return [self applicableImageResourceBySize:wantedSize mimeTypes:[CGUpnpAvItem dlnaStandardImageMimeTypes]];
}

- (NSString *)thumbnailUrl
{
	NSString *tbUrl = [self albumArtURI];
	if (tbUrl && 0 < [tbUrl length])
		return tbUrl;
	for (CGUpnpAvResource *res in [self resources]) {
		if ([res isThumbnail])
			return [res url];
	}
	return nil;
}

- (NSString *)smallImageUrl
{
	return [[self smallImageResource] url];
}

- (NSString *)mediumImageUrl
{
	return [[self mediumImageResource] url];
}

- (NSString *)largeImageUrl
{
	return [[self largeImageResource] url];
}

- (NSString *)lowestImageUrl
{
	return [[self lowestImageResource] url];
}

- (NSString *)highestImageUrl
{
	return [[self highestImageResource] url];
}

- (NSString *)applicableImageUrlBySize:(CGSize)size
{
	return [[self applicableImageResourceBySize:size] url];
}

- (CGUpnpAvResource *)movieResource
{
	for (CGUpnpAvResource *res in [self resources]) {
		if ([res isMovie])
			return res;
	}
	return nil;
}

- (CGUpnpAvResource *)videoResource
{
	return [self movieResource];
}

- (CGUpnpAvResource *)audioResource
{
	for (CGUpnpAvResource *res in [self resources]) {
		if ([res isAudio])
			return res;
	}
	return nil;
}

- (CGUpnpAvResource *)imageResource
{
	return [self lowestImageResource];
}

-(BOOL)hasMovieResource
{
	if ([self movieResource] != nil)
		return YES;
	return NO;
}

-(BOOL)hasVideoResource
{
	return [self hasMovieResource];
}

-(BOOL)hasAudioResource
{
	if ([self audioResource] != nil)
		return YES;
	return NO;
}

-(BOOL)hasImageResource
{
	if ([self imageResource] != nil)
		return YES;
	return NO;
}

-(BOOL)isMovieClass
{
	if ([[self upnpClass] rangeOfString:@"movie"].location != NSNotFound || [[self upnpClass] rangeOfString:@"video"].location != NSNotFound)
		return YES;
	return NO;
}

-(BOOL)isVideoClass
{
	return [self isMovieClass];
}

-(BOOL)isAudioClass
{
	if ([[self upnpClass] rangeOfString:@"audio"].location != NSNotFound || [[self upnpClass] rangeOfString:@"music"].location != NSNotFound)
		return YES;
	return NO;
}

-(BOOL)isImageClass
{
	if ([[self upnpClass] rangeOfString:@"image"].location != NSNotFound || [[self upnpClass] rangeOfString:@"photo"].location != NSNotFound)
		return YES;
	return NO;
}

#pragma mark

- (void)setArtist:(NSString *)aValue
{
    [self setElementValueWithName:@CG_UPNPAV_OBJECT_ARTIST stringValue:aValue];
}

- (void)setAlbum:(NSString *)aValue
{
    [self setElementValueWithName:@CG_UPNPAV_OBJECT_ALBUM stringValue:aValue];
}

- (void)setGenre:(NSString *)aValue
{
    [self setElementValueWithName:@CG_UPNPAV_OBJECT_GENRE stringValue:aValue];
}

#pragma mark save

- (BOOL)writeToFile:(NSString *)path
{
	CGUpnpAvResource *avResource = [self resource];
	if (avResource == nil)
		return NO;

	NSURL *avResourceUrl = [NSURL URLWithString:[avResource url]];
	if (avResourceUrl == nil)
		return NO;

	NSData *avResourceData = [NSData dataWithContentsOfURL:avResourceUrl];
	if (avResourceData == nil)
		return NO;
		
	return [avResourceData writeToFile:path atomically:NO];
}

@end
