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

- (IBAction)searchDMS:(id)sender 
{
	if (dmc)
		[dmc search];
}

- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column
{
	if (column == 0)
		return 2;
	NSString *path = [sender pathToColumn:column];
	NSLog(@"%@", path);
	return 1;
}

- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(NSInteger)row column:(NSInteger)column
{
	[cell setTitle:@"test"];
	[cell setLeaf:NO];
}

@end
