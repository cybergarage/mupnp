//
//  CGXml.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/29.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface CGXml : NSObject 
{
}
+ (NSString *)escapestring:(NSString *)aString;
+ (NSString *)unescapestring:(NSString *)aString;
@end

