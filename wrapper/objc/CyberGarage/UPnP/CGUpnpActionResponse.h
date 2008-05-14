//
//  CGUpnpActionResponse.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/13.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CSERVICE_H_)
typedef void CGUpnpActionResponse;
#endif

@interface CGUpnpActionResponse : NSObject 
{
	NSDictionary *arguments;
	int statusCode;
}
@property(readonly) NSDictionary *arguments;
@property(readonly) int statusCode;
- (id) initWithResult:(NSDictionary *)arguments statusCode:(int) statusCode;
@end
