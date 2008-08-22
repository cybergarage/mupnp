//
//  CGUpnpAvResource.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>
#import <Foundation/NSXMLElement.h>

@class CGXmlNode;

/**
 * The CGUpnpAvResource class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvResource : CGXmlNode 
{
}
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
- (NSString *)url;
- (long long)size;
- (NSString *)protocolInfo;
- (NSString *)protocolInfoAtIndex:(NSUInteger)anIndex;
- (NSString *)protocol;
- (NSString *)network;
- (NSString *)contentFormat;
- (NSString *)additionalInfo;
- (NSString *)additionalInfoForKey:(NSString*)aKey;
- (NSString *)dlnaOrgPn;
- (NSString *)dlnaOrgOp;
- (NSString *)dlnaOrgFlags;
@end

