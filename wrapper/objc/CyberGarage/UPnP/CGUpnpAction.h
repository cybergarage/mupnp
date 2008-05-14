//
//  CGUpnpAction.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CACTION_H_)
typedef void CgUpnpAction;
#endif

@interface CGUpnpAction : NSObject 
{
	CgUpnpAction *cObject;
}
@property(readonly) CgUpnpAction *cObject;
- (id) initWithCObject:(CgUpnpAction *)cobj;
- (NSString *)name;
@end
