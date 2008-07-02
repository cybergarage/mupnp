//
//  CGUpnpAvServer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <CGUpnpAvContentDirectory.h>

/**
 * The CGUpnpAvServer class is a UPnP/AV media server class.
 */
@interface CGUpnpAvServer : NSObject
{
	CGUpnpAvContentDirectory *contentDirectory;
}
@property(readonly) CGUpnpAvContentDirectory *contentDirectory;
@end
