//
//  CGUpnpAvObject.h
//  mUPnP for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#if defined(TARGET_OS_IPHONE)
#import <UIKit/UIKit.h>
#endif

#import <CyberLink/CGXmlNode.h>

#if !defined(_CG_CLINKCAV_CONTENT_H_)
typedef void CgUpnpAvContent;
#endif

/**
 * The CGUpnpAvObject class is a wrapper class for CgUpnpAvObject of mUPnP for C.
 */

#define CGUPNPAV_OBJECT_MOVIE_FILEEXT @"mov"
#define CGUPNPAV_OBJECT_SOUND_FILEEXT @"snd"
#define CGUPNPAV_OBJECT_IMAGE_FILEEXT @"img"

@interface CGUpnpAvObject : CGXmlNode {
}
@property (retain) CGUpnpAvObject* parent;
@property (retain) id userObject;
+ (NSArray*)arrayWithXMLString:(NSString*)aXmlString;
- (id)init;
#if !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement*)aXmlNode;
#else
- (id)initWithXMLNode:(CgXmlNode*)aXmlNode;
- (id)initWithMediaContent:(CgUpnpAvContent*)aMediaContent;
#endif
- (BOOL)isEqual:(id)anObject;
- (BOOL)isContainer;
- (BOOL)isItem;
- (void)setParent:(CGUpnpAvObject*)aParent;
- (CGUpnpAvObject*)parent;
- (CGUpnpAvObject*)ancestor;
- (NSString*)objectId;
- (NSUInteger)childCount;
- (NSString*)title;
- (NSString*)upnpClass;
- (NSString*)date;
- (NSString*)albumArtURI;
- (void)setObjectId:(NSString*)aValue;
- (void)setTitle:(NSString*)aValue;
- (void)setUpnpClass:(NSString*)aValue;
- (BOOL)isObjectId:(NSString*)aObjectId;
- (BOOL)isTitle:(NSString*)aTitle;
- (BOOL)isUpnpClass:(NSString*)aUpnpClass;
- (NSString*)toFileNameString;
@end
