//
//  CGUpnpAvContainer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSXMLElement.h>

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
- (CGUpnpAvObject *)childAtIndex:(NSUInteger)anIndex;
- (CGUpnpAvObject *)childforId:(NSString *)anObjectId;
- (CGUpnpAvObject *)childforTitle:(NSString *)aTitle;
- (CGUpnpAvObject *)objectForId:(NSString *)anObjectId;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath;
@end

