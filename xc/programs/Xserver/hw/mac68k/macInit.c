#include <servermd.h>
#include "macbsd.h"
#include "dixstruct.h"
#include "dix.h"
#include "opaque.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>


#define CATCH_BROKEN_MODE 1


void ProcessInputEvents(void);


int mac_adbfd;

Time mac_lasteventtime;

int mac_scrs = 0;
fbinfo_t mac_fbs[MAXSCREENS];

DevicePtr macbsd_mouse;
DevicePtr macbsd_kbd;

int macbsd_mouseproc(
	DevicePtr mouse,
	int what);

int macbsd_kbdproc(
	DevicePtr mouse,
	int what);

Bool macbsdMonoInit(
	int	index,
	ScreenPtr	screen,
	int	argc,
	char	**argv);


void macbsd_enqevents(
	void);


static void handle_sigio(
	int flags)
{
	macbsd_enqevents();
}

int initialize_desktop()
{
	int				i, j;
	struct imagedata		*id;
	int				totalscreens;
	int				argnum = 1;

	mac_adbfd = open ("/dev/adb", O_RDWR);

	if (mac_adbfd == -1) {
		ErrorF("Couldn't open /dev/adb...");
		FatalError(sys_errlist[errno]);
	}

	macbsd_getmouse();	/* find mouse and set up or error */
	macbsd_getkbd();	/* find keyboard and set up or error */

	signal(SIGIO, handle_sigio);
}


choose_best_depths()
{
#if 0
	int scr, mode, best;
	struct imagedata *si;
	int umph, bestumph;	/* "umph" is goodness of screen */
		/* Could have a nicer calculation, like depth versus */
		/* resolution versus dimensions with weights... */
	char scrstr[100];

	for(scr = 0; scr < mac_ci.numscreens; scr++){
		best = -1;
		if(mac_si[scr].nummodes > 0){
			best = 0;
			si = &mac_si[scr].modes[0].modeinfo;
			bestumph = si->pixelSize * (si->right - si->left) *
				(si->bottom - si->top);
	
			for(mode = 1; mode < mac_si[scr].nummodes; mode++){
				si = &mac_si[scr].modes[mode].modeinfo;
				umph = si->pixelSize * (si->right - si->left) *
					(si->bottom - si->top);
				if(umph > bestumph){
					bestumph = umph;
					best = mode;
				}
			}
		}else{
			sprintf(scrstr, "Cannot find modes for screen %d."
				"  Ignoring.", scr);
			ErrorF(scrstr);
		}
		mac_bestmode[scr] = best;
	}
#endif
}


Bool macbsd_screeninit(
	ScreenPtr screen)
{
	miDCInitialize(screen, &mac_mousefuncs);

	return TRUE;
}



parse_args(
	int argc,
	char **argv)
{
	/* try to match argv params to screens and depths */
	/* BARF I really should do this, but not right now, */
	/* while I am feverishly trying to finish the other X code. */
}


