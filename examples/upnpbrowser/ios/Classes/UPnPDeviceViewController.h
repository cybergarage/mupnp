/******************************************************************
*
* mUPnP for Objective-C
*
* UPnPDeviceTableViewCell.h
*
* Copyright (C) Satoshi Konno 2011
*
* This is licensed under BSD-style license, see file COPYING.
*
******************************************************************/

#import <mUPnP/CGUpnp.h>

@interface UPnPDeviceViewController : UITableViewController {
  CGUpnpControlPoint* upnpCtrlPoint;
  BOOL firstSearched;
  UIActivityIndicatorView* activityIndicator;
  NSArray* upnpDevices;
}
@property (retain) CGUpnpControlPoint* upnpCtrlPoint;
@property (readwrite) BOOL firstSearched;
@property (retain) UIActivityIndicatorView* activityIndicator;
@property (retain) NSArray* upnpDevices;
- (id)initWithStyle:(UITableViewStyle)style;
- (void)search:(id)sender;
@end
