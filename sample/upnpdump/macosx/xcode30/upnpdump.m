#import <Foundation/Foundation.h>
#import <CyberGarage/UPnP/CGUpnp.h>

int main (int argc, const char * argv[])
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];
	[ctrlPoint search];
	NSArray *devArray = [ctrlPoint getDeviceArray];
	for (CGUpnpDevice *dev in devArray) {
		NSLog(@"%@", [dev friendlyName]);
	}

	[pool drain];
	return 0;
}
