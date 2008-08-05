#import <Cocoa/Cocoa.h>

#import <CyberLink/UPnP.h>
#import <UPnPAV.h>

@interface DLNAController : NSObject {
CGUpnpAvController *dmc;
}
+(NSMutableString *)pathToColum:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column;
- (IBAction)searchDMS:(id)sender;
- (IBAction)itemSelected:(id)sender;
- (IBAction)doClick:(id)sender;
- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column;
- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(NSInteger)row column:(NSInteger)column;
- (BOOL)browser:(NSBrowser *)sender selectRow:(NSInteger)row inColumn:(NSInteger)column;
@end