setup_screens(
	ScreenInfo *xsi,
	int argc,
	char **argv)
{
	char	fname[128];
	int	done = 0;
	unsigned char	*vaddr;
	int	scr;
	
	do {
		sprintf(fname, "/dev/grf%d", mac_scrs);
#if DEBUG
		ErrorF("attempting %s...\n", fname);
#endif
		if ((mac_fbs[mac_scrs].fd = open(fname, O_RDWR)) != -1) {
#if DEBUG
			ErrorF("opened successfully\n");
#endif
			if (ioctl (mac_fbs[mac_scrs].fd, GRFIOCGMODE,
				&mac_fbs[mac_scrs].idata) != 0)
			{
				ErrorF("Couldn't get info for %d.\n",mac_scrs);
				FatalError(sys_errlist[errno]);
			}
			vaddr = mmap(0, mac_fbs[mac_scrs].idata.fbsize,
				(PROT_READ|PROT_WRITE),MAP_SHARED,
				mac_fbs[mac_scrs].fd,
				mac_fbs[mac_scrs].idata.fboff);
			if (vaddr == (caddr_t) -1) {
				ErrorF("Couldn't map SCREEN %d.\n", mac_scrs);
				FatalError(sys_errlist[errno]);
			}
			mac_fbs[mac_scrs].modenum = 0;
			mac_fbs[mac_scrs].vaddr = vaddr;
			mac_fbs[mac_scrs].added = FALSE;
			mac_scrs++;
		} else {
			if (errno != ENXIO) {
				ErrorF("Couldn't open %s.\n", fname);
				FatalError(sys_errlist[errno]);
			}
			done = 1;
		}
	} while(!done);

	if(mac_scrs == 0)
		FatalError("Can't run X server with no screens!");

	xsi->imageByteOrder = IMAGE_BYTE_ORDER;
	xsi->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
	xsi->bitmapScanlinePad = BITMAP_SCANLINE_PAD;
	xsi->bitmapBitOrder = BITMAP_BIT_ORDER;

	xsi->formats[0].depth = 1;
	xsi->formats[0].bitsPerPixel = 1;
	xsi->formats[0].scanlinePad = BITMAP_SCANLINE_PAD;

	xsi->formats[1].depth = 8;
	xsi->formats[1].bitsPerPixel = 8;
	xsi->formats[1].scanlinePad = BITMAP_SCANLINE_PAD;

	xsi->formats[2].depth = 16;
	xsi->formats[2].bitsPerPixel = 16;
	xsi->formats[2].scanlinePad = BITMAP_SCANLINE_PAD;

	xsi->formats[3].depth = 24;
	xsi->formats[3].bitsPerPixel = 32;
	xsi->formats[3].scanlinePad = BITMAP_SCANLINE_PAD;

	xsi->numPixmapFormats = 4;

#if DEBUG
	ErrorF("# of screens: %d\n", mac_scrs);
#endif
	for(scr = 0; scr < mac_scrs; scr++){
#if DEBUG
		ErrorF("adding screen %d\n", scr);
#endif
#if CATCH_BROKEN_MODE
		if(mac_fbs[scr].idata.psize == 0)
			mac_fbs[scr].idata.psize = 1;
#endif /* CATCH_BROKEN_MODE */
		if(mac_fbs[scr].idata.psize == 1)
			AddScreen(macbsdMonoInit, argc, argv);
		else
#if COLOR_SUPPORT	/* Damn, I'm lazy! */
			AddScreen(macbsdColorInit, argc, argv);
#else
			FatalError("I don't support color screens!\n");
#endif
	}

	xsi->numScreens = scr;
}


void InitOutput(
	ScreenInfo *xsi,
	int argc,
	char **argv)
{
	static int dejavu = 0;

	if(dejavu)
		return;
	else
		dejavu = 1;

	/* set up all screens */
	initialize_desktop();
	choose_best_depths();
	parse_args(argc, argv);
	setup_screens(xsi, argc, argv);
}


void InitInput(
	int argc,
	char **argv)
{
	DevicePtr mouse;
	DevicePtr kbd;
	static int dejavu = 0;

	if(dejavu)
		return;
	else
		dejavu = 1;

	macbsd_mouse = mouse = AddInputDevice((DeviceProc)macbsd_mouseproc,
		TRUE);
	RegisterPointerDevice(mouse);
	miRegisterPointerDevice(screenInfo.screens[0], mouse);

	macbsd_kbd = kbd = AddInputDevice((DeviceProc)macbsd_kbdproc, TRUE);
	RegisterKeyboardDevice(kbd);

	if (!mieqInit (kbd, mouse))
		FatalError ("could not initialize event queue");

	mac_lasteventtime = time(0) * 1000;
}


void AbortDDX(void)
{
	close(mac_adbfd);
}


void ddxGiveUp(void)
{
}


int ddxProcessArgument(
	int argc,
	char **argv,
	int i)
{
	if(strcmp(argv[i], "-screen") == 0)
		return(3);
	return(0);
}


void ddxUseMsg(void)
{
	ErrorF("-screen S D\t\t\tuse depth <D> for screen <S>");
	ErrorF("\t(use the 'console' program for screen information)");
}


void MessageF(
	char *s)
{
	ErrorF(s);
}
