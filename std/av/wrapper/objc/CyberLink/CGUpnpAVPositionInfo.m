//
//  CGUpnpAVPositionInfo.m
//  
//
//  Created by Satoshi Konno on 10/12/01.
//  Copyright 2010 Yahoo Japan Corporation. All rights reserved.
//

#import "CGUpnpAVPositionInfo.h"

@implementation CGUpnpAVPositionInfo

@synthesize upnpAction;

-(id)initWithAction:(CGUpnpAction *)aUpnpAction
{
	if (self = [super init]) {
		[self setUpnpAction:aUpnpAction];
	}
	return self;
}

-(NSString *)trackDuration
{
	return [[self upnpAction] argumentValueForName:@"TrackDuration"];
}

-(NSString *)absTime
{
	return [[self upnpAction] argumentValueForName:@"AbsTime"];
}

@end
