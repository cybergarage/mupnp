//
//  ControlPoint.h
//  clinkc
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <cybergarage/upnp/ccontrolpoint.h>

@interface CGControlPoint : NSObject {
	CgUpnpControlPoint *cgCtrlPoint;

}

@end
