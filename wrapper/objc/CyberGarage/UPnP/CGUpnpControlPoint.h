//
//  CGUpnpControlPoint.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <cybergarage/upnp/ccontrolpoint.h>

@interface CGUpnpControlPoint : NSObject 
{
	CgUpnpControlPoint *_cObject;
}
- (CgUpnpControlPoint *)cObject;
- (void)search;
- (void)searchWithST:(NSString *)aST;
- (NSArray *)getDeviceArray;
@end
