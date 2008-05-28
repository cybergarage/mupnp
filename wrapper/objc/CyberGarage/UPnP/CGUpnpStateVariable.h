//
//  CGUpnpStateVariable.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CSTATEVARIABLE_H_)
typedef void CgUpnpStateVariable;
#endif

@interface CGUpnpStateVariable : NSObject 
{
	CgUpnpStateVariable *cObject;
}
@property(readonly) CgUpnpStateVariable *cObject;
- (id)initWithCObject:(CgUpnpStateVariable *)cobj;
- (NSString *)name;
- (NSString *)value;
- (BOOL)query;
- (int)statusCode;
@end
