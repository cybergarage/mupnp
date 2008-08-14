#import <Cocoa/Cocoa.h>

#import <CyberLink/UPnP.h>
#import <UPnPAV.h>

@class DLNAImageView;

@interface DLNAController : NSObject {
IBOutlet NSBrowser *dmcBrowser;
CGUpnpAvController *dmc;
DLNAImageView *imageView;
NSView *browserView;
}
@property(readwrite,retain) DLNAImageView *imageView;
@property(readwrite,retain) NSView *browserView;
+(NSMutableString *)pathToColum:(NSBrowser *)browser numberOfRowsInColumn:(NSInteger)column;
+(NSMutableString *)selectedPath:(NSBrowser *)browser;
+(NSBrowserCell *)selectedCell:(NSBrowser *)browser;
+(NSMutableString *)selectedItemPath:(NSBrowser *)browser;
- (IBAction)searchDMS:(id)sender;
- (IBAction)itemSelected:(id)sender;
- (IBAction)doClick:(id)sender;
- (IBAction)doDoubleClick:(id)sender;
- (IBAction)doClickImage:(id)sender;
- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column;
- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(NSInteger)row column:(NSInteger)column;
@end
