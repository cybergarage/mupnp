//
//  CGUpnpArgument.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CARGUMENT_H_)
typedef void CgUpnpArgument;
#endif

@interface CGUpnpArgument : NSObject 
{
	CgUpnpArgument *cObject;
}
@property(readonly) CgUpnpArgument *cObject;
- (id) initWithCObject:(CgUpnpArgument *)cobj;
- (NSString *)name;
@end
