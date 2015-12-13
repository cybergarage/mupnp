//
//  CGUpnpAvEvent.h
//  mUPnP
//
//  Created by Ares on 12/12/15.
//  Copyright © 2015 Ares. All rights reserved.
//

#ifndef _MUPNP_PROPERTY_H_
    typedef void mUpnpProperty;
#endif

typedef enum : NSUInteger {
    EVENT_UNKNOWN,
    // AV Transport State
    EVENT_AVT_NO_MEDIA_PRESENT,
    EVENT_AVT_TRANSITIONING,
    EVENT_AVT_PLAYING,
    EVENT_AVT_STOPPED,
    EVENT_AVT_DURATION,
    // AV Renderer Control State
    EVENT_RCS_VOLUME,
} EventType;

@interface CGUpnpAvEvent : NSObject

@property (nonatomic, retain) NSString *eventName;
@property (nonatomic, retain) NSString *serviceID;
@property (nonatomic, assign) EventType eventType;
@property (nonatomic, assign) NSInteger duration;
@property (nonatomic, assign) NSInteger volume;
@property (nonatomic, retain) NSString *trackURI;

- (id)initWithEventProperty:(mUpnpProperty *)property;

@end
