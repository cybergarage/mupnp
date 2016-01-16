/******************************************************************
*
* mUPnP for Objective-C
*
* UPnPBrowserAppDelegate.h
*
* Copyright (C) Satoshi Konno 2011
*
* This is licensed under BSD-style license, see file COPYING.
*
******************************************************************/

@interface UPnPBrowserAppDelegate : NSObject <UIApplicationDelegate> {

  UIWindow* window;
  UINavigationController* navigationController;
}

@property (nonatomic, retain) IBOutlet UIWindow* window;
@property (nonatomic, retain) IBOutlet UINavigationController* navigationController;

@end
