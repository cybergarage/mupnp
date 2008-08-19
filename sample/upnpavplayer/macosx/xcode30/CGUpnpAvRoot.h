//
//  CGUpnpAvRoot.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#if defined(__MACOSX__)
#import <Cocoa/Cocoa.h>
#else
#import <UIKit/UIKit.h>
#endif

@class CGUpnpAvContainer;

/**
 * The CGUpnpAvRoot class is a root container of UPnP/AV.
 */
@interface CGUpnpAvRoot : CGUpnpAvContainer 
{
}
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
@end

