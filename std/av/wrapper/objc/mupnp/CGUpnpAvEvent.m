//
//  CGUpnpAvEvent.m
//  mUPnP
//
//  Created by Ares on 12/12/15.
//  Copyright © 2015 Ares. All rights reserved.
//

#include <mupnp/event/property.h>
#include <mupnp/xml/xml.h>
#include <mupnp/std/av/cupnpav.h>
#import <mupnp/CGXmlNode.h>

#import "CGUpnpAvEvent.h"

#define XMLNS_RCS @"/RCS"
#define XMLNS_AVT @"/AVT"

@interface CGUpnpAvEvent() {
    CGXmlNode *eventRoot;
}

@end


@implementation CGUpnpAvEvent

- (id)initWithEventProperty:(mUpnpProperty *)property
{
    self = [super init];
    if (nil != self)
    {
        _serviceID = [NSString stringWithCString:property->sid->value encoding:NSUTF8StringEncoding];
        _eventName = [NSString stringWithCString:property->name->value encoding:NSUTF8StringEncoding];
        
        char *resultXml;
        mUpnpXmlParser *xmlParser;
        mUpnpXmlNodeList *rootNode;
        mUpnpXmlNode *eventNode;
        
        resultXml = property->value->value;
        if (mupnp_strlen(resultXml) <= 0)
            return nil;
        
        rootNode = mupnp_xml_nodelist_new();
        xmlParser = mupnp_xml_parser_new();
        if (mupnp_xml_parse(xmlParser, rootNode, resultXml, mupnp_strlen(resultXml))) {
            eventNode = mupnp_xml_nodelist_getbyname(rootNode, "Event");
            if (eventNode) {
                eventRoot = [[CGXmlNode alloc] initWithXMLNode:eventNode];
            }
        }
        mupnp_xml_nodelist_delete(rootNode);
        mupnp_xml_parser_delete(xmlParser);
        
        [self fillAvEventProperty];
    }
    
    return self;
}

- (void)fillAvEventProperty
{
    CGXmlNode *instanceNote = [eventRoot elementForName:@"InstanceID"];
    if (nil == instanceNote)
    {
        return;
    }
    
    NSString *xmlnsValue = [eventRoot attributeValueForName:@"xmlns"];
    if ([xmlnsValue containsString:XMLNS_AVT])
    {
        CGXmlNode *transportStateNote = [instanceNote elementForName:@"TransportState"];
        CGXmlNode *trackURINote = [instanceNote elementForName:@"CurrentTrackURI"];
        CGXmlNode *trackDurationNote = [instanceNote elementForName:@"CurrentTrackDuration"];
        
        if (nil != transportStateNote)
        {
            NSString *stateValue = [transportStateNote attributeValueForName:@"val"];
            if ([stateValue isEqualToString:@"NO_MEDIA_PRESENT"])
            {
                _eventType = EVENT_AVT_NO_MEDIA_PRESENT;
            }
            else if ([stateValue isEqualToString:@"TRANSITIONING"])
            {
                _eventType = EVENT_AVT_TRANSITIONING;
            }
            else if ([stateValue isEqualToString:@"PLAYING"])
            {
                _eventType = EVENT_AVT_PLAYING;
            }
            else if ([stateValue isEqualToString:@"STOPPED"])
            {
                _eventType = EVENT_AVT_STOPPED;
            }
        }
        else if (nil != trackURINote)
        {
            _trackURI = [trackURINote attributeValueForName:@"val"];
        }
        else if (nil != trackDurationNote)
        {
            NSString *durationValue = [trackDurationNote attributeValueForName:@"val"];
            NSArray *tempArray = [durationValue componentsSeparatedByString:@":"];
            _duration = [tempArray[2] integerValue] + [tempArray[1] integerValue] * 60 + [tempArray[0] integerValue] * 60 * 60;
        }
    }
    else if ([xmlnsValue containsString:XMLNS_RCS])
    {
        CGXmlNode *volumeNote = [instanceNote elementForName:@"Volume"];
        if (nil != volumeNote)
        {
            NSString *value = [volumeNote attributeValueForName:@"val"];
            _volume = [value integerValue];
        }
    }
}

@end
