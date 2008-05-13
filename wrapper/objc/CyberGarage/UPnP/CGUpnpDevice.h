//
//  CGUpnpDevice.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CDEVICE_H_)
typedef void CgUpnpDevice;
#endif

@interface CGUpnpDevice : NSObject 
{
	CgUpnpDevice *_cObject;
}
@property(readonly) CgUpnpDevice *_cObject;
- (id) initWithCObject(CgUpnpDevice *cobj);
- (NSString *)friendlyName;
- (NSString *)deviceType;
- CGUpnpService *getServiceByID:(NSString *)id;
- CGUpnpService *getServiceByType:(NSString *)type;
@end
