#import <Cocoa/Cocoa.h>

#import <CyberLink/UPnP.h>
#import <UPnPAV.h>

@interface DLNAController : NSObject {
IBOutlet NSBrowser *dmcBrowser;
CGUpnpAvController *dmc;
}
+(NSMutableString *)pathToColum:(NSBrowser *)browser numberOfRowsInColumn:(NSInteger)column;
+(NSMutableString *)selectedPath:(NSBrowser *)browser;
+(NSBrowserCell *)selectedCell:(NSBrowser *)browser;
+(NSMutableString *)selectedItemPath:(NSBrowser *)browser;
- (IBAction)searchDMS:(id)sender;
- (IBAction)itemSelected:(id)sender;
- (IBAction)doClick:(id)sender;
- (IBAction)doDoubleClick:(id)sender;
- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column;
- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(NSInteger)row column:(NSInteger)column;
@end
