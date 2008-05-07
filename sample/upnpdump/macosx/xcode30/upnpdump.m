#import <Foundation/Foundation.h>
#import <CyberGarage/UPnP/CGUpnp.h>

int main (int argc, const char * argv[])
{
	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];
	[ctrlPoint search];
	sleep(5);
	NSArray *devArray = [ctrlPoint getDeviceArray];
	for (CGUpnpDevice *dev in devArray) {
		NSLog(@"%@", [dev friendlyName]);
	}
	return 0;
}
