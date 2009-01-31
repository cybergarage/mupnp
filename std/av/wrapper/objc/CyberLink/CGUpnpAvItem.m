//
//  CGUpnpAvObject.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvResource.h"
#import "CGUpnpAvItem.h"

@implementation CGUpnpAvItem

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	resourceArray = [[NSMutableArray alloc] init];
	return self;
}

#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode
#endif
{
	if ((self = [super initWithXMLNode:aXmlNode]) == nil)
		return nil;
	resourceArray = [[NSMutableArray alloc] init];
	return self;
}

- (void)dealloc
{
	[resourceArray release];
	[super dealloc];
}

- (void) finalize
{
	[resourceArray release];
	[super finalize];
}

- (void)addResource:(CGUpnpAvResource *)res
{
	[resourceArray addObject:res];
}

- (void)removeResource:(CGUpnpAvResource *)res
{
	[resourceArray removeObject:res];
}

- (NSArray *)resources
{
	return resourceArray;
}

- (CGUpnpAvResource *)smallImageResource
{
	for (CGUpnpAvResource *res in [self resources]) {
		if ([res isSmallImage])
			return [[res retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvResource *)mediumImageResource
{
	for (CGUpnpAvResource *res in [self resources]) {
		if ([res isMediumImage])
			return [[res retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvResource *)largeImageResource
{
	for (CGUpnpAvResource *res in [self resources]) {
		if ([res isLargeImage])
			return [[res retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvResource *)lowestImageResource
{
	CGUpnpAvResource *imgRes = nil;
	imgRes = [self smallImageResource];
	if (imgRes == nil) {
		imgRes = [self mediumImageResource];
		if (imgRes == nil) {
			imgRes = [self largeImageResource];
			if (imgRes == nil) {
				for (CGUpnpAvResource *res in [self resources]) {
					if (![res isThumbnail]) {
						imgRes = res;
						break;
					}
				}
			}
		}
	
	}
	return [[imgRes retain] autorelease];
}

- (CGUpnpAvResource *)highestImageResource
{
	CGUpnpAvResource *imgRes = nil;
	imgRes = [self largeImageResource];
	if (imgRes == nil) {
		imgRes = [self mediumImageResource];
		if (imgRes == nil) {
			imgRes = [self smallImageResource];
			if (imgRes == nil) {
				for (CGUpnpAvResource *res in [self resources]) {
					if (![res isThumbnail]) {
						imgRes = res;
						break;
					}
				}
			}
		}
		
	}
	return [[imgRes retain] autorelease];
}

- (NSString *)thumbnailUrl
{
	NSString *tbUrl = [self albumArtURI];
	if (tbUrl && 0 < [tbUrl length])
		return [[tbUrl retain] autorelease];
	for (CGUpnpAvResource *res in [self resources]) {
		if ([res isThumbnail])
			return [[[res url] retain] autorelease];
	}
	return nil;
}

- (NSString *)smallImageUrl
{
	return [[[[self smallImageResource] url] retain] autorelease];
}

- (NSString *)mediumImageUrl
{
	return [[[[self mediumImageResource] url] retain] autorelease];
}

- (NSString *)largeImageUrl
{
	return [[[[self largeImageResource] url] retain] autorelease];
}

- (NSString *)lowestImageUrl
{
	return [[[[self lowestImageResource] url] retain] autorelease];
}

- (NSString *)highestImageUrl
{
	return [[[[self highestImageResource] url] retain] autorelease];
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

@end
