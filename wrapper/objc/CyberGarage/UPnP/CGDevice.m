//
//  ControlPoint.m
//  clinkc
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGControlPoint.h"

@implementation CGControlPoint

- (id) init
{
	cgCtrlPoint = cg_upnp_controlpoint_new();
	if (cgCtrlPoint)
		self = nil;
	return self;
}

- (void) finalize
{
	if (cgCtrlPoint)
		cg_upnp_controlpoint_delete(cgCtrlPoint);
	[super finalize];
}

@end
