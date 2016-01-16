/******************************************************************
*
* mUPnP for Objective-C
*
* UPnPDeviceTableViewCell.m
*
* Copyright (C) Satoshi Konno 2011
*
* This is licensed under BSD-style license, see file COPYING.
*
******************************************************************/

#import "UPnPDeviceViewController.h"
#import "UPnPDeviceTableViewCell.h"
#import "UPnPPresentationViewController.h"

#import <mUPnP/CGUpnp.h>

@implementation UPnPDeviceViewController

@synthesize upnpCtrlPoint;
@synthesize firstSearched;
@synthesize activityIndicator;
@synthesize upnpDevices;

- (id)initWithStyle:(UITableViewStyle)style 
{
	if (self = [super initWithStyle:style]) {
	}
	return self;
}

- (void)viewDidLoad
{
	[super viewDidLoad];	
	
	[self setUpnpCtrlPoint:[[[CGUpnpControlPoint alloc] init] autorelease]];
	[[self upnpCtrlPoint] start];
	 
	[[self navigationItem] setRightBarButtonItem:[[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(search:)]];	
	[self setUpnpDevices:nil];
	[self search:nil];
}

- (void) dealloc
{
	[[self upnpCtrlPoint] stop];
	[super dealloc];
}

- (void) finalize
{
	[[self upnpCtrlPoint] stop];
	[super finalize];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView 
{
	return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{	
	NSLog(@"numberOfRowsInSection = %d", section);
	NSLog(@"firstSearched = %d", [self firstSearched]);
	
	[self setUpnpDevices:[upnpCtrlPoint devices]];
	return [[self upnpDevices] count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
	NSLog(@"numberOfRowsInSection = %@", indexPath);

	static NSString *CELLID = @"upnprootobj";
	
	UPnPDeviceTableViewCell *cell = (UPnPDeviceTableViewCell *)[tableView dequeueReusableCellWithIdentifier:CELLID];
	if (cell == nil) {
		cell = [[[UPnPDeviceTableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CELLID] autorelease];
	}
	
	int row = [indexPath indexAtPosition:1];

	NSArray *deviceArray = [upnpCtrlPoint devices];
	if (row < [deviceArray count]) {
		CGUpnpDevice *device = [deviceArray objectAtIndex:row];
		/*
		UIImage *icon = [UIImage imageWithContentsOfFile:imagePath];
		cell.image = icon;
		*/
		[cell setDevice:device];
		//[cell setText:[server friendlyName]];
		//[cell setAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
	}

	return cell;
}

 - (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath 
{
	[tableView deselectRowAtIndexPath:indexPath animated:YES];

#if defined(CG_IMEDIASUITE_OBJECTVIEWCONTROLLER_USE_INDEXPATH)
	NSIndexPath *nextObjectIndexPath = [[NSIndexPath alloc] initWithIndex:[indexPath indexAtPosition:1]];
	[newController setObjectIndexPath:nextObjectIndexPath];
#endif

	int row = [indexPath indexAtPosition:1];
	CGUpnpDevice *device = [upnpDevices objectAtIndex:row];
	if (device == nil)
		return;

	NSLog(@"device = %@, %@", device, [device friendlyName]);
	
	UPnPPresentationViewController *newController = [[UPnPPresentationViewController alloc] init];
	[newController setDevice:device];
	[newController setTitle:[device friendlyName]];
	[[self navigationController] pushViewController:newController animated:YES];
	[newController release];
}

- (void)viewWillAppear:(BOOL)animated 
{
	[super viewWillAppear:animated];
	[[[self navigationController] navigationBar] setBarStyle:UIBarStyleBlackOpaque];
	
}

- (void)viewDidAppear:(BOOL)animated 
{
	[super viewDidAppear:animated];
	[[self view] setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
	[[self view] setAutoresizesSubviews:YES];
}

- (void)viewWillDisappear:(BOOL)animated 
{
}

- (void)viewDidDisappear:(BOOL)animated 
{
}

- (void)didReceiveMemoryWarning 
{
	[super didReceiveMemoryWarning]; 
}

- (void)search:(id)sender
{
	[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:YES];	
	[[self upnpCtrlPoint ] search];
	[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO];	
	[NSTimer scheduledTimerWithTimeInterval:((double)[upnpCtrlPoint ssdpSearchMX] * 1.5) target:self selector:@selector(searchUpdateTimer:) userInfo:nil repeats:NO];
}

- (void)searchUpdateTimer:(NSTimer *)timer 
{
	[[self tableView] reloadData];
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return NO;
}

@end
