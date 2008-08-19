//
//  CGUpnpAvContainer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#if defined(__MACOSX__)
#import <Cocoa/Cocoa.h>
#else
#import <UIKit/UIKit.h>
#endif

@class CGUpnpAvObject;

/**
 * The CGUpnpAvContainer class is a container of UPnP/AV container.
 */
@interface CGUpnpAvContainer : CGUpnpAvObject
{
	NSMutableArray *childArray;
}
- (id)init;
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
- (void)addChild:(CGUpnpAvObject *)obj;
- (void)addChildren:(NSArray *)objArray;
- (void)removeChild:(CGUpnpAvObject *)obj;
- (void)removeAllChildren;
- (NSArray *)children;
- (CGUpnpAvObject *)childAtIndex:(NSUInteger)index;
- (CGUpnpAvObject *)childforId:(NSString *)aObjectId;
- (CGUpnpAvObject *)childforTitle:(NSString *)aTitle;
- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath;
@end

