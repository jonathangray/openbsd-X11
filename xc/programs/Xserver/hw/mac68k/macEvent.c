#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "macbsd.h"


void macbsd_enqevents(
	void)
{
	int len, i;
	adb_event_t e[128];
	char str[100];

	while(1){
		len = read (mac_adbfd, e, sizeof (e));

		if(len == -1){
			ErrorF("Failure to read from ADB device\n");
			FatalError(sys_errlist[errno]);
		}

		if(len == 0)
			break;

		len /= sizeof (e[0]);

		for (i = 0; i < len; i++) {
			if (e[i].def_addr == ADBADDR_MS) {
				macbsd_processmouse(macbsd_mouse, &e[i]);
			}else if (e[i].def_addr == ADBADDR_KBD) {
				macbsd_processkbd(macbsd_kbd, &e[i]);
			}
		}
	}
}


void ProcessInputEvents (
	void)
{
	mieqProcessInputEvents ();
	miPointerUpdate ();
}
