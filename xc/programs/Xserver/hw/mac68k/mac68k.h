#define NEED_EVENTS

#include "X.h"
#include "misc.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "screenint.h"
#include "inputstr.h"
#include "input.h"
#include "cursorstr.h"
#include "cursor.h"
#include "pixmapstr.h"
#include "pixmap.h"
#include "windowstr.h"
#include "gc.h"
#include "gcstruct.h"
#include "regionstr.h"
#include "colormap.h"
#include "miscstruct.h"
#include "dix.h"
#include "mfb.h"
#include "mi.h"
#include "mipointer.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <machine/param.h>
#include <machine/adbsys.h>
#include <machine/grfioctl.h>

#define TVTOMILLI(tv)	((tv).tv_usec / 1000 + (tv).tv_sec * 1000)


#if !defined(MAXSCREENS)
#define MAXSCREENS	8	/* God forbid anyone should have more... */
#endif


extern int mac_adbfd;


extern Time mac_lasteventtime;

extern DevicePtr macbsd_mouse;
extern DevicePtr macbsd_kbd;


typedef struct fbinfo_s {
	int modenum;
	caddr_t vaddr;
	struct grfmode idata;
	int added;
	int fd;
} fbinfo_t;

extern fbinfo_t mac_fbs[MAXSCREENS];
extern int mac_scrs;

extern miPointerScreenFuncRec mac_mousefuncs;
