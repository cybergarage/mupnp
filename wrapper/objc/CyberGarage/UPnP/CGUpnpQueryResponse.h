//
//  CGUpnpQueryResponse.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/13.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#if !defined(_CG_UPNP_CSERVICE_H_)
typedef void CGUpnpQueryResponse;
#endif

@interface CGUpnpQueryResponse : NSObject 
{
	NSString *value;
	int	statusCode;
}
@property(readonly) NSString *value;
@property(readonly) int statusCode;
- (id) initWithResult:(NSString *)value statusCode:(int) statusCode;
@end
