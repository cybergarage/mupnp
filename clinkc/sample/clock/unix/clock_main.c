/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: clock_main.c
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#include <cybergarage/upnp/cupnp.h>

#include "clock_device.h"

#if defined(TENGINE)
#if defined(PROCESS_BASE) /* Process based */
#include <basic.h>
#define MBEG	int main( W ac, TC *av[] )
#define MEND	0
#define DIR	"/SYS/bin/"
#else /* T-Kernel based */
#include <basic.h>
#include <tk/tkernel.h>
#include <sys/pinfo.h>
#define MBEG	ER main( INT ac, UB *av[] )
#define MEND	E_SYS
#define DIR	"/SYS/bin/"
#endif
#endif

CgUpnpDevice *clockDev;

////////////////////////////////////////////////////////// 
//  InitApp
////////////////////////////////////////////////////////// 

BOOL InitApp()
{
	clockDev = upnp_clock_device_new();
	cg_upnp_device_start(clockDev);
}

void ExitApp()
{
	cg_upnp_device_stop(clockDev);
	cg_upnp_device_delete(clockDev);
}

////////////////////////////////////////////////////////// 
//  main
////////////////////////////////////////////////////////// 

#if defined(TENGINE)
MBEG
#else
int main( int argc, char* argv[] )
#endif
{
	if (InitApp() == FALSE) {
		fprintf(stderr, "Couldn't create this clock device !!\n");
		exit(0);
	}
				
#if !defined(TENGINE)
	kbinit();
#endif
	int ch = 0;
	do
	{
#if !defined(TENGINE)
		if (0 < kbhit()) {
			ch = getchar();
			ch = toupper( ch );
		}
		else
			cg_wait(1000);
#else
		cg_wait(1000);
#endif
		
		upnp_clock_device_update(clockDev);
		
	} while( ch != 'Q');

#if !defined(TENGINE)
	kbexit();
#endif
	
	ExitApp();
	
	return(0);
}
