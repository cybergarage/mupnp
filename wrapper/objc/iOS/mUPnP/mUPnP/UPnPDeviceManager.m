//
//  UPnPDeviceManager.m
//  UpnpPlaylist
//
//  Created by Ares on 11/9/15.
//  Copyright © 2015 Ares. All rights reserved.
//

#import "UPnPDeviceManager.h"
#import "CGUpnpAvController.h"
#import "CGUpnpAvRenderer.h"
#import "CGUpnpAvServer.h"

@interface UPnPDeviceManager() <CGUpnpAvControllerDelegate>
{
    CGUpnpAvController *avControlPoint;
    NSMutableArray *observerArray;
    NSMutableArray *mediaRendererArray;
    NSMutableArray *mediaServerArray;
}

@end


UPnPDeviceManager *gUPnPDeviceManager;

@implementation UPnPDeviceManager

+ (UPnPDeviceManager *)sharedInstance {
    if (nil == gUPnPDeviceManager) {
        gUPnPDeviceManager = [[UPnPDeviceManager alloc] init];
    }
    return gUPnPDeviceManager;
}

- (id)init {
    self = [super init];
    if (nil != self) {
        mediaRendererArray = [NSMutableArray array];
        mediaServerArray = [NSMutableArray array];
        observerArray = [NSMutableArray array];
        
        avControlPoint = [[CGUpnpAvController alloc] init];
        [avControlPoint setDelegate:self];
        [avControlPoint start];
    }
    return self;
}

- (void)dealloc {
    [avControlPoint stop];
    avControlPoint = nil;
}

#pragma property rewirte

- (void)setCurrentAvRenderer:(CGUpnpAvRenderer *)currentAvRenderer {
    if (_currentAvRenderer == currentAvRenderer) {
        return;
    } else {
        [_currentAvRenderer endGeneratingPlaybackNotifications];
    }
    _currentAvRenderer = currentAvRenderer;
    [_currentAvRenderer beginGeneratingPlaybackNotifications];
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidChangedCurrentAvRenderer:)]) {
            [observer deviceManagerDidChangedCurrentAvRenderer:currentAvRenderer];
        }
    }
}

#pragma mark Help Function

- (void)registerAsObserver:(id<UPnPDeviceManagerObserver>)observer {
    if (nil != observer && ! [observerArray containsObject:observer]) {
        [observerArray addObject:observer];
    }
}

- (void)unregisterObserver:(id<UPnPDeviceManagerObserver>)observer {
    [observerArray removeObject:observer];
}

#pragma mark API

- (NSArray *)mediaRendererArray {
    NSArray *devices = [avControlPoint renderers];
    return devices;
}

- (NSArray *)mediaServerArray {
    NSArray *devices = [avControlPoint servers];
    return devices;
}

- (void)searchMediaRenderers {
    [avControlPoint searchWithST:[NSString stringWithUTF8String:CG_UPNPAV_DMR_DEVICE_TYPE]];
}

- (void)searchMediaServers {
    [avControlPoint searchWithST:[NSString stringWithUTF8String:CG_UPNPAV_DMS_DEVICE_TYPE]];
}

#pragma mark CGUpnpAvControllerDelegate

- (void)controlPoint:(CGUpnpControlPoint *)controlPoint deviceAdded:(NSString *)deviceUdn {
    NSLog(@"%@ added.", deviceUdn);
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidDeviceArrayUpdated)]) {
            [observer deviceManagerDidDeviceArrayUpdated];
        }
    }
}

- (void)controlPoint:(CGUpnpControlPoint *)controlPoint deviceUpdated:(NSString *)deviceUdn {
    NSLog(@"%@ updated.", deviceUdn);
    
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidDeviceArrayUpdated)]) {
            [observer deviceManagerDidDeviceArrayUpdated];
        }
    }
}

- (void)controlPoint:(CGUpnpControlPoint *)controlPoint deviceRemoved:(NSString *)deviceUdn {
    NSLog(@"%@ removed.", deviceUdn);
    
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidDeviceArrayUpdated)]) {
            [observer deviceManagerDidDeviceArrayUpdated];
        }
    }
}

- (void)controlPoint:(CGUpnpControlPoint *)controlPoint deviceInvalid:(NSString *)deviceUdn {
    NSLog(@"%@ invalid.", deviceUdn);
   
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidDeviceArrayUpdated)]) {
            [observer deviceManagerDidDeviceArrayUpdated];
        }
    }
}

- (void)upnpAvController:(CGUpnpAvController *)controller didRendererPositionInfoUpdated:(CGUpnpAvRenderer *)renderer {
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidRendererPositionInfoUpdated:)]) {
            [observer deviceManagerDidRendererPositionInfoUpdated:renderer];
        }
    }
}

- (void)upnpAvController:(CGUpnpAvController *)controller didRendererPlaybackStateUpdated:(CGUpnpAvRenderer *)renderer {
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidRendererPlaybackStateUpdated:)]) {
            [observer deviceManagerDidRendererPlaybackStateUpdated:renderer];
        }
    }
}

- (void)upnpAvController:(CGUpnpAvController *)controller didRendererTrackURIUpdated:(CGUpnpAvRenderer *)renderer {
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidRendererTrackURIUpdated:)]) {
            [observer deviceManagerDidRendererTrackURIUpdated:renderer];
        }
    }
}

- (void)upnpAvController:(CGUpnpAvController *)controller didRendererTrackDurationUpdated:(CGUpnpAvRenderer *)renderer {
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidRendererTrackDurationUpdated:)]) {
            [observer deviceManagerDidRendererTrackDurationUpdated:renderer];
        }
    }
}

- (void)upnpAvController:(CGUpnpAvController *)controller didRendererVolumeUpdated:(CGUpnpAvRenderer *)renderer {
    for (id<UPnPDeviceManagerObserver> observer in observerArray) {
        if ([observer respondsToSelector:@selector(deviceManagerDidRendererVolumeUpdated:)]) {
            [observer deviceManagerDidRendererVolumeUpdated:renderer];
        }
    }
}

@end
