#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "macbsd.h"
#include "Xproto.h"
#include "inputstr.h"


extern CARD8 *macIIModMap[];		/* Borrowed from A/UX code */
extern KeySymsRec macIIKeySyms[];	/* Borrowed from A/UX code */


void macbsd_bell()
{
	printf("");
	fflush(stdout);
}


void macbsd_kbdctrl()
{
}


int macbsd_kbdproc(
	DevicePtr kbd,
	int what)
{
	switch(what){
		case DEVICE_INIT:
			if(kbd != LookupKeyboardDevice()){
				ErrorF("Kbd routines can only handle DESKTOP"
					" keyboards.\n");
				return(!Success);
			}

			InitKeyboardDeviceStruct(kbd, &macIIKeySyms[0],
				macIIModMap[1], macbsd_bell, macbsd_kbdctrl);

			kbd->on = FALSE;
			break;

		case DEVICE_ON:
			kbd->on = TRUE;
			break;

		case DEVICE_OFF:
			kbd->on = FALSE;
			break;

		case DEVICE_CLOSE:
			/* nothing! */
			break;
	}

	return(Success);
}


void macbsd_getkbd(
	void)
{
	int index;

	/* Find out where there is a keyboard */

	/* FatalError("Cannot run X server without a mouse.\n"); */
}


void macbsd_processkbd(
	DevicePtr	kbd,
	adb_event_t	*event)
{
	xEvent	xev;
	int	d;

	mac_lasteventtime = xev.u.keyButtonPointer.time =
		TVTOMILLI(event->timestamp);

	xev.u.u.type = (event->u.k.key & 0x80) ? KeyRelease : KeyPress;
	d = xev.u.u.detail = (event->u.k.key & 0x7f);
	if (d < 8) {
		switch (d) {
		default:
		case 0: xev.u.u.detail = 0x0a; break;
		case 1: xev.u.u.detail = 0x34; break;
		case 2: xev.u.u.detail = 0x3f; break;
		case 3: xev.u.u.detail = 0x40; break;
		case 4: xev.u.u.detail = 0x42; break;
		case 5: xev.u.u.detail = 0x44; break;
		case 6: xev.u.u.detail = 0x46; break;
		case 7: xev.u.u.detail = 0x48; break;
		}
	}

	mieqEnqueue(&xev);
}


Bool LegalModifier(
	unsigned int key,
	DevicePtr dev)
{
	return(TRUE);
}
