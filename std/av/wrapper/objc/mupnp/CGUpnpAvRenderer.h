//
//  CGUpnpAvServer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

#import <mUPnP/CGUPnPDevice.h>
#import <mUPnP/CGUpnpAVPositionInfo.h>

#import <MediaPlayer/MPMediaItem.h>

@class CGUpnpAVPositionInfo;

typedef enum : NSUInteger {
    DMRMusicPlaybackStateStopped,
    DMRMusicPlaybackStatePlaying,
    DMRMusicPlaybackStatePaused,
    DMRMusicPlaybackStateInterrupted,
    DMRMusicPlaybackStateSeekingForward,
    DMRMusicPlaybackStateSeekingBackward
} DMRMusicPlaybackState;

typedef enum : NSUInteger {
    DMRMusicRepeatModeDefault, // the user's preference for repeat mode
    DMRMusicRepeatModeNone,
    DMRMusicRepeatModeOne,
    DMRMusicRepeatModeAll
} DMRMusicRepeatMode;

typedef enum : NSUInteger {
    DMRMusicShuffleModeDefault, // the user's preference for shuffle mode
    DMRMusicShuffleModeOff,
    DMRMusicShuffleModeSongs,
    DMRMusicShuffleModeAlbums
} DMRMusicShuffleMode;


@interface DMRMediaItem : NSObject

@property (nonatomic, retain) NSString *assetURL;
@property (nonatomic, retain) MPMediaItem *mediaItem;

@end


#if !defined(_CG_CLINKCAV_MEDIARENDERER_H_)
typedef void mUpnpAvRenderer;
#endif

/**
 * The CGUpnpAvRenderer class is a UPnP/AV media server class.
 */
@interface CGUpnpAvRenderer : CGUpnpDevice
{
}

@property (assign,readonly) mUpnpAvRenderer *cAvObject;

@property(nonatomic, retain) NSArray<DMRMediaItem *> *playerItemCollection;
@property (nonatomic, retain) DMRMediaItem *nowPlayingItem;
@property (nonatomic, readonly) NSUInteger indexOfNowPlayingItem; // return NSNotFound if the index is not valid
@property (nonatomic) float volume;
@property (nonatomic, readonly) DMRMusicPlaybackState playbackState;
@property (nonatomic) DMRMusicRepeatMode repeatMode;
@property (nonatomic) DMRMusicShuffleMode shuffleMode;

- (id)init;
- (id)initWithCObject:(mUpnpDevice *)cobj;
- (BOOL)play;
- (BOOL)stop;
- (BOOL)pause;
- (BOOL)seek:(float)absTime;
- (BOOL)isPlaying;
/*
- (BOOL)start
- (BOOL)stop
*/
- (CGUpnpAVPositionInfo *)positionInfo;
- (CGUpnpService *)renderControlService;
- (CGUpnpService *)transportService;
- (CGUpnpService *)connectionManagerService;

/**
 * Wrap the render with necessary infomation for playing by jink2005@163.com
 */

- (void)playMusicWithIndex:(NSInteger)index;
- (DMRMediaItem *)itemAtIndex:(NSInteger)index;
- (void)skipToNextItem; //If already at the last item, will end playback.
- (void)skipToBeginning; // Restarts playback at the beginning of the currently playing media item.
- (void)skipToPreviousItem; // If already at the first item, this will end playback.

// These methods determine whether playback notifications will be generated.
// Calls to begin/endGeneratingPlaybackNotifications are nestable.
- (void)beginGeneratingPlaybackNotifications;
- (void)endGeneratingPlaybackNotifications;


@end
