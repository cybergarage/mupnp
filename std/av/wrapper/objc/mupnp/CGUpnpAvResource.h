//
//  CGUpnpAvResource.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <mUpnP/CGXmlNode.h>
#import <UIKit/UIKit.h>

/**
 * The CGUpnpAvResource class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvResource : CGXmlNode 
{
}
- (id)init;
#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(mUpnpXmlNode *)aXmlNode;
#endif
- (NSString *)url;
- (void)setUrl:(NSString *)resUrl;
- (long long)size;
- (void)setSize:(long long)size;
- (void)setDuration:(NSUInteger)duration;
- (NSString *)protocolInfo;
- (void)setProtocolInfo:(NSString *)protocolInfo;
- (NSString *)protocolInfoAtIndex:(NSUInteger)anIndex;
- (NSString *)protocol;
- (NSString *)network;
- (NSString *)contentFormat;
- (NSString *)mimeType;
- (NSString *)extention;
- (NSString *)additionalInfo;
- (NSString *)additionalInfoForKey:(NSString*)aKey;
- (NSString *)dlnaOrgPn;
- (NSString *)dlnaOrgOp;
- (NSString *)dlnaOrgFlags;
- (CGSize)resolution;
- (BOOL)isThumbnail;
- (BOOL)isSmallImage;
- (BOOL)isMediumImage;
- (BOOL)isLargeImage;
- (BOOL)isImage;
- (BOOL)isMovie;
- (BOOL)isVideo;
- (BOOL)isAudio;

- (NSComparisonResult)imageSizeCompare:(CGUpnpAvResource *)otherResource;

@end

