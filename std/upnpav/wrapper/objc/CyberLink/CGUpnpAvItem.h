//
//  CGUpnpAvItem.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CyberLink/CGUpnpAvObject.h>
#import <CyberLink/CGUpnpAvResource.h>

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvItem : CGUpnpAvObject 
{
	NSMutableArray *resourceArray;
}
- (id)init;
#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode;
#endif
- (void)addResource:(CGUpnpAvResource *)res;
- (void)removeResource:(CGUpnpAvResource *)res;
- (NSArray *)resources;
- (NSString *)thumbnailUrl;
@end
