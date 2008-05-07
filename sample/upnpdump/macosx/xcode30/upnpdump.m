#import <Foundation/Foundation.h>
#import <CyberGarage/UPnP/CGUpnpControlPoint.h>

int main (int argc, const char * argv[])
{
/*
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    NSLog(@"Hello, World!");
    [pool drain];
*/
	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];
	[ctrlPoint search];
	sleep(5);
	NSArray *devArray = [ctrlPoint getDeviceArray];
	int i;
	for (i=0; i < [devArray count]; i++) {
		printf("%d¥n", i);
	}
	return 0;
}
