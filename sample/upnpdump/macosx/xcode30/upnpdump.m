#import <Foundation/Foundation.h>
#import <CyberGarage/UPnP/CGUpnp.h>
#include <stdio.h>

/*
/////////////////////////////////////////////////////////////////////////////////
// Print Device
/////////////////////////////////////////////////////////////////////////////////

void PrintDeviceInfo(CgUpnpDevice *dev, int indent)
{
	char indentStr[128];
	int n;
	CgUpnpService *service;
	int serviceCnt;
	CgUpnpAction *action;
	int actionCnt;
	CgUpnpArgumentList *arg;
	int argCnt;
	CgUpnpStateVariable *stateVar;
	int stateVarCnt;
	
	for (n=0; n<indent && n<(sizeof(indentStr)-1); n++)
		indentStr[n] = ' ';
	indentStr[n] = '\0';
	
	serviceCnt = 0;
	for (service = cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		printf("%s service[%d] = %s\n", indentStr, ++serviceCnt, cg_upnp_service_getservicetype(service));
		actionCnt = 0;
		for (action = cg_upnp_service_getactions(service); action != NULL; action = cg_upnp_action_next(action)) {
			printf("%s  action[%d] = %s\n", indentStr, ++actionCnt, cg_upnp_action_getname(action));
			argCnt = 0;
			for (arg = cg_upnp_action_getarguments(action); arg != NULL; arg = cg_upnp_argument_next(arg)) {
				printf("%s   arg[%d] = %s\n", indentStr, ++argCnt, cg_upnp_argument_getname(arg));
			}
		}
		stateVarCnt = 0;
		for (stateVar = cg_upnp_service_getstatevariables(service); stateVar != NULL; stateVar = cg_upnp_statevariable_next(stateVar)) {
			printf("%s  stateVar[%d] = %s = %s\n", 
			       indentStr, 
			       ++stateVarCnt, 
			       cg_upnp_statevariable_getname(stateVar),
			       cg_upnp_statevariable_getvalue(stateVar));
		}
	}
}
*/

void PrintDevice(CGUpnpDevice *dev) 
{
	static int devNum = 0;
	
	NSLog(@"[%d] %@", devNum, [dev friendlyName]);
	NSLog(@"  %@", [dev deviceType]);

	int serviceNum = 0;
	NSArray *serviceArray = [dev services];
	for (CGUpnpService *service in serviceArray) {
		NSLog(@"    [%d] %@", serviceNum, [service serviceType]);
		serviceNum++;
	}
	
	devNum++;
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
