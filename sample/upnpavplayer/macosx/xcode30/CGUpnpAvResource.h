//
//  CGUpnpAvResource.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class CGXmlNode;

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvResource : CGXmlNode 
{
}
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
//- (NSString *)mimeType;
- (NSString *)url;
- (long long)size;
- (NSString *)dlnaOrgPN;
- (NSString *)dlnaOrgOP;
@end

