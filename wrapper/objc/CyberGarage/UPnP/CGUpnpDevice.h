//
//  CGUpnpDevice.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <cybergarage/upnp/cdevice.h>

@interface CGUpnpDevice : NSObject 
{
	CgUpnpDevice *_cObject;
}
- (CgUpnpDevice *)cObject;
- (void)setCObject:(CgUpnpDevice *)object;
- (NSString *)friendlyName;
@end
