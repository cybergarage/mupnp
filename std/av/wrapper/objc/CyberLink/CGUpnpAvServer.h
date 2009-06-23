//
//  CGUpnpAvServer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

#import <CyberLink/UPnP.h>

@class CGUpnpAvContentDirectory;
@class CGUpnpAvObject;
@class CGUpnpAvContainer;

#if !defined(_CG_CLINKC_MEDIASERVER_H_)
typedef void CgUpnpMediaServer;
#endif

/**
 * The CGUpnpAvServer class is a UPnP/AV media server class.
 */
@interface CGUpnpAvServer : CGUpnpDevice
{
	CGUpnpAvContentDirectory *contentDirectory;
	CgUpnpMediaServer *cAvObject;
}
@property(readonly) CGUpnpAvContentDirectory *contentDirectory;
@property(readonly) CgUpnpMediaServer *cAvObject;
/**
 * Create a new UPnP/AV server.
 * 
 * @return New instance if successfull; otherwise nil.
 */
- (id)init;
/**
 * Create a new UPnP/AV server with the C instance.
 * 
 * @return New instance if successfull; otherwise nil.
 */
- (id)initWithCObject:(CgUpnpDevice *)cobj;
- (void)setUserObject:(id)aUserObj;
- (id)userObject;
- (NSArray *)browse:(NSString *)aObjectId browseFlag:(NSString *)aBrowseFlag;
- (NSArray *)browse:(NSString *)aObjectId browseFlag:(NSString *)aBrowseFlag withRequestedCount:(NSUInteger) aRequestedCount;
- (CGUpnpAvObject *)browseMetadata:(NSString *)aObjectId;
- (NSArray *)browseDirectChildren:(NSString *)aObjectId;
- (NSArray *)browseDirectChildren:(NSString *)aObjectId withRequestedCount:(NSUInteger) aRequestedCount;
- (NSArray *)search:(NSString *)aSearchCriteria;
- (CGUpnpAvContainer *)rootObject;
- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath;
- (BOOL)start;
- (BOOL)stop;
@end
