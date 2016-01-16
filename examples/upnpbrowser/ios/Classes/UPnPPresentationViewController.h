/******************************************************************
*
* mUPnP for Objective-C
*
* UPnPPresentationViewController.h
*
* Copyright (C) Satoshi Konno 2011
*
* This is licensed under BSD-style license, see file COPYING.
*
******************************************************************/

#import <UIKit/UIKit.h>
#import <iAd/ADBannerView.h>
#import <mUPnP/CGUpnp.h>

@interface UPnPPresentationViewController : UIViewController <UIWebViewDelegate> {
}
@property (retain) UIWebView* webView;
@property (retain) ADBannerView* adView;
@property (retain) CGUpnpDevice* device;
@property (retain) NSString* presentationURL;
@end
