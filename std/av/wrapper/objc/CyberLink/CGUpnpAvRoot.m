//
//  CGUpnpAvContainer.m
//  mUPnP for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <cybergarage/upnp/std/av/cupnpav.h>

#import "CGUpnpAvConstants.h"
#import "CGUpnpAvContainer.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvRoot.h"
#import "CGXmlNode.h"

@implementation CGUpnpAvRoot

- (id)init
{
  if ((self = [super init]) == nil)
    return nil;
  [self setObjectId:@CG_UPNPAV_OBJECT_ROOT_ID];
  return self;
}

#if !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement*)aXmlNode
#else
- (id)initWithXMLNode:(CgXmlNode*)aXmlNode
#endif
{
  if ((self = [super initWithXMLNode:aXmlNode]) == nil)
    return nil;
  return self;
}

- (void)dealloc
{
  [super dealloc];
}

@end
