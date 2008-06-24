#import <Cocoa/Cocoa.h>

#import <CyberLink/UPnP.h>

#import <CGUpnpAvController.h>

@interface DLNAController : NSObject {
CGUpnpAvController *dmc;
}
- (IBAction)searchDMS:(id)sender;
- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column;
- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(NSInteger)row column:(NSInteger)column;
@end
