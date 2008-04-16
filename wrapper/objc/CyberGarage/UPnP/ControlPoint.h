//
//  ControlPoint.h
//  clinkc
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include <cybergarage/upnp/ccontrolpoint.h>

@interface ControlPoint : NSObject {
	CgUpnpControlPoint *cgCtrlPoint;

}

@end
