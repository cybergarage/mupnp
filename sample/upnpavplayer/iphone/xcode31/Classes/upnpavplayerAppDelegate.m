//
//  upnpavplayerAppDelegate.m
//  upnpavplayer
//
//  Created by Satoshi Konno on 08/06/18.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "upnpavplayerAppDelegate.h"
#import "RootViewController.h"


@implementation upnpavplayerAppDelegate

@synthesize window;
@synthesize navigationController;


- (id)init {
	if (self = [super init]) {
		// 
	}
	return self;
}


- (void)applicationDidFinishLaunching:(UIApplication *)application {
	
	// Configure and show the window
	[window addSubview:[navigationController view]];
	[window makeKeyAndVisible];
}


- (void)applicationWillTerminate:(UIApplication *)application {
	// Save data if appropriate
}


- (void)dealloc {
	[navigationController release];
	[window release];
	[super dealloc];
}

@end
