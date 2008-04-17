//
//  CGUpnpDevice.h
//  clinkc
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <cybergarage/upnp/cdevice.h>

@interface CGUpnpDevice : NSObject 
{
	CgUpnpDevice *_object;
}
- (CgUpnpDevice *)object;
- (void)setObject:(CgUpnpDevice *)object;
- (NSString *)friendlyName;
@end
