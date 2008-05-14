#import <Foundation/Foundation.h>
#import <CyberGarage/UPnP/CGUpnp.h>
#include <stdio.h>

void PrintDevice(CGUpnpDevice *dev) 
{
	static int devNum = 0;
	int serviceNum = 0;
	int actionNum = 0;
	int argNum = 0;
	int statVarNum = 0;
	
	NSLog(@"[%d] %@", ++devNum, [dev friendlyName]);
	NSLog(@"  deviceType = %@", [dev deviceType]);

	NSArray *serviceArray = [dev services];
	for (CGUpnpService *service in serviceArray) {
		NSLog(@"    [%d] service = %@", ++serviceNum, [service serviceType]);

		NSArray *actionArray = [service actions];
		actionNum = 0;
		for (CGUpnpAction *action in actionArray) {
			//NSLog(@"      [%d] action = %@", ++actionNum, [action name]);
			NSDictionary *argDir = [action arguments];
			argNum = 0;
			for (NSString *name in argDir) {
				//NSLog(@"        [%d] argument = %@", ++argNum, name);
			}
		}

		NSDictionary *statVarDir = [service stateVariables];
		statVarNum = 0;
		for (NSString *name in statVarDir) {
			//NSLog(@"      [%d] stateVariable = %@", ++statVarNum, name);
		}
	}
}

int main (int argc, const char * argv[])
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];
	[ctrlPoint search];
	NSArray *devArray = [ctrlPoint devices];
	for (CGUpnpDevice *dev in devArray)
		PrintDevice(dev);

	[pool drain];
	return 0;
}
