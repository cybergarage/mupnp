//
//  CGUpnpAvContainer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <CGUpnpAvObject.h>

/**
 * The CGUpnpAvContainer class is a container of UPnP/AV container.
 */
@interface CGUpnpAvContainer : CGUpnpAvObject 
{
	NSMutableArray *childArray;
}
- (void)addChildObject:(CGUpnpAvObject *)obj;
- (void)removeChildObject:(CGUpnpAvObject *)obj;
- (NSArray *)children;
- (CGUpnpAvObject *)childforTitle:(NSString *)title;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)titlePath;
@end

