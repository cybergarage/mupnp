#import "DLNAController.h"

@implementation DLNAController

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
		
	dmc = [[CGUpnpAvController alloc] init];
	if (!dmc)
		return nil;
		
	return self;
}

- (void)dealloc
{
	[dmc release];
	[super dealloc];
}

- (void)finalize
{
	[dmc release];
	[super finalize];
}

+(NSMutableString *)pathToColum:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column
{
	[sender setPathSeparator:@"¥t"];
	NSString *pathToColum = [sender pathToColumn:column];
	NSArray *pathArray = [pathToColum componentsSeparatedByString:@"¥t"];
	NSMutableString *path = [NSMutableString string];
	for (NSString *pathStr in pathArray) {
		if ([pathStr isEqualToString:@"¥t"] || [pathStr length] <= 0)
			continue;
		[path appendString:@"/"];
		[path appendString:[CGXml escapestring:pathStr]];
	}
	return path;
}

- (IBAction)searchDMS:(id)sender 
{
	if (dmc)
		[dmc search];
	NSWindow *mainWin = [NSApp mainWindow];
	if (mainWin == nil)
		return;
	[mainWin update];
}

- (IBAction)itemSelected:(id)sender
{
}

- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column
{		
	if (column == 0) {
		NSArray *serverArray = [dmc servers];
		if ([serverArray count] <= 0) {
			[dmc search];
			serverArray = [dmc servers];
		}
		return [serverArray count];
	}

	NSString *path = [DLNAController pathToColum:sender numberOfRowsInColumn:column];

	//[NSCursor closedHandCursor];
	NSArray *avObjs = [dmc browseWithTitlePath:path];
	//[NSCursor arrowCursor];
	
	if (avObjs == nil)
		return 0;
		
	return [avObjs count];
}

- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(NSInteger)row column:(NSInteger)column
{
	if (column == 0) {
		NSArray *serverArray = [dmc servers];
		if (row < [serverArray count]) {
			CGUpnpAvServer *server = [serverArray objectAtIndex:row];
			[cell setTitle:[server friendlyName]];
			[cell setLeaf:NO];
		}
	}

	NSString *path = [DLNAController pathToColum:sender numberOfRowsInColumn:column];
	
	CGUpnpAvObject *avObj = [dmc objectForTitlePath:path];
	if (avObj == nil)
		return;
	if (![avObj isContainer])
		return;
	CGUpnpAvContainer *avCon = (CGUpnpAvContainer *)avObj;
	CGUpnpAvObject *childObj = [avCon childAtIndex:row];
	if (childObj == nil) {
		[cell setLeaf:YES];
		return;
	}
	[cell setTitle:[childObj title]];
	if (childObj.isContainer)
		[cell setLeaf:NO];
	else if (childObj.isItem)
		[cell setLeaf:YES];
}

/*
- (BOOL)browser:(NSBrowser *)sender selectCellWithString:(NSString *)title inColumn:(NSInteger)column
{
	NSMutableString *path = [DLNAController pathToColum:sender numberOfRowsInColumn:column];

	[path appendString:@"/"];
	[path appendString:[CGXml escapestring:title]];
	
	return YES;
}
*/

@end
