//
//  ControlPoint.m
//  clinkc
//
//  Created by 今野 賢 on 08/03/14.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "ControlPoint.h"

@implementation ControlPoint

- (id) init
{
	cgCtrlPoint = cg_upnp_controlpoint_new();
}

- (void) dealloc
{
	if (cgCtrlPoint)
		cg_upnp_controlpoint_delete(cgCtrlPoint);
	[super dealloc];
}

@end
