//
//  UPnPDeviceManager.h
//  UpnpPlaylist
//
//  Created by Ares on 11/9/15.
//  Copyright © 2015 Ares. All rights reserved.
//

#import <Foundation/Foundation.h>

@class CGUpnpAvRenderer;


@protocol UPnPDeviceManagerObserver;


@interface UPnPDeviceManager : NSObject

+ (UPnPDeviceManager *)sharedInstance;

@property (nonatomic, weak) CGUpnpAvRenderer *currentAvRenderer;

- (NSArray *)mediaRendererArray;
- (NSArray *)mediaServerArray;
- (void)searchMediaRenderers;
- (void)searchMediaServers;

- (void)registerAsObserver:(id<UPnPDeviceManagerObserver>)observer;
- (void)unregisterObserver:(id<UPnPDeviceManagerObserver>)observer;

@end


@protocol UPnPDeviceManagerObserver <NSObject>

@optional
- (void)deviceManagerDidDeviceArrayUpdated;
- (void)deviceManagerDidChangedCurrentAvRenderer:(CGUpnpAvRenderer *)avRenderer;
- (void)deviceManagerDidRendererVolumeUpdated:(CGUpnpAvRenderer *)avRenderer;
- (void)deviceManagerDidRendererPlaybackStateUpdated:(CGUpnpAvRenderer *)avRenderer;
- (void)deviceManagerDidRendererTrackDurationUpdated:(CGUpnpAvRenderer *)avRenderer;
- (void)deviceManagerDidRendererTrackURIUpdated:(CGUpnpAvRenderer *)avRenderer;
- (void)deviceManagerDidRendererPositionInfoUpdated:(CGUpnpAvRenderer *)avRenderer;

@end