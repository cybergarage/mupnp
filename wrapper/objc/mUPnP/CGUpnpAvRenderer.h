/* Hosted renderer service lifecycle. BSD-style license; see COPYING. */
#import <Foundation/Foundation.h>

typedef struct CgUpnpAvRenderer CgUpnpAvRenderer;

/** Thin owner of cg_upnpav_dmr_* (not the legacy remote-control AV wrapper).
 * init is inert; start explicitly publishes services on normal mUPnP interfaces.
 * No playback or event callback backend is provided. Keep compatibility use
 * opt-in, start immediately before the intended discovery, and stop afterward
 * or when the app leaves the foreground. Never enable a background advertiser.
 * Serialize lifecycle on the controller's existing queue, away from UI work. */
@interface CGUpnpAvRenderer : NSObject {
  @private
  CgUpnpAvRenderer* _cRenderer;
}
- (instancetype)init;
- (instancetype)initWithFriendlyName:(NSString*)friendlyName;
@property (nonatomic, readonly) CgUpnpAvRenderer* cRenderer; // borrowed; never delete externally
@property (nonatomic, readonly, getter=isRunning) BOOL running;
@property (nonatomic, readonly, copy) NSString* friendlyName;
@property (nonatomic, readonly, copy) NSString* udn;
- (BOOL)start;
- (BOOL)stop;
@end
