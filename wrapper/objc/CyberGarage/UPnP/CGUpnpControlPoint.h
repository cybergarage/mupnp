//
//  CGUpnpControlPoint.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CCONTROLPOINT_H_)
typedef void CgUpnpControlPoint;
#endif

@interface CGUpnpControlPoint : NSObject 
{
	CgUpnpControlPoint *cObject;
}
@property(readonly) CgUpnpControlPoint *cObject;
- (CgUpnpControlPoint *)cObject;
- (BOOL)start;
- (BOOL)stop;
- (void)search;
- (void)searchWithST:(NSString *)aST;
- (NSArray *)getDeviceArray;
@end
