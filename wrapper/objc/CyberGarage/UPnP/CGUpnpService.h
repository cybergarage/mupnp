//
//  CGUpnpService.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CSERVICE_H_)
typedef void CgUpnpService;
#endif

@interface CGUpnpDevice : NSObject 
{
	CgUpnpService *_cObject;
}
- (CgUpnpService *)cObject;
- (void)setCObject:(CgUpnpService *)object;
- (NSString *)serviceType;
@end
