//
//  CGXml.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/29.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "CGXml.h"

@implementation CGXml

#warning TODO Need check the line commented.

+ (NSString *)escapestring:(NSString *)aString
{
//    return [[aString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding] stringByReplacingOccurrencesOfString:@"/" withString:@"%2F"];
    return [aString stringByReplacingOccurrencesOfString:@"/" withString:@"%2F"];
}

+ (NSString *)escapestringFromIndex:(NSString *)aString index:(NSUInteger)anIndex
{
//	return [[[aString substringFromIndex:anIndex] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding] stringByReplacingOccurrencesOfString:@"/" withString:@"%2F"];
    return [aString stringByReplacingOccurrencesOfString:@"/" withString:@"%2F"];
}

+ (NSString *)unescapestring:(NSString *)aString;
{
//	return [aString stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    return aString;
}

@end

