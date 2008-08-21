//
//  CGUpnpAvResource.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

@class CGXmlNode;

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvResource : CGXmlNode 
{
}
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
- (NSString *)url;
- (long long)size;
- (NSAtring *)protocolInfo;
- (NSString *)protocolInfoAtIndex:(NSUInteger)anIndex;
- (NSString *)protocol
- (NSString *)network
- (NSString *)contentFormat
- (NSString *)additionalInfo
- (NSString *)additionalInfoForKey:(NSString*)aKey;
- (NSString *)dlnaOrgPn;
- (NSString *)dlnaOrgOp;
- (NSString *)dlnaOrgFlags;
@end

