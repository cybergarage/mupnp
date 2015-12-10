//
//  CGUpnpAvServer.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <mupnp/std/av/cmediarenderer.h>

#import "CGUpnpAvRenderer.h"
#import "CGUpnpAVPositionInfo.h"
#import "CGUpnpAVPositionInfo.h"
#import "CGUpnpService.h"
#import "CGUpnpAction.h"

#define AUDIO_PROTOCOL_M4A @"http-get:*:audio/mp4:*;DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01700000000000000000000000000000"
#define AUDIO_PROTOCOL_MP3 @"http-get:*:audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01700000000000000000000000000000"
#define AUDIO_PROTOCOL_WAV @"http-get:*:audio/wav:*;DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01700000000000000000000000000000"
#define AUDIO_PROTOCOL_AIFF @"http-get:*:audio/x-aiff:*;DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01700000000000000000000000000000"


@implementation DMRMediaItem

@end


@interface CGUpnpAvRenderer()
@property (assign) int currentPlayMode;
@end

enum {
	CGUpnpAvRendererPlayModePlay,
	CGUpnpAvRendererPlayModePause,
	CGUpnpAvRendererPlayModeStop,
};

@implementation CGUpnpAvRenderer

@synthesize cAvObject;
@synthesize currentPlayMode;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;

	cAvObject = mupnp_upnpav_dmr_new();
	[self setCObject:mupnp_upnpav_dmr_getdevice(cAvObject)];
	
	[self setCurrentPlayMode:CGUpnpAvRendererPlayModeStop];
	
	return self;
}

- (id) initWithCObject:(mUpnpDevice *)cobj
{
	if ((self = [super initWithCObject:cobj]) == nil)
		return nil;

	cAvObject = NULL;

	return self;
}

- (CGUpnpAction *)actionOfTransportServiceForName:(NSString *)serviceName
{
	CGUpnpService *avTransService = [self getServiceForType:@"urn:schemas-upnp-org:service:AVTransport:1"];
	if (!avTransService)
		return nil;
	
	return [avTransService getActionForName:serviceName];
}

- (BOOL)setAVTransportURI:(NSString *)aURL;
{
	CGUpnpAction *action = [self actionOfTransportServiceForName:@"SetAVTransportURI"];
	if (!action)
		return NO;
    
	[action setArgumentValue:@"0" forName:@"InstanceID"];
	[action setArgumentValue:aURL forName:@"CurrentURI"];
	[action setArgumentValue:@"" forName:@"CurrentURIMetaData"];
	
	if (![action post])
		return NO;
	
	return YES;
}

- (BOOL)play;
{
	CGUpnpAction *action = [self actionOfTransportServiceForName:@"Play"];
	if (!action)
		return NO;
	
	[action setArgumentValue:@"0" forName:@"InstanceID"];
	[action setArgumentValue:@"1" forName:@"Speed"];
	
	if (![action post])
		return NO;
	
	[self setCurrentPlayMode:CGUpnpAvRendererPlayModePlay];
	
	return YES;
}

- (BOOL)stop;
{
	CGUpnpAction *action = [self actionOfTransportServiceForName:@"Stop"];
	if (!action)
		return NO;
	
	[action setArgumentValue:@"0" forName:@"InstanceID"];
	
	if (![action post])
		return NO;
	
	[self setCurrentPlayMode:CGUpnpAvRendererPlayModeStop];
	
	return YES;
}

- (BOOL)pause
{
	CGUpnpAction *action = [self actionOfTransportServiceForName:@"Pause"];
	if (!action)
		return NO;
	
	[action setArgumentValue:@"0" forName:@"InstanceID"];
	
	if (![action post])
		return NO;
	
	[self setCurrentPlayMode:CGUpnpAvRendererPlayModePause];
	
	return YES;
}

- (BOOL)seek:(float)absTime
{
	CGUpnpAction *action = [self actionOfTransportServiceForName:@"Seek"];
	if (!action)
		return NO;
	
	[action setArgumentValue:@"0" forName:@"InstanceID"];
	[action setArgumentValue:@"ABS_TIME" forName:@"Unit"];
	[action setArgumentValue:[NSString stringWithDurationTime:absTime] forName:@"Target"];
	
	if (![action post])
		return NO;
	
	return YES;
}

- (BOOL)isPlaying
{
	if ([self currentPlayMode] == CGUpnpAvRendererPlayModePlay)
		return YES;
	return NO;
}

- (CGUpnpAVPositionInfo *)positionInfo
{
	CGUpnpAction *action = [self actionOfTransportServiceForName:@"GetPositionInfo"];
	if (!action)
		return NO;
	
	[action setArgumentValue:@"0" forName:@"InstanceID"];
	
	if (![action post])
		return nil;
	
	return [[CGUpnpAVPositionInfo alloc] initWithAction:action];
}

/*
- (BOOL)start
{
	if (!cAvObject)
		return NO;
	return cg_upnpav_dms_start(cAvObject);
}

- (BOOL)stop
{
	if (!cAvObject)
		return NO;
	return cg_upnpav_dms_stop(cAvObject);
}
*/

#pragma mark Wrap the render with necessary infomation for playing

- (void)setNowPlayingItem:(DMRMediaItem *)nowPlayingItem {
    _nowPlayingItem = nowPlayingItem;
    if ([self setAVTransportURI:nowPlayingItem.assetURL]) {
        [self play];
    }
}

- (void)setPlayerItemCollection:(NSArray<DMRMediaItem *> *)playerItemCollection {
    _playerItemCollection = playerItemCollection;
}

- (void)playMusicWithIndex:(NSInteger)index {
    if (index < [self.playerItemCollection count]) {
        DMRMediaItem *item = [self.playerItemCollection objectAtIndex:index];
        [self setNowPlayingItem:item];
    }
}

- (DMRMediaItem *)itemAtIndex:(NSInteger)index {
    DMRMediaItem *item = nil;
    if (index < [self.playerItemCollection count]) {
        item = [self.playerItemCollection objectAtIndex:index];
    }
    return item;
}

- (void)skipToNextItem {
    
}

- (void)skipToBeginning {
    
}

- (void)skipToPreviousItem {
    
}

- (void)beginGeneratingPlaybackNotifications {
    
}

- (void)endGeneratingPlaybackNotifications {
    
}

@end
